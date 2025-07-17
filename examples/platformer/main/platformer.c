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
const bool DEBUG_NO_X = false;
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
#define MAP_WIDTH_TILES 128
#define MAP_HEIGHT_TILES 64
#define MAP_WIDTH_PIXELS (MAP_WIDTH_TILES * TILE_SIZE_PX)
#define MAP_HEIGHT_PIXELS (MAP_HEIGHT_TILES * TILE_SIZE_PX)

// Hard coded rows in the tilemap for transparency and one-way interaction
#define TILEMAP_ONEWAY_PLATFORM_ROW_0 0
#define TILEMAP_ONEWAY_PLATFORM_ROW_11 11
#define TILEMAP_TRANSPARENT_ROW_11 11
#define TILEMAP_TRANSPARENT_ROW_12 12

#define BLOCK_NULL 0xFF

const int DEFAULT_LIFE_COUNT = 3;
// give it a few frames before you can continue
const int POST_DEATH_FRAME_DELAY = 60;

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
  GSTATE_DED,
  GSTATE_GAMEOVER

} GameState;

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
  bool up;
  bool down;
  bool left;
  bool right;
  bool run;
  bool jump;
} Inputs;

typedef enum
{

  MM_FALL,
  MM_WALK,
  MM_RUN,
  MM_JUMP,

} MoveMode;

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
  STYPE_TESTMOB,
  STYPE_RESERVED_2,
  STYPE_RESERVED_3,
  STYPE_RESERVED_4,
  STYPE_RESERVED_5,
  STYPE_RESERVED_6,
  STYPE_RESERVED_7,
  STYPE_RESERVED_8,
  STYPE_RESERVED_9,
  STYPE_RESERVED_10,
  STYPE_RESERVED_11,
  STYPE_RESERVED_12,
  STYPE_RESERVED_13,
  STYPE_RESERVED_14,
  STYPE_RESERVED_15,
  // second row of sprites in tilemap
  STYPE_AREA_TOPLEFT,
  STYPE_AREA_TOPRIGHT,
  STYPE_AREA_BOTTOMLEFT,
  STYPE_AREA_BOTTOMRIGHT

} SpriteType;

typedef enum
{
  SOLIDMASK_NONE,     // non solid
  SOLIDMASK_TILE,     // solid: tile
  SOLIDMASK_SOLID,    // solid: other creature
  SOLIDMASK_ONESIDED, // solid: one-way platform
  SOLIDMASK_PLATFORM  // solid: moving platform
} Solidity;

// profile of spite behaviour
// such as runspeed, can it walk
// off edges, etc
// prefix "sub" means subpixels/fixed point math
// prefix "world" means regular world space 1:1 pixels
typedef struct
{

  // maximum speed (in subpixels) while
  // walking or running (per frame)
  int max_subspeed_walk;
  int max_subspeed_run;

  // accel values when walking, running, etc
  // in subpixels
  int subaccel_walk;
  int subaccel_run;
  int subaccel_air;

  // movement damping values
  int subdamping_walk;
  int subdamping_run;
  int subdamping_air;

  // max frames for which the up force
  // is applied
  int max_jump_boost_frames;
  int sub_jumpforce;
  int sub_gravity;
  // max of like 256 since that's
  // bigger than a tile in subpixels
  int max_subfallspeed;

  int default_health;

  Solidity solid;

} SpriteProfile;
// TODO: rename to spriteblueprint?

typedef struct
{
  const char *name;
  TileLayer *bgLayer;
  TileLayer *colLayer;

} Level;

Level level0 = {
    "Duck Fails",
    &tl_level_0_layer_0,
    &tl_level_0_layer_1,
};

Level level1 = {
    "Duck & Cover",
    &tl_level_1_layer_0,
    &tl_level_1_layer_1,
};

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
  p->max_subspeed_walk = 80;
  p->max_subspeed_run = 140;

  p->subaccel_walk = 8;
  p->subaccel_run = 9;
  p->subaccel_air = 6;

  p->subdamping_walk = 6;
  p->subdamping_run = 6;
  p->subdamping_air = 4;

  p->max_jump_boost_frames = 16;
  p->sub_jumpforce = 14;
  p->sub_gravity = 9;
  p->max_subfallspeed = 120;

  p->solid = SOLIDMASK_SOLID;

  p->default_health = 3;

  if (inType == STYPE_PLAYER)
  {
    // default
  }
  else if (inType == STYPE_TESTMOB)
  {
    p->max_subspeed_walk = 10;
    p->subaccel_walk = 1;
    p->subdamping_walk = 0;
    p->solid = SOLIDMASK_PLATFORM;
    p->default_health = 1;
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "No sprite profile for sprite type %d", (int)inType);
  }
}

typedef struct
{

  // Config options
  // from which the rest of the struct is calculated

  SpriteType sType;
  SpriteProfile profile;
  char name[10];
  Vec2 subSpawnPos;

  // Runtime stuff
  // calculated via ResetSprite()

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

  bool isGrounded;
  bool isOnWall;

  bool onGroundLastFrame;
  bool onWallLastFrame;
  int jumpFrameNum;

  // config options
  bool isPlayer;
  Anchor_H anchorH;
  Anchor_V anchorV;

  bool facingRight;
  bool wasRunningLastTimeWasOnGround;

  Inputs input;
  MoveMode moveMode;

  AnimGroup *anims;
  AnimFrames *activeFrameSet;
  // updated per frame, to measure elapsed frames
  // TODO: rename to like gameFrameWhenAnimChanged
  int lastGameframe;
  // reset on anim changes
  int animIndex;
  bool animReversing;
  AnimTypes animID;

  int health;

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

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s Anim ID %d", frameCounter, spr->name, (int)inType);

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
  default:
    // we'll patch this in a sec
    spr->activeFrameSet = NULL;
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled animation index %d\n", (int)inType);
    break;
  }

  // Nothing found, switch to default/idle, which everything should have
  if (spr->activeFrameSet == NULL)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Sprite has no anim for type %d\n", (int)inType);
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

  AnimFrames *frameSet = spr->activeFrameSet;
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

//
// Protos
//
//__TEST__ This can be scrubbed when things are moved to headers
Solidity CheckGrounded(Sprite *spr);
Vec2 GetPointOnSprite(Sprite *spr, bool hitBox, Anchor_H anchorH, Anchor_V anchorV);
Solidity CheckSpriteCollision(Sprite *spr, Direction dir, Vec2 *subOffset, const char *src);
void GotoGameState(GameState inState);
void UnloadTileLayers();
void DecompressAllTileLayers(Level *currentLevel, PersistentData *inData);

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
  if (mm == MM_RUN || mm == MM_WALK)
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

void SetMoveMode(Sprite *spr, MoveMode inMode)
{

  MoveMode oldMode = inMode;
  spr->moveMode = inMode;

  if (oldMode != inMode)
  {
    vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Move mode %d", frameCounter, (int)inMode);
  }
}

// Player spawn pos
Vec2 GetPlayerWorldStartPos()
{
  Vec2 rVal = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 36)};
  return rVal;
}

void OnSpriteDied(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "NULL sprites can't die");
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Sprite %s died. RIP", frameCounter, spr->name);

  SetAnim(spr, ANIIMTYPE_DIE);

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
    printf("TODO: non-player sprite death\n");
  }
}

void SpriteTakeDamage(Sprite *spr, int value, Sprite *sourceOrNull)
{

  const char *srcName = sourceOrNull != NULL ? sourceOrNull->name : "WORLD";

  if (value > spr->health)
  {
    value = spr->health;
  }

  spr->health -= value;

  if (spr->health <= 0)
  {
    OnSpriteDied(spr);
  }
  else
  {
    printf("TODO: pushback damage\n");
  }
}

void CheckFellOffMap(Sprite *spr)
{

  Vec2 topleftPoint = GetPointOnSprite(spr, false, spr->anchorH, spr->anchorV);
  if (topleftPoint.y < MAP_HEIGHT_PIXELS + TILE_SIZE_PX)
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d Sprite %s fell off map at yPos %d\n", frameCounter, spr->name, spr->subPos.y);

  SpriteTakeDamage(spr, spr->health, NULL);
}

void ResetSprite(Sprite *spr)
{

  // update other struct vals
  spr->facingRight = true;
  spr->wasRunningLastTimeWasOnGround = false;

  spr->subPos = ZeroVec();
  spr->lastSubPos = ZeroVec();

  spr->subVelo = ZeroVec();
  spr->lastSubVelo = ZeroVec();

  SetMoveMode(spr, MM_FALL);

  spr->isGrounded = false;
  spr->isOnWall = false;
  spr->onGroundLastFrame = false;
  spr->onWallLastFrame = false;
  spr->jumpFrameNum = 0;

  // temporary config stuff
  spr->subPos = spr->subSpawnPos;
  spr->isPlayer = true;
  spr->anchorH = ANCHOR_VTOP;
  spr->anchorV = ANCHOR_HLEFT;

  spr->anims = &animgroup_player;
  spr->activeFrameSet = &spr->anims->idleFrames;
  spr->animIndex = 0;
  spr->animReversing = false;
  spr->lastGameframe = frameCounter;
  spr->animID = ANIMTYPE_IDLE;

  // Reset/create the movement profile
  CreateProfile(&spr->profile, spr->sType);

  // And apply anything that's determined from the profile
  spr->health = spr->profile.default_health;

  // update the hitbox, bounding box, etc
  OnSpriteMoved(spr);
}

Sprite *CreateSprite(SpriteType inType, Vec2 worldStartPos, const char *inName)
{

  if (numSprites == MAX_SPRITES)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Out of sprite slots");
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

  sprites[numSprites] = returnVal;
  numSprites++;

  return returnVal;
}

// TODO: placeholder for anything malloced
void UnloadSprite(Sprite *spr)
{
  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Attempt to unload a null sprite!");
    return;
  }
  vmupro_log(VMUPRO_LOG_ERROR, TAG, "Frame %d Sprite %s unloading", frameCounter, spr->name);
  free(spr);
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

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Unloaded level & sprites");
}

void LoadLevel(int inLevelNum)
{

  UnloadLevel();

  // TODO: bounds checking?
  // but then we wouldn't get minus worlds.
  currentLevel = allLevels[inLevelNum];

  pData.levelNum = inLevelNum;
  DecompressAllTileLayers(currentLevel, &pData);

  player = CreateSprite(STYPE_PLAYER, GetPlayerWorldStartPos(), "player");

  if (!DEBUG_ONLY_SPAWN_PLAYER)
  {
    Vec2 testPos = GetPlayerWorldPos();
    testPos.x += TILE_SIZE_PX * 10;
    testPos.y -= TILE_SIZE_PX * 4;
    CreateSprite(STYPE_TESTMOB, testPos, "testmob1");

    testPos = GetPlayerWorldPos();
    testPos.x += TILE_SIZE_PX * 4;
    testPos.y -= TILE_SIZE_PX * 1;
    CreateSprite(STYPE_TESTMOB, testPos, "testmob2");
  }
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

uint32_t CalcDJB2(uint8_t *inBytes, uint32_t inLength)
{

  uint32_t returnVal = 5381;
  for (int i = 0; i < inLength; i++)
  {
    returnVal = (returnVal << 5) + returnVal + inBytes[i];
  }
  return returnVal;
}

bool RLE16BitDecode(uint8_t *inBytes, uint32_t inLength, uint8_t *outBytes, uint32_t outLength)
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

bool RLE8BitDecode(uint8_t *inBytes, uint32_t inLength, uint8_t *outBytes, uint32_t outLength)
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

void DecompressImage(Img *img)
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
uint8_t *ImgData(Img *img)
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

uint8_t *DecompressTileLayer(TileLayer *inLayer)
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

  decompressedTileLayerTable[0] = DecompressTileLayer(inLevel->bgLayer);
  decompressedTileLayerTable[1] = DecompressTileLayer(inLevel->colLayer);
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

void UpdatePatrollInputs(Sprite *spr)
{

  // might lose ground and bonk on the same frame
  // don't want to trigger both
  bool originallyFacingRight = spr->facingRight;

  // 1: check if we're going to run out of ground

  // offset a bit based on where we're going
  // a tile should do, since it gives time
  // to dampen and change direction smoothly
  Vec2 subOffset = {originallyFacingRight ? TILE_SIZE_SUB : -TILE_SIZE_SUB, 1};
  bool groundAhead = CheckSpriteCollision(spr, DIR_DOWN, &subOffset, "patrol_ground") != SOLIDMASK_NONE;

  // 2: check if we'd bonk into something

  Direction dir = spr->facingRight ? DIR_RIGHT : DIR_LEFT;
  Vec2 wallSubOffset = {dir == DIR_RIGHT ? TILE_SIZE_SUB / 2 : -TILE_SIZE_SUB / 2, 0};
  bool bonk = CheckSpriteCollision(spr, dir, &wallSubOffset, "patrol_wall") != SOLIDMASK_NONE;

  // Turn around
  if (bonk || !groundAhead)
  {
    spr->facingRight = !originallyFacingRight;
  }

  // Apply the new values

  spr->input.right = spr->facingRight;
  spr->input.left = !spr->facingRight;
}

bool SpriteIsMoving(Sprite *spr)
{

  return (spr->subVelo.x != 0) || (spr->subVelo.y != 0);
}

bool SpriteIsDead(Sprite *spr)
{
  return spr->animID == ANIIMTYPE_DIE;
}

bool AllowSpriteInput(Sprite *spr)
{

  if (SpriteIsDead(spr))
    return false;
  // TODO: knockback
  // TODO: some kinda sequence
  return true;
}

void UpdateSpriteInputs(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to UpdateSpriteInputs");
    return;
  }

  // Zero out the inputs
  Inputs *inp = &spr->input;
  memset(&spr->input, 0, sizeof(Inputs));

  // if we're not in a playable state
  // just early exit since the inputs are wiped
  if (!AllowSpriteInput(spr))
  {
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

    vmupro_btn_read();

    inp->up = vmupro_btn_held(DPad_Up);
    inp->down = vmupro_btn_held(DPad_Down);
    inp->left = vmupro_btn_held(DPad_Left);
    inp->right = vmupro_btn_held(DPad_Right);
    inp->jump = vmupro_btn_held(Btn_B);
    inp->run = vmupro_btn_held(Btn_A);
  }
  else if (spr->sType == STYPE_TESTMOB)
  {

    UpdatePatrollInputs(spr);
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled sprite type in UpdateSpriteInputs");
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

  // bit of a hack, but hey, everything on rowss 11 and 12 are transparent
  if (inBlockRow == TILEMAP_TRANSPARENT_ROW_11 || inBlockRow == TILEMAP_TRANSPARENT_ROW_12)
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

  // check if cam's going off left of the level
  int camLeft = snapWorldPos.x; // - (SCREEN_WIDTH / 2);
  if (camLeft < 0)
    camLeft = 0;

  // check if cam's going off right of the level
  int camRight = camLeft + SCREEN_WIDTH;
  if (camRight >= MAP_WIDTH_PIXELS)
  {
    int delta = camRight - MAP_WIDTH_PIXELS;
    camLeft -= delta;
  }

  // check if cam's going off the top of the level
  // player's about 3/4 of the way down the screen
  int camTop = snapWorldPos.y - (SCREEN_WIDTH / 2);
  if (camTop < 0)
  {
    camTop = 0;
  }

  // check if cam's going off the bottom of the level
  int camBottom = camTop + SCREEN_HEIGHT;
  if (camBottom >= MAP_HEIGHT_PIXELS)
  {
    int delta = camBottom - MAP_HEIGHT_PIXELS;
    camTop -= delta;
  }

  camX = camLeft;
  camY = camTop;
}

void DrawBackground()
{

  Img *img = &img_bg_1;

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

int GetXSubAccel(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  SpriteProfile *prof = &spr->profile;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Accel: Unhandled movement mode: %d", (int)mMode);
    return prof->subaccel_walk;
    break;

  case MM_JUMP:
  case MM_FALL:
    return prof->subaccel_air;
    break;

  case MM_WALK:
    return prof->subaccel_walk;
    break;
  case MM_RUN:
    return prof->subaccel_run;
    break;
  }
}

int GetMaxXSubSpeed(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  SpriteProfile *prof = &spr->profile;
  bool wasRunningWhenLastGrounded = spr->wasRunningLastTimeWasOnGround;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Subspeed: Unhandled movement mode: %d", (int)mMode);
    return prof->max_subspeed_walk;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      return prof->max_subspeed_run;
    }
    else
    {
      return prof->max_subspeed_walk;
    }

    break;

  case MM_WALK:
    return prof->max_subspeed_walk;
    break;

  case MM_RUN:
    return prof->max_subspeed_run;
    break;
  }
}

int GetXDamping(Sprite *spr)
{

  MoveMode mMode = spr->moveMode;
  SpriteProfile *prof = &spr->profile;
  bool wasRunningWhenLastGrounded = spr->wasRunningLastTimeWasOnGround;

  switch (mMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Damping: Unhandled movement mode: %d", (int)mMode);
    return prof->subdamping_walk;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      return prof->subdamping_run;
    }
    else
    {
      return prof->subdamping_walk;
    }

    break;

  case MM_WALK:
    return prof->subdamping_walk;
    break;

  case MM_RUN:
    return prof->subdamping_run;
    break;
  }
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

  // Used to work out which pivot points
  // to use
  Direction whereWasCollision;

  // the points we'll look up
  // e.g. top row, bottom row, etc
  // used to work out the bounding box collision
  // check points
  Anchor_H anchorH[3];
  Anchor_V anchorV[3];

  //
  // Block hit info
  //

  int lastBlockHitIndex; // 0-2

  // the point(s) we're checking for collision
  // e.g. for moving down we'd use 3:
  // bottom left, bottom middle, bottom right
  Vec2 subCheckPos[3];

  // top left to bottom right
  // e.g. top left, top middle, top right
  // or top left, middle left, bottom left
  // -1 for nothing
  int blockID[3];

  //
  // Sprite hit info:
  //

  int lastSpriteHitIndex; // 0-2

  // the ejection edge for the block we hit
  // e.g. if we're moving right, it'll be
  // - the x coord of the block's left edge
  // - the y coord of the hitbox point we're checking
  Vec2 blockSubEjectPoint[3];

  // other sprites we might have hit
  Sprite *otherSprites[3];
  Vec2 spriteSubEjectPoint[3];
  Solidity spriteSolidity[3];

  //
  // Shared hit info (blocks/sprites)
  //

  Solidity hitMask;
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

Vec2 GetTileSubPosFromRowAndCol(Vec2 *rowAndcol)
{

  Vec2 returnVal;
  returnVal.x = rowAndcol->x * TILE_SIZE_SUB;
  returnVal.y = rowAndcol->y * TILE_SIZE_SUB;
  return returnVal;
}

bool IsBlockOneWay(int blockId)
{

  int rowNum = blockId / TILEMAP_WIDTH_TILES;

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

bool Ignore2SidedBlock(int blockId, int layer, Sprite *spr, Vec2 *tileSubPos)
{

  // if it's not 2-sideable, just early exit
  if (!IsBlockOneWay(blockId))
    return false;

  bool movingUp = spr->subVelo.y < 0;
  bool movingHorz = spr->subVelo.x != 0;
  bool movingVert = spr->subVelo.y != 0;

  // Opt 1:
  // We've moving horizointally through one
  // e.g. we're on the ground, and it's around head height
  // we pass through it as long as it's *above* our feet
  // so the ground is unaffected
  // tldr; walking through it horizontally

  Vec2 subFootPos = GetPointOnSprite(spr, true, ANCHOR_HMID, ANCHOR_VBOTTOM);
  bool higher = tileSubPos->y < subFootPos.y;

  if (movingHorz && !movingVert && higher)
  {
    return true;
  }

  // Opt2:
  // We're just jumping through it

  if (movingUp)
  {
    return true;
  }

  // if (movingDown) return false;

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

// subOffsetOrNull adds an offset to where we check for collisions
// e.g. when moving right we check currentPos.x + velo.x
// for where we'll be, rather than where we are
// Used for for collision and for ground checks
void GetHitInfo(HitInfo *rVal, Sprite *spr, Direction dir, Vec2 *subOffsetOrNull, const char *src)
{

  rVal->whereWasCollision = dir;
  rVal->lastBlockHitIndex = -1;
  rVal->lastSpriteHitIndex = -1;

  // get a list of points to check for
  // whatever direction we're moving
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

  // finally run some tile collision checks:
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

      bool ignore = Ignore2SidedBlock(blockId, LAYER_COLS, spr, &tileSubPos);

      if (ignore)
      {
        // printf("ignoring 2 sided block\n");
        continue;
      }

      rVal->hitMask |= SOLIDMASK_TILE;

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
  // Check sprite/sprite collisions
  // separate loop to simplify logic
  // (early continues, etc)
  //

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

      Solidity otherSolid = otherSprite->profile.solid;

      // ignore nonsolid stuff
      if (otherSolid == SOLIDMASK_NONE)
        continue;

      bool inside = SubPointInHitbox(&rVal->subCheckPos[i], otherSprite);

      if (!inside)
        continue;

      // printf("Sprite %s is inside %s\n", spr->name, otherSprite->name);

      // mask it so we know we hit something
      rVal->hitMask |= otherSolid;
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
  if (!info->hitMask)
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
      bool groundedNow = CheckGrounded(spr);
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
int GetHitInfoAndEjectionInfo(HitInfo *info, Sprite *spr, bool horz)
{

  memset(info, 0, sizeof(HitInfo));

  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;
  bool movingDown = spr->subVelo.y > 0;
  bool movingUp = spr->subVelo.y < 0;

  // We're not using prediction by default
  // but it could later be applied
  Vec2 subCheckOffset = {0, 0};

  Direction dir = DIR_RIGHT;
  if (horz)
  {
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
Solidity CheckSpriteCollision(Sprite *spr, Direction dir, Vec2 *subOffset, const char *src)
{

  HitInfo nhi;
  memset(&nhi, 0, sizeof(HitInfo));
  GetHitInfo(&nhi, spr, dir, subOffset, src);

  return nhi.hitMask;
}

Solidity CheckGrounded(Sprite *spr)
{
  // hitbox ends on the very last subpixel
  // so adding one takes you into the next tile
  Vec2 offset = {0, 1};
  HitInfo nhi;
  memset(&nhi, 0, sizeof(HitInfo));
  GetHitInfo(&nhi, spr, DIR_DOWN, &offset, "checkgrounded");
  return nhi.hitMask;
}

// the first part of the jump, triggering it
void TryJump(Sprite *spr)
{

  if (!spr->input.jump)
  {
    return;
  }

  if (SpriteJumping(spr))
  {
    return;
  }
  if (!spr->isGrounded)
  {
    return;
  }

  SetMoveMode(spr, MM_JUMP);
  spr->jumpFrameNum = 0;
}

// prevent the jump button applying further up force
void StopJumpBoost(Sprite *spr, const char *src)
{
  if (spr->jumpFrameNum < spr->profile.max_jump_boost_frames)
  {
    printf("__DBG__ jump boost canceled, src='%s'\n", src);
    spr->jumpFrameNum = spr->profile.max_jump_boost_frames;
  }
}

void TryContinueJump(Sprite *spr)
{

  if (!SpriteJumping(spr))
  {
    return;
  }
  if (spr->jumpFrameNum >= spr->profile.max_jump_boost_frames)
  {
    return;
  }

  // we're jumping, and we've not run out of boost frames

  if (spr->input.jump)
  {
    // add jump velo
    spr->subVelo.y -= spr->profile.sub_jumpforce;
    // printf("__DBG__ Frame %d Applying jump on frame %d - velo %d\n", frameCounter, spr->jumpFrameNum, spr->subVelo.y);
    spr->jumpFrameNum++;
  }
  else
  {
    // user has released jump, prevent further re-presses
    // until we land
    StopJumpBoost(spr, "ReachedFrameMax");
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
  SetMoveMode(spr, MM_FALL);
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

  bool someKindaOffTheGround = (!spr->onGroundLastFrame) || MoveModeInAir(spr);
  if (!someKindaOffTheGround)
  {
    return;
  }

  if (false)
  {
    printf("__DBG__ Frame %d landed @ %d/%d\n", frameCounter, spr->subPos.y, spr->subPos.y >> SHIFT);
  }
  // we're on the ground
  // we weren't during the last frame
  if (spr->input.run)
  {
    SetMoveMode(spr, MM_RUN);
  }
  else
  {
    SetMoveMode(spr, MM_WALK);
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

  // if the sprite is dead, we can skip a lot of
  // the movement logic
  if (spr->animID == ANIIMTYPE_DIE)
  {

    AddVecInts(&spr->subVelo, 0, spr->profile.sub_gravity);
    AddVec(&spr->subPos, &spr->subVelo);

    return;
  }

  Inputs *inp = &spr->input;

  // read in all of our state values
  // (the result of the previous frame)
  // i.e. we don't want to check the grounded state now
  // then perform a jump + land in the same frame.

  MoveMode mm = spr->moveMode;
  bool inAir = MoveModeInAir(spr);
  bool isJumping = SpriteJumping(spr);

  int maxSubSpeedX = GetMaxXSubSpeed(spr);
  int maxSubSpeedY = player->profile.max_subfallspeed;

  int subAccelX = GetXSubAccel(spr);
  int subAccelY = spr->isGrounded ? 0 : player->profile.sub_gravity;

  int subDampX = GetXDamping(spr);
  int subDampY = 0;

  TryJump(spr);
  TryContinueJump(spr);

  bool spriteXInput = inp->right || inp->left;
  bool spriteYInput = inp->up || inp->down;
  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;
  bool movingUp = spr->subVelo.y < 0;
  bool movingDown = spr->subVelo.y > 0;

  if (inp->right && !spr->facingRight)
  {
    spr->facingRight = true;
  }
  if (inp->left && spr->facingRight)
  {
    spr->facingRight = false;
  }

  // Debug/testing
  if (DEBUG_NO_GRAV)
  {
    subDampY = 1;

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

  // Sanity check
  if (Abs(spr->subVelo.x) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's x velo exceeds a full tile size!");
  }

  // did we hit anything on h or v this frame?
  // -1, 0, 1
  int vBonk = 0;
  int hBonk = 0;

  if (!DEBUG_NO_X)
  {
    // Apply X velo to X movement + update bounding boxes
    AddSubPos2(spr, spr->subVelo.x, 0);

    // Eject from any X Collisions
    hBonk = GetHitInfoAndEjectionInfo(&xHitInfo, spr, true);

  } // DEBUG_NO_X

  //
  // Y/Horizontal damp, clamp, move, eject
  //

  // Damp Y Velo

  if (DEBUG_NO_GRAV)
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
    AddSubPos2(spr, 0, spr->subVelo.y);

    // Eject from any Y collisions
    vBonk = GetHitInfoAndEjectionInfo(&yHitInfo, spr, false);

  } // DEBUG_NO_Y

  // if we always eject from X first
  // then we can land slightly in the ground from a high jump
  // then start ejecting left/right
  // if we always do Y first, then we can jump against a wall
  // and start ejecting up and down
  // solution: if we're overlapping both at once
  // then eject in the direction that gives us the shortest exit
  // remember: only update x or y where appropriate
  // else you could push against a wall and smush against it indefinitely.

  // snap to x and y
  if (xHitInfo.hitMask && !yHitInfo.hitMask)
  {
    // printf("Frame %d hit on X only\n", frameCounter);
    SetSubPosX(spr, xHitInfo.snapPoint.x);
    spr->subVelo.x = 0;
  }
  else if (!xHitInfo.hitMask && yHitInfo.hitMask)
  {
    printf("Frame %d hit on Y only\n", frameCounter);
    SetSubPosY(spr, yHitInfo.snapPoint.y);
    spr->subVelo.y = 0;
  }
  else if (xHitInfo.hitMask && yHitInfo.hitMask)
  {

    int xDist = Abs(xHitInfo.snapPoint.x - spr->subPos.x);
    int yDist = Abs(yHitInfo.snapPoint.y - spr->subPos.y);
    printf("Frame %d hit on X and Y dists=%d,%d\n", frameCounter, xDist, yDist);

    if (xDist < yDist)
    {
      printf("....Snapped on Y to: %d/%d , %d/%d\n", spr->subPos.x, spr->subPos.x >> SHIFT, spr->subPos.x, spr->subPos.x >> SHIFT);
      SetSubPosX(spr, xHitInfo.snapPoint.x);
      spr->subVelo.x = 0;

      // re-run the Y hit
      vBonk = GetHitInfoAndEjectionInfo(&yHitInfo, spr, false);
      if (yHitInfo.hitMask)
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
      SetSubPosY(spr, yHitInfo.snapPoint.y);
      printf("....Snapped on Y to: %d/%d , %d/%d\n", spr->subPos.x, spr->subPos.x >> SHIFT, spr->subPos.x, spr->subPos.x >> SHIFT);
      spr->subVelo.y = 0;

      // re-run the X hit
      hBonk = GetHitInfoAndEjectionInfo(&xHitInfo, spr, true);
      if (xHitInfo.hitMask)
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

  spr->isGrounded = CheckGrounded(spr) != SOLIDMASK_NONE;
  // printf("Frame %d is grounded %d yVel = %d\n", frameCounter, spr->isGrounded, spr->subVelo.y);
  spr->isOnWall = (vBonk != 0);

  CheckLanded(spr);
  CheckFallen(spr);
  CheckFellOffMap(spr);

  // head or feet bonked
  // prevent jump boost
  if (spr->isGrounded && vBonk != 0)
  {
    StopJumpBoost(spr, "LandedOrHeadBonk");
  }
}

void MoveAllSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    if (i > 0 && DEBUG_ONLY_MOVE_PLAYER)
      continue;
    Sprite *spr = sprites[i];
    SolveMovement(spr);
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

  if (dying)
  {
    SetAnim(spr, ANIIMTYPE_DIE);
  }
  else if (idle)
  {
    SetAnim(spr, ANIMTYPE_IDLE);
  }
  else
  {
    if (spr->moveMode == MM_WALK)
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
    else if (spr->moveMode == MM_FALL)
    {
      SetAnim(spr, ANIMTYPE_FALL);
    }
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
    DrawSprite(spr);
  }
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

    vmupro_push_double_buffer_frame();

    vmupro_sleep_ms(10);

    if (vmupro_btn_held(Btn_Mode))
    {
      break;
    }

    // test: cycle through sprite offsets
    if (vmupro_btn_pressed(Btn_A))
    {
      // printf("PlayerX world:%d sub: %d \n", GetWorldPos(player).x, GetSubPos(player).x);
      // printf("PlayerY world:%d sub: %d \n", GetWorldPos(player).y, GetSubPos(player).y);
      // printf("Player grounded? %d\n", (int)player->isGrounded);
      LoadLevel(1);
    }

    if (vmupro_btn_pressed(Btn_B))
    {
      // player->anchorV = (player->anchorV + 1) % (3);
      //  player->anchorH = (player->anchorH + 1) % (3);
      // OnSpriteMoved(player);
    }

    frameCounter++;
    uiStateFrameCounter++;
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
