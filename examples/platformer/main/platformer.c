
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

const bool DEBUG_BBOX = false;

// shift fixed point maths to/from world/subpixel coords
#define SHIFT 4

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TILE_SIZE_PX 16
// e.g. 1 tile is 256 subpixel units
#define TILE_SIZE_SUB (16 << SHIFT)

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

typedef struct
{
  // typically the image's bbox
  BBox worldBBox;
  // we're just using the sprite bbox for now
  BBox worldHitBox;

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

  // Vec2 subAccel;
  // Vec2 lastSubAccel;

  bool isPlayer;
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
void OnSpriteUpdated(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "OnSpriteUpdated() - null sprite");
    return;
  }

  const Img *img = spr->img;

  if (spr->img == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Set the sprite img before attempting to update the bboxs");
    return;
  }

  bool forPlayer = spr->isPlayer;
  Vec2 worldOrigin = GetWorldPos(spr);
  if (forPlayer)
  {
    // if it's the player, we'll centre around the feet
    spr->worldBBox.x = worldOrigin.x - (img->width / 2);
    spr->worldBBox.y = worldOrigin.y - (img->height);
    spr->worldBBox.width = img->width;
    spr->worldBBox.height = img->height;
  }
  else
  {
    // else we'll centre around the centre.
    spr->worldBBox.x = worldOrigin.x - (img->width / 2);
    spr->worldBBox.y = worldOrigin.y - (img->height / 2);
    spr->worldBBox.width = img->width;
    spr->worldBBox.height = img->height;
  }

  spr->worldHitBox = spr->worldBBox;
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
  OnSpriteUpdated(spr);
}

void AddSubPos(Sprite *spr, Vec2 *delta)
{
  AddVec(&spr->subPos, delta);
  OnSpriteUpdated(spr);
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

bool SpriteInAir(Sprite *spr)
{

  MoveMode mm = spr->moveMode;
  if (mm == MM_FALL || mm == MM_JUMP)
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

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Move mode %d", (int)inMode);
}

void ResetSprite(Sprite *spr)
{

  spr->img = &img_player_idle_raw;

  // update other struct vals
  spr->facingRight = true;
  spr->wasRunningLastTimeWasOnGround = false;

  spr->subPos = ZeroVec();
  spr->lastSubPos = ZeroVec();

  spr->subVelo = ZeroVec();
  spr->lastSubVelo = ZeroVec();

  // spr->subAccel = ZeroVec();
  // spr->lastSubAccel = ZeroVec();

  SetMoveMode(spr, MM_FALL);
  memset(&spr->input, 0, sizeof(Inputs));

  // temp sensible start pos
  Vec2 worldStartPos = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4)};
  SetWorldPos(spr, &worldStartPos);
}

void LoadLevel(int levelNum)
{

  currentLevel = (LevelData *)level_1_layer_0_data;
  ResetSprite(&player.spr);
  // filthy hack
  player.spr.isPlayer = true;
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

typedef enum
{
  PIV_LEFT_TOP,
  PIV_MIDDLE_TOP,
  PIV_RIGHT_TOP,
  PIV_LEFT_MIDDLE,
  PIV_MIDDLE_MIDDLE,
  PIV_RIGHT_MIDDLE,
  PIV_LEFT_BOTTOM,
  PIV_MIDDLE_BOTTOM,
  PIV_RIGHT_BOTTOM,
  PIV_NONE_NONE
} Pivot;

Vec2 GetWorldPointOnSprite(Sprite *spr, Pivot piv)
{

  // TODO: switch to an actual hitbox

  int returnX = 0;
  int returnY = 0;

  BBox *aabb = &spr->worldHitBox;

  switch (piv)
  {

  // top row
  case PIV_LEFT_TOP:
    returnX = aabb->x;
    returnY = aabb->y;
    break;

  case PIV_MIDDLE_TOP:
    returnX = aabb->x + (aabb->width / 2);
    returnY = aabb->y;
    break;

  case PIV_RIGHT_TOP:
    returnX = aabb->x + aabb->width;
    returnY = aabb->y;
    break;

  // middle row
  case PIV_LEFT_MIDDLE:
    returnX = aabb->x;
    returnY = aabb->y + (aabb->height / 2);
    break;

  case PIV_MIDDLE_MIDDLE:
    returnX = aabb->x + (aabb->width / 2);
    returnY = aabb->y + (aabb->height / 2);
    break;

  case PIV_RIGHT_MIDDLE:
    returnX = aabb->x + aabb->width;
    returnY = aabb->y + (aabb->height / 2);
    break;

  // bottom row
  case PIV_LEFT_BOTTOM:
    returnX = aabb->x;
    returnY = aabb->y + aabb->height;
    break;

  case PIV_MIDDLE_BOTTOM:
    returnX = aabb->x + (aabb->width / 2);
    returnY = aabb->y + aabb->height;
    break;

  case PIV_RIGHT_BOTTOM:
    returnX = aabb->x + aabb->width;
    returnY = aabb->y + aabb->height;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unhandled pivot mode");
    break;
  }

  Vec2 returnVal = {returnX, returnY};
  return returnVal;
}

void ApplyGravity(Sprite *inSpr, Vec2 *accel)
{

  // TEMP:
  if (inSpr->input.down)
  {
    accel->y = SUB_GRAVITY;
  }
  if (inSpr->input.up)
  {
    accel->y = -SUB_GRAVITY;
  }
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
  Pivot piv[3];

  // e.g. for downward would be
  // bottom left, bottom middle, bottom right
  Vec2 worldPos[3];

  // top left to bottom right
  // e.g. top left, top middle, top right
  // or top left, middle left, bottom left
  // -1 for nothing
  int blockID[3];

  // the ejection edge for the block we hit
  // e.g. if we're moving right, it'll be
  // - the x coord of the block's left edge
  // - the y coord of the hitbox point we're checking
  Vec2 worldEjectionPoint[3];

  // how far we've pushed into the wall
  // note: rough va
  int worldSnapDistance;

  bool hitSomething;
  int lastHitIndex;

} HitInfo;

HitInfo NewHitInfo(Sprite *spr, Direction dir, Vec2 *offsetOrNull)
{

  HitInfo rVal;

  rVal.dir = dir;
  rVal.worldSnapDistance = 0;
  rVal.lastHitIndex = -1;

  // get a list of points to check for
  // whatever direction we're moving
  switch (dir)
  {
  case DIR_UP:
    // top row
    rVal.piv[0] = PIV_LEFT_TOP;
    rVal.piv[1] = PIV_MIDDLE_TOP;
    rVal.piv[2] = PIV_RIGHT_TOP;
    break;

  case DIR_RIGHT:
    rVal.piv[0] = PIV_RIGHT_TOP;
    rVal.piv[1] = PIV_RIGHT_MIDDLE;
    rVal.piv[2] = PIV_RIGHT_BOTTOM;
    break;

  case DIR_DOWN:
    rVal.piv[0] = PIV_LEFT_BOTTOM;
    rVal.piv[1] = PIV_MIDDLE_BOTTOM;
    rVal.piv[2] = PIV_RIGHT_BOTTOM;
    break;

  case DIR_LEFT:
    rVal.piv[0] = PIV_LEFT_TOP;
    rVal.piv[1] = PIV_LEFT_MIDDLE;
    rVal.piv[2] = PIV_LEFT_BOTTOM;
    break;

  default:
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Unknown direction");
    break;
  }

  // convert the pivot points to actual world points
  for (int i = 0; i < 3; i++)
  {
    rVal.worldPos[i] = GetWorldPointOnSprite(spr, rVal.piv[i]);

    if (offsetOrNull != NULL)
    {
      AddVec(&rVal.worldPos[i], offsetOrNull);
    }

    // let's debug to screen
    Vec2 screenPos = World2Screen(&rVal.worldPos[i]);
    vmupro_draw_rect(screenPos.x - 2, screenPos.y - 2, screenPos.x + 4, screenPos.y + 4, VMUPRO_COLOR_WHITE);
  }

  // and clear the collision return vals

  rVal.hitSomething = false;

  // finally run some tile collision checks:
  for (int i = 0; i < 3; i++)
  {
    int tileCol = rVal.worldPos[i].x / TILE_SIZE_PX;
    int tileRow = rVal.worldPos[i].y / TILE_SIZE_PX;
    int blockId = GetBlockIDAtColRow(tileCol, tileRow);
    if (blockId != BLOCK_NULL)
    {
      rVal.hitSomething = true;
      rVal.blockID[i] = blockId;
      rVal.lastHitIndex = i;

      // calculate the world ejection point based on the dir
      if ( dir == DIR_RIGHT ){
        // left side of the block we hit
        rVal.worldEjectionPoint[i].x = tileCol * TILE_SIZE_PX;
        rVal.worldEjectionPoint[i].y = rVal.worldPos[i].y;
        // what will it take to snap out
        rVal.worldSnapDistance = rVal.worldEjectionPoint[i].x - rVal.worldPos[i].x;
      } else if ( dir == DIR_LEFT ){
        // we hit the right side of the block
        rVal.worldEjectionPoint[i].x = (tileCol * TILE_SIZE_PX) + TILE_SIZE_PX;
        rVal.worldEjectionPoint[i].y = rVal.worldPos[i].y;
        // what will it take to snap out
        rVal.worldSnapDistance = rVal.worldEjectionPoint[i].x - rVal.worldPos[i].x;
      } else if ( dir == DIR_DOWN ){
        // we hit the top of the block
        rVal.worldEjectionPoint[i].x = rVal.worldPos[i].x;
        rVal.worldEjectionPoint[i].y = (tileRow * TILE_SIZE_PX);
        rVal.worldSnapDistance = rVal.worldEjectionPoint[i].y - rVal.worldPos[i].y;
      } else if (dir == DIR_UP ){
        // we hit the bottom of the block
        rVal.worldEjectionPoint[i].x = rVal.worldPos[i].x;
        rVal.worldEjectionPoint[i].y = (tileRow * TILE_SIZE_PX) + TILE_SIZE_PX;
        rVal.worldSnapDistance = rVal.worldEjectionPoint[i].y - rVal.worldPos[i].y;
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

  printf("HitInfo dir %d hit = %d,  ids = %d, %d, %d\n", (int)info->dir, info->hitSomething, info->blockID[0], info->blockID[1], info->blockID[2]);
}

// Attempts to apply velo to pos, taking collisions into account
void TryMove(Sprite *spr, bool horz)
{

  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;
  bool movingDown = spr->subVelo.y > 0;
  bool movingUp = spr->subVelo.y < 0;

  Vec2 worldCheckOffset = {0, 0};

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
      return;
    }
    worldCheckOffset.x = spr->subVelo.x >> SHIFT;
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
      return;
    }
    worldCheckOffset.y = spr->subVelo.y >> SHIFT;
  }

  HitInfo info = NewHitInfo(spr, dir, &worldCheckOffset);

  if ( info.hitSomething ){
    //PrintHitInfo(&info);
  }

  // just apply the movement for now for debugging
  // __TEST__
  if (horz)
  {
    
    if ( info.hitSomething ){
      int idx = info.lastHitIndex;
      spr->subVelo.x = 0;
      
      // __TEST__ we'll split this into another function in a sec
      int worldX = info.worldEjectionPoint[idx].x -spr->worldHitBox.width/2;
      int subX = worldX << SHIFT;
      int subY = spr->subPos.y;
      Vec2 sub = {subX, subY};
      SetSubPos(spr, &sub);
      

    } else {
      AddSubPos2(spr, spr->subVelo.x, 0);
    }
  }
  else
  {
    AddSubPos2(spr, 0, spr->subVelo.y);
  }
}

void SolvePlayer()
{

  Sprite *spr = &player.spr;
  Inputs *inp = &spr->input;

  MoveMode mm = spr->moveMode;
  bool inAir = SpriteInAir(spr);
  bool isJumping = SpriteJumping(spr);

  int maxSubSpeedX = GetMaxXSubSpeedForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int subAccelX = GetXSubAccelForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int subAccelY = 0;
  int subDampX = GetXDampingForMode(mm, spr->wasRunningLastTimeWasOnGround);
  int subDampY = 0;

  bool playerInput = inp->right || inp->left;
  bool movingRight = spr->subVelo.x > 0;
  bool movingLeft = spr->subVelo.x < 0;

  // increase accel while turning
  bool turning = (inp->left && movingRight) || (inp->right && movingLeft);
  if (turning)
    subAccelX = (subAccelX * 4) / 3;

  if (!playerInput)
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

  Vec2 subAccel = {subAccelX, subAccelY};
  ApplyGravity(spr, &subAccel);

  // apply acceleration to the velo
  AddVec(&spr->subVelo, &subAccel);

  //
  // Dampen
  //
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

  Vec2 subDamp = {subDampX, subDampY};
  AddVec(&spr->subVelo, &subDamp);

  //
  // Clamp
  //

  if (spr->subVelo.x > maxSubSpeedX)
  {
    spr->subVelo.x = maxSubSpeedX;
  }
  else if (spr->subVelo.x < -maxSubSpeedX)
  {
    spr->subVelo.x = -maxSubSpeedX;
  }

  // some sanity checks?
  if (Abs(spr->subVelo.x) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's x velo exceeds a full tile size!");
  }
  if (Abs(spr->subVelo.y) > (TILE_SIZE_SUB))
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's y velo exceeds a full tile size!");
  }

  // Add velo to pos
  // AddVec(&spr->subPos, &spr->subVelo);

  // Start with H movement
  TryMove(spr, true);
  // TryMove(spr, false);

  /*
  // check for foot collision
  Vec2 worldFootPos = GetWorldPointOnSprite(spr, PIV_MIDDLE_BOTTOM);


  // for debugging
  // __TEST__
  Vec2 screenFootPos = World2Screen(&worldFootPos);
  vmupro_draw_rect( screenFootPos.x, screenFootPos.y, screenFootPos.x + 2, screenFootPos.y + 2, VMUPRO_COLOR_GREY );

  int tileCol = worldFootPos.x / TILE_SIZE_PX;
  int tileRow = worldFootPos.y / TILE_SIZE_PX;

  int blockId = GetBlockIDAtColRow(tileCol, tileRow);

  if ( blockId > -1 ){
    printf("block id at %dx%d = %d\n", tileCol, tileRow, blockId);
  }
  */
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

  // we moved left/right?
  Vec2 *lastSubPos = &player.spr.lastSubPos;
  Vec2 subPos = GetSubPos(spr);
  int xDelta = subPos.x - lastSubPos->x;
  int yDelta = subPos.y - lastSubPos->y;

  if (spr->facingRight && xDelta < 0)
  {
    // moved left
    spr->facingRight = false;
  }
  if (!spr->facingRight && xDelta > 0)
  {
    // moved right
    spr->facingRight = true;
  }

  bool goingUp = yDelta < 0;

  // __TEST__ temporary thing
  if (goingUp)
  {
    spr->img = &img_player_fall_raw;
    OnSpriteUpdated(&player.spr);
  }
  else
  {
    spr->img = &img_player_idle_raw;
    OnSpriteUpdated(&player.spr);
  }

  // update the img pointer
  img = player.spr.img;

  /* __TEST__ using masks
  vmupro_drawflags_t flags = (player.facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);

    uint8_t mask[img->width * img->height];
    for( int i = 0; i < sizeof(mask); i++ ){
      int x = (i / 8);
      mask[i] = (x % 2 == 0 );
    }
  vmupro_blit_buffer_masked(img->data, mask, screenBoxPos.x, screenBoxPos.y, img->width, img->height, flags);
  */

  vmupro_drawflags_t flags = (spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);
  vmupro_blit_buffer_transparent(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, VMUPRO_COLOR_BLACK, flags);

  // vmupro_drawflags_t flags = (spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);
  // vmupro_blit_buffer_flipped(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, flags);

  // draw something at the player's feet pos for debugging
  // vmupro_blit_buffer_at(img->data, screenFeetPos.x, screenFeetPos.y, img->width, img->height);
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

    if (DEBUG_BBOX)
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

    frameCounter++;
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
