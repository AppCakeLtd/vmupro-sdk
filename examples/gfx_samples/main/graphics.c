
#include <string.h>
#include "vmupro_sdk.h"
#include "placeholder.h"
#include "images/icons.h"
#include "sdk_tile_bg_brown.h"
#include "sdk_tile_bg_blue.h"

const char *TAG = "[GFX Samples]";

// foreground: camera/player/test objs
// TODO

// ground
#define GROUND_WIDTH_TILES_X 16
#define GROUND_WIDTH_TILES_Y 16
#define NUM_GROUND_TILES GROUND_WIDTH_TILES_X *GROUND_WIDTH_TILES_Y
// arrange it y/x for better memory cache access
Img *tileList[GROUND_WIDTH_TILES_Y][GROUND_WIDTH_TILES_X];

// background
int bgScrollX = 0;
int bgScrollY = 0;
const int BG_TILE_SIZE = 65;
const int SCREEN_WIDTH = 240;
const int SCREEN_HEIGHT = 240;

// test function
int testNum = 0;

// Little dvd bouncer that bounces off the edge of the screen
// used for various tests
typedef struct
{
  bool reverse; // positive
  int xPos;
  int yPos;
  int max;
} DVDBounce;

// 2 sets of xy positions
DVDBounce bounce1 = {false, 10, 24, SCREEN_WIDTH};
DVDBounce bounce2 = {false, 80, 86, SCREEN_WIDTH};
// For params like rotation, alpha
DVDBounce bounce3 = {false, 0, 0, 255};
DVDBounce bounce4 = {false, 128, 128, 255};

void UpdateDVDBounce(DVDBounce *bounce)
{

  if (!bounce->reverse)
  {
    bounce->xPos++;
    if (bounce->xPos >= bounce->max - 1)
    {
      bounce->reverse = true;
    }
  }
  else
  {
    bounce->xPos--;
    if (bounce->xPos < 0)
    {
      bounce->reverse = false;
    }
  }

  if (!bounce->reverse)
  {
    bounce->yPos++;
    if (bounce->yPos >= bounce->max - 1)
    {
      bounce->reverse = true;
    }
  }
  else
  {
    bounce->yPos--;
    if (bounce->yPos < 0)
    {
      bounce->reverse = false;
    }
  }
}

void InitForeground()
{
}

void DrawForeground()
{
}

void InitGround()
{
  memset(tileList, 0, sizeof(tileList[0]) * NUM_GROUND_TILES);
}

// Temporary solution
void DrawGround()
{

  // these would be encoded somewhere instead of in-line like this
  // usually by something like asperite or a custom tool
  // little block at the bottom left
  tileList[12][1] = &img_ground_grass_tl_raw;
  tileList[12][2] = &img_ground_grass_tm_raw;
  tileList[12][3] = &img_ground_grass_tm_raw;
  tileList[12][4] = &img_ground_grass_tr_raw;
  tileList[13][1] = &img_ground_grass_bl_raw;
  tileList[13][2] = &img_ground_grass_bm_raw;
  tileList[13][3] = &img_ground_grass_bm_raw;
  tileList[13][4] = &img_ground_grass_br_raw;

  // next block
  tileList[11][5] = &img_ground_grass_tl_raw;
  tileList[11][6] = &img_ground_grass_tm_raw;
  tileList[11][7] = &img_ground_grass_tm_raw;
  tileList[11][8] = &img_ground_grass_tr_raw;
  tileList[12][5] = &img_ground_grass_ml_raw;
  tileList[12][6] = &img_ground_grass_mm_raw;
  tileList[12][7] = &img_ground_grass_mm_raw;
  tileList[12][8] = &img_ground_grass_mr_raw;
  tileList[13][5] = &img_ground_grass_bl_raw;
  tileList[13][6] = &img_ground_grass_bm_raw;
  tileList[13][7] = &img_ground_grass_bm_raw;
  tileList[13][8] = &img_ground_grass_br_raw;

  // the big middle block
  tileList[9][8] = &img_ground_grass_tl_raw;
  tileList[9][9] = &img_ground_grass_tm_raw;
  tileList[9][10] = &img_ground_grass_tm_raw;
  tileList[9][11] = &img_ground_grass_tm_raw;
  tileList[9][12] = &img_ground_grass_tr_raw;

  for (int y = 0; y < 16; y++)
  {
    for (int x = 0; x < 16; x++)
    {
      Img *src = tileList[y][x];
      if (src == NULL)
        continue;

      int drawX = x * 16;
      int drawY = y * 16;
      vmupro_blit_buffer_at(src->data, drawX, drawY, src->width, src->height);
    }
  }
}

void InitBackground()
{
}

void DrawBackground()
{
  int method = 2;

  // method 1:
  // draw an extra tile, and scroll them slightly off screen with modulo
  if (method == 1)
  {
    // Start by making the user feel kinda sick
    bgScrollX += 1;
    bgScrollX = bgScrollX % BG_TILE_SIZE;
    bgScrollY += 1;
    bgScrollY = bgScrollY % BG_TILE_SIZE;
    Img *img = &img_sdk_tile_bg_brown_raw;
    vmupro_blit_tile_pattern(img->data, img->width, img->height, 0 - BG_TILE_SIZE + bgScrollX, 0 - BG_TILE_SIZE + bgScrollY, SCREEN_WIDTH + BG_TILE_SIZE, SCREEN_HEIGHT + BG_TILE_SIZE);
  }

  // method 2:
  // let the sdk handle it, with blit_scrolling_background()
  if (method == 2)
  {
    Img *img = &img_sdk_tile_bg_brown_raw;
    bgScrollX += 1;
    bgScrollY += 1;
    vmupro_blit_scrolling_background(img->data, img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  // method 3:
  // let the sdk handle it, with vmupro_blit_infinite_scrolling_background()
  // note: still broken
  if (method == 3)
  {
    Img *img = &img_sdk_tile_bg_brown_raw;
    bgScrollX += 1;
    bgScrollY += 1;
    vmupro_blit_infinite_scrolling_background(img->data, img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}

void DrawTestFunction(int testNum)
{

  UpdateDVDBounce(&bounce1);
  UpdateDVDBounce(&bounce2);
  UpdateDVDBounce(&bounce3);
  UpdateDVDBounce(&bounce4);

  // simple image
  // issue for claude: flickering + detached columns with image size 55x55
  if (testNum == 0)
  {
    Img *img = &img_vmu_circle_raw;
    uint8_t * data = img_vmu_circle_raw.data;    
    vmupro_blit_buffer_at(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height);
    vmupro_blit_buffer_at(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height);
  }

  // rotated image
  // issue for claude: flickering + detatched columns, same as method #1
  if (testNum == 1)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_rotated_90(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, (bounce3.xPos / 16) % 4);
    vmupro_blit_buffer_rotated_90(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, (bounce3.yPos / 16) % 4);
  }

  // alpha blended image
  // issue for claude:
  //                  - SIMD part displays utter garbage
  //                  - Scalar part works but the byte order appears wrong
  if (testNum == 2)
  {
    Img *img = &img_vmu_circle_raw;
    int alpha = bounce3.xPos;
    vmupro_blit_buffer_blended(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, alpha);
    vmupro_blit_buffer_blended(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, 20);
  }

  // colour addition
  // issue flor claude: - immediately crashes the device
  if (testNum == 3)
  {
    Img *img = &img_vmu_circle_raw;
    uint16_t rgb565 = VMUPRO_COLOR_GREEN;
    // optional
    rgb565 = (rgb565 << 8) | (rgb565 >> 8);
    //vmupro_blit_buffer_color_add(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, rgb565);
  }

  if (testNum == 4)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_at(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height);
    vmupro_blit_buffer_at(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height);
  }
}

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "GFX Samples 3");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  InitForeground();
  InitGround();
  InitBackground();

  // Wait a bit to actually show the changes

  while (true)
  {

    vmupro_color_t col = VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    DrawBackground();
    DrawGround();
    DrawForeground();
    DrawTestFunction(testNum);

    vmupro_push_double_buffer_frame();

    // Nice long delay so we know what should be drawn at any given time
    vmupro_sleep_ms(32);

    if (vmupro_btn_pressed(DPad_Up) || vmupro_btn_pressed(DPad_Right))
    {
      testNum++;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Switched to test %d", testNum);
    }
    if (vmupro_btn_pressed(DPad_Down) || vmupro_btn_pressed(DPad_Down))
    {
      testNum--;
      if (testNum < 0)
      {
        testNum = 0;
      }
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Switched to test %d", testNum);
    }

    vmupro_btn_read();
    if (vmupro_btn_confirm_pressed())
    {
      break;
    }
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
