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
 
 // Example of using midi with opl3 instruments with SDL audio

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <unistd.h>
#include <SDL.h>

#include "mid.h"

#undef FALSE
#undef TRUE
#define FALSE 0
#define TRUE 1

#define BUFFSMPL 4096

SDL_AudioSpec audiostream;

unsigned char *pMid;

void __cdecl playcallback(void *userdata, unsigned char *stream, int length)
{
	mid_getmix(stream, length);
}

#undef main
int main(int argc, char *argv[])
{
	// if no arguments
	if(argc == 1) {
		printf("Usage: midiplay.exe midifile\n   Press any key! \n");
		return 1;
	}

	pMid = mid_load(argv[1]);
	if(pMid == NULL) {
		printf("Error reading %s\n", argv[1]);
		return 1;
	}

	// init sound system
	if(mid_init(44100, 2) == FALSE) {
		printf("Error initializing midi");
		return 1;
	}

	// init SDL soundsystem
	audiostream.freq = 44100;
	audiostream.format = AUDIO_S16;
	audiostream.channels = 2;
	audiostream.samples = BUFFSMPL;
	audiostream.callback = &playcallback;
	audiostream.userdata = 0;

	if(SDL_OpenAudio(&audiostream, 0) != 0) {
		printf("Error initializing SDL sound system");
		return 1;
	}

	SDL_PauseAudio(0);

	// play midi
	if(mid_play(pMid) == FALSE) {
		printf("Error reading midi %s\n", argv[1]);
		return 1;
	}

	// wait for keypress
	while(!kbhit()) usleep(50000);

	// deinit SDL sound
	SDL_PauseAudio(1);
	SDL_CloseAudio();

	// deinit sound system
	mid_deinit();

	free(pMid);

	return 0;
}