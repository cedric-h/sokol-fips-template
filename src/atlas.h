//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// rTexpacker v5.0 Atlas Descriptor Code exporter v5.0                          //
//                                                                              //
// more info and bugs-report:  github.com/raylibtech/rtools                     //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2019-2025 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define ATLAS_ATLAS_IMAGE_PATH      "atlas.png"
#define ATLAS_ATLAS_SPRITE_COUNT    29

// Atlas sprite properties
typedef struct rtpAtlasSprite {
    const char *nameId;
    const char *tag;
    int originX, originY;
    int positionX, positionY;
    int sourceWidth, sourceHeight;
    int padding;
    bool trimmed;
    int trimRecX, trimRecY, trimRecWidth, trimRecHeight;
    int colliderType;
    int colliderPosX, colliderPosY, colliderSizeX, colliderSizeY;
} rtpAtlasSprite;

// Atlas sprites array
static rtpAtlasSprite rtpDescAtlas[29] = {
    { "back_to_map", "", 0, 0, 0, 0, 175, 175, 0, false, 4, 4, 166, 170, 0, 0, 0, 0, 0 },
    { "back", "", 0, 0, 539, 496, 133, 133, 0, false, 3, 3, 127, 126, 0, 0, 0, 0, 0 },
    { "bed", "", 0, 0, 0, 496, 137, 137, 0, false, 5, 3, 127, 126, 0, 0, 0, 0, 0 },
    { "camp", "", 0, 0, 131, 633, 128, 128, 0, false, 1, 5, 126, 115, 0, 0, 0, 0, 0 },
    { "captured", "", 0, 0, 476, 337, 156, 156, 0, false, 1, 5, 155, 146, 0, 0, 0, 0, 0 },
    { "chest", "", 0, 0, 632, 337, 148, 148, 0, false, 2, 17, 145, 118, 0, 0, 0, 0, 0 },
    { "crown", "", 0, 0, 672, 496, 132, 132, 0, false, 5, 15, 123, 105, 0, 0, 0, 0, 0 },
    { "diamond", "", 0, 0, 259, 633, 126, 126, 0, false, 0, 0, 125, 126, 0, 0, 0, 0, 0 },
    { "dice", "", 0, 0, 175, 0, 175, 175, 0, false, 5, 5, 166, 165, 0, 0, 0, 0, 0 },
    { "fecundity", "", 0, 0, 0, 175, 162, 162, 0, false, 2, 3, 158, 158, 0, 0, 0, 0, 0 },
    { "fire", "", 0, 0, 804, 496, 131, 131, 0, false, 15, 2, 102, 126, 0, 0, 0, 0, 0 },
    { "fleur", "", 0, 0, 805, 175, 160, 160, 0, false, 6, 1, 150, 158, 0, 0, 0, 0, 0 },
    { "food", "", 0, 0, 350, 0, 170, 170, 0, false, 5, 2, 159, 166, 0, 0, 0, 0, 0 },
    { "forward", "", 0, 0, 406, 496, 133, 133, 0, false, 3, 3, 127, 126, 0, 0, 0, 0, 0 },
    { "girth", "", 0, 0, 520, 0, 168, 168, 0, false, 5, 5, 158, 158, 0, 0, 0, 0, 0 },
    { "grave", "", 0, 0, 162, 175, 162, 162, 0, false, 12, 2, 138, 157, 0, 0, 0, 0, 0 },
    { "heal", "", 0, 0, 645, 175, 160, 160, 0, false, 5, 6, 151, 150, 0, 0, 0, 0, 0 },
    { "hole", "", 0, 0, 324, 175, 161, 161, 0, false, 3, 64, 155, 68, 0, 0, 0, 0, 0 },
    { "key", "", 0, 0, 318, 337, 158, 158, 0, false, 0, 0, 158, 158, 0, 0, 0, 0, 0 },
    { "market", "", 0, 0, 485, 175, 160, 160, 0, false, 5, 1, 149, 158, 0, 0, 0, 0, 0 },
    { "plant", "", 0, 0, 137, 496, 135, 135, 0, false, 12, 3, 113, 126, 0, 0, 0, 0, 0 },
    { "scroll", "", 0, 0, 780, 337, 143, 143, 0, false, 8, 8, 127, 126, 0, 0, 0, 0, 0 },
    { "shovel", "", 0, 0, 385, 633, 120, 120, 0, false, 0, 2, 115, 116, 0, 0, 0, 0, 0 },
    { "soup", "", 0, 0, 852, 0, 163, 163, 0, false, 11, 2, 148, 158, 0, 0, 0, 0, 0 },
    { "speed", "", 0, 0, 0, 337, 159, 159, 0, false, 0, 7, 157, 148, 0, 0, 0, 0, 0 },
    { "strength", "", 0, 0, 688, 0, 164, 164, 0, false, 11, 3, 146, 156, 0, 0, 0, 0, 0 },
    { "swords", "", 0, 0, 0, 633, 131, 131, 0, false, 5, 6, 121, 120, 0, 0, 0, 0, 0 },
    { "telescope", "", 0, 0, 159, 337, 159, 159, 0, false, 1, 1, 156, 155, 0, 0, 0, 0, 0 },
    { "wrench", "", 0, 0, 272, 496, 134, 134, 0, false, 4, 4, 127, 126, 0, 0, 0, 0, 0 },
};
