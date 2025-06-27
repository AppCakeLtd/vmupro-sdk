
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

const bool DEBUG_BBOX = true;

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
  // typically the image's bbox
  BBox bbox;
  // the actual image data
  const Img *img;
  // since the player's pos might be
  // the feet pos for example
  Vec2 pos;
} Sprite;

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

// in: absolute position in level
// out: position on screen
// (obvs might be off screen, check that elsewhere)
Vec2 GetScreenPos(Vec2 *srcPos)
{

  Vec2 returnVal;
  returnVal.x = srcPos->x - camX;
  returnVal.y = srcPos->y - camY;
  return returnVal;
}

// in screen spsace
void DrawSpriteBoundingBox(Sprite *inSprite, uint16_t inCol)
{

  BBox *box = &inSprite->bbox;
  Vec2 absPos = inSprite->bbox.vecs.pos;
  Vec2 screenPos = GetScreenPos(&absPos);
  int x2 = screenPos.x + box->width;
  int y2 = screenPos.y + box->height;
  vmupro_draw_rect(screenPos.x, screenPos.y, x2, y2, inCol);
  
}

// Updates the bounding box when the pos or img changes
void OnSpriteUpdated(Sprite *spr, bool forPlayer)
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

  if (forPlayer)
  {
    // if it's the player, we'll centre around the feet
    spr->bbox.x = spr->pos.x - (img->width / 2);
    spr->bbox.y = spr->pos.y - (img->height);
    spr->bbox.width = img->width;
    spr->bbox.height = img->height;
  }
  else
  {
    // else we'll centre around the centre.
    spr->bbox.x = spr->pos.x - (img->width / 2);
    spr->bbox.y = spr->pos.y - (img->height / 2);
    spr->bbox.width = img->width;
    spr->bbox.height = img->height;
  }
}

Vec2 *GetPlayerPos()
{

  return &player.spr.pos;
}

void SetPlayerPos(int inX, int inY)
{

  player.spr.pos.x = inX;
  player.spr.pos.y = inY;
  OnSpriteUpdated(&player.spr, true);
}

void MovePlayer(int inX, int inY)
{

  Vec2 tPos = *GetPlayerPos();
  tPos.x += inX;
  tPos.y += inY;
  SetPlayerPos(tPos.x, tPos.y);
}

void ResetPlayer(Player *ply)
{

  ply->spr.img = &img_vmu_circle_raw;
  Vec2 startPos = {80, MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4)};
  SetPlayerPos(startPos.x, startPos.y);
}

void LoadLevel(int levelNum)
{

  currentLevel = (LevelData *)level_1_layer_0_data;
  ResetPlayer(&player);
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

void UpdateInputs()
{

  vmupro_btn_read();

  Vec2 *pPos = GetPlayerPos();
  if (vmupro_btn_held(DPad_Right))
  {

    MovePlayer(3, 0);
  }
  if (vmupro_btn_held(DPad_Left))
  {
    MovePlayer(-3, 0);
  }

  if (vmupro_btn_held(DPad_Up))
  {
    MovePlayer(0, -3);
  }
  if (vmupro_btn_held(DPad_Down))
  {
    MovePlayer(0, 3);
  }
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

  Vec2 *pPos = GetPlayerPos();

  // check if cam's going off left of the level
  int camLeft = pPos->x - (SCREEN_WIDTH / 2);
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
  int camTop = pPos->y - (SCREEN_WIDTH / 2);
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

void DrawBackgroundTiles()
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

void DrawPlayer()
{

  Vec2 *absFeetPos = GetPlayerPos();
  Vec2 screenFeetPos = GetScreenPos(absFeetPos);

  // draw based on the actual bounding box
  Vec2 absBoxPos = player.spr.bbox.vecs.pos;
  Vec2 screenBoxPos = GetScreenPos(&absBoxPos);

  const Img *img = player.spr.img;

  vmupro_blit_buffer_at(img->data, screenBoxPos.x, screenBoxPos.y, img->width, img->height);

  // draw something at the player's feet pos for debugging
  vmupro_blit_buffer_at(img->data, screenFeetPos.x, screenFeetPos.y, img->width, img->height);
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

    DrawBackgroundTiles();

    DrawPlayer();

    if (DEBUG_BBOX)
    {
      DrawSpriteBoundingBox(&player.spr, VMUPRO_COLOR_WHITE);
    }

    vmupro_push_double_buffer_frame();


    vmupro_sleep_ms(32);

    UpdateInputs();

    if (vmupro_btn_confirm_pressed())
    {
      break;
    }

    frameCounter++;
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
