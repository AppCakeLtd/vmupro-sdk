/******************************************************************************
 *  Sega Master System / GameGear Emulator
 *  Copyright (C) 1998-2007  Charles MacDonald
 *
 *  additionnal code by Eke-Eke (SMS Plus GX)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   Sega Master System manager
 *
 ******************************************************************************/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define APP_NAME "SMS Plus"
#define APP_VERSION "1.3"

#define PALETTE_SIZE 0x20

/* Mask for removing unused pixel data */
#define PIXEL_MASK 0x1F

/* These can be used for 'input.pad[]' */
#define INPUT_UP 0x00000001
#define INPUT_DOWN 0x00000002
#define INPUT_LEFT 0x00000004
#define INPUT_RIGHT 0x00000008
#define INPUT_BUTTON1 0x00000010
#define INPUT_BUTTON2 0x00000020

/* These can be used for 'input.system' */
#define INPUT_START 0x00000001 /* Game Gear only */
#define INPUT_PAUSE 0x00000002 /* Master System only */
#define INPUT_RESET 0x00000004 /* Master System only */

enum { SRAM_SAVE = 0, SRAM_LOAD = 1 };

/* User input structure */
typedef struct {
  uint8_t pad[2];
  int analog[2][2];
  uint8_t system;
} input_t;

/* Game image structure */
typedef struct {
  uint8_t *rom;
  uint8_t loaded;
  uint32 size;
  uint8_t pages;
  uint32 crc;
  uint32 sram_crc;
  int8 mapper;
  // uint8_t sram[0x8000];
  uint8_t *sram;  // [0x8000];
  uint8_t fcr[4];
} cart_t;

/* Bitmap structure */
typedef struct {
  unsigned char *data;
  int width;
  int height;
  int pitch;
  // int depth;
  int granularity;

  struct {
    int outWidth;
    int outHeight;
    int xStart;
    int yOffset;
    uint8_t cropped;
    uint8_t fill;
    int fboffset;
  } vmpro_output;

  struct {
    int x, y, w, h;
    int ox, oy, ow, oh;
    int changed;
  } viewport;
} bitmap_t;

/* Global variables */
extern bitmap_t bitmap; /* Display bitmap */
extern cart_t cart;     /* Game cartridge data */
extern input_t input;   /* Controller input */

/* Function prototypes */
extern void system_frame(int skip_render);
extern void system_init2(void);
extern void system_shutdown(void);
extern void system_reset(void);
extern void system_manage_sram(uint8_t *sram, int cartslot, int mode);
extern void system_poweron(void);
extern void system_poweroff(void);

#endif /* _SYSTEM_H_ */
