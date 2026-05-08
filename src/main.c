#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#include "src/core.glsl.h"

#define __EAB_RAND_IMPL

#include "base.h"
#include "font.h"

typedef uint16_t draw_Idx;
typedef struct {
    f2 pos, uv;
    Color color;
    float size; 
} draw_Vtx;
typedef struct {
    sg_buffer vtx_buf;
    draw_Vtx *vtx, *vtx_wtr;
    size_t vtx_cap;

    sg_buffer idx_buf;
    draw_Idx *idx, *idx_wtr;
    size_t idx_cap;
} draw_Geo;

static void draw_geo_reset(draw_Geo *g) {
    g->vtx_wtr = g->vtx;
    g->idx_wtr = g->idx;
}

static size_t draw_geo_vtx_count(draw_Geo *g) {
    return g->vtx_wtr - g->vtx;
}

static size_t draw_geo_idx_count(draw_Geo *g) {
    return g->idx_wtr - g->idx;
}

static void draw_geo_ensure_can_hold_rects(draw_Geo *g, size_t rect_count) {
    size_t new_vtx_count = draw_geo_vtx_count(g) + 4*rect_count;
    size_t new_idx_count = draw_geo_idx_count(g) + 6*rect_count;
    if (new_vtx_count >= g->vtx_cap) {
        size_t vtx_count = draw_geo_vtx_count(g);
        g->vtx_cap = new_vtx_count + new_vtx_count/4;
        g->vtx = realloc(g->vtx, sizeof(draw_Vtx) * g->vtx_cap);
        g->vtx_wtr = g->vtx + vtx_count;

        if (sg_query_buffer_state(g->vtx_buf) == SG_RESOURCESTATE_VALID)
            sg_destroy_buffer(g->vtx_buf);

        g->vtx_buf = sg_make_buffer(&(sg_buffer_desc){
            .usage.immutable = false,
            .usage.stream_update = true,
            .size = sizeof(draw_Vtx) * g->vtx_cap,
        });
    }
    if (new_idx_count >= g->idx_cap) {
        size_t idx_count = draw_geo_idx_count(g);
        g->idx_cap = new_idx_count + new_idx_count/4;
        g->idx = realloc(g->idx, sizeof(draw_Idx) * g->idx_cap);
        g->idx_wtr = g->idx + idx_count;

        if (sg_query_buffer_state(g->idx_buf) == SG_RESOURCESTATE_VALID)
            sg_destroy_buffer(g->idx_buf);

        g->idx_buf = sg_make_buffer(&(sg_buffer_desc){
            .usage.index_buffer = true,
            .usage.immutable = false,
            .usage.stream_update = true,
            .size = sizeof(draw_Idx) * g->idx_cap,
        });
    }
}

static void draw_geo_init(draw_Geo *g, size_t rect_count) {
    draw_geo_ensure_can_hold_rects(g, rect_count);
}

static void draw_geo_upload(draw_Geo *g) {
    sg_update_buffer(g->vtx_buf, &(sg_range){
        .ptr = g->vtx,
        .size = draw_geo_vtx_count(g) * sizeof(draw_Vtx),
    });

    sg_update_buffer(g->idx_buf, &(sg_range){
        .ptr = g->idx,
        .size = draw_geo_idx_count(g) * sizeof(draw_Idx),
    });
}

// static void draw_geo_line(draw_Geo *g, f2 from, f2 to, float thickness, Color c) {
//     draw_geo_ensure_can_hold_rects(g, 1);
// }

static void draw_geo_str(draw_Geo *g, char *str, float size) {

    float pen_x = 0;
    for (char *c = str; *c; c++) {

        draw_geo_ensure_can_hold_rects(g, 1);

        font_LetterRegion *l = &font_letter_regions[(size_t)*c];
        float min_uv_x = (l->x            ) / (float)font_TEX_SIZE_X;
        float min_uv_y = (l->y            ) / (float)font_TEX_SIZE_Y;
        float max_uv_x = (l->x + l->size_x) / (float)font_TEX_SIZE_X;
        float max_uv_y = (l->y + l->size_y) / (float)font_TEX_SIZE_Y;

        float scale = size / font_BASE_CHAR_SIZE;
        float y = l->top * scale;
        float min_px_x = sapp_widthf() *0.5f + pen_x + 0;
        float min_px_y = sapp_heightf()*0.5f +     y + 0;
        float max_px_x = sapp_widthf() *0.5f + pen_x + l->size_x*scale;
        float max_px_y = sapp_heightf()*0.5f +     y + l->size_y*scale;

        float min_pos_x = 2.0f*(min_px_x / sapp_widthf() ) - 1.0f;
        float min_pos_y = 2.0f*(min_px_y / sapp_heightf()) - 1.0f;
        float max_pos_x = 2.0f*(max_px_x / sapp_widthf() ) - 1.0f;
        float max_pos_y = 2.0f*(max_px_y / sapp_heightf()) - 1.0f;

        Color b = { 0, 0, 0, 255 };
        draw_Idx i = draw_geo_vtx_count(g);
        *g->vtx_wtr++ = (draw_Vtx) { { min_pos_x, max_pos_y }, { min_uv_x, min_uv_y }, b, size };
        *g->vtx_wtr++ = (draw_Vtx) { { max_pos_x, min_pos_y }, { max_uv_x, max_uv_y }, b, size };
        *g->vtx_wtr++ = (draw_Vtx) { { min_pos_x, min_pos_y }, { min_uv_x, max_uv_y }, b, size };
        *g->vtx_wtr++ = (draw_Vtx) { { max_pos_x, max_pos_y }, { max_uv_x, min_uv_y }, b, size };

        *g->idx_wtr++ = i + 0;
        *g->idx_wtr++ = i + 1;
        *g->idx_wtr++ = i + 2;
        *g->idx_wtr++ = i + 1;
        *g->idx_wtr++ = i + 0;
        *g->idx_wtr++ = i + 3;

        pen_x += l->advance * scale;
    }
}

static struct {
    draw_Geo geo;

    sg_pipeline pip;
    sg_pass_action pass_action;

    sg_view font_tex;
    sg_sampler font_smp;
} draw;

static struct {
    uint64_t start;
} eng = {0};

static void init(void) {
    stm_setup();
    eng.start = stm_now();

    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    draw_geo_init(&draw.geo, 10);

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
        draw.font_tex = sg_make_view(&(sg_view_desc){ .texture = { .image = img } });
        draw.font_smp = sg_make_sampler(&(sg_sampler_desc){
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });
    }

    draw.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(core_shader_desc(sg_query_backend())),
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
                [ATTR_core_pos0  ].format = SG_VERTEXFORMAT_FLOAT2,
                [ATTR_core_uv0   ].format = SG_VERTEXFORMAT_FLOAT2,
                [ATTR_core_color0].format = SG_VERTEXFORMAT_UBYTE4N,
                [ATTR_core_size0 ].format = SG_VERTEXFORMAT_FLOAT,
            }
        },
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    draw.pass_action = (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = { 1.0f, 1.0f, 1.0f, 1.0f }
        }
    };
}

static void frame(void) {
    sg_begin_pass(&(sg_pass){
        .action = draw.pass_action,
        .swapchain = sglue_swapchain()
    });
    sg_apply_pipeline(draw.pip);

    {
        draw_geo_reset(&draw.geo);

        float size = font_BASE_CHAR_SIZE * 2.0f*(1.5f + sinf(
            stm_sec(stm_since(eng.start))
        ));
        draw_geo_str(&draw.geo, "quick brown", size);

        draw_geo_upload(&draw.geo);

        sg_apply_bindings(&(sg_bindings) {
            .vertex_buffers[0] = draw.geo.vtx_buf,
            .index_buffer = draw.geo.idx_buf,
            .views[VIEW_font_tex] = draw.font_tex,
            .samplers[SMP_font_smp] = draw.font_smp,
        });
        sg_draw(0, draw_geo_idx_count(&draw.geo), 1);
    }

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
        .window_title = "eab",
        .high_dpi = true,
        .logger.func = slog_func,
    };
}
