#include "sokol_gfx.h"
#include "src/core.glsl.h"

#include "base.h"
#include "tex.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

#define tex_WRITE_ATLAS false

#if tex_WRITE_ATLAS
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"
#endif

#define ATLAS_SIZE (1 << 13)

typedef enum {
    tex_SpritePhase_Free,
    tex_SpritePhase_NeedsBake,
    tex_SpritePhase_OnSheet,
} tex_SpritePhase;

#define tex_Sprite_PATH_LEN 200
typedef struct {
    tex_SpritePhase phase;
    char path[tex_Sprite_PATH_LEN];

    /* valid when OnSheet */
    draw_Rect uv_rect;

    /* valid when NeedsBake */
    struct { uint8_t *data; size_t size_x, size_y; } img;
} tex_Sprite;

static struct {
    bool spritesheet_clean;
    tex_Sprite sprites[1024];

    sg_view views[SG_MAX_VIEW_BINDSLOTS];
    sg_sampler samplers[SG_MAX_SAMPLER_BINDSLOTS];
} tex = {0};

void tex_system_init(void) {
}
void tex_system_bind(sg_bindings *b) {
    if (!tex.spritesheet_clean)
        tex_bake_spritesheet();

    for (size_t i = 0; i < countof(tex.samplers); i++) {
        if (tex.samplers[i].id == 0) continue;
        b->samplers[i] = tex.samplers[i];
    }

    for (size_t i = 0; i < countof(tex.views); i++) {
        if (tex.views[i].id == 0) continue;
        b->views[i] = tex.views[i];
    }
}

void tex_bake_spritesheet(void) {
    tex.spritesheet_clean = true;

    size_t needs_bake_count = 0;
    for (size_t i = 0; i < countof(tex.sprites); i++)
        if (tex.sprites[i].phase == tex_SpritePhase_NeedsBake ||
            tex.sprites[i].phase == tex_SpritePhase_OnSheet) {
            needs_bake_count += 1;
        }

    stbrp_rect *rects = malloc(needs_bake_count * sizeof(stbrp_rect));
    size_t rects_i = 0;

    size_t pad = 12;

    for (size_t i = 0; i < countof(tex.sprites); i++)
        if (tex.sprites[i].phase == tex_SpritePhase_NeedsBake ||
            tex.sprites[i].phase == tex_SpritePhase_OnSheet) {
            rects[rects_i].id = i;
            rects[rects_i].w = tex.sprites[i].img.size_x + pad*2;
            rects[rects_i].h = tex.sprites[i].img.size_y + pad*2;
            rects_i++;
        }

    stbrp_context ctx = {0};
    stbrp_node *nodes = malloc(ATLAS_SIZE * sizeof(stbrp_node));
    stbrp_init_target(&ctx, ATLAS_SIZE, ATLAS_SIZE, nodes, ATLAS_SIZE);

    stbrp_pack_rects(&ctx, rects, needs_bake_count);

    /* blit rects into atlas */
    uint8_t *atlas_data = malloc(ATLAS_SIZE*ATLAS_SIZE*4);
    for (size_t i = 0; i < needs_bake_count; i++) {
        stbrp_rect *rect = rects + i;
        tex_Sprite *src = tex.sprites + rect->id;

        if (!(src->phase == tex_SpritePhase_NeedsBake ||
              src->phase == tex_SpritePhase_OnSheet))
            continue;

        if (!rect->was_packed) {
            puts("ignoring, wasn't packed");
            continue;
        }

        src->phase = tex_SpritePhase_OnSheet;
        size_t rx = rect->x + pad;
        size_t ry = rect->y + pad;
        size_t rw = rect->w - pad*2;
        size_t rh = rect->h - pad*2;
        src->uv_rect.min_x = (float)(rx     ) / (float)ATLAS_SIZE;
        src->uv_rect.min_y = (float)(ry     ) / (float)ATLAS_SIZE;
        src->uv_rect.max_x = (float)(rx + rw) / (float)ATLAS_SIZE;
        src->uv_rect.max_y = (float)(ry + rh) / (float)ATLAS_SIZE;

        for (size_t y = 0; y < src->img.size_y; y++)
            for (size_t x = 0; x < src->img.size_x; x++) {
                size_t dst_x = rx + x;
                size_t dst_y = ry + y;
                size_t dst_i = (ATLAS_SIZE*dst_y + dst_x)*4;
                size_t src_i = (src->img.size_y*y + x)*4;

                uint8_t a = src->img.data[src_i + 3];
                /* store premultiplied */
                atlas_data[dst_i + 0] = (a * src->img.data[src_i + 0] + 127) / 255;
                atlas_data[dst_i + 1] = (a * src->img.data[src_i + 1] + 127) / 255;
                atlas_data[dst_i + 2] = (a * src->img.data[src_i + 2] + 127) / 255;
                atlas_data[dst_i + 3] = (a * src->img.data[src_i + 3] + 127) / 255;
            }
        // free(src->img.data); can't; we re-atlas a lot right now
    }

    free(rects);
    free(nodes);

    sg_image img = sg_make_image(&(sg_image_desc){
        .width = ATLAS_SIZE,
        .height = ATLAS_SIZE,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.mip_levels[0] = (sg_range) {
            .ptr = atlas_data,
            .size = ATLAS_SIZE * ATLAS_SIZE * 4,
        },
    });

#if tex_WRITE_ATLAS
    puts("writing atlas ...");
    stbi_write_png(
        EXE_PATH"/atlas.png",
        ATLAS_SIZE,
        ATLAS_SIZE,
        4,
        atlas_data,
        ATLAS_SIZE*4
    );
    puts("done!");
#endif

    free(atlas_data);

    tex.views[VIEW_tex_tex] =
        sg_make_view(&(sg_view_desc){
            .texture = { .image = img }
        });
    tex.samplers[SMP_tex_smp] =
        sg_make_sampler(&(sg_sampler_desc){
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });
}

void tex_system_free(void) {
}

tex_Tex tex_init(char *path) {
    if (!path[0]) return (tex_Tex) { 0 };
    for (size_t i = 1; i < countof(tex.sprites); i++) {
        if (!tex.sprites[i].phase) continue;
        if (strcmp(tex.sprites[i].path, path) == 0)
            return (tex_Tex) { i };
    }

    char full_path[200] = {0};
    snprintf(full_path, sizeof(full_path), EXE_PATH"/%s", path);

    int size_x, size_y, n;
    uint8_t *data = stbi_load(full_path, &size_x, &size_y, &n, 0);
    if (data == NULL || n != 4) {
        printf("[%s] stbi_load failed: %s\n", path, stbi_failure_reason());
        if (n != 4) {
            printf("[%s] expected 4 channels, found %d!\n", path, n);
        }

        static uint8_t px[4] = { 255, 0, 255, 255 };
        data = px;
        size_x = 1;
        size_y = 1;
        n = 4;
    }

    tex.spritesheet_clean = false;

    for (size_t i = 1; i < countof(tex.sprites); i++) {
        if (tex.sprites[i].phase) continue;
        tex.sprites[i] = (tex_Sprite) { 0 };
        tex.sprites[i].phase = tex_SpritePhase_NeedsBake;
        strlcpy(tex.sprites[i].path, path, tex_Sprite_PATH_LEN);
        tex.sprites[i].img.size_x = size_x;
        tex.sprites[i].img.size_y = size_y;
        tex.sprites[i].img.data = data;
        return (tex_Tex) { i };
    }
    puts("ran out of tex sprite space!");
    return (tex_Tex) { 0 };
}

draw_Rect tex_uv_rect(tex_Tex t) {
    if (t.id == 0) return (draw_Rect) {
        .min_x = 0.0f,
        .min_y = 0.0f,
        .max_x = 1.0f,
        .max_y = 1.0f,
    };

    if (tex.sprites[t.id].phase != tex_SpritePhase_OnSheet)
        return (draw_Rect) {
            .min_x = 0.0f,
            .min_y = 0.0f,
            .max_x = 1.0f,
            .max_y = 1.0f,
        };

    return tex.sprites[t.id].uv_rect;
}

size_t tex_size_x(tex_Tex t) { return tex.sprites[t.id].img.size_x; }
size_t tex_size_y(tex_Tex t) { return tex.sprites[t.id].img.size_y; }

void tex_free(tex_Tex _) {
    return;
}
