#ifndef __EAB_UI_IMPL
#define __EAB_UI_IMPL

#include "clay.h"
#include "draw.h"
#include "guy.h"
#include "sound.h"
#include "tex.h"

typedef enum {
    ui_Font_Button,
    ui_Font_Title,
    ui_Font_SubTitle,
    ui_Font_Cost,
    ui_Font_Desc,
    ui_Font_Name,
    ui_Font_COUNT,
} ui_Font;

typedef enum {
    ui_Icon_Swords,
    ui_Icon_Diamond,
    ui_Icon_Wrench,
    ui_Icon_Back,
    ui_Icon_Forward,
    ui_Icon_Bed,
    ui_Icon_Camp,
    ui_Icon_Fire,
    ui_Icon_Plant,
    ui_Icon_Scroll,
    ui_Icon_Shovel,
    ui_Icon_Crown,
    ui_Icon_Fleur,
    ui_Icon_Food,
    ui_Icon_Dice,
    ui_Icon_Soup,
    ui_Icon_BackToMap,
    ui_Icon_Grave,
    ui_Icon_Heal,
    ui_Icon_Captured,
    ui_Icon_Chest,
    ui_Icon_Key,
    ui_Icon_Market,
    ui_Icon_Hole,
    ui_Icon_Telescope,
    ui_Icon_Girth,
    ui_Icon_Speed,
    ui_Icon_Strength,
    ui_Icon_Fecundity,
    ui_Icon_COUNT,
} ui_Icon;

typedef enum {
    ui_Sound_Click,
    ui_Sound_CinematicOpening,
    ui_Sound_BattleEnter,
    ui_Sound_BattleVictory,
    ui_Sound_BattleDefeat,
    ui_Sound_PageTurn,
    ui_Sound_CampEnter,
    ui_Sound_CampLeave,
    ui_Sound_FurnitureUnlock,
    ui_Sound_GotFood,
    ui_Sound_COUNT,
} ui_Sound;

size_t ui_font_size(ui_Font);
Clay_TextElementConfig ui_font(ui_Font);
Clay_TextElementConfig ui_font_ex(ui_Font, Clay_TextElementConfig);
tex_Tex ui_icon(ui_Icon);
sound_Sound ui_sound(ui_Sound);
void ui_init(void);
void ui_free(void);
void ui_update(void);
void ui_input(sapp_event *ev);
void ui_render(Clay_RenderCommandArray render_cmds, draw_Geo *geo);

typedef struct {
    ui_Icon icon;
    f2 start, end;
    double start_t, end_t;
    float size;
} ui_FlyingIcon;
/* returns true if there was room for the flying icon */
bool ui_flying_icon(ui_FlyingIcon icon);
void ui_flying_icon_end_t_from_speed(ui_FlyingIcon *icon, float speed);
/* offsets start/end randomly */
void ui_flying_icon_jitter(ui_FlyingIcon *icon, float radius);

Clay_String ui_layout_alloc(size_t size);
#define ui_sprintf(dst, fmt, ...)                                              \
    do {                                                                       \
        int _ui_len = snprintf(NULL, 0, (fmt), ##__VA_ARGS__);                 \
        (dst) = ui_layout_alloc(_ui_len + 1);                                  \
        snprintf((char *)(dst).chars, (dst).length + 1, (fmt), ##__VA_ARGS__); \
    } while (0)

typedef enum {
    ui_Click_NONE,
    ui_Click_Pressed,
    ui_Click_Released,
} ui_Click;

ui_Click ui_big_button(Clay_String text, tex_Tex icon);


typedef struct {
    tex_Tex icon;
    bool disabled;
    Color clr_normal, clr_hovered;
} ui_SmallButton_Config;
ui_Click ui_small_button(tex_Tex icon, bool disabled);
ui_Click ui_small_button_ex(ui_SmallButton_Config config);

void ui_guy_show_detail_page(guy_Guy *guy);
bool ui_takeover(void);

#endif
