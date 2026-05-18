#include "view.h"
#include "save.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;

    sound_Sound sound_furniture_buy, sound_reroll;

    save_Furniture options[3];
} view;

void roll_options(void) {
    memset(&view.options, 0, sizeof(view.options));

    save_Furniture available[save_Furniture_COUNT] = {0};
    size_t available_i = 0;

    for (save_Furniture f = 0; f < countof(available); f++) {
        save_FurnitureConfig *fc = save_furniture_configs + f;
        if (save_count_furniture(f) < (int)fc->max_count)
            available[available_i++] = f;
    }

    for (size_t i = 0; i < min(available_i, countof(view.options)); i++) {
        save_Furniture f = save_Furniture_NONE;

        /* keep rerolling until we don't have any dupes */
        while (f == save_Furniture_NONE) {
            f = available[rand_int(available_i)];

            for (size_t j = 0; j < i; j++)
                if (view.options[j] == f) {
                    f = save_Furniture_NONE;
                    break;
                }
        }

        view.options[i] = f;
    }
}

void view_buyfurniture_init(view_Transition _) {
    memset(&view, 0, sizeof(view));

    roll_options();

    view.sound_furniture_buy = sound_init("./resources/audio/furniture_buy.wav");
    view.sound_reroll = sound_init("./resources/audio/reroll.wav");
}
void view_buyfurniture_free(void) {
    sound_free(view.sound_furniture_buy);
    sound_free(view.sound_reroll);
}

void view_buyfurniture_input(sapp_event *ev) {
    ui_input(ev);
}

view_Transition view_buyfurniture_update(uint64_t _) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_buyfurniture_render(void) {
    draw_frame_start((Color) { 255, 255, 255, 255 });
    ui_render(ui_create_layout(), draw_geo_default());
    draw_frame_end();
}

ui_Click ui_furniture_button(ui_Icon icon, uint32_t cost, Clay_String text) {
    ui_Click ret = ui_Click_NONE;

    bool disabled = save.run.coin < cost;

    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(),
            },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
        },
    }) {

        /* cost */
        CLAY_AUTO_ID({
            .layout = {
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_FIXED(34),
                },
            },
        }) {

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(32),
                        .width = CLAY_SIZING_FIXED(32),
                    },
                },
                .image = { .imageData = ui_icon(ui_Icon_Fleur) }
            });

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_FIXED(10) } }
            });

            Clay_String tmp;
            ui_sprintf(tmp, "x%d", cost);
            CLAY_TEXT(tmp, ui_font(ui_Font_Cost));

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

        }

        Clay_Color enabled_black = 
            (disabled)
                ? (Clay_Color) { 120, 120, 120, 255}
                : (Clay_Color) {   0,   0,   0, 255}
            ;

        /* button */
        CLAY_AUTO_ID({
            .layout = {
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                .sizing = {
                    .height = CLAY_SIZING_FIT(),
                    .width = CLAY_SIZING_GROW(),
                },
                .padding = { 16, 16, 16, 16 },
            },
            .border = {
                .width = CLAY_BORDER_OUTSIDE(4),
                .color = enabled_black,
            },
            .backgroundColor = (Clay_Hovered() && !disabled)
                ? (Clay_Color) { 128, 128, 128, 128 }
                : (Clay_Color) { 255, 255, 255, 255 },
            .cornerRadius = CLAY_CORNER_RADIUS(6),
        }) {

            if (!disabled && Clay_Hovered()) {
                base_set_cursor(SAPP_MOUSECURSOR_POINTING_HAND);

                switch (Clay_GetPointerState().state) {
                    case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
                        ret = ui_Click_Pressed;
                        break;
                    case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
                        ret = ui_Click_Released;
                        break;
                    default:
                        break;
                }
            }

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(50),
                        .width = CLAY_SIZING_FIXED(50),
                    },
                },
                .image = {
                    .imageData = ui_icon(icon),
                },
                .backgroundColor = disabled
                    ? (Clay_Color) { 200, 200, 200, 200 }
                    : (Clay_Color) {   0,   0,   0,   0 },
            });

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

            CLAY_TEXT(text, ui_font_ex(ui_Font_Desc, (Clay_TextElementConfig) {
                .textAlignment = CLAY_TEXT_ALIGN_RIGHT,
                .textColor = enabled_black,
                .lineHeight = 24,
            }));
        }
    }

    return ret;
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        CLAY(CLAY_ID("title"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
                .sizing = { .width = CLAY_SIZING_GROW() },
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("furnish"),
                ui_font(ui_Font_SubTitle),
            );

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(32),
                        .width = CLAY_SIZING_FIXED(32),
                    },
                },
                .image = { .imageData = ui_icon(ui_Icon_Fleur) }
            });

            Clay_String tmp;
            ui_sprintf(tmp, "x%d", save.run.coin);
            CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
        }

        CLAY_AUTO_ID({
            .layout = { .sizing = { .height = CLAY_SIZING_FIXED(10) } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 38,
            },
        }) {
            for (int i = 0; i < 3; i++) {
                save_Furniture f = view.options[i];
                if (!f) continue;

                save_FurnitureConfig *fc = save_furniture_configs + f;
                switch (ui_furniture_button(
                    fc->icon,
                    fc->cost,
                    (Clay_String) {
                        .isStaticallyAllocated = true,
                        .chars = fc->desc.str,
                        .length = fc->desc.len,
                    }
                )) {
                    case ui_Click_Released: {
                        save.run.coin -= fc->cost;
                        for (size_t i = 0; i < countof(save.run.furniture); i++) {
                            if (save.run.furniture[i] == save_Furniture_NONE) {
                                save.run.furniture[i] = f;
                                break;
                            }
                        }
                        view.options[i] = save_Furniture_NONE;
                    } break;

                    case ui_Click_Pressed: {
                        sound_play(view.sound_furniture_buy);
                    } break;

                    default:
                        break;
                }
            }
        }

        CLAY_AUTO_ID({
            .layout = { .sizing = { .height = CLAY_SIZING_GROW() } }
        });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
        }) {
            CLAY_AUTO_ID({
                .layout = {
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                    .padding = { 6, 0, 0, 0 },
                }
            }) {
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .height = CLAY_SIZING_FIXED(32),
                            .width = CLAY_SIZING_FIXED(32),
                        },
                    },
                    .image = { .imageData = ui_icon(ui_Icon_Fleur) }
                });

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_FIXED(10) } }
                });

                CLAY_TEXT(CLAY_STRING("x5"), ui_font(ui_Font_Cost));
            }

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
            }) {

                switch (ui_small_button(
                        ui_icon(ui_Icon_Dice),
                        save.run.coin < 5
                    )) {
                    case ui_Click_Pressed: {
                        sound_play(view.sound_reroll);
                    } break;
                    case ui_Click_Released: {
                        save.run.coin -= 5;
                        roll_options();
                    } break;
                    default: break;
                }

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                });

                switch (ui_small_button(ui_icon(ui_Icon_BackToMap), false)) {
                    case ui_Click_Pressed: {
                        sound_play(ui_sound(ui_Sound_PageTurn));
                    } break;
                    case ui_Click_Released: {
                        view.next_view.kind = view_TransitionKind_BackToWorldMap;
                    } break;
                    default: break;
                }
            }
        }
    }
    return Clay_EndLayout(sapp_frame_duration());
}
