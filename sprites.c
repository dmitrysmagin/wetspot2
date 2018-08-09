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

#ifndef USE_SDL2
#include <SDL_rotozoom.h>
#else
#include <SDL2_rotozoom.h>
#endif

#include "wetspot2.h"
#include "sprites.h"
#include "timer.h"
#include "input.h"

SDL_Surface *sprites;

#define SET_GAME_AREA_POINT(x, y, color) \
	{ if ((x) >= 0 && (x) < 320 && (y) >= 0 && (y) < 200) \
	*(unsigned char *)((y) * 320 + (x) + gamescreen->pixels) = color; }

void BlastLine(int x1, int y1, int x2, int y2, unsigned char color)
{
	const int deltaX = abs(x2 - x1);
	const int deltaY = abs(y2 - y1);
	static int temp, i;

	SDL_LockSurface(gamescreen);

	if (deltaX == 0) {
		if (x1 < 0 || x1 >= 320) return;
		if (y1 > y2) { temp = y2; y2 = y1; y1 = temp; }
		if (y1 < 0) y1 = 0;
		if (y2 >= 200) y2 = 200 - 1;
		for (i = y1; i <= y2; ++i)
		{
			*(char *)(gamescreen->pixels + i * 320 + x1) = color;
		}
	} else if (deltaY == 0) {
		if (y1 < 0 || y1 >= 200) return;
		if (x1 > x2) { temp = x2; x2 = x1; x1 = temp; }
		if (x1 < 0) x1 = 0;
		if (x2 >= 320) x2 = 320 - 1;
		for (i = x1; i <= x2; ++i) {
			*(char *)(gamescreen->pixels + y1 * 320 + i) = color;
		}
	} else {
		const int signX = x1 < x2 ? 1 : -1;
		const int signY = y1 < y2 ? 1 : -1;

		int error = deltaX - deltaY;
		SET_GAME_AREA_POINT(x2, y2, color);

		while(x1 != x2 || y1 != y2) {
			SET_GAME_AREA_POINT(x1, y1, color);
			const int error2 = error * 2;
			if(error2 > -deltaY) {
				error -= deltaY;
				x1 += signX;
			}
			if(error2 < deltaX) {
				error += deltaX;
				y1 += signY;
			}
		}
	}

	SDL_UnlockSurface(gamescreen);
}

void BlastPset(int x, int y, unsigned char color)
{
	SDL_LockSurface(gamescreen);

	if(x >= 0 && x < gamescreen->w &&
	   y >= 0 && y < gamescreen->h) {
		*(unsigned char *)(gamescreen->pixels + y * gamescreen->w + x) = color;
	}

	SDL_UnlockSurface(gamescreen);
}

void PutBox(int x1, int y1, int x2, int y2, unsigned char col)
{
	BlastLine(x1, y1, x2, y1, col);
	BlastLine(x1, y2, x2, y2, col);
	BlastLine(x1, y1, x1, y2, col);
	BlastLine(x2, y1, x2, y2, col);
}

void PutShape(int spritenum, int x, int y)
{
	SDL_Rect src, dst;

	src.x = (spritenum % 20) * 16;
	src.y = (spritenum / 20) * 16;
	src.w = 16;
	src.h = 16;

	dst.x = x;
	dst.y = y;

	SDL_BlitSurface(sprites, &src, gamescreen, &dst);
}

void BlitAndWait(int cycles)
{
	SDL_Rect dst;

	dst.x = 0;
	dst.y = 20;

#ifdef SCALE_SCREEN
	SDL_Surface *tmp = zoomSurface(gamescreen, 2, 2, 0);
	SDL_FillRect(screen, NULL, 0);
	SDL_BlitSurface(tmp, NULL, screen, NULL);
	SDL_FreeSurface(tmp);
#else
	SDL_BlitSurface(gamescreen, NULL, screen, &dst);
#endif

#ifndef USE_SDL2
	SDL_Flip(screen);
#else
	SDL_UpdateTexture(screenTexture, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
#endif

	for(int j = 0; j < cycles; j++) {
		SDL_PumpEvents();
		Wait();
	}

}
