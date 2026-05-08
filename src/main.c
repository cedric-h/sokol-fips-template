#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "src/triangle.glsl.h"

#define __EAB_RAND_IMPL

#include "base.h"
#include "font.h"

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

    {
        font_LetterRegion *l = &font_letter_regions['c'];
        float min_uv_x = (l->x            ) / (float)font_TEX_SIZE_X;
        float min_uv_y = (l->y            ) / (float)font_TEX_SIZE_Y;
        float max_uv_x = (l->x + l->size_x) / (float)font_TEX_SIZE_X;
        float max_uv_y = (l->y + l->size_y) / (float)font_TEX_SIZE_Y;

        float size = font_BASE_CHAR_SIZE * 1.0f;
        float scale = size / font_BASE_CHAR_SIZE;
        float min_px_x = sapp_widthf() *0.5f + 0;
        float min_px_y = sapp_heightf()*0.5f + 0;
        float max_px_x = sapp_widthf() *0.5f + l->size_x*scale;
        float max_px_y = sapp_heightf()*0.5f + l->size_y*scale;

        float min_pos_x = 1.0f - 2.0f*(min_px_x / sapp_widthf() );
        float min_pos_y = 1.0f - 2.0f*(min_px_y / sapp_heightf());
        float max_pos_x = 1.0f - 2.0f*(max_px_x / sapp_widthf() );
        float max_pos_y = 1.0f - 2.0f*(max_px_y / sapp_heightf());

        Color b = { 0, 0, 0, 255 };
        struct { f2 pos, uv; Color color; float size; } vertices[] = {
            { { min_pos_x, max_pos_y }, { max_uv_x, max_uv_y }, b, size },
            { { max_pos_x, min_pos_y }, { min_uv_x, min_uv_y }, b, size },
            { { min_pos_x, min_pos_y }, { max_uv_x, min_uv_y }, b, size },
            { { max_pos_x, max_pos_y }, { min_uv_x, max_uv_y }, b, size },
        };

        // test
        // struct { f2 pos, uv; } vertices[] = {
        //     { { -0.5f,  0.5f }, {  0.0f,  1.0f } },
        //     { {  0.5f, -0.5f }, {  1.0f,  0.0f } },
        //     { { -0.5f, -0.5f }, {  0.0f,  0.0f } },
        //     { {  0.5f,  0.5f }, {  1.0f,  1.0f } },
        // };

        state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
            .data = SG_RANGE(vertices),
        });

        // create an index buffer for the cube
        uint16_t indices[] = {
            0, 1, 2,  1, 0, 3,
        };
        state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
            .usage.index_buffer = true,
            .data = SG_RANGE(indices),
        });
    }

    {
        /* create the font texture! */
        uint8_t pixels[font_TEX_SIZE_X * font_TEX_SIZE_Y] = {0};
        for (
           size_t letter_idx = 0;
           letter_idx < countof(font_letter_regions);
           letter_idx++
        ) {
            font_LetterRegion *lr = font_letter_regions + letter_idx;

            size_t i = lr->data_start;
            for (size_t pixel_y = 0; pixel_y < lr->size_y; pixel_y++)
                for (size_t pixel_x = 0; pixel_x < lr->size_x; pixel_x++) {
                    size_t x = lr->x + pixel_x;
                    size_t y = lr->y + pixel_y;
                    pixels[font_TEX_SIZE_X*y + x] = font_tex_bytes[i++];
                }
        }
        sg_image img = sg_make_image(&(sg_image_desc){
            .width = font_TEX_SIZE_X,
            .height = font_TEX_SIZE_Y,
            .pixel_format = SG_PIXELFORMAT_R8,
            .data.mip_levels[0] = SG_RANGE(pixels),
        });
        state.bind.views[VIEW_triangle_tex] =
            sg_make_view(&(sg_view_desc){ .texture = { .image = img } });

        // create a sampler object with default attributes
        state.bind.samplers[SMP_triangle_smp] =
            sg_make_sampler(&(sg_sampler_desc){
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
            });
    }

    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(triangle_shader_desc(sg_query_backend())),
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .colors[0].blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        },
        .layout = {
            .attrs = {
                [ATTR_triangle_pos0  ].format = SG_VERTEXFORMAT_FLOAT2,
                [ATTR_triangle_uv0   ].format = SG_VERTEXFORMAT_FLOAT2,
                [ATTR_triangle_color0].format = SG_VERTEXFORMAT_UBYTE4N,
                [ATTR_triangle_size0 ].format = SG_VERTEXFORMAT_FLOAT,
            }
        },
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    state.pass_action = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 1.0f, 1.0f, 1.0f, 1.0f }
        }
    };
}

static void frame(void) {
    sg_begin_pass(&(sg_pass){
        .action = state.pass_action,
        .swapchain = sglue_swapchain()
    });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 6, 1);
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
