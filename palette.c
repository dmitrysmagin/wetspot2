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
#include <SDL.h>

#include "wetspot2.h"
#include "palette.h"

SDL_Color gamepal[256];
SDL_Color enemypal[3][80];
SDL_Color syspal[256];

void LoadPalette(char *name, SDL_Color *pal, int colnum)
{
	FILE *f;

	char tpal[768];

	if(colnum > 256) colnum = 256;

	f = fopen(name, "rb");
	fread(tpal, 1, 768, f);
	fclose(f);

	for(int i = 0; i < colnum; i++, pal++) {
		pal->r = tpal[i * 3] << 2;
		pal->g = tpal[i * 3 + 1] << 2;
		pal->b = tpal[i * 3 + 2] << 2;
		pal->b = tpal[i * 3 + 2] << 2;
	}
}