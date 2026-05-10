#ifndef __EAB_TEX_IMPL
#define __EAB_TEX_IMPL

#include <stddef.h>

typedef struct { size_t id; } tex_Tex;
tex_Tex tex_init(char *path);
void    tex_free(tex_Tex);

#endif
