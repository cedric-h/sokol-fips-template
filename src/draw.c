#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "base.h"
#include "draw.h"

#include "font.h"
#include "src/core.glsl.h"

static f2 draw_screen_to_gl(f2 pos) {
    return (f2) {
        .x = 2.0f*(pos.x / (sapp_widthf()  / sapp_dpi_scale())) - 1.0f,
        .y = 2.0f*(pos.y / (sapp_heightf() / sapp_dpi_scale())) - 1.0f,
    };
}

void draw_geo_reset(draw_Geo *g) {
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

void draw_geo_init(draw_Geo *g, size_t rect_count) {
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

void draw_geo_line(
    draw_Geo *g,
    f2 a, f2 b,
    float thickness,
    Color color
) {
    draw_geo_ensure_can_hold_rects(g, 1);

    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dlen = dx*dx + dy*dy;
    if (dlen <= 0) return;
    dlen = sqrtf(dlen);
    float nx = -dy / dlen * thickness*0.5;
    float ny =  dx / dlen * thickness*0.5;

    draw_Idx i = draw_geo_vtx_count(g);
    draw_Vtx_Bytes by = { .variant = draw_ShaderVariant_Solid };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2){ a.x - nx, a.y - ny }), {}, color, by };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2){ a.x + nx, a.y + ny }), {}, color, by };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2){ b.x - nx, b.y - ny }), {}, color, by };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2){ b.x + nx, b.y + ny }), {}, color, by };

    *g->idx_wtr++ = i + 0;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 2;
    *g->idx_wtr++ = i + 2;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 3;
}

void draw_geo_tex(
    draw_Geo *g,
    tex_Tex tex,
    draw_Rect r,
    Color color
) {
    draw_geo_ensure_can_hold_rects(g, 1);

    draw_Rect uv = tex_uv_rect(tex);

    draw_Vtx_Bytes b = {
        .variant = draw_ShaderVariant_Tex,
    };

    draw_Idx i = draw_geo_vtx_count(g);
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.min_x, r.min_y }), { uv.min_x, uv.min_y }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.max_x, r.max_y }), { uv.max_x, uv.max_y }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.min_x, r.max_y }), { uv.min_x, uv.max_y }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.max_x, r.min_y }), { uv.max_x, uv.min_y }, color, b };

    *g->idx_wtr++ = i + 0;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 2;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 0;
    *g->idx_wtr++ = i + 3;
}

void draw_geo_rect(
    draw_Geo *g,
    draw_Rect r,
    Color color
) {
    draw_geo_ensure_can_hold_rects(g, 1);

    draw_Vtx_Bytes b = {
        .variant = draw_ShaderVariant_Solid,
    };

    draw_Idx i = draw_geo_vtx_count(g);
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.min_x, r.min_y }), { 0, 0 }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.max_x, r.max_y }), { 0, 0 }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.min_x, r.max_y }), { 0, 0 }, color, b };
    *g->vtx_wtr++ = (draw_Vtx) { draw_screen_to_gl((f2) { r.max_x, r.min_y }), { 0, 0 }, color, b };

    *g->idx_wtr++ = i + 0;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 2;
    *g->idx_wtr++ = i + 1;
    *g->idx_wtr++ = i + 0;
    *g->idx_wtr++ = i + 3;
}

void draw_geo_str_ui(
    draw_Geo *g,
    f2 pos,
    char *str,
    size_t str_len,
    uint8_t size,
    Color color
) {

    float pen_x = pos.x;
    for (size_t i = 0; i < str_len; i++) {

        draw_geo_ensure_can_hold_rects(g, 1);

        /* this is a caps-only font, so atlas only has lowercase */
        size_t char_idx = str[i] | (1 << 5);
        font_LetterRegion *l = &font_letter_regions[char_idx];
        float scale = (float)size / (float)font_BASE_CHAR_SIZE;

        if (l->size_x == 0 || l->size_y == 0 || str[i] == ' ') {
            pen_x += l->advance * scale;
            continue;
        }

        float min_uv_x = (l->x            ) / (float)font_TEX_SIZE_X;
        float min_uv_y = (l->y            ) / (float)font_TEX_SIZE_Y;
        float max_uv_x = (l->x + l->size_x) / (float)font_TEX_SIZE_X;
        float max_uv_y = (l->y + l->size_y) / (float)font_TEX_SIZE_Y;

        float x = pen_x;
        float y = pos.y;
        float min_px_x = x + 0;
        float min_px_y = y + 0;
        float max_px_x = x + l->size_x*scale;
        float max_px_y = y + l->size_y*scale;

        f2 min_pos = draw_screen_to_gl((f2) { min_px_x, min_px_y });
        f2 max_pos = draw_screen_to_gl((f2) { max_px_x, max_px_y });

        draw_Idx i = draw_geo_vtx_count(g);
        draw_Vtx_Bytes b = {
            .variant = draw_ShaderVariant_Font,
            .byte1 = (uint8_t)size /* "byte1" is used as size in font shader */
        };
        *g->vtx_wtr++ = (draw_Vtx) { { min_pos.x, min_pos.y }, { min_uv_x, min_uv_y }, color, b };
        *g->vtx_wtr++ = (draw_Vtx) { { max_pos.x, max_pos.y }, { max_uv_x, max_uv_y }, color, b };
        *g->vtx_wtr++ = (draw_Vtx) { { min_pos.x, max_pos.y }, { min_uv_x, max_uv_y }, color, b };
        *g->vtx_wtr++ = (draw_Vtx) { { max_pos.x, min_pos.y }, { max_uv_x, min_uv_y }, color, b };

        *g->idx_wtr++ = i + 0;
        *g->idx_wtr++ = i + 1;
        *g->idx_wtr++ = i + 2;
        *g->idx_wtr++ = i + 1;
        *g->idx_wtr++ = i + 0;
        *g->idx_wtr++ = i + 3;

        pen_x += l->advance * scale;
    }
}

draw_TextSize draw_measure_str(
    char *str,
    size_t n_chars,
    uint8_t font_size
) {
    draw_TextSize size = {0};

    for (size_t i = 0; i < n_chars; i++) {
        /* this is a caps-only font, so atlas only has lowercase */
        size_t char_idx = str[i] | (1 << 5);
        font_LetterRegion *l = &font_letter_regions[char_idx];

        float scale = (float)font_size / (float)font_BASE_CHAR_SIZE;
        float x = (float)l->advance * scale;
        float y = (float)l->size_y * scale;
        size.width += x;
        size.height = max(size.height, y);
    }

    return size;
}

static struct {
    sg_pipeline pip;

    sg_bindings core_geo_bindings;
    draw_Geo geo_default;
} draw;

draw_Geo *draw_geo_default(void) {
    return &draw.geo_default;
}

void draw_init(void) {
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

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

        draw.core_geo_bindings.views[VIEW_font_tex] =
            sg_make_view(&(sg_view_desc){
                .texture = { .image = img }
            });
        draw.core_geo_bindings.samplers[SMP_font_smp] =
            sg_make_sampler(&(sg_sampler_desc){
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
            });
    }
    tex_system_init();

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
                [ATTR_core_bytes0].format = SG_VERTEXFORMAT_UBYTE4,
            }
        },
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
    });

    draw_geo_init(&draw.geo_default, 1 << 12);
}

void draw_free(void) {
    tex_system_free();
    sg_shutdown();
}

void draw_geo_draw(draw_Geo *g) {
    draw_geo_upload(g);

    {
        sg_bindings b = draw.core_geo_bindings;
        tex_system_bind(&b);
        b.vertex_buffers[0] = g->vtx_buf;
        b.index_buffer = g->idx_buf;
        sg_apply_bindings(&b);
    }
    sg_draw(0, draw_geo_idx_count(g), 1);
}

void draw_frame_start(Color bg) {
    sg_begin_pass(&(sg_pass){
        .action = (sg_pass_action) {
            .colors[0] = {
                .load_action = SG_LOADACTION_CLEAR,
                .clear_value = {
                    (float)bg.r / 255.0f,
                    (float)bg.g / 255.0f,
                    (float)bg.b / 255.0f,
                    (float)bg.a / 255.0f,
                }
            }
        },
        .swapchain = sglue_swapchain()
    });
    sg_apply_pipeline(draw.pip);

    draw_geo_reset(&draw.geo_default);
}

void draw_frame_end(void) {
    draw_geo_draw(&draw.geo_default);
    sg_end_pass();
    sg_commit();
}
