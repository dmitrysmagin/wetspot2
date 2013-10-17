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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "wetspot2.h"
#include "font.h"
#include "palette.h"
#include "timer.h"
#include "logo.h"
#include "sprites.h"
#include "menu.h"
#include "world.h"
#include "input.h"
#include "sound.h"

#include "midiplay/mid.h"

Mix_Chunk *sfx[MAXSOUNDS];
void *pmidi = NULL;

void PlaySound(int i)
{
	Mix_PlayChannel(-1, sfx[i], 0);
}

void MidiHook(void *userdata, unsigned char *stream, int length)
{
	mid_getmix(stream, length);

	short *p = (short *)stream;

	// boost up some volume
	// LATER: manage volume 0..128 here
	for(int i = 0; i < length / 2; i++, p++) {
		int s = *p; // int <- short
		s *= 4;
		if(s > 32767) s = 32767;
		else if(s < -32768) s = -32768;
		*p = s;
	}
}

void LoadMIDI(char *filename)
{
	printf("Loading MIDI: %s\n", filename);

	if(pmidi) free(pmidi);

	pmidi = mid_load(filename);
}

void PlayMIDI()
{
	if(pmidi) {
		mid_play(pmidi);
		Mix_ResumeMusic();
	}
}

void StopMIDI()
{
	Mix_PauseMusic();
	mid_stop();
}

void SoundInit()
{
	Mix_Init(0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

	atexit(Mix_Quit);
	atexit(Mix_CloseAudio);
	atexit(SoundDeinit);

	mid_init(22050, 2);
	Mix_HookMusic(MidiHook, NULL);
}

void SoundDeinit()
{
	Mix_HookMusic(NULL, NULL);
	mid_deinit();
}