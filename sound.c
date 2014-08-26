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
Mix_Music *music;
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

// Take midiname and consequently change extension until the file is found
static int find_modfile(char *modname, char *midiname)
{
	char *p, *ext[] = {
		".mid", ".MID", ".mod", ".MOD", ".s3m", ".S3M", ".it", ".IT",
		".xm", ".XM", ".ogg", ".OGG", ".mp3", "MP3", ".flac", ".FLAC"
	};

	strcpy(modname, midiname);

	for (unsigned int i = 0; i < sizeof(ext) / sizeof(char *); i++) {
		p = strrchr(modname, '.');
		if (p) *p = 0; // cut off extension if exists
		strcat(modname, ext[i]);

		FILE *f = fopen(modname, "rb");
		if (f) {
			printf("Loading %s: %s\n", ext[i], modname);
			fclose(f);
			return i;
		}
	}

	return -1;
}
void LoadMIDI(char *filename)
{
	char modname[256];

	if(pmidi) {
		free(pmidi);
		pmidi = NULL;
	}

	if (find_modfile(modname, filename) < 2) {
		pmidi = mid_load(filename);
	} else {
		if (music) Mix_FreeMusic(music);
		music = Mix_LoadMUS(modname);

		if (!music) {
			printf("Mix_LoadMUS error: %s\n", Mix_GetError());
		}
	}
}

void PlayMIDI()
{
	if(pmidi) {
		Mix_HookMusic(MidiHook, NULL);
		mid_play(pmidi);
	} else {
		Mix_PlayMusic(music, -1);
	}

	Mix_ResumeMusic();
}

void StopMIDI()
{
	if (pmidi) {
		Mix_HookMusic(NULL, NULL);
		mid_stop();
	} else {
		Mix_HaltMusic();
	}

	Mix_PauseMusic();
}

void SoundInit()
{
	Mix_Init(0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

	atexit(Mix_Quit);
	atexit(Mix_CloseAudio);
	atexit(SoundDeinit);

	mid_init(22050, 2);
}

void SoundDeinit()
{
	mid_deinit();
	if (music) Mix_FreeMusic(music);
}