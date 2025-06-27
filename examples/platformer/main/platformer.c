
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

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

int playerX = 80;
int playerY = MAP_HEIGHT_PIXELS - (TILE_SIZE_PX * 4);
int camX = 0;
int camY = 0;

typedef struct
{
  uint32_t widthInTiles;
  uint32_t heightInTiles;
  uint32_t blockData[];
} LevelData;

LevelData *currentLevel = NULL;

int scrollx = 0;
int scrollReverse = false;

void LoadLevel(int levelNum)
{

  currentLevel = (LevelData *)level_1_layer_0_data;
}

#define BLOCK_NULL 0xFFFFFFFF

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

  // check if cam's going off left of the level
  int camLeft = playerX - (SCREEN_WIDTH / 2);
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
  int camTop = playerY - (SCREEN_WIDTH / 2);
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

void DrawPlayer()
{

  int playerDrawPosX = playerX - camX;
  int playerDrawPosY = playerY - camY;

  // draw the 'player'
  const Img *img = &img_vmu_circle_raw;
  vmupro_blit_buffer_at(img->data, playerDrawPosX - 22, playerDrawPosY - 22, img->width, img->height);
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

    // srcx, srcy, tilemapwidth
    // blit from the 2nd tile in the spritesheet
    // const Img *sheet = &img_tilemap_raw;
    // const int TILESIZE = 16 * 3;
    // vmupro_blit_tile(sheet->data, 20, 20, 16, 0, TILESIZE, TILESIZE, sheet->width);

    // vmupro_blit_tile(sheet->data, 20, 70, 16, 16, TILESIZE, TILESIZE, sheet->width);

    // uint32_t *layer0 = (uint32_t *)level_1_layer_0_data;

    SolveCamera();

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

    DrawPlayer();

    // if (!scrollReverse)
    // {
    //   scrollx++;
    //   if (scrollx == extraTiles * 16)
    //   {
    //     scrollReverse = true;
    //   }
    // }
    // else
    // {
    //   scrollx--;
    //   if (scrollx == 0)
    //   {
    //     scrollReverse = false;
    //   }
    // }

    vmupro_push_double_buffer_frame();

    vmupro_sleep_ms(32);

    vmupro_btn_read();
    if (vmupro_btn_confirm_pressed())
    {
      break;
    }

    if (vmupro_btn_held(DPad_Right))
    {
      playerX += 3;
    }
    if (vmupro_btn_held(DPad_Left))
    {
      playerX -= 3;
    }

    if (vmupro_btn_held(DPad_Up))
    {
      playerY -= 3;
    }
    if (vmupro_btn_held(DPad_Down))
    {
      playerY += 3;
    }
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
