/******************************************************************************
 *  Sega Master System / GameGear Emulator
 *  Copyright (C) 1998-2007  Charles MacDonald
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
 *   ROM File Loading support
 *
 ******************************************************************************/

#include "shared.h"
#include "esp_heap_caps.h"

extern unsigned long crc32(int crc, uint8_t *buf, int len);

#define GAME_DATABASE_CNT 93

typedef struct {
  uint32 crc;
  uint8_t glasses_3d;
  uint8_t device;
  uint8_t mapper;
  uint8_t display;
  uint8_t territory;
  uint8_t console;
  const char *name;
} rominfo_t;

const rominfo_t game_list[GAME_DATABASE_CNT] = {
    /* games requiring CODEMASTER mapper */
    {0x29822980, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Cosmic Spacehead"},
    {0x6CAA625B, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GG, "Cosmic Spacehead (GG)"},
    {0xEA5C3A6F,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Dinobasher - Starring Bignose the Caveman [Proto]"},
    {0x152F0DCC, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Drop Zone"},
    {0x5E53C7F7, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GG, "Ernie Els Golf"},
    {0x8813514B,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Excellent Dizzy Collection, The [Proto]"},
    {0xAA140C9C,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Excellent Dizzy Collection, The [SMS-GG]"},
    {0xB9664AE1, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Fantastic Dizzy"},
    {0xC888222B,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Fantastic Dizzy [SMS-GG]"},
    {0x76C5BDFB, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Jang Pung 2 [SMS-GG]"},
    {0xD9A7F170, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Man Overboard!"},
    {0xA577CE46, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Micro Machines"},
    {0xF7C524F6,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Micro Machines [BAD DUMP]"},
    {0xDBE8895C,
     0,
     DEVICE_PAD2B,
     MAPPER_CODIES,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Micro Machines 2 - Turbo Tournament"},
    {0xC1756BEE, 0, DEVICE_PAD2B, MAPPER_CODIES, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Pete Sampras Tennis"},

    /* games requiring KOREA mappers */
    {0x17AB6883, 0, DEVICE_PAD2B, MAPPER_NONE, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "FA Tetris (KR)"},
    {0x61E8806F, 0, DEVICE_PAD2B, MAPPER_NONE, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Flash Point (KR)"},
    {0x89B79E77, 0, DEVICE_PAD2B, MAPPER_KOREA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Dodgeball King (KR)"},
    {0x18FB98A3, 0, DEVICE_PAD2B, MAPPER_KOREA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Jang Pung 3 (KR)"},
    {0x97D03541, 0, DEVICE_PAD2B, MAPPER_KOREA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Sangokushi 3 (KR)"},
    {0x67C2F0FF, 0, DEVICE_PAD2B, MAPPER_KOREA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Super Boy 2 (KR)"},
    {0x445525E2,
     0,
     DEVICE_PAD2B,
     MAPPER_KOREA_MSX,
     DISPLAY_NTSC,
     TERRITORY_DOMESTIC,
     CONSOLE_SMS2,
     "Penguin Adventure (KR)"},
    {0x83F0EEDE,
     0,
     DEVICE_PAD2B,
     MAPPER_KOREA_MSX,
     DISPLAY_NTSC,
     TERRITORY_DOMESTIC,
     CONSOLE_SMS2,
     "Street Master (KR)"},
    {0xA05258F5, 0, DEVICE_PAD2B, MAPPER_KOREA_MSX, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS2, "Won-Si-In (KR)"},
    {0x06965ED9,
     0,
     DEVICE_PAD2B,
     MAPPER_KOREA_MSX,
     DISPLAY_NTSC,
     TERRITORY_DOMESTIC,
     CONSOLE_SMS2,
     "F-1 Spirit - The way to Formula-1 (KR)"},

    /* games that require PAL timings (from MEKA.nam by Omar Cornut) */
    {0x72420F38, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Addams Familly"},
    {0x2D48C1D3,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Back to the Future Part III"},
    {0x1CBB7BF1, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Battlemaniacs (BR)"},
    {0x1B10A951, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Bram Stoker's Dracula"},
    {0xC0E25D62, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "California Games II"},
    {0xC9DBF936, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Home Alone"},
    {0x0047B615, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Predator2"},
    {0xF42E145C,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Quest for the Shaven Yak Starring Ren Hoek & Stimpy (BR)"},
    {0x9F951756, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "RoboCop 3"},
    {0x1575581D, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Shadow of the Beast"},
    {0x96B3F29E, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Sonic Blast (BR)"},
    {0x5B3B922C,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Sonic the Hedgehog 2 [V0]"},
    {0xD6F2BFCA,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Sonic the Hedgehog 2 [V1]"},
    {0xCA1D3752, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Space Harrier [50 Hz]"},
    {0x85CFC9C9, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_PAL, TERRITORY_EXPORT, CONSOLE_SMS2, "Taito Chase H.Q."},

    /* games requiring 315-5124 VDP (Mark-III, Sega Master System) */
    {0x32759751, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Y's (J)"},

    /* games requiring Game Gear SMS compatibility mode */
    {0x59840FD6,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Castle of Illusion - Starring Mickey Mouse"},
    {0x9942B69B,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_DOMESTIC,
     CONSOLE_GGMS,
     "Castle of Illusion - Starring Mickey Mouse (J)"},
    {0x5877B10D,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_DOMESTIC,
     CONSOLE_GGMS,
     "Castle of Illusion - Starring Mickey Mouse (J) [HACK]"},
    {0x9C76FB3A, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Rastan Saga [SMS-GG]"},
    {0x7BB81E3D,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Taito Chase H.Q. [SMS-GG]"},
    {0x44FBE8F6,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Taito Chase H.Q. [SMS-GG][HACK]"},
    {0x18086B70,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Taito Chase H.Q. [SMS-GG][HACK][BAD]"},
    {0xDA8E95A9,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "WWF Wrestlemania Steel Cage Challenge [SMS-GG]"},
    {0xCB42BD33,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "WWF Wrestlemania Steel Cage Challenge [SMS-GG] [BAD DUMP]"},
    {0x1D93246E,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Olympic Gold [SMS-GG] [A]"},
    {0xA2f9C7AF,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Olympic Gold [SMS-GG] [B]"},
    {0xF037EC00, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Out Run Europa [SMS-GG]"},
    {0x2AA12D7E, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Out Run Europa [SMS-GG]"},
    {0xE5F789B9, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Predator 2 [SMS-GG]"},
    {0x311D2863,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Prince of Persia [SMS-GG] [A]"},
    {0x45F058d6,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Prince of Persia [SMS-GG] [b1]"},
    {0xBA6344FC,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "Prince of Persia [SMS-GG] [B]"},
    {0x56201996,
     0,
     DEVICE_PAD2B,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_GGMS,
     "R.C. Grand Prix [SMS-GG]"},
    {0x10DBBEF4, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_GGMS, "Super Kick Off [SMS-GG]"},
    {0xBD1CC7DF, 0, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_GGMS, "Super Tetris (KR)"},

    /* games requiring 3D Glasses */
    {0xFBF96C81, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Blade Eagle 3-D (BR)"},
    {0x8ECD201C, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Blade Eagle 3-D"},
    {0x31B8040B, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Maze Hunter 3-D"},
    {0x871562b0, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Maze Walker"},
    {0xABD48AD2, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Poseidon Wars 3-D"},
    {0x6BD5C2BF, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Space Harrier 3-D"},
    {0x156948f9, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Space Harrier 3-D (J)"},
    {0xA3EF13CB, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Zaxxon 3-D"},
    {0xBBA74147, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Zaxxon 3-D [Proto]"},
    {0xD6F43DDA, 1, DEVICE_PAD2B, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Out Run 3-D"},

    /* games requiring Light Phaser & 3D Glasses */
    {0xFBE5CFBB, 1, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Missile Defense 3D"},
    {0xE79BB689,
     1,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Missile Defense 3D [BIOS]"},

    /* games requiring Light Phaser */
    {0x861B6E79,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Assault City [Light Phaser]"},
    {0x5FC74D2A, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Gangster Town"},
    {0xE167A561,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Hang-On / Safari Hunt"},
    {0xC5083000,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Hang-On / Safari Hunt [BAD DUMP]"},
    {0x91E93385,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Hang-On / Safari Hunt [BIOS]"},
    {0xE8EA842C,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Marksman Shooting / Trap Shooting"},
    {0xE8215C2E,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Marksman Shooting / Trap Shooting / Safari Hunt"},
    {0x205CAAE8,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_PAL,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Operation Wolf"}, /* can be also played using the PLAYER2 gamepad */
    {0x23283F37,
     0,
     DEVICE_LIGHTGUN,
     MAPPER_SEGA,
     DISPLAY_NTSC,
     TERRITORY_EXPORT,
     CONSOLE_SMS2,
     "Operation Wolf [A]"}, /* can be also played using the PLAYER2 gamepad */
    {0xDA5A7013, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Rambo 3"},
    {0x79AC8E7F, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Rescue Mission"},
    {0x4B051022, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Shooting Gallery"},
    {0xA908CFF5, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Spacegun"},
    {0x5359762D, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Wanted"},
    {0x0CA95637, 0, DEVICE_LIGHTGUN, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Laser Ghost"},

    /* games requiring Paddle */
    {0xF9DBB533, 0, DEVICE_PADDLE, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Alex Kidd BMX Trial"},
    {0xA6FA42D0, 0, DEVICE_PADDLE, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Galactic Protector"},
    {0x29BC7FAD, 0, DEVICE_PADDLE, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Megumi Rescue"},
    {0x315917D4, 0, DEVICE_PADDLE, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_DOMESTIC, CONSOLE_SMS, "Woody Pop"},

    /* games requiring Sport Pad (NOT EMULATED YET) */
    {0x946B8C4A, 0, DEVICE_SPORTSPAD, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Great Ice Hockey"},
    {0xE42E4998, 0, DEVICE_SPORTSPAD, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Sports Pad Football"},
    {0x41C948BF, 0, DEVICE_SPORTSPAD, MAPPER_SEGA, DISPLAY_NTSC, TERRITORY_EXPORT, CONSOLE_SMS2, "Sports Pad Soccer"}

};

void set_config() {
  int i;

  /* default sms settings */
  cart.mapper    = MAPPER_SEGA;
  sms.console    = CONSOLE_SMS2;
  sms.territory  = TERRITORY_EXPORT;
  sms.display    = DISPLAY_NTSC;
  sms.glasses_3d = 0;
  sms.device[0]  = DEVICE_PAD2B;
  sms.device[1]  = DEVICE_PAD2B;
  sms.use_fm     = option.fm;

  /* console type detection */
  /* SMS Header is located at 0x7ff0 */
  if ((cart.size > 0x7000) && (!memcmp(&cart.rom[0x7ff0], "TMR SEGA", 8))) {
    printf("cart header\n");
    uint8_t region = (cart.rom[0x7fff] & 0xf0) >> 4;

    switch (region) {
      case 5:
        printf("GG domestic detected.\n");
        sms.console   = CONSOLE_GG;
        sms.territory = TERRITORY_DOMESTIC;
        break;

      case 6:
      case 7:
        printf("GG Export detected.\n");
        sms.console   = CONSOLE_GG;
        sms.territory = TERRITORY_EXPORT;
        break;

      case 3:
        printf("SMS domestic detected.\n");
        sms.console   = CONSOLE_SMS;
        sms.territory = TERRITORY_DOMESTIC;
        break;

      default:
        printf("SMS2 export detected.\n");
        sms.console   = CONSOLE_SMS2;
        sms.territory = TERRITORY_EXPORT;
        break;
    }
  }

  sms.gun_offset = 20; /* default offset */

  /* retrieve game settings from database */
  for (i = 0; i < GAME_DATABASE_CNT; i++) {
    if (cart.crc == game_list[i].crc) {
      cart.mapper    = game_list[i].mapper;
      sms.display    = game_list[i].display;
      sms.territory  = game_list[i].territory;
      sms.glasses_3d = game_list[i].glasses_3d;
      sms.console    = game_list[i].console;
      sms.device[0]  = game_list[i].device;
      if (game_list[i].device != DEVICE_LIGHTGUN) sms.device[1] = game_list[i].device;

      if ((strcmp(game_list[i].name, "Spacegun") == 0) || (strcmp(game_list[i].name, "Gangster Town") == 0)) {
        /* these games seem to use different gun position calculation method */
        sms.gun_offset = 16;
      }
      i = GAME_DATABASE_CNT;
    }
  }

  /* enable BIOS on SMS only */
  bios.enabled &= 2;
  if (IS_SMS) bios.enabled |= option.use_bios;

#if 1
  /* force settings if AUTO is not set*/
  if (option.console == 1)
    sms.console = CONSOLE_SMS;
  else if (option.console == 2)
    sms.console = CONSOLE_SMS2;
  else if (option.console == 3)
    sms.console = CONSOLE_GG;
  else if (option.console == 4)
    sms.console = CONSOLE_GGMS;
  else if (option.console == 5) {
    sms.console = CONSOLE_SG1000;
    cart.mapper = MAPPER_NONE;
  }
  else if (option.console == 6) {
    sms.console = CONSOLE_COLECO;
    cart.mapper = MAPPER_NONE;
  }

  if (option.country == 1) /* USA */
  {
    sms.display   = DISPLAY_NTSC;
    sms.territory = TERRITORY_EXPORT;
  }
  else if (option.country == 2) /* EUROPE */
  {
    sms.display   = DISPLAY_PAL;
    sms.territory = TERRITORY_EXPORT;
  }
  else if (option.country == 3) /* JAPAN */
  {
    sms.display   = DISPLAY_NTSC;
    sms.territory = TERRITORY_DOMESTIC;
  }
#endif
  printf(
      "Cart console: 0x%02X, territory: %d, display: %d, CRC: 0x%08lX\n",
      sms.console,
      sms.territory,
      sms.display,
      cart.crc
  );
}

int load_rom(char *filename) {
  FILE *fd = NULL;

  size_t nameLength = strlen(filename);
  if (nameLength < 4) {
    printf("%s: file name too short. filename='%s', nameLength=%d\n", __func__, filename, nameLength);
    abort();
  }

  if (strcmp(filename + (nameLength - 4), ".col") == 0) {
    fd = fopen("/sd/roms/col/BIOS.col", "rb");
    if (!fd) abort();

    coleco.rom = (uint8_t *)malloc(0x2000);

    fread(coleco.rom, 0x2000, 1, fd);

    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("memw");

    fclose(fd);

    option.console = 6;
    printf("Colecovision BIOS loaded.\n");
  }


  fd = fopen(filename, "rb");
  if (!fd) abort();

  /* Seek to end of file, and get size */

  fseek(fd, 0, SEEK_END);
  size_t actual_size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  cart.size = actual_size;
  if (cart.size < 0x4000) cart.size = 0x4000;

  cart.rom   = (uint8_t *)malloc(cart.size);
  size_t cnt = fread(cart.rom, cart.size, 1, fd);
  // if (cnt != 1) abort();
  __asm__("nop");
  __asm__("nop");
  __asm__("nop");
  __asm__("nop");
  __asm__("memw");

  fclose(fd);


  // cart.sram = ESP32_PSRAM + 0x280000;


  /* Take care of image header, if present */
  if ((cart.size / 512) & 1) {
    cart.size -= 512;
    memcpy(cart.rom, cart.rom + 512, cart.size);
  }

  /* 16k pages */
  cart.pages = cart.size / 0x4000;

  cart.crc    = crc32(0, cart.rom, option.console == 6 ? actual_size : cart.size);
  cart.loaded = 1;

  set_config();

  printf("%s: OK. cart.crc=%#010lx\n", __func__, cart.crc);

  return 1;
}

int load_rom_data(uint8_t *romdata, size_t romsize) {
  cart.size = romsize;
  if (cart.size < 0x4000) cart.size = 0x4000;
  cart.rom = romdata;
  // TODO: handle coleco (.col) and the bios

  /* /\* Take care of image header, if present *\/ */
  /* if ((cart.size / 512) & 1) */
  /* { */
  /*   cart.size -= 512; */
  /*   memcpy (cart.rom, cart.rom + 512, cart.size); */
  /* } */

  /* 16k pages */
  cart.pages = cart.size / 0x4000;

  cart.crc    = crc32(0, cart.rom, option.console == 6 ? romsize : cart.size);
  cart.loaded = 1;

  printf("Cart loaded: size: %li, pages: %d, crc: %li\n", cart.size, cart.pages, cart.crc);

  set_config();

  return 1;
}
