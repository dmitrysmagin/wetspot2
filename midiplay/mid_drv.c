/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// This is a low-level backend for opl3 using ymf262 emulator from MAME
// If needed this could be replaced with any other opl3 emu or even
// with calls to real ym262 chip. :)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mid.h"
#include "mid_core.h"
#include "ymf262.h"

static int samplerate;
static int framesample;
static int initialized = 0;
static int ym = 0;

// length in bytes, not samples
void mid_getmix(void *stream, int length)
{
	int c, cnt = 0;
	extern int mid_playing;

	if(initialized && mid_playing)
		for(c = 0; c < length/4; c++) {
			if(cnt >= framesample) {
				cnt = 0;
				mid_update_frame();
			}

			YMF262UpdateOne(ym, (INT16 *)stream + c * 2, 1);
			cnt += 1;
		}
}

// reg = 00..ff for ports 220, 221
// reg = 100..1ff
void fm_write(int reg, int data)
{
	int port = 0;

	if((reg & 0x100) != 0) {reg &= 0xff; port = 2;}

	YMF262Write(ym, port, reg);
	YMF262Write(ym, port + 1, data);
}

void fm_reset()
{
	YMF262ResetChip(ym);
	fm_write(0x105, 1); // enable 18 channels
}

int mid_init(int rate, int channels)
{
#ifdef USE_SDL2
    rate *= 4;
#endif
	samplerate = rate;
	framesample = samplerate / 60;

	ym = YMF262Init(1, OPL3_INTERNAL_FREQ, rate);
	if(ym == -1) return (initialized = 0);

	fm_reset();

	return (initialized = 1);
}

int mid_deinit()
{
	mid_stop();
	YMF262Shutdown();
	return 1;
}

// value in Hz for timer
void mid_set_timer(int value)
{
	if(value == 0) value = 1;
	framesample = samplerate / value;
}

void *mid_load(char *name)
{
	FILE *fh;
	int fsize;
	void *p;

	fh = fopen(name, "rb");
	if(!fh) return NULL;

	fseek(fh, 0, SEEK_END);
	fsize = ftell(fh) ;
	fseek(fh, 0, SEEK_SET);
	p = (void *)malloc(fsize);
	fread(p, 1, fsize, fh);
	fclose(fh);

	return p;
}
