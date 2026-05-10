#include "save.h"

save_Save save = {};

int save_count_furniture(save_Furniture furniture) {
    int ret = 0;
    for (size_t i = 0; i < countof(save.run.furniture); i++) {
        if (save.run.furniture[i] == save_Furniture_NONE)
            continue;

        ret += save.run.furniture[i] == furniture;
    }

    return ret;
}

save_FurnitureConfig save_furniture_configs[] = {
    // [save_Furniture_Tikitorch] = {
    //     .icon = ui_Icon_Fire,
    //     .cost = 30,
    //     .max_count = 1,
    //     .desc = STRING(
    //         "Inspire guys to\n"
    //         "explode in battle"
    //     )
    // },
    [save_Furniture_PocketCamp] = {
        .icon = ui_Icon_Bed,
        .cost = 20,
        .max_count = 1,
        .desc = STRING(
            "Camp on any tile"
        )
    },
    // [save_Furniture_CrusaderScrolls] = {
    //     .icon = ui_Icon_Scroll,
    //     .cost = 25,
    //     .max_count = 999,
    //     .desc = STRING(
    //         "if all share ancestor:\n"
    //         "+30% damage\n"
    //         "otherwise, -30% damage"
    //     )
    // },
    [save_Furniture_Crown] = {
        .icon = ui_Icon_Crown,
        .cost = 35,
        .max_count = 1,
        .desc = STRING(
            "Crown a guy to\n"
            "x2 all their stats\n"
        )
    },
    [save_Furniture_Telescope] = {
        .icon = ui_Icon_Telescope,
        .cost = 10,
        .max_count = 1,
        .desc = STRING(
            "Use on map to see\n"
            "what lies ahead"
        )
    },
    /* Guys surrender sooner when you are bigger */
    /* Captured guys are fully healed */
    /* All guys heal 10% at end of battle */
    /* uno reverse card - once, swap your army and the opposing army */
};
_Static_assert(
    countof(save_furniture_configs) == save_Furniture_COUNT,
    "missing furniture config"
);
