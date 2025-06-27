
#include <string.h>
#include "vmupro_sdk.h"
#include "images/images.h"
#include "images/level_1_layer_0.h"

const char *TAG = "[Platformer]";

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

uint32_t GetBlockIDAtPos(int x, int y)
{

  if (currentLevel == NULL)
    return 0;

  uint32_t offset = (y * currentLevel->widthInTiles) + x;

  uint32_t block = currentLevel->blockData[offset];
  return block - 1;
}

void DrawLevelBlock(int x, int y)
{

  if (currentLevel == NULL)
    return;

  uint32_t blockId = GetBlockIDAtPos(x, y);

  if (blockId == 0xFFFFFFFF)
  {
    return;
  }

  const uint32_t TILEMAP_WIDTH_TILES = 8;
  const uint32_t TILE_SIZE = 16;

  uint32_t pixSrcX = (blockId % TILEMAP_WIDTH_TILES) * TILE_SIZE;
  uint32_t pixSrcY = (blockId / TILEMAP_WIDTH_TILES) * TILE_SIZE;

  uint32_t pixTargX = x * TILE_SIZE;
  uint32_t pixTargY = y * TILE_SIZE;

  const Img *sheet = &img_tilemap_raw;
  vmupro_blit_tile(sheet->data, pixTargX + scrollx, pixTargY, pixSrcX, pixSrcY, TILE_SIZE, TILE_SIZE, sheet->width);
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

    const Img *img = &img_vmu_circle_raw;
    // vmupro_blit_buffer_at(img->data, 120, 20, img->width, img->height);

    // srcx, srcy, tilemapwidth
    // blit from the 2nd tile in the spritesheet
    // const Img *sheet = &img_tilemap_raw;
    // const int TILESIZE = 16 * 3;
    // vmupro_blit_tile(sheet->data, 20, 20, 16, 0, TILESIZE, TILESIZE, sheet->width);

    // vmupro_blit_tile(sheet->data, 20, 70, 16, 16, TILESIZE, TILESIZE, sheet->width);

    // uint32_t *layer0 = (uint32_t *)level_1_layer_0_data;

    int extraTiles = 4;

    for (int y = 0; y < 16; y++)
    {
      for (int x = 0; x < 16 + extraTiles; x++)
      {
        DrawLevelBlock(x, y);
      }
    }

    if (!scrollReverse)
    {
      scrollx++;
      if (scrollx == extraTiles * 16)
      {
        scrollReverse = true;
      }
    }
    else
    {
      scrollx--;
      if (scrollx == 0)
      {
        scrollReverse = false;
      }
    }

    vmupro_push_double_buffer_frame();

    vmupro_sleep_ms(32);

    vmupro_btn_read();
    if (vmupro_btn_confirm_pressed())
    {
      break;
    }
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
