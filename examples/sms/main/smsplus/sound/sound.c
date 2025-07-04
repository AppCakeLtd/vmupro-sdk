// #pragma GCC optimize ("O3")
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
 *   Sound emulation.
 *
 ******************************************************************************/

#include "shared.h"
#include "smsconfig.h"

sms_snd_t sms_snd;
static int16 **fm_buffer;
static int16 **psg_buffer;
int *smptab;
int smptab_len;

int sound_init(void) {
  uint8_t *fmbuf    = NULL;
  uint8_t *psgbuf   = NULL;
  int restore_sound = 0;
  int i;

  sms_snd.fm_which       = option.fm;
  sms_snd.fps            = (sms.display == DISPLAY_NTSC) ? FPS_NTSC : FPS_PAL;
  sms_snd.fm_clock       = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
  sms_snd.psg_clock      = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
  sms_snd.sample_rate    = option.sndrate;
  sms_snd.mixer_callback = NULL;

  /* Save register settings */
  if (sms_snd.enabled) {
    restore_sound = 1;

    psgbuf = malloc(SN76489_GetContextSize());
    if (!psgbuf) abort();

    memcpy(psgbuf, SN76489_GetContextPtr(0), SN76489_GetContextSize());
#if 0
    fmbuf = malloc(FM_GetContextSize());
    FM_GetContext(fmbuf);
#endif
  }

  /* If we are reinitializing, shut down sound emulation */
  if (sms_snd.enabled) {
    sound_shutdown();
  }

  /* Disable sound until initialization is complete */
  sms_snd.enabled = 0;

  /* Check if sample rate is invalid */
  if (sms_snd.sample_rate < 8000 || sms_snd.sample_rate > 48000) {
    abort();
  }

  /* Assign stream mixing callback if none provided */
  if (!sms_snd.mixer_callback) sms_snd.mixer_callback = sound_mixer_callback;

  /* Calculate number of samples generated per frame */
  sms_snd.sample_count = (sms_snd.sample_rate / sms_snd.fps) + 1;
  printf(
      "%s: sample_count=%d fps=%d (actual=%f)\n",
      __func__,
      sms_snd.sample_count,
      sms_snd.fps,
      (float)sms_snd.sample_rate / (float)sms_snd.fps
  );

  /* Calculate size of sample buffer */
  sms_snd.buffer_size = sms_snd.sample_count * 2;
  printf("%s: sms_snd.buffer_size=%d\n", __func__, sms_snd.buffer_size);

  /* Free sample buffer position table if previously allocated */
  if (smptab) {
    free(smptab);
    smptab = NULL;
  }

  /* Prepare incremental info */
  sms_snd.done_so_far = 0;
  smptab_len          = (sms.display == DISPLAY_NTSC) ? 262 : 313;
  smptab              = malloc(smptab_len * sizeof(int));
  if (!smptab) abort();

  for (i = 0; i < smptab_len; i++) {
    double calc = (sms_snd.sample_count * i);
    calc        = calc / (double)smptab_len;
    smptab[i]   = (int)calc;
  }

  /* Allocate emulated sound streams */
  for (i = 0; i < STREAM_MAX; i++) {
    sms_snd.stream[i] = malloc(sms_snd.buffer_size);
    if (!sms_snd.stream[i]) abort();
    memset(sms_snd.stream[i], 0, sms_snd.buffer_size);
  }

#ifndef NGC
  /* Allocate sound output streams */
  sms_snd.output[0] = malloc(sms_snd.buffer_size);
  sms_snd.output[1] = malloc(sms_snd.buffer_size);
  if (!sms_snd.output[0] || !sms_snd.output[1]) abort();
#endif

  /* Set up buffer pointers */
  fm_buffer  = (int16 **)&sms_snd.stream[STREAM_FM_MO];
  psg_buffer = (int16 **)&sms_snd.stream[STREAM_PSG_L];

  /* Set up SN76489 emulation */
  SN76489_Init(0, sms_snd.psg_clock, sms_snd.sample_rate);
  SN76489_Config(0, MUTE_ALLON, BOOST_OFF /*BOOST_ON*/, VOL_FULL, (sms.console < CONSOLE_SMS) ? FB_SC3000 : FB_SEGAVDP);

#if 0
  /* Set up YM2413 emulation */
  FM_Init();
#endif

  /* Restore YM2413 register settings */
  if (restore_sound) {
    memcpy(SN76489_GetContextPtr(0), psgbuf, SN76489_GetContextSize());
    // FM_SetContext(fmbuf);
    free(fmbuf);
    free(psgbuf);
  }

  /* Inform other functions that we can use sound */
  sms_snd.enabled = 1;

  return 1;
}

void sound_shutdown(void) {
  int i;

  if (!sms_snd.enabled) return;

  /* Free emulated sound streams */
  for (i = 0; i < STREAM_MAX; i++) {
    if (sms_snd.stream[i]) {
      free(sms_snd.stream[i]);
      sms_snd.stream[i] = NULL;
    }
  }

#ifndef NGC
  /* Free sound output buffers */
  for (i = 0; i < 2; i++) {
    if (sms_snd.output[i]) {
      free(sms_snd.output[i]);
      sms_snd.output[i] = NULL;
    }
  }
#endif

  /* Shut down SN76489 emulation */
  SN76489_Shutdown();

#if 0
  /* Shut down YM2413 emulation */
  FM_Shutdown();
#endif
}

void sms_sound_reset(void) {
  if (!sms_snd.enabled) return;

  /* Reset SN76489 emulator */
  SN76489_Reset(0);

#if 0
  /* Reset YM2413 emulator */
  FM_Reset();
#endif
}

void sound_update(int line) {
  int16 *fm[2], *psg[2];

  if (!sms_snd.enabled) return;

  /* Finish buffers at end of frame */
  if (line == smptab_len - 1) {
    psg[0] = psg_buffer[0] + sms_snd.done_so_far;
    psg[1] = psg_buffer[1] + sms_snd.done_so_far;
    fm[0]  = fm_buffer[0] + sms_snd.done_so_far;
    fm[1]  = fm_buffer[1] + sms_snd.done_so_far;

    /* Generate SN76489 sample data */
    SN76489_Update(0, psg, sms_snd.sample_count - sms_snd.done_so_far);

#if 0
    /* Generate YM2413 sample data */
    FM_Update(fm, sms_snd.sample_count - sms_snd.done_so_far);
#endif

    /* Mix streams into output buffer */
    sms_snd.mixer_callback(sms_snd.stream, sms_snd.output, sms_snd.sample_count);

    /* Reset */
    sms_snd.done_so_far = 0;
  }
  else {
    int tinybit;

    tinybit = smptab[line] - sms_snd.done_so_far;

    /* Do a tiny bit */
    psg[0] = psg_buffer[0] + sms_snd.done_so_far;
    psg[1] = psg_buffer[1] + sms_snd.done_so_far;
    fm[0]  = fm_buffer[0] + sms_snd.done_so_far;
    fm[1]  = fm_buffer[1] + sms_snd.done_so_far;

    /* Generate SN76489 sample data */
    SN76489_Update(0, psg, tinybit);

#if 0
    /* Generate YM2413 sample data */
    FM_Update(fm, tinybit);
#endif

    /* Sum total */
    sms_snd.done_so_far += tinybit;
  }
}

/* Generic FM+PSG stereo mixer callback */
void sound_mixer_callback(int16 **stream, int16 **output, int length) {
  int i;
  for (i = 0; i < length; i++) {
    // int16 temp = (fm_buffer[0][i] + fm_buffer[1][i]) / 2;
    // int16 temp = psg_buffer[1][i];
    output[0][i] = psg_buffer[0][i] * 2.75f;
    output[1][i] = psg_buffer[1][i] * 2.75f;
  }
}

/*--------------------------------------------------------------------------*/
/* Sound chip access handlers                                               */
/*--------------------------------------------------------------------------*/

void psg_stereo_w(int data) {
  if (!sms_snd.enabled) return;
  SN76489_GGStereoWrite(0, data);
}

void stream_update(int which, int position) {}

void psg_write(int data) {
  if (!sms_snd.enabled) return;
  SN76489_Write(0, data);
}

/*--------------------------------------------------------------------------*/
/* Mark III FM Unit / Master System (J) built-in FM handlers                */
/*--------------------------------------------------------------------------*/

int fmunit_detect_r(void) { return sms.fm_detect; }

void fmunit_detect_w(int data) {
  if (!sms_snd.enabled || !sms.use_fm) return;
  sms.fm_detect = data;
}

void fmunit_write(int offset, int data) {
  if (!sms_snd.enabled || !sms.use_fm) return;
  abort();  // FM_Write(offset, data);
}
