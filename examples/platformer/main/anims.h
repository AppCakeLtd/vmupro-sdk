

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
    ANIMTYPE_BUTTBOUNCE,
    ANIMTYPE_KNOCKBACK,
    ANIMTYPE_STUNNED,
    ANIIMTYPE_DIE,
} AnimTypes;

const char *AnimTypeToString(AnimTypes inMode)
{

    switch (inMode)
    {
    default:
        vmupro_log(VMUPRO_LOG_ERROR, "anims", "UNNAMED ANIM TYPE: %d\n", (int)inMode);
        return "UNNAMED ANIM TYPE";
        break;

    case ANIMTYPE_IDLE:
        return "IDLE";
        break;
    case ANIMTYPE_FALL:
        return "FALL";
        break;
    case ANIMTYPE_WALK:
        return "WALK";
        break;
    case ANIMTYPE_JUMP:
        return "JUMP";
        break;
    case ANIMTYPE_DASH:
        return "DASH";
        break;
    case ANIMTYPE_BUTTSTOMP:
        return "BUTTSTOMP";
        break;
    case ANIMTYPE_BUTTBOUNCE:
        return "BUTTBOUNCE";
        break;
    case ANIMTYPE_KNOCKBACK:
        return "KNOCKBACK";
        break;
    case ANIMTYPE_STUNNED:
        return "STUNNED";
        break;
    case ANIIMTYPE_DIE:
        return "DIE";
        break;
    }
}

typedef struct
{
    AnimFrames idleFrames;
    AnimFrames fallFrames;
    AnimFrames walkFrames;
    AnimFrames jumpFrames;
    AnimFrames dashFrames;
    AnimFrames buttstompFrames;
    AnimFrames buttBounceFrames;
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

const Img *imgs_player_buttdash[] = {
    &img_player_buttdash_0};

const Img *imgs_player_buttbounce[] = {
    &img_player_buttbounce_0};

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
const AnimGroup animgroup_player = {

    .idleFrames = {.frameSpeed = 20, .numImages = ARRAYSIZE(imgs_player_idle), .images = imgs_player_idle},
    .fallFrames = {.frameSpeed = 4, .numImages = ARRAYSIZE(imgs_player_fall), .images = imgs_player_fall},
    .walkFrames = {.frameSpeed = 3, .numImages = ARRAYSIZE(imgs_player_walk), .images = imgs_player_walk},
    .jumpFrames = {.frameSpeed = 7, .numImages = ARRAYSIZE(imgs_player_jump), .images = imgs_player_jump},
    .dashFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_dash), .images = imgs_player_dash},
    .buttstompFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_buttdash), .images = imgs_player_buttdash},
    .buttBounceFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_buttbounce), .images = imgs_player_buttbounce},
    .knockbackFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_knockback), .images = imgs_player_knockback},
    .stunFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_stun), .images = imgs_player_stun},
    .dieFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_player_die), .images = imgs_player_die}};

//
// MOB1
//

// pad it out a bit with some delays
const Img *imgs_mob1_idle[] = {
    &img_mob1_idle_0,
    &img_mob1_idle_0,
    &img_mob1_idle_0,
    &img_mob1_idle_1,
    &img_mob1_idle_0,
    &img_mob1_idle_0,
    &img_mob1_idle_0,
    &img_mob1_idle_2,
    &img_mob1_idle_3,
    &img_mob1_idle_3,
    &img_mob1_idle_4,
    &img_mob1_idle_3,
    &img_mob1_idle_4,
    &img_mob1_idle_3,
};

const Img *imgs_mob1_fall[] = {
    &img_mob1_fall_0, &img_mob1_fall_1, &img_mob1_fall_2, &img_mob1_fall_1};

const Img *imgs_mob1_walk[] = {
    &img_mob1_walk_0,
    &img_mob1_walk_1,
    &img_mob1_walk_2,
    &img_mob1_walk_3,
    &img_mob1_walk_4,
    &img_mob1_walk_5,
    &img_mob1_walk_6,
    &img_mob1_walk_7,
};

const Img *imgs_mob1_jump[] = {
    &img_mob1_jump_0, &img_mob1_jump_1};

const Img *imgs_mob1_dash[] = {
    &img_mob1_dash_0};

const Img *imgs_mob1_buttdash[] = {
    &img_mob1_buttdash_0};

const Img *imgs_mob1_buttbounce[] = {
    &img_mob1_buttbounce_0};

const Img *imgs_mob1_knockback[] = {
    &img_mob1_knockback_0};

const Img *imgs_mob1_stun[] = {
    &img_mob1_stun_0};

const Img *imgs_mob1_die[] = {
    &img_mob1_die_0};

// C++ would be nice here
const AnimGroup animgroup_mob1 = {

    .idleFrames = {.frameSpeed = 20, .numImages = ARRAYSIZE(imgs_mob1_idle), .images = imgs_mob1_idle},
    .fallFrames = {.frameSpeed = 4, .numImages = ARRAYSIZE(imgs_mob1_fall), .images = imgs_mob1_fall},
    .walkFrames = {.frameSpeed = 3, .numImages = ARRAYSIZE(imgs_mob1_walk), .images = imgs_mob1_walk},
    .jumpFrames = {.frameSpeed = 7, .numImages = ARRAYSIZE(imgs_mob1_jump), .images = imgs_mob1_jump},
    .dashFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_dash), .images = imgs_mob1_dash},
    .buttstompFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_buttdash), .images = imgs_mob1_buttdash},
    .buttBounceFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_buttbounce), .images = imgs_mob1_buttbounce},
    .knockbackFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_knockback), .images = imgs_mob1_knockback},
    .stunFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_stun), .images = imgs_mob1_stun},
    .dieFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob1_die), .images = imgs_mob1_die}};

//
// MOB2
//

// pad it out a bit with some delays
const Img *imgs_mob2_idle[] = {
    &img_mob2_idle_0,
    &img_mob2_idle_0,
    &img_mob2_idle_0,
    &img_mob2_idle_1,
    &img_mob2_idle_0,
    &img_mob2_idle_0,
    &img_mob2_idle_0,
    &img_mob2_idle_2,
    &img_mob2_idle_3,
    &img_mob2_idle_3,
    &img_mob2_idle_4,
    &img_mob2_idle_3,
    &img_mob2_idle_4,
    &img_mob2_idle_3,
};

const Img *imgs_mob2_fall[] = {
    &img_mob2_fall_0, &img_mob2_fall_1, &img_mob2_fall_2, &img_mob2_fall_1};

const Img *imgs_mob2_walk[] = {
    &img_mob2_walk_0,
    &img_mob2_walk_1,
    &img_mob2_walk_2,
    &img_mob2_walk_3,
    &img_mob2_walk_4,
    &img_mob2_walk_5,
    &img_mob2_walk_6,
    &img_mob2_walk_7,
};

const Img *imgs_mob2_jump[] = {
    &img_mob2_jump_0, &img_mob2_jump_1};

const Img *imgs_mob2_dash[] = {
    &img_mob2_dash_0};

const Img *imgs_mob2_buttdash[] = {
    &img_mob2_buttdash_0};

const Img *imgs_mob2_buttbounce[] = {
    &img_mob2_buttbounce_0};

const Img *imgs_mob2_knockback[] = {
    &img_mob2_knockback_0};

const Img *imgs_mob2_stun[] = {
    &img_mob2_stun_0};

const Img *imgs_mob2_die[] = {
    &img_mob2_die_0};

// C++ would be nice here
const AnimGroup animgroup_mob2 = {

    .idleFrames = {.frameSpeed = 20, .numImages = ARRAYSIZE(imgs_mob2_idle), .images = imgs_mob2_idle},
    .fallFrames = {.frameSpeed = 4, .numImages = ARRAYSIZE(imgs_mob2_fall), .images = imgs_mob2_fall},
    .walkFrames = {.frameSpeed = 3, .numImages = ARRAYSIZE(imgs_mob2_walk), .images = imgs_mob2_walk},
    .jumpFrames = {.frameSpeed = 7, .numImages = ARRAYSIZE(imgs_mob2_jump), .images = imgs_mob2_jump},
    .dashFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_dash), .images = imgs_mob2_dash},
    .buttstompFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_buttdash), .images = imgs_mob2_buttdash},
    .buttBounceFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_buttbounce), .images = imgs_mob2_buttbounce},
    .knockbackFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_knockback), .images = imgs_mob2_knockback},
    .stunFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_stun), .images = imgs_mob2_stun},
    .dieFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_mob2_die), .images = imgs_mob2_die}};

//
// Various
//

const Img *imgs_door_all[] = {&img_door_0};
const AnimGroup animgroup_door = {.idleFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_door_all), .images = imgs_door_all}};

const Img *imgs_platform0_all[] = {&img_platform0_0};
const AnimGroup animgroup_platform0 = {.idleFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_platform0_all), .images = imgs_platform0_all}};

const Img *imgs_spikeball_all[] = {&img_spikeball_idle_0, &img_spikeball_idle_1};
const AnimGroup animgroup_spikeball = {.idleFrames = {.frameSpeed = 20, .numImages = ARRAYSIZE(imgs_spikeball_all), .images = imgs_spikeball_all}};

const Img *imgs_particle_brown_all[] = {&img_particle_brown_0};
const AnimGroup animgroup_particle_brown = {.idleFrames = {.frameSpeed = 1, .numImages = ARRAYSIZE(imgs_particle_brown_all), .images = imgs_particle_brown_all}};

const Img *imgs_crawler_move[] = {&img_crawler_walk_0, &img_crawler_walk_1, &img_crawler_walk_2, &img_crawler_walk_3, &img_crawler_walk_2, &img_crawler_walk_1};
const Img *imgs_crawler_stunned[] = {&img_crawler_stunned_0};

const int CRAWLER_ANIM_SPEED = 20;

const AnimGroup animgroup_crawler = {

    .idleFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .fallFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .walkFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .jumpFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .dashFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .buttstompFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .buttBounceFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .knockbackFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_move), .images = imgs_crawler_move},
    .stunFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_stunned), .images = imgs_crawler_stunned},
    .dieFrames = {.frameSpeed = CRAWLER_ANIM_SPEED, .numImages = ARRAYSIZE(imgs_crawler_stunned), .images = imgs_crawler_stunned}};