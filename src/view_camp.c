#include "view.h"
#include "ui.h"
#include "save.h"
#include "draw.h"
#include "guy.h"
#include <math.h>
#include <stdio.h>

// #include "svg.h"
// #include "svg/orgy_circle.h"

#define ORGY_CIRCLE_X    175
#define ORGY_CIRCLE_Y    405
#define ORGY_CIRCLE_SIZE 250 /* radius */

#define RECYCLER_X 380
#define RECYCLER_Y 250
#define RECYCLER_SIZE 50

typedef enum {
    camp_ItemKind_NONE,
    camp_ItemKind_Furniture,
    camp_ItemKind_Guy,
} camp_ItemKind;
typedef struct {
    camp_ItemKind kind;
    union {
        save_Furniture furniture;

        /* TODO: generational indexing here so we can know when a guy is reused */
        guy_Guy *guy;
    };
    struct { float x, y; } pos;
} camp_Item;

static struct {
    size_t guys_in_orgy_circle_male;
    float guys_in_orgy_circle_cost_to_breed;
    float guys_in_orgy_circle_childcount_female;

    f2 mouse_down_pos;

    view_TransitionKind to_go_back;
    view_Transition next_view;
    double held_item_t;
    int held_item_idx;
    sound_Sound stew[2];
    tex_Tex orgy_circle;

    f2 mouse_pos;
    enum {
        view_Mouse_NONE,
        view_Mouse_Pressed,
        view_Mouse_Released,
    } mouse;
} view = {};

/* not in view because it's kept across view changes while view is reset */
static struct {
    uint32_t run_id;
    camp_Item items[countof(save.run.furniture) + countof(save.run.guys)];
} keep;

static camp_Item *camp_get_item(camp_ItemKind kind, size_t idx) {
    switch (kind) {
        case camp_ItemKind_NONE:
            return NULL;
        case camp_ItemKind_Furniture:
            return keep.items + idx;
        case camp_ItemKind_Guy:
            return keep.items + countof(save.run.furniture) + idx;
    }
}

static float camp_get_item_size(camp_Item *item) {
    switch (item->kind) {
        case camp_ItemKind_NONE:
            return 0;
        case camp_ItemKind_Furniture: {
            if (item->kind == camp_ItemKind_Furniture &&
                item->furniture == save_Furniture_Crown &&
                &keep.items[view.held_item_idx] == item
            )
                return -35; /* can penetrate objects up to this */
            return 25;
        }
        case camp_ItemKind_Guy:
            return 26*guy_size(item->guy);
    }
}

static bool camp_release_to_inspect(void) {
    
    float dist = sqrtf(
        (view.mouse_pos.x - view.mouse_down_pos.x)*
            (view.mouse_pos.x - view.mouse_down_pos.x) +
        (view.mouse_pos.y - view.mouse_down_pos.y)*
            (view.mouse_pos.y - view.mouse_down_pos.y)
    );

    return
        (dist < 5.0f) &&
        (view.held_item_idx > -1) &&
        (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy) &&
        ((base_play_duration() - view.held_item_t) < 0.5);
}

static camp_Item camp_make_item(camp_ItemKind kind) {
    float w = base_screen_size_x();
    float h = base_screen_size_y();
    float x = lerp(w*0.2, w*0.8, randf());
    float y = lerp(h*0.2, h*0.8, randf());

    /* push them out of the orgy circle */
    {
        float dist = sqrtf((x - ORGY_CIRCLE_X)*(x - ORGY_CIRCLE_X) +
                           (y - ORGY_CIRCLE_Y)*(y - ORGY_CIRCLE_Y));
        if (dist < ORGY_CIRCLE_SIZE/2) {
            x = ORGY_CIRCLE_X + (x - ORGY_CIRCLE_X)/dist * ORGY_CIRCLE_SIZE;
            y = ORGY_CIRCLE_Y + (y - ORGY_CIRCLE_Y)/dist * ORGY_CIRCLE_SIZE;
        }
    }

    return (camp_Item) {
        .kind = kind,
        .pos = { x, y }
    };
}

void view_camp_init(view_Transition t) {
    memset(&view, 0, sizeof(view));
    view.held_item_idx = -1;

    /* world map likes to know where you're coming from so it
     * knows whether or not to play a camp movement animation */
    view.to_go_back = view_TransitionKind_BackToWorldMap;
    if (t.kind == view_TransitionKind_StartPocketCamp)
        view.to_go_back = view_TransitionKind_BackToWorldMapFromPocketCamp;

    if (save.run.id != keep.run_id) {
        memset(&keep, 0, sizeof(keep));
        keep.run_id = save.run.id;
    }

    view.stew[0] = sound_init("./resources/audio/stew1.wav");
    view.stew[1] = sound_init("./resources/audio/stew2.wav");
    view.orgy_circle = tex_init("./resources/camp/orgy_circle.png");

    /* find items added to the save since last camp and initialize them */
    {
        for (size_t furn_i = 0; furn_i < countof(save.run.furniture); furn_i++) {
            save_Furniture f = save.run.furniture[furn_i];
            if (f == save_Furniture_NONE) continue;

            camp_Item *i = camp_get_item(camp_ItemKind_Furniture, furn_i);
            if (i->kind == camp_ItemKind_NONE) {
                *i = camp_make_item(camp_ItemKind_Furniture);
                i->furniture = f;
            }
        }

        for (size_t guy_i = 0; guy_i < countof(save.run.guys); guy_i++) {
            guy_Guy *g = save.run.guys + guy_i;
            if (g->state == guy_GuyState_NONE) {
                *g = (guy_Guy) {0};
                *camp_get_item(camp_ItemKind_Guy, guy_i) = (camp_Item) {0};
                continue;
            }

            camp_Item *i = camp_get_item(camp_ItemKind_Guy, guy_i);

            /* TODO: check if generational index is different and reset if this
             * guy's memory has been reused */

            if (i->kind == camp_ItemKind_NONE) {
                *i = camp_make_item(camp_ItemKind_Guy);
                i->guy = g;
            }
        }
    }
}
void view_camp_free(void) {
    sound_free(view.stew[0]);
    sound_free(view.stew[1]);
    tex_free(view.orgy_circle);
}

/* construct the "to_fornications" view transition */
static view_Transition to_fornications(void) {
    view_Transition ret = {
        .kind = view_TransitionKind_CampFornications,
    };

    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;
        float from_center =
            sqrtf((ORGY_CIRCLE_X - item->pos.x)*(ORGY_CIRCLE_X - item->pos.x) +
                  (ORGY_CIRCLE_Y - item->pos.y)*(ORGY_CIRCLE_Y - item->pos.y));

        if (from_center > (ORGY_CIRCLE_SIZE/2))
            continue;

        if (item->kind == camp_ItemKind_Guy) {
            for (size_t j = 0; j < countof(ret.fornications.in_orgy); j++) {
                if (ret.fornications.in_orgy[j] != NULL)
                    continue;
                ret.fornications.in_orgy[j] = item->guy;
                break;
            }
        }
    }
    
    return ret;
}

void view_camp_input(sapp_event *ev) {
    switch (ev->type) {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            view.mouse_pos.x = ev->mouse_x / sapp_dpi_scale();
            view.mouse_pos.y = ev->mouse_y / sapp_dpi_scale();
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            view.mouse = view_Mouse_Pressed;
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            view.mouse = view_Mouse_Released;
            break;
        default: break;
    }
    ui_input(ev);
}

view_Transition view_camp_update(uint64_t _) {

    /* push things in/out of the orgy circle */
    view.guys_in_orgy_circle_male = 0;
    view.guys_in_orgy_circle_cost_to_breed = 1;
    view.guys_in_orgy_circle_childcount_female = 0;
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;
        float from_center =
            sqrtf((ORGY_CIRCLE_X - item->pos.x)*(ORGY_CIRCLE_X - item->pos.x) +
                  (ORGY_CIRCLE_Y - item->pos.y)*(ORGY_CIRCLE_Y - item->pos.y));

        if (from_center < (ORGY_CIRCLE_SIZE/2)) {
            if (item->kind == camp_ItemKind_Guy) {
                view.guys_in_orgy_circle_male   += item->guy->sex == guy_Sex_Male;
                if (item->guy->sex == guy_Sex_Female) {
                    float fec = guy_fecundity(item->guy);
                    view.guys_in_orgy_circle_cost_to_breed += guy_girth(item->guy) * fec;
                    view.guys_in_orgy_circle_childcount_female += fec;
                }
            }
        }
        float from_edge = (ORGY_CIRCLE_SIZE/2 - 5) - from_center;
        float from_edge_min = 50;
        if (fabsf(from_edge) < from_edge_min) {
            float out_x = (item->pos.x - ORGY_CIRCLE_X) / from_center;
            float out_y = (item->pos.y - ORGY_CIRCLE_Y) / from_center;
            item->pos.x += out_x * (from_edge - from_edge_min*sign(from_edge))*0.4;
            item->pos.y += out_y * (from_edge - from_edge_min*sign(from_edge))*0.4;
        }
    }

    /* push things out of the guy recycler */
    for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
        camp_Item *j = keep.items + item_j;
        if (j->kind == camp_ItemKind_NONE) continue;

        float dx = j->pos.x - RECYCLER_X;
        float dy = j->pos.y - RECYCLER_Y;
        float dist = sqrtf(dx*dx + dy*dy);
        float overlap = (camp_get_item_size(j) + RECYCLER_SIZE) - dist;
        if (overlap > 0) {
            j->pos.x += (dx/dist) * overlap*0.5;
            j->pos.y += (dy/dist) * overlap*0.5;
        }
    }

    /* push items out of each other */
    for (size_t item_i = 0; item_i < countof(keep.items); item_i++) {
        camp_Item *i = keep.items + item_i;
        if (i->kind == camp_ItemKind_NONE) continue;

        for (size_t item_j = item_i + 1; item_j < countof(keep.items); item_j++) {
            camp_Item *j = keep.items + item_j;
            if (j->kind == camp_ItemKind_NONE) continue;

            float dx = j->pos.x - i->pos.x;
            float dy = j->pos.y - i->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (camp_get_item_size(i) + camp_get_item_size(j)) - dist;
            if (overlap > 0) {
                j->pos.x += (dx/dist) * overlap/2;
                j->pos.y += (dy/dist) * overlap/2;
                i->pos.x -= (dx/dist) * overlap/2;
                i->pos.y -= (dy/dist) * overlap/2;
            }
        }
    }

    /* keep them on the screen */
    for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
        camp_Item *j = keep.items + item_j;
        if (j->kind == camp_ItemKind_NONE) continue;

        float w = base_screen_size_x();
        float h = base_screen_size_y();
        j->pos.x = max(w*0.2, min(w*0.8, j->pos.x));
        j->pos.y = max(h*0.2, min(h*0.8, j->pos.y));
    }

    ui_update();
    return view.next_view;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_camp_render(void) {
    draw_frame_start(save_biome_color[save.run.biome]);

    /* held things go to mouse */
    if (view.held_item_idx != -1) {
        camp_Item *item = keep.items + view.held_item_idx;
        f2 m = view.mouse_pos;
        item->pos.x = m.x;
        item->pos.y = m.y;

        if (item->kind == camp_ItemKind_Furniture &&
            item->furniture == save_Furniture_Crown
        ) {
            for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
                camp_Item *j = keep.items + item_j;
                if (j->kind != camp_ItemKind_Guy) continue;
                if (j == item) continue;

                j->guy->crowned = false;
                
                float dx = j->pos.x - item->pos.x;
                float dy = j->pos.y - item->pos.y;
                if (sqrtf(dx*dx + dy*dy) < camp_get_item_size(j))
                    j->guy->crowned = true;
            }

        }
    }

    /* draw guy recycler */
    bool over_guy_recyler = false;
    {
        float x = RECYCLER_X;
        float y = RECYCLER_Y;
        float size = RECYCLER_SIZE + 10;

        ui_Font font = ui_Font_Desc;
        draw_geo_str(
            draw_geo_default(),
            (f2) { x - size*0.4, y - size*0.8 },
            "guy",
            ui_font_size(font),
            (Color) { 0, 0, 0, 255 }
        );
        draw_geo_str(
            draw_geo_default(),
            (f2) { x - size*0.7, y + size/2 },
            "recyler",
            ui_font_size(font),
            (Color) { 0, 0, 0, 255 }
        );

        bool holding_guy = (view.held_item_idx != -1) &&
            (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy);

        {
            float icon_size = size;
            float t = 0;
            if (holding_guy) {
                t = fabsf(sinf(base_play_duration()*7));
            }

            f2 m = view.mouse_pos;
            float dist = sqrtf((x - m.x)*(x - m.x) +
                               (y - m.y)*(y - m.y));
            if (holding_guy) {
                float t = inv_lerp(size, size/2, dist - size);
                icon_size *= 1.0 + 0.3*min(1, max(0, t));
            }

            over_guy_recyler = dist < (size/2);

            float ix = x;
            float iy = y - 10*t;
            float xsize = icon_size + 5*t;
            float ysize = icon_size - 5*t;
            draw_geo_tex(
                draw_geo_default(),
                ui_icon(ui_Icon_Soup),
                (draw_Rect) {
                    .min_x = ix - xsize/2,
                    .min_y = iy - ysize/2,
                    .max_x = ix + xsize/2,
                    .max_y = iy + ysize/2,
                },
                (Color) { 255, 255, 255, 255 }
            );
        }
    }

    /* find thing closest to mouse */
    camp_Item *item_closest_mouse = NULL;
    float item_closest_mouse_dist = 30.0f;
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;

        f2 m = view.mouse_pos;
        float dist = sqrtf((item->pos.x - m.x)*(item->pos.x - m.x) +
                           (item->pos.y - m.y)*(item->pos.y - m.y));
        if (dist < item_closest_mouse_dist) {
            item_closest_mouse_dist = dist;
            item_closest_mouse = item;
        }
    }

    /* draw items */
    for (size_t i = 0; i < countof(keep.items); i++) {
        camp_Item *item = keep.items + i;
        if (item->kind == camp_ItemKind_NONE) continue;

        float scale = 1;
        float rotation = 0;

        if (item_closest_mouse == item && view.held_item_idx == -1) {
            scale *= 1.05;
            rotation += sinf(base_play_duration()*15)*5.0f;

            base_set_cursor(SAPP_MOUSECURSOR_POINTING_HAND);
            if (view.mouse == view_Mouse_Pressed) {
                view.mouse_down_pos.x = view.mouse_pos.x;
                view.mouse_down_pos.y = view.mouse_pos.y;
                view.held_item_idx = i;
                view.held_item_t = base_play_duration();
            }
        }


        // RL_BeginMode2D((RL_Camera2D) {
        //     .offset = { item->pos.x, item->pos.y },
        //     .target = { item->pos.x, item->pos.y },
        //     .rotation = rotation,
        //     .zoom = scale,
        // });
        switch (item->kind) {
            case camp_ItemKind_NONE: break;

            case camp_ItemKind_Furniture: {
                float size = 50.0f * scale;
                save_FurnitureConfig *fc = save_furniture_configs + item->furniture;

                Color tint = { 255, 255, 255, 255 };

                if (item->furniture == save_Furniture_Crown) {
                    for (size_t item_j = 0; item_j < countof(keep.items); item_j++) {
                        camp_Item *j = keep.items + item_j;
                        if (j->kind != camp_ItemKind_Guy) continue;
                        if (j == item) continue;

                        if (j->guy->crowned) {
                            tint = (Color) { 255, 255, 255, 100 };
                            break;
                        }
                    }
                }

                draw_geo_tex(
                    draw_geo_default(),
                    ui_icon(fc->icon),
                    (draw_Rect) {
                        .min_x = item->pos.x - size/2,
                        .min_y = item->pos.y - size/2,
                        .max_x = item->pos.x + size/2,
                        .max_y = item->pos.y + size/2,
                    },
                    tint
                );
            }; break;

            case camp_ItemKind_Guy: {
                guy_DrawFlags flags = guy_DrawFlags_Hp;
                if (item_closest_mouse == item) flags |= guy_DrawFlags_Name;
                guy_draw(item->guy, item->pos.x, item->pos.y, flags);
            }; break;
        }
        // RL_EndMode2D();
    }


    /* draw orgy circle */
    {
        float x = ORGY_CIRCLE_X;
        float y = ORGY_CIRCLE_Y;
        float size = ORGY_CIRCLE_SIZE;
        f2 m = view.mouse_pos;
        bool hover = sqrtf((x - m.x)*(x - m.x) + (y - m.y)*(y - m.y)) < (size/2);
        hover = hover && (view.held_item_idx != -1);
        // svg_draw(&svg_orgy_circle, (svg_Rect) {
        //     .min_x = x-size/2,
        //     .min_y = y-size/2,
        //     .max_x = x+size/2,
        //     .max_y = y+size/2,
        // }, (Color) { hover ? 155 : 0, 0, 0, 255 } );
        draw_geo_tex(
            draw_geo_default(),
            view.orgy_circle,
            (draw_Rect) {
                .min_x = x-size/2,
                .min_y = y-size/2,
                .max_x = x+size/2,
                .max_y = y+size/2,
            },
            (Color) { hover ? 155 : 0, 0, 0, 255 }
        );

        ui_Font font = ui_Font_Desc;
        draw_geo_str(
            draw_geo_default(),
            (f2) { x + 10 - size/2, y - 30 - size/2 },
            "the orgy circle",
            ui_font_size(font),
            (Color) { 0, 0, 0, 255 }
        );

        char est_output[30] = {0};
        snprintf(
            est_output,
            sizeof(est_output),
            "est. output = %.1f",
            view.guys_in_orgy_circle_childcount_female
        );
        draw_geo_str(
            draw_geo_default(),
            (f2) { x + 15 - size/2, y + 15 + size/2 },
            est_output,
            ui_font_size(font),
            (Color) { 0, 0, 0, 255 }
        );
    }

    ui_render(ui_create_layout(), draw_geo_default());

    draw_frame_end();

    if (view.mouse == view_Mouse_Released) {
        if (over_guy_recyler &&
            (view.held_item_idx != -1) &&
            (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy)
        ) {
            camp_Item *i = keep.items + view.held_item_idx;

            save.run.food += guy_meat(i->guy);

            i->guy->state = guy_GuyState_NONE;
            i->kind = camp_ItemKind_NONE;
            sound_play(view.stew[rand_int(countof(view.stew))]);
        } else if (camp_release_to_inspect()) {
            ui_guy_show_detail_page(
                keep.items[view.held_item_idx].guy
            );
        }

        view.held_item_idx = -1;
    }

    view.mouse = view_Mouse_NONE;
}

ui_Click ui_small_button_with_cost(
    tex_Tex icon,
    bool disabled,
    float cost
) {
    ui_Click ret = false;

    CLAY(CLAY_IDI("camp_small_button", icon.id), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childGap = 5,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
            },
        }
    }) {
        ret = ui_small_button(icon, disabled);

        CLAY_AUTO_ID({
            .layout = {
                .sizing = { .width = CLAY_SIZING_GROW() },
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_CENTER,
                },
            },
        }) {


            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(32),
                        .width = CLAY_SIZING_FIXED(32),
                    },
                },
                .image = { .imageData = ui_icon(ui_Icon_Food) }
            });

            Clay_String tmp;
            ui_sprintf(tmp, "x%.1f", cost);
            CLAY_TEXT(tmp, ui_font(ui_Font_Cost));

        }
    }

    return ret;
}

static void ui_stat_icon(ui_Icon icon, uint32_t size) {
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {
                .height = CLAY_SIZING_FIXED(size),
                .width = CLAY_SIZING_FIXED(size),
            },
        },
        .image = { .imageData = ui_icon(icon) }
    });
}

static void ui_stat_tally(ui_Icon icon, float amount) {

    CLAY_AUTO_ID({
        .layout.childGap = 8,
        .layout.childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
    }) {
        ui_stat_icon(icon, 32);

        Clay_String tmp;
        ui_sprintf(tmp, "%.2f", amount);
        CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
    }
}

static void ui_hp_tally(guy_Guy *guy) {

    CLAY_AUTO_ID({
        .layout.childGap = 8,
        .layout.childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
    }) {
        ui_stat_icon(ui_Icon_Heal, 32);

        Clay_String tmp;
        ui_sprintf(tmp, "%d/%d", guy->hp, guy_maxhp(guy));
        CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
    }
}


static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 4 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        CLAY(CLAY_ID("title"), {
            .layout = {
                .padding = { 0, 0, 0, 24 },
                .sizing = { .width = CLAY_SIZING_GROW() },
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            }
        }) {
            CLAY_TEXT(
                CLAY_STRING("camp"),
                ui_font(ui_Font_SubTitle),
            );

            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
            });

            CLAY_AUTO_ID({
                .layout = {
                    .sizing = {
                        .height = CLAY_SIZING_FIXED(32),
                        .width = CLAY_SIZING_FIXED(32),
                    },
                },
                .image = { .imageData = ui_icon(ui_Icon_Food) }
            });

            Clay_String tmp;
            ui_sprintf(tmp, "x%.1f", save.run.food);
            CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
        }

        CLAY_AUTO_ID({
            .layout = { .sizing = { .height = CLAY_SIZING_GROW() } }
        });

        if ((view.held_item_idx > -1) &&
            (keep.items[view.held_item_idx].kind == camp_ItemKind_Guy)
        ) {

            if (camp_release_to_inspect()) CLAY_AUTO_ID({
                .layout.sizing.width = CLAY_SIZING_GROW(),
                .layout.childAlignment.x = CLAY_ALIGN_X_CENTER,
                .layout.childGap = 2,
            }) {
                ui_stat_icon(ui_Icon_Scroll, 28);
                CLAY_TEXT(CLAY_STRING(" release to inspect "), ui_font(ui_Font_Desc));
                ui_stat_icon(ui_Icon_Scroll, 28);
            }

            guy_Guy *guy = keep.items[view.held_item_idx].guy;
            CLAY_AUTO_ID({
                .layout.sizing.width = CLAY_SIZING_GROW(),
                .layout.padding.bottom = 20,
            }) {
                CLAY_AUTO_ID({
                    .layout.layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .layout.childGap = 16,
                }) {
                    ui_stat_tally(ui_Icon_Strength, guy_strength(guy));
                    ui_hp_tally(guy);
                }

                CLAY_AUTO_ID({
                    .layout.sizing.width = CLAY_SIZING_GROW(),
                    .layout.sizing.height = CLAY_SIZING_GROW(),
                    .layout.childAlignment.x = CLAY_ALIGN_X_CENTER,
                    .layout.childAlignment.y = CLAY_ALIGN_Y_CENTER,
                }) {
                    Clay_String tmp;
                    ui_sprintf(tmp, "%s", guy_sex_str(guy->sex));
                    CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
                }

                CLAY_AUTO_ID({
                    .layout.layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .layout.childGap = 16,
                }) {
                    ui_stat_tally(ui_Icon_Fecundity, guy_fecundity(guy) );
                    ui_stat_tally(ui_Icon_Speed,     guy_metabolism(guy));
                }
            }
        } else {
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW() },
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                },
            }) {
                float heal_cost = 0;
                float bed_cost = view.guys_in_orgy_circle_cost_to_breed *
                    (view.guys_in_orgy_circle_male > 0) *
                    (view.guys_in_orgy_circle_childcount_female > 0);
                for (size_t i = 0; i < countof(save.run.guys); i++) {
                    guy_Guy *g = save.run.guys + i;
                    if (g->state == guy_GuyState_NONE)
                        continue;

                    float heal_needed = (float)(guy_maxhp(g) - g->hp) / 100.0f;
                    heal_cost += heal_needed * guy_hunger(g);
                }

                CLAY_AUTO_ID({
                    .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
                }) {
                    switch (ui_small_button_with_cost(
                            ui_icon(ui_Icon_Bed),
                            (bed_cost == 0) || (save.run.food < bed_cost),
                            bed_cost
                        )) {
                        case ui_Click_Pressed: {
                            sound_play(ui_sound(ui_Sound_Click));
                        } break;
                        case ui_Click_Released: {
                            save.run.food -= bed_cost;
                            view.next_view = to_fornications();

                        } break;
                        default: break;
                    }

                    CLAY_AUTO_ID({
                        .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                    });

                    switch (ui_small_button_with_cost(
                            ui_icon(ui_Icon_Heal),
                            (heal_cost == 0) || (save.run.food < heal_cost),
                            heal_cost
                        )) {
                        case ui_Click_Pressed: {
                            sound_play(ui_sound(ui_Sound_Click));
                        } break;
                        case ui_Click_Released: {
                            save.run.food -= heal_cost;

                            for (size_t i = 0; i < countof(save.run.guys); i++) {
                                guy_Guy *g = save.run.guys + i;
                                if (g->state == guy_GuyState_NONE)
                                    continue;
                                g->hp = guy_maxhp(g);
                            }
                        } break;
                        default: break;
                    }

                    CLAY_AUTO_ID({
                        .layout = { .sizing = { .width = CLAY_SIZING_GROW() } }
                    });

                    switch (ui_small_button(
                        ui_icon(ui_Icon_BackToMap),
                        false
                    )) {
                        case ui_Click_Pressed: {
                            sound_play(ui_sound(ui_Sound_CampLeave));
                        } break;
                        case ui_Click_Released: {
                            view.next_view = (view_Transition) {
                                .kind = view.to_go_back
                            };
                        } break;
                        default: break;
                    }
                }
            }
        }
    }
    return Clay_EndLayout(sapp_frame_duration());
}
