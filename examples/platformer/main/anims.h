

#pragma once

#include <stdio.h>
#include <stdint.h>
#include "images/images.h"

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef enum
{
    ANIMMODE_LOOP,
    ANIMMODE_PINGPONG,
    ANIMMODE_ONESHOT,
} AnimMode;

// unfortunately we gotta do pointer-to-pointer
// so let's sweep that under the rug a bit.
typedef const Img *const *ImageArray;

typedef struct
{
    AnimMode mode;
    int frameSpeed; // 2 = update every 2nd frame
    int numImages;
    ImageArray images;
} AnimFrames;

typedef enum
{
    ANIMTYPE_IDLE,
    ANIMTYPE_WALK,
    ANIMTYPE_JUMP,
    ANIMTYPE_FALL,
    ANIIMTYPE_DIE,
} AnimTypes;

typedef struct
{
    AnimFrames idleFrames;
    AnimFrames walkFrames;
    AnimFrames jumpFrames;
    AnimFrames fallFrames;
    AnimFrames dieFrames;
} AnimGroup;

typedef struct
{
    const AnimGroup *grp;
    const AnimFrames *activeFrames;
    int lastFrame; // last global game frame
} Anim;

// pad it out a bit with some delays
const Img *imgs_player_idle[] = {
    &img_player_idle_0_raw,
    &img_player_idle_0_raw,
    &img_player_idle_0_raw,
    &img_player_idle_1_raw,
    &img_player_idle_0_raw,
    &img_player_idle_0_raw,
    &img_player_idle_0_raw,
    &img_player_idle_2_raw,
    &img_player_idle_3_raw,
    &img_player_idle_3_raw,
    &img_player_idle_4_raw,
    &img_player_idle_3_raw,
    &img_player_idle_4_raw,
    &img_player_idle_3_raw,
};

const Img *imgs_player_walk[] = {
    &img_player_walk_0_raw,
    &img_player_walk_1_raw,
    &img_player_walk_2_raw,
    &img_player_walk_3_raw,
    &img_player_walk_4_raw,
    &img_player_walk_5_raw,
    &img_player_walk_6_raw,
    &img_player_walk_7_raw,
};

const Img *imgs_player_jump[] = {
    &img_player_jump_0_raw, &img_player_jump_1_raw};

const Img *imgs_player_fall[] = {
    &img_player_fall_0_raw, &img_player_fall_1_raw, &img_player_fall_2_raw, &img_player_fall_1_raw};

const Img *imgs_player_die[] = {
    &img_player_die_0_raw};

// e.g.
/*
AnimFrames frames_player_idle = {
    .frameSpeed = 2,
    .numImages = ARRAYSIZE(imgs_player_idle),
    .images = imgs_player_idle
};
*/

// C++ would be nice here
AnimGroup animgroup_player = {

    .idleFrames = {
        .frameSpeed = 20,
        .numImages = ARRAYSIZE(imgs_player_idle),
        .images = imgs_player_idle},
    .walkFrames = {.frameSpeed = 3, .numImages = ARRAYSIZE(imgs_player_walk), .images = imgs_player_walk},
    .jumpFrames = {.frameSpeed = 7, .numImages = ARRAYSIZE(imgs_player_jump), .images = imgs_player_jump},
    .fallFrames = {.frameSpeed = 4, .numImages = ARRAYSIZE(imgs_player_fall), .images = imgs_player_fall},
    .dieFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_die), .images = imgs_player_die}
};

//__TEST__
/*
Anim Anims_Player = {
    .grp = NULL,
    .activeFrames = NULL,
    .lastFrame = 0,
};
*/
