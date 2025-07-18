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
 *   Nintendo Gamecube State Management
 *
 ******************************************************************************/

#include "shared.h"

// static unsigned char* state = (unsigned char*)ESP32_PSRAM + 0x100000; //[0x10000];
// static unsigned int bufferptr;

/*
system_load_state: sizeof sms=8216
system_load_state: sizeof vdp=16524
system_load_state: sizeof Z80=72
system_load_state: sizeof SN76489_Context=92

system_save_state: sizeof sms=8216
system_save_state: sizeof vdp=16524
system_save_state: sizeof Z80=72
system_save_state: sizeof SN76489_Context=92
*/

// The three malloc-ed data structures are:
// sms.wram
// vdp.vram
// cart.sram

int system_save_state(void *mem) {
  int i;

  printf("%s: sizeof sms=%d\n", __func__, sizeof(sms));
  printf("%s: sizeof vdp=%d\n", __func__, sizeof(vdp));
  printf("%s: sizeof Z80=%d\n", __func__, sizeof(Z80));
  printf("%s: sizeof SN76489_Context=%d\n", __func__, sizeof(SN76489_Context));

  /*** Save SMS Context ***/
  // fwrite(&sms, sizeof(sms), 1, mem);

  uint8_t *wram = sms.wram;
  // cannot directly write sms since it contains a pointer to malloc-ed memory,
  // so we must save the malloced data and then separately save the non-malloced
  // data
  fwrite(wram, 0x2000, 1, mem);
  // now save the rest of the sms_t struct
  sms.wram = NULL;
  fwrite(&sms, sizeof(sms), 1, mem);
  // now restore the sms_t struct
  sms.wram = wram;

  /*** Save VDP state ***/
  // Same for vdp, we must save the malloced data and then separately save the
  // non-malloced data
  // fwrite(&vdp, sizeof(vdp), 1, mem);
  uint8_t *vram = vdp.vram;
  fwrite(vram, 0x4000, 1, mem);
  vdp.vram = NULL;
  fwrite(&vdp, sizeof(vdp), 1, mem);
  vdp.vram = vram;

  /*** Save cart info ***/
  for (i = 0; i < 4; i++) {
    fwrite(&cart.fcr[i], 1, 1, mem);
  }

  /*** Save SRAM ***/
  fwrite(cart.sram, 0x8000, 1, mem);

  /*** Save Z80 Context ***/
  fwrite(&Z80, sizeof(Z80), 1, mem);

#if 0
  /*** Save YM2413 ***/
  memcpy (&state[bufferptr], FM_GetContextPtr (), FM_GetContextSize ());
  bufferptr += FM_GetContextSize ();
#endif

  /*** Save SN76489 ***/
  fwrite(SN76489_GetContextPtr(0), SN76489_GetContextSize(), 1, mem);

  return 0;
}

void system_load_state(void *mem) {
  int i;
  uint8_t *buf;

  printf("%s: sizeof sms=%d\n", __func__, sizeof(sms));
  printf("%s: sizeof vdp=%d\n", __func__, sizeof(vdp));
  printf("%s: sizeof Z80=%d\n", __func__, sizeof(Z80));
  printf("%s: sizeof SN76489_Context=%d\n", __func__, sizeof(SN76489_Context));

  /* Initialize everything */
  system_reset();

  // first read the malloc-ed data from sms.wram
  fread(sms.wram, 0x2000, 1, mem);
  // save the pointer to the malloc-ed data
  uint8_t *wram = sms.wram;

  // Then read the rest of the sms_t struct
  /*** Set SMS Context ***/
  sms_t sms_tmp;
  fread(&sms_tmp, sizeof(sms_tmp), 1, mem);
  if (sms.console != sms_tmp.console) {
    system_reset();
    printf("%s: Bad save data\n", __func__);
    return;
  }
  sms = sms_tmp;
  // restore the pointer to the malloc-ed data
  sms.wram = wram;

  // first read the malloc-ed data from vdp.vram
  fread(vdp.vram, 0x4000, 1, mem);
  // save the pointer to the malloc-ed data
  uint8_t *vram = vdp.vram;
  /*** Set vdp state ***/
  fread(&vdp, sizeof(vdp), 1, mem);
  // restore the pointer to the malloc-ed data
  vdp.vram = vram;

  /** restore video & audio settings (needed if timing changed) ***/
  vdp_init();
  sound_init();

  /*** Set cart info ***/
  for (i = 0; i < 4; i++) {
    fread(&cart.fcr[i], 1, 1, mem);
  }

  /*** Set SRAM ***/
  fread(cart.sram, 0x8000, 1, mem);

  /*** Set Z80 Context ***/
  int (*irq_cb)(int) = Z80.irq_callback;
  fread(&Z80, sizeof(Z80), 1, mem);
  Z80.irq_callback = irq_cb;

#if 0
  /*** Set YM2413 ***/
  buf = malloc(FM_GetContextSize());
  memcpy (buf, &state[bufferptr], FM_GetContextSize ());
  FM_SetContext(buf);
  free(buf);
  bufferptr += FM_GetContextSize ();
#endif

  // Preserve clock rate
  SN76489_Context *psg = (SN76489_Context *)SN76489_GetContextPtr(0);
  float psg_Clock      = psg->Clock;
  float psg_dClock     = psg->dClock;

  /*** Set SN76489 ***/
  fread(SN76489_GetContextPtr(0), SN76489_GetContextSize(), 1, mem);

  // Restore clock rate
  psg->Clock  = psg_Clock;
  psg->dClock = psg_dClock;


  if ((sms.console != CONSOLE_COLECO) && (sms.console != CONSOLE_SG1000)) {
    /* Cartridge by default */
    cartslot.rom    = cart.rom;
    cartslot.pages  = cart.pages;
    cartslot.mapper = cart.mapper;
    cartslot.fcr    = &cart.fcr[0];

    /* Restore mapping */
    mapper_reset();
    cpu_readmap[0] = &cartslot.rom[0];
    if (cartslot.mapper != MAPPER_KOREA_MSX) {
      mapper_16k_w(0, cartslot.fcr[0]);
      mapper_16k_w(1, cartslot.fcr[1]);
      mapper_16k_w(2, cartslot.fcr[2]);
      mapper_16k_w(3, cartslot.fcr[3]);
    }
    else {
      mapper_8k_w(0, cartslot.fcr[0]);
      mapper_8k_w(1, cartslot.fcr[1]);
      mapper_8k_w(2, cartslot.fcr[2]);
      mapper_8k_w(3, cartslot.fcr[3]);
    }
  }

  // /* Force full pattern cache update */
  // bg_list_index = 0x200;
  // for(i = 0; i < 0x200; i++)
  // {
  //   bg_name_list[i] = i;
  //   bg_name_dirty[i] = -1;
  // }

  /* Restore palette */
  for (i = 0; i < PALETTE_SIZE; i++) palette_sync(i);
}
