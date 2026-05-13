#include "view.h"
#include "ui.h"
#include "save.h"
#include "ease.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "draw.h"

#define map_CAMP_MOVE_ANIM_DURATION 0.5
#define map_SKIP_BATTLES true
#define map_FREE_CAMERA false

typedef enum {
    map_StopStage_NONE,
    map_StopStage_New,
    map_StopStage_Visited,
} map_StopStage;
typedef enum {
    map_StopKind_NONE,
    map_StopKind_Battle,
    map_StopKind_Rest,
    map_StopKind_Chest,
    map_StopKind_Key,
    map_StopKind_Market,
} map_StopKind;
typedef struct Stop Stop;
struct Stop {
    map_StopStage stage;
    Stop *parent;

    uint16_t steps_from_root;
    save_Biome biome;

    map_StopKind kind;
    float x, y;

    struct {
        uint16_t variant;
        f2 pos;
    } assets[3];
};

#define map_STOPS_MAX 400
static struct {
    uint32_t run_id;
    bool inited;

    Stop all[map_STOPS_MAX];
    Stop *start, *current, *previous;

    /* next stop to allocate */
    Stop *next;
} stops;

#define map_MAX_ASSETS_PER_BIOME 10
static struct {
    double ts_telescope_out_anim_start;
    bool telescope_active;
    double ts_enter_anim_start, ts_enter_anim_done;
    double ts_move_anim_start, ts_move_anim_done;
    view_Transition next_view;
    draw_Geo terrain_geo;

    struct {
        size_t texture_count;
        tex_Tex textures[map_MAX_ASSETS_PER_BIOME];
    } biome_art[save_Biome_COUNT];

    sound_Sound sound_scope_out;

    struct {
        f2 pos;
        float zoom;

        /* don't do camera movement with the mouse, it's mine! */
        bool mouse_captured;

        /* this mouse movement is being used for camera movement */
        bool moving_cam;

        f2 mouse_pos_down;
    } cam;

    f2 mouse_pos;
    enum {
        view_Mouse_NONE,
        view_Mouse_Pressed,
        view_Mouse_Released,
    } mouse;
} view = {};

static void map_stops_init(void);
static void map_stops_layout(void);
static void map_biome_art_init(void);
void view_worldmap_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    if (save.run.id != stops.run_id) {
        memset(&stops, 0, sizeof(stops));
        stops.run_id = save.run.id;
    }
    
    draw_geo_init(&view.terrain_geo, 1 << 12);
    map_biome_art_init();

    if (!stops.inited) {
        stops.inited = true;

        map_stops_init();
        for (int i = 0; i < 10000; i++) map_stops_layout();
    }

    view.cam.pos.x = -stops.current->x;
    view.cam.pos.y = -stops.current->y;
    view.cam.zoom = 1.0f;

    view.ts_enter_anim_start = base_play_duration();
    view.ts_enter_anim_done = base_play_duration();
    if (t.kind == view_TransitionKind_StartRun) {
        view.ts_enter_anim_done += 3;
    }

    view.ts_move_anim_start = base_play_duration();
    view.ts_move_anim_done = base_play_duration();
    /* note that view_TransitionKind_BackToWorldMapFromPocketCamp
     * will not trigger this */
    if (t.kind == view_TransitionKind_BackToWorldMap) {
        view.ts_move_anim_done += map_CAMP_MOVE_ANIM_DURATION;
    }
    if (t.kind == view_TransitionKind_BackToWorldMapFromPocketCamp) {
        stops.previous = stops.current;
    }
}
static void map_biome_art_init(void) {
    struct {
        char *paths[map_MAX_ASSETS_PER_BIOME];
    } assets_for_biome[save_Biome_COUNT] = {
        [save_Biome_DarkForest] = {
            "darkforest_env1.png",
            "darkforest_env2.png",
            "darkforest_env3.png",
            "darkforest_env4.png",
            "darkforest_env5.png",
            "darkforest_env6.png",
        },
        [save_Biome_Desert] = {
            "desert_env1.png",
            "desert_env2.png",
            "desert_env3.png",
            "desert_env4.png",
        },
        [save_Biome_Forest] = {
            "forest_env1.png",
            "forest_env2.png",
            "forest_env3.png",
            "forest_env4.png",
            "forest_env5.png",
            "forest_env6.png",
            "forest_env7.png",
        },
        [save_Biome_Plains] = {
            "forest_env1.png",
            "forest_env2.png",
            "forest_env3.png",
            "forest_env4.png",
            "forest_env5.png",
            "forest_env6.png",
            "forest_env7.png",
        },
    };

    for (save_Biome biome = 0; biome < save_Biome_COUNT; biome++) {
        for (int i = 0; i < map_MAX_ASSETS_PER_BIOME; i++) {
            char *asset = assets_for_biome[biome].paths[i];
            if (asset == NULL) continue;

            char path[50] = {0};
            snprintf(path, sizeof(path) - 1, "resources/env/%s", asset);
            tex_Tex t = tex_init(path);
            view.biome_art[biome].textures[i] = t;
            view.biome_art[biome].texture_count += 1;
        }
    }

    view.sound_scope_out = sound_init("resources/src/audio/leather1.wav");
}

void view_worldmap_free(void) {
    for (save_Biome biome = 0; biome < save_Biome_COUNT; biome++) {
        for (
            size_t i = 0;
            i < view.biome_art[biome].texture_count;
            i++
        ) {
            tex_free(view.biome_art[biome].textures[i]);
        }
    }
    sound_free(view.sound_scope_out);

    draw_geo_free(&view.terrain_geo);
}

void view_worldmap_input(sapp_event *ev) {
    ui_input(ev);

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
}


view_Transition view_worldmap_update(uint64_t _) {
    // for (int i = 0; i < 100; i++) map_stops_layout();

    ui_update();
    return view.next_view;
}

static void map_stops_assign_assets(Stop *stop) {
    for (size_t i = 0; i < countof(stop->assets); i++) {
        // float x = lerpf(30, 80, randf()) * ((randf() < 0.5) ? -1 : 1);
        // float y = lerpf(30, 80, randf()) * ((randf() < 0.5) ? -1 : 1);
        float t = (float)i / (float)countof(stop->assets);
        float x = cosf(t*M_PI*2.0) * 60; // lerpf(50, 100, randf());
        float y = sinf(t*M_PI*2.0) * 60; // lerpf(50, 100, randf());
        stop->assets[i].pos.x = x;
        stop->assets[i].pos.y = y;
        stop->assets[i].variant = rand_int(
            view.biome_art[stop->biome].texture_count
        );
    }
}

static void map_stops_set_current(Stop *stop) {
    view.ts_move_anim_start = base_play_duration();
    view.ts_move_anim_done = base_play_duration() + map_CAMP_MOVE_ANIM_DURATION;
    stops.current->stage = map_StopStage_Visited;
    stops.previous = stops.current;
    stops.current = stop;
    save.run.biome = stop->biome;
}

static Stop *map_stops_init_arm(Stop *base, save_Biome biome, int length, float angle);
static void map_stops_init(void) {
    stops.next = stops.all;

    Stop *start = stops.next++;
    *start = (Stop) {
        .stage = map_StopStage_Visited,
        .kind = map_StopKind_NONE,
        .biome = save_Biome_Plains,
        .steps_from_root = 0,
        .x = 0,
        .y = 0,
    };
    map_stops_assign_assets(start);
    stops.start = start;
    stops.previous = start;
    stops.current = start;

    int arm_count = save_Biome_COUNT;
    for (int i = 1; i < arm_count; i++) {
        save_Biome biome = i;
        float jitter = 0;// 0.1 * (0.5f - randf());

        float t0 = (((float)i - 0.45f)/(float)(arm_count - 1));
        float t1 = (((float)i + 0.45f)/(float)(arm_count - 1));
        float angle0 = M_PI*2.0f * t0 + jitter;
        float angle1 = M_PI*2.0f * t1 + jitter;
        Stop *end = map_stops_init_arm(
            start,
            biome,
            /* note: ensuring bed placement assumes this is 1 or 2 */
            1 + rand_int(2),
            lerp_rads(angle0, angle1, 0.5)
        );

        int arm_count = 3 + rand_int(3);
        int arm_idx_key = rand_int(arm_count);
        int arm_idx_chest = arm_idx_key;
        while (arm_idx_chest == arm_idx_key)
            arm_idx_chest = rand_int(arm_count);
        for (int i = 0; i < arm_count; i++) {

            float imax = arm_count - 1;
            float _0angle0 = lerp_rads(angle0, angle1, (float)(i - 0.5f)/imax);
            float _0angle1 = lerp_rads(angle0, angle1, (float)(i + 0.5f)/imax);
            Stop *end0 = map_stops_init_arm(
                end,
                biome,
                1 + rand_int(4),
                lerp_rads(_0angle0, _0angle1, 0.5)
            );
            if (i == arm_idx_key  ) end0->kind = map_StopKind_Key;
            if (i == arm_idx_chest) end0->kind = map_StopKind_Chest;

            int arm_count = 3 + rand_int(3);
            for (int i = 0; i < arm_count; i++) {
                float imax = arm_count - 1;
                float _1angle0 = lerp_rads(_0angle0, _0angle1, (float)(i - 0.5f)/imax);
                float _1angle1 = lerp_rads(_0angle0, _0angle1, (float)(i + 0.5f)/imax);
                map_stops_init_arm(
                    end0,
                    biome,
                    rand_int(4),
                    lerp_rads(_1angle0, _1angle1, 0.5)
                );
            }
        }

        /* ensuring bed placement:
         * at the beginning, there are 3 battle choices;
         * after any of these first three battles, there has to be a bed */
        Stop *arm_start = (end->parent == start) ? end : end->parent;
        for (size_t i = 0; i < countof(stops.all); i++) {
            Stop *s = stops.all + i;
            if (s->parent == arm_start) {
                s->kind = map_StopKind_Rest;
                break;
            }
        }
    }
}

static void map_stops_layout(void) {

    /* pull towards parents */
    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        Stop *p = i->parent;
        if (!i->stage) continue;
        if (!i->parent) continue;

        float dx = p->x - i->x;
        float dy = p->y - i->y;
        float dist = sqrtf(dx*dx + dy*dy);

        float optimal_dist = 100.0f;
        if (i->parent == stops.start) optimal_dist = 50.0f;

        float stretch = dist - optimal_dist;
        if (stretch > 0) {
            p->x -= (dx/dist) * stretch/2 * 0.1;
            p->y -= (dy/dist) * stretch/2 * 0.1;
            i->x += (dx/dist) * stretch/2 * 0.1;
            i->y += (dy/dist) * stretch/2 * 0.1;
        }
    }

    /* push away from each other */
    for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
        Stop *i = stops.all + stop_i;
        if (!i->stage) continue;

        for (size_t stop_j = stop_i + 1; stop_j < countof(stops.all); stop_j++) {
            Stop *j = stops.all + stop_j;
            if (!j->stage) continue;

            float dx = j->x - i->x;
            float dy = j->y - i->y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (100 + 100) - dist;
            if (overlap > 0) {
                j->x += (dx/dist) * overlap/2 * 0.1;
                j->y += (dy/dist) * overlap/2 * 0.1;
                i->x -= (dx/dist) * overlap/2 * 0.1;
                i->y -= (dy/dist) * overlap/2 * 0.1;
            }
        }
    }
}

static Stop *map_stops_init_arm(Stop *base, save_Biome biome, int length, float angle) {
    Stop *last = base;
    for (int i = 0; i < length; i++) {
        Stop *next = stops.next++;

        float dist = 100;
        float step_angle = angle + 0.1*(0.5 - randf());
        float fx = last->x + cosf(step_angle) * dist;// + 50*(0.5 - randf());
        float fy = last->y + sinf(step_angle) * dist;// + 50*(0.5 - randf());
        *next = (Stop) {
            .parent = last,
            .steps_from_root = last->steps_from_root + 1,
            .stage = map_StopStage_New,
            .x = fx,
            .y = fy,
        };

        next->biome = (next->steps_from_root < 3)
            ? save_Biome_Plains
            : biome;
        map_stops_assign_assets(next);

        float f = randf();
             if (f < 0.8) next->kind = map_StopKind_Battle;
        else if (f < 0.9)
            next->kind = (next->steps_from_root > 3)
                ? map_StopKind_Market
                : map_StopKind_Rest;
        else              next->kind = map_StopKind_Rest;

        if (next->parent->kind == map_StopKind_Rest || 
            next->parent->kind == map_StopKind_NONE)
            next->kind = map_StopKind_Battle;

        last = next;
    }

    return last;
}

static bool map_stop_available(Stop *stop) {
    return stop->parent == stops.current || stops.current->parent == stop;
}

static bool map_stop_complete(Stop *stop) {
    return stop->stage == map_StopStage_Visited;
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_worldmap_render(void) {


    {
        double cam_t = max(0, min(1, inv_lerp(
                view.ts_move_anim_start + map_CAMP_MOVE_ANIM_DURATION,
                view.ts_move_anim_done + map_CAMP_MOVE_ANIM_DURATION,
                base_play_duration()
        )));
        cam_t = ease_out_sine_double(cam_t);
        view.cam.pos.x = -lerp(stops.previous->x, stops.current->x, cam_t);
        view.cam.pos.y = -lerp(stops.previous->y, stops.current->y, cam_t);
    }

    // float cx = view.cam.pos.x;
    // float cy = view.cam.pos.y;
#if map_FREE_CAMERA
    // if (!view.cam.mouse_captured) { /* free camera controls */
    //     RL_Vector2 m = RL_GetMousePosition();

    //     if (RL_IsMouseButtonPressed(0)) {
    //         view.cam.mouse_pos_down.x = m.x;
    //         view.cam.mouse_pos_down.y = m.y;
    //         view.cam.moving_cam = true;
    //     }
    //     if (RL_IsMouseButtonDown(0) && view.cam.moving_cam) {
    //         cx += m.x - view.cam.mouse_pos_down.x;
    //         cy += m.y - view.cam.mouse_pos_down.y;
    //     }
    //     if (RL_IsMouseButtonReleased(0) && view.cam.moving_cam) {
    //         view.cam.pos.x += m.x - view.cam.mouse_pos_down.x;
    //         view.cam.pos.y += m.y - view.cam.mouse_pos_down.y;
    //         cx = view.cam.pos.x;
    //         cy = view.cam.pos.y;
    //         view.cam.moving_cam = false;
    //     }

    //     view.cam.zoom += 0.02*RL_GetMouseWheelMoveV().y;
    //     view.cam.zoom = fabsf(view.cam.zoom);
    // }
#endif

    /* this may get set back to true by the end of this function */
    view.cam.mouse_captured = false;

    {
        float zoom = lerp(
            0.01f,
            0.9f * view.cam.zoom,
            ease_out_sine_double(min(1, inv_lerp(
                view.ts_enter_anim_start,
                view.ts_enter_anim_done,
                base_play_duration()
            )))
        );

        {
            const float telescope_zoom = 0.35f;
            const double anim_duration = 1.0;

            double t = min(1, inv_lerp(
                view.ts_telescope_out_anim_start,
                view.ts_telescope_out_anim_start + anim_duration,
                base_play_duration()
            ));

            if (view.telescope_active)
                zoom = lerp(
                    zoom,
                    telescope_zoom,
                    ease_out_sine_double(t)
                );
            else
                zoom = lerp(
                    telescope_zoom,
                    zoom,
                    ease_in_sine_double(t)
                );
        }
    }

    draw_frame_start((Color) { 97, 131, 161, 255 });

    save_Biome biome_render_order[save_Biome_COUNT] = {
        save_Biome_Desert,
        save_Biome_DarkForest,
        save_Biome_Forest,
        save_Biome_Plains,
    };
    for (int i = 0; i < save_Biome_COUNT; i++) {
        save_Biome b = biome_render_order[i];

        for (size_t i = 0; i < countof(stops.all); i++) {
            Stop *stop = stops.all + i;
            if (!stop->stage) break;
            if (stop->biome != b) continue;

            draw_geo_circle(
                &view.terrain_geo,
                (f2) { stop->x, stop->y },
                140.0f,
                save_biome_color[stop->biome]
            );
        }
    }

    for (size_t s = 0; s < countof(stops.all); s++) {
        Stop *stop = stops.all + s;
        if (!stop->stage) continue;

        for (size_t i = 0; i < countof(stop->assets); i++) {
            tex_Tex t = view
                .biome_art[stop->biome]
                .textures[stop->assets[i].variant];

            // RL_DrawLineBezier(
            float scale = 0.1;
            draw_geo_tex(
                &view.terrain_geo,
                t,
                draw_rect_make_from_top_left(
                    stop->x + stop->assets[i].pos.x - tex_size_x(t)*0.5*scale,
                    stop->y + stop->assets[i].pos.y - tex_size_y(t)*0.8*scale,
                    scale
                ),
                (Color) { 255, 255, 255, 75 }
            );

            // if (0) RL_DrawCircle(
            //     stop->x + stop->assets[i].pos.x,
            //     stop->y + stop->assets[i].pos.y,
            //     10,
            //     (RL_Color) { 255, 0, 0, 255 }
            // );
        }
    }

    for (size_t i = 0; i < countof(stops.all); i++) {
        Stop *stop = stops.all + i;
        if (!stop->stage) continue;
        if (!stop->parent) continue;

        // RL_DrawLineBezier(
        draw_geo_line(
            &view.terrain_geo,
            (f2) { stop->x, stop->y },
            (f2) { stop->parent->x, stop->parent->y },
            10.0f,
            (Color) { 119, 74, 82, 255 }
        );
    }

    for (size_t i = 0; i < countof(stops.all); i++) {
        Stop *stop = stops.all + i;
        if (!stop->stage) continue;
        float x = stop->x;
        float y = stop->y;

        ui_Icon icon = 0;
        switch (stop->kind) {
            case map_StopKind_NONE:   icon = ui_Icon_Hole; break;
            case map_StopKind_Battle: icon = ui_Icon_Swords; break;
            case map_StopKind_Rest:   icon = ui_Icon_Bed; break;
            case map_StopKind_Chest:  icon = ui_Icon_Chest; break;
            case map_StopKind_Key:    icon = ui_Icon_Key; break;
            case map_StopKind_Market: icon = ui_Icon_Market; break;
        }
        float size = 65;

        Color tint = (Color){ 255, 255, 255, 255 };

        /* places you've been before are the grave */
        if (map_stop_complete(stop))
            icon = ui_Icon_Captured;

        if (stops.current != stop) {
            /* but new things you can move to pulse */
            if (!map_stop_complete(stop) && map_stop_available(stop))
                size *= 1.0f + 0.1*(
                    1 + 0.5*sinf(base_play_duration()*10)
                );

            /* things you can't click on are tiny and dark */
            if (!map_stop_available(stop)) {
                size *= lerp(0.7, 1.0, map_stop_complete(stop));
                tint = (Color) { 120, 120, 120, 255 };
            }
        }

        if (stops.current == stop) {
            float t = 1;
            if (stops.previous != stop) {
                t = min(1, inv_lerp(
                        view.ts_move_anim_start,
                        view.ts_move_anim_done,
                        base_play_duration()
                ));
                t = ease_out_circ(t);
            }

            /* draw a fading out version of this spot's icon */
            draw_geo_tex(
                &view.terrain_geo,
                ui_icon(icon),
                (draw_Rect) {
                    .min_x = stop->x - size/2,
                    .max_x = stop->x + size/2,
                    .min_y = stop->y - size/2,
                    .max_y = stop->y + size/2
                },
                (Color) { 255, 255, 255, lerp(255, 0, t) }
            );

            /* hijack this space to draw the moving camp icon */
            icon = ui_Icon_Camp;
            x = lerp(stops.previous->x, x, t);
            y = lerp(stops.previous->y, y, t);
        }

        if (map_stop_available(stop)) {
            f2 m = draw_camera_world_from_screen(&view.terrain_geo.camera, view.mouse_pos);
            float dist = sqrtf((m.x - x)*(m.x - x) + (m.y - y)*(m.y - y));

            /* where this stop is on the screen */
            f2 screen = draw_camera_screen_from_world(&view.terrain_geo.camera, (f2) {
                stop->x,
                stop->y,
            });

            if (dist < size*0.5) do {
                size *= 1.15;
                view.cam.mouse_captured = true;

                base_set_cursor(SAPP_MOUSECURSOR_POINTING_HAND);

                if (view.mouse == view_Mouse_Pressed && map_stop_complete(stop)) {
                    map_stops_set_current(stop);
                    break;
                }

                switch (stop->kind) {

                    case map_StopKind_Battle: {
                        if (view.mouse == view_Mouse_Pressed)
                            sound_play(ui_sound(ui_Sound_BattleEnter));
                        if (view.mouse == view_Mouse_Released) {
                            map_stops_set_current(stop);
                            view.next_view.battle.steps_from_root = stop->steps_from_root;
#if !map_SKIP_BATTLES
                            view.next_view.kind = view_TransitionKind_StartBattle;
#endif
                        }
                    } break;

                    case map_StopKind_Rest: {
                        if (save_count_furniture(save_Furniture_PocketCamp) > 0) {
                            if (view.mouse == view_Mouse_Pressed)
                                sound_play(ui_sound(ui_Sound_GotFood));
                            if (view.mouse == view_Mouse_Released) {
                                int new_food = gaussian_rand(5, 2);
                                new_food = max(0, new_food);

                                save.run.food += new_food;

                                for (int i = 0; i < new_food; i++) {
                                    ui_FlyingIcon fi = {
                                        .start.x = screen.x,
                                        .start.y = screen.y,
                                        .end.x = base_screen_size_x()*0.9,
                                        .end.y = base_screen_size_y()*0.1,
                                        .start_t = base_play_duration() + i*0.02,
                                        .icon = ui_Icon_Food,
                                        .size = 10,
                                    };
                                    ui_flying_icon_jitter(&fi, 10);
                                    ui_flying_icon_end_t_from_speed(&fi, 0.004f);
                                    ui_flying_icon(fi);
                                }

                                map_stops_set_current(stop);
                            }
                            break;
                        }

                        if (view.mouse == view_Mouse_Pressed)
                            sound_play(ui_sound(ui_Sound_CampEnter));
                        if (view.mouse == view_Mouse_Released) {
                            map_stops_set_current(stop);
                            view.next_view.kind = view_TransitionKind_StartCamp;
                        }
                    } break;
                    
                    case map_StopKind_Market: {
                        if (view.mouse == view_Mouse_Pressed)
                            sound_play(ui_sound(ui_Sound_CampEnter));
                        if (view.mouse == view_Mouse_Released) {
                            map_stops_set_current(stop);
                            view.next_view.kind = view_TransitionKind_BuyFurniture;
                        }
                    } break;
                    
                    case map_StopKind_Chest: {
                        if (view.mouse == view_Mouse_Released) {
                            if (save.run.key_count > 0) {
                                save.run.key_count -= 1;
                                map_stops_set_current(stop);
                                sound_play(ui_sound(ui_Sound_FurnitureUnlock));

                                uint32_t coins_earned = gaussian_rand(15, 5);
                                save.run.coin += coins_earned;

                                for (uint32_t i = 0; i < coins_earned; i++) {
                                    ui_FlyingIcon fi = {
                                        .start.x = screen.x,
                                        .start.y = screen.y,
                                        .end.x = base_screen_size_x()*0.90,
                                        .end.y = base_screen_size_y()*0.05,
                                        .start_t = base_play_duration(),
                                        .icon = ui_Icon_Fleur,
                                        .size = 10,
                                    };
                                    ui_flying_icon_jitter(&fi, 10);
                                    ui_flying_icon_end_t_from_speed(&fi, 0.004f);
                                    fi.end_t += i*0.1;
                                    ui_flying_icon(fi);
                                }

                            } else {
                                sound_play(ui_sound(ui_Sound_CampLeave));
                            }
                        }
                    } break;

                    case map_StopKind_Key: {
                        if (view.mouse == view_Mouse_Pressed) {
                            sound_play(ui_sound(ui_Sound_CampLeave));
                        } else if (view.mouse == view_Mouse_Released) {
                            save.run.key_count += 1;
                            map_stops_set_current(stop);

                            ui_FlyingIcon fi = {
                                .start.x = screen.x + lerpf(-20, 20, randf()),
                                .start.y = screen.y + lerpf(-20, 20, randf()),
                                .end.x = base_screen_size_x()*0.85,
                                .end.y = base_screen_size_y()*0.15,
                                .start_t = base_play_duration(),
                                .icon = ui_Icon_Key,
                                .size = 12,
                            };
                            ui_flying_icon_end_t_from_speed(&fi, 0.004);
                            ui_flying_icon(fi);

                        }
                    } break;

                    case map_StopKind_NONE: {
                        if (view.mouse == view_Mouse_Released) {
                            map_stops_set_current(stop);
                        }
                    } break;
                }
            } while (false);
        }

        draw_geo_tex(
            &view.terrain_geo,
            ui_icon(icon),
            (draw_Rect) {
                .min_x = x - size/2,
                .max_x = x + size/2,
                .min_y = y - size/2,
                .max_y = y + size/2
            },
            tint
        );
    }

    view.terrain_geo.camera.focus.x = view.cam.pos.x + base_screen_size_x()*0.5;
    view.terrain_geo.camera.focus.y = view.cam.pos.y + base_screen_size_y()*0.5;
    view.terrain_geo.camera.zoom = view.cam.zoom;
    draw_geo_draw(&view.terrain_geo);
    draw_geo_reset(&view.terrain_geo);
    /* cam end */


    draw_geo_circle(draw_geo_default(), view.mouse_pos, 10.0f,
        (Color) { 255, 0, 0, 255 });

    ui_render(ui_create_layout(), draw_geo_default());

    draw_frame_end();
    view.mouse = view_Mouse_NONE;
}

static void ui_tally(ui_Icon icon, uint32_t count) {
    CLAY_AUTO_ID({
        .layout.childAlignment = {
            .y = CLAY_ALIGN_Y_CENTER,
        },
    }) {
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {
                    .height = CLAY_SIZING_FIXED(24),
                    .width = CLAY_SIZING_FIXED(24),
                },
            },
            .image = { .imageData = ui_icon(icon) }
        });

        Clay_String tmp;
        ui_sprintf(tmp, " x%d", count);
        CLAY_TEXT(tmp, ui_font_ex(ui_Font_Desc, (Clay_TextElementConfig) {
            .textColor = { 255, 255, 255, 255 },
        }));
    }
}

static Clay_RenderCommandArray ui_create_layout(void) {
    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = { 32, 32, 32, 32 },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
        },
    }) {

        CLAY_AUTO_ID({
            .layout.sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
        }) {
            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 10,
                }
            }) {
                Clay_TextElementConfig text = ui_font_ex(ui_Font_Desc, (Clay_TextElementConfig) {
                    .textColor = { 255, 255, 255, 255 },
                });
                Clay_String tmp;

                uint32_t stops_total = 0;
                uint32_t stops_conquered = 0;
                for (size_t stop_i = 0; stop_i < countof(stops.all); stop_i++) {
                    Stop *i = stops.all + stop_i;
                    switch (i->stage) {
                        case map_StopStage_NONE: break;
                        case map_StopStage_Visited: {
                            stops_total++;
                            stops_conquered++;
                        } break;
                        case map_StopStage_New: {
                            stops_total++;
                        } break;
                    }
                }

                float p = 100.0f * (float)stops_conquered/(float)stops_total;
                ui_sprintf(tmp, "%.1f%% conquered", p);
                CLAY_TEXT(tmp, text);
            }

            CLAY_AUTO_ID({ .layout.sizing.width = CLAY_SIZING_GROW() });

            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 10,
                }
            }) {
                ui_tally(ui_Icon_Fleur, save.run.coin);
                ui_tally(ui_Icon_Food, save.run.food);
                if (save.run.key_count > 0)
                    ui_tally(ui_Icon_Key, save.run.key_count);
            }
        }

        CLAY_AUTO_ID({ .layout.sizing.height = CLAY_SIZING_GROW() });

        CLAY_AUTO_ID({
            .layout.sizing.width = CLAY_SIZING_GROW(),
            .layout.childGap = 16,
        }) {
            CLAY_AUTO_ID({ .layout.sizing.width = CLAY_SIZING_GROW() });

            if (save_count_furniture(save_Furniture_Telescope)) {
                switch (ui_small_button_ex((ui_SmallButton_Config) {
                    .icon = ui_icon(ui_Icon_Telescope),
                    .clr_normal = (Color) { 128, 128, 128, 168 },
                    .clr_hovered = (Color) { 255, 255, 255, 168 },
                })) {
                    case ui_Click_Pressed: {
                        sound_play(view.sound_scope_out);
                    } break;

                    case ui_Click_Released: {
                        double done = view.ts_telescope_out_anim_start + 1.0f;
                        if (done < base_play_duration()) {
                            view.ts_telescope_out_anim_start = base_play_duration();
                            view.telescope_active ^= 1;
                        }
                    } break;

                    case ui_Click_NONE: break;
                };
            }

            if (save_count_furniture(save_Furniture_PocketCamp)) {
                switch (ui_small_button_ex((ui_SmallButton_Config) {
                    .icon = ui_icon(ui_Icon_Bed),
                    .clr_normal = (Color) { 128, 128, 128, 168 },
                    .clr_hovered = (Color) { 255, 255, 255, 168 },
                })) {
                    case ui_Click_Pressed: {
                        sound_play(ui_sound(ui_Sound_CampEnter));
                    } break;

                    case ui_Click_Released: {
                        view.next_view.kind = view_TransitionKind_StartPocketCamp;
                    } break;

                    case ui_Click_NONE: break;
                };
            }
        }

    }

    return Clay_EndLayout(sapp_frame_duration());
}
