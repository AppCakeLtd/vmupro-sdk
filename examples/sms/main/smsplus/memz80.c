#pragma GCC optimize("O3")
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
 *   Z80 memory handlers
 *
 ******************************************************************************/

#include "shared.h"

/* Pull-up resistors on data bus */
uint8_t data_bus_pullup   = 0x00;
uint8_t data_bus_pulldown = 0x00;

/* Read unmapped memory */
uint8_t z80_read_unmapped(void) {
  int pc = Z80.pc.w.l;
  uint8_t data;
  pc   = (pc - 1) & 0xFFFF;
  data = cpu_readmap[pc >> 13][pc & 0x03FF];
  return ((data | data_bus_pullup) & ~data_bus_pulldown);
}

/* Port $3E (Memory Control Port) */
void memctrl_w(uint8_t data) {
  /* detect CARTRIDGE/BIOS enabled/disabled */
  if (IS_SMS) {
    /* autodetect loaded BIOS ROM */
    if (!(bios.enabled & 2) && ((data & 0xE8) == 0xE8)) {
      bios.enabled = 0;  // option.use_bios | 2;
      memcpy(bios.rom, cart.rom, cart.size);
      memcpy(bios.fcr, cart.fcr, 4);
      bios.pages  = cart.pages;
      cart.loaded = 0;
    }

    /* disables CART & BIOS by default */
    cartslot.rom    = NULL;
    cartslot.mapper = MAPPER_NONE;

    switch (data & 0x48) {
      case 0x00: /* BIOS & CART enabled */
      case 0x08: /* BIOS disabled, CART enabled */
        if (cart.loaded) {
          cartslot.rom    = cart.rom;
          cartslot.pages  = cart.pages;
          cartslot.mapper = cart.mapper;
          cartslot.fcr    = &cart.fcr[0];
        }
        break;

      case 0x40: /* BIOS enabled, CART disabled */
        cartslot.rom    = bios.rom;
        cartslot.pages  = bios.pages;
        cartslot.mapper = MAPPER_SEGA;
        cartslot.fcr    = &bios.fcr[0];
        break;

      default:
        break;
    }

    mapper_reset();

    /* reset cartslot mapping */
    if (cartslot.rom) {
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
    else {
      uint8_t i;
      for (i = 0x00; i <= 0x2F; i++) {
        cpu_readmap[i]  = dummy_read;
        cpu_writemap[i] = dummy_write;
      }
    }
  }

  /* update register value */
  sms.memctrl = data;
}

/*--------------------------------------------------------------------------*/
/* Sega Master System port handlers                                         */
/*--------------------------------------------------------------------------*/
void sms_port_w(uint16 port, uint8_t data) {
  port &= 0xFF;

  /* access FM unit */
  if (port >= 0xF0) {
    switch (port) {
      case 0xF0:
        fmunit_write(0, data);
        return;

      case 0xF1:
        fmunit_write(1, data);
        return;

      case 0xF2:
        fmunit_detect_w(data);
        return;
    }
  }

  switch (port & 0xC1) {
    case 0x00:
      memctrl_w(data);
      return;

    case 0x01:
      pio_ctrl_w(data);
      return;

    case 0x40:
    case 0x41:
      psg_write(data);
      return;

    case 0x80:
    case 0x81:
      vdp_write(port, data);
      return;

    case 0xC0:
    case 0xC1:
      return;
  }
}

uint8_t sms_port_r(uint16 port) {
  port &= 0xFF;

  /* FM unit */
  if (port == 0xF2) return fmunit_detect_r() & pio_port_r(port);

  switch (port & 0xC0) {
    case 0x00:
      return z80_read_unmapped();

    case 0x40:
      return vdp_counter_r(port);

    case 0x80:
      return vdp_read(port);

    case 0xC0:
      return pio_port_r(port);
  }

  /* Just to please the compiler */
  return -1;
}

/*--------------------------------------------------------------------------*/
/* Game Gear port handlers                                                  */
/*--------------------------------------------------------------------------*/

void gg_port_w(uint16 port, uint8_t data) {
  port &= 0xFF;

  if (port <= 0x20) {
    sio_w(port, data);
    return;
  }

  switch (port & 0xC1) {
    case 0x00:
      memctrl_w(data);
      return;

    case 0x01:
      pio_ctrl_w(data);
      return;

    case 0x40:
    case 0x41:
      psg_write(data);
      return;

    case 0x80:
    case 0x81:
      gg_vdp_write(port, data);
      return;
  }
}

uint8_t gg_port_r(uint16 port) {
  port &= 0xFF;

  if (port <= 0x20) return sio_r(port);

  switch (port & 0xC0) {
    case 0x00:
      return z80_read_unmapped();

    case 0x40:
      return vdp_counter_r(port);

    case 0x80:
      return vdp_read(port);

    case 0xC0:
      switch (port) {
        case 0xC0:
        case 0xC1:
        case 0xDC:
        case 0xDD:
          return pio_port_r(port);
      }
      return z80_read_unmapped();
  }

  /* Just to please the compiler */
  return -1;
}

/*--------------------------------------------------------------------------*/
/* Game Gear (MS) port handlers                                             */
/*--------------------------------------------------------------------------*/

void ggms_port_w(uint16 port, uint8_t data) {
  port &= 0xFF;

  if (port <= 0x20) {
    sio_w(port, data);
    return;
  }

  switch (port & 0xC1) {
    case 0x00:
      memctrl_w(data);
      return;

    case 0x01:
      pio_ctrl_w(data);
      return;

    case 0x40:
    case 0x41:
      psg_write(data);
      return;

    case 0x80:
    case 0x81:
      vdp_write(port, data); /* fixed */
      return;
  }
}

uint8_t ggms_port_r(uint16 port) {
  port &= 0xFF;

  if (port <= 0x20) return sio_r(port);

  switch (port & 0xC0) {
    case 0x00:
      return z80_read_unmapped();

    case 0x40:
      return vdp_counter_r(port);

    case 0x80:
      return vdp_read(port);

    case 0xC0:
      switch (port) {
        case 0xC0:
        case 0xC1:
        case 0xDC:
        case 0xDD:
          return pio_port_r(port);
      }
      return z80_read_unmapped();
  }

  /* Just to please the compiler */
  return -1;
}

/*--------------------------------------------------------------------------*/
/* MegaDrive / Genesis port handlers                                        */
/*--------------------------------------------------------------------------*/

void md_port_w(uint16 port, uint8_t data) {
  switch (port & 0xC1) {
    case 0x00:
      /* No memory control register */
      return;

    case 0x01:
      pio_ctrl_w(data);
      return;

    case 0x40:
    case 0x41:
      psg_write(data);
      return;

    case 0x80:
    case 0x81:
      md_vdp_write(port, data);
      return;
  }
}

uint8_t md_port_r(uint16 port) {
  switch (port & 0xC0) {
    case 0x00:
      return z80_read_unmapped();

    case 0x40:
      return vdp_counter_r(port);

    case 0x80:
      return vdp_read(port);

    case 0xC0:
      switch (port) {
        case 0xC0:
        case 0xC1:
        case 0xDC:
        case 0xDD:
          return pio_port_r(port);
      }
      return z80_read_unmapped();
  }

  /* Just to please the compiler */
  return -1;
}

/*--------------------------------------------------------------------------*/
/* SG1000,SC3000,SF7000 port handlers                                       */
/*--------------------------------------------------------------------------*/

void tms_port_w(uint16 port, uint8_t data) {
  switch (port & 0xC0) {
    case 0x40:
      psg_write(data);
      return;

    case 0x80:
      tms_write(port, data);
      return;

    default:
      return;
  }
}

uint8_t tms_port_r(uint16 port) {
  switch (port & 0xC0) {
    case 0x80:
      return vdp_read(port);

    case 0xC0:
      return pio_port_r(port);

    default:
      return 0xff;
  }
}

/*--------------------------------------------------------------------------*/
/* Colecovision port handlers                                               */
/*--------------------------------------------------------------------------*/
void coleco_port_w(uint16 port, uint8_t data) {
  /* A7 is used as enable input */
  /* A6 & A5 are used to decode the address */
  switch (port & 0xE0) {
    case 0x80:
      coleco.pio_mode = 0;
      return;

    case 0xa0:
      tms_write(port, data);
      return;

    case 0xc0:
      coleco.pio_mode = 1;
      return;

    case 0xe0:
      psg_write(data);
      return;

    default:
      return;
  }
}

uint8_t coleco_port_r(uint16 port) {
  /* A7 is used as enable input */
  /* A6 & A5 are used to decode the address */
  switch (port & 0xE0) {
    case 0xa0:
      return vdp_read(port);

    case 0xe0:
      return coleco_pio_r((port >> 1) & 1);

    default:
      return 0xff;
  }
}
