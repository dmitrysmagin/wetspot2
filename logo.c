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
#include <SDL.h>
#include <SDL_mixer.h>

#include "wetspot2.h"
#include "font.h"
#include "palette.h"
#include "timer.h"
#include "logo.h"
#include "sprites.h"
#include "menu.h"
#include "input.h"
#include "sound.h"

SDL_Surface *logo;

int SpotLight[31][31];

// Draw a spotlight at the given coordinates
void DrawLight(int x, int y)
{
	int addr;
	int col;

	SDL_LockSurface(gamescreen);

	addr = y * 320 + x;

	for(int yy = 0; yy < 31; yy++) {
		for(int xx = 0; xx < 31; xx++) {
			if(x + xx > 1 && x + xx < 318) {
				col = *(char *)(logo->pixels + addr + xx) + SpotLight[yy][xx];
				if(col < 0) col = 0;
				if(col > 127) col = 127;
				*(char *)(gamescreen->pixels + (yy + y) * 320 + (xx + x)) = col;
			}
		}
		addr += 320;
	}

	SDL_UnlockSurface(gamescreen);
}

// Display animated Enhanced Creations logo
void Logo()
{
	int sy[3] = {60, 70, 96}, ay[3] = {0, -4, -8};
	SDL_Color logopal[256], blackpal[256];

	// Initialize the spotlight array
	for(int y = 0; y < 31; y++)
		for(int x = 0; x < 31; x++)
			SpotLight[y][x] = 56 - (sqrt((x - 15) * (x - 15) + (y - 15) * (y - 15)) * 7);

	// Initialize logo palette
	memset(logopal, 0, sizeof(logopal));
	for(int i = 0; i < 64; i++) {
		logopal[i].r = 0;
		logopal[i].g = i << 2;
		logopal[i].b = 0;
		logopal[i + 64].r = 0;
		logopal[i + 64].g = 63 << 2;
		logopal[i + 64].b = 0;
	}

	SDL_SetPaletteColors(gamescreen->format->palette, logopal, 0, 128);

	// Animate the spotlights
	for(int i = -30; i <= 382; i += 3) {
		SDL_FillRect(gamescreen, NULL, 0);

		for(int ii = 0; ii <= 2; ii++) {
			sy[ii] += ay[ii];
			DrawLight(i - (ii * 31), sy[ii]);
			if(sy[ii] > 115) ay[ii] = -ay[ii]; else ay[ii]++;
		}

		BlitAndWait(2);
		if(SDL_Pressed()) goto _exit;
	}

	// Flash the screen by setting a black palette
	memset(blackpal, 0, sizeof(blackpal));

	SDL_SetPaletteColors(gamescreen->format->palette, blackpal, 0, 256);
	SDL_BlitSurface(logo, NULL, gamescreen, NULL);

	// Fade the colors to their original hues
	for(int j = 0; j < 63; j++) {
		for(int i = 0; i < 256; i++) {
			if(blackpal[i].r < logopal[i].r) blackpal[i].r += 4;
			if(blackpal[i].g < logopal[i].g) blackpal[i].g += 4;
			if(blackpal[i].b < logopal[i].b) blackpal[i].b += 4;
		}

		SDL_SetPaletteColors(gamescreen->format->palette, blackpal, 0, 256);
		SDL_BlitSurface(logo, NULL, gamescreen, NULL);

		BlitAndWait(1);
		if(SDL_Pressed()) goto _exit;
	}

	for(int i = 0; i < 150; i++) {
		if(SDL_Pressed()) goto _exit;
		BlitAndWait(1);
	}
_exit:
	SDL_PurgeEvents();
}

void Intro()
{
	static char *IntroText[3] = {
		"A GAME BY ANGELO MOTTOLA",
		"CUBY & COBY ARE BACK...",
		"...AND THEY CLAIM..."
	};

	// Restore game palette
	SDL_SetPaletteColors(gamescreen->format->palette, gamepal, 0, 256);

	// Display the three intro messages
	for(int i = 0; i < 3; i++) {
		SDL_FillRect(gamescreen, NULL, 0);
		SPrint(IntroText[i], ((320 - (strlen(IntroText[i]) * 8)) / 2), 96, 56);
		for(int i = 0; i < 200; i++) {
			BlitAndWait(1);
			if(SDL_Pressed()) goto _exit;
		}
	}

	// At the last message begin animating the crabs
	int frame = 0, aframe = 1;

	for(int i = 0; i < 155; i += 2) {
		SDL_FillRect(gamescreen, NULL, 0);
		SPrint(IntroText[2], ((320 - (strlen(IntroText[2]) * 8)) / 2), 96, 56);
		PutShape(12 + frame, i - 16, 92);
		PutShape(24 + frame, 320 - i, 92);
		if(i % 4 == 0) {
			frame += aframe;
			if(frame == 0 || frame == 2) aframe = -aframe;
		}
		BlitAndWait(2);
		if(SDL_Pressed()) goto _exit;
	}

	// Make the crabs do their victory gesture...
	for(int i = 0; i < 3; i++) {
		SDL_FillRect(gamescreen, NULL, 0);
		SPrint(IntroText[2], ((320 - (strlen(IntroText[2]) * 8)) / 2), 96, 56);
		PutShape(16 + i, 138, 92);
		PutShape(36 + i, 166, 92);
		BlitAndWait(8);
		if(SDL_Pressed()) goto _exit;
	}

	// ... and let they say: "Revenge!"
	PlaySound(17); //DMAplay EMSseg, 0, SoundLen(17), 11025&
	SPrint("REVENGE!", 128, 80, 192);

	for(int i = 0; i < 150; i++) {
		if(SDL_Pressed()) goto _exit;
		BlitAndWait(1);
	}
_exit:
	SDL_PurgeEvents();
}
