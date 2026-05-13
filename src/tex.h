#ifndef __EAB_TEX_IMPL
#define __EAB_TEX_IMPL

#include <stddef.h>

typedef struct { size_t id; } tex_Tex;
tex_Tex tex_init(char *path);
void    tex_free(tex_Tex);
size_t tex_size_x(tex_Tex);
size_t tex_size_y(tex_Tex);
void tex_bake_spritesheet(void);

#ifdef SOKOL_GFX_INCLUDED
void tex_system_init(void);
void tex_system_bind(sg_bindings *b);
void tex_system_free(void);
#include "draw.h"
draw_Rect tex_uv_rect(tex_Tex t);
#endif

#endif
