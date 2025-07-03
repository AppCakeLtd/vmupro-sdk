
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

// float in air for collision testing
const bool NO_GRAV = false;
const bool DEBUG_SPRITEBOX = false;
const bool DEBUG_HITPOINTS = true;

// shift fixed point maths to/from world/subpixel coords
#define SHIFT 4

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TILE_SIZE_PX 16
// e.g. 1 tile is 256 subpixel units
#define TILE_SIZE_SUB (TILE_SIZE_PX << SHIFT)

// the spritesheet/atlas
#define TILEMAP_WIDTH_TILES 8
#define TILEMAP_HEIGHT_TILES 22
#define TILEMAP_WIDTH_PIXELS (TILEMAP_WIDTH_TILES * TILE_SIZE_PX)
#define TILEMAP_HEIGHT_PIXELS (TILEMAP_HEIGHT_TILES * TILE_SIZE_PX)

// the actual map
#define MAP_WIDTH_TILES 128
#define MAP_HEIGHT_TILES 32
#define MAP_WIDTH_PIXELS (MAP_WIDTH_TILES * TILE_SIZE_PX)
#define MAP_HEIGHT_PIXELS (MAP_HEIGHT_TILES * TILE_SIZE_PX)

#define BLOCK_NULL 0xFFFFFFFF

// maximum speed (in subpixels) while walking or running
// per frame.
const int MAX_SUBSPEED_WALK = 100;
const int MAX_SUBSPEED_RUN = 200;

const int SUBACCEL_WALK = 12;
const int SUBACCEL_RUN = 9;
const int SUBACCEL_AIR = 6;

const int SUBDAMPING_WALK = 8;
const int SUBDAMPING_RUN = 6;
const int SUBDAMPING_AIR = 4;

// max frames for which the up force is applied
const int MAX_JUMP_BOOST_FRAMES = 16;
const int SUB_JUMPFORCE = 12 * 2;
const int SUB_GRAVITY = 2;
const int MAX_SUBFALLSPEED = 40;

int camX = 0;
int camY = 0;
int frameCounter = 0;

typedef struct
{
  uint32_t widthInTiles;
  uint32_t heightInTiles;
  uint32_t blockData[];
} LevelData;

LevelData *currentLevel = NULL;

typedef struct
{
  int x;
  int y;
} Vec2;

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

typedef struct
{
  // typically the image's bbox
  // in world coords
  BBox worldBBox;
  // the actual hitbox in subpixel space
  BBox subHitBox;

  // the actual image data
  const Img *img;
  // Player's pos will be the feet pos for example
  // the sprite will be offset from this by -(width/2)
  // these values are subpixel / fixed point
  Vec2 subPos;
  // calculated at the end of the frame
  // since we may do multiple tweaks per frame
  Vec2 lastSubPos;

  Vec2 subVelo;
  Vec2 lastSubVelo;

  bool isGrounded;
  bool onGroundLastFrame;
  int jumpFrameNum;

  // config options
  bool isPlayer;
  Anchor_H anchorH;
  Anchor_V anchorV;

  bool facingRight;
  bool wasRunningLastTimeWasOnGround;

  Inputs input;
  MoveMode moveMode;
} Sprite;

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

typedef struct
{
  Sprite spr;
} Player;

typedef struct
{
  Sprite pr;
} Mob;

Player player;
Mob testMob;

//__TEST__
//
// Protos
//
bool CheckGrounded(Sprite *spr);

// in screen spsace
void DrawSpriteBoundingBox(Sprite *inSprite, uint16_t inCol)
{

  BBox *box = &inSprite->worldBBox;
  Vec2 worldPos = inSprite->worldBBox.vecs.pos;
  Vec2 screenPos = World2Screen(&worldPos);
  int x2 = screenPos.x + box->width;
  int y2 = screenPos.y + box->height;
  vmupro_draw_rect(screenPos.x, screenPos.y, x2, y2, inCol);
}

// Updates the bounding box when the pos or img changes
void OnSpriteMoved(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "OnSpriteMoved() - null sprite");
    return;
  }

  const Img *img = spr->img;

  if (spr->img == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Set the sprite img before attempting to update the bboxs");
    return;
  }

  // store the sprite box pos in world space
  // and the hitbox pos in subpixel space

  bool forPlayer = spr->isPlayer;
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
  Vec2 wPos = GetWorldPos(&player.spr);
  return wPos;
}

void SetSubPos(Sprite *spr, Vec2 *newPos)
{
  spr->subPos = *newPos;
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

  spr->moveMode = inMode;

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Move mode %d", frameCounter, (int)inMode);
}

void ResetSprite(Sprite *spr)
{

  memset(spr, 0, sizeof(Sprite));

  spr->img = &img_player_idle_raw;

  // update other struct vals
  spr->facingRight = true;
  spr->wasRunningLastTimeWasOnGround = false;

  spr->subPos = ZeroVec();
  spr->lastSubPos = ZeroVec();

  spr->subVelo = ZeroVec();
  spr->lastSubVelo = ZeroVec();

  SetMoveMode(spr, MM_FALL);

  spr->isGrounded = false;
  spr->onGroundLastFrame = false;
  spr->jumpFrameNum = 0;

  // temporary config stuff
  Vec2 worldStartPos = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4)};
  SetWorldPos(spr, &worldStartPos);
  spr->isPlayer = true;
  spr->anchorH = ANCHOR_HMID;
  spr->anchorV = ANCHOR_VBOTTOM;

  // update the hitbox, bounding box, etc
  OnSpriteMoved(spr);
}

void LoadLevel(int levelNum)
{

  currentLevel = (LevelData *)level_1_layer_0_data;
  ResetSprite(&player.spr);
}

// returns atlas block 0-max
// note: the .map file uses 0x00 for blank spots
// so we'll always sub 1 to get a 0-indexed
// value into our spritesheet/atlas
uint32_t GetBlockIDAtColRow(int blockCol, int blockRow)
{

  if (currentLevel == NULL)
    return BLOCK_NULL;

  // bounds check the input values
  if (blockCol < 0 || blockRow < 0)
  {
    return BLOCK_NULL;
  }

  if (blockCol >= currentLevel->widthInTiles || blockRow >= currentLevel->heightInTiles)
  {
    return BLOCK_NULL;
  }

  uint32_t offset = (blockRow * currentLevel->widthInTiles) + blockCol;

  uint32_t block = currentLevel->blockData[offset];
  return block - 1;
}

void UpdatePlayerInputs()
{

  vmupro_btn_read();

  Sprite *spr = &player.spr;
  Inputs *inp = &spr->input;

  inp->up = vmupro_btn_held(DPad_Up);
  inp->down = vmupro_btn_held(DPad_Down);
  inp->left = vmupro_btn_held(DPad_Left);
  inp->right = vmupro_btn_held(DPad_Right);
  inp->jump = vmupro_btn_held(Btn_B);
  inp->run = vmupro_btn_held(Btn_A);
}

void DrawLevelBlock(int x, int y)
{

  if (currentLevel == NULL)
    return;

  uint32_t blockId = GetBlockIDAtColRow(x, y);

  if (blockId == BLOCK_NULL)
  {
    return;
  }

  uint32_t pixSrcX = (blockId % TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;
  uint32_t pixSrcY = (blockId / TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;

  uint32_t pixTargX = x * TILE_SIZE_PX;
  uint32_t pixTargY = y * TILE_SIZE_PX;

  const Img *sheet = &img_tilemap_raw;
  vmupro_blit_tile(sheet->data, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width);
}

// center the camera on the player
void SolveCamera()
{

  Sprite *spr = &player.spr;
  Vec2 playerWorldPos = GetWorldPos(spr);

  // check if cam's going off left of the level
  int camLeft = playerWorldPos.x - (SCREEN_WIDTH / 2);
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
  int playerYOffset = (SCREEN_WIDTH * 4) / 3;
  int camTop = playerWorldPos.y - (SCREEN_WIDTH / 2);
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

  Img *img = &img_bg_1_raw;

  int bgScrollX = (camX * 5) / 4;
  int bgScrollY = (camY * 5) / 4;

  vmupro_blit_scrolling_background(img->data, img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DrawGroundtiles()
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
      DrawLevelBlock(realXTile, realYTile);
    }
  }
}

// store the last abs pos vs current
// do it at the end of the frame in case
// the pos is updated multiple times in a frame
// (collision, etc)
void EndOfFrame(Sprite *inSpr)
{
  inSpr->lastSubPos = inSpr->subPos;
  inSpr->lastSubVelo = inSpr->subVelo;
  inSpr->onGroundLastFrame = inSpr->isGrounded;
  // inSpr->lastSubAccel = inSpr->subAccel;
}

int GetXSubAccelForMode(MoveMode inMode, bool wasRunningWhenLastGrounded)
{

  switch (inMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled movement mode: %d", (int)inMode);
    return SUBACCEL_WALK;
    break;

  case MM_JUMP:
  case MM_FALL:
    return SUBACCEL_AIR;
    break;

  case MM_WALK:
    return SUBACCEL_WALK;
    break;
  case MM_RUN:
    return SUBACCEL_RUN;
    break;
  }
}

int GetMaxXSubSpeedForMode(MoveMode inMode, bool wasRunningWhenLastGrounded)
{

  switch (inMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled movement mode: %d", (int)inMode);
    return MAX_SUBSPEED_WALK;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      return MAX_SUBSPEED_RUN;
    }
    else
    {
      return MAX_SUBSPEED_WALK;
    }

    break;

  case MM_WALK:
    return MAX_SUBSPEED_WALK;
    break;

  case MM_RUN:
    return MAX_SUBSPEED_RUN;
    break;
  }
}

int GetXDampingForMode(MoveMode inMode, bool wasRunningWhenLastGrounded)
{

  switch (inMode)
  {
  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled movement mode: %d", (int)inMode);
    return MAX_SUBSPEED_WALK;
    break;

  case MM_JUMP:
  case MM_FALL:
    if (wasRunningWhenLastGrounded)
    {
      return SUBDAMPING_RUN;
    }
    else
    {
      return SUBDAMPING_WALK;
    }

    break;

  case MM_WALK:
    return SUBDAMPING_WALK;
    break;

  case MM_RUN:
    return SUBDAMPING_RUN;
    break;
  }
}

// hitbox = false: return sprite box in world coords
// hitbox = true: return hitbox in subpixel coords
Vec2 GetPointOnSprite(Sprite *spr, bool hitBox, Anchor_H anchorH, Anchor_V anchorV)
{

  // TODO: switch to an actual hitbox

  int returnX = 0;
  int returnY = 0;

  BBox *aabb = hitBox ? &spr->subHitBox : &spr->worldBBox;

  // a note on hitbox widths...
  // say we have a 16px wide player at x = 0;
  // there's a column of blocks at index 1, or x = 16
  // if we use playerx + width (or 0 + 16) we get 16
  // which is *inside* the column, despite us not touching it
  // i.e. the character occupies pixels 0-15, not 0-16

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
    if (hitBox)
    {
      returnX -= (1 << SHIFT);
    }
    else
    {
      returnX -= 1;
    }
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
    if (hitBox)
    {
      returnY -= (1 << SHIFT);
    }
    else
    {
      returnY -= 1;
    }
  }
  else
  {
    vmupro_log(VMUPRO_LOG_ERROR, "TAG", "Unknown V alignment");
  }

  Vec2 returnVal = {returnX, returnY};
  return returnVal;
}

typedef enum
{
  DIR_UP,
  DIR_RIGHT,
  DIR_DOWN,
  DIR_LEFT,
} Direction;

typedef struct
{

  // Used to work out which pivot points
  // to use
  Direction dir;

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

  // top left to bottom right
  // e.g. top left, top middle, top right
  // or top left, middle left, bottom left
  // -1 for nothing
  int blockID[3];

  // the ejection edge for the block we hit
  // e.g. if we're moving right, it'll be
  // - the x coord of the block's left edge
  // - the y coord of the hitbox point we're checking
  Vec2 subEjectionPoint[3];

  bool hitSomething;
  int lastHitIndex;

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

// subOffsetOrNull adds an offset to where we check for collisions
// e.g. when moving right we check currentPos.x + velo.x
// for where we'll be, rather than where we are
// Used for for collision and for ground checks
HitInfo NewHitInfo(Sprite *spr, Direction dir, Vec2 *subOffsetOrNull, const char *src)
{

  HitInfo rVal;
  memset(&rVal, 0, sizeof(HitInfo));

  rVal.dir = dir;
  rVal.lastHitIndex = -1;

  // get a list of points to check for
  // whatever direction we're moving
  switch (dir)
  {
  case DIR_UP:
    // top row
    rVal.anchorH[0] = ANCHOR_HLEFT;
    rVal.anchorH[1] = ANCHOR_HMID;
    rVal.anchorH[2] = ANCHOR_HRIGHT;
    rVal.anchorV[0] = ANCHOR_VTOP;
    rVal.anchorV[1] = ANCHOR_VTOP;
    rVal.anchorV[2] = ANCHOR_VTOP;
    break;

  case DIR_RIGHT:
    rVal.anchorH[0] = ANCHOR_HRIGHT;
    rVal.anchorH[1] = ANCHOR_HRIGHT;
    rVal.anchorH[2] = ANCHOR_HRIGHT;
    rVal.anchorV[0] = ANCHOR_VTOP;
    rVal.anchorV[1] = ANCHOR_VMID;
    rVal.anchorV[2] = ANCHOR_VBOTTOM;
    break;

  case DIR_DOWN:
    rVal.anchorH[0] = ANCHOR_HLEFT;
    rVal.anchorH[1] = ANCHOR_HMID;
    rVal.anchorH[2] = ANCHOR_HRIGHT;
    rVal.anchorV[0] = ANCHOR_VBOTTOM;
    rVal.anchorV[1] = ANCHOR_VBOTTOM;
    rVal.anchorV[2] = ANCHOR_VBOTTOM;
    break;

  case DIR_LEFT:
    rVal.anchorH[0] = ANCHOR_HLEFT;
    rVal.anchorH[1] = ANCHOR_HLEFT;
    rVal.anchorH[2] = ANCHOR_HLEFT;
    rVal.anchorV[0] = ANCHOR_VTOP;
    rVal.anchorV[1] = ANCHOR_VMID;
    rVal.anchorV[2] = ANCHOR_VBOTTOM;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unknown direction");
    break;
  }

  // convert the pivot points to actual world points
  for (int i = 0; i < 3; i++)
  {
    rVal.subCheckPos[i] = GetPointOnSprite(spr, true, rVal.anchorH[i], rVal.anchorV[i]);

    printf("__TEST__ Frame %d %s ypos on sprite %d\n", frameCounter, src, rVal.subCheckPos[i].y);

    if (subOffsetOrNull != NULL)
    {
      AddVec(&rVal.subCheckPos[i], subOffsetOrNull);
    }

    printf("__TEST__ Frame %d %s ypos with offset %d\n", frameCounter, src, rVal.subCheckPos[i].y);

    if (DEBUG_HITPOINTS)
    {
      // let's debug to screen
      Vec2 screenPos = Sub2Screen(&rVal.subCheckPos[i]);
      uint16_t col = VMUPRO_COLOR_WHITE;
      vmupro_draw_rect(screenPos.x - 2, screenPos.y - 2, screenPos.x + 4, screenPos.y + 4, col);
    }
  }

  // and clear the collision return vals

  rVal.hitSomething = false;

  // finally run some tile collision checks:
  for (int i = 0; i < 3; i++)
  {

    // just the literal tile indexes into the x/y grid
    Vec2 tileRowAndCol = GetTileRowAndColFromSubPos(&rVal.subCheckPos[i]);

    int blockId = GetBlockIDAtColRow(tileRowAndCol.x, tileRowAndCol.y);
    if (blockId != BLOCK_NULL)
    {

      rVal.hitSomething = true;

      rVal.blockID[i] = blockId;
      rVal.lastHitIndex = i;

      // essentially we're rounding down and then back up
      Vec2 tileSubPos = GetTileSubPosFromRowAndCol(&tileRowAndCol);

      // default to the hit point being wherever we cheked on the sprite
      // we'll tweak the x/y in a sec, depending on where we hit the block
      rVal.subEjectionPoint[i].x = rVal.subCheckPos[i].x;
      rVal.subEjectionPoint[i].y = rVal.subCheckPos[i].y;

      if (dir == DIR_RIGHT)
      {
        // collided on the sprite's right
        // so the hit point is the block's X pos
        rVal.subEjectionPoint[i].x = tileSubPos.x;
      }
      else if (dir == DIR_LEFT)
      {
        // collided on the sprite's left
        // so the hit point is the block's x+width
        rVal.subEjectionPoint[i].x = tileSubPos.x + TILE_SIZE_SUB;
      }
      else if (dir == DIR_DOWN)
      {
        // collided on the sprite's bottom
        // so the hitpoint is the block's top
        rVal.subEjectionPoint[i].y = tileSubPos.y;
      }
      else if (dir == DIR_UP)
      {
        // collided on the sprite's top
        // so the hitpoint is teh block's bottom
        rVal.subEjectionPoint[i].y = tileSubPos.y + TILE_SIZE_SUB;
      }
    }
    else
    {
      rVal.blockID[i] = BLOCK_NULL;
    }
  }

  return rVal;
}

void PrintHitInfo(HitInfo *info)
{

  printf("HitInfo dir %d hit = %d\n", info->dir, (int)info->hitSomething);
  printf("   ids = %d, %d, %d\n", info->blockID[0], info->blockID[1], info->blockID[2]);
  printf("   chX = %d, %d, %d\n", info->subCheckPos[0].x, info->subCheckPos[1].x, info->subCheckPos[2].x);
  printf("   chY = %d, %d, %d\n", info->subCheckPos[0].y, info->subCheckPos[1].y, info->subCheckPos[2].y);
  printf("   ejX = %d, %d, %d\n", info->subEjectionPoint[0].x, info->subEjectionPoint[1].x, info->subEjectionPoint[2].x);
  printf("   ejY = %d, %d, %d\n", info->subEjectionPoint[0].y, info->subEjectionPoint[1].y, info->subEjectionPoint[2].y);
}

// Perform the ejection part after collecting hit info
void EjectHitInfo(Sprite *spr, HitInfo *info, bool horz)
{

  // the direction we hit something at
  Direction dir = info->dir;

  // simple early exit
  if (!info->hitSomething)
  {
    if (horz)
    {
      AddSubPos2(spr, spr->subVelo.x, 0);
    }
    else
    {
      AddSubPos2(spr, 0, spr->subVelo.y);
    }

    return;
  }

  int idx = info->lastHitIndex;

  if (dir == DIR_RIGHT)
  {

    printf("__TEST__ eject right\n");

    // we hit something while moving right
    spr->subVelo.x = 0;

    // e.g. the point on the block we just hit
    int subX = info->subEjectionPoint[idx].x;

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
    SetSubPos(spr, &sub);
  }

  if (dir == DIR_LEFT)
  {

    printf("__TEST__ eject left\n");

    spr->subVelo.x = 0;

    int subX = info->subEjectionPoint[idx].x;

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
    SetSubPos(spr, &sub);
  }

  if (dir == DIR_UP)
  {

    printf("__TEST__ eject up\n");

    spr->subVelo.y = 0;

    int subY = info->subEjectionPoint[idx].y;

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
    SetSubPos(spr, &sub);
  }

  if (dir == DIR_DOWN)
  {

    printf("__TEST__ Frame %d eject down from %d\n", frameCounter, spr->subPos.y);

    spr->subVelo.y = 0;

    int subY = info->subEjectionPoint[idx].y;

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
    SetSubPos(spr, &sub);
    printf("__TEST__ Frame %d ejected to %d\n", frameCounter, sub.y);

    bool groundedNow = CheckGrounded(spr);
    printf("__TEST__ post ejection ground check = %d\n", (int)groundedNow);
  }
}

// Attempts to apply velo to pos, taking collisions into account
// returns the sign of the movement direction
// e.g. -1 for jump, 1 for ground
// e.g. -1 for left, 1 for right
int TryMove(Sprite *spr, bool horz)
{

  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;
  bool movingDown = spr->subVelo.y > 0;
  bool movingUp = spr->subVelo.y < 0;

  // when we do the sprite collision detection
  // we might want to check where we'll *be* rather
  // than where we *are*
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
    //__TEST__
    // subCheckOffset.x = spr->subVelo.x;
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
    //__TEST__
    // subCheckOffset.y = spr->subVelo.y;
  }

  HitInfo info = NewHitInfo(spr, dir, &subCheckOffset, "TryMove");

  if (info.hitSomething)
  {
    // PrintHitInfo(&info);
  }

  EjectHitInfo(spr, &info, horz);

  if (info.hitSomething)
  {
    return (dir == DIR_RIGHT || dir == DIR_DOWN) ? 1 : -1;
  }
  return 0;
}

bool CheckGrounded(Sprite *spr)
{

  // Check 1 subpix below the player
  Vec2 subGroundCheckOffset = {0, 0};
  HitInfo nhi = NewHitInfo(spr, DIR_DOWN, &subGroundCheckOffset, "groundcheck");

  return nhi.hitSomething;
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
  if (spr->jumpFrameNum < MAX_JUMP_BOOST_FRAMES)
  {
    printf("__TEST__ stop jump boost %s\n", src);
    spr->jumpFrameNum = MAX_JUMP_BOOST_FRAMES;
  }
}

void TryContinueJump(Sprite *spr)
{

  if (!SpriteJumping(spr))
  {
    return;
  }
  if (spr->jumpFrameNum >= MAX_JUMP_BOOST_FRAMES)
  {
    return;
  }

  // we're jumping, and we've not run out of boost frames

  if (spr->input.jump)
  {
    // add jump velo
    spr->subVelo.y -= SUB_JUMPFORCE;
    printf("__TEST__ Frame %d Applying jump on frame %d\n", frameCounter, spr->jumpFrameNum);
    spr->jumpFrameNum++;
  }
  else
  {
    // user has released jump, prevent further re-presses
    // until we land
    StopJumpBoost(spr, "fc");
  }
}

//__TEST__
// e.g. walking off an edge
void CheckFallen(Sprite *spr)
{

  // we were walking/running
  // we were on the ground
  // we are no longer on the ground

  if (!MoveModeOnGround(spr))
  {
    return;
  }
  if (!spr->isGrounded)
  {
    return;
  }
  if (!spr->onGroundLastFrame)
  {
    return;
  }

  // we were on the ground, but are no longer
  // we're not jumping...
  // so we're falling
  printf("__TEST__ walked off a ledge\n");
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

  printf("__TEST__ Frame %d landed @ %d/%d\n", frameCounter, spr->subPos.y, spr->subPos.y >> SHIFT);
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
// - check ground state
// - check landing, etc

void SolvePlayer()
{

  Sprite *spr = &player.spr;
  Inputs *inp = &spr->input;

  // set up all of our state values
  // as the result of the previous frame
  // i.e. we don't want to check the grounded state now
  // then perform a jump + land in the same frame.

  MoveMode mm = spr->moveMode;
  bool inAir = MoveModeInAir(spr);
  bool isJumping = SpriteJumping(spr);

  int maxSubSpeedX = GetMaxXSubSpeedForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int maxSubSpeedY = MAX_SUBFALLSPEED;

  int subAccelX = GetXSubAccelForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int subAccelY = spr->isGrounded ? 0 : SUB_GRAVITY;

  int subDampX = GetXDampingForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int subDampY = 0;

  TryJump(spr);
  TryContinueJump(spr);

  bool playerXInput = inp->right || inp->left;
  bool playerYInput = inp->up || inp->down;
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
  if (NO_GRAV)
  {
    subDampY = 1;

    if (!playerYInput)
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

  if (!playerXInput)
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

  // Apply X velo to X movement
  AddVecInts(&spr->subPos, spr->subVelo.x, 0);

  // Eject from any X Collisions
  TryMove(spr, true);

  //
  // Y/Horizontal damp, clamp, move, eject
  //

  // Damp Y Velo

  if (NO_GRAV)
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

  // Add Velo to movement
  AddVecInts(&spr->subPos, 0, spr->subVelo.y);

  Vec2 subDamp = {subDampX, subDampY};
  AddVec(&spr->subVelo, &subDamp);

  // Sanity check
  if (Abs(spr->subVelo.y) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's y velo exceeds a full tile size!");
  }

  // Eject from any Y collisions
  TryMove(spr, true);
  int vBonk = TryMove(spr, false);

  spr->isGrounded = CheckGrounded(spr);

  CheckLanded(spr);

  // head or feet bonked
  // prevent jump boost
  if (spr->isGrounded && vBonk != 0)
  {
    StopJumpBoost(spr, "bonk");
  }
  
}

void DrawPlayer()
{

  Sprite *spr = &player.spr;

  Vec2 worldOrigin = GetWorldPos(spr);
  Vec2 screenOrigin = World2Screen(&worldOrigin);

  // draw based on the actual bounding box
  Vec2 worldBoxPos = spr->worldBBox.vecs.pos;
  Vec2 screenBoxPos = World2Screen(&worldBoxPos);

  const Img *img = player.spr.img;

  bool goingUp = player.spr.subVelo.y < 0;

  // __TEST__ temporary thing
  if (goingUp)
  {
    spr->img = &img_player_fall_raw;
    OnSpriteMoved(&player.spr);
  }
  else
  {
    spr->img = &img_player_idle_raw;
    OnSpriteMoved(&player.spr);
  }

  // update the img pointer
  img = player.spr.img;

  vmupro_drawflags_t flags = (spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);
  uint16_t mask = *(uint16_t *)&img->data[0];
  vmupro_blit_buffer_transparent(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, mask, flags);
}

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "8BM Platformer Example");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  LoadLevel(0);

  while (true)
  {

    vmupro_color_t col = VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    SolveCamera();

    DrawBackground();
    DrawGroundtiles();

    UpdatePlayerInputs();
    SolvePlayer();

    DrawPlayer();

    if (DEBUG_SPRITEBOX)
    {
      DrawSpriteBoundingBox(&player.spr, VMUPRO_COLOR_WHITE);
    }

    vmupro_push_double_buffer_frame();

    EndOfFrame(&player.spr);

    vmupro_sleep_ms(10);

    if (vmupro_btn_held(Btn_Mode))
    {
      break;
    }

    // test: cycle through sprite offsets
    if (vmupro_btn_pressed(Btn_A))
    {
      printf("PlayerX world:%d sub: %d \n", GetWorldPos(&player.spr).x, GetSubPos(&player.spr).x);
      printf("PlayerY world:%d sub: %d \n", GetWorldPos(&player.spr).y, GetSubPos(&player.spr).y);
      printf("Player grounded? %d\n", (int)player.spr.isGrounded);
    }

    if (vmupro_btn_pressed(Btn_B))
    {
      // player.spr.anchorV = (player.spr.anchorV + 1) % (3);
      //  player.spr.anchorH = (player.spr.anchorH + 1) % (3);
      // OnSpriteMoved(&player.spr);
    }

    frameCounter++;
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
