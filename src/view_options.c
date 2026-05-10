#include "sound.h"
#include "view.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>

static struct {
    view_Transition next_view;
} view;

void view_options_init(view_Transition _) {
    memset(&view, 0, sizeof(view));
}
void view_options_free(void) {}

view_Transition view_options_update(uint64_t _) {
    ui_update();
    return view.next_view;
}
static Clay_RenderCommandArray ui_create_layout(void);
void view_options_render(void) {
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
                CLAY_STRING("options"),
                ui_font(ui_Font_SubTitle),
            );
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
            ui_icon(ui_Icon_Back)
        )) {
            case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_Title;
            } break;
            default: break;
        }

    }
    return Clay_EndLayout(0);
}
