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
#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>

#define WKEY_FIRE	(1 << 0)
#define WKEY_DOWN	(1 << 1)
#define WKEY_LEFT	(1 << 2)
#define WKEY_UP		(1 << 3)
#define WKEY_RIGHT	(1 << 4)

#ifndef USE_SDL2
#define SKEY_DOWN	SDLK_DOWN
#define SKEY_LEFT	SDLK_LEFT
#define SKEY_UP		SDLK_UP
#define SKEY_RIGHT	SDLK_RIGHT
#define SKEY_BACKSPACE	SDLK_BACKSPACE
#define SKEY_LCTRL	SDLK_LCTRL
#define SKEY_LALT	SDLK_LALT
#define SKEY_LSHIFT	SDLK_LSHIFT
#define SKEY_SPACE	SDLK_SPACE
#define SKEY_ESCAPE	SDLK_ESCAPE
#define SKEY_RETURN	SDLK_RETURN
#define SKEY_TAB	SDLK_TAB
#else
#define SKEY_DOWN	SDL_SCANCODE_DOWN
#define SKEY_LEFT	SDL_SCANCODE_LEFT
#define SKEY_UP		SDL_SCANCODE_UP
#define SKEY_RIGHT	SDL_SCANCODE_RIGHT
#define SKEY_BACKSPACE	SDL_SCANCODE_BACKSPACE
#define SKEY_LCTRL	SDL_SCANCODE_LCTRL
#define SKEY_LALT	SDL_SCANCODE_LALT
#define SKEY_LSHIFT	SDL_SCANCODE_LSHIFT
#define SKEY_SPACE	SDL_SCANCODE_SPACE
#define SKEY_ESCAPE	SDL_SCANCODE_ESCAPE
#define SKEY_RETURN	SDL_SCANCODE_RETURN
#define SKEY_TAB	SDL_SCANCODE_TAB

#endif

int GetPlayerInput(int pnum);
void SetPlayerKeys(int pnum);

int SDL_Pressed();
void SDL_PurgeEvents();

#endif
