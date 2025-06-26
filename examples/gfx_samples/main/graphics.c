
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
  int min; // to test going off screen
  int max; // also to test going off screen
} DVDBounce;

// 2 sets of xy positions
DVDBounce bounce1 = {false, 10, 24, -10, SCREEN_WIDTH + 10};
DVDBounce bounce2 = {false, 80, 86, -10, SCREEN_WIDTH + 10};
// For params like rotation, alpha
DVDBounce bounce3 = {false, 0, 0, 0, SCREEN_WIDTH};
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
    if (bounce->xPos < bounce->min)
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
    if (bounce->yPos < bounce->min)
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
  // issue for claude: still hangs the device
  if (method == 3)
  {
    Img *img = &img_sdk_tile_bg_brown_raw;
    bgScrollX += 1;
    bgScrollY += 1;
    vmupro_blit_infinite_scrolling_background(img->data, img->width, img->height, bgScrollX, bgScrollY, SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}

uint8_t mask_55x55_a[55 * 55];
uint8_t mask_55x55_b[55 * 55];

void InitTestFunctions()
{

  for (int i = 0; i < 55 * 55; i++)
  {
    if (i % 4 > 1)
    {
      mask_55x55_a[i] = 0;
    }
    else
    {
      mask_55x55_a[i] = 1;
    }

    if (i % 9 > 4)
    {
      mask_55x55_b[i] = 0;
    }
    else
    {
      mask_55x55_b[i] = 1;
    }
  }
}

void DrawTestFunctions(int testNum)
{

  UpdateDVDBounce(&bounce1);
  UpdateDVDBounce(&bounce2);
  UpdateDVDBounce(&bounce3);
  UpdateDVDBounce(&bounce4);

  // simple image

  // # 0, vmupro_blit_buffer_at(), normal buffer blit
  if (testNum == 0)
  {
    Img *img = &img_vmu_circle_raw;
    uint8_t *data = img_vmu_circle_raw.data;
    vmupro_blit_buffer_at(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height);
    vmupro_blit_buffer_at(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height);
    static bool shownMsg0 = false;
    if (!shownMsg0)
    {
      shownMsg0 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_at", testNum);
    }
  }

  // #1, vmupro_blit_buffer_rotated_90(), normal blit with rotation
  if (testNum == 1)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_rotated_90(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, (bounce3.xPos / 16) % 4);
    vmupro_blit_buffer_rotated_90(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, (bounce3.yPos / 16) % 4);
    static bool shownMsg1 = false;
    if (!shownMsg1)
    {
      shownMsg1 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_rotated_90", testNum);
    }
  }

  // #2, vmupro_blit_buffer_blended(), blit with alpha blending
  // issues for claude:
  //  - SIMD part displays utter garbage
  //  - Scalar part works but the byte order appears wrong
  //  - wraps when x or y are below 0
  if (testNum == 2)
  {
    Img *img = &img_vmu_circle_raw;
    int alpha = bounce3.xPos;
    vmupro_blit_buffer_blended(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, alpha);
    vmupro_blit_buffer_blended(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, 20);
    static bool shownMsg2 = false;
    if (!shownMsg2)
    {
      shownMsg2 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_blended", testNum);
    }
  }

  // #3, vmupro_blit_buffer_color_add(), blit w/ constant colour
  // issues for claude:
  //  - immediately crashes the device
  if (testNum == 3)
  {
    Img *img = &img_vmu_circle_raw;
    uint16_t rgb565 = VMUPRO_COLOR_BLUE;
    vmupro_blit_buffer_color_add(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, rgb565);
    static bool shownMsg3 = false;
    if (!shownMsg3)
    {
      shownMsg3 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_color_add", testNum);
    }
  }

  // #4, vmupro_blit_buffer_color_multiply(), blit w/ colour multiply
  // issues for claude:
  //  - only displays a few flickering pixels from a single row
  //  - eventually crashes
  if (testNum == 4)
  {

    Img *img = &img_vmu_circle_raw;
    uint16_t rgb565 = VMUPRO_COLOR_GREEN;
    vmupro_blit_buffer_color_multiply(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, rgb565);
    static bool shownMsg4 = false;
    if (!shownMsg4)
    {
      shownMsg4 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_color_multiply", testNum);
    }
  }

  // #5, vmupro_blit_buffer_flip_h() & vmupro_blit_buffer_flip_v(), blit flipped
  // issues for claude:
  //  - crashes and reboots the device (caught on vid)
  if (testNum == 5)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_flip_h(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height);
    vmupro_blit_buffer_flip_v(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height);
    static bool shownMsg5 = false;
    if (!shownMsg5)
    {
      shownMsg5 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_flip_h & h", testNum);
    }
  }

  // #6, vmupro_blit_buffer_fixed_alpha(), blit with fixed alpha value
  // issues for claude:
  //  - displays garbage data like colour multiply
  //  - the edges are vaguely correct looking, maybe slightly miscoloured
  //  - wraps around the screen edges, unlike other funcions
  if (testNum == 6)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_fixed_alpha(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, 0);
    vmupro_blit_buffer_fixed_alpha(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, 1);
    vmupro_blit_buffer_fixed_alpha(img->data, bounce3.xPos, bounce3.yPos, img->width, img->height, 2);

    static bool shownMsg6 = false;
    if (!shownMsg6)
    {
      shownMsg6 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_fixed_alpha", testNum);
    }
  }

  // #7, vmupro_blit_buffer_masked(), blit with a mask buffer
  if (testNum == 7)
  {
    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_masked(img->data, mask_55x55_a, bounce1.xPos, bounce1.yPos, img->width, img->height);
    vmupro_blit_buffer_masked(img->data, mask_55x55_b, bounce2.xPos, bounce2.yPos, img->width, img->height);
    static bool shownMsg7 = false;
    if (!shownMsg7)
    {
      shownMsg7 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_masked", testNum);
    }
  }

  // # 8, vmupro_blit_buffer_mosaic(), pixellate stuff
  // issues for claude:
  //  - mosaic values of 0 render nothing
  if (testNum == 8)
  {

    Img *img = &img_vmu_circle_raw;
    vmupro_blit_buffer_mosaic(img->data, bounce1.xPos, bounce1.yPos, img->width, img->height, 1);
    vmupro_blit_buffer_mosaic(img->data, bounce2.xPos, bounce2.yPos, img->width, img->height, 2);
    vmupro_blit_buffer_mosaic(img->data, bounce3.xPos, bounce3.yPos, img->width, img->height, 3);

    static bool shownMsg8 = false;
    if (!shownMsg8)
    {
      shownMsg8 = true;
      vmupro_log(VMUPRO_LOG_INFO, TAG, "Function %d - vmupro_blit_buffer_mosaic", testNum);
    }
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
  InitTestFunctions();

  // Wait a bit to actually show the changes

  while (true)
  {

    vmupro_color_t col = VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    DrawBackground();
    DrawGround();
    DrawForeground();
    DrawTestFunctions(testNum);

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
