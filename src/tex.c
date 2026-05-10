#include "sokol_gfx.h"
#include "src/core.glsl.h"

#include "base.h"
#include "atlas.h"
#include "tex.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#define ATLAS_SIZE 1024

void tex_system_init(sg_bindings *b) {
    int x,y,n;
    uint8_t *data = stbi_load(resource_dir"/atlas.png", &x, &y, &n, 0);
    if (data == NULL || n != 4) {
        if (n != 4) {
            puts("unexpected number of channels!");
        }

        static uint8_t px[4] = { 255, 0, 255, 255 };
        data = px;
        x = 1;
        y = 1;
        n = 4;
    }

    sg_image img = sg_make_image(&(sg_image_desc){
        .width = x,
        .height = y,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.mip_levels[0] = (sg_range) {
            .ptr = data,
            .size = x * y * 4,
        },
    });

    b->views[VIEW_tex_tex] =
        sg_make_view(&(sg_view_desc){
            .texture = { .image = img }
        });
    b->samplers[SMP_tex_smp] =
        sg_make_sampler(&(sg_sampler_desc){
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });
}

void tex_system_free(void) {
}

tex_Tex tex_init(char *path) {

    if (!path[0]) return (tex_Tex) { 0 };
    if (strstr(path, "guy2")) return (tex_Tex) { 0 };

    for (size_t i = 0; i < countof(rtpDescAtlas); i++) {
        if (strstr(path, rtpDescAtlas[i].nameId)) {
            printf("found %s in %s\n", rtpDescAtlas[i].nameId, path);
            return (tex_Tex) { i };
        }
    }
    return (tex_Tex) { 1 };
}

draw_Rect tex_uv_rect(tex_Tex t) {
    if (t.id == 0) return (draw_Rect) {
        .min_x = 0.0f,
        .min_y = 0.0f,
        .max_x = 1.0f,
        .max_y = 1.0f,
    };

    float min_x = rtpDescAtlas[t.id].positionX;
    float min_y = rtpDescAtlas[t.id].positionY;
    float max_x = rtpDescAtlas[t.id].positionX + rtpDescAtlas[t.id].sourceWidth;
    float max_y = rtpDescAtlas[t.id].positionY + rtpDescAtlas[t.id].sourceHeight;
    return (draw_Rect) {
        .min_x = min_x / ATLAS_SIZE,
        .min_y = min_y / ATLAS_SIZE,
        .max_x = max_x / ATLAS_SIZE,
        .max_y = max_y / ATLAS_SIZE,
    };
}

void tex_free(tex_Tex _) {
    return;
}
