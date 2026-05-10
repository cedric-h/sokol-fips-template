#include "base.h"
#include "save.h"
#include "guy.h"
#include "ease.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>

guy_GeneConfig guy_gene_configs[] = {

    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Human, .sex = guy_Sex_Any, .amount = 1.0 },
    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Human, .sex = guy_Sex_Any, .amount = 1.0 },
    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Human, .sex = guy_Sex_Any, .amount = 1.1 },
    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Human, .sex = guy_Sex_Any, .amount = 1.2 },

    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .amount = 3.0 },
    { .category = guy_GeneCategory_Fecundity, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .amount = 2.5 },

    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Human, .sex = guy_Sex_Male  , .amount = 1.1 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1.0 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1.0 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Human, .sex = guy_Sex_Female, .amount = 0.9 },
    
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Bunny, .sex = guy_Sex_Male  , .amount = 1.55 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 1.50 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 1.50 },
    { .category = guy_GeneCategory_Metabolism, .race = guy_Race_Bunny, .sex = guy_Sex_Female, .amount = 1.45 },

    { .category = guy_GeneCategory_Strength, .race = guy_Race_Human, .sex = guy_Sex_Male  , .amount = 1.1 },
    { .category = guy_GeneCategory_Strength, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1.0 },
    { .category = guy_GeneCategory_Strength, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1.0 },
    { .category = guy_GeneCategory_Strength, .race = guy_Race_Human, .sex = guy_Sex_Female, .amount = 0.9 },
    
    { .category = guy_GeneCategory_Strength, .race = guy_Race_Bunny, .sex = guy_Sex_Male  , .amount = 0.35 },
    { .category = guy_GeneCategory_Strength, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.20 },

    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Male  , .amount = 1+0.150 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Male  , .amount = 1+0.075 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Male  , .amount = 1+0.030 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1+0.022 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1+0.015 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1-0.015 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1-0.022 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Any   , .amount = 1-0.075 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Female, .amount = 1-0.150 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Female, .amount = 1-0.187 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Human, .sex = guy_Sex_Female, .amount = 1-0.225 },

    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Male  , .amount = 0.5+0.150 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Male  , .amount = 0.5+0.075 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Male  , .amount = 0.5+0.030 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.5+0.022 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.5+0.015 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.5-0.015 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.5-0.022 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Any   , .amount = 0.5-0.075 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Female, .amount = 0.5-0.150 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Female, .amount = 0.5-0.187 },
    { .category = guy_GeneCategory_Girth, .race = guy_Race_Bunny, .sex = guy_Sex_Female, .amount = 0.5-0.225 },


    /* humans unfortunately lack tails */
    { .category = guy_GeneCategory_TailAsset,  .race = guy_Race_Human, .sex = guy_Sex_Any },
    { .category = guy_GeneCategory_LimbsAsset, .race = guy_Race_Human, .sex = guy_Sex_Any },
    { .category = guy_GeneCategory_BowAsset, .race = guy_Race_Human, .sex = guy_Sex_Any },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanFrame1   },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_ToolFork      },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_ToolStapler   },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_ToolNewspaper },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_ToolPan },
    { .category = guy_GeneCategory_MouthAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanMouth1 },
    { .category = guy_GeneCategory_MouthAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanMouth2 },
    { .category = guy_GeneCategory_MouthAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanMouth3 },
    { .category = guy_GeneCategory_MouthAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanMouth4 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanEyes1 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanEyes2 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanEyes3 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Human, .sex = guy_Sex_Any, .asset = guy_Asset_HumanEyes4 },
    { .category = guy_GeneCategory_HairAsset, .race = guy_Race_Human, .sex = guy_Sex_Male, .asset = guy_Asset_HumanHair3Front },
    { .category = guy_GeneCategory_HairAsset, .race = guy_Race_Human, .sex = guy_Sex_Male, .asset = guy_Asset_HumanHair2Front },
    { .category = guy_GeneCategory_HairAsset, .race = guy_Race_Human, .sex = guy_Sex_Female, .asset = guy_Asset_HumanHair1Front },
    { .category = guy_GeneCategory_HairAsset, .race = guy_Race_Human, .sex = guy_Sex_Female, .asset[0] = guy_Asset_HumanHair4Front,
                                                                                             .asset[1] = guy_Asset_HumanHair4Back },

    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 244, 214, 201, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 238, 193, 143, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 248, 213, 162, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 251, 228, 199, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 134,  89,  66, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 72, 37, 18, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 45, 33, 35, 255 } },
    { .category = guy_GeneCategory_SkinColor, .race = guy_Race_Human, .sex = guy_Sex_Any, .color = { 245, 189, 162, 255 } },

    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 255, 220, 114, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 62, 26, 25, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 83, 49, 55, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 95, 24, 14, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 110, 54, 40, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 148, 70, 37, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 217, 153, 90, 255 } },
    { .race = guy_Race_Human, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 237, 209, 177, 255 } },

    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 255, 220, 114, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 62, 26, 25, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 83, 49, 55, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 95, 24, 14, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 110, 54, 40, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 148, 70, 37, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 217, 153, 90, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 237, 209, 177, 255 } },


    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 255, 220, 114, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 62, 26, 25, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 83, 49, 55, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 95, 24, 14, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 110, 54, 40, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 148, 70, 37, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 217, 153, 90, 255 } },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 237, 209, 177, 255 } },

    { .category = guy_GeneCategory_HairAsset , .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyHair1 },
    { .category = guy_GeneCategory_BowAsset  , .race = guy_Race_Bunny, .sex = guy_Sex_Female, .asset = guy_Asset_BunnyBow1Female },
    { .category = guy_GeneCategory_BowAsset  , .race = guy_Race_Bunny, .sex = guy_Sex_Male, .asset = guy_Asset_BunnyBow1Male },
    { .category = guy_GeneCategory_LimbsAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_NONE },
    { .category = guy_GeneCategory_TailAsset , .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyTail1 },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyFrame1 },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_ToolHairbrush },
    { .category = guy_GeneCategory_ToolAsset,  .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_ToolCarrot },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyFrame2 },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyFrame3 },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyFrame4 },
    { .category = guy_GeneCategory_FrameAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyFrame5 },
    { .category = guy_GeneCategory_MouthAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyMouth1 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyEyes1 },
    { .category = guy_GeneCategory_EyesAsset, .race = guy_Race_Bunny, .sex = guy_Sex_Any, .asset = guy_Asset_BunnyEyes2 },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Jo " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Mel " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Pat " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Jay " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Tay " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Sal " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Vic " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Nic " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Ash " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Itt " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Archer " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Kyle " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Tim " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Tom " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jon " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jim " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Josh " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Bill " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Joel " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Greg " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Charles " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Karl " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jeff " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Jack " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Earl " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male, .str = "Guy " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jill " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Kate " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Cat " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Beth " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ella " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Elle " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Elli " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jess " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Tina " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Deb " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Debi " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Trish " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jen " },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Jeni " },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Mc" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "O'" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Brigg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Den" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Hen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ben" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Jen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Quag" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Cart" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Sword" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Shield" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Dun" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Run" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Rune" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Dune" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Hill" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Play" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Shakes" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Send" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Man" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Farm" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Kwik" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ep" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Gold" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "Ein" },

    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "hall" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "ham" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "rourke" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "fawkes" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "things" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "peck" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "stuff" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "croft" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "cross" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "drix" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "son" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "sen" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mann" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "macher" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "meier" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "stein" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "berg" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "hands" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "malley" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mason" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "wright" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "mund" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "smith" },
    { .race = guy_Race_Human, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "kowski" },

    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Ga" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Fin" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Bael" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Ta" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Li" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Li" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Ara" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "El" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "El" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Sil" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Caeh" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ae" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Le" },

    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "la" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "ur" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "ar" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "ra" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "li" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "vi" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "o" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "le" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any, .str = "la" },

    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "norn" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "rond" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "van" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "fin" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "iel" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "zel" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "renn" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "las" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "wen" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "wynne" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "born" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "non" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "driel" },
    { .race = guy_Race_Bunny, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "iel" },

    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 225, 225, 225, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 225, 225, 225, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 225, 225, 225, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 225, 225, 225, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 225, 225, 225, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 205, 205, 205, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any  , .color = { 243, 208,  85, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any  , .color = {  25,  25,  25, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any  , .color = {  55,  55,  55, 245 } },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_TailAsset , .sex = guy_Sex_Any                                    },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any                                    },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_BowAsset  , .sex = guy_Sex_Any                                    },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_HairAsset , .sex = guy_Sex_Any                                    },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_RaccoonFrame1 },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_RaccoonMouth1 },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_EyesAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_RaccoonEyes1 },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_ToolHairbrush },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Any   , .amount = 1.2f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount = 2.1f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount = 1.9f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount = 1.8f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Any   , .amount = 1.2f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Male  , .amount = 0.9f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Any   , .amount = 0.7f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Female, .amount = 0.5f },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Stolen" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Golden" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Black" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Dark" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Shrouded" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Hoarded" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Sinking" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Sweeping" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Secret" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Sudden" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Hidden" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Goth" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Masked" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Sharp" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Poison" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameBase, .sex = guy_Sex_Any   , .str = " " },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Dagger" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Strike" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Blow" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Knock" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Kick" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Crawl" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Sneak" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Bear" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Eyes" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Ambush" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Valor" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Mask" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Horde" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Drop" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Paw" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Whisper" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Murmur" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Ninja" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Fighter" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Warrior" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Secret" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Fang" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Fangs" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Teeth" },
    { .race = guy_Race_Raccoon, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Thief" },

    { .race = guy_Race_Bat, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 141,  114,  91, 255 } },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = {  98,   72,  86, 255 } },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 100,  100, 115, 255 } },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any   , .color = { 141,  114,  91, 255 } },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_HairAsset , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_TailAsset , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_BowAsset  , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_BatFrame1  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_BatMouth1  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_EyesAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_BatEyes1   },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_ToolFlail },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Any   , .amount =  1.5f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Any   , .amount =  1.5f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount =  2.0f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Any   , .amount =  2.0f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Any   , .amount =  0.7f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Any   , .amount =  0.5f                  },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Blut" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Blood" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Wrac" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Drac" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Dac" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Rac" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Sac" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ul" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "oo" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ah" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ahl" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ith" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "fang" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "a" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "lis" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "liss" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "sseth" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "fang" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "ar" },
    { .race = guy_Race_Bat, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "aru" },

    { .race = guy_Race_Spider, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 141,  114,  91, 255 } },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = {  98,   72,  86, 255 } },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 100,  100, 115, 255 } },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any   , .color = { 141,  114,  91, 255 } },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_HairAsset , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_TailAsset , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_BowAsset  , .sex = guy_Sex_Any   ,                                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_SpiderFrame1  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_SpiderMouth1  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_EyesAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_SpiderEyes1   },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_ToolBone      },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Any   , .amount =  0.5f                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount =  2.0f                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Any   , .amount = 10.0f                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Any   , .amount =  0.1f                  },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Mommy " },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Daddy " },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Long" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Short" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "No" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Some" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Many" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Black" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Grey" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "White" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Legs" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Arms" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Toes" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Hair" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "Nose" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Female, .str = "Widow" },
    { .race = guy_Race_Spider, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Male  , .str = "Widower" },

    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = {  87,  82,  51, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 184, 224, 115, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 172, 145,  64, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 167, 153, 126, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 108, 109,  62, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 125, 108,  74, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = {  75, 109, 123, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = { 228, 129,  63, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any   , .color = { 204,  54,  34, 255 } },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_HairAsset , .sex = guy_Sex_Any   ,                                 },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_TailAsset , .sex = guy_Sex_Any   ,                                 },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_FrogLimbs1   },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_BowAsset  , .sex = guy_Sex_Any   ,                               },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_FrogFrame1   },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any   , .asset = guy_Asset_FrogMouth1   },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_EyesAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_FrogEyes1    },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_ToolSwamp    },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any   , .asset = guy_Asset_ToolDagger   },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Any   , .amount = 1.0f                  },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any   , .amount = 1.0f                  },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Any   , .amount = 2.0f                  },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Any   , .amount = 0.8f                  },

    { .race = guy_Race_Frog, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Rib-" },
    { .race = guy_Race_Frog, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "Rr-" },
#define FROG_NAME_PART(part, spacer, __sex) \
    { .race = guy_Race_Frog, .category = guy_GeneCategory_NamePrefix, .sex = (__sex), .str = (part spacer) }, \
    { .race = guy_Race_Frog, .category = guy_GeneCategory_NameBase  , .sex = (__sex), .str = (part spacer) }, \
    { .race = guy_Race_Frog, .category = guy_GeneCategory_NameSuffix, .sex = (__sex), .str = (part       ) },
    FROG_NAME_PART("Ribbit",  "-", guy_Sex_Any)
    FROG_NAME_PART("Ribbit",  "-", guy_Sex_Any)
    FROG_NAME_PART("Croak",   "-", guy_Sex_Any)
    FROG_NAME_PART("Croak",   "-", guy_Sex_Any)
    FROG_NAME_PART("Gurgle",  "-", guy_Sex_Any)
    FROG_NAME_PART("Swamp",   "-", guy_Sex_Any)
    FROG_NAME_PART("Hop",     "-", guy_Sex_Any)
    FROG_NAME_PART("Jump",    "-", guy_Sex_Any)
    FROG_NAME_PART("Spring",  "-", guy_Sex_Any)
    FROG_NAME_PART("Boing",   "-", guy_Sex_Any)
    FROG_NAME_PART("Boing",   "-", guy_Sex_Any)
    FROG_NAME_PART("Boing",   "-", guy_Sex_Any)
    FROG_NAME_PART("Sproing", "-", guy_Sex_Any)
    FROG_NAME_PART("Frog",    " ", guy_Sex_Any)
    FROG_NAME_PART("Toad",    " ", guy_Sex_Any)
    FROG_NAME_PART("Leg",     "-", guy_Sex_Any)
    FROG_NAME_PART("Lily",    "-", guy_Sex_Female)

    { .race = guy_Race_Birb, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Male  , .color = {  88, 120, 212, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Male  , .color = { 128, 140, 212, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Female, .color = { 190,  41,  51, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Female, .color = { 190,  91, 101, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Male  , .color = {  88, 120, 212, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Male  , .color = { 128, 140, 212, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Female, .color = { 190,  41,  51, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Female, .color = { 190,  91, 101, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairColor , .sex = guy_Sex_Any   , .color = {  90, 191, 101, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_SkinColor , .sex = guy_Sex_Any   , .color = {  90, 191, 101, 255} },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_TailAsset , .sex = guy_Sex_Any }, /* attached to frame */
    { .race = guy_Race_Birb, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any }, /* N/A */
    { .race = guy_Race_Birb, .category = guy_GeneCategory_BowAsset,   .sex = guy_Sex_Any }, /* N/A */
    { .race = guy_Race_Birb, .category = guy_GeneCategory_HairAsset , .sex = guy_Sex_Any }, /* attached to frame */
    { .race = guy_Race_Birb, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any, .asset = guy_Asset_BirbFrame1 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any, .asset = guy_Asset_BirbMouth1 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_EyesAsset , .sex = guy_Sex_Any, .asset = guy_Asset_BirbEyes1 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any, .asset = guy_Asset_ToolSpear },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_ToolAsset , .sex = guy_Sex_Any, .asset = guy_Asset_ToolWand },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Male  , .amount = 0.8 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Male  , .amount = 0.7 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Female, .amount = 1.2 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Strength  , .sex = guy_Sex_Female, .amount = 1.3 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Male  , .amount = 0.8 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Male  , .amount = 0.7 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Female, .amount = 1.2 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Girth     , .sex = guy_Sex_Female, .amount = 1.3 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Male  , .amount = 0.8 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Male  , .amount = 0.7 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Female, .amount = 1.2 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Female, .amount = 1.3 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Male  , .amount = 9.0 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Male  , .amount = 9.0 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Female, .amount = 1.5 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_Fecundity , .sex = guy_Sex_Female, .amount = 1.8 },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Female, .str = "Ms. " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Male  , .str = "Mr. " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Lil " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Big " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Dr. " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Prof. " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any   , .str = "Twee " },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Tweet" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Warbl" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Peck" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Cock" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Hoot" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Beak" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Feath" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Bead" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Wing" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Owl" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Falcon" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Eag" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Hawk" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Tit" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any   , .str = "Bird" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "les" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "ler" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "lers" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "bird" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "er" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "ers" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "ley" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "ey" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "y" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = " 9000" },
    { .race = guy_Race_Birb, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any   , .str = "" },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 139, 141, 150, 255 } },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 139, 141, 150, 255 } },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 108, 129,  71, 255 } },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 108, 129,  71, 255 } },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_SkinColor, .sex = guy_Sex_Any, .color = { 124, 101,  60, 255 } },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_HairColor, .sex = guy_Sex_Any, .color = { 124, 101,  60, 255 } },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_TailAsset, .sex = guy_Sex_Any },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_HairAsset, .sex = guy_Sex_Any },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_LimbsAsset, .sex = guy_Sex_Any },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_BowAsset,   .sex = guy_Sex_Any, .asset = guy_Asset_MoaiBow1 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_BowAsset,   .sex = guy_Sex_Any, .asset = guy_Asset_MoaiBow2 },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiFrame1 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiFrame2 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_FrameAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiFrame3 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_ToolAsset,  .sex = guy_Sex_Any, .asset = guy_Asset_ToolCactus    },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_ToolAsset,  .sex = guy_Sex_Any, .asset = guy_Asset_ToolLasso     },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_ToolAsset,  .sex = guy_Sex_Any, .asset = guy_Asset_ToolSlingshot },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_ToolAsset,  .sex = guy_Sex_Any, .asset = guy_Asset_ToolClub      },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiMouth1 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiMouth2 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_MouthAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiMouth3 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_EyesAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiEyes1 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_EyesAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiEyes2 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_EyesAsset, .sex = guy_Sex_Any, .asset = guy_Asset_MoaiEyes3 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Strength, .sex = guy_Sex_Any, .amount = 1.4 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any, .amount = 0.8 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Metabolism, .sex = guy_Sex_Any, .amount = 0.9 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Fecundity, .sex = guy_Sex_Any, .amount = 0.9 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Fecundity, .sex = guy_Sex_Any, .amount = 0.7 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Girth, .sex = guy_Sex_Any, .amount = 1.4 },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_Girth, .sex = guy_Sex_Any, .amount = 1.5 },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "GR" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "SCH" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NamePrefix, .sex = guy_Sex_Any, .str = "K" },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "URB" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "IRM" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ORB" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ORP" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "URM" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "OOB" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameBase  , .sex = guy_Sex_Any, .str = "ORM" },

    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "LES" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "LIN" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "NAK" },
    { .race = guy_Race_Moai, .category = guy_GeneCategory_NameSuffix, .sex = guy_Sex_Any, .str = "URKK" },


};

/* raw meta stats */
float guy_girth(guy_Guy *guy) {
    float x = 0;

    size_t n = 0;
    for (size_t i = guy_GeneLoc_Girth1; i <= guy_GeneLoc_GirthLast; i++)
        n++, x += guy->genes[i]->amount;
    return x/(float)n * (guy->crowned ? 2 : 1);
}
float guy_strength(guy_Guy *guy) {
    float x = 0;

    size_t n = 0;
    for (size_t i = guy_GeneLoc_Strength1; i <= guy_GeneLoc_StrengthLast; i++)
        n++, x += guy->genes[i]->amount;
    return x/(float)n * (guy->crowned ? 2 : 1);
}
float guy_metabolism(guy_Guy *guy) {
    return guy->genes[guy_GeneLoc_Metabolism]->amount * (guy->crowned ? 2 : 1);
}
float guy_fecundity(guy_Guy *guy) {
    return guy->genes[guy_GeneLoc_Fecundity]->amount * (guy->crowned ? 2 : 1);
}

/* girth: +size, +max hp, -knockback received, -movement speed, +hunger, +meat */
/* strength: +damage, +knockback dealt, +movement speed */
/* metabolism: +movement speed, +hunger, +attack speed, +heals, -longevity */
uint16_t guy_maxhp(guy_Guy *g) {
    return roundf(guy_girth(g) * 100.0f);
}
float guy_speed(guy_Guy *g) {
    return 2*(guy_metabolism(g) + guy_strength(g)*0.2 - guy_girth(g)*0.2);
}
uint32_t guy_initiative(guy_Guy *g) {
    float speed = 100.0f / guy_metabolism(g);
    return roundf(gaussian_randf(speed, speed*0.2));
}
float guy_size(guy_Guy *g) {
    return guy_girth(g);
}
float guy_damage(guy_Guy *g) {
    return guy_strength(g)*15;
}
float guy_meat(guy_Guy *g) {
    return guy_girth(g);
}
float guy_hunger(guy_Guy *g) {
    return guy_metabolism(g) + guy_girth(g)*0.2;
}
uint32_t guy_childcount(guy_Guy *g) {
    float f = guy_fecundity(g);
    float o = roundf(gaussian_randf(f, f*0.32f));
    return max(1, o);
}

Color guy_color_skin(guy_Guy *guy) {
    return color_lerp(
        color_lerp(
            guy->genes[guy_GeneLoc_SkinColor1]->color,
            guy->genes[guy_GeneLoc_SkinColor2]->color,
            0.5
        ),
        color_lerp(
            guy->genes[guy_GeneLoc_SkinColor2]->color,
            guy->genes[guy_GeneLoc_SkinColor3]->color,
            0.5
        ),
        0.5
    );
}
Color guy_color_hair(guy_Guy *guy) {
    return color_lerp(
        color_lerp(
            guy->genes[guy_GeneLoc_HairColor1]->color,
            guy->genes[guy_GeneLoc_HairColor2]->color,
            0.5
        ),
        color_lerp(
            guy->genes[guy_GeneLoc_HairColor2]->color,
            guy->genes[guy_GeneLoc_HairColor3]->color,
            0.5
        ),
        0.5
    );
}

guy_Guy *guy_alloc(void) {
    for (size_t i = 0; i < countof(save.run.guys); i++) {
        if (save.run.guys[i].state == guy_GuyState_NONE)
            return save.run.guys + i;
    }
    return NULL;
}

static size_t guy_count_applicable_genes(guy_Sex sex, guy_Race race, guy_GeneLoc loc) {
    size_t applicable_count = 0;

    for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
        guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

        if ((cfg->sex & sex) &&
            (!race || (cfg->race == race)) &&
            (cfg->category == guy_gene_loc_categories[loc])
        ) {
            applicable_count += 1;
        }
    }

    return applicable_count;
}

guy_Guy guy_from_chaos(guy_Sex sex) {
    guy_Guy guy = {
        .sex = sex,
        .state = guy_GuyState_Inited,
    };

    for (guy_GeneLoc loc = 0; loc < guy_GeneLoc_COUNT; loc++) {

        /* find applicable gene for this loc and sex/race */
        float applicable_count = guy_count_applicable_genes(sex, guy_Race_NONE, loc);

        size_t applicable_idx = rand_int(applicable_count);
        guy_GeneConfig *applicable = NULL;
        for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
            guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

            if ((cfg->sex & sex) &&
                (cfg->category == guy_gene_loc_categories[loc])
            ) {
                if (applicable_idx == 0) {
                    applicable = cfg;
                    break;
                }
                applicable_idx -= 1;
            }
        }

        if (applicable) {
            guy.genes[loc] = applicable;
        }
    }

    guy.hp = guy_maxhp(&guy);
    return guy;
}

guy_Guy guy_from_race(guy_Race race, guy_Sex sex) {
    guy_Guy guy = {
        .sex = sex,
        .state = guy_GuyState_Inited,
    };

    for (guy_GeneLoc loc = 0; loc < guy_GeneLoc_COUNT; loc++) {

        /* find applicable gene for this loc and sex/race */
        float applicable_count = guy_count_applicable_genes(sex, race, loc);

        size_t applicable_idx = rand_int(applicable_count);
        guy_GeneConfig *applicable = NULL;
        for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
            guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

            if ((cfg->sex & sex) &&
                (cfg->race == race) &&
                (cfg->category == guy_gene_loc_categories[loc])
            ) {
                if (applicable_idx == 0) {
                    applicable = cfg;
                    break;
                }
                applicable_idx -= 1;
            }
        }

        if (applicable) {
            guy.genes[loc] = applicable;
        }
    }

    guy.hp = guy_maxhp(&guy);
    return guy;
}

static struct {
    tex_Tex assets[guy_Asset_COUNT];
} guy_system;

void guy_system_init() {

    /* throw error on missing gene */
    for (int i = 0; i < 2; i++) {
        guy_Sex sex = i ? guy_Sex_Male : guy_Sex_Female;
        char *sex_name = i ? "Male" : "Female";

        for (guy_Race race = 1; race < guy_Race_COUNT; race++) {
            for (guy_GeneLoc loc = 0; loc < guy_GeneLoc_COUNT; loc++) {
                if (guy_count_applicable_genes(sex, race, loc) == 0) {
                    printf(
                        "no '%s' genes where race='%s' & sex='%s'!\n",
                        guy_gene_loc_names[loc],
                        guy_race_names[race],
                        sex_name
                    );
                    raise(SIGTRAP);
                }
            }
        }
    }

    /* throw error on unused asset */
    for (guy_Asset asset = guy_Asset_NONE + 1; asset < guy_Asset_COUNT; asset++) {
        bool asset_used = false;

        for (size_t cfg_i = 0; cfg_i < countof(guy_gene_configs); cfg_i++) {
            guy_GeneConfig *cfg = guy_gene_configs + cfg_i;

            bool asset_gene = false;
            for (size_t i = 0; i < countof(guy_asset_gene_categories); i++) {
                if (cfg->category == guy_asset_gene_categories[i]) {
                    asset_gene = true;
                    break;
                }
            }

            if (!asset_gene) continue;

            if (cfg->asset[0] == asset || cfg->asset[1] == asset) {
                asset_used = true;
                break;
            }
        }

        if (!asset_used) {
            printf("asset '%s' never used!\n", guy_asset_paths[asset]);
            raise(SIGTRAP);
        }
    }

    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        guy_system.assets[i] = tex_init(guy_asset_paths[i]);
}
void guy_system_free() {
    for (int i = guy_Asset_NONE+1; i < guy_Asset_COUNT; i++)
        tex_free(guy_system.assets[i]);
}

char *guy_sex_str(guy_Sex sex) {
    switch (sex) {
        case guy_Sex_Male:   return "male";   break;
        case guy_Sex_Female: return "female"; break;
        case guy_Sex_Any:    return "any";    break;
        default:             return "???";    break;
    }
}

void guy_name(guy_Guy *guy_guy, char name[GUY_NAME_LEN_MAX]) {
    snprintf(
        name,
        GUY_NAME_LEN_MAX,
        "%s%s%s",
        guy_guy->genes[guy_GeneLoc_NamePrefix]->str,
        guy_guy->genes[guy_GeneLoc_NameBase]->str,
        guy_guy->genes[guy_GeneLoc_NameSuffix]->str
    );
}

typedef struct {
    Color hair, mouth, skin, eyes;
    float size;
    f2 pos;
} guy_DrawCtx;

static void guy_draw_layer(guy_DrawCtx *ctx, Color c, tex_Tex t) {
    /* features are oriented on a canvas x3 larger than the guy so that
     * all of the positioning information can be authored alongside the
     * object inside art tools, and rendering them is simply compositing
     * layers.
     *
     * layers which are larger or smaller than the expected 600.0f are
     * scaled up or down proportionally, centered around the center of the 3x3 */
    // float size = 3*ctx->size * (t.width / 600.0f);
    // RL_DrawTexturePro(
    //     t,
    //     (RL_Rectangle) { 0, 0, t.width, t.height },
    //     (RL_Rectangle) {
    //         ctx->pos.x - size/2,
    //         ctx->pos.y - size/2,
    //         size,
    //         size
    //     },
    //     (RL_Vector2) { 0, 0 },
    //     0,
    //     (RL_Color) { c.r, c.g, c.b, c.a }
    // );
}

void guy_draw(guy_Guy *guy, float x, float y, guy_DrawFlags flags) {
    guy_draw_ex((guy_DrawEx) {
        .guy = guy,
        .pos = (f2) { x, y },
        .target = (f2) { x, y },
        .swing_t = 0,
        .hurt_t = 0,
        .flags = flags
    });
}

void guy_draw_ex(guy_DrawEx ex) {

    typedef enum {
        guy_Layer_Tail,
        guy_Layer_HairBack,
        guy_Layer_Frame,
        guy_Layer_Mouth,
        guy_Layer_Eyes,
        guy_Layer_HairFront,
        guy_Layer_Limbs,
        guy_Layer_Bow,
        guy_Layer_COUNT,
    } guy_Layer;

    tex_Tex tool = guy_system.assets[ex.guy->genes[guy_GeneLoc_ToolAsset]->asset[0]];
    guy_Asset layer_assets[] = {
        [guy_Layer_Tail     ] = ex.guy->genes[guy_GeneLoc_TailAsset ]->asset[0],
        [guy_Layer_HairBack ] = ex.guy->genes[guy_GeneLoc_HairAsset ]->asset[1],
        [guy_Layer_Frame    ] = ex.guy->genes[guy_GeneLoc_FrameAsset]->asset[0],
        [guy_Layer_Mouth    ] = ex.guy->genes[guy_GeneLoc_MouthAsset]->asset[0],
        [guy_Layer_Eyes     ] = ex.guy->genes[guy_GeneLoc_EyesAsset ]->asset[0],
        [guy_Layer_HairFront] = ex.guy->genes[guy_GeneLoc_HairAsset ]->asset[0],
        [guy_Layer_Limbs    ] = ex.guy->genes[guy_GeneLoc_LimbsAsset]->asset[0],
        [guy_Layer_Bow      ] = ex.guy->genes[guy_GeneLoc_BowAsset  ]->asset[0],
    };

    _Static_assert(countof(layer_assets) == guy_Layer_COUNT, "missing asset for guy layer");

    if (ex.size == 0) {
        ex.size = 30*guy_size(ex.guy);
    } else {
        /* we need to account for how some layers may exceed 600px,
         * so everything needs to be shrunk a bit to keep it in bounds */
        // float max_size = 1.0f;
        // for (guy_Layer i = 0; i < guy_Layer_COUNT; i++) {
        //     float w = guy_system.assets[layer_assets[i]].width;
        //     max_size = max(max_size, w / 600.0f);
        // }
        // ex.size /= max_size;
    }

    guy_DrawCtx ctx = {
        .size  = ex.size,
        .skin  = guy_color_skin(ex.guy),
        .hair  = guy_color_hair(ex.guy),
        .eyes  = (Color) { 255, 255, 255, 255 },
        .mouth = (Color) { 255, 255, 255, 255 },
        .pos   = ex.pos,
    };

    /* hurt go red */
    if (ex.hurt_t > 0 && (base_play_duration() - ex.hurt_t) < 0.4) {
        double t = ease_out_circ(
            (base_play_duration() - ex.hurt_t) / 0.4
        );
        ctx.skin.r = lerp(min(255, ctx.skin.r + 32), ctx.skin.r, t);
    }

    guy_draw_layer(&ctx, ctx. hair, guy_system.assets[layer_assets[guy_Layer_Tail     ]]);
    guy_draw_layer(&ctx, ctx. hair, guy_system.assets[layer_assets[guy_Layer_HairBack ]]);
    guy_draw_layer(&ctx, ctx. skin, guy_system.assets[layer_assets[guy_Layer_Frame    ]]);
    guy_draw_layer(&ctx, ctx.mouth, guy_system.assets[layer_assets[guy_Layer_Mouth    ]]);
    guy_draw_layer(&ctx, ctx. eyes, guy_system.assets[layer_assets[guy_Layer_Eyes     ]]);
    guy_draw_layer(&ctx, ctx. hair, guy_system.assets[layer_assets[guy_Layer_HairFront]]);
    guy_draw_layer(&ctx, ctx. skin, guy_system.assets[layer_assets[guy_Layer_Limbs    ]]);
    guy_draw_layer(&ctx, ctx. eyes, guy_system.assets[layer_assets[guy_Layer_Bow      ]]);

    // {
    //     float sword_size = ctx.size * 1.2 * sqrtf(sqrtf(guy_strength(ex.guy)));

    //     /* from the origin to the pommel */
    //     float pommel_x = sword_size*0.2;
    //     float pommel_y = sword_size*0.6;

    //     float sword_x = ex.pos.x + ctx.size*0.7f + sword_size*0.2f + pommel_x;
    //     float sword_y = ex.pos.y + pommel_y;
    //     size_t i = (size_t)(void *)ex.guy%69;

    //     float rot = sinf(GOLDEN_RATIO*i + RL_GetTime()*2) * 5;
    //     do {
    //         if (!(ex.flags & guy_DrawFlags_Target))
    //             continue;

    //         float anim_speed = 1.2f;
    //         float dx = ex.pos.x - ex.target.x;
    //         float dy = ex.pos.y - ex.target.y;
    //         if ((fabsf(dx) + fabsf(dy)) == 0)
    //             continue;

    //         rot = rot / 180.0f * M_PI;
    //         rot += atan2f(dy, dx);
    //         rot -= M_PI*0.75;
    //         if (ex.swing_t != 0) {
    //             double t = (RL_GetTime() - ex.swing_t) * anim_speed * 2;

    //             float rot_start = rot;
    //             float rad_overshoot     = rot - M_PI*0.50;
    //             float rad_from          = rot - M_PI*0.40;
    //             float rad_to            = rot + M_PI*0.40;
    //             float rad_followthrough = rot + M_PI*0.55;

    //             double prepare_t = t / 0.1;
    //             if (prepare_t > 0 && prepare_t < 1) {
    //                 rot = lerp_rads(rot_start, rad_from, ease_in_back(prepare_t));
    //             }
    //             t -= 0.1;

    //             double hold_t = t / 0.3;
    //             if (hold_t > 0 && hold_t < 1) {
    //                 rot = lerp_rads(
    //                     rad_from,
    //                     rad_overshoot,
    //                     hold_t
    //                 );
    //             }
    //             t -= 0.3;

    //             double attack_t = t / 0.35;
    //             if (attack_t > 0 && attack_t < 1) {
    //                 rot = lerp_rads(rad_overshoot, rad_to, ease_in_back(attack_t));
    //             }
    //             t -= 0.35;

    //             double followthrough_t = t / 0.15;
    //             if (followthrough_t > 0 && followthrough_t < 1) {
    //                 rot = lerp_rads(
    //                     rad_to,
    //                     rad_followthrough,
    //                     followthrough_t
    //                 );
    //             }
    //             t -= 0.15;

    //             double return_t = t / 0.3;
    //             if (return_t > 0 && return_t < 1) {
    //                 rot = lerp_rads(
    //                     rad_followthrough,
    //                     rot_start,
    //                     ease_in_back(return_t)
    //                 );
    //             }
    //             t -= 0.3;
    //         }

    //         if (ex.swing_t != 0) {
    //             float dl = sqrtf(dx*dx + dy*dy);

    //             /* push sword_x towards rot */
    //             double t = (RL_GetTime() - ex.swing_t)*anim_speed*0.8;
    //             float fwd_travel = 35;
    //             float back_travel = -8;

    //             double back_t = t / 0.1;
    //             if (back_t < 1) {
    //                 float a = lerpf(0, back_travel, ease_in_back(back_t));
    //                 sword_x -= dx/dl * a;
    //                 sword_y -= dy/dl * a;
    //             }
    //             t -= 0.1;

    //             double stab_t = t / 0.1;
    //             if (stab_t > 0 && stab_t < 1) {
    //                 float a = lerpf(back_travel, fwd_travel, ease_in_back(stab_t));
    //                 sword_x -= dx/dl * a;
    //                 sword_y -= dy/dl * a;
    //             }
    //             t -= 0.1;

    //             double return_t = t / 0.3;
    //             if (return_t > 0 && return_t < 1) {
    //                 sword_x -= dx/dl * lerpf(fwd_travel, 0, return_t);
    //                 sword_y -= dy/dl * lerpf(fwd_travel, 0, return_t);
    //             }
    //             t -= 0.3;
    //         }

    //         rot = (rot / M_PI) * 180;
    //     } while (false);

    //     RL_DrawTexturePro(
    //         tool,
    //         (RL_Rectangle) { 0, 0, tool.width, tool.height },
    //         (RL_Rectangle) {
    //             sword_x - sword_size/2,
    //             sword_y - sword_size/2,
    //             sword_size,
    //             sword_size
    //         },
    //         (RL_Vector2) {
    //             pommel_x,
    //             pommel_y,
    //         },
    //         rot,
    //         (RL_Color) { 255, 255, 255, 255 }
    //     );
    // }

    // if (guy_guy->crowned) {
    //     float hair_size = size*0.5;
    //     RL_Texture t = *ui_icon(ui_Icon_Crown);
    // }

    // if (ex.flags & guy_DrawFlags_Hp && ex.guy->hp != guy_maxhp(ex.guy)) {
    //     float w = 40;

    //     float t = (float)ex.guy->hp / (float)guy_maxhp(ex.guy);
    //     w *= t;

    //     RL_Color good = { 100, 255, 100, 255 };
    //     RL_Color mid  = { 255, 255, 100, 255 };
    //     RL_Color bad  = { 255, 100, 100, 255 };
    //     RL_Color clr = (t > 0.5) ?
    //         ColorLerp(good, mid, inv_lerpf(1.0f, 0.5f, t)) :
    //         ColorLerp( mid, bad, inv_lerpf(0.5f, 0.0f, t));

    //     RL_DrawRectangle(
    //         ex.pos.x - w/2,
    //         ex.pos.y + ex.size*0.8,
    //         w,
    //         5,
    //         (RL_Color) { clr.r, clr.g, clr.b, 155 }
    //     );
    // }

    // if (ex.flags & guy_DrawFlags_Name) {
    //     ui_Font font = ui_Font_Name;
    //     char name[GUY_NAME_LEN_MAX] = {0};
    //     guy_name(ex.guy, name);
    //     float w = RL_MeasureTextEx(ui_font_rl(font), name, ui_font_size(font), 1).x;
    //     RL_DrawTextEx(
    //         ui_font_rl(font),
    //         name,
    //         (RL_Vector2) { ex.pos.x - w/2, ex.pos.y + ctx.size*0.8 },
    //         ui_font_size(font),
    //         1,
    //         (RL_Color) { 0, 0, 0, 255 }
    //     );
    // }
}

guy_Guy guy_from_parents(guy_Guy *mom, guy_Guy *dad) {
    return guy_from_parents_ex(
        mom,
        dad,
        rand_int(2) ? guy_Sex_Male : guy_Sex_Female
    );
}
guy_Guy guy_from_parents_ex(guy_Guy *mom, guy_Guy *dad, guy_Sex sex) {
    assert(mom->sex == guy_Sex_Female);
    assert(dad->sex == guy_Sex_Male);

    guy_Guy kid = {
        .sex = sex,
        .state = guy_GuyState_Inited,
        .hp = 10,
    };

    for (guy_GeneLoc i = 0; i < guy_GeneLoc_COUNT; i++) {
        guy_GeneConfig *mom_gene = mom->genes[i];
        guy_GeneConfig *dad_gene = dad->genes[i];

        assert(mom_gene != NULL);
        assert(dad_gene != NULL);

        if ((mom_gene->sex & kid.sex) && (dad_gene->sex & kid.sex))
            kid.genes[i] = rand_int(2) ? mom_gene : dad_gene;
        else {
            if (sex == guy_Sex_Male)
                kid.genes[i] = dad_gene;
            else
                kid.genes[i] = mom_gene;
        }
    }

    kid.hp = guy_maxhp(&kid);
    return kid;
}
