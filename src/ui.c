#include <stdio.h>

#include "base.h"
#include "view.h"
#include "draw.h"
#include "ease.h"
#include "clay.h"

#include "ui.h"

static void ui_clay_init(void);
static void ui_clay_free(void);

size_t ui_font_sizes[] = {
    [ui_Font_Button  ] = 63,
    [ui_Font_Title   ] = 100,
    [ui_Font_SubTitle] = 86,
    [ui_Font_Cost    ] = 45,
    [ui_Font_Desc    ] = 32,
    [ui_Font_Name    ] = 18,
};
_Static_assert(countof(ui_font_sizes) == ui_Font_COUNT, "missing font path");

char *ui_icon_paths[] = {
    [ui_Icon_Swords   ] = "resources/icon/swords.png",
    [ui_Icon_Diamond  ] = "resources/icon/diamond.png",
    [ui_Icon_Wrench   ] = "resources/icon/wrench.png",
    [ui_Icon_Back     ] = "resources/icon/back.png",
    [ui_Icon_Forward  ] = "resources/icon/forward.png",
    [ui_Icon_Bed      ] = "resources/icon/bed.png",
    [ui_Icon_Camp     ] = "resources/icon/camp.png",
    [ui_Icon_Fire     ] = "resources/icon/fire.png",
    [ui_Icon_Plant    ] = "resources/icon/plant.png",
    [ui_Icon_Scroll   ] = "resources/icon/scroll.png",
    [ui_Icon_Shovel   ] = "resources/icon/shovel.png",
    [ui_Icon_Crown    ] = "resources/icon/crown.png",
    [ui_Icon_Fleur    ] = "resources/icon/fleur.png",
    [ui_Icon_Food     ] = "resources/icon/food.png",
    [ui_Icon_Dice     ] = "resources/icon/dice.png",
    [ui_Icon_Soup     ] = "resources/icon/soup.png",
    [ui_Icon_BackToMap] = "resources/icon/back_to_map.png",
    [ui_Icon_Grave    ] = "resources/icon/grave.png",
    [ui_Icon_Heal     ] = "resources/icon/heal.png",
	[ui_Icon_Captured ] = "resources/icon/captured.png",
	[ui_Icon_Chest    ] = "resources/icon/chest.png",
	[ui_Icon_Key      ] = "resources/icon/key.png",
	[ui_Icon_Market   ] = "resources/icon/market.png",
	[ui_Icon_Hole     ] = "resources/icon/hole.png",
	[ui_Icon_Telescope] = "resources/icon/telescope.png",
	[ui_Icon_Girth    ] = "resources/icon/girth.png",
	[ui_Icon_Speed    ] = "resources/icon/speed.png",
	[ui_Icon_Strength ] = "resources/icon/strength.png",
	[ui_Icon_Fecundity] = "resources/icon/fecundity.png",
};
_Static_assert(countof(ui_icon_paths) == ui_Icon_COUNT, "missing icon path");

char *ui_sound_paths[] = {
    [ui_Sound_Click           ] = "resources/audio/click1.wav",
    [ui_Sound_CinematicOpening] = "resources/audio/cinematic_opening.wav",
    [ui_Sound_BattleEnter     ] = "resources/audio/battle_enter.wav",
    [ui_Sound_BattleVictory   ] = "resources/audio/battle_victory.wav",
    [ui_Sound_BattleDefeat    ] = "resources/audio/battle_defeat.wav",
    [ui_Sound_PageTurn        ] = "resources/audio/page_turn.wav",
    [ui_Sound_CampEnter       ] = "resources/audio/camp_enter.wav",
    [ui_Sound_CampLeave       ] = "resources/audio/camp_leave.wav",
    [ui_Sound_FurnitureUnlock ] = "resources/audio/furniture_unlock.wav",
    [ui_Sound_GotFood         ] = "resources/audio/got_food.wav",
};
_Static_assert(countof(ui_sound_paths) == ui_Sound_COUNT, "missing sound path");

#define ui_FLYING_ICON_MAX 200
static struct {
    Clay_Arena clay_memory;

    char *layout_arena_backing, *layout_arena;
    size_t layout_arena_size;

    tex_Tex icons [ ui_Icon_COUNT];
    sound_Sound sounds[ui_Sound_COUNT];

    ui_FlyingIcon flying_icon[ui_FLYING_ICON_MAX];

    struct {
        bool active, show_stats;
        enum {
            ui_DetailTab_Overview,
            ui_DetailTab_Race,
            ui_DetailTab_Genes,
            ui_DetailTab_Relatives,
        } tab;
        guy_GeneConfig *gene_hovered;
        guy_Guy *guy;
    } guy_detail;

    struct {
        f2 scroll, mouse_pos;
        bool mouse_down;
    } input;
} ui = {0};

size_t ui_font_size(ui_Font f) { return ui_font_sizes[f]; }

Clay_TextElementConfig ui_font(ui_Font f) {
    return ui_font_ex(f, (Clay_TextElementConfig){
        .textColor = { 0, 0, 0, 255 },
    });
}
Clay_TextElementConfig ui_font_ex(ui_Font f, Clay_TextElementConfig tec) {
    tec.fontId = f;
    if (tec.fontSize == 0)
        tec.fontSize = ui_font_sizes[f];
    return tec;
}
tex_Tex ui_icon(ui_Icon i) { return ui.icons[i]; }
sound_Sound ui_sound(ui_Sound s) { return ui.sounds[s]; }

/* allocate strings that have a lifetime of a single layout */
Clay_String ui_layout_alloc(size_t alloc_size) {
    size_t new_size = (ui.layout_arena - ui.layout_arena_backing) + alloc_size;
    if (new_size >= ui.layout_arena_size) {
        printf("ran out of space in ui layout arena, resetting it ...\n");
        ui.layout_arena = ui.layout_arena_backing;
    }

    char *base = ui.layout_arena;
    ui.layout_arena += alloc_size;
    return (Clay_String) { .chars = base, .length = alloc_size - 1 };
}

void ui_handle_clay_errors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);

    switch (errorData.errorType) {
        case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED: {
            Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
            ui_clay_free();
            ui_clay_init();
        } break;

        case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED: {
            Clay_SetMaxMeasureTextCacheWordCount(
                Clay_GetMaxMeasureTextCacheWordCount() * 2
            );
            ui_clay_free();
            ui_clay_init();
        } break;

        default:
            break;
    }
}

static void ui_clay_init(void) {
    ui.layout_arena_backing = malloc(1 << 12);
    ui.layout_arena_size = 1 << 12;
    ui.layout_arena = ui.layout_arena_backing;

    uint64_t total_mem_size = Clay_MinMemorySize();
    ui.clay_memory = Clay_CreateArenaWithCapacityAndMemory(
        total_mem_size,
        malloc(total_mem_size)
    );
    Clay_Initialize(
        ui.clay_memory,
        (Clay_Dimensions) {
            (float)base_screen_size_x(),
            (float)base_screen_size_y()
        },
        (Clay_ErrorHandler) { ui_handle_clay_errors, 0 }
    );
}

static void ui_clay_free(void) {
    free(ui.layout_arena_backing);
    free(ui.clay_memory.memory);
}

static Clay_Dimensions ui_measure_text(
    Clay_StringSlice text,
    Clay_TextElementConfig *config,
    void *_
);

void ui_init(void) {
    ui_clay_init();

    for (int i = 0; i < ui_Icon_COUNT; i++) {
        ui.icons[i] = tex_init(ui_icon_paths[i]);
    }
    // tex_bake_spritesheet();

    for (int i = 0; i < ui_Sound_COUNT; i++)
        ui.sounds[i] = sound_init(ui_sound_paths[i]);

    Clay_SetMeasureTextFunction(ui_measure_text, NULL);
}
void ui_free(void) {
    ui_clay_free();

    for (int i = 0; i < ui_Icon_COUNT; i++)
        tex_free(ui.icons[i]);

    for (int i = 0; i < ui_Sound_COUNT; i++)
        sound_free(ui.sounds[i]);
}

void ui_update(void) {

    // Clay_SetDebugModeEnabled(false);

    Clay_SetLayoutDimensions(
        (Clay_Dimensions) {
            (float)base_screen_size_x(),
            (float)base_screen_size_y()
        }
    );

    Clay_SetPointerState(
        (Clay_Vector2) {
            ui.input.mouse_pos.x,
            ui.input.mouse_pos.y,
        },
        ui.input.mouse_down
    );
    Clay_UpdateScrollContainers(
        true,
        (Clay_Vector2) {
            ui.input.scroll.x,
            ui.input.scroll.y,
        },
        sapp_frame_duration()
    );
    ui.input.scroll = (f2){ 0, 0 };

}

/* offsets start/end randomly */
void ui_flying_icon_jitter(ui_FlyingIcon *fi, float radius) {
    fi->start.x += gaussian_randf(0, radius);
    fi->start.y += gaussian_randf(0, radius);
    fi->end.x += gaussian_randf(0, radius);
    fi->end.y += gaussian_randf(0, radius);
}

/* calculates the duration of the animation as a function of the
 * distance traveled, by setting end_t to start_t plus the product
 * of speed multiplied by the distance between start and end */
void ui_flying_icon_end_t_from_speed(ui_FlyingIcon *fi, float speed) {
    fi->end_t = fi->start_t + speed*sqrtf(
        (fi->start.x - fi->end.x)*
            (fi->start.x - fi->end.x) +
        (fi->start.y - fi->end.y)*
            (fi->start.y - fi->end.y)
    );
}

bool ui_flying_icon(ui_FlyingIcon new_icon) {
    for (int i = 0; i < ui_FLYING_ICON_MAX; i++) {
        ui_FlyingIcon *icon = ui.flying_icon + i;
        if (icon->end_t > base_play_duration()) continue;

        *icon = new_icon;
        return true;
    }
    return false;
}

static void ui_render_cmds(Clay_RenderCommandArray render_cmds, draw_Geo *g);
void ui_render(Clay_RenderCommandArray render_cmds, draw_Geo *g) {
    ui_render_cmds(render_cmds, g);

    ui.layout_arena = ui.layout_arena_backing;

    for (int i = 0; i < ui_FLYING_ICON_MAX; i++) {
        ui_FlyingIcon *item = ui.flying_icon + i;
        double t = inv_lerp(
            item->start_t,
            item->end_t,
            base_play_duration()
        );

        if (t < 0 || t > 1) continue;

        float a = 1.0f - (fabs(0.5 - t)/0.5f);
        a = min(1, a * 5);

        t = ease_out_sine(t);
        float x = lerpf(item->start.x, item->end.x, t);
        float y = lerpf(item->start.y, item->end.y, t);
        draw_geo_tex(
            g,
            ui_icon(item->icon),
            (draw_Rect) {
                .min_x = x - item->size,
                .min_y = y - item->size,
                .max_x = x + item->size,
                .max_y = y + item->size,
            },
            (Color) { 255, 255, 255, 255*a }
        );
    }
}

ui_Click ui_big_button(Clay_String text, tex_Tex icon) {
    ui_Click ret = ui_Click_NONE;

    CLAY(CLAY_SIDI(text, icon.id), {
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = {0, 0, 0, 255}
        },
        .backgroundColor = Clay_Hovered()
            ? (Clay_Color) { 128, 128, 128, 128 }
            : (Clay_Color) { 255, 255, 255, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .padding = { 32, 32, 16, 16 },
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIT(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {
        if (Clay_Hovered()) {
            base_set_cursor(SAPP_MOUSECURSOR_POINTING_HAND);

            switch (Clay_GetPointerState().state) {
                case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
                    ret = ui_Click_Pressed;
                    break;
                case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
                    ret = ui_Click_Released;
                    break;
                default:
                    break;
            }
        }

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(60),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
            .image = { .imageData = icon }
        });

        /* spacer */
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
        });

        CLAY_TEXT(text, ui_font(ui_Font_Button));
    }

    return ret;
}

static Clay_Color ui_clay_color_from_color(Color color) {
    return (Clay_Color) { color.r, color.g, color.b, color.a };
}

ui_Click ui_small_button(tex_Tex icon, bool disabled) {
    return ui_small_button_ex((ui_SmallButton_Config) {
        .icon = icon,
        .disabled = disabled,
        .clr_normal = (Color) { 255, 255, 255, 255 },
        .clr_hovered = (Color) { 128, 128, 128, 128 },
    });
}
ui_Click ui_small_button_ex(ui_SmallButton_Config cfg) {
    tex_Tex icon = cfg.icon;
    bool disabled = cfg.disabled;
    ui_Click ret = ui_Click_NONE;

    Clay_Color enabled_black = 
        (disabled)
            ? (Clay_Color) { 120, 120, 120, 120 }
            : (Clay_Color) {   0,   0,   0, 255 }
        ;

    CLAY(CLAY_IDI("ui_small_button", icon.id), {
        .border = {
            .width = CLAY_BORDER_OUTSIDE(4),
            .color = enabled_black,
        },
        .backgroundColor = (!disabled && Clay_Hovered())
            ? ui_clay_color_from_color(cfg.clr_hovered)
            : ui_clay_color_from_color(cfg.clr_normal),
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .layout = {
            .padding = { 16, 16, 16, 16 },
            .sizing = {
                .width = CLAY_SIZING_FIT(0),
                .height = CLAY_SIZING_FIT(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        }
    }) {
        if (!disabled && Clay_Hovered()) {
            base_set_cursor(SAPP_MOUSECURSOR_POINTING_HAND);

            switch (Clay_GetPointerState().state) {
                case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
                    ret = ui_Click_Pressed;
                    break;
                case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
                    ret = ui_Click_Released;
                    break;
                default:
                    break;
            }
        }

        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(60),
                    .height = CLAY_SIZING_FIXED(60),
                },
            },
            .image = {
                .imageData = icon,
                .tint = (disabled)
                    ? (Clay_Color) { 255, 255, 255, 200 }
                    : (Clay_Color) { 255, 255, 255, 255 },
            },
        });
    }

    return ret;
}

static void ui_gene_tally(
    guy_Guy *guy,
    char *gene_desc,
    float sum,
    guy_GeneCategory category
) {
    Clay_String tmp;

    ui_sprintf(tmp, "%s: %.2f", gene_desc, sum);
    CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

    CLAY_AUTO_ID({
        .layout.childGap = 6,
    }) {
        ui_sprintf(tmp, "%s GENES: ", gene_desc);
        CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

        bool first = true;
        for (guy_GeneLoc i = 0; i < guy_GeneLoc_COUNT; i++) {
            if (guy_gene_loc_categories[i] != category)
                continue;

            if (!first)
                CLAY_TEXT(CLAY_STRING(","), ui_font(ui_Font_Desc));
            first = false;

            CLAY_AUTO_ID({}) {
                ui_sprintf(tmp, "%.2f", guy->genes[i]->amount);
                CLAY_TEXT(tmp, ui_font(ui_Font_Desc));

                if (Clay_Hovered())
                    ui.guy_detail.gene_hovered = guy->genes[i];
            }
        }
    }

}

/* returns true if hovered */
static bool ui_swatch(Color color) {
    bool hover = false;

    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(20),
                .height = CLAY_SIZING_FIXED(20),
            },
        },
        .backgroundColor = { color.r, color.g, color.b, color.a },
        .cornerRadius = CLAY_CORNER_RADIUS(4),
    }) {
        hover = Clay_Hovered();
    }

    return hover;
}

static void ui_gene_tally_color(
    guy_Guy *guy,
    char *gene_desc,
    Color sum,
    guy_GeneCategory category
) {
    CLAY_AUTO_ID({
        .layout.childGap = 6,
    }) {
        Clay_String tmp;

        ui_sprintf(tmp, "%s: ", gene_desc);
        CLAY_TEXT(tmp, ui_font(ui_Font_Desc));
        ui_swatch(sum);

        CLAY_TEXT(CLAY_STRING("("), ui_font(ui_Font_Desc));
        bool first = true;
        for (guy_GeneLoc i = 0; i < guy_GeneLoc_COUNT; i++) {
            if (guy_gene_loc_categories[i] != category)
                continue;

            if (!first)
                CLAY_TEXT(CLAY_STRING(","), ui_font(ui_Font_Desc));
            first = false;

            if (ui_swatch(guy->genes[i]->color))
                ui.guy_detail.gene_hovered = guy->genes[i];
        }
        CLAY_TEXT(CLAY_STRING(")"), ui_font(ui_Font_Desc));
    }
}

static void ui_race_summary(guy_Guy *guy) {
    uint32_t gene_count_by_race[guy_Race_COUNT] = {0};

    for (guy_GeneLoc i = 0; i < guy_GeneLoc_COUNT; i++) {
        gene_count_by_race[guy->genes[i]->race] += 1;
    }

    typedef struct {
        guy_Race race;
        uint32_t count;
    } Entry;
    Entry entries[guy_Race_COUNT] = {0};

    for (guy_Race race = 1; race < guy_Race_COUNT; race++) {
        Entry e = { race, gene_count_by_race[race] };

        size_t i = race;
        while (i > 0 && entries[i - 1].count < e.count)
            entries[i] = entries[i - 1], i--;
        entries[i] = e;
    }

    CLAY_AUTO_ID({
        .layout.layoutDirection = CLAY_TOP_TO_BOTTOM,
        .layout.childGap = 8,
    }) {
        CLAY_TEXT(CLAY_STRING("RACE:"), ui_font(ui_Font_Cost));

        for (size_t i = 0; i < countof(entries); i++) {
            uint32_t count = entries[i].count;
            if (count == 0) continue;
            guy_Race race = entries[i].race;
            char *race_name = guy_race_names[race];

            float f = 100.0f * (
                (float)count / (float)(guy_GeneLoc_COUNT)
            );

            Clay_String tmp;
            ui_sprintf(tmp, "%.1f%% %s", f, race_name);
            CLAY_TEXT(tmp, ui_font(ui_Font_Desc));
        }
    }
}

static void ui_closest_relatives(guy_Guy *guy) {

    struct {
        guy_Guy *guy;
        uint32_t shared_genes;
    } closest[3] = {0};

    for (size_t i = 0; i < countof(save.run.guys); i++) {
        guy_Guy *rel = save.run.guys + i;
        if (rel == guy) continue;
        if (rel->state == guy_GuyState_NONE) continue;

        uint32_t related_count = 0;

        for (guy_GeneLoc i = 0; i < guy_GeneLoc_COUNT; i++)
            related_count += rel->genes[i] == guy->genes[i];

        size_t i = countof(closest) - 1;
        while (i > 0 && closest[i - 1].shared_genes < related_count)
            closest[i] = closest[i - 1], i--;
        closest[i].guy = rel;
        closest[i].shared_genes = related_count;
    }

    CLAY_AUTO_ID({
        .layout.layoutDirection = CLAY_TOP_TO_BOTTOM,
        .layout.sizing.width = CLAY_SIZING_GROW(),
        .layout.childAlignment.x = CLAY_ALIGN_X_CENTER,
    }) {
        for (size_t i = 0; i < countof(closest); i++) {
            if (closest[i].guy == NULL)
                continue;

            CLAY_AUTO_ID({
                .layout.childAlignment.y = CLAY_ALIGN_Y_CENTER,
                .layout.childGap = 16,
            }) {
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(90),
                            .height = CLAY_SIZING_FIXED(90),
                        },
                        .padding = { 16, 16, 16, 16 },
                    },
                    .custom = { .customData = closest[i].guy }
                });

                uint32_t count = closest[i].shared_genes;
                float p = 100 * (
                    (float)count / (float)guy_GeneLoc_COUNT
                );
                Clay_String tmp;
                ui_sprintf(tmp, "%.1f%% DNA", p);
                CLAY_TEXT(tmp, ui_font(ui_Font_Cost));

            }
        }
    }
}

Clay_RenderCommandArray ui_guy_detail(void) {
    guy_Guy *guy = ui.guy_detail.guy;

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = { 255, 255, 255, 255 }
    }) {

        CLAY(CLAY_ID("content"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 16,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(0)
                },
            },
        }) {
            char name[GUY_NAME_LEN_MAX] = {0};
            guy_name(guy, name);

            Clay_String tmp;
            ui_sprintf(tmp, "%s", name);
            CLAY_AUTO_ID({
                .clip.horizontal = true,
            }) {
                CLAY_TEXT(tmp, ui_font(ui_Font_Button));
            }

            CLAY_AUTO_ID({
                .layout.sizing.width = CLAY_SIZING_GROW(),
            }) {
                CLAY_AUTO_ID({ .layout.sizing.width = CLAY_SIZING_GROW() });

                CLAY_AUTO_ID({
                    .border = {
                        .width = CLAY_BORDER_OUTSIDE(4),
                        .color = {0, 0, 0, 255}
                    },
                    // .layout.padding = { 32, 32, 32, 32 },
                }) {
                    CLAY_AUTO_ID({
                        .layout = {
                            .sizing = {
                                .width = CLAY_SIZING_FIXED(180),
                                .height = CLAY_SIZING_FIXED(180),
                            },
                        },
                        .custom = { .customData = guy }
                    });
                }

                CLAY_AUTO_ID({ .layout.sizing.width = CLAY_SIZING_GROW() });
            }

            switch (ui.guy_detail.tab) {
                case ui_DetailTab_Overview: {

                    switch (ui_big_button(
                        CLAY_STRING("RACE"),
                        ui_icon(ui_Icon_Telescope)
                    )) {
                        case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
                        case ui_Click_Released: {
                            ui.guy_detail.tab = ui_DetailTab_Race;
                        } break;
                        default: break;
                    }

                    switch (ui_big_button(
                        CLAY_STRING("GENES"),
                        ui_icon(ui_Icon_Soup)
                    )) {
                        case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
                        case ui_Click_Released: {
                            ui.guy_detail.tab = ui_DetailTab_Genes;
                        } break;
                        default: break;
                    }

                    switch (ui_big_button(
                        CLAY_STRING("RELATED"),
                        ui_icon(ui_Icon_Fecundity)
                    )) {
                        case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
                        case ui_Click_Released: {
                            ui.guy_detail.tab = ui_DetailTab_Relatives;
                        } break;
                        default: break;
                    }

                } break;

                case ui_DetailTab_Race: {
                    ui_race_summary(guy);
                } break;

                case ui_DetailTab_Genes: {
                    CLAY_AUTO_ID({
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .sizing = {
                                .width = CLAY_SIZING_GROW(0),
                                .height = CLAY_SIZING_GROW(0)
                            },
                            .padding = { 4, 4, 4, 4 },
                            .childGap = 4,
                        },
                        .clip = {
                            .horizontal = true,
                            .vertical = true,
                            .childOffset = Clay_GetScrollOffset()
                        },
                    }) {

                        ui_gene_tally_color(
                            guy,
                            "hair",
                            guy_color_hair(guy),
                            guy_GeneCategory_HairColor
                        );

                        ui_gene_tally_color(
                            guy,
                            "skin",
                            guy_color_skin(guy),
                            guy_GeneCategory_SkinColor
                        );

                        ui_gene_tally(
                            guy,
                            "kids per night",
                            guy_fecundity(guy),
                            guy_GeneCategory_Fecundity
                        );

                        ui_gene_tally(
                            guy,
                            "metabolism",
                            guy_metabolism(guy),
                            guy_GeneCategory_Metabolism
                        );

                        ui_gene_tally(
                            guy,
                            "girth",
                            guy_girth(guy),
                            guy_GeneCategory_Girth
                        );

                        ui_gene_tally(
                            guy,
                            "strength",
                            guy_strength(guy),
                            guy_GeneCategory_Strength
                        );
                    }
            
                    if (ui.guy_detail.gene_hovered) {
                        guy_GeneConfig *gene = ui.guy_detail.gene_hovered;
                        size_t gene_number = gene - &guy_gene_configs[0];
                        ui_sprintf(
                            tmp,
                            "%s gene #%ld (sex: %s)",
                            guy_race_names[gene->race],
                            gene_number,
                            guy_sex_str(gene->sex)
                        );
                        CLAY_TEXT(tmp, ui_font(ui_Font_Desc));
                    } else {
                        CLAY_TEXT(
                            CLAY_STRING("HOVER OVER GENE"),
                            ui_font(ui_Font_Desc)
                        );
                    }

                } break;

                case ui_DetailTab_Relatives: {
                    CLAY_TEXT(
                        CLAY_STRING("closest relatives:"),
                        ui_font(ui_Font_Desc)
                    );
                    ui_closest_relatives(guy);
                } break;
            }

        }

        CLAY_AUTO_ID({ .layout.sizing.height = CLAY_SIZING_GROW() });

        switch (ui_big_button(
            CLAY_STRING("BACK"),
            ui_icon(ui_Icon_Back)
        )) {
            case ui_Click_Pressed: sound_play(ui_sound(ui_Sound_Click)); break;
            case ui_Click_Released: {
                if (ui.guy_detail.tab == ui_DetailTab_Overview)
                    ui.guy_detail.active = false;
                else
                    ui.guy_detail.tab = ui_DetailTab_Overview;
            } break;
            default: break;
        }

    }

    ui.guy_detail.gene_hovered = NULL;

    return Clay_EndLayout(sapp_frame_duration());
}

void ui_guy_show_detail_page(guy_Guy *guy) {
    ui.guy_detail.active = true;
    ui.guy_detail.guy = guy;
}

bool ui_takeover(void) {
    return ui.guy_detail.active;
}

static Clay_Dimensions ui_measure_text(
    Clay_StringSlice text,
    Clay_TextElementConfig *config,
    void *_
) {
    draw_TextSize size = draw_measure_str((char *)text.chars, text.length, config->fontSize);
    return (Clay_Dimensions) {
        .width = size.width,
        .height = size.height,
    };
}

static void ui_render_cmds(Clay_RenderCommandArray render_cmds, draw_Geo *geo) {
    for (int j = 0; j < render_cmds.length; j++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(
            &render_cmds,
            j
        );
        Clay_BoundingBox bbox = cmd->boundingBox;
        float min_x = bbox.x;
        float min_y = bbox.y;
        float max_x = bbox.x + bbox.width;
        float max_y = bbox.y + bbox.height;
        switch (cmd->commandType) {

            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextRenderData *trd = &cmd->renderData.text;

                draw_geo_str_ui(
                    geo,
                    (f2){ bbox.x, bbox.y },
                    (char *)trd->stringContents.chars,
                    trd->stringContents.length,
                    trd->fontSize,
                    (Color) {
                        trd->textColor.r,
                        trd->textColor.g,
                        trd->textColor.b,
                        trd->textColor.a,
                    }
                );

                break;
            } break;

            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                Clay_ImageRenderData *ird = &cmd->renderData.image;

                Clay_Color tint = ird->tint;
                if (ird->tint.r == 0 && ird->tint.g == 0 &&
                    ird->tint.b == 0 && ird->tint.a == 0)
                    tint = (Clay_Color) { 255, 255, 255, 255 };

                draw_geo_tex(
                    geo,
                    ird->imageData,
                    (draw_Rect) {
                        .min_x = min_x,
                        .min_y = min_y,
                        .max_x = max_x,
                        .max_y = max_y,
                    },
                    (Color) { tint.r, tint.g, tint.b, tint.a }
                );
            } break;

            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
            } break;

            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
            } break;

            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_START: {
            } break;

            case CLAY_RENDER_COMMAND_TYPE_OVERLAY_COLOR_END: {
            } break;

            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleRenderData *config = &cmd->renderData.rectangle;
                Color c = (Color) {
                    config->backgroundColor.r,
                    config->backgroundColor.g,
                    config->backgroundColor.b,
                    config->backgroundColor.a
                };
                draw_geo_rect(
                    geo,
                    (draw_Rect) {
                        .min_x = min_x,
                        .min_y = min_y,
                        .max_x = max_x,
                        .max_y = max_y
                    },
                    c
                );
            } break;

            case CLAY_RENDER_COMMAND_TYPE_BORDER: {

                Clay_BorderRenderData *config = &cmd->renderData.border;
                Color c = (Color) {
                    config->color.r,
                    config->color.g,
                    config->color.b,
                    config->color.a
                };

                if (config->width.left > 0)
                    draw_geo_line(
                        geo,
                        (f2) { min_x, min_y },
                        (f2) { min_x, max_y },
                        config->width.left,
                        c
                    );

                if (config->width.right > 0)
                    draw_geo_line(
                        geo,
                        (f2) { max_x, min_y },
                        (f2) { max_x, max_y },
                        config->width.right,
                        c
                    );

                if (config->width.top > 0)
                    draw_geo_line(
                        geo,
                        (f2) { min_x, min_y },
                        (f2) { max_x, min_y },
                        config->width.top,
                        c
                    );

                if (config->width.bottom > 0)
                    draw_geo_line(
                        geo,
                        (f2) { min_x, max_y },
                        (f2) { max_x, max_y },
                        config->width.bottom,
                        c
                    );

            } break;

            case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
                Clay_CustomRenderData *config = &cmd->renderData.custom;
                guy_Guy *custom_element = (guy_Guy *)config->customData;
                if (!custom_element) continue;

                float w = cmd->boundingBox.width /2;
                float h = cmd->boundingBox.height/2;
                float x = cmd->boundingBox.x + w;
                float y = cmd->boundingBox.y + h + w/3;
                w -=  ui_font_size(ui_Font_Desc) + 5.0f;
                h -=  ui_font_size(ui_Font_Desc) + 5.0f;
                y -= (ui_font_size(ui_Font_Desc) + 5.0f)*0.5f;
                guy_draw_ex((guy_DrawEx) {
                    .guy = custom_element,
                    .pos = (f2) { x, y },
                    .target = (f2) { x, y },
                    .size = w,
                    .flags = guy_DrawFlags_Name,
                });
                break;
            } break;

            default: {
                printf("Error: unhandled render command.");
                exit(1);
            }
        }
    }
}

void ui_input(sapp_event* ev) {
    switch(ev->type) {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            ui.input.mouse_pos.x = ev->mouse_x / sapp_dpi_scale();
            ui.input.mouse_pos.y = ev->mouse_y / sapp_dpi_scale();
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            ui.input.mouse_down = true;
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            ui.input.mouse_down = false;
            break;
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            ui.input.scroll.x += ev->scroll_x;
            ui.input.scroll.y += ev->scroll_y;
            break;
        default: break;
    }
}
