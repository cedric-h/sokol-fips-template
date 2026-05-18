#include "save.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;

    guy_Guy *captured[countof(save.run.guys)];
    float food, coin;
} view;

void view_battlevictory_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
    view.food = t.battle_victory.food;
    view.coin = t.battle_victory.coin;
    memcpy(view.captured, t.battle_victory.captured, sizeof(view.captured));
}
void view_battlevictory_free(void) {}

view_Transition view_battlevictory_update(uint64_t _) {
    ui_update();
    return view.next_view;
}

void view_battlevictory_input(sapp_event *ev) {
    ui_input(ev);
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battlevictory_render(void) {
    draw_frame_start((Color) { 255, 255, 255, 255 });
    ui_render(ui_create_layout(), draw_geo_default());
    draw_frame_end();
}

static void ui_tally(ui_Icon icon, float added, float total) {
    CLAY_AUTO_ID({
        .layout = {
            .childAlignment = {
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(45),
                    .height = CLAY_SIZING_FIXED(45),
                },
            },
            .image = { .imageData = ui_icon(icon) }
        });

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_FIXED(15) } }
        });

        Clay_String tmp;
        ui_sprintf(tmp, "%.1f (+ %.1f)", total, added);
        CLAY_TEXT(tmp, ui_font(ui_Font_Button));
    }
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
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("victory"),
                ui_font(ui_Font_SubTitle),
            );
        }

        CLAY_AUTO_ID({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .padding = { 32, 32, 32, 32 },
                .childGap = 32,
            },
        }) {
            ui_tally(ui_Icon_Food, view.food, save.run.food);
            ui_tally(ui_Icon_Fleur, view.coin, save.run.coin);
        }

        CLAY_AUTO_ID({
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        }) {
            CLAY_TEXT(
                CLAY_STRING("captured"),
                ui_font(ui_Font_Button),
            );

            for (size_t i = 0; i < countof(view.captured); i++) {
                if (view.captured[i] == NULL) break;

                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(90),
                            .height = CLAY_SIZING_FIXED(90),
                        },
                    },
                    .custom = { .customData = view.captured[i] }
                }) {
                    if (Clay_Hovered() &&
                        (Clay_GetPointerState().state == 
                            CLAY_POINTER_DATA_RELEASED_THIS_FRAME)) {
                        ui_guy_show_detail_page(view.captured[i]);
                    }
                }
            }
        }


        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(20),
                    .height = CLAY_SIZING_GROW(),
                },
            },
        }) {
        }

        switch (ui_big_button(
            CLAY_STRING("DONE"),
            ui_icon(ui_Icon_Fleur)
        )) {
            case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_PageTurn)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_BackToWorldMap;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(sapp_frame_duration());
}
