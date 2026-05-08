#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "src/triangle.glsl.h"

static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    float vertices[] = {
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 1.0f, 1.0f
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices),
    });

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(triangle_shader_desc(sg_query_backend())),
        .layout = {
            .attrs = {
                [ATTR_triangle_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_triangle_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
    });

    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action=SG_LOADACTION_CLEAR, .clear_value={0.0f, 0.0f, 0.0f, 1.0f } }
    };
}

static void frame(void) {
    sg_begin_pass(&(sg_pass){ .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    sg_shutdown();
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
        .width = 640,
        .height = 480,
        .window_title = "Triangle",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
