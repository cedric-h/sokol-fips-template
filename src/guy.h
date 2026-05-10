#ifndef __EAB_GUY_IMPL
#define __EAB_GUY_IMPL

#include <stdbool.h>
#include "base.h"

#define guy_assets \
    x(guy_Asset_NONE, "./resources/guy2/none.png") \
\
    x(guy_Asset_ToolHairbrush, "./resources/tool/hairbrush.png") \
    x(guy_Asset_ToolWand, "./resources/tool/wand.png") \
    x(guy_Asset_ToolSpear, "./resources/tool/spear.png") \
    x(guy_Asset_ToolCarrot, "./resources/tool/carrot.png") \
\
    x(guy_Asset_ToolStapler, "./resources/tool/stapler.png") \
    x(guy_Asset_ToolNewspaper, "./resources/tool/newspaper.png") \
    x(guy_Asset_ToolFork, "./resources/tool/fork.png") \
    x(guy_Asset_ToolPan, "./resources/tool/pan.png") \
\
    x(guy_Asset_ToolCactus, "./resources/tool/cactus.png") \
    x(guy_Asset_ToolLasso, "./resources/tool/lasso.png") \
    x(guy_Asset_ToolSlingshot, "./resources/tool/slingshot.png") \
    x(guy_Asset_ToolClub, "./resources/tool/club.png") \
\
    x(guy_Asset_ToolBone, "./resources/tool/bone.png") \
    x(guy_Asset_ToolFlail, "./resources/tool/flail.png") \
    x(guy_Asset_ToolDagger, "./resources/tool/dagger.png") \
    x(guy_Asset_ToolSwamp, "./resources/tool/swamp.png") \
\
    x(guy_Asset_BatFrame1, "./resources/guy2/bat_frame1.png") \
    x(guy_Asset_BatEyes1, "./resources/guy2/bat_eyes1.png") \
    x(guy_Asset_BatMouth1, "./resources/guy2/bat_mouth1.png") \
\
    x(guy_Asset_SpiderFrame1, "./resources/guy2/spider_frame1.png") \
    x(guy_Asset_SpiderEyes1, "./resources/guy2/spider_eyes1.png") \
    x(guy_Asset_SpiderMouth1, "./resources/guy2/spider_mouth1.png") \
\
    x(guy_Asset_FrogFrame1, "./resources/guy2/frog_frame1.png") \
    x(guy_Asset_FrogEyes1, "./resources/guy2/frog_eyes1.png") \
    x(guy_Asset_FrogMouth1, "./resources/guy2/frog_mouth1.png") \
    x(guy_Asset_FrogLimbs1, "./resources/guy2/frog_limbs1.png") \
\
    x(guy_Asset_MoaiFrame1, "./resources/guy2/moai_frame1.png") \
    x(guy_Asset_MoaiFrame2, "./resources/guy2/moai_frame2.png") \
    x(guy_Asset_MoaiFrame3, "./resources/guy2/moai_frame3.png") \
    x(guy_Asset_MoaiEyes1, "./resources/guy2/moai_eyes1.png") \
    x(guy_Asset_MoaiEyes2, "./resources/guy2/moai_eyes2.png") \
    x(guy_Asset_MoaiEyes3, "./resources/guy2/moai_eyes3.png") \
    x(guy_Asset_MoaiMouth1, "./resources/guy2/moai_mouth1.png") \
    x(guy_Asset_MoaiMouth2, "./resources/guy2/moai_mouth2.png") \
    x(guy_Asset_MoaiMouth3, "./resources/guy2/moai_mouth3.png") \
    x(guy_Asset_MoaiBow1, "./resources/guy2/moai_bow1.png") \
    x(guy_Asset_MoaiBow2, "./resources/guy2/moai_bow2.png") \
\
    x(guy_Asset_BirbFrame1, "./resources/guy2/bird_frame1.png") \
    x(guy_Asset_BirbMouth1, "./resources/guy2/bird_mouth1.png") \
    x(guy_Asset_BirbEyes1, "./resources/guy2/bird_eyes1.png") \
\
    x(guy_Asset_RaccoonFrame1, "./resources/guy2/raccoon_frame1.png") \
    x(guy_Asset_RaccoonMouth1, "./resources/guy2/raccoon_mouth1.png") \
    x(guy_Asset_RaccoonEyes1, "./resources/guy2/raccoon_eyes1.png") \
\
    x(guy_Asset_BunnyFrame1, "./resources/guy2/bunny_frame1.png") \
    x(guy_Asset_BunnyFrame2, "./resources/guy2/bunny_frame2.png") \
    x(guy_Asset_BunnyFrame3, "./resources/guy2/bunny_frame3.png") \
    x(guy_Asset_BunnyFrame4, "./resources/guy2/bunny_frame4.png") \
    x(guy_Asset_BunnyFrame5, "./resources/guy2/bunny_frame5.png") \
    x(guy_Asset_BunnyEyes1, "./resources/guy2/bunny_eyes1.png") \
    x(guy_Asset_BunnyEyes2, "./resources/guy2/bunny_eyes2.png") \
    x(guy_Asset_BunnyMouth1, "./resources/guy2/bunny_mouth1.png") \
    x(guy_Asset_BunnyTail1, "./resources/guy2/bunny_tail1.png") \
    x(guy_Asset_BunnyHair1, "./resources/guy2/bunny_hair1.png") \
    x(guy_Asset_BunnyBow1Male, "./resources/guy2/bunny_bow1_male.png") \
    x(guy_Asset_BunnyBow1Female, "./resources/guy2/bunny_bow1_female.png") \
\
    x(guy_Asset_HumanEyes1,      "./resources/guy2/human_eyes1.png") \
    x(guy_Asset_HumanEyes2,      "./resources/guy2/human_eyes2.png") \
    x(guy_Asset_HumanEyes3,      "./resources/guy2/human_eyes3.png") \
    x(guy_Asset_HumanEyes4,      "./resources/guy2/human_eyes4.png") \
    x(guy_Asset_HumanFrame1,     "./resources/guy2/human_frame1.png") \
    x(guy_Asset_HumanHair1Front, "./resources/guy2/human_hair1_front.png") \
    x(guy_Asset_HumanHair2Front, "./resources/guy2/human_hair2_front.png") \
    x(guy_Asset_HumanHair3Front, "./resources/guy2/human_hair3_front.png") \
    x(guy_Asset_HumanHair4Front, "./resources/guy2/human_hair4_front.png") \
    x(guy_Asset_HumanHair4Back,  "./resources/guy2/human_hair4_back.png") \
    x(guy_Asset_HumanMouth1,    "./resources/guy2/human_mouth1.png") \
    x(guy_Asset_HumanMouth2,    "./resources/guy2/human_mouth2.png") \
    x(guy_Asset_HumanMouth3,    "./resources/guy2/human_mouth3.png") \
    x(guy_Asset_HumanMouth4,    "./resources/guy2/human_mouth4.png") \

typedef enum {
#define x(e, _) e,
guy_assets
#undef x
    guy_Asset_COUNT,
} guy_Asset;

static char *guy_asset_paths[] = {
#define x(e, x) [e] = x,
guy_assets
#undef x
};

typedef enum {
    guy_GeneCategory_NONE,
    guy_GeneCategory_SkinColor,
    guy_GeneCategory_HairColor,

    guy_GeneCategory_TailAsset,
    guy_GeneCategory_HairAsset,
    guy_GeneCategory_FrameAsset,
    guy_GeneCategory_MouthAsset,
    guy_GeneCategory_EyesAsset,
    guy_GeneCategory_LimbsAsset,
    guy_GeneCategory_BowAsset,
    guy_GeneCategory_ToolAsset,

    guy_GeneCategory_Girth,
    guy_GeneCategory_Strength,
    guy_GeneCategory_Metabolism,
    guy_GeneCategory_Fecundity,

    guy_GeneCategory_NamePrefix,
    guy_GeneCategory_NameBase  ,
    guy_GeneCategory_NameSuffix,
} guy_GeneCategory;

#undef guy_assets

#define guy_gene_locs \
    x(guy_GeneLoc_SkinColor1, "skin color 1", guy_GeneCategory_SkinColor) \
    x(guy_GeneLoc_SkinColor2, "skin color 2", guy_GeneCategory_SkinColor) \
    x(guy_GeneLoc_SkinColor3, "skin color 3", guy_GeneCategory_SkinColor) \
    x(guy_GeneLoc_SkinColor4, "skin color 4", guy_GeneCategory_SkinColor) \
    section(guy_GeneLoc_SkinColorLast, guy_GeneLoc_SkinColor4) \
\
    x(guy_GeneLoc_HairColor1, "hair color 1", guy_GeneCategory_HairColor) \
    x(guy_GeneLoc_HairColor2, "hair color 2", guy_GeneCategory_HairColor) \
    x(guy_GeneLoc_HairColor3, "hair color 3", guy_GeneCategory_HairColor) \
    x(guy_GeneLoc_HairColor4, "hair color 4", guy_GeneCategory_HairColor) \
    section(guy_GeneLoc_HairColorLast, guy_GeneLoc_HairColor4) \
\
    x(guy_GeneLoc_TailAsset,  "tail" , guy_GeneCategory_TailAsset ) \
    x(guy_GeneLoc_HairAsset,  "hair" , guy_GeneCategory_HairAsset ) \
    x(guy_GeneLoc_FrameAsset, "frame", guy_GeneCategory_FrameAsset) \
    x(guy_GeneLoc_MouthAsset, "mouth", guy_GeneCategory_MouthAsset) \
    x(guy_GeneLoc_EyesAsset,  "eyes" , guy_GeneCategory_EyesAsset ) \
    x(guy_GeneLoc_LimbsAsset, "limbs", guy_GeneCategory_LimbsAsset) \
    x(guy_GeneLoc_BowAsset,   "bow",   guy_GeneCategory_BowAsset) \
    x(guy_GeneLoc_ToolAsset,  "tool",  guy_GeneCategory_ToolAsset ) \
\
    x(guy_GeneLoc_Strength1, "strength 1", guy_GeneCategory_Strength) \
    x(guy_GeneLoc_Strength2, "strength 2", guy_GeneCategory_Strength) \
    x(guy_GeneLoc_Strength3, "strength 3", guy_GeneCategory_Strength) \
    section(guy_GeneLoc_StrengthLast, guy_GeneLoc_Strength3) \
\
    x(guy_GeneLoc_Metabolism, "metabolism", guy_GeneCategory_Metabolism) \
    x(guy_GeneLoc_Fecundity,  "fecundity" , guy_GeneCategory_Fecundity ) \
\
    x(guy_GeneLoc_Girth1, "girth 1", guy_GeneCategory_Girth) \
    x(guy_GeneLoc_Girth2, "girth 2", guy_GeneCategory_Girth) \
    x(guy_GeneLoc_Girth3, "girth 3", guy_GeneCategory_Girth) \
    x(guy_GeneLoc_Girth4, "girth 4", guy_GeneCategory_Girth) \
    section(guy_GeneLoc_GirthLast, guy_GeneLoc_Girth4) \
\
    x(guy_GeneLoc_NamePrefix, "name prefix", guy_GeneCategory_NamePrefix) \
    x(guy_GeneLoc_NameBase  , "name base"  , guy_GeneCategory_NameBase  ) \
    x(guy_GeneLoc_NameSuffix, "name suffix", guy_GeneCategory_NameSuffix) \

typedef enum {
#define x(loc, name, category) loc,
#define section(a, b) a = b,
guy_gene_locs
#undef x
#undef section
    guy_GeneLoc_COUNT,
} guy_GeneLoc;

static char *guy_gene_loc_names[guy_GeneLoc_COUNT] = {
#define x(loc, name, category) [loc] = name,
#define section(a, b)
guy_gene_locs
#undef x
#undef section
};

static guy_GeneCategory guy_gene_loc_categories[guy_GeneLoc_COUNT] = {
#define x(loc, name, category) [loc] = category,
#define section(a, b)
guy_gene_locs
#undef x
#undef section
};

/* just used to throw errors on unused assets, as of 2026/05 */
static guy_GeneCategory guy_asset_gene_categories[] = {
    guy_GeneCategory_TailAsset,
    guy_GeneCategory_HairAsset,
    guy_GeneCategory_FrameAsset,
    guy_GeneCategory_MouthAsset,
    guy_GeneCategory_EyesAsset,
    guy_GeneCategory_LimbsAsset,
    guy_GeneCategory_BowAsset,
    guy_GeneCategory_ToolAsset,
};

#undef guy_gene_locs


typedef enum {
    guy_Race_NONE,
    guy_Race_Human,
    guy_Race_Bunny,
    guy_Race_Moai,
    guy_Race_Birb,
    guy_Race_Raccoon,
    guy_Race_Frog,
    guy_Race_Spider,
    guy_Race_Bat,
    guy_Race_COUNT,
} guy_Race;

static char *guy_race_names[] = {
    [guy_Race_NONE] = "N/A",
    [guy_Race_Human] = "Human",
    [guy_Race_Bunny] = "Bunny",
    [guy_Race_Moai] = "Doone",
    [guy_Race_Birb] = "Birb",
    [guy_Race_Raccoon] = "Rapoonda",
    [guy_Race_Frog] = "Phrawgh",
    [guy_Race_Spider] = "Crawley",
    [guy_Race_Bat] = "Bat Demon",
};
_Static_assert(
    countof(guy_race_names) == guy_Race_COUNT,
    "missing name for race"
);

typedef enum {
    guy_Sex_Male   = (1 << 0),
    guy_Sex_Female = (1 << 1),
    guy_Sex_Any    = guy_Sex_Male | guy_Sex_Female,
} guy_Sex;

typedef struct {
    guy_GeneCategory category;
    guy_Race race;
    guy_Sex sex;
    union {
        Color color;
        float amount;
        char *str;
        guy_Asset asset[2];
    };
} guy_GeneConfig;

extern guy_GeneConfig guy_gene_configs[];

typedef enum {
    guy_GuyState_NONE,
    guy_GuyState_Inited,
} guy_GuyState;
typedef struct {
    guy_GuyState state;
    guy_Sex sex;
    guy_GeneConfig *genes[guy_GeneLoc_COUNT];
    uint16_t hp;
    bool crowned;
} guy_Guy;

guy_Guy guy_from_race(guy_Race race, guy_Sex sex);
guy_Guy guy_from_chaos(guy_Sex sex);
guy_Guy guy_from_parents(guy_Guy *mom, guy_Guy *dad);
guy_Guy guy_from_parents_ex(guy_Guy *mom, guy_Guy *dad, guy_Sex sex);
guy_Guy *guy_alloc(void);

/* base stats */
float guy_girth(guy_Guy *guy);
float guy_metabolism(guy_Guy *guy);
float guy_fecundity(guy_Guy *guy);
float guy_strength(guy_Guy *guy);

/* derived stats */
uint16_t guy_maxhp(guy_Guy *guy);
float guy_size(guy_Guy *guy);
float guy_speed(guy_Guy *guy);
float guy_damage(guy_Guy *guy);
float guy_meat(guy_Guy *guy); /* r u good eatin? how much meat u got on u?? */
float guy_hunger(guy_Guy *guy);
uint32_t guy_childcount(guy_Guy *guy);
uint32_t guy_initiative(guy_Guy *guy);

Color guy_color_skin(guy_Guy *guy);
Color guy_color_hair(guy_Guy *guy);

void guy_system_init();
void guy_system_free();

#define GUY_NAME_LEN_MAX 30
void guy_name(guy_Guy *guy_guy, char name[GUY_NAME_LEN_MAX]);
char *guy_sex_str(guy_Sex sex);

typedef enum {
    guy_DrawFlags_Name   = (1 << 0),
    guy_DrawFlags_Hp     = (1 << 1),
    guy_DrawFlags_Target = (1 << 2),
} guy_DrawFlags;
void guy_draw(guy_Guy *guy, float x, float y, guy_DrawFlags flags);

typedef struct {
    guy_Guy *guy;
    f2 pos, target;
    float size;
    double swing_t, hurt_t;
    guy_DrawFlags flags;
} guy_DrawEx;

void guy_draw_ex(guy_DrawEx ex);
#endif
