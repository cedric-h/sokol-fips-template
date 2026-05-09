#ifndef __UI_EAB_IMPL
#define __UI_EAB_IMPL

#include "clay.h"
#include "draw.h"

void ui_init(void);
void ui_free(void);
void ui_update(void);
void ui_render(Clay_RenderCommandArray render_cmds, draw_Geo *geo);

#endif
