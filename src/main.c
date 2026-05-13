#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_time.h"

#include <stdio.h>

#define __EAB_RAND_IMPL
#include "rand.h"

#include "base.h"
#define VIEW_HANDLERS
#include "view.h"
#include "draw.h"
#include "ui.h"

static struct {
    View view;
    sapp_mouse_cursor cursor_this_frame;
    uint64_t update_count;
    uint64_t start;
} game = {0};

float base_screen_size_x(void) { return sapp_widthf () / sapp_dpi_scale(); }
float base_screen_size_y(void) { return sapp_heightf() / sapp_dpi_scale(); }
double base_play_duration(void) { return stm_sec(stm_since(game.start)); }
void base_set_cursor(sapp_mouse_cursor c) { game.cursor_this_frame = c; }

static void init(void) {
    stm_setup();
    game.start = stm_now();

    draw_init();
    guy_system_init();
    ui_init();

    game.view = View_WorldMap;
    // save.run.coin = 185;
    save.run.furniture[0] = save_Furniture_Telescope;
    // save.run.furniture[0] = save_Furniture_Bed;
    // {
    //     guy_Guy mom = guy_from_race(guy_Race_Moai, guy_Sex_Female);
    //     guy_Guy dad = guy_from_race(guy_Race_Moai, guy_Sex_Male);
    //     for (int i = 0; i < 30; i++)
    //         save.run.guys[i] = guy_from_parents(&mom, &dad);
    // }
    // save.run.biome = save_Biome_Desert;
    view_handlers[game.view].init((view_Transition) {
        .battle.steps_from_root = 15
    });
}

static void cleanup(void) {
    view_handlers[game.view].free();

    ui_free();
    guy_system_free();
    draw_free();
}

static void frame(void) {
    sapp_set_mouse_cursor(game.cursor_this_frame);
    game.cursor_this_frame = SAPP_MOUSECURSOR_DEFAULT;

    game.update_count += 1;

    view_Transition transition = { 0 };

start:
    transition = view_handlers[game.view].update(game.update_count);
    transition.update = game.update_count;
    if (transition.kind != view_TransitionKind_NONE) {
        view_handlers[game.view].free();

        switch (transition.kind) {
            case view_TransitionKind_NONE: assert(false); break;

            case view_TransitionKind_Title: game.view = View_Title; break;
            case view_TransitionKind_Options: game.view = View_Options; break;
            case view_TransitionKind_CampTech: game.view = View_CampTech; break;

            case view_TransitionKind_StartRun: {
                uint32_t run_id = save.run.id;
                memset(&save.run, 0, sizeof(save.run));
                save.run.id = run_id + 1;
                save.run.coin = 0;
                save.run.food = 10;

                // save.run.furniture[0] = save_Furniture_Telescope;

                for (int i = 0; i < 3; i++) {
                    guy_Race race = guy_Race_Human;
                    guy_Sex sex = i%2 ? guy_Sex_Male : guy_Sex_Female;
                    save.run.guys[i] = guy_from_race(race, sex);
                }

                game.view = View_WorldMap;
            } break;

        //     case view_TransitionKind_StartPocketCamp:
        //     case view_TransitionKind_StartCamp: {
        //         game.view = View_Camp;
        //     } break;
        //     case view_TransitionKind_StartBattle: {
        //         game.view = View_Battle;
        //     } break;

        //     case view_TransitionKind_BattleDefeat: { 
        //         game.view = View_BattleDefeat;
        //     } break;

        //     case view_TransitionKind_BattleVictory: { 
        //         game.view = View_BattleVictory;
        //     } break;

        //     case view_TransitionKind_BuyFurniture: {
        //         game.view = View_Furniture;
        //     } break;

        //     case view_TransitionKind_CampFornications: {
        //         game.view = View_Fornications;
        //     }; break;

        //     case view_TransitionKind_BackToCampFromFornications: {
        //         game.view = View_Camp;
        //     }; break;

        //     case view_TransitionKind_BackToWorldMapFromPocketCamp:
        //     case view_TransitionKind_BackToWorldMap: {
        //         game.view = View_WorldMap;
        //     } break;

        };

        view_handlers[game.view].init(transition);
        goto start;
    }

    view_handlers[game.view].render();
}


static void input(const sapp_event *ev) {
    /* global input that overrides views */
    switch (ev->type) {
        case SAPP_EVENTTYPE_KEY_DOWN: {
#ifdef __APPLE__
            if (ev->key_code == SAPP_KEYCODE_Q &&
                (ev->modifiers & SAPP_MODIFIER_SUPER)
            )
                sapp_request_quit();
#endif
        } break;

        default: break;
    }

    view_handlers[game.view].input(ev);
}

sapp_desc sokol_main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = input,
        .width = 9*50,
        .height = 16*50,
        .window_title = "eab",
        .high_dpi = true,
        .logger.func = slog_func,
    };
}
