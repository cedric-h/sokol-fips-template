#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_time.h"

#include <stdio.h>

#define __EAB_RAND_IMPL

#include "base.h"
#include "draw.h"
#include "ui.h"

static struct {
    uint64_t start;
    draw_Geo geo;
} eng = {0};

static void init(void) {
    stm_setup();
    draw_init();
    ui_init();

    eng.start = stm_now();

    draw_geo_init(&eng.geo, 10);
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
                (Clay_TextElementConfig){
                    .textColor = { 0, 0, 0, 255 },
                    .fontSize = 70,
                },
            );
        }

    }

    return Clay_EndLayout(0);
}

static void frame(void) {
    draw_frame_start();

    {
        draw_geo_reset(&eng.geo);
        ui_update();
        ui_render(ui_create_layout(), &eng.geo);

        // float size = 48.0f * (1.5f + sinf(
        //     stm_sec(stm_since(eng.start))
        // ));
        // draw_geo_str(
        //     &eng.geo,
        //     (f2) {0},
        //     "quick brown",
        //     size,
        //     (Color) { 0, 0, 0, 255 }
        // );
        // draw_geo_line(
        //     &eng.geo,
        //     (f2) { 100 + -50, 100 + -50 },
        //     (f2) { 100 +  50, 100 +  50 },
        //     15.0f,
        //     (Color) { 155, 20, 20, 255 }
        // );
        // draw_geo_line(
        //     &eng.geo,
        //     (f2) { 100 +  50, 100 + -50 },
        //     (f2) { 100 + -50, 100 +  50 },
        //     15.0f,
        //     (Color) { 155, 20, 20, 255 }
        // );
        draw_geo_draw(&eng.geo);
    }

    draw_frame_end();
}

static void cleanup(void) {
    draw_free();
    ui_free();
}

static void input(const sapp_event* ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN: {
#ifdef __APPLE__
            if (ev->key_code == SAPP_KEYCODE_Q &&
                (ev->modifiers & SAPP_MODIFIER_SUPER)
            )
                sapp_request_quit();
#endif
        } break;

        default: break;
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = input,
        .width = 9*50,
        .height = 16*50,
        .window_title = "eab",
        .high_dpi = true,
        .logger.func = slog_func,
    };
}
