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
 *   Sega Master System console emulation
 *
 ******************************************************************************/

#ifndef _SMS_H_
#define _SMS_H_

#include "stdint.h"

#define CYCLES_PER_LINE 228

enum { SLOT_BIOS = 0, SLOT_CARD = 1, SLOT_CART = 2, SLOT_EXP = 3 };

enum { MAPPER_NONE = 0, MAPPER_SEGA = 1, MAPPER_CODIES = 2, MAPPER_KOREA = 3, MAPPER_KOREA_MSX = 4 };

enum { DISPLAY_NTSC = 0, DISPLAY_PAL = 1 };

enum { FPS_NTSC = 60, FPS_PAL = 50 };

enum { CLOCK_NTSC = 3579545, CLOCK_PAL = 3546895, CLOCK_NTSC_SMS1 = 3579527 };

enum {
  CONSOLE_COLECO = 0x10,
  CONSOLE_SG1000 = 0x11,
  CONSOLE_SC3000 = 0x12,
  CONSOLE_SF7000 = 0x13,

  CONSOLE_SMS  = 0x20,
  CONSOLE_SMS2 = 0x21,

  CONSOLE_GG   = 0x40,
  CONSOLE_GGMS = 0x41,

  CONSOLE_MD     = 0x80,
  CONSOLE_MDPBC  = 0x81,
  CONSOLE_GEN    = 0x82,
  CONSOLE_GENPBC = 0x83
};

#define HWTYPE_TMS CONSOLE_COLECO
#define HWTYPE_SMS CONSOLE_SMS
#define HWTYPE_GG CONSOLE_GG
#define HWTYPE_MD CONSOLE_MD

#define IS_TMS (sms.console & HWTYPE_TMS)
#define IS_SMS (sms.console & HWTYPE_SMS)
#define IS_GG (sms.console & HWTYPE_GG)
#define IS_MD (sms.console & HWTYPE_MD)

enum { TERRITORY_DOMESTIC = 0, TERRITORY_EXPORT = 1 };

/* SMS context */
typedef struct {
  uint8_t pdr;    /* Parallel data register */
  uint8_t ddr;    /* Data direction register */
  uint8_t txdata; /* Transmit data buffer */
  uint8_t rxdata; /* Receive data buffer */

  uint8_t sctrl; /* Serial mode control and status */
  uint8_t _pad00;
  uint8_t _pad01;
  uint8_t _pad02;
} __attribute__((packed, aligned(1))) sio_t;

typedef struct {
  // uint8_t wram[0x2000];
  uint8_t *wram;  // [0x2000];

  uint8_t paused;
  uint8_t save;
  uint8_t territory;
  uint8_t console;

  uint8_t display;
  uint8_t fm_detect;
  uint8_t glasses_3d;
  uint8_t hlatch;

  uint8_t use_fm;
  uint8_t memctrl;
  uint8_t ioctrl;
  uint8_t _pad00;

  sio_t sio;

  uint8_t device[2];
  uint8_t gun_offset;
  uint8_t _pad01;
} __attribute__((packed, aligned(1))) sms_t;

/* BIOS ROM */
typedef struct {
  uint8_t *rom;
  uint8_t enabled;
  uint8_t pages;
  uint8_t fcr[4];
} __attribute__((packed, aligned(1))) bios_t;

typedef struct {
  uint8_t *rom;
  uint8_t pages;
  uint8_t *fcr;
  uint8_t mapper;
} __attribute__((packed, aligned(1))) slot_t;

typedef struct {
  uint8_t *rom;      //[0x2000];  /* BIOS ROM */
  uint8_t pio_mode;  /* PIO mode */
  uint8_t keypad[2]; /* Keypad inputs */
} __attribute__((packed, aligned(1))) t_coleco;

/* Global data */
extern sms_t sms;
extern bios_t bios;
extern slot_t cartslot;
extern t_coleco coleco;
extern uint8_t *dummy_write;
extern uint8_t *dummy_read;

/* Function prototypes */
extern void sms_init(void);
extern void sms_reset(void);
extern void sms_shutdown(void);
extern void mapper_reset(void);
extern void mapper_8k_w(int address, int data);
extern void mapper_16k_w(int address, int data);
extern int sms_irq_callback(int param);

#endif /* _SMS_H_ */
