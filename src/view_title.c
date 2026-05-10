#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;
} view;

void view_title_init(view_Transition _) {
    memset(&view, 0, sizeof(view));
}
void view_title_free(void) {}

void view_title_input(sapp_event *ev) {
    ui_input(ev);
}

view_Transition view_title_update(uint64_t _) {
    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_title_render(void) {
    draw_frame_start((Color) { 255, 255, 255, 255 });
    ui_render(ui_create_layout(), draw_geo_default());
    draw_frame_end();
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
            .padding = { 16, 16, 16, 16 },
            .childGap = 16,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
        },
        .backgroundColor = {0}
    }) {

        CLAY(CLAY_ID("title"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("eugenics auto battler"),
                ui_font_ex(
                    ui_Font_Title,
                    (Clay_TextElementConfig) {
                        .lineHeight = 80,
                        .textColor = { 0, 0, 0, 255 },
                    }
                ),
            );
        }

        CLAY(CLAY_ID("buttons"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_FIT(0),
                },
                .childGap = 16,
            },
        }) {

            switch (ui_big_button(
                CLAY_STRING("to battle!"),
                ui_icon(ui_Icon_Swords)
            )) {
                case ui_Click_Pressed:
                    sound_play(ui_sound(ui_Sound_CinematicOpening));
                    break;
                case ui_Click_Released: {
                    view.next_view.kind = view_TransitionKind_StartRun;
                } break;
                default: break;
            }

            switch (ui_big_button(
                CLAY_STRING("camp tech"),
                ui_icon(ui_Icon_Diamond)
            )) {
                case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
                case ui_Click_Released: {
                    view.next_view.kind = view_TransitionKind_CampTech;
                } break;
                default: break;
            }

            switch (ui_big_button(
                CLAY_STRING("options"),
                ui_icon(ui_Icon_Wrench)
            )) {
                case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
                case ui_Click_Released: {
                    view.next_view.kind = view_TransitionKind_Options;
                } break;
                default: break;
            }
        }
    }
    return Clay_EndLayout(sapp_frame_duration());
}
