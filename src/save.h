#ifndef __EAB_SAVE_IMPL
#define __EAB_SAVE_IMPL
#include "base.h"
#include "guy.h"
#include "ui.h"

typedef enum {
    save_Biome_Plains,
    save_Biome_Forest,
    save_Biome_DarkForest,
    save_Biome_Desert,
    save_Biome_COUNT,
} save_Biome;

static Color save_biome_color[] = {
    [save_Biome_Plains    ] = { 104, 148, 122, 255 },
    [save_Biome_Forest    ] = {  80, 109,  92, 255 },
    [save_Biome_DarkForest] = {  55,  67,  60, 255 },
    [save_Biome_Desert    ] = { 229, 196, 163, 255 },
};
_Static_assert(
    countof(save_biome_color) == save_Biome_COUNT,
    "missing biome color"
);

typedef enum {
    save_Furniture_NONE,
    // save_Furniture_Tikitorch,
    save_Furniture_PocketCamp,
    save_Furniture_Crown,
    save_Furniture_Telescope,
    // save_Furniture_CrusaderScrolls,
    save_Furniture_COUNT,
} save_Furniture;

typedef struct {
    ui_Icon icon;
    uint32_t cost;
    /* maximum amount of this type of furniture you can have */
    uint32_t max_count;
    String desc;
} save_FurnitureConfig;
extern save_FurnitureConfig save_furniture_configs[];

int save_count_furniture(save_Furniture);

typedef struct {
    uint32_t gems;
    
    /* state that gets reset at the end of a run */
    struct {
        uint32_t id;

        guy_Guy guys[100];

        uint32_t kills, battles_won;

        uint32_t key_count;
        float food;
        uint32_t coin;
        save_Furniture furniture[10];

        save_Biome biome;
    } run;

} save_Save;
extern save_Save save;
#endif
