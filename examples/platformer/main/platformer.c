// 8BitMods Platformer Example
// The goal here is to provide a working example
// with good readability and flexibility
// so it's not the most optimal code ever
// but will hopefully serve as a strong foundation for
// other projects

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "anims.h"
#include "images/maps.h"
#include "esp_heap_caps.h"
#include "vmupro_utils.h"

const char *TAG = "[Platformer]";

// could un-const these for debugging
// but the compiler at least strips
// these paths for now
const bool DEBUG_NO_GRAV = false;
const bool DEBUG_SPRITEBOX = false;
const bool DEBUG_HITBOX = false;
const bool DEBUG_HITPOINTS = false;
const bool DEBUG_SCROLL_ZONE = false;
const bool DEBUG_NO_Y = false;
const bool DEBUG_ONLY_SPAWN_PLAYER = false;
const bool DEBUG_ONLY_MOVE_PLAYER = false;

#define LAYER_BG 0
#define LAYER_COLS 1

// shift fixed point maths to/from world/subpixel coords
#define SHIFT 4

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TILE_SIZE_PX 16
// e.g. 1 tile is 256 subpixel units
#define TILE_SIZE_SUB (TILE_SIZE_PX << SHIFT)

// the spritesheet/atlas
// could read it out from the data
// but we'll improve performance by hard coding it
#define TILEMAP_WIDTH_TILES 16
#define TILEMAP_HEIGHT_TILES 16
#define TILEMAP_WIDTH_PIXELS (TILEMAP_WIDTH_TILES * TILE_SIZE_PX)
#define TILEMAP_HEIGHT_PIXELS (TILEMAP_HEIGHT_TILES * TILE_SIZE_PX)

// the actual map
// TODO: get rid
#define MAP_WIDTH_TILES 128
#define MAP_HEIGHT_TILES 64
#define MAP_WIDTH_PIXELS (MAP_WIDTH_TILES * TILE_SIZE_PX)
#define MAP_HEIGHT_PIXELS (MAP_HEIGHT_TILES * TILE_SIZE_PX)

// Hard coded rows in the tilemap for transparency and one-way interaction
#define TILEMAP_ONEWAY_PLATFORM_ROW_0 0
#define TILEMAP_ONEWAY_PLATFORM_ROW_11 11
#define TILEMAP_TRANSPARENT_ROW_11 11
#define TILEMAP_TRANSPARENT_ROW_12 12
#define TILEMAP_SMASHABLE_COL_13 13
#define TILEMAP_SPAWNDATA_ROW_13 13
#define TILEMAP_ANIMATED_TRIGGER_COL_14 14

//__TEST__ rename
#define TILEMAP_LAVA_ID_14 14
#define TILEMAP_LAVA_ID_30 30
#define TILEMAP_WATER_ID_44 46
#define TILEMAP_WATER_ID_60 62
#define TILEMAP_INSTAKILL_ID_76 78

#define BLOCK_NULL 0xFF
#define DMG_TILEMAP_LAVA 2
#define DMG_INSTAKILL 0xFF
#define DMG_ALWAYS_STUN 0xFF

// TODO: const a bunch of this
const int DEFAULT_LIFE_COUNT = 3;
// give it a few frames before you can continue
const int POST_DEATH_FRAME_DELAY = 60;
const int TRANSITION_FRAME_DELAY = 20;
const int INVULN_FRAME_DELAY = 20;
const int DOOR_THRESH_FRAMES = 15;
const int DOOR_THRESH_SPEED = 30;
const int DASHBONK_THRESH_SPEED = 64;
const int DASHBONK_MINIMAL_KNOCKBACK = 8;
const int BUTTDASH_THRESH_SPEED = 100;        // how fast you should be hitting the ground before a buttstomp happens
const int BUTTBOUNCE_MAX_VEL = TILE_SIZE_SUB; // how much uppy bounce before we clamp it
const int HEADBUTT_THRESH_SPEED = 16;         // much lower than butt stomp, it should virtually always pass
const uint32_t COYOTE_TIME_FRAME_THRESH = 3;

// not stored on the sprite
// since we may despawn/respawn
// or control other sprites
typedef struct
{

  int lifeCount;
  int levelNum;

} PersistentData;

PersistentData pData;

int camX = 0;
int camY = 0;
int frameCounter = 0;
int globalIndexer = 0;
int dirIndexer = 0;

uint32_t CalcDJB2(uint8_t *inBytes, uint32_t inLength)
{

  uint32_t returnVal = 5381;
  for (int i = 0; i < inLength; i++)
  {
    returnVal = (returnVal << 5) + returnVal + inBytes[i];
  }
  return returnVal;
}

uint32_t rng = 1234;

uint32_t GetRNG(int maxVal)
{
  // it'll do
  rng = (rng << 5) + rng;
  return rng % maxVal;
}

bool didDecompressImages = false;
uint8_t *decompressedImageDataTable[allImagesLength];

// We only have 2 tile layers per level
// 256*64 = 16k per layer
// 0 for background
// 1 for foreground
enum
{
  MAX_DECOMPRESSED_TILE_LAYERS = 2
};
bool hasDecompressedTileLayers = false;
uint8_t *decompressedTileLayerTable[MAX_DECOMPRESSED_TILE_LAYERS];

// prevent rubber banding, move the camera within a scrolling
// area which allows you to see further ahead than behind
// based on where you've walked on the screen, and where
// you're facing
#define SCROLLZONE_WIDTH 28
#define SCROLLZONE_MAXOFFSET 40
#define SCROLLZONE_SPEED 3
int scrollZoneWorldX = 50;
int scrollzoneOffsetX = 0;

typedef struct
{
  int x;
  int y;
} Vec2;

typedef enum
{
  GSTATE_UNINIT,
  GSTATE_START,
  GSTATE_INTROFADE,
  GSTATE_INGAME,
  GSTATE_PAUSED,
  GSTATE_TRANSITION,
  GSTATE_DED,
  GSTATE_GAMEOVER

} GameState;

typedef enum
{
  DIRINDEX_HORZ,
  DIRINDEX_VERT,
  DIRINDEX_DOWN,
  DIRINDEX_NONE
} DirIndices;

// temporary
Vec2 uiAnimOffset;
Vec2 uiAnimVelo;
// how long we've been on this particular game state
int uiStateFrameCounter;

GameState gState = GSTATE_UNINIT;

typedef struct
{
  union
  {
    struct
    {
      int x;
      int y;
      int width;
      int height;
    };

    struct
    {
      Vec2 pos;
      Vec2 size;
    } vecs;
  };
} BBox;

typedef struct
{
  // val = how many frames held for
  uint32_t up;
  uint32_t down;
  uint32_t left;
  uint32_t right;
  uint32_t run;
  uint32_t jump;
} Inputs;

typedef enum
{

  MM_FALL,
  MM_WALK,
  MM_DASH,
  MM_JUMP,
  MM_KNOCKBACK,
  MM_BUTTDASH,
  MM_BUTTBOUNCE,
  MM_STUNNED,

} MoveMode;

const char *MoveModetoString(MoveMode inMode)
{

  switch (inMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "UNNAMED MOVE MODE %d\n", (int)inMode);
    return "UNKNOWN MOVVE MODE";
    break;

  case MM_FALL:
    return "FALL";
    break;
  case MM_WALK:
    return "WALK";
    break;
  case MM_DASH:
    return "DASH";
    break;
  case MM_JUMP:
    return "JUMP";
    break;
  case MM_KNOCKBACK:
    return "KNOCKBACK";
    break;
  case MM_BUTTDASH:
    return "BUTTDASH";
    break;
  case MM_BUTTBOUNCE:
    return "BUTTBOUNCE";
    break;
  case MM_STUNNED:
    return "STUNNED";
    break;
  }
}

typedef enum
{
  KNOCK_NUDGE,   // nudged while stunned
  KNOCK_MINIMAL, // dashed into an enemy, knock back a tiiiny amount for visual flare
  KNOCK_SOFT,    // dashed into a wall or something
  KNOCK_HARD,    // bumped into something spiky
} KnockbackStrength;

typedef enum
{
  BUTTSTR_GROUND, // normal ground bounce, etc
  BUTTSTR_ENEMY,  // slightly bigger bounce of enemies
} ButtStrength;

typedef enum
{
  ANCHOR_HLEFT,
  ANCHOR_HMID,
  ANCHOR_HRIGHT,
} Anchor_H;

typedef enum
{
  ANCHOR_VTOP,
  ANCHOR_VMID,
  ANCHOR_VBOTTOM,
} Anchor_V;

typedef enum
{
  // first row of sprites in tilemap
  STYPE_PLAYER,
  STYPE_CHECKPOINT,
  STYPE_FIN,
  STYPE_RESERVED_3,
  STYPE_DOOR,
  STYPE_ROOM_MARKER,
  STYPE_PLATFORM_0,
  STYPE_RESERVED_6,
  STYPE_RESERVED_7,
  STYPE_RESERVED_9,
  STYPE_PARTICLE_BROWN,
  STYPE_RESERVED_11,
  STYPE_RESERVED_12,
  STYPE_RESERVED_13,
  STYPE_RESERVED_14,
  STYPE_RESERVED_15,
  // second row of sprites in tilemap
  STYPE_INDEXER_0,
  STYPE_INDEXER_1,
  STYPE_INDEXER_2,
  STYPE_INDEXER_3,
  STYPE_INDEXER_4,
  STYPE_INDEXER_5,
  STYPE_INDEXER_6,
  STYPE_ROW2_7,
  STYPE_DIRECTION_0_H,
  STYPE_DIRECTION_1_V,
  STYPE_DIRECTION_2_D,
  STYPE_DIRECTION_3_NONE,
  STYPE_ROW2_12,
  STYPE_ROW2_13,
  STYPE_ROW2_14,
  STYPE_ROW2_15,
  // third row of sprites in the tilemap
  STYPE_GREENDUCK,
  STYPE_REDDUCK,
  STYPE_ROW3_2,
  STYPE_ROW3_3,
  STYPE_SPIKEBALL,
  STYPE_MAX
} SpriteType;

// Note: we do some >= on these, be careful changing them
typedef enum
{
  SOLIDMASK_NONE = 0x0,            // non solid
  SOLIDMASK_TILE_TRIGGER = 0x01,   // we can touch it, could be liquid, instadeath, etc
  SOLIDMASK_SPRITE_TRIGGER = 0x02, // we can touch it, but doesn't block us (door, pickup, etc)
  SOLIDMASK_TILE_SOLID = 0x04,     // solid: tile
  SOLIDMASK_SPRITE_SOLID = 0x08,   // solid: other creature
  SOLIDMASK_ONESIDED = 0x10,       // solid: one-way platform
  SOLIDMASK_PLATFORM = 0x20        // solid: moving platform
} Solidity;

typedef enum
{
  IMASK_NONE = 0x00,
  IMASK_DMGOUT_HORZ = 0x01,                  // hurty if you walk into it horizontally
  IMASK_DMGOUT_VERT = 0x02,                  // hurty if you land or bonk it
  IMASK_CAN_BE_RIDDEN = 0x04,                //
  IMASK_CAN_RIDE_STUFF = 0x08,               //
  IMASK_DRAW_FIRST = 0x10,                   // doors n things, draw at the back
  IMASK_DRAW_LAST = 0x20,                    // anything which should be drawn on top (not implemented)
  IMASK_SKIP_INPUT = 0x40,                   // don't bother processing input (doors, spikes, projectiles)
  IMASK_SKIP_MOVEMENT = 0x80,                // static, don't process the movement steps (doors, spikes)
  IMASK_SKIP_ANIMSETS = 0x100,               // use only the IDLE anim set (no other move types)
  IMASK_DMGIN_STUNSME = 0x200,               // when something hits me, i get stunned (no dmg unless stunned)
  IMASK_DMGIN_KNOCKSME = 0x400,              // when something hits me, i bounce a bit (w/ dmg)
  IMASK_DMGOUT_IGNORED_WHEN_BOUNCED = 0x800, // don't deal damage if the player buttstomps me
  IMASK_IGNORE_COLLISIONS = 0x1000,          // e.g. moving platforms
  IMASK_SPECIAL_MOVES = 0x2000,              // buttstomp etc
  IMASK_PLATFORM_MOVEMENT = 0x4000,          // e.g. platforms 'walking' accross the sky
} InteractionMask;

typedef struct
{

  // maximum speed (in subpixels) while
  // walking or running (per frame)
  const int max_subspeed_walk;
  const int max_subspeed_run;

  // max of like 256 since that's
  // bigger than a tile in subpixels
  const int max_subfallspeed;

  // accel values when walking, running, etc
  // in subpixels
  const int subaccel_walk;
  const int subaccel_run;
  const int subaccel_air;

  // movement damping values
  const int subdamping_walk;
  const int subdamping_run;
  const int subdamping_air;
  const int subdamping_stunned;

  // max frames for which the up force
  // is applied
  const int max_jump_boost_frames;
  const int max_buttbounce_frames;
  const int sub_buttbounceforce;
  const int max_dash_frames;
  const int max_knockback_frames;
  const int sub_jumpforce;
  const int sub_dashforce;
  const int dashDelayFrames;
  const int sub_gravity;

} PhysParams;

const PhysParams physDefault = {

    .max_subspeed_walk = 80,
    .max_subspeed_run = 140,
    .subaccel_walk = 8,
    .subaccel_run = 9,
    .subaccel_air = 6,
    .subdamping_walk = 6,
    .subdamping_run = 6,
    .subdamping_air = 4,
    .subdamping_stunned = 4,
    .max_jump_boost_frames = 16,
    .max_buttbounce_frames = 16,
    .max_dash_frames = 16,
    .max_knockback_frames = 12,
    .sub_jumpforce = 14,
    .sub_dashforce = 14,
    .sub_buttbounceforce = 14,
    .dashDelayFrames = 50,
    .sub_gravity = 9,
    .max_subfallspeed = 120,

};

const PhysParams physTestMob = {

    .max_subspeed_walk = 10,
    .max_subspeed_run = 140,
    .subaccel_walk = 1, // make 'em slippery
    .subaccel_run = 9,
    .subaccel_air = 6,
    .subdamping_walk = 0,
    .subdamping_run = 6,
    .subdamping_air = 4,
    .subdamping_stunned = 4,
    .max_jump_boost_frames = 16,
    .max_buttbounce_frames = 16,
    .max_dash_frames = 16,
    .max_knockback_frames = 12,
    .sub_jumpforce = 14,
    .sub_dashforce = 14,
    .sub_buttbounceforce = 14,
    .dashDelayFrames = 50,
    .sub_gravity = 9,
    .max_subfallspeed = 120,

};

const PhysParams physPlatform = {

    .max_subspeed_walk = 4,
    .max_subspeed_run = 0,
    .subaccel_walk = 4,
    .subaccel_run = 0,
    .subaccel_air = 0,
    .subdamping_walk = 0,
    .subdamping_run = 0,
    .subdamping_air = 0,
    .subdamping_stunned = 0,
    .max_jump_boost_frames = 0,
    .max_buttbounce_frames = 0,
    .max_dash_frames = 0,
    .max_knockback_frames = 0,
    .sub_jumpforce = 0,
    .sub_dashforce = 0,
    .sub_buttbounceforce = 0,
    .dashDelayFrames = 0,
    .sub_gravity = 0,
    .max_subfallspeed = 0,

};

// profile of spite behaviour
// such as runspeed, can it walk
// off edges, etc
// prefix "sub" means subpixels/fixed point math
// prefix "world" means regular world space 1:1 pixels
typedef struct
{

  int default_health;
  int damage_multiplier;
  int default_despawnTimer;

  Solidity solid;
  InteractionMask iMask;
  const PhysParams *physParams;
  const AnimGroup *defaultAnimGroup;

  Vec2 startVelo;

} SpriteProfile;
// TODO: rename to spriteblueprint?

#define MAX_ROOMS 6
Vec2 roomTopLeftPositions[MAX_ROOMS];
Vec2 roomBottomRightPositions[MAX_ROOMS];
bool hasTopLeftRoomPosition[MAX_ROOMS];
bool hasBottomRightRoomPositions[MAX_ROOMS];

typedef struct
{
  const char *name;
  const TileLayer *bgLayer;
  const TileLayer *colLayer;
  const TileLayer *sparseSpawnData;
} Level;

Level level0 = {
    "Duck Fails",
    &tl_level_0_layer_0,
    &tl_level_0_layer_1,
    &tl_level_0_layer_2};

Level level1 = {
    "Duck & Cover",
    &tl_level_1_layer_0,
    &tl_level_1_layer_1,
    &tl_level_1_layer_2};

Level *allLevels[] = {
    &level0,
    &level1};

Level *currentLevel = NULL;

void CreateProfile(SpriteProfile *inProfile, SpriteType inType)
{

  if (inProfile == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to create sprite profile");
  }

  memset(inProfile, 0, sizeof(SpriteProfile));

  // fill in player defaults
  // then tweak anything we need from there
  SpriteProfile *p = inProfile;

  // some default vals
  p->default_health = 10;
  p->damage_multiplier = 1;

  p->default_despawnTimer = 0;

  p->solid = SOLIDMASK_SPRITE_SOLID;
  p->iMask = IMASK_NONE;
  p->physParams = &physDefault;

  // doors are big, we'll spot mistakes quickly
  p->defaultAnimGroup = &animgroup_door;

  p->startVelo.x = 0;
  p->startVelo.y = 0;

  if (inType == STYPE_PLAYER)
  {
    // default
    p->default_health = 10;
    p->damage_multiplier = 1;
    p->solid = SOLIDMASK_SPRITE_SOLID;
    p->iMask = IMASK_CAN_BE_RIDDEN | IMASK_CAN_RIDE_STUFF | IMASK_DMGIN_KNOCKSME | IMASK_SPECIAL_MOVES;
    p->physParams = &physDefault;
    p->defaultAnimGroup = &animgroup_player;
  }
  else if (inType == STYPE_GREENDUCK)
  {
    p->default_health = 1;
    p->damage_multiplier = 1;
    p->solid = SOLIDMASK_PLATFORM;
    p->iMask = IMASK_CAN_BE_RIDDEN | IMASK_CAN_RIDE_STUFF | IMASK_DMGIN_STUNSME;
    p->physParams = &physTestMob;
    p->defaultAnimGroup = &animgroup_mob1;
  }
  else if (inType == STYPE_REDDUCK)
  {
    p->default_health = 1;
    p->damage_multiplier = 1;
    p->solid = SOLIDMASK_PLATFORM;
    p->iMask = IMASK_DMGIN_KNOCKSME | IMASK_DMGOUT_HORZ | IMASK_DMGOUT_VERT | IMASK_DMGOUT_IGNORED_WHEN_BOUNCED;
    p->physParams = &physTestMob;
    p->defaultAnimGroup = &animgroup_mob2;
  }
  else if (inType == STYPE_PLATFORM_0)
  {
    p->solid = SOLIDMASK_ONESIDED;
    p->defaultAnimGroup = &animgroup_platform0;
    p->physParams = &physPlatform;
    p->iMask = IMASK_SKIP_ANIMSETS | IMASK_CAN_BE_RIDDEN | IMASK_IGNORE_COLLISIONS | IMASK_PLATFORM_MOVEMENT;
  }
  else if (inType == STYPE_DOOR)
  {
    p->solid = SOLIDMASK_SPRITE_TRIGGER;
    p->defaultAnimGroup = &animgroup_door;
    p->iMask = IMASK_SKIP_ANIMSETS | IMASK_SKIP_INPUT | IMASK_SKIP_MOVEMENT | IMASK_DRAW_FIRST;
  }
  else if (inType == STYPE_SPIKEBALL)
  {
    p->solid = SOLIDMASK_SPRITE_SOLID;
    p->defaultAnimGroup = &animgroup_spikeball;
    p->iMask = IMASK_SKIP_ANIMSETS | IMASK_SKIP_INPUT | IMASK_SKIP_MOVEMENT | IMASK_DMGOUT_HORZ | IMASK_DMGOUT_VERT;
  }
  else if (inType == STYPE_PARTICLE_BROWN)
  {
    p->solid = SOLIDMASK_NONE;
    p->iMask = IMASK_SKIP_ANIMSETS | IMASK_SKIP_INPUT;
    p->defaultAnimGroup = &animgroup_particle_brown;
    p->startVelo.x = GetRNG(40) - 20;
    p->startVelo.y = -GetRNG(40) + 40;
    p->default_despawnTimer = 60;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "No sprite profile for sprite type %d", (int)inType);
  }
}

// forward decl for internal references
typedef struct Sprite Sprite;

typedef struct Sprite
{

  // sentinel value
  // since sprites are zero'd on unload
  // if something references a sprite with a false
  // sentinel value, we've got a stale ref bug
  bool sentinel;

  //
  // Config options
  // from which the rest of the struct is calculated
  //

  SpriteType sType;
  SpriteProfile profile;
  const PhysParams *phys;

  char name[10];
  Vec2 subSpawnPos;

  //
  // Runtime stuff
  // calculated via ResetSprite()
  // which transfers spawn data from the profile
  //

  // the frame we spawned on
  int spawnFrame;
  // indexer for e.g. room edge groups, door ids, etc
  int indexer;
  // 2nd param for e.g. horz/vert movement
  int dirIndexer;

  // despawn at the end of the frame
  // so we don't screw with collision/loop logic
  bool markedForDespawn;
  int despawnTimer;

  // typically the image's bbox
  // in world coords
  BBox worldBBox;
  // the actual hitbox in subpixel space
  BBox subHitBox;

  // The actual fixed-point position of the sprite
  // could be middle-bottom (feet) or top-left (head)
  Vec2 subPos;
  // calculated at the end of the frame
  // since we may do multiple tweaks per frame
  Vec2 lastSubPos;

  Vec2 subVelo;
  Vec2 lastSubVelo;

  Vec2 subKnockbackAccel;

  // for coyote time
  uint32_t lastGroundedFrame;
  // see usage
  Sprite *thingImRiding;
  bool isGrounded;
  bool isOnWall;
  bool onGroundLastFrame;
  bool onWallLastFrame;
  bool inLiquid;
  bool inLiquidLastFrame;
  int jumpFrameNum;
  int dashFrameNum;
  int knockbackFrameNum;
  int invulnFrameNum;

  int buttBounceFrameNum;
  int numButtStomps;
  int buttstompSubVelo;

  bool mustReleaseJump;
  bool mustReleaseDash;

  // config options
  bool isPlayer;
  Anchor_H anchorH;
  Anchor_V anchorV;

  // note: possibly better named "facing positive"
  // as it's used occasionally for vertical platforms
  bool facingRight;
  bool wasRunningLastTimeWasOnGround;

  Inputs input;
  MoveMode moveMode;

  const AnimGroup *anims;
  const AnimFrames *activeFrameSet;
  // updated per frame, to measure elapsed frames
  // TODO: rename to like gameFrameWhenAnimChanged
  int lastGameframe;
  // reset on anim changes
  int animIndex;
  bool animReversing;
  AnimTypes animID;

  int health;

  // to simplify one-way platforms
  // we'll make sure the player
  int highestYSubPosInJump;
  int lastgroundedYSubPos;

} Sprite;

#define MAX_SPRITES 20
int numSprites = 0;
Sprite *sprites[MAX_SPRITES];

typedef enum
{
  DIR_UP,
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LEFT,
} Direction;

void SetAnim(Sprite *spr, AnimTypes inType)
{

  if (spr->animID == inType)
  {
    return;
  }

  if (spr->profile.iMask & IMASK_SKIP_ANIMSETS)
  {
    return;
  }

  const char *animString = AnimTypeToString(inType);
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s Anim %s", frameCounter, spr->name, animString);

  // TODO: could move this to a table
  switch (inType)
  {
  case ANIMTYPE_IDLE:
    spr->activeFrameSet = &spr->anims->idleFrames;
    break;
  case ANIMTYPE_WALK:
    spr->activeFrameSet = &spr->anims->walkFrames;
    break;
  case ANIMTYPE_JUMP:
    spr->activeFrameSet = &spr->anims->jumpFrames;
    break;
  case ANIMTYPE_FALL:
    spr->activeFrameSet = &spr->anims->fallFrames;
    break;
  case ANIIMTYPE_DIE:
    spr->activeFrameSet = &spr->anims->dieFrames;
    break;
  case ANIMTYPE_DASH:
    spr->activeFrameSet = &spr->anims->dashFrames;
    break;
  case ANIMTYPE_KNOCKBACK:
    spr->activeFrameSet = &spr->anims->knockbackFrames;
    break;
  case ANIMTYPE_BUTTBOUNCE:
    spr->activeFrameSet = &spr->anims->buttBounceFrames;
    break;
  case ANIMTYPE_BUTTSTOMP:
    spr->activeFrameSet = &spr->anims->buttstompFrames;
    break;
  case ANIMTYPE_STUNNED:
    spr->activeFrameSet = &spr->anims->stunFrames;
    break;
  default:
    // we'll patch this in a sec
    spr->activeFrameSet = NULL;
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled animation index %d\n", (int)inType);
    break;
  }

  // Nothing found, switch to default/idle, which everything should have
  if (spr->activeFrameSet == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite has no anim for type %d\n", (int)inType);
    spr->activeFrameSet = &spr->anims->idleFrames;
  }

  spr->animIndex = 0;
  spr->lastGameframe = frameCounter;
  spr->animID = inType;
  spr->animReversing = false;

  // just in case
  if (spr->activeFrameSet->numImages == 0)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Sprite was assigned an empty frame set\n");
  }
}

AnimTypes GetAnim(Sprite *spr)
{
  return spr->animID;
}

bool ValidateAnim(Sprite *spr)
{

  // is it valid?
  if (spr->activeFrameSet == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite %s has null frameset!", spr->name);
    return false;
  }

  // is it within bounds?
  if (spr->animIndex < 0 || spr->animIndex >= spr->activeFrameSet->numImages)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite %s's frame index is outside the bounds 0-%d!", spr->name, spr->activeFrameSet->numImages);
    spr->animIndex = 0;
  }

  return true;
}

const Img *GetActiveImage(Sprite *spr)
{

  if (!ValidateAnim(spr))
  {
    return &img_player_idle_0;
  }

  const Img *returnVal = spr->activeFrameSet->images[spr->animIndex];
  return returnVal;
}

void UpdateAnimation(Sprite *spr)
{

  if (!ValidateAnim(spr))
  {
    return;
  }

  const AnimFrames *frameSet = spr->activeFrameSet;
  AnimMode mode = frameSet->mode;
  int animSpeed = frameSet->frameSpeed;
  bool framesPassed = (frameCounter >= spr->lastGameframe + animSpeed);

  if (!framesPassed)
    return;

  if (mode == ANIMMODE_LOOP)
  {

    spr->animIndex++;
    if (spr->animIndex >= frameSet->numImages)
    {
      spr->animIndex = 0;
    }
  }
  else if (mode == ANIMMODE_ONESHOT)
  {

    // check if it's within range
    if (spr->animIndex < frameSet->numImages - 1)
    {
      // advance to the end, then stay there
      spr->animIndex++;
    }
  }
  else if (mode == ANIMMODE_PINGPONG)
  {

    // pre-check states to avoid playing a frame twice
    if (!spr->animReversing && spr->animIndex >= frameSet->numImages - 1)
    {
      spr->animReversing = true;
    }
    if (spr->animReversing && spr->animIndex <= 0)
    {
      spr->animReversing = false;
    }

    if (!spr->animReversing)
    {
      spr->animIndex++;
    }
    else
    {
      spr->animIndex--;
    }
  }

  spr->lastGameframe = frameCounter;
}

Vec2 Sub2World(Vec2 *inVec)
{
  Vec2 rVal = {inVec->x >> SHIFT, inVec->y >> SHIFT};
  return rVal;
}

Vec2 World2Sub(Vec2 *inVec)
{
  Vec2 rVal = {inVec->x << SHIFT, inVec->y << SHIFT};
  return rVal;
}

Vec2 World2Screen(Vec2 *srcPos)
{

  Vec2 returnVal;
  returnVal.x = srcPos->x - camX;
  returnVal.y = srcPos->y - camY;
  return returnVal;
}

Vec2 Sub2Screen(Vec2 *srcPos)
{
  Vec2 sub2world = Sub2World(srcPos);
  Vec2 world2Scr = World2Screen(&sub2world);
  return world2Scr;
}

Vec2 GetSubPos(Sprite *inSprite)
{
  return inSprite->subPos;
}

Vec2 GetWorldPos(Sprite *inSprite)
{
  Vec2 rVal = Sub2World(&inSprite->subPos);
  return rVal;
}

// Returns the foot/center pos
// not your sprite pos
Vec2 GetScreenPos(Sprite *inSprite)
{
  return Sub2Screen(&inSprite->subPos);
}

// Get the camera bounding box in world coords
BBox CameraBBoxWorld()
{
  BBox rVal;
  rVal.x = camX;
  rVal.y = camY;
  rVal.width = SCREEN_WIDTH;
  rVal.height = SCREEN_HEIGHT;
  return rVal;
}

Sprite *player = NULL;

typedef struct
{
  Solidity solidMask;
  Sprite *otherSpriteOrNull;
} GroundHitInfo;

//
// Protos
//
//__TEST__ This can be scrubbed when things are moved to headers
bool CheckGrounded(Sprite *spr, GroundHitInfo *groundHitInfoOrNull);
Vec2 GetPointOnSprite(Sprite *spr, bool hitBox, Anchor_H anchorH, Anchor_V anchorV);
Solidity CheckSpriteCollision(Sprite *spr, Direction dir, Vec2 *subOffset, const char *src, bool ignorePlayer);
void GotoGameState(GameState inState);
void UnloadTileLayers();
void DecompressAllTileLayers(Level *currentLevel, PersistentData *inData);
void StopJumpBoost(Sprite *spr, bool resetMoveMode, const char *src);
void StopDashBoost(Sprite *spr, bool resetMovemode, const char *src);
void TryKnockback(Sprite *spr, KnockbackStrength inStr, const char *cause, Sprite *srcOrNull);
void StopKnockback(Sprite *spr, bool resetMoveMode, const char *cause);
void StopButtstomp(Sprite *spr, bool resetMoveMode, const char *cause);
bool TryButtBounce(Sprite *spr, ButtStrength inStr, const char *cause);

void DrawBBoxScreen(BBox *box, uint16_t inCol)
{

  Vec2 screenPos = box->vecs.pos;
  int x2 = screenPos.x + box->width;
  int y2 = screenPos.y + box->height;
  vmupro_draw_rect(screenPos.x, screenPos.y, x2, y2, inCol);
}

void DrawBBoxWorld(BBox *box, uint16_t inCol)
{

  Vec2 worldPos = box->vecs.pos;
  Vec2 screenPos = World2Screen(&worldPos);
  int x2 = screenPos.x + box->width;
  int y2 = screenPos.y + box->height;
  vmupro_draw_rect(screenPos.x, screenPos.y, x2, y2, inCol);
}

void DrawBBoxSub(BBox *box, uint16_t inCol)
{

  Vec2 subPos = box->vecs.pos;
  Vec2 worldPos = Sub2World(&subPos);
  Vec2 screenPos = World2Screen(&worldPos);

  Vec2 subSize = box->vecs.size;
  Vec2 worldSize = Sub2World(&subSize);

  int x2 = screenPos.x + worldSize.x;
  int y2 = screenPos.y + worldSize.y;
  vmupro_draw_rect(screenPos.x, screenPos.y, x2, y2, inCol);
}

// World->Screen space
void DrawSpriteBoundingBox(Sprite *inSprite, uint16_t inCol)
{

  BBox *box = &inSprite->worldBBox;
  DrawBBoxWorld(box, inCol);
}

// Subpixel->Screen space
void DrawSpriteHitBox(Sprite *inSprite, uint16_t inCol)
{
  BBox *hitbox = &inSprite->subHitBox;
  DrawBBoxSub(hitbox, inCol);
}

// Updates the bounding box when the pos or img changes
void OnSpriteMoved(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "OnSpriteMoved() - null sprite");
    return;
  }

  const Img *img = GetActiveImage(spr);

  if (img == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Set the sprite img before attempting to update the bboxs");
    return;
  }

  // store the sprite box pos in world space
  // and the hitbox pos in subpixel space

  Vec2 worldOrigin = GetWorldPos(spr);
  Vec2 subOrigin = GetSubPos(spr);

  spr->worldBBox.x = worldOrigin.x;
  spr->subHitBox.x = subOrigin.x;
  spr->worldBBox.y = worldOrigin.y;
  spr->subHitBox.y = subOrigin.y;

  int worldWidth = img->width;
  int worldHeight = img->height;
  int subWidth = (worldWidth << SHIFT);
  int subHeight = (worldHeight << SHIFT);

  spr->worldBBox.width = worldWidth;
  spr->worldBBox.height = worldHeight;
  spr->subHitBox.width = subWidth;
  spr->subHitBox.height = subHeight;

  // horizontal part
  if (spr->anchorH == ANCHOR_HLEFT)
  {
    // left side of the sprite is on the origin
  }
  else if (spr->anchorH == ANCHOR_HMID)
  {
    // sprite is h-centered on the origin
    spr->worldBBox.x -= worldWidth / 2;
    spr->subHitBox.x -= subWidth / 2;
  }
  else if (spr->anchorH == ANCHOR_HRIGHT)
  {
    // right side of sprite is on the origin
    spr->worldBBox.x -= img->width;
    spr->subHitBox.x -= subWidth;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite has unhandled horz alginment type");
  }

  // vertical part
  if (spr->anchorV == ANCHOR_VTOP)
  {
    // top of the sprite is on the origin
    // e.g. something crawling along a ceiling
  }
  else if (spr->anchorV == ANCHOR_VMID)
  {
    // sprite is v-centered on the origin
    spr->worldBBox.y -= worldHeight / 2;
    spr->subHitBox.y -= subHeight / 2;
  }
  else if (spr->anchorV == ANCHOR_VBOTTOM)
  {
    // bottom of the sprite is on the origin
    spr->worldBBox.y -= worldHeight;
    spr->subHitBox.y -= subHeight;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite has unhandled vert alginment type");
  }
}

void AddVec(Vec2 *targ, Vec2 *delta)
{
  targ->x += delta->x;
  targ->y += delta->y;
}

Vec2 AddNewVec(Vec2 *a, Vec2 *b)
{
  Vec2 rVal;
  rVal.x = a->x + b->x;
  rVal.y = a->y + b->y;
  return rVal;
}

void SubVec(Vec2 *targ, Vec2 *delta)
{
  targ->x -= delta->x;
  targ->y -= delta->y;
}

Vec2 SubNewVec(Vec2 *a, Vec2 *b)
{
  Vec2 rVal;
  rVal.x = a->x - b->x;
  rVal.y = a->y - b->y;
  return rVal;
}

void AddVecInts(Vec2 *targ, int x, int y)
{
  targ->x += x;
  targ->y += y;
}

Vec2 ZeroVec()
{
  Vec2 rVal = {0, 0};
  return rVal;
}

// can't use GetPlayercamPos
// since the cam's (usually) based on
// the player's position
Vec2 GetPlayerWorldPos()
{
  Vec2 wPos = GetWorldPos(player);
  return wPos;
}

void SetSubPos(Sprite *spr, Vec2 *newPos)
{
  spr->subPos = *newPos;
  OnSpriteMoved(spr);
}

void SetSubPosX(Sprite *spr, int newX)
{
  spr->subPos.x = newX;
  OnSpriteMoved(spr);
}
void SetSubPosY(Sprite *spr, int newY)
{
  spr->subPos.y = newY;
  OnSpriteMoved(spr);
}

void AddSubPos(Sprite *spr, Vec2 *delta)
{
  AddVec(&spr->subPos, delta);
  OnSpriteMoved(spr);
}

void AddSubPos2(Sprite *spr, int xDelta, int yDelta)
{
  Vec2 combined = {xDelta, yDelta};
  AddSubPos(spr, &combined);
}

void SetWorldPos(Sprite *spr, Vec2 *newPos)
{
  Vec2 world2Sub = World2Sub(newPos);
  SetSubPos(spr, &world2Sub);
}

int Abs(int inVal)
{
  if (inVal < 0)
    return -inVal;
  return inVal;
}

bool MoveModeInAir(Sprite *spr)
{

  MoveMode mm = spr->moveMode;
  if (mm == MM_FALL || mm == MM_JUMP)
  {
    return true;
  }
  return false;
}

bool MoveModeOnGround(Sprite *spr)
{

  MoveMode mm = spr->moveMode;
  if (mm == MM_DASH || mm == MM_WALK)
  {
    return true;
  }
  return false;
}

// we may extend this later for knockback, etc
bool SpriteJumping(Sprite *spr)
{
  return spr->moveMode == MM_JUMP;
}

bool SpriteDashing(Sprite *spr)
{
  return spr->moveMode == MM_DASH;
}

bool SpriteIsButtDashing(Sprite *spr)
{
  return spr->moveMode == MM_BUTTDASH;
}
bool SpriteIsButtBouncing(Sprite *spr)
{
  return spr->moveMode == MM_BUTTBOUNCE;
}
// either bouncing or dashing
bool SpriteDoingButtStuff(Sprite *spr)
{
  return SpriteIsButtDashing(spr) || SpriteIsButtBouncing(spr);
}

bool SpriteStunned(Sprite *spr)
{
  return spr->moveMode == MM_STUNNED;
}

bool SpriteDashingAboveBonkThresh(Sprite *spr)
{

  if (!SpriteDashing(spr))
    return false;

  if (Abs(spr->lastSubVelo.x) < DASHBONK_THRESH_SPEED)
    return false;

  return true;
}

bool SpriteButtStompingAboveThresh(Sprite *spr)
{

  if (!SpriteIsButtDashing(spr))
    return false;

  if (Abs(spr->lastSubVelo.y) < BUTTDASH_THRESH_SPEED)
    return false;

  return true;
}

bool SpriteIsMoving(Sprite *spr)
{

  return (spr->subVelo.x != 0) || (spr->subVelo.y != 0);
}

bool SpriteIsDead(Sprite *spr)
{
  return spr->animID == ANIIMTYPE_DIE;
}

bool SpriteIsKnockback(Sprite *spr)
{
  return spr->moveMode == MM_KNOCKBACK;
}

void SetMoveMode(Sprite *spr, MoveMode inMode, const char *cause)
{

  if (SpriteIsDead(spr))
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %d set movemode to %d while dead!", frameCounter, spr, (int)inMode);
  }

  if (SpriteStunned(spr))
  {
    // ignore movement changes
    printf("__TEST__ ignoring state changes due to stunnedness\n");
    return;
  }

  MoveMode oldMode = spr->moveMode;
  const char *modeString = MoveModetoString(inMode);
  if (oldMode != inMode)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s MoveMode %s Cause= %s", frameCounter, spr->name, modeString, cause);
  }

  //
  // Cancel any ongoing anims *before* we switch modes
  // since they won't cancel unless we're in the right mode
  //

  // pretty much any change should cancel a dash
  if (inMode != MM_DASH)
  {
    StopDashBoost(spr, false, modeString);
  }
  if (inMode != MM_JUMP)
  {
    StopJumpBoost(spr, false, modeString);
  }
  if (inMode != MM_KNOCKBACK)
  {
    StopKnockback(spr, false, modeString);
  }
  if (inMode != MM_BUTTDASH && inMode != MM_BUTTBOUNCE)
  {
    StopButtstomp(spr, false, modeString);
  }

  //
  // Finally, apply the new mode
  //
  spr->moveMode = inMode;
}

// Player spawn pos
Vec2 GetPlayerWorldStartPos()
{
  Vec2 rVal = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 36)};
  return rVal;
}

bool SpriteCanDie(Sprite *spr)
{
  if (spr->health < 0)
    return false;
  if (SpriteIsDead(spr))
    return false;
  return true;
}

bool SpriteCanTakeDamage(Sprite *spr)
{

  if (!SpriteCanDie(spr))
    return false;
  if (SpriteIsKnockback(spr))
    return false;
  if (spr->invulnFrameNum > 0)
    return false;

  return true;
}

bool SpriteCanBeStunned(Sprite *spr)
{

  if (!SpriteCanTakeDamage(spr))
    return false;

  if (SpriteStunned(spr))
    return false;

  return true;
}

void MarkSpriteForDespawn(Sprite *spr, const char *cause)
{

  if (spr->markedForDespawn)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d, Sprite %s marked for despawn: cause=%s but already marked!\n", frameCounter, spr->name, cause);
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d, Sprite %s marked for despawn: cause=%s\n", frameCounter, spr->name, cause);
  spr->markedForDespawn = true;
}

void OnSpriteDied(Sprite *spr, const char *cause)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "NULL sprites can't die");
    return;
  }
  if (!SpriteCanDie(spr))
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s you can't kill that which is already dead", frameCounter, spr->name);
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s died. cause %s RIP", frameCounter, spr->name, cause);

  SetAnim(spr, ANIIMTYPE_DIE);
  // prevent collisions, other interactions
  spr->profile.iMask = IMASK_SKIP_INPUT;
  spr->profile.solid = SOLIDMASK_NONE;

  // little up boost for the death anim
  spr->subVelo.x = 0;
  // idk magic number, make it look dramatic
  spr->subVelo.y = -200;

  if (spr == player)
  {
    if (pData.lifeCount == 0)
    {
      GotoGameState(GSTATE_GAMEOVER);
    }
    else
    {
      GotoGameState(GSTATE_DED);
    }
  }
  else
  {

    // MarkSpriteForDespawn(spr, cause);
    SetAnim(spr, ANIIMTYPE_DIE);
    spr->despawnTimer = 64;
  }
}

// returns: was the stun successful
bool StunSprite(Sprite *spr, int strength, Sprite *sourceOrNull, const char *cause)
{

  const char *srcName = sourceOrNull != NULL ? sourceOrNull->name : "WORLD DMG";

  if (!SpriteCanBeStunned(spr))
  {
    if (strength != DMG_ALWAYS_STUN)
    {
      return false;
    }
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s stunned with str %d from %s, cause=%s", frameCounter, spr->name, strength, spr->name, cause);

  SetMoveMode(spr, MM_STUNNED, cause);
  return true;
}

void SpriteTakeDamage(Sprite *spr, int value, Sprite *sourceOrNull, const char *cause)
{

  const char *srcName = sourceOrNull != NULL ? sourceOrNull->name : "WORLD DMG";

  if (value == 0)
  {
    return;
  }

  // if we can't take damage, ignore unless it's instakill
  if (!SpriteCanTakeDamage(spr))
  {
    if (value != DMG_INSTAKILL)
    {
      return;
    }
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s took %d damage (hp=%d) from %s, cause=%s", frameCounter, spr->name, value, spr->health, srcName, cause);

  if (value > spr->health)
  {
    value = spr->health;
  }

  spr->health -= value;

  if (spr->health <= 0)
  {
    OnSpriteDied(spr, cause);
    return;
  }

  // maybe it's already invulnerable/doing knockback
  if (!SpriteCanTakeDamage(spr))
  {
    return;
  }

  TryKnockback(spr, KNOCK_HARD, cause, NULL);
}

void CheckFellOffMap(Sprite *spr)
{

  Vec2 topleftPoint = GetPointOnSprite(spr, false, spr->anchorH, spr->anchorV);
  if (topleftPoint.y < MAP_HEIGHT_PIXELS + TILE_SIZE_PX)
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d Sprite %s fell off map at yPos %d\n", frameCounter, spr->name, spr->subPos.y);

  SpriteTakeDamage(spr, DMG_INSTAKILL, NULL, "Fell off map");
}

void ResetSprite(Sprite *spr)
{

  // cleared on unload
  spr->sentinel = true;

  spr->markedForDespawn = false;
  spr->spawnFrame = frameCounter;
  spr->indexer = globalIndexer;
  spr->dirIndexer = dirIndexer;

  // update other struct vals
  spr->facingRight = true;
  spr->wasRunningLastTimeWasOnGround = false;

  spr->subPos = ZeroVec();
  spr->lastSubPos = ZeroVec();

  spr->subVelo = ZeroVec();
  spr->lastSubVelo = ZeroVec();

  spr->subKnockbackAccel = ZeroVec();

  SetMoveMode(spr, MM_FALL, "resetsprite");

  spr->lastGroundedFrame = 0;
  spr->thingImRiding = NULL;
  spr->isGrounded = false;
  spr->isOnWall = false;
  spr->onGroundLastFrame = false;
  spr->onWallLastFrame = false;
  spr->inLiquid = false;
  spr->inLiquidLastFrame = false;
  spr->jumpFrameNum = 0;
  spr->dashFrameNum = 0;
  spr->knockbackFrameNum = 0;
  spr->invulnFrameNum = 0;

  spr->buttBounceFrameNum = 0;
  spr->numButtStomps = 0;
  spr->buttstompSubVelo = 0;

  spr->mustReleaseDash = false;
  spr->mustReleaseJump = false;

  // temporary config stuff
  spr->subPos = spr->subSpawnPos;
  spr->isPlayer = true;
  spr->anchorH = ANCHOR_VTOP;
  spr->anchorV = ANCHOR_HLEFT;

  spr->highestYSubPosInJump = 0;
  spr->lastgroundedYSubPos = 0;

  // Reset/create the movement profile
  CreateProfile(&spr->profile, spr->sType);

  // And apply anything that's determined from the profile
  spr->despawnTimer = spr->profile.default_despawnTimer;
  spr->health = spr->profile.default_health;
  spr->phys = spr->profile.physParams;
  spr->anims = spr->profile.defaultAnimGroup;
  spr->activeFrameSet = &spr->anims->idleFrames;
  spr->animIndex = 0;
  spr->animReversing = false;
  spr->lastGameframe = frameCounter;
  spr->animID = ANIMTYPE_IDLE;

  // update the hitbox, bounding box, etc
  OnSpriteMoved(spr);
}

// TODO: let's move this to a table
// Some stuff is spawnable: doors, players, mobs, etc
// Some stuff isn't: e.g. markers to denote room boundaries
bool IsTypeSpawnable(SpriteType inType)
{

  if (inType >= STYPE_MAX)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Sprite type %d is beyond max valid value!", (int)inType);
    return false;
  }

  // could've handled this better, my bad
  if (inType >= STYPE_PLAYER && inType <= STYPE_DOOR)
  {
    return true;
  }

  if (inType >= STYPE_GREENDUCK || inType == STYPE_REDDUCK)
  {
    return true;
  }

  if (inType == STYPE_PARTICLE_BROWN)
  {
    return true;
  }

  if (inType == STYPE_PLATFORM_0)
  {
    return true;
  }

  return false;
}

// TODO: rename to reflect spawn usage
void HandleNonSpawnableSpriteType(SpriteType inType, Vec2 worldStartPos)
{

  switch (inType)
  {

  case STYPE_DIRECTION_0_H:
  case STYPE_DIRECTION_1_V:
  case STYPE_DIRECTION_2_D:
  case STYPE_DIRECTION_3_NONE:
    dirIndexer = (int)inType - STYPE_DIRECTION_0_H;
    break;

  // set the global indexer to pass vals to the spawned sprite
  case STYPE_INDEXER_0:
  case STYPE_INDEXER_1:
  case STYPE_INDEXER_2:
  case STYPE_INDEXER_3:
  case STYPE_INDEXER_4:
  case STYPE_INDEXER_5:
  case STYPE_INDEXER_6:
    // so we can have 2 params
    // e.g. a direction and distance
    globalIndexer = (int)inType - STYPE_INDEXER_0;
    break;

  case STYPE_ROOM_MARKER:

    int markerIndex = globalIndexer;

    vmupro_log(VMUPRO_LOG_INFO, TAG, "Add room position indicator: %d, %d idx %d", worldStartPos.x, worldStartPos.y, markerIndex);

    bool isBottomRight = hasTopLeftRoomPosition[markerIndex];

    // One for top left, one for bottom right, but we somehow have like 3 or more
    if (hasBottomRightRoomPositions[markerIndex])
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Mismatched room marker on index: %d", markerIndex);
      return;
    }

    if (!isBottomRight)
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "(set to top left");
      // top left room marker
      roomTopLeftPositions[markerIndex] = worldStartPos;
      hasTopLeftRoomPosition[markerIndex] = true;
    }
    else
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "(set to bottom right");
      AddVecInts(&worldStartPos, TILE_SIZE_PX, TILE_SIZE_PX);
      roomBottomRightPositions[markerIndex] = worldStartPos;
      hasBottomRightRoomPositions[markerIndex] = true;
    }

    break;

  default:
    vmupro_log(VMUPRO_LOG_WARN, TAG, "Unhandled spawn special sprite type %d", inType);
    break;
  }
}

void AddToSpriteList(Sprite *inSprite)
{

  if (inSprite->profile.iMask & IMASK_DRAW_FIRST)
  {
    // shuffle every sprite up by one
    numSprites++;
    for (int i = numSprites; i > 0; i--)
    {
      sprites[i] = sprites[i - 1];
    }
    sprites[0] = inSprite;
  }
  else
  {
    sprites[numSprites] = inSprite;
    numSprites++;
  }
}

Sprite *CreateSprite(SpriteType inType, Vec2 worldStartPos, const char *inName)
{

  if (numSprites == MAX_SPRITES)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Out of sprite slots");
    return NULL;
  }

  if (inType < 0 || inType >= STYPE_MAX)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite index %d is invalid!", inType);
    return NULL;
  }

  if (!IsTypeSpawnable(inType))
  {
    HandleNonSpawnableSpriteType(inType, worldStartPos);
    return NULL;
  }

  //
  // Initial config
  //

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Spawning sprite %s of type %d\n", frameCounter, inName, (int)inType);
  Sprite *returnVal = malloc(sizeof(Sprite));
  memset(returnVal, 0, sizeof(Sprite));

  returnVal->sType = inType;

  Vec2 subSpawnPos = World2Sub(&worldStartPos);
  returnVal->subSpawnPos = subSpawnPos;

  int inNameLen = strlen(inName);
  const int maxLen = sizeof(returnVal->name) - 1;
  if (inNameLen > maxLen)
  {
    inNameLen = maxLen;
  }
  memset(returnVal->name, 0, sizeof(returnVal->name));
  memcpy(returnVal->name, inName, inNameLen);

  //
  // Runtime stuff, which may be reset
  //

  ResetSprite(returnVal);

  //
  // Finally, assign it.
  //

  AddToSpriteList(returnVal);

  if (inType == STYPE_PLAYER)
  {
    player = returnVal;
  }

  return returnVal;
}

// let's not cache this, 'cause it's not
// 1985 and we don't want stale ref issues
// -1 if not found
int GetIndexOfSprite(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Attempt to get index of null sprite");
    return -1;
  }

  int myIndex = -1;
  for (int i = 0; i < numSprites; i++)
  {
    Sprite *check = sprites[i];
    if (check == NULL)
    {
      continue;
    }
    if (check == spr)
    {
      myIndex = i;
      break;
    }
  }

  return myIndex;
}

// TODO: placeholder for anything malloced
// returns sprite's old index, incase you want
// to clean up the array after
int UnloadSprite(Sprite *spr)
{
  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Attempt to unload a null sprite!");
    return -1;
  }

  int myIndex = GetIndexOfSprite(spr);

  // couldn't find the index of this sprite in the sprite list
  // got a logic bug somewhere
  if (myIndex == -1)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d Sprite %s is NOT in the sprite list.", frameCounter, spr->name);
    return -1;
  }

  vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d Sprite %s unloading", frameCounter, spr->name);
  // wipe the mem before freeing it
  memset(spr, 0, sizeof(Sprite));
  free(spr);
  return myIndex;
}

void UnloadSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    UnloadSprite(spr);
    sprites[i] = NULL;
  }
  numSprites = 0;
}

void UnloadRoomMarkers()
{

  memset(roomTopLeftPositions, 0, sizeof(roomTopLeftPositions) / sizeof(roomTopLeftPositions[0]));
  memset(roomBottomRightPositions, 0, sizeof(roomBottomRightPositions) / sizeof(roomBottomRightPositions[0]));
  memset(hasTopLeftRoomPosition, 0, sizeof(hasTopLeftRoomPosition) / sizeof(hasTopLeftRoomPosition[0]));
  memset(hasBottomRightRoomPositions, 0, sizeof(hasBottomRightRoomPositions) / sizeof(hasBottomRightRoomPositions[0]));
}

// - Unload sprites
// - clear the player pointer
// - restore default blocks
void UnloadLevel()
{

  if (currentLevel == NULL)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "No level loaded, skipping unload...");
    return;
  }

  UnloadTileLayers();

  pData.levelNum = -1;

  // unloaded via sprite list
  player = NULL;

  UnloadSprites();
  UnloadRoomMarkers();

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Unloaded level & sprites");
}

// Spawn the player and other sprites from the third layer
// this one's sparsely encoded so doesn't need compressed
void ReadSpawnLayer(Level *inLevel, PersistentData *inData)
{

  uint16_t *sparseData = (uint16_t *)inLevel->sparseSpawnData->compressedData;

  // switched to 16 bit, so half the length
  int readLen = inLevel->sparseSpawnData->rawSize / 2;

  for (int i = 0; i < readLen; i += 3)
  {

    uint16_t xPos = sparseData[i + 0];
    uint16_t yPos = sparseData[i + 1];
    int id = sparseData[i + 2];

    // we need to adjust the block id a bit
    // starting from the 13th row of the tilemap
    // we'll go 0, 1, 2, corresponding to sprite type IDs

    int startBlock = TILEMAP_WIDTH_TILES * TILEMAP_SPAWNDATA_ROW_13;
    int newID = id - startBlock;
    if (newID < 0)
    {
      // something else from the tilemap has snuck into this layer
      // i.e. something that isn't a spawn marker
      vmupro_log(VMUPRO_LOG_ERROR, TAG, "Encountered block ID %d/%d, BELOW the max sprite ID range", id, newID);
      return;
    }
    if (newID > 48)
    {
      // there are 48 possible positions for sprites
      // at the bottom of the tilemap
      // we could calc this magic number, but i think
      // this description clears it up better
      vmupro_log(VMUPRO_LOG_ERROR, TAG, "Encountered block ID %d/%d, BEYOND the max sprite ID range", id, newID);
      return;
    }

    Vec2 worldPos = {xPos * TILE_SIZE_PX, yPos * TILE_SIZE_PX};

    vmupro_log(VMUPRO_LOG_INFO, TAG, "Found spawn marker block ID %d at tile: %ld, %ld world: %d, %d \n", newID, xPos, yPos, worldPos.x, worldPos.y);

    const char *tag = newID == STYPE_PLAYER ? "Player" : "Mob";
    CreateSprite(newID, worldPos, tag);
  }
}

void LoadLevel(int inLevelNum)
{

  UnloadLevel();

  // TODO: bounds checking?
  // but then we wouldn't get minus worlds.
  currentLevel = allLevels[inLevelNum];
  frameCounter = 0;
  globalIndexer = 0;

  pData.levelNum = inLevelNum;
  DecompressAllTileLayers(currentLevel, &pData);
  ReadSpawnLayer(currentLevel, &pData);
}

void InitPersistentData()
{
  memset(&pData, 0, sizeof(PersistentData));
  pData.lifeCount = DEFAULT_LIFE_COUNT;
  pData.levelNum = 0;
}

void PrintBytes(const char *tag, uint8_t *bytes, uint32_t len)
{

  printf("Bytes (%s): ", tag);
  for (int i = 0; i < len; i++)
  {
    printf("0x%02x ", (uint8_t)bytes[i]);
  }
  printf("\n");
}

bool RLE16BitDecode(const uint8_t *inBytes, uint32_t inLength, uint8_t *outBytes, uint32_t outLength)
{

  uint32_t writePos = 0;

  for (int i = 0; i < inLength; i += 3)
  {

    uint8_t runLength = inBytes[i];
    uint16_t pix = inBytes[i + 2] << 8 | inBytes[i + 1];

    for (int j = 0; j < runLength; j++)
    {
      if (writePos >= outLength)
      {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Writing beyond 16 bit decompression length");
        return false;
      }
      outBytes[writePos + 0] = pix & 0xFF;
      outBytes[writePos + 1] = (pix >> 8) & 0xFF;
      writePos += 2;
    }
  }

  uint32_t bytesWritten = writePos;

  if (bytesWritten != outLength)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Wrote %ld bytes, but expected to write %ld", bytesWritten, outLength);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "...Decompressed %ld bytes to %ld", inLength, bytesWritten);
  }

  return false;
}

bool RLE8BitDecode(const uint8_t *inBytes, uint32_t inLength, uint8_t *outBytes, uint32_t outLength)
{

  uint32_t writePos = 0;

  for (int i = 0; i < inLength; i += 2)
  {

    uint8_t runLength = inBytes[i];
    uint16_t pix = inBytes[i + 1];

    for (int j = 0; j < runLength; j++)
    {
      if (writePos >= outLength)
      {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Writing beyond 16 bit decompression length");
        return false;
      }
      outBytes[writePos + 0] = pix & 0xFF;
      writePos += 1;
    }
  }

  uint32_t bytesWritten = writePos;

  if (bytesWritten != outLength)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Wrote %ld bytes, but expected to write %ld", bytesWritten, outLength);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "...Decompressed %ld bytes to %ld", inLength, bytesWritten);
  }

  return false;
}

void DecompressImage(const Img *img)
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Decompressing Image '%s'...", img->name);

  int index = img->index;
  int rawSize = img->rawSize;

  uint8_t *newData = malloc(rawSize);
  decompressedImageDataTable[index] = newData;
  if (newData == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Failed to allocate %ld bytes!", rawSize);
    return;
  }

  RLE16BitDecode(img->compressedData, img->compressedSize, newData, rawSize);

  uint32_t checksumCalced = CalcDJB2((uint8_t *)newData, rawSize);
  uint32_t checksumExpected = img->rawChecksum;

  if (checksumCalced != checksumExpected)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Decompressed Img %s expected checksum: 0x%lx, calced checksum: %lx", img->name, checksumCalced, checksumExpected);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Img %s checksum: 0x%lx (success)", img->name, checksumCalced, checksumExpected);
  }
}

// Return the decompressed img data
// from a runtime-generated table
// involves a little pointer hopping
uint8_t *ImgData(const Img *img)
{

  int index = img->index;
  return decompressedImageDataTable[index];
}

void DecompressAllImages()
{
  if (didDecompressImages)
  {
    return;
  }
  didDecompressImages = true;
  memset(decompressedImageDataTable, 0, sizeof(uint8_t *) * allImagesLength);

  for (int i = 0; i < allImagesLength; i++)
  {
    const Img *img = allImages[i];
    DecompressImage(img);
  }
}

// used for layer 0 and 1 - background and collision
// which are RLE encoded
// the spawn layer is sparesely encoded
uint8_t *DecompressTileLayer(const TileLayer *inLayer)
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Decompressing tile layer: %s");

  uint32_t rawSize = inLayer->rawSize;
  uint8_t *newData = malloc(rawSize);
  RLE8BitDecode(inLayer->compressedData, inLayer->compressedSize, newData, rawSize);

  uint32_t checksumExpected = inLayer->rawChecksum;
  uint32_t checksumCalced = CalcDJB2(newData, rawSize);

  if (checksumCalced != checksumExpected)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Decompressed TileLayer %s expected checksum: 0x%lx, calced checksum: %lx", inLayer->name, checksumCalced, checksumExpected);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "TileLayer %s checksum: 0x%lx (success)", inLayer->name, checksumCalced, checksumExpected);
  }

  return newData;
}

// Only required for layers 0 & 1 (background & collision)
void UnloadTileLayers()
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Unloading tile layers...");
  for (int i = 0; i < MAX_DECOMPRESSED_TILE_LAYERS; i++)
  {
    uint8_t *tlData = decompressedTileLayerTable[i];
    if (tlData == NULL)
      continue;
    free(tlData);
  }
  hasDecompressedTileLayers = false;

  // wipe the decompressed tile list
  memset(decompressedTileLayerTable, 0, sizeof(uint8_t *) * MAX_DECOMPRESSED_TILE_LAYERS);
}

void DecompressAllTileLayers(Level *inLevel, PersistentData *inData)
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Decompressing tile layers for level num %d / %s...", inData->levelNum, currentLevel->name);

  if (didDecompressImages)
  {
    UnloadTileLayers();
  }
  didDecompressImages = true;

  // Background
  decompressedTileLayerTable[0] = DecompressTileLayer(inLevel->bgLayer);
  // Collision
  decompressedTileLayerTable[1] = DecompressTileLayer(inLevel->colLayer);
  // Spawn layer:
  // spawned as part of the level load routine
}

void InitGame()
{
  DecompressAllImages();
  InitPersistentData();
  LoadLevel(0);
  GotoGameState(GSTATE_START);
}

// Retry current level, minus a life
void Retry()
{

  LoadLevel(pData.levelNum);
  pData.lifeCount--;
  GotoGameState(GSTATE_INGAME);
}

uint8_t *GetTileLayerData(int layer)
{
  uint8_t *rVal = (layer == 0) ? decompressedTileLayerTable[0] : decompressedTileLayerTable[1];
  return rVal;
}

// returns atlas block 0-max
// note: the .map file uses 0x00 for blank spots
// so we'll always sub 1 to get a 0-indexed
// value into our spritesheet/atlas
uint32_t GetBlockIDAtColRow(int blockCol, int blockRow, int layer)
{

  // TODO: subscreens
  uint8_t *tileData = GetTileLayerData(layer);

  if (tileData == NULL)
    return BLOCK_NULL;

  // bounds check the input values
  if (blockCol < 0 || blockRow < 0)
  {
    return BLOCK_NULL;
  }

  int widthInTiles = currentLevel->bgLayer->width;
  int heightInTiles = currentLevel->bgLayer->height;

  if (blockCol >= widthInTiles || blockRow >= heightInTiles)
  {
    return BLOCK_NULL;
  }

  uint32_t offset = (blockRow * widthInTiles) + blockCol;

  uint32_t block = tileData[offset];
  return block;
}

void UpdatePatrollInputs(Sprite *spr, bool ignorePlayer)
{

  // might lose ground and bonk on the same frame
  // don't want to trigger both
  bool originallyFacingRight = spr->facingRight;

  // 1: check if we're going to run out of ground

  // offset a bit based on where we're going
  // a tile should do, since it gives time
  // to dampen and change direction smoothly
  Vec2 subOffset = {originallyFacingRight ? TILE_SIZE_SUB : -TILE_SIZE_SUB, 1};
  bool groundAhead = CheckSpriteCollision(spr, DIR_DOWN, &subOffset, "patrol_ground", true) != SOLIDMASK_NONE;

  // 2: check if we'd bonk into something

  Direction dir = spr->facingRight ? DIR_RIGHT : DIR_LEFT;
  Vec2 wallSubOffset = {dir == DIR_RIGHT ? TILE_SIZE_SUB / 2 : -TILE_SIZE_SUB / 2, 0};
  bool bonk = CheckSpriteCollision(spr, dir, &wallSubOffset, "patrol_wall", ignorePlayer) != SOLIDMASK_NONE;

  // Turn around
  if (bonk || !groundAhead)
  {
    spr->facingRight = !originallyFacingRight;
  }

  // Apply the new values

  spr->input.right = spr->facingRight;
  spr->input.left = !spr->facingRight;
}

bool AllowSpriteInput(Sprite *spr)
{

  if (spr->profile.iMask & IMASK_SKIP_INPUT)
  {
    return false;
  }

  if (SpriteIsDead(spr))
  {
    return false;
  }

  if (SpriteIsKnockback(spr))
  {
    return false;
  }

  if (SpriteStunned(spr))
  {
    return false;
  }

  switch (gState)
  {

  // other sprites will continue in these state
  case GSTATE_START:
  case GSTATE_DED:
  case GSTATE_GAMEOVER:
    return (spr != player);
    break;

  case GSTATE_INGAME:
    return true;
    break;

  case GSTATE_UNINIT:
  case GSTATE_INTROFADE:
  case GSTATE_PAUSED:
  case GSTATE_TRANSITION:
    return false;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled game state %d in AllowSpriteInput", (int)gState);
    // don't jam up the game 'cause of an error
    return true;
  }
}

void ClearSpriteInputs(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to ClearSpriteInputs");
    return;
  }
  memset(&spr->input, 0, sizeof(Inputs));
}

void UpdateSpriteInputs(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to UpdateSpriteInputs");
    return;
  }

  // if we're not in a playable state
  // just early exit since the inputs are wiped
  if (!AllowSpriteInput(spr))
  {
    // wipe and exit
    ClearSpriteInputs(spr);
    return;
  }

  if (spr->profile.iMask & IMASK_SKIP_MOVEMENT)
  {
    ClearSpriteInputs(spr);
    return;
  }

  if (spr->sType == STYPE_PLAYER)
  {
    // other sprites can continue while the player's dead
    // but the player shouldn't walk about doing stuff
    if (gState != GSTATE_INGAME)
    {
      return;
    }

    Inputs *inp = &spr->input;
    vmupro_btn_read();

    // Could use the bools directly, but it feels wrong
    inp->up = vmupro_btn_held(DPad_Up) ? inp->up + 1 : 0;
    inp->down = vmupro_btn_held(DPad_Down) ? inp->down + 1 : 0;
    inp->left = vmupro_btn_held(DPad_Left) ? inp->left + 1 : 0;
    inp->right = vmupro_btn_held(DPad_Right) ? inp->right + 1 : 0;
    inp->jump = vmupro_btn_held(Btn_B) ? inp->jump + 1 : 0;
    inp->run = vmupro_btn_held(Btn_A) ? inp->run + 1 : 0;
  }
  else if (spr->sType == STYPE_GREENDUCK)
  {
    ClearSpriteInputs(spr);
    UpdatePatrollInputs(spr, false);
  }
  else if (spr->sType == STYPE_REDDUCK)
  {
    ClearSpriteInputs(spr);
    UpdatePatrollInputs(spr, true);
  }
  else if (spr->sType == STYPE_PLATFORM_0)
  {
    ClearSpriteInputs(spr);
    // UpdatePatrollInputs(spr, true);
    Inputs *inp = &spr->input;

    if (spr->dirIndexer == DIRINDEX_HORZ)
    {

      bool pressRight = false;
      bool pressLeft = false;

      // horz movement
      if (spr->facingRight)
      {
        // how far can it travel in world coords
        int maxDist = (spr->indexer + 1) * 4;
        maxDist *= TILE_SIZE_SUB;

        int maxRight = spr->subSpawnPos.x + maxDist;
        if (spr->subPos.x >= maxRight)
        {
          spr->facingRight = false;
        }
        else
        {
          pressRight = true;
        }
      }
      else
      {

        if (spr->subPos.x <= spr->subSpawnPos.x)
        {
          spr->facingRight = true;
        }
        else
        {
          pressLeft = true;
        }
      }

      inp->right = pressRight ? inp->right + 1 : 0;
      inp->left = pressLeft ? inp->left + 1 : 0;
    }

    if (spr->dirIndexer == DIRINDEX_VERT)
    {

      bool pressDown = false;
      bool pressUp = false;

      // horz movement
      if (spr->facingRight)
      {
        // how far can it travel in world coords
        int maxDist = (spr->indexer + 1) * 4;
        maxDist *= TILE_SIZE_SUB;

        int maxRight = spr->subSpawnPos.y + maxDist;
        if (spr->subPos.y >= maxRight)
        {
          spr->facingRight = false;
        }
        else
        {
          pressDown = true;
        }
      }
      else
      {
        if (spr->subPos.y <= spr->subSpawnPos.y)
        {
          spr->facingRight = true;
        }
        else
        {
          pressUp = true;
        }
      }

      inp->down = pressDown ? inp->down + 1 : 0;
      inp->up = pressUp ? inp->up + 1 : 0;
    }
  }
  else
  {
    // Zero out the inputs
    ClearSpriteInputs(spr);
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled sprite type %d in UpdateSpriteInputs", spr->sType);
    return;
  }
}

void InputAllSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    if (i > 0 && DEBUG_ONLY_MOVE_PLAYER)
      continue;
    Sprite *spr = sprites[i];
    UpdateSpriteInputs(spr);
  }
}

bool IsBlockOneWay(int blockId)
{

  int rowNum = blockId / TILEMAP_WIDTH_TILES;
  int colNum = blockId % TILEMAP_WIDTH_TILES;

  // ignore smashables, liquid, etc
  if (colNum >= TILEMAP_SMASHABLE_COL_13)
  {
    return false;
  }

  // Top row of the tilemap are one-way platforms
  if (rowNum == TILEMAP_ONEWAY_PLATFORM_ROW_0)
  {
    return true;
  }

  // as is row 11
  if (rowNum == TILEMAP_ONEWAY_PLATFORM_ROW_11)
  {
    return true;
  }

  return false;
}

bool IsBlockTransparent(int blockID)
{

  int rowNum = blockID / TILEMAP_WIDTH_TILES;

  if (rowNum == TILEMAP_TRANSPARENT_ROW_11 || rowNum == TILEMAP_TRANSPARENT_ROW_12)
  {
    return true;
  }
  return false;
}

void DrawLevelBlock(int inBlockCol, int inBlockRow, int layer)
{

  uint8_t *tileData = GetTileLayerData(layer);

  if (tileData == NULL)
    return;

  uint32_t blockId = GetBlockIDAtColRow(inBlockCol, inBlockRow, layer);

  if (blockId == BLOCK_NULL)
  {
    return;
  }

  uint32_t pixSrcX = (blockId % TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;
  uint32_t pixSrcY = (blockId / TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;

  uint32_t pixTargX = inBlockCol * TILE_SIZE_PX;
  uint32_t pixTargY = inBlockRow * TILE_SIZE_PX;

  const Img *sheet = &img_tilemap;
  vmupro_drawflags_t flags = VMUPRO_DRAWFLAGS_NORMAL;
  vmupro_color_t transColor = VMUPRO_COLOR_BLACK;

  uint8_t *imgData = ImgData(sheet);

  bool trans = IsBlockTransparent(blockId);

  // bit of a hack, but hey, everything on rowss 11 and 12 are transparent
  if (trans)
  {

    vmupro_blit_tile_advanced(imgData, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width, transColor, flags);
  }
  else
  {
    vmupro_blit_tile(imgData, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width);
  }
}

void DrawCamScrollZone()
{
  // c++ wouldn't make me do this :(
  BBox scrollBox = {{{scrollZoneWorldX, camY, SCROLLZONE_WIDTH - 1, SCREEN_HEIGHT - 1}}};
  DrawBBoxWorld(&scrollBox, VMUPRO_COLOR_WHITE);
}

// Return the bounds of the current room, based on the top left/bottom right
// if none is found, default to the whole map
BBox GetRoomBoundsWorld(Vec2 *playerWorldPos)
{

  BBox rVal;
  rVal.x = 0;
  rVal.y = 0;
  rVal.width = 256;
  rVal.height = 256;

  if (currentLevel == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "No level to find room bounds!");
    rVal.width = 256;
    rVal.height = 256;
    return rVal;
  }

  int foundIndex = -1;
  for (int i = 0; i < MAX_ROOMS; i++)
  {
    // this one isn't filled in
    if (!hasBottomRightRoomPositions[i])
      continue;

    Vec2 *tl = &roomTopLeftPositions[i];
    Vec2 *br = &roomBottomRightPositions[i];

    // printf("Checking index %d of %d  for xrange: %d-%d yrange: %d %d\n", i, MAX_ROOMS, tl->x, br->x, tl->y, br->y);

    if (playerWorldPos->x < tl->x)
      continue;
    if (playerWorldPos->y < tl->y)
      continue;
    if (playerWorldPos->x > br->x)
      continue;
    if (playerWorldPos->y > br->y)
      continue;

    foundIndex = i;
    break;
  }

  if (foundIndex == -1)
  {
    // not inside a room, throw a warning and just use the whole level
    rVal.width = currentLevel->bgLayer->width * TILE_SIZE_PX;
    rVal.height = currentLevel->bgLayer->height * TILE_SIZE_PX;
    // vmupro_log(VMUPRO_LOG_WARN, TAG, "Couldn't match a room index, free cam!");
    return rVal;
  }

  // inside a room, use that
  Vec2 *tl = &roomTopLeftPositions[foundIndex];
  Vec2 *br = &roomBottomRightPositions[foundIndex];

  rVal.x = tl->x;
  rVal.y = tl->y;
  rVal.width = br->x - tl->x;
  rVal.height = br->y - tl->y;
  return rVal;
}

// center the camera on the player
void SolveCamera()
{

  Sprite *spr = player;
  Vec2 playerWorldPos = GetPointOnSprite(spr, false, ANCHOR_HMID, ANCHOR_VMID);

  //
  // Make a box that moves right when the player touches the right edge
  // and moves left when you touch the left edge
  // i.e. it doesn't move if you're walking about inside it
  // but the left or right edge will always follow the player
  //
  // while we're on the left or right edge, we'll add a little
  // offset (up to a max) so the cam can show a little extra
  // to the left or right as we move
  // TLDR; you can see further in front of you than behind
  //

  Vec2 snapWorldPos = playerWorldPos;

  bool onRightEdge = playerWorldPos.x > scrollZoneWorldX + SCROLLZONE_WIDTH;
  bool onLeftEdge = playerWorldPos.x < scrollZoneWorldX;

  if (onRightEdge)
  {
    scrollZoneWorldX = playerWorldPos.x - SCROLLZONE_WIDTH;
    if (scrollzoneOffsetX > -SCROLLZONE_MAXOFFSET)
      scrollzoneOffsetX -= SCROLLZONE_SPEED;
  }

  if (onLeftEdge)
  {
    scrollZoneWorldX = playerWorldPos.x;
    if (scrollzoneOffsetX < SCROLLZONE_MAXOFFSET)
      scrollzoneOffsetX += SCROLLZONE_SPEED;
  }

  snapWorldPos.x = scrollZoneWorldX + (SCROLLZONE_WIDTH / 2);
  snapWorldPos.x -= (SCREEN_WIDTH / 2);
  snapWorldPos.x -= scrollzoneOffsetX;

  //
  // Bounds Check
  //

  BBox roomBounds = GetRoomBoundsWorld(&playerWorldPos);

  // check if cam's going off left of the level
  int camLeft = snapWorldPos.x;
  if (camLeft < roomBounds.x)
  {
    camLeft = roomBounds.x;
  }

  // check if cam's going off right of the level
  int camRight = camLeft + SCREEN_WIDTH;
  int roomRight = roomBounds.x + roomBounds.width;
  if (camRight >= roomRight)
  {
    int howFarOver = camRight - roomRight;
    camLeft -= howFarOver;
  }

  // check if cam's going off the top of the level
  // player's about 3/4 of the way down the screen
  int camTop = snapWorldPos.y - (SCREEN_WIDTH / 2);
  if (camTop < roomBounds.y)
  {
    camTop = roomBounds.y;
  }

  // check if cam's going off the bottom of the level
  int camBottom = camTop + SCREEN_HEIGHT;
  int roomBottom = roomBounds.y + roomBounds.height;
  if (camBottom >= roomBottom)
  {
    int howFarOver = camBottom - roomBottom;
    camTop -= howFarOver;
  }

  camX = camLeft;
  camY = camTop;
}

void DrawBackground()
{

  const Img *img = &img_bg_0;

  int bgScrollX = (camX * 4) / 5;
  int bgScrollY = (camY * 4) / 5;

  vmupro_blit_scrolling_background(ImgData(img), img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DrawGroundtiles(int layer)
{

  // work out the x/y range based on camera pos
  // the screen (240px) can hadle 15 and a bit 16px tiles
  // so we'll always draw 17 to allow clean scrolling
  // this is a good point for optimisation

  int leftTileIndex = camX / TILE_SIZE_PX;
  // wrapping, draw an extra tile
  leftTileIndex -= 1;

  int topTileIndex = camY / TILE_SIZE_PX;
  // wrapping, draw an extra tile
  topTileIndex -= 1;

  for (int y = 0; y < 17; y++)
  {
    int realYTile = y + topTileIndex;
    for (int x = 0; x < 17; x++)
    {
      int realXTile = x + leftTileIndex;
      DrawLevelBlock(realXTile, realYTile, layer);
    }
  }
}

// store the last abs pos vs current
// do it at the end of the frame in case
// the pos is updated multiple times in a frame
// (collision, etc)
void EndFrameSprite(Sprite *inSpr)
{
  inSpr->lastSubPos = inSpr->subPos;
  inSpr->lastSubVelo = inSpr->subVelo;
  inSpr->onGroundLastFrame = inSpr->isGrounded;
  inSpr->inLiquidLastFrame = inSpr->inLiquid;
  inSpr->onWallLastFrame = inSpr->isOnWall;
}

void EndFrameAllSprites()
{
  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    EndFrameSprite(spr);
  }
}

int GetYSubAccel(Sprite *spr)
{

  int returnVal = spr->isGrounded ? 0 : spr->phys->sub_gravity;

  if (spr->inLiquid)
  {
    returnVal /= 2;
  }

  if (SpriteIsButtDashing(spr))
  {
    returnVal *= 2;
  }

  return returnVal;
}

int GetXSubAccel(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  const PhysParams *prof = spr->phys;

  int returnVal;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Accel: Unhandled movement mode: %d", (int)mMode);
    returnVal = prof->subaccel_walk;
    break;

  case MM_STUNNED:
    returnVal = 0;
    break;
  case MM_JUMP:
  case MM_KNOCKBACK:
  case MM_BUTTDASH:
  case MM_BUTTBOUNCE:
  case MM_FALL:
    returnVal = prof->subaccel_air;
    break;
  case MM_WALK:
    returnVal = prof->subaccel_walk;
    break;
  case MM_DASH:
    returnVal = prof->subaccel_run;
    break;
  }

  if (spr->inLiquid)
  {
    returnVal /= 2;
  }

  return returnVal;
}

int GetMaxXSubSpeed(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  const PhysParams *prof = spr->phys;
  bool wasRunningWhenLastGrounded = spr->wasRunningLastTimeWasOnGround;

  int returnVal = 0;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Subspeed: Unhandled movement mode: %d", (int)mMode);
    returnVal = prof->max_subspeed_walk;
    break;

  case MM_STUNNED:
    returnVal = 120;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      returnVal = prof->max_subspeed_run;
    }
    else
    {
      returnVal = prof->max_subspeed_walk;
    }

    break;

  case MM_BUTTDASH:
  case MM_BUTTBOUNCE:
  case MM_KNOCKBACK:
  case MM_WALK:
    returnVal = prof->max_subspeed_walk;
    break;

  case MM_DASH:
    returnVal = prof->max_subspeed_run;
    break;
  }

  if (spr->inLiquid)
  {
    returnVal /= 2;
  }

  return returnVal;
}

int GetMaxYSubSpeed(Sprite *spr)
{

  int returnVal = player->phys->max_subfallspeed;

  if (spr->inLiquid)
  {
    returnVal /= 2;
  }

  return returnVal;
}

int GetXDamping(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  const PhysParams *prof = spr->phys;
  bool wasRunningWhenLastGrounded = spr->wasRunningLastTimeWasOnGround;

  int returnVal = 0;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Damping: Unhandled movement mode: %d", (int)mMode);
    returnVal = prof->subdamping_walk;
    break;

  case MM_STUNNED:
    returnVal = prof->subdamping_stunned;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      returnVal = prof->subdamping_run;
    }
    else
    {
      returnVal = prof->subdamping_walk;
    }

    break;

  case MM_BUTTDASH:
  case MM_BUTTBOUNCE:
  case MM_KNOCKBACK:
  case MM_WALK:
    returnVal = prof->subdamping_walk;
    break;

  case MM_DASH:
    returnVal = prof->subdamping_run;
    break;
  }

  if (spr->inLiquid)
  {
    returnVal *= 2;
  }

  return returnVal;
}

// hitbox = false: return sprite box in world coords
// hitbox = true: return hitbox in subpixel coords
// note: returns a point INSIDE the hitbox, always
Vec2 GetPointOnSprite(Sprite *spr, bool hitBox, Anchor_H anchorH, Anchor_V anchorV)
{

  // TODO: switch to an actual hitbox

  int returnX = 0;
  int returnY = 0;

  BBox *aabb = hitBox ? &spr->subHitBox : &spr->worldBBox;

  // quick explainer on hitbox heights & widths
  // let's say our player's pos is the top left
  // it's 16 px tall, and tiles are 16px tall
  // there's a row of ground at y=16
  // our player is sat at y=0
  // if we did y=0 + height=16, we'd get y = 16, which would be *inside* the ground
  // but what we really need is 0-15, since that's 16 values
  // likewise, horizontally, the character occupies pixels 0-15, not 0-16
  // TLDR; BOTTOM = last pixel inside the box, vertically
  ///      RIGHT  = last pixel inside the box, horizontally
  //  ____
  // |    |
  // |    |  <-- hitbox @ (0,0), with width/height (4,4)
  // |    |      the bottom right point is 3,3
  // |___X|

  if (anchorH == ANCHOR_HLEFT)
  {
    returnX = aabb->x;
  }
  else if (anchorH == ANCHOR_HMID)
  {
    returnX = aabb->x + (aabb->width / 2);
  }
  else if (anchorH == ANCHOR_HRIGHT)
  {
    returnX = aabb->x + aabb->width;
    // * see above
    returnX -= 1;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, "TAG", "Unknown H alignment");
  }

  if (anchorV == ANCHOR_VTOP)
  {
    returnY = aabb->y;
  }
  else if (anchorV == ANCHOR_VMID)
  {
    returnY = aabb->y + (aabb->height / 2);
  }
  else if (anchorV == ANCHOR_VBOTTOM)
  {
    returnY = aabb->y + aabb->height;
    // * see above
    returnY -= 1;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, "TAG", "Unknown V alignment");
  }

  Vec2 returnVal = {returnX, returnY};
  return returnVal;
}

// The struct's a little big, unwieldy, and underoptimised
// but i wanted to make something flexible with plenty
// of hit information should you need it
// it's not a bad place to target for performance improvements.
typedef struct
{

  // relative to self
  // e.g. which way are we ejecting, etc
  Direction whereWasCollision;
  bool ignorePlayer;

  // the points we'll look up
  // e.g. top row, bottom row, etc
  // used to work out the bounding box collision
  // check points
  Anchor_H anchorH[3];
  Anchor_V anchorV[3];

  // the point(s) we're checking for collision
  // e.g. for moving down we'd use 3:
  // bottom left, bottom middle, bottom right
  Vec2 subCheckPos[3];

  //
  // Block hit info
  //

  int lastBlockHitIndex; // 0-2

  // top left to bottom right
  // e.g. top left, top middle, top right
  // or top left, middle left, bottom left
  // -1 for nothing
  int blockID[3];

  // the ejection edge for the block we hit
  // e.g. if we're moving right, it'll be
  // - the x coord of the block's left edge
  // - the y coord of the hitbox point we're checking
  Vec2 blockSubEjectPoint[3];

  //
  // Sprite hit info:
  //

  int lastSpriteHitIndex; // 0-2

  // other sprites we might have hit
  Sprite *otherSprites[3];
  Vec2 spriteSubEjectPoint[3];
  Solidity spriteSolidity[3];

  //
  // Shared hit info (blocks/sprites)
  //

  // mask of e.g. | SOLID | ONE_WAY
  Solidity hitMask;
  // e.g. not a trigger
  bool hitMaskIsSolid;
  Vec2 snapPoint;

} HitInfo;

Vec2 GetTileRowAndColFromSubPos(Vec2 *subPos)
{

  Vec2 returnVal;
  // rounding down is not an issue here.
  // if the check pos is 257 for example, this shifts down to
  // worldPos = (257>>4) = 16
  // then
  // tileCol = 16 / TILE_SIZE_PX = 1
  // which is what we want
  // 255 would be tile col 0
  // e.g.
  // Vec2 worldPoint = Sub2World(subPos);
  // returnVal.x = worldPoint.x / TILE_SIZE_PX;
  // returnVal.y = worldPoint.y / TILE_SIZE_PX;
  // but we can simplify
  returnVal.x = subPos->x / TILE_SIZE_SUB;
  returnVal.y = subPos->y / TILE_SIZE_SUB;
  return returnVal;
}

int GetTileIndexFromSubPos(Vec2 *subPos)
{
  Vec2 vecVal = GetTileRowAndColFromSubPos(subPos);
  int scalarVal = (vecVal.y * currentLevel->bgLayer->width) + vecVal.x;
  return scalarVal;
}

Vec2 GetTileSubPosFromRowAndCol(Vec2 *rowAndcol)
{

  Vec2 returnVal;
  returnVal.x = rowAndcol->x * TILE_SIZE_SUB;
  returnVal.y = rowAndcol->y * TILE_SIZE_SUB;
  return returnVal;
}

bool IsTileIDBreakable(int blockID)
{

  int colNum = blockID % TILEMAP_WIDTH_TILES;
  if (colNum != TILEMAP_SMASHABLE_COL_13)
  {
    return false;
  }

  int rowNum = blockID / TILEMAP_WIDTH_TILES;
  if (rowNum >= TILEMAP_SPAWNDATA_ROW_13)
  {
    return false;
  }

  return true;
}

// right side of
bool IsTriggerTile(int blockID)
{

  // the last to cols are special cols
  int colNum = blockID % TILEMAP_WIDTH_TILES;
  if (colNum != TILEMAP_ANIMATED_TRIGGER_COL_14)
  {
    return false;
  }

  // except if it's spawndata tiles, ignore that.
  int rowNum = blockID / TILEMAP_WIDTH_TILES;
  if (rowNum >= TILEMAP_SPAWNDATA_ROW_13)
  {
    return false;
  }

  return true;
}

// Solid one-way platform or not?
bool IsOneWayPlatformSolid(int blockId, int layer, Sprite *spr, Vec2 *tileSubPos)
{

  // if it's not 2-sideable, just early exit
  if (!IsBlockOneWay(blockId))
    return true;

  bool movingUp = spr->subVelo.y < 0;
  // bool movingDown = spr->subVelo.y > 0;
  // bool movingHorz = spr->subVelo.x != 0;
  bool movingVert = spr->subVelo.y != 0;

  Vec2 subFootPos = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VBOTTOM);
  bool playerHigher = subFootPos.y < tileSubPos->y;

  // Walking along the top
  if (playerHigher && !movingVert)
  {
    // it's solid

    return true;
  }

  // else, we only care if moving down
  if (movingUp || !movingVert)
  {
    return false;
  }

  // so we're moving down
  // it's only solid as long as we've been higher than it during the last/current jump
  bool playerHasBeenHigher = spr->highestYSubPosInJump < tileSubPos->y;
  if (playerHasBeenHigher)
  {
    return true;
  }

  // else, we might be walking into it sideways
  // i.e. it's halfway up the sprite's body

  return false;
}

// ensure you're using the same sub/world/screen coords
bool IsPointInsideBox(Vec2 *point, BBox *box)
{

  if (point->x < box->x)
    return false;
  if (point->x > box->x + box->width)
    return false;
  if (point->y < box->y)
    return false;
  if (point->y > box->y + box->height)
    return false;
  return true;
}

bool SubPointInHitbox(Vec2 *subPoint, Sprite *otherSprite)
{
  return IsPointInsideBox(subPoint, &otherSprite->subHitBox);
}

bool IsBlockingCollision(Solidity inSolid)
{

  switch (inSolid)
  {
  case SOLIDMASK_NONE:
  case SOLIDMASK_TILE_TRIGGER:
  case SOLIDMASK_SPRITE_TRIGGER:
    return false;
    break;

  case SOLIDMASK_ONESIDED:
  case SOLIDMASK_PLATFORM:
  case SOLIDMASK_SPRITE_SOLID:
  case SOLIDMASK_TILE_SOLID:
    return true;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled solidity: %d\n", (int)inSolid);
    return false;
    break;
  }
}

typedef struct
{

  // the point(s) we're checking for collision
  // e.g. for moving down we'd use 3:
  // bottom left, bottom middle, bottom right
  Vec2 subCheckPos[3];

  // Did we hit any lava, death triggers, etc
  int lastTriggertileIndex; // 0-2
  int triggerTileID[3];

} TileTriggerInfo;

// check for lava, instadeath, water, etc
void HandleTileTriggers(Sprite *spr, TileTriggerInfo *info)
{

  if (spr == NULL || info == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null param passed to HandleTileTriggers");
    return;
  }
  int index = info->lastTriggertileIndex;

  if (index == -1)
    return;

  // Clear and we'll build up/merge damage, etc
  spr->inLiquid = false;
  int outDamage = 0;

  int tileID = info->triggerTileID[index];
  if (tileID == TILEMAP_LAVA_ID_14 || tileID == TILEMAP_LAVA_ID_30)
  {
    if (outDamage < DMG_TILEMAP_LAVA)
    {
      outDamage = DMG_TILEMAP_LAVA;
    }
    spr->inLiquid = true;
  }
  if (tileID == TILEMAP_WATER_ID_44 || tileID == TILEMAP_WATER_ID_60)
  {
    spr->inLiquid = true;
  }
  if (tileID == TILEMAP_INSTAKILL_ID_76)
  {
    outDamage = DMG_INSTAKILL;
  }

  // Any kinda damage?

  if (outDamage != 0)
  {
    SpriteTakeDamage(spr, outDamage, NULL, "World damage");
  }
}

void GetTileTriggerOverlaps(Sprite *spr, TileTriggerInfo *info)
{

  memset(info, 0, sizeof(TileTriggerInfo));

  info->subCheckPos[0] = GetPointOnSprite(spr, true, ANCHOR_HLEFT, ANCHOR_VBOTTOM);
  info->subCheckPos[1] = GetPointOnSprite(spr, true, ANCHOR_HRIGHT, ANCHOR_VBOTTOM);
  info->subCheckPos[2] = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VTOP);

  for (int i = 0; i < 3; i++)
  {
    info->triggerTileID[i] = BLOCK_NULL;

    Vec2 tileRowAndCol = GetTileRowAndColFromSubPos(&info->subCheckPos[i]);
    int blockId = GetBlockIDAtColRow(tileRowAndCol.x, tileRowAndCol.y, LAYER_COLS);
    if (blockId == BLOCK_NULL)
      continue;

    bool isTriggerTile = IsTriggerTile(blockId);
    if (!isTriggerTile)
      return;

    info->triggerTileID[i] = blockId;
    info->lastTriggertileIndex = i;
  }
  HandleTileTriggers(spr, info);
}

Sprite *GetSpriteTriggerOverlaps(Sprite *srcSprite)
{

  if (srcSprite == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to GetSpriteTriggerOverlaps");
    return NULL;
  }

  for (int i = 0; i < numSprites; i++)
  {

    Sprite *otherSprite = sprites[i];

    if (otherSprite == NULL)
      continue;

    if (otherSprite == srcSprite)
      continue;

    if (otherSprite->profile.solid != SOLIDMASK_SPRITE_TRIGGER)
      continue;

    Vec2 *srcSubPos = &srcSprite->subPos;

    bool inside = IsPointInsideBox(srcSubPos, &otherSprite->subHitBox);

    if (inside)
    {
      return otherSprite;
    }
  }

  return NULL;
}

//
// The basic collision check, without any ejection routine
// useful e.g.
// - to know if something is ahead
// - to know if you're on the ground
//
// Note: doesn't find trigger type overlaps
//       since that would interfere with collisions
//       and this func is completely overkill for that
//
// Note:
// subOffsetOrNull adds an offset to where we check for collisions
// e.g. when moving right we check currentPos.x + velo.x
// for where we'll be, rather than where we are, or for a lookahead
//
void GetHitInfo(HitInfo *rVal, Sprite *spr, Direction dir, Vec2 *subOffsetOrNull, const char *src)
{

  rVal->whereWasCollision = dir;
  rVal->lastBlockHitIndex = -1;
  rVal->lastSpriteHitIndex = -1;

  // get a list of points to check for
  // whatever direction we're moving
  // TODO: could be optimised
  switch (dir)
  {
  case DIR_UP:
    // top row
    rVal->anchorH[0] = ANCHOR_HLEFT;
    rVal->anchorH[1] = ANCHOR_HMID;
    rVal->anchorH[2] = ANCHOR_HRIGHT;
    rVal->anchorV[0] = ANCHOR_VTOP;
    rVal->anchorV[1] = ANCHOR_VTOP;
    rVal->anchorV[2] = ANCHOR_VTOP;
    break;

  case DIR_RIGHT:
    rVal->anchorH[0] = ANCHOR_HRIGHT;
    rVal->anchorH[1] = ANCHOR_HRIGHT;
    rVal->anchorH[2] = ANCHOR_HRIGHT;
    rVal->anchorV[0] = ANCHOR_VTOP;
    rVal->anchorV[1] = ANCHOR_VMID;
    rVal->anchorV[2] = ANCHOR_VBOTTOM;
    break;

  case DIR_DOWN:
    rVal->anchorH[0] = ANCHOR_HLEFT;
    rVal->anchorH[1] = ANCHOR_HMID;
    rVal->anchorH[2] = ANCHOR_HRIGHT;
    rVal->anchorV[0] = ANCHOR_VBOTTOM;
    rVal->anchorV[1] = ANCHOR_VBOTTOM;
    rVal->anchorV[2] = ANCHOR_VBOTTOM;
    break;

  case DIR_LEFT:
    rVal->anchorH[0] = ANCHOR_HLEFT;
    rVal->anchorH[1] = ANCHOR_HLEFT;
    rVal->anchorH[2] = ANCHOR_HLEFT;
    rVal->anchorV[0] = ANCHOR_VTOP;
    rVal->anchorV[1] = ANCHOR_VMID;
    rVal->anchorV[2] = ANCHOR_VBOTTOM;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unknown direction");
    break;
  }

  // convert the pivot points to actual world points
  for (int i = 0; i < 3; i++)
  {
    rVal->subCheckPos[i] = GetPointOnSprite(spr, true, rVal->anchorH[i], rVal->anchorV[i]);

    // if we're moving horz, our check points are normally, at top, middle, bottom:
    //  ____   ___here
    // |    |
    // |    |  <--here
    // |    |
    // |____|  ___here
    //
    // which can make it awkward to clip onto blocks while jumping
    // we can raise the bottom one and lower the top one a bit though
    //  ____
    // |    |  <--here
    // |    |  <--here
    // |    |  <--here
    // |____|
    // to get onto small 'bumps' more easily
    // or run over small gaps
    // or to easily 'clip' past corners onto ledges

    if (dir == DIR_LEFT || dir == DIR_RIGHT)
    {
      if (rVal->anchorV[i] == ANCHOR_VTOP)
      {
        rVal->subCheckPos[i].y += (4 << SHIFT); // 4 pix
      }
      if (rVal->anchorV[i] == ANCHOR_VBOTTOM)
      {
        rVal->subCheckPos[i].y -= (4 << SHIFT);
      }
    }

    int dbgSprXPos = spr->subPos.x;
    int dbgSprYPos = spr->subPos.y;
    int dbgCheckXPos = rVal->subCheckPos[i].x;
    int dbgCheckYPos = rVal->subCheckPos[i].y;
    int dbgSubOffsetX = 0;
    int dbgSubOffsetY = 0;

    if (subOffsetOrNull != NULL)
    {
      AddVec(&rVal->subCheckPos[i], subOffsetOrNull);
      dbgSubOffsetX = subOffsetOrNull->x;
      dbgSubOffsetY = subOffsetOrNull->y;
    }

    if (false)
    {
      printf("__DBG__ Frame %d HitInfo '%s' GetPointOnSprite Anchors %d %d SubOffset %d %d\n", frameCounter, src, (int)rVal->anchorH[i], (int)rVal->anchorV[i], dbgSubOffsetX, dbgSubOffsetY);
      printf("....Player    Pos   x=%d/%d y=%d/%d\n", dbgSprXPos, dbgSprXPos >> SHIFT, dbgSprYPos, dbgSprYPos >> SHIFT);
      printf("....CheckPos (pre)  x=%d/%d y=%d/%d\n", dbgCheckXPos, dbgCheckXPos >> SHIFT, dbgCheckYPos, dbgCheckYPos >> SHIFT);
      printf("....CheckPos (post) x=%d/%d y=%d/%d\n", rVal->subCheckPos[i].x, rVal->subCheckPos[i].x >> SHIFT, rVal->subCheckPos[i].y, rVal->subCheckPos[i].y >> SHIFT);
    }

    if (DEBUG_HITPOINTS)
    {
      // let's debug to screen
      Vec2 screenPos = Sub2Screen(&rVal->subCheckPos[i]);
      uint16_t col = VMUPRO_COLOR_WHITE;
      vmupro_draw_rect(screenPos.x - 2, screenPos.y - 2, screenPos.x + 4, screenPos.y + 4, col);
    }
  }

  // and clear the collision return vals

  rVal->hitMask = SOLIDMASK_NONE;
  rVal->hitMaskIsSolid = false;

  //
  // 1/2 - Check collisions against tiles
  //

  for (int i = 0; i < 3; i++)
  {

    // pre-set the null val
    rVal->blockID[i] = BLOCK_NULL;

    // just the literal tile indexes into the x/y grid
    Vec2 tileRowAndCol = GetTileRowAndColFromSubPos(&rVal->subCheckPos[i]);
    int blockId = GetBlockIDAtColRow(tileRowAndCol.x, tileRowAndCol.y, LAYER_COLS);
    if (blockId != BLOCK_NULL)
    {

      // essentially we're rounding down and then back up, to snap
      // to an exact tile ID
      Vec2 tileSubPos = GetTileSubPosFromRowAndCol(&tileRowAndCol);

      bool ignore1 = !IsOneWayPlatformSolid(blockId, LAYER_COLS, spr, &tileSubPos);
      bool ignore2 = IsTriggerTile(blockId);

      if (ignore1 || ignore2)
      {
        // printf("ignoring 2 sided block\n");
        continue;
      }

      rVal->hitMask |= SOLIDMASK_TILE_SOLID;
      rVal->hitMaskIsSolid = true;

      rVal->blockID[i] = blockId;
      rVal->lastBlockHitIndex = i;

      // default to the hit point being wherever we checked on the sprite
      // we'll tweak the x/y in a sec, depending on where we hit the block
      // that will be our ejection point
      rVal->blockSubEjectPoint[i] = rVal->subCheckPos[i];

      if (dir == DIR_RIGHT)
      {
        // collided on the sprite's right
        // so the hit point is the block's X pos
        rVal->blockSubEjectPoint[i].x = tileSubPos.x;
      }
      else if (dir == DIR_LEFT)
      {
        // collided on the sprite's left
        // so the hit point is the block's x+width
        rVal->blockSubEjectPoint[i].x = tileSubPos.x + TILE_SIZE_SUB;
      }
      else if (dir == DIR_DOWN)
      {
        // collided on the sprite's bottom
        // so the hitpoint is the block's top
        rVal->blockSubEjectPoint[i].y = tileSubPos.y;
      }
      else if (dir == DIR_UP)
      {
        // collided on the sprite's top
        // so the hitpoint is teh block's bottom
        rVal->blockSubEjectPoint[i].y = tileSubPos.y + TILE_SIZE_SUB;
      }
    } // block id != null
  } // for i to 3 (blocks)

  //
  // 2/2 Check collisions against sprites
  //

  // (separate loop simplifies logic)

  for (int i = 0; i < 3; i++)
  {
    // Then check against other sprites
    // (do both, 'cause once might be closer than the other
    for (int j = 0; j < numSprites; j++)
    {
      Sprite *otherSprite = sprites[j];

      if (otherSprite == NULL)
        continue;

      // let's not collide with ourself
      if (otherSprite == spr)
        continue;

      if (rVal->ignorePlayer && otherSprite == player)
      {
        continue;
      }

      Solidity otherSolid = otherSprite->profile.solid;

      // It's non solid, and not a trigger/no collision
      if (!IsBlockingCollision(otherSolid))
        continue;

      bool inside = SubPointInHitbox(&rVal->subCheckPos[i], otherSprite);

      if (!inside)
        continue;

      // printf("Sprite %s is inside %s\n", spr->name, otherSprite->name);

      // mask it so we know we hit something
      rVal->hitMask |= otherSolid;
      rVal->hitMaskIsSolid = IsBlockingCollision(otherSolid);
      rVal->lastSpriteHitIndex = i;

      rVal->otherSprites[i] = otherSprite;
      rVal->spriteSolidity[i] = otherSolid;

      rVal->spriteSubEjectPoint[i] = rVal->subCheckPos[i];

      // Note: remember to use the hitbox and not the pos
      // since the hitbox might be offset in some weird way or another
      if (dir == DIR_RIGHT)
      {
        // collided on the src sprite's right
        // so the hit point is the other sprite's X pos
        rVal->spriteSubEjectPoint[i].x = otherSprite->subHitBox.x;
      }
      else if (dir == DIR_LEFT)
      {
        // collided on the src sprite's left
        // so the hit point is the other sprite's x+width
        rVal->spriteSubEjectPoint[i].x = otherSprite->subHitBox.x + otherSprite->subHitBox.width;
      }
      else if (dir == DIR_DOWN)
      {
        // collided on the src sprite's bottom
        // so the hitpoint is the other sprite's top
        rVal->spriteSubEjectPoint[i].y = otherSprite->subHitBox.y;
      }
      else if (dir == DIR_UP)
      {
        // collided on the src sprite's top
        // so the hitpoint is the other sprite's bottom
        rVal->spriteSubEjectPoint[i].y = otherSprite->subHitBox.y + otherSprite->subHitBox.height;
      }

      // let's not check all the other sprites
      break;

    } // for each sprite
  } // for i to 3 (sprites)
}

void PrintHitInfo(HitInfo *info)
{

  printf("HitInfo dir %d hit = 0x%lx\n", info->whereWasCollision, (uint32_t)info->hitMask);
  printf("   ids = %d, %d, %d\n", info->blockID[0], info->blockID[1], info->blockID[2]);
  printf("   chX = %d, %d, %d\n", info->subCheckPos[0].x, info->subCheckPos[1].x, info->subCheckPos[2].x);
  printf("   chY = %d, %d, %d\n", info->subCheckPos[0].y, info->subCheckPos[1].y, info->subCheckPos[2].y);
  printf("   ejX = %d, %d, %d\n", info->blockSubEjectPoint[0].x, info->blockSubEjectPoint[1].x, info->blockSubEjectPoint[2].x);
  printf("   ejY = %d, %d, %d\n", info->blockSubEjectPoint[0].y, info->blockSubEjectPoint[1].y, info->blockSubEjectPoint[2].y);
}

// Perform the ejection part after collecting hit info
void GetEjectionInfo(Sprite *spr, HitInfo *info, bool horz)
{

  // simple early exit
  // we don't need to eject from liquids, instadeath, triggers
  if (!info->hitMaskIsSolid)
  {
    return;
  }

  // the direction we hit something at
  Direction whereWasCollision = info->whereWasCollision;
  Vec2 ejectPoint = ZeroVec();

  if (info->lastSpriteHitIndex > -1)
  {
    // doing a sprite/sprite ejection
    int idx = info->lastSpriteHitIndex;
    ejectPoint = info->spriteSubEjectPoint[idx];
  }
  else if (info->lastBlockHitIndex > -1)
  {
    // doing a sprite/block ejection
    int idx = info->lastBlockHitIndex;
    ejectPoint = info->blockSubEjectPoint[idx];
  }
  else
  {
    // We'd kinda expect one of these to have happened to set the hit mask
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "GetEjectionInfo has no valid block or sprite hits!");
    return;
  }

  // TODO: now used for both blocks and sprites
  int dbgBlockX = ejectPoint.x;
  int dbgBlockY = ejectPoint.y;

  int dbgPlayerX = spr->subPos.x;
  int dbgPlayerY = spr->subPos.y;

  if (whereWasCollision == DIR_RIGHT)
  {

    // e.g. the point on the block we just hit
    int subX = ejectPoint.x;

    // hitbox might be anchored left/right/middle
    // account for this
    if (spr->anchorH == ANCHOR_HLEFT)
    {
      subX -= spr->subHitBox.width;
    }
    else if (spr->anchorH == ANCHOR_HMID)
    {
      subX -= spr->subHitBox.width / 2;
    }
    else if (spr->anchorH == ANCHOR_HRIGHT)
    {
      subX -= 0;
    }

    int subY = spr->subPos.y;
    Vec2 sub = {subX, subY};
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_LEFT)
  {

    int subX = ejectPoint.x;

    if (spr->anchorH == ANCHOR_HLEFT)
    {
      subX += 0;
    }
    else if (spr->anchorH == ANCHOR_HMID)
    {
      subX += spr->subHitBox.width / 2;
    }
    else if (spr->anchorH == ANCHOR_HRIGHT)
    {
      subX += spr->subHitBox.width;
    }

    int subY = spr->subPos.y;
    Vec2 sub = {subX, subY};
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_UP)
  {

    int subY = ejectPoint.y;

    if (spr->anchorV == ANCHOR_VTOP)
    {
      subY += 0;
    }
    else if (spr->anchorV == ANCHOR_VMID)
    {
      subY += spr->subHitBox.height / 2;
    }
    else if (spr->anchorV == ANCHOR_VBOTTOM)
    {
      subY += spr->subHitBox.height;
    }

    int subX = spr->subPos.x;
    Vec2 sub = {subX, subY};
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_DOWN)
  {

    int subY = ejectPoint.y;

    if (spr->anchorV == ANCHOR_VTOP)
    {
      subY -= spr->subHitBox.height;
    }
    else if (spr->anchorV == ANCHOR_VMID)
    {
      subY -= spr->subHitBox.height / 2;
    }
    else if (spr->anchorV == ANCHOR_VBOTTOM)
    {
      subY -= 0;
    }

    int subX = spr->subPos.x;
    Vec2 sub = {subX, subY};
    info->snapPoint = sub;
  }

  // debug block
  if (false)
  {
    printf("__DBG__ Frame %d eject from dir (urdl) %d\n", frameCounter, whereWasCollision);
    printf("....from blockpos x=%d/%d y=%d/%d\n", dbgBlockX, dbgBlockX >> SHIFT, dbgBlockY, dbgBlockY >> SHIFT);
    printf("....from plry pos x=%d/%d y=%d/%d\n", dbgPlayerX, dbgPlayerX >> SHIFT, dbgPlayerY, dbgPlayerY >> SHIFT);
    Vec2 newPos = info->snapPoint;
    printf("....TO   plry pos x=%d/%d y=%d/%d\n", newPos.x, newPos.x >> SHIFT, newPos.y, newPos.y >> SHIFT);
    printf("....(provisionally)\n");

    // To double check your collision logics
    if (whereWasCollision == DIR_DOWN)
    {
      bool groundedNow = CheckGrounded(spr, NULL);
      if (!groundedNow)
      {
        vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite %s ejected from a down type collision, but still not grounded!", spr->name);
      }
    }
  }
}

// Attempts to apply velo to pos, taking collisions into account
// returns the sign of the movement direction
// e.g. -1 for jump, 1 for ground
// e.g. -1 for left, 1 for right
int CheckCollisionsAndEject(HitInfo *info, Sprite *spr, bool horz, Vec2 platformDelta)
{

  // TODO: need a function to init the hitinfo cleanly
  memset(info, 0, sizeof(HitInfo));
  info->lastBlockHitIndex = -1;
  info->lastSpriteHitIndex = -1;

  // Non solid, it's not hitting anything
  if (spr->profile.solid == SOLIDMASK_NONE)
  {
    return 0;
  }
  if (spr->profile.iMask & IMASK_IGNORE_COLLISIONS)
  {
    return 0;
  }

  // We're not using prediction by default
  // but it could later be applied
  Vec2 subCheckOffset = {0, 0};

  Direction dir = DIR_RIGHT;
  if (horz)
  {

    // TODO: this whole section could use a tidy
    // Could include nonzero values
    // but would need to prevent warping up when
    // approaching sprites from the left
    bool movingRight = (spr->subVelo.x + platformDelta.x) > 0;
    bool movingLeft = (spr->subVelo.x + platformDelta.x) < 0;

    if (movingRight)
    {
      dir = DIR_RIGHT;
    }
    else if (movingLeft)
    {
      dir = DIR_LEFT;
    }
    else
    {
      return 0;
    }
  }
  else
  {

    bool movingDown = (spr->subVelo.y + platformDelta.y) > 0;
    bool movingUp = (spr->subVelo.y + platformDelta.y) < 0;

    if (movingDown)
    {
      dir = DIR_DOWN;
    }
    else if (movingUp)
    {
      dir = DIR_UP;
    }
    else
    {
      return 0;
    }
  }

  GetHitInfo(info, spr, dir, &subCheckOffset, "TryMove");

  if (info->hitMask)
  {
    // PrintHitInfo(&info);
  }

  GetEjectionInfo(spr, info, horz);

  if (info->hitMask)
  {
    // return sign of direction
    return (dir == DIR_RIGHT || dir == DIR_DOWN) ? 1 : -1;
  }
  return 0;
}

// Check for any collisions on the given sprite using 3 points
// e.g. for UP it'd be top left, top middle, top right
// for right it'd be top right, middle right, bottom right
// Apply an offset to check for stuff ahead, behind, above, below, etc
// Note: for a ground check, add 1 to y, since the hitbox ends on the last
//       subpixel before the next ground tile
Solidity CheckSpriteCollision(Sprite *spr, Direction dir, Vec2 *subOffset, const char *src, bool ignorePlayer)
{

  HitInfo nhi;
  memset(&nhi, 0, sizeof(HitInfo));
  nhi.ignorePlayer = ignorePlayer;
  GetHitInfo(&nhi, spr, dir, subOffset, src);
  return nhi.hitMask;
}

bool CheckGrounded(Sprite *spr, GroundHitInfo *groundHitInfoOrNull)
{

  if (spr->profile.solid == SOLIDMASK_NONE)
  {
    return false;
  }

  // prevent being knocked back and landing on something in the same frame
  if (SpriteIsKnockback(spr) && spr->knockbackFrameNum < 10)
  {
    return false;
  }

  // hitbox ends on the very last subpixel
  // so adding one takes you into the next tile
  Vec2 offset = {0, 1};
  HitInfo nhi;
  memset(&nhi, 0, sizeof(HitInfo));
  GetHitInfo(&nhi, spr, DIR_DOWN, &offset, "checkgrounded");

  if (groundHitInfoOrNull != NULL)
  {
    // ensure it's clear
    memset(groundHitInfoOrNull, 0, sizeof(GroundHitInfo));
    groundHitInfoOrNull->solidMask = nhi.hitMask;

    if (nhi.lastSpriteHitIndex > -1)
    {
      groundHitInfoOrNull->otherSpriteOrNull = nhi.otherSprites[nhi.lastSpriteHitIndex];
    }
  }

  return nhi.hitMask != SOLIDMASK_NONE;
}

bool IsGroundedOrCoyoteTime(Sprite *spr)
{
  bool rVal = spr->isGrounded || (frameCounter - spr->lastGroundedFrame) < COYOTE_TIME_FRAME_THRESH;
  return rVal;
}

bool IsInLiquid(Sprite *spr)
{
  return spr->inLiquid;
}

// the first part of the jump, triggering it
void TryJump(Sprite *spr)
{

  // reset jump state if user released
  if (spr->mustReleaseJump)
  {
    if (!spr->input.jump)
    {
      spr->mustReleaseJump = false;
    }
  }

  if (!spr->input.jump)
  {
    return;
  }

  if (spr->mustReleaseJump)
  {
    return;
  }

  bool inLiquid = IsInLiquid(spr);

  if (!inLiquid)
  {

    if (spr->jumpFrameNum != 0)
    {
      return;
    }

    if (SpriteJumping(spr))
    {
      return;
    }

    bool grounded = IsGroundedOrCoyoteTime(spr);

    if (!grounded)
    {
      return;
    }
  }

  SetMoveMode(spr, MM_JUMP, "TryJump");
  spr->mustReleaseJump = true;
  // for water
  spr->jumpFrameNum = 0;
}

void TryDash(Sprite *spr)
{

  if (spr->mustReleaseDash)
  {
    if (!spr->input.run)
    {
      spr->mustReleaseDash = false;
    }
  }

  if (spr->mustReleaseDash)
  {
    return;
  }

  // if we're in the delay phase, count up to 0
  // before allowing another dash
  if (spr->dashFrameNum < 0)
  {
    spr->dashFrameNum++;
    if (spr->dashFrameNum == 0)
    {
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Dash counter ready %d", spr->dashFrameNum);
    }
    return;
  }

  if (spr->dashFrameNum != 0)
  {
    return;
  }

  if (!spr->input.run)
  {
    return;
  }

  if (SpriteJumping(spr))
  {
    return;
  }

  if (SpriteDashing(spr))
  {
    return;
  }

  bool grounded = IsGroundedOrCoyoteTime(spr);
  if (!grounded)
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d sprite %s, starting dash", frameCounter, spr->name);
  SetMoveMode(spr, MM_DASH, "TryDash");
  spr->dashFrameNum = 0;
}

bool TryInitButtDash(Sprite *spr)
{

  if (spr->mustReleaseDash)
  {
    if (!spr->input.run)
    {
      spr->mustReleaseDash = false;
    }
  }

  if (spr->mustReleaseDash)
  {
    return false;
  }

  if (!spr->input.run)
  {
    return false;
  }

  if (IsGroundedOrCoyoteTime(spr))
  {
    return false;
  }

  if (!SpriteJumping(spr))
  {
    return false;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d sprite %s, Init ButtDash", frameCounter, spr->name);
  SetMoveMode(spr, MM_BUTTDASH, "TryButtDash");
  spr->mustReleaseDash = true;
  spr->buttBounceFrameNum = 0;
  spr->numButtStomps = 0;
  return true;
}

void TryContinueButtStomp(Sprite *spr)
{

  // first update input for either dash or bounce

  if (!SpriteDoingButtStuff(spr))
  {
    return;
  }

  if (!spr->input.run)
  {
    SetMoveMode(spr, MM_FALL, "ButtDash/Bounce Released");
  }

  // have we switched from a bounce back to a dash?
  // i.e. reached the apex
  if (SpriteIsButtBouncing(spr))
  {
    if (spr->subVelo.y > 0)
    {
      SetMoveMode(spr, MM_BUTTDASH, "apex");
      return;
    }
  }

  // then focus primarily on the butt bounce

  if (!SpriteIsButtBouncing(spr))
  {
    return;
  }

  // only apply velo up for a few frames
  // but once it runs out, we'll stay in butt bounce
  // untill
  if (spr->buttBounceFrameNum >= spr->phys->max_buttbounce_frames)
  {
    return;
  }
  // Expected to be 0 until we actually bounce
  spr->subVelo.y -= spr->buttstompSubVelo;
  spr->buttBounceFrameNum++;
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s, applying butt bounce %d velo %d", frameCounter, spr->name, spr->buttstompSubVelo, spr->subVelo.y);
}

void ResetHighestJumpPoint(Sprite *spr, const char *cause)
{

  Vec2 feetPos = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VMID);
  // grab the feet pos
  spr->lastgroundedYSubPos = feetPos.y;
  spr->highestYSubPosInJump = spr->lastgroundedYSubPos;
}

// return: did we bounce or were already bouncing
// Note: the final return height requires 2 things:
// 1: a force to apply every frame, like jumping
// 2: an initial 'reflection' force for the bounce
// too much of either and you can jump higher than starting pos
// not enough per-frame force feels underwhelming
// not enough reflection looks unnatural, (bounce slow @ start, fast at end)
// it's a balancing act
bool TryButtBounce(Sprite *spr, ButtStrength inStr, const char *cause)
{

  // e.g. a butt bounce off a mob may register as both
  // a butt bounce and a ground bounce
  if (SpriteIsButtBouncing(spr))
  {
    printf("Preventing double butt bounce due to %s\n", cause);
    // return true since we're actually still butt bouncing
    return true;
  }

  if (!SpriteButtStompingAboveThresh(spr))
  {
    return false;
  }

  // calc the height fallen
  int highest = spr->highestYSubPosInJump;
  Vec2 now = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VBOTTOM);
  int delta = now.y - highest;
  if (delta < 0)
  {
    printf("__TEST__ negative jump height\n");
    return false;
  }
  // printf("__TEST__ jump height: %d (h %d & n %d)\n", delta, highest, now.y);

  int bounceVel = Abs(delta) / 250; // Abs(spr->lastSubVelo.y);
  // bouncing off a mob should let you jump sliiightly higher
  if (inStr == BUTTSTR_ENEMY)
  {
    // enough to clear slightly higher than usual
    bounceVel = spr->phys->sub_gravity + 2;
  }

  if (bounceVel > BUTTBOUNCE_MAX_VEL)
  {
    vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d Sprite %s, clamping bounce vel from %d to %d\n", frameCounter, spr->name, bounceVel, BUTTBOUNCE_MAX_VEL);
    bounceVel = BUTTBOUNCE_MAX_VEL;
  }

  // we gonna butt stomp
  // reset the y velo since it's still going to be something nuts
  spr->subVelo.y = -Abs(spr->lastSubVelo.y) / 2;
  spr->buttstompSubVelo = bounceVel;
  spr->numButtStomps++;
  spr->buttBounceFrameNum = 0; // reload!
  ResetHighestJumpPoint(spr, "bounce");
  SetMoveMode(spr, MM_BUTTBOUNCE, cause);

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s, ButtDash->ButtBounce #%d with velo %d cause %s", frameCounter, spr->name, spr->buttBounceFrameNum, spr->buttstompSubVelo, cause);
  return true;
}

// prevent the jump button applying further up force
// doesn't change move state!
// possible causes:
// - bonked head          (remain in jump, stop applying force)
// - out of boost frames  (remain in jump, stop applying force)
// - landed on ground     (will return to walking movestate)
// - took damage          (will handle switc move states)
void StopJumpBoost(Sprite *spr, bool resetMoveMode, const char *src)
{
  if (!SpriteJumping(spr))
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s, jumpboost canceled, src= '%s' resetMoveMode = %d\n", frameCounter, spr->name, src, resetMoveMode);
  spr->jumpFrameNum = spr->phys->max_jump_boost_frames;

  if (resetMoveMode)
  {
    SetMoveMode(spr, MM_WALK, "StopJumpBoostWithReset");
  }
}

void StopButtstomp(Sprite *spr, bool resetMoveMode, const char *cause)
{

  if (!SpriteIsButtDashing(spr))
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s cancelling buttsomp on %d/%d due to %s\n", frameCounter, spr->name, spr->numButtStomps, spr->buttBounceFrameNum, cause);

  spr->buttBounceFrameNum = 0;
  spr->buttstompSubVelo = 0;
  spr->numButtStomps = 0;

  if (resetMoveMode)
  {
    SetMoveMode(spr, MM_WALK, "StopButtStompWithReset");
  }
}

// prevent the dash button applying further force
// doesn't change move state!
// possible causes:
// - bonked wall/sprite   (caller should apply knockback state)
// - out of dash frames   (caller should return to walk state)
// - jumped/fell          (caller should return to appropriate state)
void StopDashBoost(Sprite *spr, bool resetMovemode, const char *src)
{

  if (!SpriteDashing(spr))
  {
    // vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d, Sprite %s isn't dashing, can't StopDashBoost(%s)", frameCounter, spr->name, src);
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s, dashboost canceled, src= '%s' resetMoveMode = %d\n", frameCounter, spr->name, src, resetMovemode);
  spr->dashFrameNum = -spr->phys->dashDelayFrames;

  if (resetMovemode)
  {
    SetMoveMode(spr, MM_WALK, "StopDashBoostWithReset");
  }
}

void TryContinueJump(Sprite *spr)
{

  if (!SpriteJumping(spr))
  {
    return;
  }
  if (spr->jumpFrameNum >= spr->phys->max_jump_boost_frames)
  {
    return;
  }

  // we're jumping, and we've not run out of boost frames
  if (spr->input.jump)
  {
    // add jump velo
    spr->subVelo.y -= spr->phys->sub_jumpforce;
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s, applying jump %d velo %d", frameCounter, spr->name, spr->phys->sub_jumpforce, spr->subVelo.y);
    spr->jumpFrameNum++;

    if (spr->jumpFrameNum >= spr->phys->max_jump_boost_frames)
    {
      StopJumpBoost(spr, false, "ReachedFrameMax");
    }
  }
  else
  {
    // user has released jump, prevent further re-presses
    // until we land
    StopJumpBoost(spr, false, "released");
  }
}

void TryContinueDash(Sprite *spr)
{

  if (!SpriteDashing(spr))
  {
    return;
  }

  // <- for delay
  if (spr->dashFrameNum < 0 || spr->dashFrameNum >= spr->phys->max_dash_frames)
  {
    return;
  }

  if (spr->input.run)
  {
    // add dash velo
    if (spr->facingRight)
    {
      spr->subVelo.x += spr->phys->sub_dashforce;
    }
    else
    {
      spr->subVelo.x -= spr->phys->sub_dashforce;
    }
    spr->dashFrameNum++;
    if (spr->dashFrameNum >= spr->phys->max_dash_frames)
    {
      StopDashBoost(spr, true, "ReachedFrameMax");
    }
  }
  else
  {
    // user released dash early, prevent a re-dash
    StopDashBoost(spr, true, "Released");
  }
}

void TryContinueKnockback(Sprite *spr)
{

  if (!SpriteIsKnockback(spr))
  {
    return;
  }

  spr->knockbackFrameNum++;
  if (spr->knockbackFrameNum >= spr->phys->max_knockback_frames)
  {
    StopKnockback(spr, true, "ReachedFrameMax");
  }
}

// e.g. walking off an edge
void CheckFallen(Sprite *spr)
{

  // we were walking/running on the ground
  // we were on the ground
  // we are no longer on the ground
  // we aren't jumping either

  if (!MoveModeOnGround(spr))
  {
    return;
  }

  if (!spr->onGroundLastFrame)
  {
    return;
  }

  if (spr->isGrounded)
  {
    return;
  }

  if (SpriteJumping(spr))
  {
    return;
  }

  // we were on the ground, but are no longer
  // we're not jumping...
  // so we're falling
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s walked off edge", frameCounter, spr->name);
  if (SpriteDashing(spr))
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "(ignored due to dash)");
  }
  else
  {
    SetMoveMode(spr, MM_FALL, "CheckFallen");
  }
}

bool DontLandCauseImJumping(Sprite *spr)
{
  // don't land if we clip the corner of a stair
  // while jumping up
  if (spr->moveMode == MM_JUMP && spr->subVelo.y < 0)
  {
    return true;
  }
  return false;
}

void CheckLanded(Sprite *spr)
{

  if (!spr->isGrounded)
  {
    return;
  }
  // let's also check if movemode is air
  // in case the character jumped and landed on the
  // same frame due to odd geometry, etc

  // TODO: bug: if a bounce doesn't produce any velo it doesn't trigger
  // the last vs now check, and could fail to land
  bool someKindaOffTheGround = (!spr->onGroundLastFrame) || MoveModeInAir(spr);
  if (!someKindaOffTheGround)
  {
    return;
  }

  if (DontLandCauseImJumping(spr))
  {
    return;
  }

  if (TryButtBounce(spr, BUTTSTR_GROUND, "checklanded"))
  {
    return;
  }
  // vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s speed %d below thresh %d for butt stomp", frameCounter, spr->name, spr->lastSubVelo.y, BUTTSTOMP_THRESH_SPEED);

  // we're on the ground
  // we weren't during the last frame
  // note: will cancel buttstomp
  SetMoveMode(spr, MM_WALK, "CheckLanded");
  spr->jumpFrameNum = 0;
}

Sprite *FindSpriteOfType(SpriteType inType, int indexerOrNeg, Sprite *excludeOrNull)
{

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    if (spr == excludeOrNull)
      continue;

    if (indexerOrNeg > -1)
    {
      if (spr->indexer != indexerOrNeg)
      {
        continue;
      }
    }

    if (spr->sType == inType)
      return spr;
  }

  return NULL;
}

void HandleDoorTransition(Sprite *activator, Sprite *door)
{

  if (!AllowSpriteInput(activator))
    return;

  // We could add some in-air version of things
  // but this should do for now
  // - hold up for a bit
  // - must have slowed down
  // - ensure lowish velo

  if (activator->input.up < DOOR_THRESH_FRAMES)
  {
    return;
  }

  int absx = Abs(activator->subVelo.x);
  if (absx > DOOR_THRESH_SPEED)
  {
    return;
  }

  // let's do the door thing!

  // find the other door with the same ID
  Sprite *connectedDoor = FindSpriteOfType(door->sType, door->indexer, door);

  if (connectedDoor == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Couldn't find a matching door!\n");
    return;
  }

  // find the delta between the center of the other door
  // and the center of the player
  // and then add that to the player's pos
  Vec2 doorCenterSub = GetPointOnSprite(connectedDoor, true, ANCHOR_HMID, ANCHOR_VMID);
  Vec2 playerCenterSub = GetPointOnSprite(player, true, ANCHOR_HMID, ANCHOR_VMID);

  Vec2 delta = {doorCenterSub.x - playerCenterSub.x, doorCenterSub.y - playerCenterSub.y};

  Vec2 newPos = {player->subPos.x + delta.x, player->subPos.y + delta.y};

  SetSubPos(player, &newPos);
  player->subVelo = ZeroVec();
  GotoGameState(GSTATE_TRANSITION);
}

void HandleSpriteTriggers(Sprite *srcSprite, Sprite *trigger)
{

  if (srcSprite == NULL || trigger == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "HandleSpriteTriggers got a null sprite ref or trigger ref");
    return;
  }

  SpriteType sType = trigger->sType;

  switch (sType)
  {

  case STYPE_DOOR:
    HandleDoorTransition(srcSprite, trigger);
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled sprite type %d in HandleSpriteTriggers!", sType);
    return;
    break;
  }
}

bool SpriteCanRideSprite(Sprite *rider, Sprite *thingImRiding)
{

  if (rider == NULL || thingImRiding == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null rider or thing to ride");
    return false;
  }

  if (!(thingImRiding->profile.iMask & IMASK_CAN_BE_RIDDEN))
  {
    return false;
  }

  // only allow a small set of valid anims, states
  // err on the side of caution
  if (!thingImRiding->sentinel)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d, Sprite %s is riding a sprite with a stale ref!", frameCounter, rider->name);
    return false;
  }
  if (!AllowSpriteInput(rider) || !AllowSpriteInput(thingImRiding))
    return false;

  if (rider->inLiquid)
  {
    return false;
  }

  if (!(rider->profile.iMask & IMASK_CAN_RIDE_STUFF))
    return false;
  if (!(thingImRiding->profile.iMask & IMASK_CAN_BE_RIDDEN))
    return false;

  // make sure we're on top of it
  // TODO: could prob simplify with regular scalar maffs
  Vec2 riderFeetPos = GetPointOnSprite(rider, true, ANCHOR_HMID, ANCHOR_VBOTTOM);
  Vec2 thingImRidingHeadPos = GetPointOnSprite(rider, true, ANCHOR_HMID, ANCHOR_VTOP);
  // give a little wiggle room
  if (riderFeetPos.y < thingImRidingHeadPos.y + 4)
  {
    return false;
  }

  return true;
}

void TryKnockback(Sprite *spr, KnockbackStrength inStr, const char *cause, Sprite *srcOrNull)
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d, Sprite %s knockback! str= %d, cause = '%s'", frameCounter, spr->name, (int)inStr, cause);

  if (SpriteIsKnockback(spr))
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d, Sprite %s is already in knockback sate! cause='%s'", frameCounter, spr->name, cause);
    return;
  }

  int yKnock = 0;
  int xKnock = 0;
  switch (inStr)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d unhandled knockback strength %d", frameCounter, (int)inStr);
    break;

  case KNOCK_NUDGE:
    if (srcOrNull == NULL)
    {
      vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d unhandled knockback strength %d", frameCounter, (int)inStr);
      break;
    }
    bool nudgeRight = srcOrNull->subPos.x > spr->subPos.x;
    xKnock = nudgeRight ? -DASHBONK_MINIMAL_KNOCKBACK * 2 : DASHBONK_MINIMAL_KNOCKBACK * 2;
    yKnock = -(spr->phys->sub_gravity + 2);
    break;

  case KNOCK_MINIMAL:
    xKnock = spr->facingRight ? -DASHBONK_MINIMAL_KNOCKBACK : DASHBONK_MINIMAL_KNOCKBACK;
    yKnock = -(spr->phys->sub_gravity + 2);
    break;

  case KNOCK_SOFT:
    xKnock = -spr->lastSubVelo.x / 5;
    yKnock = -(spr->phys->sub_gravity + 2);
    break;

  case KNOCK_HARD:
    xKnock = spr->phys->max_subspeed_run; // TODO: something usable for now
    xKnock = spr->facingRight ? -xKnock : xKnock;
    yKnock = -(spr->phys->sub_gravity + 4);
    spr->invulnFrameNum = INVULN_FRAME_DELAY;
    // wipe any builtup velo, to avoid gravity preventing us escaping lava
    spr->subVelo.y = 0;
    break;
  }
  spr->subKnockbackAccel.x = xKnock;
  spr->subKnockbackAccel.y = yKnock;

  SetMoveMode(spr, MM_KNOCKBACK, "TryKnockback");
}

// TODO: is this vs world only?
void OnDashBonk(Sprite *spr)
{

  if (!SpriteDashingAboveBonkThresh(spr))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d, Sprite %s can't dashbonk if not dashing", frameCounter, spr->name);
    return;
  }

  // we hit something, dash bounce
  TryKnockback(spr, KNOCK_SOFT, "H or Head bonk", NULL);
}

void StopKnockback(Sprite *spr, bool resetMoveMode, const char *cause)
{

  if (!SpriteIsKnockback(spr))
  {
    return;
  }

  spr->knockbackFrameNum = 0;
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d, Sprite %s stopping knockback! cause= %s, reset= %d", frameCounter, spr->name, cause, resetMoveMode);

  if (resetMoveMode)
  {
    SetMoveMode(spr, MM_FALL, "StopKnockbackWithReset");
  }
}

void DestroyBlock(Vec2 *rowAndCol)
{

  // TODO: bounds check
  int oldVal = GetBlockIDAtColRow(rowAndCol->x, rowAndCol->y, LAYER_COLS);
  ;
  if (oldVal == BLOCK_NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Can't delete tile at row/col %d/%d since it's already NULL", rowAndCol->x, rowAndCol->y);
  }

  uint8_t *tileData = GetTileLayerData(LAYER_COLS);

  int indexIntoTileArray = (rowAndCol->y * currentLevel->colLayer->width) + rowAndCol->x;
  // destroy the tile in the tilemap, but not in the hitinfo
  // since we might still be using it
  tileData[indexIntoTileArray] = BLOCK_NULL;

  // spawn some destroy sprites

  Vec2 worldSpawnPoint;
  worldSpawnPoint.x = rowAndCol->x * TILE_SIZE_PX;
  worldSpawnPoint.y = rowAndCol->y * TILE_SIZE_PX;

  for (int i = 0; i < 4; i++)
  {
    CreateSprite(STYPE_PARTICLE_BROWN, worldSpawnPoint, "Particle");
    if (i == 0 || i == 2)
    {
      worldSpawnPoint.x += TILE_SIZE_PX;
    }
    if (i == 1)
    {
      worldSpawnPoint.x += TILE_SIZE_PX;
      worldSpawnPoint.y += TILE_SIZE_PX;
    }
  }
}

void ProcessTileTouches(Sprite *spr, HitInfo *info, bool horz)
{

  for (int i = 0; i < 3; i++)
  {
    if (info->blockID[i] == -1)
    {
      continue;
    }

    int tileID = info->blockID[i];

    if (!IsTileIDBreakable(tileID))
    {
      continue;
    }

    if (horz)
    {

      if (info->whereWasCollision != DIR_RIGHT && info->whereWasCollision != DIR_LEFT)
      {
        continue;
      }

      if (SpriteDashingAboveBonkThresh(spr))
      {

        if (IsTileIDBreakable(tileID))
        {
          // destroy it
          Vec2 rowAndCol = GetTileRowAndColFromSubPos(&info->subCheckPos[i]);
          DestroyBlock(&rowAndCol);
        }
        else
        {
          // bounce off it
          OnDashBonk(spr);
        }
      } // above thresh
    } // horz

    if (!horz)
    {

      if (info->whereWasCollision != DIR_UP && info->whereWasCollision != DIR_DOWN)
      {
        continue;
      }

      if (IsTileIDBreakable(tileID))
      {
        if (SpriteButtStompingAboveThresh(spr))
        {

          // Destroy it
          Vec2 rowAndCol = GetTileRowAndColFromSubPos(&info->subCheckPos[i]);
          DestroyBlock(&rowAndCol);

          TryButtBounce(spr, BUTTSTR_GROUND, "breakable tile");
        }
        else if (spr->lastSubVelo.y < -HEADBUTT_THRESH_SPEED)
        {
          // headbutt it
          Vec2 rowAndCol = GetTileRowAndColFromSubPos(&info->subCheckPos[i]);
          DestroyBlock(&rowAndCol);
        }
      }
    }

  } // foreach
}

void OnSpriteGotTouched(Sprite *toucher, Sprite *target, bool horz)
{

  InteractionMask hvMask = horz ? IMASK_DMGOUT_HORZ : IMASK_DMGOUT_VERT;
  InteractionMask targetMask = target->profile.iMask;

  // hurts to touch, do the damage, call it a day
  if (targetMask & hvMask)
  {

    bool ignoreDamageDueToBeingButtBounced = !horz && (targetMask & IMASK_DMGOUT_IGNORED_WHEN_BOUNCED) && SpriteDoingButtStuff(toucher);

    if (!ignoreDamageDueToBeingButtBounced)
    {
      SpriteTakeDamage(toucher, target->profile.damage_multiplier, target, "touch a hurty");
      return;
    }
  }

  // doesn't hurt to touch
  // are we charging it?
  bool dashing = SpriteDashingAboveBonkThresh(toucher);
  bool buttstomping = SpriteButtStompingAboveThresh(toucher);

  if (dashing || buttstomping)
  {

    bool takeDamage = (target->profile.iMask & IMASK_DMGIN_KNOCKSME);
    if (takeDamage)
    {
      SpriteTakeDamage(target, toucher->profile.damage_multiplier, toucher, "Toucher Dashed damage taker");
    }
    else if (target->profile.iMask & IMASK_DMGIN_STUNSME)
    {

      if (SpriteStunned(target))
      {
        // already stunned, take damage
        SpriteTakeDamage(target, toucher->profile.damage_multiplier, toucher, "toucher dashed pre-stunned stunnable");
      }
      else
      {
        bool didStun = StunSprite(target, toucher->profile.damage_multiplier, toucher, "toucher dashed stunnable");
        if (didStun && !SpriteIsButtDashing(toucher))
        {
          // knock back the player a bit, if they're not already buttstomping
          TryKnockback(toucher, KNOCK_MINIMAL, "Stunned a thing", NULL);
        }
      }
    }
    else
    {
      // just apply a small knockback
      TryKnockback(target, KNOCK_SOFT, "Toucher dashed non-damage taker", NULL);
    }

    if (buttstomping)
    {
      TryButtBounce(toucher, BUTTSTR_ENEMY, "sprite touch");
    }
  }
  else
  {
    // doesn't hurt us, ignore

    // if i'm stunned, i can be pushed
    if (SpriteStunned(target))
    {
      TryKnockback(target, KNOCK_NUDGE, "nudged while stunned", toucher);
    }
  }
}

void ProcessSpriteTouches(Sprite *spr, HitInfo *info, bool horz)
{

  Sprite *lastTarget = NULL;
  for (int i = 0; i < 3; i++)
  {

    if (info->otherSprites[i] == NULL)
    {
      continue;
    }

    // Don't need to process the same once twice
    Sprite *targ = info->otherSprites[i];
    if (targ == lastTarget)
    {
      continue;
    }

    OnSpriteGotTouched(spr, targ, horz);
    lastTarget = targ;
  }
}

// basic order of operations
// - use state from previous frame, since it's fully resolved
// - check inputs
// - apply acceleration to velo
// - apply X velo to position
// - resolve horz collisions
// - apply Y velo to positions
// - resolve vert collisions
// - double check that x/y ejection hasn't caused another collision
// - check ground state
// - check landing, etc
void SolveMovement(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to SolveMovement");
    return;
  }

  // skip the whole loop if it's a door or something
  if (spr->profile.iMask & IMASK_SKIP_MOVEMENT)
  {
    return;
  }

  // if the sprite is dead, we can skip a lot of
  // the movement logic
  if (spr->animID == ANIIMTYPE_DIE)
  {

    AddVecInts(&spr->subVelo, 0, spr->phys->sub_gravity);
    AddVec(&spr->subPos, &spr->subVelo);

    return;
  }

  bool knockbackAtStartofFrame = SpriteIsKnockback(spr);

  Inputs *inp = &spr->input;
  InteractionMask iMask = spr->profile.iMask;

  // read in all of our state values
  // (the result of the previous frame)
  // i.e. we don't want to check the grounded state now
  // then perform a jump + land in the same frame.

  int maxSubSpeedX = GetMaxXSubSpeed(spr);
  int maxSubSpeedY = GetMaxYSubSpeed(spr);

  int subAccelX = GetXSubAccel(spr);
  int subAccelY = GetYSubAccel(spr);

  int subDampX = GetXDamping(spr);
  int subDampY = 0;

  if (iMask & IMASK_PLATFORM_MOVEMENT)
  {
    // simple, basic movement
    // e.g. platforms 'walking' accross the sky
    maxSubSpeedX = spr->phys->max_subspeed_walk;
    maxSubSpeedY = spr->phys->max_subspeed_walk;

    subAccelX = spr->phys->subaccel_walk;
    subAccelY = spr->phys->subaccel_walk;

    subDampX = spr->phys->subaccel_walk;
    subDampY = spr->phys->subaccel_walk;
  }
  else
  {

    maxSubSpeedX = GetMaxXSubSpeed(spr);
    maxSubSpeedY = GetMaxYSubSpeed(spr);

    subAccelX = GetXSubAccel(spr);
    subAccelY = GetYSubAccel(spr);

    subDampX = GetXDamping(spr);
    subDampY = 0;
  }

  if (iMask & IMASK_SPECIAL_MOVES)
  {
    TryJump(spr);
    TryContinueJump(spr);
    TryDash(spr);
    TryContinueDash(spr);
    TryInitButtDash(spr);
    TryContinueButtStomp(spr);
  }
  TryContinueKnockback(spr);

  bool spriteXInput = false;
  bool spriteYInput = false;
  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;
  bool movingUp = spr->subVelo.y < 0;
  bool movingDown = spr->subVelo.y > 0;

  bool inControl = AllowSpriteInput(spr);
  bool isSolid = IsBlockingCollision(spr->profile.solid);

  if (inControl)
  {

    spriteXInput = inp->right || inp->left;
    spriteYInput = inp->up || inp->down;

    if (inp->right && !spr->facingRight)
    {
      spr->facingRight = true;
    }
    if (inp->left && spr->facingRight)
    {
      spr->facingRight = false;
    }
  }
  else if (SpriteIsKnockback(spr))
  {
    spriteXInput = true;
    spriteYInput = true;
    // TODO: can jam up if you never leave the ground
    subAccelX += spr->subKnockbackAccel.x;
    subAccelY += spr->subKnockbackAccel.y;
  }
  else if (SpriteStunned(spr))
  {
    spriteXInput = true;
    spriteYInput = true;
    subAccelX += spr->subKnockbackAccel.x;
    subAccelY += spr->subKnockbackAccel.y;
    spr->subKnockbackAccel.x = 0;
    spr->subKnockbackAccel.y = 0;
  }

  // Debug/testing
  if (iMask & IMASK_PLATFORM_MOVEMENT)
  {
    if (!spriteYInput)
    {
      subAccelY = 0;
    }
    else
    {
      if (inp->up)
      {
        subAccelY = -subAccelY;
      }
      if (inp->down)
      {
        subAccelY = subAccelY;
      }
    }
  }

  // increase accel while turning
  bool turning = (inp->left && movingRight) || (inp->right && movingLeft);
  if (turning)
    subAccelX = (subAccelX * 4) / 3;

  if (!spriteXInput)
  {
    subAccelX = 0;
  }
  else
  {

    if (inp->left)
    {
      subAccelX = -subAccelX;
    }
    if (inp->right)
    {
      subAccelX = subAccelX;
    }
  }

  // Finalise accelration
  Vec2 subAccel = {subAccelX, subAccelY};
  AddVec(&spr->subVelo, &subAccel);

  //
  // And then add the delta of anything we're riding
  //

  Vec2 ridingPlatformDelta = ZeroVec();
  if (spr->thingImRiding != NULL)
  {
    if (SpriteCanRideSprite(spr, spr->thingImRiding))
    {
      // add however far the thing we're riding moved to our velo
      // this way its movement is bundled into our own
      // movement calcs and helps preven glitching into stuff
      ridingPlatformDelta = SubNewVec(&spr->thingImRiding->subPos, &spr->thingImRiding->lastSubPos);
    }
    else
    {
      spr->thingImRiding = NULL;
      vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d, Sprite %s is riding a sprite with a stale ref!", frameCounter, spr->name);
    }
  }
  // clear immediately to avoid stale references
  // the ground check will later re-set the value
  spr->thingImRiding = NULL;

  HitInfo xHitInfo;
  memset(&xHitInfo, 0, sizeof(HitInfo));
  HitInfo yHitInfo;
  memset(&yHitInfo, 0, sizeof(HitInfo));

  // We'll split the movement + collision ejection
  // into x & y components.
  // without this we might e.g. land inside the ground
  // then the x ejection routine would zip you off to the side
  // or vice versa
  // We could also pre-predict if we're about to hit something
  // based on (pos + velo) and clamp accordingly, but this
  // can introduce challenges when adding new features
  // if not designed with this in mind.

  //
  // X/Horizontal damp, clamp, move, eject
  //

  // Damp X Velo

  if (movingRight && !inp->right)
  {
    // clamp it so we don't go into the negative
    if (subDampX > spr->subVelo.x)
    {
      subDampX = spr->subVelo.x;
    }
    subDampX *= -1;
  }
  else if (movingLeft && !inp->left)
  {
    if (subDampX > -spr->subVelo.x)
    {
      subDampX = -spr->subVelo.x;
    }
    // it's already +ve so will be subbed from a neg
  }
  else
  {

    subDampX = 0;
  }

  AddVecInts(&spr->subVelo, subDampX, 0);

  // Clamp X Velo

  if (spr->subVelo.x > maxSubSpeedX)
  {
    spr->subVelo.x = maxSubSpeedX;
  }
  else if (spr->subVelo.x < -maxSubSpeedX)
  {
    spr->subVelo.x = -maxSubSpeedX;
  }

  /*
  if ( ridingPlatformDelta.x > 0 && spr->subVelo.x < ridingPlatformDelta.x ){
    spr->subVelo.x = ridingPlatformDelta.x;
  }
  if ( ridingPlatformDelta.x < 0 && spr->subVelo.x > ridingPlatformDelta.x ){
    spr->subVelo.x = ridingPlatformDelta.x;
  }
  */

  // Sanity check
  if (Abs(spr->subVelo.x) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's x velo exceeds a full tile size!");
  }

  // did we hit anything on h or v this frame?
  // -1, 0, 1
  int vBonk = 0;
  int hBonk = 0;

  // Apply X velo to X movement + update bounding boxes
  AddSubPos2(spr, spr->subVelo.x + ridingPlatformDelta.x, 0);

  // Eject from any X Collisions
  hBonk = CheckCollisionsAndEject(&xHitInfo, spr, true, ridingPlatformDelta);

  //
  // Y/Horizontal damp, clamp, move, eject
  //

  // Damp Y Velo

  if (iMask & IMASK_PLATFORM_MOVEMENT)
  {

    if (movingDown && !inp->down)
    {

      // clamp it to avoid going into the negative
      if (subDampY > spr->subVelo.y)
      {
        subDampY = spr->subVelo.y;
      }
      subDampY *= -1;
    }
    else if (movingUp && !inp->up)
    {

      if (subDampY > -spr->subVelo.y)
      {
        subDampY = -spr->subVelo.y;
      }
      // already negative, so will be double negative
    }
    else
    {
      subDampY = 0;
    }

    AddVecInts(&spr->subVelo, 0, subDampY);

  } // no-grav

  // Clamp Y Velo

  if (spr->subVelo.y > maxSubSpeedY)
  {
    spr->subVelo.y = maxSubSpeedY;
  }
  else if (spr->subVelo.y < -maxSubSpeedY)
  {
    spr->subVelo.y = -maxSubSpeedY;
  }

  // Sanity check
  if (Abs(spr->subVelo.y) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's y velo exceeds a full tile size!");
  }

  if (!DEBUG_NO_Y)
  {
    // Apply Y velo to Y movement + update bounding boxes
    AddSubPos2(spr, 0, spr->subVelo.y + ridingPlatformDelta.y);

    // Eject from any Y collisions
    vBonk = CheckCollisionsAndEject(&yHitInfo, spr, false, ridingPlatformDelta);

  } // DEBUG_NO_Y

  // if we always eject from X first
  // then we can land slightly in the ground from a high jump
  // which would then start ejecting left/right (not good)
  // if we always do Y first, then we can jump against a wall
  // and start ejecting up and down (also not good)
  // solution: if we're overlapping both at once
  // then eject in the direction that gives us the shortest exit
  // remember: only update x or y where appropriate
  // else you could push against a wall and smush against it indefinitely.

  // snap to x and y
  if (xHitInfo.hitMaskIsSolid && !yHitInfo.hitMaskIsSolid)
  {
    // printf("Frame %d hit on X only\n", frameCounter);
    SetSubPosX(spr, xHitInfo.snapPoint.x);
    spr->subVelo.x = 0;
  }
  else if (!xHitInfo.hitMaskIsSolid && yHitInfo.hitMaskIsSolid)
  {
    // printf("Frame %d hit on Y only\n", frameCounter);
    SetSubPosY(spr, yHitInfo.snapPoint.y);
    spr->subVelo.y = 0;
  }
  else if (xHitInfo.hitMaskIsSolid && yHitInfo.hitMaskIsSolid)
  {

    int xDist = Abs(xHitInfo.snapPoint.x - spr->subPos.x);
    int yDist = Abs(yHitInfo.snapPoint.y - spr->subPos.y);
    printf("Frame %d hit on X and Y dists=%d,%d\n", frameCounter, xDist, yDist);

    if (xDist < yDist)
    {
      // we're less horizontally inside the wall than vertically

      printf("....Snapped on X to: %d/%d , %d/%d\n", spr->subPos.x, spr->subPos.x >> SHIFT, spr->subPos.x, spr->subPos.x >> SHIFT);
      SetSubPosX(spr, xHitInfo.snapPoint.x);
      spr->subVelo.x = 0;

      // re-run the Y hit
      vBonk = CheckCollisionsAndEject(&yHitInfo, spr, false, ZeroVec());
      if (yHitInfo.hitMaskIsSolid)
      {
        printf("....Still got a Y collision to resolve: %d\n", yHitInfo.snapPoint.y);
        SetSubPosY(spr, yHitInfo.snapPoint.y);
        spr->subVelo.y = 0;
      }
      else
      {
        printf("....No y collision to resolve\n");
      }
    }
    else
    {

      // we're less vertically inside the wall than horizontally

      SetSubPosY(spr, yHitInfo.snapPoint.y);
      printf("....Snapped on Y to: %d/%d , %d/%d\n", spr->subPos.x, spr->subPos.x >> SHIFT, spr->subPos.x, spr->subPos.x >> SHIFT);
      spr->subVelo.y = 0;

      // re-run the X hit
      hBonk = CheckCollisionsAndEject(&xHitInfo, spr, true, ZeroVec());
      if (xHitInfo.hitMaskIsSolid)
      {
        printf("....Still got a X collision to resolve: %d\n", xHitInfo.snapPoint.x);
        SetSubPosX(spr, xHitInfo.snapPoint.x);
        spr->subVelo.x = 0;
      }
      else
      {
        printf("....No X collision to resolve\n");
      }
    }
  }

  //
  // Handle tile triggers (liquid, lava, instadeath, etc)
  //
  if (isSolid)
  {
    // player can check against triggers
    // triggers can sit idle

    TileTriggerInfo tileTrigInfo;
    GetTileTriggerOverlaps(spr, &tileTrigInfo);

    //
    // Then process stuff the sprite might be touching
    //
    ProcessTileTouches(spr, &xHitInfo, true);
    ProcessTileTouches(spr, &yHitInfo, false);
    ProcessSpriteTouches(spr, &xHitInfo, true);
    ProcessSpriteTouches(spr, &yHitInfo, false);
  }

  bool knockbackAfterProcessingTouches = SpriteIsKnockback(spr);

  // prevent being knocked back then landing on that thing on the same frame
  bool knockbackThisFrame = knockbackAfterProcessingTouches && !knockbackAtStartofFrame;

  //
  // After any damage is dealt, etc
  //

  GroundHitInfo ghi = {0};
  spr->isGrounded = CheckGrounded(spr, &ghi);

  // Are we riding anything?

  Sprite *standingOn = ghi.otherSpriteOrNull;
  if (standingOn != NULL)
  {
    bool canRide = SpriteCanRideSprite(spr, ghi.otherSpriteOrNull);
    if (canRide)
    {
      spr->thingImRiding = standingOn;
    }
  }

  // For coyote time

  if (spr->isGrounded)
  {
    spr->lastGroundedFrame = frameCounter;
  }

  // printf("Frame %d is grounded %d yVel = %d\n", frameCounter, spr->isGrounded, spr->subVelo.y);
  spr->isOnWall = (vBonk != 0);

  if (isSolid)
  {
    CheckLanded(spr);
    CheckFallen(spr);
  }
  CheckFellOffMap(spr);

  // head or feet bonked
  // prevent jump boost
  if (spr->isGrounded || vBonk != 0)
  {
    // don't need to change move mode, checklanded will have set that
    // special case, clippin the edge of a stair while jumping up

    if (!DontLandCauseImJumping(spr))
    {
      StopJumpBoost(spr, false, "LandedOrHeadBonk (v)");
    }

    StopKnockback(spr, false, "LandedOrHeadBonk (v)");
  }
  if (hBonk != 0 || vBonk < 0)
  {

    if (SpriteIsKnockback(spr) && !knockbackThisFrame)
    {
      // we're dash bouncing, but we hit something new
      // stop
      StopKnockback(spr, true, "H or Head bonk");
    }
  }

  // Keep track of our jump height
  // so we can check if we jumped higher than a one-way platform
  // (or landed on it from above)
  // if not, we don't land on it

  if (spr->isGrounded)
  {
    ResetHighestJumpPoint(spr, "Grounded");
  }
  else
  {
    Vec2 feetPos = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VMID);
    if (feetPos.y < spr->highestYSubPosInJump)
    {
      spr->highestYSubPosInJump = feetPos.y;
    }
  }

  // check triggers
  // let's not allow both on the same frame
  // to avoid e.g. double door transitions
  Sprite *trigger = GetSpriteTriggerOverlaps(spr);

  if (trigger != NULL)
  {
    HandleSpriteTriggers(spr, trigger);
  }

  if (spr->invulnFrameNum > 0)
  {
    spr->invulnFrameNum--;
  }
}

// When riding another sprite, we want its movement to be processed first
// so that we can move by however much it moved
// else we might do our own move tick, then the thing we're riding moves after
void FixSpriteIndices(Sprite *rider, Sprite *thingBeingRidden)
{

  int riderIndex = GetIndexOfSprite(rider);
  int thingBeingRiddenIndex = GetIndexOfSprite(thingBeingRidden);

  if (riderIndex == -1 || thingBeingRiddenIndex == -1)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Can't find sprite indices to swap!");
    return;
  }

  // the rider index must be higher
  if (riderIndex > thingBeingRiddenIndex)
  {
    // already higher, all good
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Swapping sprite indices %d and %d\n", riderIndex, thingBeingRiddenIndex);
  sprites[riderIndex] = thingBeingRidden;
  sprites[thingBeingRiddenIndex] = rider;
}

void MoveAllSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    if (spr != player && DEBUG_ONLY_MOVE_PLAYER)
    {
      continue;
    }
    SolveMovement(spr);
  }

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    if (spr->thingImRiding != NULL)
    {
      FixSpriteIndices(spr, spr->thingImRiding);
    }
  }
}

// TODO: not very efficient
bool IsSpriteOnScreen(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprites can't be on screen");
  }

  BBox camWorldBBox = CameraBBoxWorld();

  // TODO: should we check other points?
  Vec2 testPoint = GetPointOnSprite(spr, false, ANCHOR_HLEFT, ANCHOR_VTOP);
  bool inside = IsPointInsideBox(&testPoint, &camWorldBBox);
  if (inside)
  {
    return true;
  }

  testPoint = GetPointOnSprite(spr, false, ANCHOR_HRIGHT, ANCHOR_VBOTTOM);
  inside = IsPointInsideBox(&testPoint, &camWorldBBox);
  if (inside)
  {
    return true;
  }
  return false;
}

void DrawSprite(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to DrawSprite");
    return;
  }

  // draw based on the actual bounding box
  Vec2 worldBoxPos = spr->worldBBox.vecs.pos;
  Vec2 screenBoxPos = World2Screen(&worldBoxPos);

  bool goingUp = spr->subVelo.y < 0;

  bool isMoving = SpriteIsMoving(spr);
  bool isTryingToMove = (spr->input.left || spr->input.right);
  bool idle = (!isMoving && !isTryingToMove);

  bool dying = (spr->animID == ANIIMTYPE_DIE);

  // TODO: make a table for this
  if (dying)
  {
    SetAnim(spr, ANIIMTYPE_DIE);
  }
  else if (spr->moveMode == MM_STUNNED)
  {
    SetAnim(spr, ANIMTYPE_STUNNED);
  }
  else if (idle)
  {
    SetAnim(spr, ANIMTYPE_IDLE);
  }
  else if (spr->moveMode == MM_DASH)
  {
    SetAnim(spr, ANIMTYPE_DASH);
  }
  else if (spr->moveMode == MM_KNOCKBACK)
  {
    SetAnim(spr, ANIMTYPE_KNOCKBACK);
  }
  else if (spr->moveMode == MM_WALK)
  {
    SetAnim(spr, ANIMTYPE_WALK);
  }
  else if (spr->moveMode == MM_JUMP)
  {
    if (goingUp)
    {
      SetAnim(spr, ANIMTYPE_JUMP);
    }
    else
    {
      SetAnim(spr, ANIMTYPE_FALL);
    }
  }
  else if (spr->moveMode == MM_BUTTDASH || spr->moveMode == MM_BUTTBOUNCE)
  {
    if (goingUp)
    {
      SetAnim(spr, ANIMTYPE_BUTTBOUNCE);
    }
    else
    {
      SetAnim(spr, ANIMTYPE_BUTTSTOMP);
    }
  }
  else if (spr->moveMode == MM_FALL)
  {
    SetAnim(spr, ANIMTYPE_FALL);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d Sprite %s unhandled move state", frameCounter, spr->name);
  }

  UpdateAnimation(spr);
  OnSpriteMoved(spr);

  // Still update, but, don't draw if off screen
  bool onScreen = IsSpriteOnScreen(spr);
  if (!onScreen)
  {
    // vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d Sprite %s is off screen", frameCounter, spr->name);
    return;
  }

  // update the img pointer, in case it's changed due to anims
  const Img *img = GetActiveImage(spr);
  uint8_t *imgData = ImgData(img);

  vmupro_drawflags_t flags = (!spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H);
  uint16_t mask = *(uint16_t *)&imgData[0];
  vmupro_blit_buffer_transparent(imgData, screenBoxPos.x, screenBoxPos.y, img->width, img->height, mask, flags);
}

void DrawAllSprites()
{
  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    // draw the player last so
    // - you don't end up stuck behind stuff
    // - any platform you're riding can update before you
    if (spr == player)
    {
      continue;
    }

    DrawSprite(spr);
  }
  DrawSprite(player);
}

void DrawDebugAllSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];

    if (DEBUG_SPRITEBOX)
    {
      DrawSpriteBoundingBox(spr, VMUPRO_COLOR_WHITE);
    }
    if (DEBUG_HITBOX)
    {
      DrawSpriteHitBox(spr, VMUPRO_COLOR_WHITE);
    }
  }

  if (DEBUG_SCROLL_ZONE)
  {
    DrawCamScrollZone();
  }
}

void GotoGameState(GameState inState)
{

  if (gState == inState)
  {
    vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d, Attempt to switch to same game state: %d\n", frameCounter, inState);
  }

  gState = inState;
  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d switched to game state %d", frameCounter, (int)inState);

  uiAnimOffset = ZeroVec();
  uiAnimVelo = ZeroVec();
  uiStateFrameCounter = 0;
}

void DrawUIElementCenteredWithVelo(const Img *img)
{

  uint8_t *imgData = ImgData(img);

  vmupro_drawflags_t flags = VMUPRO_DRAWFLAGS_NORMAL;
  uint16_t mask = *(uint16_t *)&imgData[0];
  int x = SCREEN_WIDTH / 2 - img->width / 2;
  x += uiAnimOffset.x;
  int y = SCREEN_HEIGHT / 2 - img->height / 2;
  y += uiAnimOffset.y;
  vmupro_blit_buffer_transparent(imgData, x, y, img->width, img->height, mask, flags);
}

void DrawUI()
{

  if (gState == GSTATE_UNINIT)
  {
    return;
  }

  if (gState == GSTATE_START)
  {

    // TODO: very temporary code
    // Draw the temporary "start" graphic
    const Img *img = &img_ui_temp_start;
    DrawUIElementCenteredWithVelo(img);

    vmupro_btn_read();
    if (vmupro_btn_confirm_released() || vmupro_btn_dismiss_released())
    {
      GotoGameState(GSTATE_INTROFADE);
      // apply a small upward force to the ui
      // before gravity kicks in
      uiAnimVelo.y = -2;
    }
    return;
  }

  if (gState == GSTATE_INTROFADE)
  {

    const Img *img = &img_ui_temp_start;
    DrawUIElementCenteredWithVelo(img);

    uiAnimOffset.y += uiAnimVelo.y;
    // add gravity
    uiAnimVelo.y += 2;

    if (uiAnimOffset.y > SCREEN_HEIGHT)
    {
      GotoGameState(GSTATE_INGAME);
    }
    return;
  }

  if (gState == GSTATE_DED)
  {

    const Img *img = &img_ui_temp_dead;
    DrawUIElementCenteredWithVelo(img);

    if (uiStateFrameCounter >= POST_DEATH_FRAME_DELAY)
    {
      vmupro_btn_read();
      if (vmupro_btn_confirm_released() || vmupro_btn_dismiss_released())
      {
        Retry();
      }
    }
  }

  if (gState == GSTATE_TRANSITION)
  {
    const Img *img = &img_ui_temp_transition;
    DrawUIElementCenteredWithVelo(img);

    if (uiStateFrameCounter >= TRANSITION_FRAME_DELAY)
    {
      GotoGameState(GSTATE_INGAME);
    }
  }

  if (gState == GSTATE_GAMEOVER)
  {

    const Img *img = &img_ui_temp_gameover;
    DrawUIElementCenteredWithVelo(img);

    if (uiStateFrameCounter >= POST_DEATH_FRAME_DELAY)
    {
      vmupro_btn_read();
      if (vmupro_btn_confirm_released() || vmupro_btn_dismiss_released())
      {
        InitGame();
      }
    }
  }
}

void SpawnDuckAboveMe(Sprite *srcSprite)
{

  Vec2 srcHeadWorld = GetPointOnSprite(srcSprite, false, ANCHOR_HMID, ANCHOR_VTOP);
  srcHeadWorld.y -= TILE_SIZE_PX * 3;
  CreateSprite(STYPE_GREENDUCK, srcHeadWorld, "TEST DUCK");
}

void DespawnSprite(Sprite *spr)
{

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Despawning sprite %s", spr->name);

  int idx = UnloadSprite(spr);

  if (idx == -1)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite %s has no valid index!", spr->name);
    return;
  }

  // shuffle everything down by one
  for (int i = idx; i < numSprites; i++)
  {
    sprites[i] = sprites[i + 1];
  }
  // clear the end one
  sprites[numSprites] = NULL;
  // and reduce the sprite count
  numSprites--;
}

void DespawnAllMarkedSprites()
{

  for (int i = numSprites - 1; i >= 0; i--)
  {
    Sprite *spr = sprites[i];

    if (spr->despawnTimer > 0)
    {
      spr->despawnTimer--;
      if (spr->despawnTimer == 0)
      {
        MarkSpriteForDespawn(spr, "timer");
      }
    }

    if (spr->markedForDespawn)
    {
      DespawnSprite(spr);
    }
  }
}

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "8BM Platformer Example");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  InitGame();

  while (true)
  {

    vmupro_color_t col = VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    SolveCamera();

    DrawBackground();
    DrawGroundtiles(LAYER_BG);
    DrawGroundtiles(LAYER_COLS);

    InputAllSprites();
    MoveAllSprites();
    DrawAllSprites();
    EndFrameAllSprites();

    DrawUI();

    DrawDebugAllSprites();
    DespawnAllMarkedSprites();

    vmupro_push_double_buffer_frame();

    vmupro_sleep_ms(10);

    if (vmupro_btn_held(Btn_Mode))
    {
      Retry();
    }

    // test: cycle through sprite offsets
    if (vmupro_btn_pressed(Btn_A))
    {
      // printf("PlayerX world:%d sub: %d \n", GetWorldPos(player).x, GetSubPos(player).x);
      // printf("PlayerY world:%d sub: %d \n", GetWorldPos(player).y, GetSubPos(player).y);
      // printf("Player grounded? %d\n", (int)player->isGrounded);
      // LoadLevel(1);
      // SpawnDuckAboveMe(player);
      // Vec2 playerWorld = Sub2World(&player->subPos);
      // CreateSprite(STYPE_PARTICLE_BROWN, playerWorld, "TEST PARTICLE");
    }

    frameCounter++;
    uiStateFrameCounter++;
    // re-seed the rng
    rng = GetRNG(1);
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
