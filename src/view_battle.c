#include "view.h"
#include "ui.h"
#include "save.h"
#include "draw.h"
#include <string.h>
#include <stdio.h>

#define battle_LOG_TARGETTING false

typedef enum {
    battle_Team_Player,
    battle_Team_Baddie,
    battle_Team_COUNT,
} battle_Team;
typedef enum {
    battle_GuyPhase_NONE,
    battle_GuyPhase_Approaching,
    battle_GuyPhase_Attacking,
    battle_GuyPhase_Hurting,
    battle_GuyPhase_Fleeing,
} battle_GuyPhase;

typedef struct battle_Guy battle_Guy;
struct battle_Guy {
    /* if this is 0, you are said to be "out of phase," a euphemism for "dead" */
    battle_GuyPhase phase;
    battle_Team team;

    guy_Guy *guy;
    battle_Guy *target;

    /* used for hop animation */
    uint64_t last_attack_update;
    float distance_traveled_last_update;
    double swing_t;

    /* initiative exists to solve the problem of
     * "this guy hit me first, and he just kept hitting me until I died."
     * (this is a problem because you never get a swing in edgewise)
     *
     * at initialization and after an attack is launched, initiative is reset to
     * an integer like 10. 
     * whenever the enemy is in range, one is subtracted from the initiative.
     *
     * when it reaches 0, an attack is launched and initiative is reset.
     **/
    uint32_t initiative;

    f2 pos;

    struct {
        double t;
        f2 knockback;
    } last_hurt;
};

#define BADDIE_MAX_COUNT 101
#define COMBATANT_MAX_COUNT (countof(save.run.guys) + BADDIE_MAX_COUNT)
typedef enum {
    battle_Outcome_Unknown,
    battle_Outcome_Victory,
    battle_Outcome_Defeat,
} battle_Outcome;
static struct {
    battle_Outcome outcome;
    double outcome_t;
    bool outcome_can_leave;

    uint32_t original_baddie_count;
    float difficulty;

    view_Transition next_view;
    guy_Guy baddies[BADDIE_MAX_COUNT];
    uint64_t last_hit_update;

    battle_Guy guys[COMBATANT_MAX_COUNT];
    struct { bool active; f2 pos; float size; } graves[COMBATANT_MAX_COUNT];

    float food, coin;

    sound_Sound sound_hit, sound_dead;
} view;

static battle_Guy battle_guy_init(battle_Guy g) {
    g.initiative = rand_int(guy_initiative(g.guy));
    return g;
}

static size_t baddies_init(size_t steps_from_root) {
    float difficulty = -0.5f;

    difficulty += steps_from_root * 2.0f;
    difficulty += save.run.battles_won * 0.5f;
    difficulty += gaussian_randf(0.0f, (steps_from_root > 2)*1.0f);
    view.difficulty = difficulty;

    typedef enum {
        GuyBlendKind_Primary,
        GuyBlendKind_Split,
    } GuyBlendKind;
    struct {
        GuyBlendKind kind;
        guy_Race races[3];
    } blend = {0};

    blend.kind = GuyBlendKind_Primary;
    // blend.kind = RL_GetRandomValue(0, 1)
    //     ? GuyBlendKind_Split
    //     : GuyBlendKind_Primary;
    switch (save.run.biome) {

        case save_Biome_Plains: {
            blend.races[0] = guy_Race_Bunny;
            blend.races[1] = guy_Race_Human;
        } break;

        case save_Biome_Forest: {
            blend.races[0] = guy_Race_Birb;
            blend.races[1] = guy_Race_Raccoon;
        } break;

        case save_Biome_DarkForest: {
            blend.races[0] = guy_Race_Bat;
            blend.races[1] = guy_Race_Spider;
        } break;

        case save_Biome_Desert: {
            blend.races[0] = guy_Race_Moai;
        } break;

        case save_Biome_COUNT: return 0;

    }
    
    size_t unit_count = 0;
    uint32_t race_count = 0;
    for (size_t i = 0; i < countof(blend.races); i++) {
        race_count += blend.races[i] != guy_Race_NONE;
    }

    switch (blend.kind) {
        case GuyBlendKind_Primary: {
            guy_Race primary = blend.races[rand_int(race_count)];

#define HALF_GENERATION_SIZE 10
#define PRIMARY_COUNT (7*2) /* 70% primary, 30% other */
#define GENERATION_SIZE (HALF_GENERATION_SIZE*2)
            guy_Guy genepool_a[GENERATION_SIZE] = {0};
            guy_Guy genepool_b[GENERATION_SIZE] = {0};
            
            for (size_t i = 0; i < GENERATION_SIZE; i++) {
                guy_Sex sex = (i % 2) ? guy_Sex_Male : guy_Sex_Female;
                if (i < PRIMARY_COUNT || race_count == 1)
                    genepool_a[i] = guy_from_race(primary, sex);
                else {
                    guy_Race secondary = blend.races[rand_int(race_count)];
                    genepool_a[i] = guy_from_race(secondary, sex);
                }
            }

            guy_Guy *this_gen = genepool_a;
            guy_Guy *next_gen = genepool_b;
            for (int j = 0; j < 3; j++) {
                for (size_t i = 0; i < GENERATION_SIZE; i++) {
                    guy_Sex sex = (i % 2) ? guy_Sex_Male : guy_Sex_Female;

                    guy_Guy mom = this_gen[rand_int(HALF_GENERATION_SIZE)*2 + 0];
                    guy_Guy dad = this_gen[rand_int(HALF_GENERATION_SIZE)*2 + 1];
                    next_gen[i] = guy_from_parents_ex(&mom, &dad, sex);
                }

                guy_Guy *tmp_gen = next_gen;
                next_gen = this_gen;
                this_gen = tmp_gen;
            }

            float guy_juice = difficulty;
            while (guy_juice > 0) {
                guy_Guy mom = this_gen[rand_int(HALF_GENERATION_SIZE)*2 + 0];
                guy_Guy dad = this_gen[rand_int(HALF_GENERATION_SIZE)*2 + 1];
                guy_Guy guy = guy_from_parents(&mom, &dad);
                float guy_difficulty = 
                    guy_strength(&guy) *
                    guy_metabolism(&guy) *
                    guy_girth(&guy);

                if (guy_difficulty < 1.0f)
                    guy_difficulty = sqrtf(guy_difficulty);

                guy_juice -= guy_difficulty;

                view.baddies[unit_count++] = guy;
            }

#undef GENERATION_SIZE
#undef PRIMARY_COUNT
#undef HALF_GENERATION_SIZE
        } break;

        case GuyBlendKind_Split: {
        } break;
    }

    return unit_count;
}

void view_battle_init(view_Transition t) {
    memset(&view, 0, sizeof(view));

    view.last_hit_update = t.update;

    view.sound_hit  = sound_init("./resources/audio/hit.wav");
    view.sound_dead = sound_init("./resources/audio/dead.wav");

    float w = base_screen_size_x();
    float h = base_screen_size_y();
    size_t guy_i = 0;

    /* breed up some baddies */
    size_t unit_count = baddies_init(t.battle.steps_from_root);
    view.original_baddie_count = unit_count;
    for (size_t i = 0; i < unit_count; i++) {
        if (!view.baddies[i].state) break;

        float x = lerp(w*0.1, w*0.9, (float)i / (float)(unit_count - 1));
        float y = lerp(h*0.1, h*0.3, randf());
        view.guys[guy_i++] = battle_guy_init((battle_Guy) {
            .phase = battle_GuyPhase_Approaching,
            .team = battle_Team_Baddie,
            .guy = view.baddies + i,
            .pos = { x, y },
        });
    }

    size_t guy_count = 0;
    for (size_t i = 0; i < countof(save.run.guys); i++)
        guy_count += save.run.guys[i].state != guy_GuyState_NONE;

    for (size_t i = 0; i < countof(save.run.guys); i++) {
        guy_Guy *guy = save.run.guys + i;
        if (guy->state == guy_GuyState_NONE) continue;

        float x = lerp(w*0.1, w*0.9, (float)i / (float)(guy_count - 1));
        float y = lerp(h*0.7, h*0.9, randf());
        view.guys[guy_i++] = battle_guy_init((battle_Guy) {
            .phase = battle_GuyPhase_Approaching,
            .team = battle_Team_Player,
            .guy = guy,
            .pos = { x, y },
        });
    }
}
void view_battle_free(void) {

    sound_free(view.sound_hit);
    sound_free(view.sound_dead);
}

static float battle_guy_size(battle_Guy *bg) {
    return 30.0f * guy_size(bg->guy);
}

view_Transition view_battle_update(uint64_t update) {
    ui_update();

    /* win conditions, initiate fleeing */
    if (view.outcome == battle_Outcome_Unknown) {
        uint16_t baddie_count = 0;
        uint16_t player_count = 0;

        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase) continue;

            switch (bguy->team) {
                case battle_Team_Baddie: baddie_count += 1; break;
                case battle_Team_Player: player_count += 1; break;
                case battle_Team_COUNT: break;
            }
        }

        /* baddie surrender logic */
        if (baddie_count < view.original_baddie_count &&
            baddie_count <= 2 &&
            baddie_count < player_count
        ) {
            view.outcome = battle_Outcome_Victory;
            view.outcome_t = base_play_duration();

            for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
                battle_Guy *bguy = view.guys + guy_i;
                if (!bguy->phase) continue;
                if (bguy->team != battle_Team_Baddie) continue;

                bguy->phase = battle_GuyPhase_Fleeing;
                bguy->target = NULL;
            }
        }

        if (player_count == 0) {
            view.outcome = battle_Outcome_Defeat;
            view.outcome_t = base_play_duration();
        }
    }

    /* targeting */
    {
        uint16_t guy_target_counts[countof(view.guys)] = {0};

        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase) continue;

            /* actively drop targets who are fleeing
             * (not enough to just give them a really low target score) */
            if (bguy->target != NULL && (
                bguy->target->phase == battle_GuyPhase_Fleeing
            ))
                bguy->target = NULL;

            /* clear outdated targets */
            if (bguy->target != NULL && !bguy->target->phase)
                bguy->target = NULL;

            if (bguy->target != NULL) {
                size_t target_i = bguy->target - view.guys;
                guy_target_counts[target_i] += 1;
            }
        }

        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase) continue;
            if (bguy->phase == battle_GuyPhase_Fleeing) continue;

            battle_Guy *current_target = NULL;
            float current_target_score = -999e9;
            for (size_t guy_j = 0; guy_j < countof(view.guys); guy_j++) {
                battle_Guy *target = view.guys + guy_j;
                if (!target->phase) continue;
                if (target->phase == battle_GuyPhase_Fleeing) continue;

                /* can't target team members */
                if (target->team == bguy->team) continue;

                float target_score = 0;

                /* distance negatively impacts score */
                /* (probably not worth chasing that really far away guy) */
                float dist = sqrtf(
                    (target->pos.x - bguy->pos.x)*(target->pos.x - bguy->pos.x) +
                    (target->pos.y - bguy->pos.y)*(target->pos.y - bguy->pos.y)
                );
                target_score -= 0.005*dist;
                /* if they're super close - okay maybe just do it */
                if (dist < 40.0f) target_score += 4;

                /* many targets already = lower score */
                /* (anti dog-piling measure) */
                target_score -= guy_target_counts[target - view.guys];

                /* if someone is targeting you, well ... */
                target_score += 2*(target->target == bguy);

                /* prioritize staying locked on to current target */
                if ((update - view.last_hit_update) < 200)
                    target_score += 4*(target == bguy->target);

                if (target_score > current_target_score) {
                    current_target_score = target_score;
                    current_target = target;
                }
            }

#if battle_LOG_TARGETTING
            if (current_target != NULL && bguy->target != current_target) {
                char my_name[GUY_NAME_LEN_MAX] = {0};
                guy_name(bguy->guy, my_name);

                char ex_name[GUY_NAME_LEN_MAX] = {0};
                if (bguy->target) guy_name(bguy->target->guy, ex_name);

                char nu_name[GUY_NAME_LEN_MAX] = {0};
                guy_name(current_target->guy, nu_name);

                printf(
                    "[%s] switching target \"%s\" -> \"%s\"\n",
                    my_name, ex_name, nu_name
                );
            }
#endif

            bguy->target = current_target;
            if (current_target) {
                guy_target_counts[current_target - view.guys] += 1;
            }
        }
    }

    /* enemy AI phase state machine */
    {
        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
            battle_Guy *bguy = view.guys + guy_i;
            if (!bguy->phase)
                continue;

            float bguy_start_x = bguy->pos.x;
            float bguy_start_y = bguy->pos.y;

            /* distance towards which the guy moves in the approaching phase */
            float attacking_dist = 10.0f + guy_size(bguy->guy)*40;
            if (bguy->target != NULL && bguy->target->phase)
                attacking_dist += guy_size(bguy->target->guy)*40;

            switch (bguy->phase) {
                case battle_GuyPhase_NONE: continue;

                case battle_GuyPhase_Approaching: {
                    if (bguy->target == NULL) break;

                    float dx = bguy->target->pos.x - bguy->pos.x;
                    float dy = bguy->target->pos.y - bguy->pos.y;
                    float dlen = sqrtf(dx*dx + dy*dy);

                    float from_best_dist = dlen - attacking_dist;
                    {
                        float speed = min(
                            guy_speed(bguy->guy), fabsf(from_best_dist)
                        );
                        bguy->pos.x += dx/dlen * speed * sign(from_best_dist);
                        bguy->pos.y += dy/dlen * speed * sign(from_best_dist);
                    }

                    if (from_best_dist < 5) {

                        if (bguy->initiative == 0) {
                            bguy->phase = battle_GuyPhase_Attacking;
                            bguy->swing_t = base_play_duration();
                            bguy->last_attack_update = update;
                        } else {
                            bguy->initiative -= 1;
                        }
                    }
                }; break;

                case battle_GuyPhase_Attacking: {
                    if (bguy->target == NULL) {
                        bguy->phase = battle_GuyPhase_Approaching;
                        break;
                    }

                    float dx = bguy->target->pos.x - bguy->pos.x;
                    float dy = bguy->target->pos.y - bguy->pos.y;
                    float dlen = sqrtf(dx*dx + dy*dy);

                    /* "best dist" is a bit closer in attacking phase */
                    float from_best_dist = dlen - attacking_dist*0.8;
                    {
                        float speed = min(
                            guy_speed(bguy->guy), fabsf(from_best_dist)
                        );
                        bguy->pos.x += dx/dlen * speed * sign(from_best_dist);
                        bguy->pos.y += dy/dlen * speed * sign(from_best_dist);
                    }

                    uint64_t updates_since = update - bguy->last_attack_update;

                    if (updates_since == 20) {
                        /* initiative isn't spent until the attack is launched */
                        bguy->initiative = guy_initiative(bguy->guy);

                        /* find guys in range and kill them */
                        for (size_t victim_i = 0; victim_i < countof(view.guys); victim_i++) {
                            battle_Guy *victim = view.guys + victim_i;
                            if (!victim->phase) continue;

                            /* no friendly fire */
                            if (victim->team == bguy->team) continue;

                            float vdx = victim->pos.x - bguy->pos.x;
                            float vdy = victim->pos.y - bguy->pos.y;
                            float vdlen = sqrtf(vdx*vdx + vdy*vdy);

                            if (!(vdlen < (attacking_dist+15) && vdlen > 0))
                                continue;

                            view.last_hit_update = update;

                            float dmg = guy_damage(bguy->guy);
                            victim->phase = battle_GuyPhase_Hurting;
                            victim->last_hurt.knockback.x += dx/dlen * dmg;
                            victim->last_hurt.knockback.y += dy/dlen * dmg;

                            victim->guy->hp = max(0.0f, victim->guy->hp - roundf(dmg));
                            if (victim->guy->hp == 0) {
                                float meat = guy_meat(victim->guy);
                                view.food += meat;
                                save.run.food += meat;

                                for (size_t i = 0; i < countof(view.graves); i++) {
                                    if (view.graves[i].active)
                                        continue;
                                    view.graves[i].active = true;
                                    view.graves[i].pos = victim->pos;
                                    view.graves[i].size = guy_size(victim->guy);
                                    break;
                                }

                                victim->guy->state = guy_GuyState_NONE;
                                victim->guy = NULL;
                                victim->phase = battle_GuyPhase_NONE;

                                ui_FlyingIcon fi = {
                                    .start.x = victim->pos.x,
                                    .start.y = victim->pos.y,
                                    .end.x = base_screen_size_x()*0.9,
                                    .end.y = base_screen_size_y()*0.1,
                                    .start_t = base_play_duration(),
                                    .icon = ui_Icon_Food,
                                    .size = 10,
                                };
                                ui_flying_icon_end_t_from_speed(&fi, 0.004);
                                ui_flying_icon(fi);

                                sound_set_pitch(
                                    view.sound_dead,
                                    0.5f + (float)(guy_i % 12)/12
                                );
                                sound_play(view.sound_dead);

                            } else {
                                sound_set_pitch(
                                    view.sound_hit,
                                    lerp(0.35f, 1.1f, (float)(guy_i % 12)/12)
                                );
                                sound_play(view.sound_hit);
                            }
                        }
                    }

                    /**
                     * you could just have a higher initiative instead of this
                     * pause, but it's probably beneficial to have the guy not
                     * move for a big before launching the next bout */
                    if (updates_since > 30) {
                        bguy->phase = battle_GuyPhase_Approaching;
                    }
                }; break;

                case battle_GuyPhase_Hurting: {

                    /* you can still burn off initiative while hurting */
                    if (bguy->initiative > 0)
                        bguy->initiative -= 1;

                    bguy->pos.x += bguy->last_hurt.knockback.x;
                    bguy->pos.y += bguy->last_hurt.knockback.y;

                    bguy->last_hurt.knockback.x *= 0.9;
                    bguy->last_hurt.knockback.y *= 0.9;
                    bguy->last_hurt.t = base_play_duration();

                    float dlen = sqrtf(
                        bguy->last_hurt.knockback.x*bguy->last_hurt.knockback.x +
                        bguy->last_hurt.knockback.y*bguy->last_hurt.knockback.y
                    );
                    if (dlen < 5) {
                    }
                    if (dlen < 0.1) {
                        bguy->phase = battle_GuyPhase_Approaching;
                    }
                }; break;

                case battle_GuyPhase_Fleeing: {
                    float dx = base_screen_size_x()*0.5 - bguy->pos.x;
                    float dy = base_screen_size_y()*0.1 - bguy->pos.y;
                    float dlen = sqrtf(dx*dx + dy*dy);

                    float from_best_dist = dlen - attacking_dist;
                    {
                        float speed = min(
                            guy_speed(bguy->guy), fabsf(from_best_dist)
                        );
                        bguy->pos.x += dx/dlen * speed * sign(from_best_dist);
                        bguy->pos.y += dy/dlen * speed * sign(from_best_dist);
                    }

                }; break;
            }

            bguy->distance_traveled_last_update = sqrtf(
                (bguy->pos.x - bguy_start_x)*(bguy->pos.x - bguy_start_x) +
                (bguy->pos.y - bguy_start_y)*(bguy->pos.y - bguy_start_y)
            );

        }
    }

    /* push guys out of each other */
    for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
        battle_Guy *i = view.guys + guy_i;
        if (!i->phase) continue;

        for (size_t guy_j = guy_i+1; guy_j < countof(view.guys); guy_j++) {
            battle_Guy *j = view.guys + guy_j;
            if (!j->phase) continue;

            float dx = j->pos.x - i->pos.x;
            float dy = j->pos.y - i->pos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            float overlap = (battle_guy_size(i) + battle_guy_size(j)) - dist;
            if (overlap > 0) {
                j->pos.x += (dx/dist) * overlap/2;
                j->pos.y += (dy/dist) * overlap/2;
                i->pos.x -= (dx/dist) * overlap/2;
                i->pos.y -= (dy/dist) * overlap/2;
            }
        }
    }

    /* keep them on the screen */
    for (size_t guy_j = 0; guy_j < countof(view.guys); guy_j++) {
        battle_Guy *j = view.guys + guy_j;
        if (!j->phase) continue;

        float w = base_screen_size_x();
        float h = base_screen_size_y();
        j->pos.x = max(w*0.1, min(w*0.9, j->pos.x));
        j->pos.y = max(h*0.1, min(h*0.9, j->pos.y));
    }


    return view.next_view;
}

void view_battle_input(sapp_event *ev) {
    ui_input(ev);
}

static Clay_RenderCommandArray ui_create_layout(void);
void view_battle_render(void) {
    draw_frame_start(save_biome_color[save.run.biome]);

    for (size_t i = 0; i < countof(view.graves); i++) {
        if (!view.graves[i].active) continue;
        float size = view.graves[i].size * 20.0f;
        draw_geo_tex(
            draw_geo_default(),
            ui_icon(ui_Icon_Grave),
            (draw_Rect) {
                .min_x = view.graves[i].pos.x - size,
                .min_y = view.graves[i].pos.y - size,
                .max_x = view.graves[i].pos.x + size,
                .max_y = view.graves[i].pos.y + size,
            },
            (Color) { 255, 255, 255, 255 }
        );
    }

    for (size_t i = 0; i < countof(view.guys); i++) {
        battle_Guy *bguy = view.guys + i;
        if (!bguy->phase) continue;

        float x = bguy->pos.x;
        float y = bguy->pos.y;

        y += sinf(base_play_duration()*20 + GOLDEN_RATIO*i) *
            2 * bguy->distance_traveled_last_update;

        guy_DrawFlags flags = guy_DrawFlags_Hp;

        f2 target = {0};
        if (bguy->target != NULL) {
            flags |= guy_DrawFlags_Target;
            target.x = bguy->target->pos.x;
            target.y = bguy->target->pos.y;
        }

        guy_draw_ex((guy_DrawEx) {
            .guy = bguy->guy,
            .pos = (f2) { x, y },
            .target = target,
            .swing_t = bguy->swing_t,
            .hurt_t = bguy->last_hurt.t,
            .flags = flags
        });
    }

    ui_render(ui_create_layout(), draw_geo_default());
    // ui_render(test_layout()); /* instantly choose win/lose, good for testing */

    draw_frame_end();
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
            .padding = { 32, 32, 32, 32 },
            .childGap = 16,
        },
        .backgroundColor = {0}
    }) {

        CLAY_AUTO_ID({
            .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
        }) {
            CLAY_AUTO_ID({
                .layout = { .sizing = { .width = CLAY_SIZING_GROW() } },
            });

            Clay_String tmp;

            CLAY_AUTO_ID({
                .layout.layoutDirection = CLAY_TOP_TO_BOTTOM,
                .layout.childGap = 12,
            }) {
                CLAY_AUTO_ID({}) {
                    CLAY_AUTO_ID({
                        .layout = {
                            .sizing = {
                                .height = CLAY_SIZING_FIXED(32),
                                .width = CLAY_SIZING_FIXED(32),
                            },
                        },
                        .image = { .imageData = ui_icon(ui_Icon_Food) }
                    });

                    ui_sprintf(tmp, "x%.1f", save.run.food);
                    CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
                }

                CLAY_AUTO_ID({}) {
                    CLAY_AUTO_ID({
                        .layout = {
                            .sizing = {
                                .height = CLAY_SIZING_FIXED(32),
                                .width = CLAY_SIZING_FIXED(32),
                            },
                        },
                        .image = { .imageData = ui_icon(ui_Icon_Fleur) }
                    });

                    ui_sprintf(tmp, "x%d", save.run.coin);
                    CLAY_TEXT(tmp, ui_font(ui_Font_Cost));
                }
            }
        }

        CLAY_AUTO_ID({
            .layout = { .sizing = { .height = CLAY_SIZING_GROW() } },
        });

        double outcome_t = base_play_duration() - view.outcome_t;
        outcome_t -= 2.0;

        battle_Outcome outcome = view.outcome;

        if (outcome_t < 0)
            outcome = battle_Outcome_Unknown;

        switch (outcome) {

            case battle_Outcome_Unknown: {
            } break;

            case battle_Outcome_Victory: {
                size_t chars_since_outcome = floorf(
                    ((base_play_duration() - outcome_t) / 0.2)
                );

                char *button_text = "victory!!";
                size_t button_text_len = sizeof("victory!!") - 1;
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "%*.*s",
                    (int)button_text_len,
                    (int)chars_since_outcome,
                    button_text
                );
                if (!view.outcome_can_leave && (
                    chars_since_outcome >= button_text_len
                )) {
                    view.outcome_can_leave = true;
                    sound_play(ui_sound(ui_Sound_BattleVictory));

                    uint32_t new_coin = gaussian_rand(
                        view.difficulty,
                        (view.difficulty < 5) ? 1 : 3
                    );
                    new_coin = max(1, new_coin);
                    save.run.coin += new_coin;
                    view.coin += new_coin;

                    for (uint32_t i = 0; i < new_coin; i++) {
                        ui_FlyingIcon fi = {
                            .start.x = base_screen_size_x()*0.40,
                            .start.y = base_screen_size_y()*0.90,
                            .end.x   = base_screen_size_x()*0.80,
                            .end.y   = base_screen_size_y()*0.10,
                            .start_t = base_play_duration(),
                            .icon = ui_Icon_Fleur,
                            .size = 10,
                        };
                        ui_flying_icon_jitter(&fi, 10);
                        ui_flying_icon_end_t_from_speed(&fi, 0.004f);
                        fi.end_t += i*0.1;
                        ui_flying_icon(fi);
                    }

                }

                switch (ui_big_button(
                    tmp,
                    ui_icon(ui_Icon_Crown)
                )) {
                    case ui_Click_Pressed: {
                        sound_play(ui_sound(ui_Sound_PageTurn));
                    } break;
                    case ui_Click_Released: {

                        save.run.battles_won += 1;

                        view.next_view = (view_Transition) {
                            .kind = view_TransitionKind_BattleVictory,
                            .battle_victory = {
                                .coin = view.coin,
                                .food = view.food,
                            }
                        };

                        size_t capture_i = 0;
                        for (size_t guy_i = 0; guy_i < countof(view.guys); guy_i++) {
                            battle_Guy *bguy = view.guys + guy_i;
                            if (!bguy->phase) continue;
                            if (bguy->team != battle_Team_Baddie) continue;
                            guy_Guy *space = guy_alloc();
                            if (space == NULL) {
                                printf("no space for capture!");
                                break;
                            }

                            char my_name[GUY_NAME_LEN_MAX] = {0};
                            guy_name(bguy->guy, my_name);
                            printf("capturing %s!\n", my_name);

                            *space = *bguy->guy;
                            view.next_view.battle_victory.captured[capture_i++] = space;
                        }

                    } break;
                    default: break;
                }
            }; break;

            case battle_Outcome_Defeat: {
                size_t chars_since_outcome = floorf(
                    (outcome_t / 0.2)
                );

                char *button_text = "defeat";
                size_t button_text_len = sizeof("defeat") - 1;
                Clay_String tmp;
                ui_sprintf(
                    tmp,
                    "%*.*s",
                    (int)button_text_len,
                    (int)chars_since_outcome,
                    button_text
                );
                if (!view.outcome_can_leave && (
                    chars_since_outcome >= button_text_len
                )) {
                    view.outcome_can_leave = true;
                    sound_play(ui_sound(ui_Sound_BattleDefeat));
                }

                switch (ui_big_button(
                    tmp,
                    ui_icon(ui_Icon_Scroll)
                )) {
                    case ui_Click_Pressed: {
                        sound_play(ui_sound(ui_Sound_PageTurn));
                    } break;
                    case ui_Click_Released: {
                        view.next_view.kind = view_TransitionKind_BattleDefeat;
                    } break;
                    default: break;
                }
            }

        }
    }

    return Clay_EndLayout(base_play_duration());
}

static Clay_RenderCommandArray test_layout(void) {
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
        .backgroundColor = {0}
    }) {

        switch (ui_big_button(
            CLAY_STRING("victory"),
            ui_icon(ui_Icon_Crown)
        )) {
            case ui_Click_Pressed: {
                sound_play(ui_sound(ui_Sound_BattleVictory));
            } break;
            case ui_Click_Released: {
                view.next_view = (view_Transition) {
                    .kind = view_TransitionKind_BattleVictory,
                    .battle_victory = {
                        .coin = 23,
                        .food = 13,
                    }
                };
            } break;
            default: break;
        }

        switch (ui_big_button(
            CLAY_STRING("defeat"),
            ui_icon(ui_Icon_Scroll)
        )) {
            case ui_Click_Pressed: {
                sound_play(ui_sound(ui_Sound_BattleDefeat));
            } break;
            case ui_Click_Released: {
                view.next_view.kind = view_TransitionKind_BattleDefeat;
            } break;
            default: break;
        }
    }

    return Clay_EndLayout(sapp_frame_duration());
}
