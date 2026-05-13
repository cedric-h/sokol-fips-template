#ifndef __EAB_VIEW_IMPL
#define __EAB_VIEW_IMPL
#include <stdint.h>
#include "guy.h"
#include "save.h"

#define views \
    x(      options, View_Options      ) \
    x(        title, View_Title        ) \
    x(     camptech, View_CampTech     ) \
    x(     worldmap, View_WorldMap     ) \
    // x(       battle, View_Battle       ) \
    // x(battlevictory, View_BattleVictory) \
    // x(    furniture, View_Furniture    ) \
    // x( battledefeat, View_BattleDefeat ) \
    // x(         camp, View_Camp         ) \
    // x( fornications, View_Fornications ) \
    // x(guycustomizer, View_GuyCustomizer) \


typedef enum {
    view_TransitionKind_NONE,
    view_TransitionKind_Title,
    view_TransitionKind_Options,
    view_TransitionKind_CampTech,
    view_TransitionKind_StartRun,
    view_TransitionKind_StartCamp,
    view_TransitionKind_StartPocketCamp,
    view_TransitionKind_StartBattle,
    view_TransitionKind_BattleVictory,
    view_TransitionKind_BattleDefeat,
    view_TransitionKind_BuyFurniture,
    view_TransitionKind_CampFornications,
    view_TransitionKind_BackToCampFromFornications,
    view_TransitionKind_BackToWorldMap,
    view_TransitionKind_BackToWorldMapFromPocketCamp,
} view_TransitionKind;
typedef struct {
    view_TransitionKind kind;

    uint32_t update;

    struct {
        /* how far this node is from the start */
        size_t steps_from_root;
    } battle;

    struct {
        guy_Guy *captured[countof(save.run.guys)];
        float food, coin;
    } battle_victory;

    struct {
        guy_Guy *in_orgy[countof(save.run.guys)];
    } fornications;

} view_Transition;

typedef enum {
    View_NONE,
#define x(_, view_enum) view_enum,
views
#undef x
    View_COUNT,
} View;

#define x(view_name, _) \
    void view_##view_name##_init  (view_Transition); \
    void view_##view_name##_free  (void); \
    view_Transition view_##view_name##_update(uint64_t t); \
    void view_##view_name##_render(void); \
    void view_##view_name##_input(sapp_event *ev);
views
#undef x

#ifdef VIEW_HANDLERS
typedef struct {
    void (*init)(view_Transition);
    void (*free)(void);
    view_Transition (*update)(uint64_t t);
    void (*render)(void);
    void (*input)(sapp_event *ev);
} ViewHandler;

static ViewHandler view_handlers[View_COUNT] = {
#define x(view_name, view_enum) \
    [view_enum] = { \
        .init   = view_##view_name##_init  , \
        .free   = view_##view_name##_free  , \
        .update = view_##view_name##_update, \
        .render = view_##view_name##_render, \
        .input  = view_##view_name##_input , \
    },
views
#undef x
};
#endif// VIEW_HANDLERS

#undef views
#endif
