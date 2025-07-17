

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
    ANIMTYPE_FALL,
    ANIMTYPE_WALK,
    ANIMTYPE_JUMP,
    ANIMTYPE_DASH,
    ANIMTYPE_BUTTSTOMP,
    ANIMTYPE_KNOCKBACK,
    ANIMTYPE_STUNNED,
    ANIIMTYPE_DIE,
} AnimTypes;

typedef struct
{
    AnimFrames idleFrames;
    AnimFrames fallFrames;
    AnimFrames walkFrames;
    AnimFrames jumpFrames;
    AnimFrames dashFrames;
    AnimFrames buttstompFrames;
    AnimFrames knockbackFrames;
    AnimFrames stunFrames;
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
    &img_player_idle_0,
    &img_player_idle_0,
    &img_player_idle_0,
    &img_player_idle_1,
    &img_player_idle_0,
    &img_player_idle_0,
    &img_player_idle_0,
    &img_player_idle_2,
    &img_player_idle_3,
    &img_player_idle_3,
    &img_player_idle_4,
    &img_player_idle_3,
    &img_player_idle_4,
    &img_player_idle_3,
};

const Img *imgs_player_fall[] = {
    &img_player_fall_0, &img_player_fall_1, &img_player_fall_2, &img_player_fall_1};

const Img *imgs_player_walk[] = {
    &img_player_walk_0,
    &img_player_walk_1,
    &img_player_walk_2,
    &img_player_walk_3,
    &img_player_walk_4,
    &img_player_walk_5,
    &img_player_walk_6,
    &img_player_walk_7,
};

const Img *imgs_player_jump[] = {
    &img_player_jump_0, &img_player_jump_1};

const Img *imgs_player_dash[] = {
    &img_player_dash_0};

const Img *imgs_player_buttstomp[] = {
    &img_player_buttstomp_0};

const Img *imgs_player_knockback[] = {
    &img_player_knockback_0};

const Img *imgs_player_stun[] = {
    &img_player_stun_0};

const Img *imgs_player_die[] = {
    &img_player_die_0};

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
    .fallFrames = {.frameSpeed = 4, .numImages = ARRAYSIZE(imgs_player_fall), .images = imgs_player_fall},
    .walkFrames = {.frameSpeed = 3, .numImages = ARRAYSIZE(imgs_player_walk), .images = imgs_player_walk},
    .jumpFrames = {.frameSpeed = 7, .numImages = ARRAYSIZE(imgs_player_jump), .images = imgs_player_jump},
    .dashFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_dash), .images = imgs_player_dash},
    .buttstompFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_buttstomp), .images = imgs_player_buttstomp},
    .knockbackFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_knockback), .images = imgs_player_knockback},
    .stunFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_stun), .images = imgs_player_stun},
    .dieFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_die), .images = imgs_player_die}};

//__TEST__
/*
Anim Anims_Player = {
    .grp = NULL,
    .activeFrames = NULL,
    .lastFrame = 0,
};
*/
