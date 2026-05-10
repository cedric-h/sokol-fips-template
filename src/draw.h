#ifndef __EAB_DRAW_IMPL
#define __EAB_DRAW_IMPL

typedef struct { float min_x, min_y, max_x, max_y; } draw_Rect;

#include "base.h"
#include "tex.h"

#ifndef SOKOL_GFX_INCLUDED
typedef struct { uint32_t id; } sg_buffer;
#endif

void draw_init(void);
void draw_free(void);
void draw_frame_start(Color bg);
void draw_frame_end(void);

typedef uint16_t draw_Idx;
typedef uint8_t draw_ShaderVariant;
enum {
    draw_ShaderVariant_Font = 0,
    draw_ShaderVariant_Solid = 1,
    draw_ShaderVariant_Tex = 2,
};
typedef struct {
    draw_ShaderVariant variant;
    uint8_t byte1, byte2, byte3;
} draw_Vtx_Bytes;
typedef struct {
    f2 pos, uv;
    Color color;
    /* shader variant and misc. parameters */
    draw_Vtx_Bytes bytes;
} draw_Vtx;
typedef struct {
    sg_buffer vtx_buf;
    draw_Vtx *vtx, *vtx_wtr;
    size_t vtx_cap;

    sg_buffer idx_buf;
    draw_Idx *idx, *idx_wtr;
    size_t idx_cap;
} draw_Geo;

draw_Geo *draw_geo_default(void);

void draw_geo_reset(draw_Geo *g);
void draw_geo_init(draw_Geo *g, size_t rect_count);
void draw_geo_draw(draw_Geo *g);
void draw_geo_line(
    draw_Geo *g,
    f2 a, f2 b,
    float thickness,
    Color color
);

typedef struct { size_t width, height; } draw_TextSize;
draw_TextSize draw_measure_str(
    char *str,
    size_t n_chars,
    uint8_t size
);

void draw_geo_tex(
    draw_Geo *geo,
    tex_Tex tex,
    draw_Rect rect,
    Color color
);

void draw_geo_rect(
    draw_Geo *geo,
    draw_Rect rect,
    Color color
);

void draw_geo_str(
    draw_Geo *g,
    f2 pos,
    char *str,
    uint8_t size,
    Color color
);

void draw_geo_str_ui(
    draw_Geo *g,
    f2 pos,
    char *str,
    size_t str_len,
    uint8_t size,
    Color color
);

#endif
