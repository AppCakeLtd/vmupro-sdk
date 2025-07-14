
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "anims.h"
#include "images/level_1_layer_0.h"
#include "images/level_1_layer_1.h"

const char *TAG = "[Platformer]";

// float in air for collision testing
const bool NO_GRAV = false;
const bool DEBUG_SPRITEBOX = false;
const bool DEBUG_HITPOINTS = false;
const bool DEBUG_SCROLL_ZONE = false;
const bool DEBUG_NO_X = false;
const bool DEBUG_NO_Y = false;

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

const int SUBACCEL_WALK = 8;
const int SUBACCEL_RUN = 9;
const int SUBACCEL_AIR = 6;

const int SUBDAMPING_WALK = 6;
const int SUBDAMPING_RUN = 6;
const int SUBDAMPING_AIR = 4;

// max frames for which the up force is applied
const int MAX_JUMP_BOOST_FRAMES = 16;
const int SUB_JUMPFORCE = 14;
const int SUB_GRAVITY = 9;
// max of like 256 since that's bigger than a tile
const int MAX_SUBFALLSPEED = 120;

int camX = 0;
int camY = 0;
int frameCounter = 0;

// prevent rubber banding, move the camera within a scrolling
// area which allows you to see further ahead/behind
// based on position
#define SCROLLZONE_WIDTH 28
#define SCROLLZONE_MAXOFFSET 40
#define SCROLLZONE_SPEED 3
int scrollZoneWorldX = 50;
int scrollzoneOffsetX = 0;

typedef struct
{
  uint32_t widthInTiles;
  uint32_t heightInTiles;
  uint8_t blockData[];
} LevelData;

LevelData *levelBG = NULL;
LevelData *levelCols = NULL;

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

typedef enum
{
  STYPE_PLAYER,
  STYPE_TESTMOB
} SpriteType;

typedef struct
{
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

  AnimGroup *anims;
  AnimFrames *activeFrameSet;
  // updated per frame, to measure elapsed frames
  int lastGameframe;
  // reset on anim changes
  int animIndex;
  bool animReversing;
  AnimTypes animID;

  SpriteType sType;

} Sprite;

#define MAX_SPRITES 20
int numSprites = 0;
Sprite *sprites[MAX_SPRITES];

void SetAnim(Sprite *spr, AnimTypes inType)
{

  if (spr->animID == inType)
  {
    return;
  }

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Anim ID %d", frameCounter, (int)inType);

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
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite has null frameset!\n");
    return false;
  }

  // is it within bounds?
  if (spr->animIndex < 0 || spr->animIndex >= spr->activeFrameSet->numImages)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Sprite's frame index is outside the bounds 0-%d!\n", spr->activeFrameSet->numImages);
    spr->animIndex = 0;
  }

  return true;
}

const Img *GetActiveImage(Sprite *spr)
{

  if (!ValidateAnim(spr))
  {
    return &img_player_idle_0_raw;
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

Sprite *player = NULL;

//
// Protos
//
//__TEST__ This can be scrubbed when things are moved to headers
bool CheckGrounded(Sprite *spr);
Vec2 GetPointOnSprite(Sprite *spr, bool hitBox, Anchor_H anchorH, Anchor_V anchorV);

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

// in screen spsace
void DrawSpriteBoundingBox(Sprite *inSprite, uint16_t inCol)
{

  BBox *box = &inSprite->worldBBox;
  DrawBBoxWorld(box, inCol);
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

  spr->moveMode = inMode;

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Frame %d Move mode %d", frameCounter, (int)inMode);
}

Vec2 GetPlayerWorldStartPos()
{
  Vec2 rVal = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4)};
  return rVal;
}

bool CheckFellOffMap()
{

  Vec2 topleftPoint = GetPointOnSprite(player, false, player->anchorH, player->anchorV);
  if (topleftPoint.y > MAP_HEIGHT_PIXELS + TILE_SIZE_PX)
  {
    vmupro_log(VMUPRO_LOG_WARN, TAG, "Frame %d player fell off map at yPos %d\n", frameCounter, player->subPos.y);
    Vec2 worldStartPos = GetPlayerWorldStartPos();
    SetWorldPos(player, &worldStartPos);
    return true;
  }
  return false;
}

void ResetSprite(Sprite *spr)
{

  memset(spr, 0, sizeof(Sprite));

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
  Vec2 worldStartPos = GetPlayerWorldStartPos();
  SetWorldPos(spr, &worldStartPos);
  spr->isPlayer = true;
  spr->anchorH = ANCHOR_VTOP;
  spr->anchorV = ANCHOR_HLEFT;

  spr->anims = &animgroup_player;
  spr->activeFrameSet = &spr->anims->idleFrames;
  spr->animIndex = 0;
  spr->animReversing = false;
  spr->lastGameframe = frameCounter;
  spr->animID = ANIMTYPE_IDLE;

  // update the hitbox, bounding box, etc
  OnSpriteMoved(spr);
}

Sprite *CreateSprite(SpriteType inType, Vec2 worldPos)
{

  if (numSprites == MAX_SPRITES)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Out of sprite slots");
    return NULL;
  }

  Sprite *returnVal = malloc(sizeof(Sprite));
  memset(returnVal, 0, sizeof(Sprite));

  ResetSprite(returnVal);

  sprites[numSprites] = returnVal;
  numSprites++;

  return returnVal;
}

void LoadLevel(int levelNum)
{

  levelBG = (LevelData *)level_1_layer_0_data;
  levelCols = (LevelData *)level_1_layer_1_data;

  player = CreateSprite(STYPE_PLAYER, GetPlayerWorldStartPos());

  Vec2 testPos = GetPlayerWorldPos();
  testPos.x += 64;
  CreateSprite(STYPE_TESTMOB, testPos);
}

// returns atlas block 0-max
// note: the .map file uses 0x00 for blank spots
// so we'll always sub 1 to get a 0-indexed
// value into our spritesheet/atlas
uint32_t GetBlockIDAtColRow(int blockCol, int blockRow, int layer)
{

  LevelData *level = layer == 0 ? levelBG : levelCols;

  if (level == NULL)
    return BLOCK_NULL;

  // bounds check the input values
  if (blockCol < 0 || blockRow < 0)
  {
    return BLOCK_NULL;
  }

  if (blockCol >= level->widthInTiles || blockRow >= level->heightInTiles)
  {
    return BLOCK_NULL;
  }

  uint32_t offset = (blockRow * level->widthInTiles) + blockCol;

  uint32_t block = level->blockData[offset];
  return block - 1;
}

void UpdateSpriteInputs(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to UpdateSpriteInputs");
    return;
  }

  Inputs *inp = &spr->input;

  if (spr->sType == STYPE_PLAYER)
  {

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
    memset(&spr->input, 0, sizeof(Inputs));

    bool pingPong = (frameCounter / 100) % 2 == 0;
    inp->right = pingPong;
    inp->left = !pingPong;
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
    Sprite *spr = sprites[i];
    UpdateSpriteInputs(spr);
  }
}

void DrawLevelBlock(int x, int y, int layer)
{

  LevelData *level = layer == 0 ? levelBG : levelCols;

  if (level == NULL)
    return;

  uint32_t blockId = GetBlockIDAtColRow(x, y, layer);

  if (blockId == BLOCK_NULL)
  {
    return;
  }

  uint32_t pixSrcX = (blockId % TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;
  uint32_t pixSrcY = (blockId / TILEMAP_WIDTH_TILES) * TILE_SIZE_PX;

  uint32_t pixTargX = x * TILE_SIZE_PX;
  uint32_t pixTargY = y * TILE_SIZE_PX;

  const Img *sheet = &img_tilemap_raw;
  vmupro_drawflags_t flags = VMUPRO_DRAWFLAGS_NORMAL;
  vmupro_color_t transColor = VMUPRO_COLOR_BLACK;
  // vmupro_blit_tile(sheet->data, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width);

  // bit of a hack, but hey, everything at or below row 18 in the tilemap is transparent
  if (pixSrcY >= 18 * TILE_SIZE_PX)
  {
    vmupro_blit_tile_advanced(sheet->data, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width, transColor, flags);
  }
  else
  {
    vmupro_blit_tile(sheet->data, pixTargX - camX, pixTargY - camY, pixSrcX, pixSrcY, TILE_SIZE_PX, TILE_SIZE_PX, sheet->width);
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

  Img *img = &img_bg_1_raw;

  int bgScrollX = (camX * 4) / 5;
  int bgScrollY = (camY * 4) / 5;

  vmupro_blit_scrolling_background(img->data, img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
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
  Direction whereWasCollision;

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

bool IsBlockID2Sided(int blockId)
{

  if (blockId >= 48 && blockId <= 50)
    return true;
  if (blockId >= 59 && blockId <= 60)
    return true;
  if (blockId >= 72 && blockId <= 74)
    return true;
  if (blockId >= 83 && blockId <= 84)
    return true;
  if (blockId >= 144)
    return true;

  return false;
}

bool Ignore2SidedBlock(int blockId, int layer, Sprite *spr, Vec2 *tileSubPos)
{

  // if it's not 2-sideable, just early exit
  if (!IsBlockID2Sided(blockId))
    return false;

  bool movingDown = spr->subVelo.y > 0;
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

// subOffsetOrNull adds an offset to where we check for collisions
// e.g. when moving right we check currentPos.x + velo.x
// for where we'll be, rather than where we are
// Used for for collision and for ground checks
void GetHitInfo(HitInfo *rVal, Sprite *spr, Direction dir, Vec2 *subOffsetOrNull, const char *src)
{

  rVal->whereWasCollision = dir;
  rVal->lastHitIndex = -1;

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

  rVal->hitSomething = false;

  // finally run some tile collision checks:
  for (int i = 0; i < 3; i++)
  {

    // just the literal tile indexes into the x/y grid
    Vec2 tileRowAndCol = GetTileRowAndColFromSubPos(&rVal->subCheckPos[i]);

    int blockId = GetBlockIDAtColRow(tileRowAndCol.x, tileRowAndCol.y, LAYER_COLS);
    if (blockId != BLOCK_NULL)
    {

      // essentially we're rounding down and then back up
      Vec2 tileSubPos = GetTileSubPosFromRowAndCol(&tileRowAndCol);

      bool ignore = Ignore2SidedBlock(blockId, LAYER_COLS, spr, &tileSubPos);

      if (ignore)
      {
        // printf("ignoring 2 sided block\n");
        continue;
      }

      rVal->hitSomething = true;

      rVal->blockID[i] = blockId;
      rVal->lastHitIndex = i;

      // default to the hit point being wherever we cheked on the sprite
      // we'll tweak the x/y in a sec, depending on where we hit the block
      rVal->subEjectionPoint[i].x = rVal->subCheckPos[i].x;
      rVal->subEjectionPoint[i].y = rVal->subCheckPos[i].y;

      if (dir == DIR_RIGHT)
      {
        // collided on the sprite's right
        // so the hit point is the block's X pos
        rVal->subEjectionPoint[i].x = tileSubPos.x;
      }
      else if (dir == DIR_LEFT)
      {
        // collided on the sprite's left
        // so the hit point is the block's x+width
        rVal->subEjectionPoint[i].x = tileSubPos.x + TILE_SIZE_SUB;
      }
      else if (dir == DIR_DOWN)
      {
        // collided on the sprite's bottom
        // so the hitpoint is the block's top
        rVal->subEjectionPoint[i].y = tileSubPos.y;
      }
      else if (dir == DIR_UP)
      {
        // collided on the sprite's top
        // so the hitpoint is teh block's bottom
        rVal->subEjectionPoint[i].y = tileSubPos.y + TILE_SIZE_SUB;
      }
    }
    else
    {
      rVal->blockID[i] = BLOCK_NULL;
    }
  }
}

void PrintHitInfo(HitInfo *info)
{

  printf("HitInfo dir %d hit = %d\n", info->whereWasCollision, (int)info->hitSomething);
  printf("   ids = %d, %d, %d\n", info->blockID[0], info->blockID[1], info->blockID[2]);
  printf("   chX = %d, %d, %d\n", info->subCheckPos[0].x, info->subCheckPos[1].x, info->subCheckPos[2].x);
  printf("   chY = %d, %d, %d\n", info->subCheckPos[0].y, info->subCheckPos[1].y, info->subCheckPos[2].y);
  printf("   ejX = %d, %d, %d\n", info->subEjectionPoint[0].x, info->subEjectionPoint[1].x, info->subEjectionPoint[2].x);
  printf("   ejY = %d, %d, %d\n", info->subEjectionPoint[0].y, info->subEjectionPoint[1].y, info->subEjectionPoint[2].y);
}

// Perform the ejection part after collecting hit info
void GetEjectionInfo(Sprite *spr, HitInfo *info, bool horz)
{

  // simple early exit
  if (!info->hitSomething)
  {
    return;
  }

  // the direction we hit something at
  Direction whereWasCollision = info->whereWasCollision;
  // and which of the 3 points generated a collision
  int idx = info->lastHitIndex;

  int dbgBlockX = info->subEjectionPoint[idx].x;
  int dbgPlayerX = spr->subPos.x;
  int dbgBlockY = info->subEjectionPoint[idx].y;
  int dbgPlayerY = spr->subPos.y;

  if (whereWasCollision == DIR_RIGHT)
  {

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
    // SetSubPos(spr, &sub);
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_LEFT)
  {

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
    // SetSubPos(spr, &sub);
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_UP)
  {

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
    // SetSubPos(spr, &sub);
    info->snapPoint = sub;
  }

  if (whereWasCollision == DIR_DOWN)
  {

    int dbgBlockY = info->subEjectionPoint[idx].y;
    int dbgPlayerY = spr->subPos.y;

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
    // SetSubPos(spr, &sub);
    info->snapPoint = sub;
  }

  // debug block
  if (true)
  {
    printf("__DBG__ Frame %d eject from dir (urdl) %d\n", frameCounter, whereWasCollision);
    printf("....from blockpos x=%d/%d y=%d/%d\n", dbgBlockX, dbgBlockX >> SHIFT, dbgBlockY, dbgBlockY >> SHIFT);
    printf("....from plry pos x=%d/%d y=%d/%d\n", dbgPlayerX, dbgPlayerX >> SHIFT, dbgPlayerY, dbgPlayerY >> SHIFT);
    Vec2 newPos = info->snapPoint;
    printf("....TO   plry pos x=%d/%d y=%d/%d\n", newPos.x, newPos.x >> SHIFT, newPos.y, newPos.y >> SHIFT);
    printf("....(provisionally)\n");

    // // To double check your collision logics
    // if (whereWasCollision == DIR_DOWN)
    // {
    //   bool groundedNow = CheckGrounded(spr);
    //   printf("__DBG__ post ejection ground check = %d\n", (int)groundedNow);
    // }
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

  if (info->hitSomething)
  {
    // PrintHitInfo(&info);
  }

  GetEjectionInfo(spr, info, horz);

  if (info->hitSomething)
  {
    // return sign of direction
    return (dir == DIR_RIGHT || dir == DIR_DOWN) ? 1 : -1;
  }
  return 0;
}

bool CheckGrounded(Sprite *spr)
{

  // the hitbox ends on the very last subpixel
  Vec2 subGroundCheckOffset = {0, 1};
  HitInfo nhi;
  memset(&nhi, 0, sizeof(HitInfo));
  GetHitInfo(&nhi, spr, DIR_DOWN, &subGroundCheckOffset, "groundcheck");

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
    printf("__DBG__ jump boost canceled, src='%s'\n", src);
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

//__TEST__ Not yet applied
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
// - check ground state
// - check landing, etc

void SolveMovement(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to SolveMovement");
    return;
  }

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
  if (NO_GRAV)
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

  if (!DEBUG_NO_X)
  {
    // Apply X velo to X movement + update bounding boxes
    AddSubPos2(spr, spr->subVelo.x, 0);

    // Eject from any X Collisions
    GetHitInfoAndEjectionInfo(&xHitInfo, spr, true);

  } // DEBUG_NO_X

  int vBonk = 0;

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
  if (xHitInfo.hitSomething && !yHitInfo.hitSomething)
  {
    // printf("Frame %d hit on X only\n", frameCounter);
    SetSubPosX(spr, xHitInfo.snapPoint.x);
    spr->subVelo.x = 0;
  }
  else if (!xHitInfo.hitSomething && yHitInfo.hitSomething)
  {
    printf("Frame %d hit on Y only\n", frameCounter);
    SetSubPosY(spr, yHitInfo.snapPoint.y);
    spr->subVelo.y = 0;
  }
  else if (xHitInfo.hitSomething && yHitInfo.hitSomething)
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
      GetHitInfoAndEjectionInfo(&yHitInfo, spr, false);
      if (yHitInfo.hitSomething)
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
      GetHitInfoAndEjectionInfo(&xHitInfo, spr, true);
      if (xHitInfo.hitSomething)
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

  spr->isGrounded = CheckGrounded(spr);
  // printf("Frame %d is grounded %d yVel = %d\n", frameCounter, spr->isGrounded, spr->subVelo.y);

  CheckLanded(spr);
  CheckFellOffMap();

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
    Sprite *spr = sprites[i];
    SolveMovement(spr);
  }
}

bool SpriteIsMoving(Sprite *spr)
{

  return (spr->subVelo.x != 0) || (spr->subVelo.y != 0);
}

void DrawSprite(Sprite *spr)
{

  if (spr == NULL)
  {
    vmupro_log(VMUPRO_LOG_ERROR, TAG, "Null sprite passed to DrawSprite");
    return;
  }

  Vec2 worldOrigin = GetWorldPos(spr);
  Vec2 screenOrigin = World2Screen(&worldOrigin);

  // draw based on the actual bounding box
  Vec2 worldBoxPos = spr->worldBBox.vecs.pos;
  Vec2 screenBoxPos = World2Screen(&worldBoxPos);

  bool goingUp = spr->subVelo.y < 0;

  bool isMoving = SpriteIsMoving(spr);

  // everything else
  if (!isMoving)
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

  // update the img pointer, in case it's changed due to anims
  const Img *img = GetActiveImage(spr);

  vmupro_drawflags_t flags = (!spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H);
  uint16_t mask = *(uint16_t *)&img->data[0];
  vmupro_blit_buffer_transparent(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, mask, flags);
}

void DrawAllSprites()
{

  for (int i = 0; i < numSprites; i++)
  {
    Sprite *spr = sprites[i];
    DrawSprite(spr);
  }
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
    DrawGroundtiles(LAYER_BG);
    DrawGroundtiles(LAYER_COLS);

    InputAllSprites();
    MoveAllSprites();
    DrawAllSprites();

    if (DEBUG_SPRITEBOX)
    {
      DrawSpriteBoundingBox(player, VMUPRO_COLOR_WHITE);
    }
    if (DEBUG_SCROLL_ZONE)
    {
      DrawCamScrollZone();
    }

    vmupro_push_double_buffer_frame();

    EndOfFrame(player);

    vmupro_sleep_ms(10);

    if (vmupro_btn_held(Btn_Mode))
    {
      break;
    }

    // test: cycle through sprite offsets
    if (vmupro_btn_pressed(Btn_A))
    {
      printf("PlayerX world:%d sub: %d \n", GetWorldPos(player).x, GetSubPos(player).x);
      printf("PlayerY world:%d sub: %d \n", GetWorldPos(player).y, GetSubPos(player).y);
      printf("Player grounded? %d\n", (int)player->isGrounded);
    }

    if (vmupro_btn_pressed(Btn_B))
    {
      // player->anchorV = (player->anchorV + 1) % (3);
      //  player->anchorH = (player->anchorH + 1) % (3);
      // OnSpriteMoved(player);
    }

    frameCounter++;
    Anims_Player.lastFrame = frameCounter;
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
