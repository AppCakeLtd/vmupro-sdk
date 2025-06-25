#include "vmupro_sdk.h"
#include "placeholder.h"
#include "images/icons.h"
#include "sdk_tile_bg_brown.h"
#include "sdk_tile_bg_blue.h"

const char *TAG = "[GFX Samples]";


// Temporary solution
void DrawGround(){

  // should be static, but just for demo
  Img * tileList [16][16] = {};


  // these would be encoded somewhere instead of in-line like this
  // usually by something like asperite or a custom tool
  // little block at the bottom left
  tileList[1][12] = &img_ground_grass_tl_raw;
  tileList[2][12] = &img_ground_grass_tm_raw;
  tileList[3][12] = &img_ground_grass_tm_raw;
  tileList[4][12] = &img_ground_grass_tr_raw;
  tileList[1][13] = &img_ground_grass_bl_raw;
  tileList[2][13] = &img_ground_grass_bm_raw;
  tileList[3][13] = &img_ground_grass_bm_raw;
  tileList[4][13] = &img_ground_grass_br_raw;

  // next block
  tileList[5][11] = &img_ground_grass_tl_raw;
  tileList[6][11] = &img_ground_grass_tm_raw;
  tileList[7][11] = &img_ground_grass_tm_raw;
  tileList[8][11] = &img_ground_grass_tr_raw;
  tileList[5][12] = &img_ground_grass_ml_raw;
  tileList[6][12] = &img_ground_grass_mm_raw;
  tileList[7][12] = &img_ground_grass_mm_raw;
  tileList[8][12] = &img_ground_grass_mr_raw;
  tileList[5][13] = &img_ground_grass_bl_raw;
  tileList[6][13] = &img_ground_grass_bm_raw;
  tileList[7][13] = &img_ground_grass_bm_raw;
  tileList[8][13] = &img_ground_grass_br_raw;

  // the big middle block
  tileList[8][9] = &img_ground_grass_tl_raw;
  tileList[9][9] = &img_ground_grass_tm_raw;
  tileList[10][9] = &img_ground_grass_tm_raw;
  tileList[11][9] = &img_ground_grass_tm_raw;
  tileList[12][9] = &img_ground_grass_tr_raw;




  for( int y = 0; y < 16; y++ ){
    for( int x = 0; x < 16; x++ ){
      Img * src = tileList[x][y];
      if ( src == NULL ) continue;

      int drawX = x * 16;
      int drawY = y * 16;
      vmupro_blit_buffer_at( src->data, drawX, drawY, src->width, src->height);
    }
  }

}

void app_main(void)
{
  vmupro_log(VMUPRO_LOG_INFO, TAG, "GFX Samples 3");

  vmupro_display_clear(VMUPRO_COLOR_GREY);
  vmupro_display_refresh();

  vmupro_start_double_buffer_renderer();

  // Wait a bit to actually show the changes
  bool flipflop = false;

  bool scroll_dir_x = true;
  int scroll_pos_x = 10;
  bool scroll_dir_y = true;
  int scroll_pos_y = 24;

  int bgScrollX = 0;
  int bgScrollY = 0;
  const int BG_TILE_SIZE = 65;
  const int SCREEN_WIDTH = 240;
  const int SCREEN_HEIGHT = 240;

  while (true)
  {

    // vmupro_color_t col = flipflop ? VMUPRO_COLOR_BLUE : VMUPRO_COLOR_RED;
    // vmupro_color_t shapeCol = flipflop ? VMUPRO_COLOR_RED : VMUPRO_COLOR_BLUE;

    vmupro_color_t col = VMUPRO_COLOR_BLUE;
    vmupro_display_clear(col);

    

    // Start by making the user feel kinda sick
    bgScrollX += 1;
    bgScrollX = bgScrollX % BG_TILE_SIZE;
    bgScrollY += 1;
    bgScrollY = bgScrollY % BG_TILE_SIZE;
    Img * img = &img_sdk_tile_bg_brown_raw;
    vmupro_blit_tile_pattern( img->data, img->width, img->height, 0 - BG_TILE_SIZE + bgScrollX, 0 - BG_TILE_SIZE + bgScrollY, SCREEN_WIDTH + BG_TILE_SIZE, SCREEN_HEIGHT + BG_TILE_SIZE);

    // vmupro_blit_buffer_at((uint8_t *)&placeholder, scroll_pos_x, scroll_pos_y, 76, 76);
    DrawGround();

    vmupro_push_double_buffer_frame();

    if (scroll_dir_x)
    {
      scroll_pos_x++;
      // let it go out of bounds so we see what happens)
      if (scroll_pos_x >= 240 - 1)
      {
        scroll_dir_x = false;
      }
    }
    else
    {
      scroll_pos_x--;
      if (scroll_pos_x < 0)
      {
        scroll_dir_x = true;
      }
    }

    if (scroll_dir_y)
    {
      scroll_pos_y++;
      // let it go out of bounds so we see what happens)
      if (scroll_pos_y >= 240 - 1)
      {
        scroll_dir_y = false;
      }
    }
    else
    {
      scroll_pos_y--;
      if (scroll_pos_y < 0)
      {
        scroll_dir_y = true;
      }
    }

    // Nice long delay so we know what should be drawn at any given time
    vmupro_sleep_ms(32);

    flipflop = !flipflop;

    vmupro_btn_read();
    if (vmupro_btn_confirm_pressed())
    {
      break;
    }
  }

  // Terminate the renderer
  vmupro_stop_double_buffer_renderer();
}
