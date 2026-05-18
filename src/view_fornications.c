#include "view.h"
#include "save.h"
#include "guy.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

static struct {
    view_Transition next_view;
    struct { guy_Guy *mom, *dad, *kid; } families[countof(save.run.guys)];
} view;

void view_fornications_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    size_t male_count = 0;
    for (size_t i = 0; i < countof(t.fornications.in_orgy); i++) {
        guy_Guy *dad = t.fornications.in_orgy[i];
        if (dad == NULL || !(dad->sex & guy_Sex_Male)) continue;
        male_count += 1;
    }
    assert(male_count > 0);

    struct { guy_Guy kid, *mom, *dad; } new_guys[countof(save.run.guys)] = {0};
    size_t new_guy_i = 0;

    for (size_t i = 0; i < countof(t.fornications.in_orgy); i++) {
        guy_Guy *mom = t.fornications.in_orgy[i];
        if (mom == NULL || !mom->state || !(mom->sex & guy_Sex_Female)) continue;

        size_t dad_idx = rand_int(male_count);

        for (size_t j = 0; j < countof(t.fornications.in_orgy); j++) {
            guy_Guy *dad = t.fornications.in_orgy[j];
            if (dad == NULL || !dad->state || !(dad->sex & guy_Sex_Male)) continue;

            if (dad_idx == 0) {
                uint32_t childcount = guy_childcount(mom);

                for (uint32_t i = 0; i < childcount; i++) {
                    if ((new_guy_i+1) == countof(new_guys))
                        puts("no more room!");
                    else {
                        new_guys[new_guy_i].mom = mom;
                        new_guys[new_guy_i].dad = dad;
                        new_guys[new_guy_i].kid = guy_from_parents(mom, dad);
                        new_guy_i++;
                    }
                }

                break;
            }

            dad_idx -= 1;
        }
    }

    size_t kid_i = 0;
    for (size_t i = 0; i < countof(new_guys); i++) {
        if (new_guys[i].kid.state == guy_GuyState_NONE)
            continue;

        guy_Guy *kid = guy_alloc();
        if (kid == NULL) {
            printf("no space for kid!");
            break;
        }

        *kid = new_guys[i].kid;
        view.families[kid_i].mom = new_guys[i].mom;
        view.families[kid_i].dad = new_guys[i].dad;
        view.families[kid_i].kid = kid;
        kid_i++;
    }
}
void view_fornications_free(void) {}

view_Transition view_fornications_update(uint64_t _) {
    ui_update();
    return view.next_view;
}
void view_fornications_input(sapp_event *ev) {
    ui_input(ev);
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_fornications_render(void) {
    draw_frame_start((Color) { 255, 255, 255, 255 });
    ui_render(ui_create_layout(), draw_geo_default());
    draw_frame_end();
}

static void ui_tally(guy_Guy *mom, guy_Guy *dad, guy_Guy *kid) {
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW()
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(90),
                    .height = CLAY_SIZING_FIXED(90),
                },
            },
            .custom = { .customData = mom }
        });

        // CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } } });
        CLAY_TEXT(CLAY_STRING("+"), ui_font(ui_Font_Button));
        // CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } } });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(90),
                    .height = CLAY_SIZING_FIXED(90),
                },
            },
            .custom = { .customData = dad }
        });

        // CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } } });
        CLAY_TEXT(CLAY_STRING("="), ui_font(ui_Font_Button));
        // CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } } });

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(90),
                    .height = CLAY_SIZING_FIXED(90),
                },
            },
            .custom = { .customData = kid }
        });

        // CLAY_AUTO_ID({ .layout = { .sizing = { .width = CLAY_SIZING_FIXED(25) } } });
        CLAY_TEXT(CLAY_STRING("!"), ui_font(ui_Font_Button));
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
                CLAY_STRING("fornications!"),
                ui_font(ui_Font_Button),
            );
        }

        CLAY(CLAY_ID("tally_holder"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(0)
                },
                .childGap = 32,
            },
            .backgroundColor = {0},
            .clip = {
                .vertical = true,
                .childOffset = Clay_GetScrollOffset(),
            },
        }) {
            for (size_t i = 0; i < countof(view.families); i++) {
                if (view.families[i].mom == NULL ||
                    view.families[i].dad == NULL)
                    continue;
                ui_tally(view.families[i].mom,
                         view.families[i].dad,
                         view.families[i].kid);
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
            CLAY_STRING("BACK"),
            ui_icon(ui_Icon_Camp)
        )) {
            case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_PageTurn)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_BackToCampFromFornications;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(base_play_duration());
}
