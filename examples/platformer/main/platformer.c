
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

const bool DEBUG_BBOX = false;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TILE_SIZE_PX 16

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
const int MAX_SUBSPEED_WALK = 16;
const int MAX_SUBSPEED_RUN = 16;

const int SUBACCEL_WALK = 12;
const int SUBACCEL_RUN = 9;
const int SUBACCEL_AIR = 6;

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
  MM_JUMPFROMWALK,
  MM_JUMPFROMRUN

} MoveMode;

typedef struct
{
  // typically the image's bbox
  BBox worldBBox;
  // the actual image data
  const Img *img;
  // Player's pos will be the feet pos for example
  // the sprite will be offset from this by -(width/2)
  // these values are subpixel / fixed point
  Vec2 subPos;
  // calculated at the end of the frame
  // since we may do multiple tweaks per frame
  Vec2 lastSubPos;

  Vec2 subAccel;
  Vec2 lastSubAccel;

  bool isPlayer;
  bool facingRight;
  Inputs input;
  MoveMode moveMode;
} Sprite;

#define SHIFT 4

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

bool SpriteInAir(Sprite *spr)
{

  MoveMode mm = spr->moveMode;
  if (mm == MM_FALL || mm == MM_JUMPFROMRUN || mm == MM_JUMPFROMWALK)
  {
    return true;
  }
  return false;
}

void SetMoveMode(Sprite *spr, MoveMode inMode)
{

  spr->moveMode = inMode;

  vmupro_log(VMUPRO_LOG_INFO, TAG, "Move mode %d", (int)inMode);
}

void ResetSprite(Sprite *spr)
{

  spr->img = &img_player_idle_raw;

  // temp
  Vec2 worldStartPos = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4)};
  SetWorldPos(spr, &worldStartPos);

  // update other struct vals
  spr->facingRight = true;
  spr->subPos = ZeroVec();
  spr->lastSubPos = ZeroVec();
  spr->subAccel = ZeroVec();
  spr->lastSubAccel = ZeroVec();
  SetMoveMode(spr, MM_FALL);
  memset(&spr->input, 0, sizeof(Inputs));
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
uint32_t GetBlockIDAtPos(int x, int y)
{

  if (currentLevel == NULL)
    return BLOCK_NULL;

  // bounds check the input values
  if (x < 0 || y < 0)
  {
    return BLOCK_NULL;
  }

  if (x >= currentLevel->widthInTiles || y >= currentLevel->heightInTiles)
  {
    return BLOCK_NULL;
  }

  uint32_t offset = (y * currentLevel->widthInTiles) + x;

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

  uint32_t blockId = GetBlockIDAtPos(x, y);

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
  inSpr->lastSubAccel = inSpr->subAccel;
}

void SolvePlayer()
{

  Sprite *spr = &player.spr;
  Inputs *inp = &spr->input;

  MoveMode mm = spr->moveMode;
  bool inAir = SpriteInAir(spr);

  if (inp->right)
  {
    AddSubPos2(spr, 3, 0);
  }
  if (inp->left)
  {
    AddSubPos2(spr, -3, 0);
  }

  if (inp->up)
  {
    AddSubPos2(spr, 0, -3);
  }
  if (inp->down)
  {
    AddSubPos2(spr, 0, 3);
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

  // vmupro_drawflags_t flags = (player.facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);
  // vmupro_blit_buffer_transparent(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, VMUPRO_COLOR_BLACK, flags);

  vmupro_drawflags_t flags = (spr->facingRight * VMUPRO_DRAWFLAGS_FLIP_H) | (goingUp * VMUPRO_DRAWFLAGS_FLIP_V);
  vmupro_blit_buffer_flipped(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height, flags);

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
