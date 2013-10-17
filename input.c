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
#include <SDL/SDL.h>

#include "wetspot2.h"
#include "font.h"
#include "palette.h"
#include "timer.h"
#include "logo.h"
#include "sprites.h"
#include "menu.h"
#include "world.h"
#include "input.h"

int SDL_Pressed()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
		if(event.type == SDL_KEYDOWN) return TRUE;

	return FALSE;
}

void SDL_PurgeEvents()
{
	SDL_Event event;

	while(SDL_PollEvent(&event));
}
