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
#include "font.h"

// legacy data
char tfont[224][8]; // bit order is inverted

//SDL_Surface *font;

// Generic print used by other functions
void GPrint(char *s, int x, int y, int c, int g)
{
	int sh[8] = {1, 2, 4, 8, 16, 32, 64, 128};
	char *dst = (char *)gamescreen->pixels + y * 320 + x;

	SDL_LockSurface(gamescreen);

	for(; *s; s++, dst += 8) {
		for(int j = 0; j < 8; j++) {
			for(int i = 0; i < 8; i++) {
				if(tfont[*s - 32][j] & sh[7-i]) {
					*(dst + j * 320 + i) = (g == 1 ? c + j : c);
				}
			}
		}
	}

	SDL_UnlockSurface(gamescreen);
}

// Outs text centered on the screen at the given y position. This is used
// to display menu texts only (the palette is not the same of the game)
void MPrint(char *s, int y, int c, int sc)
{
	int x = (320 - strlen(s) * 8) / 2;

	if(sc == 255) {
		GPrint(s, x, y, c, 0);
	} else {
		GPrint(s, x, y + 1, sc, 0);
		GPrint(s, x + 1, y, sc, 0);
		GPrint(s, x, y, c, 0);
	}
}

// Prints text at specified location with gradient color
void SPrint(char *s, int x, int y, int c)
{
	GPrint(s, x, y, c, 1);
}

void PutScore(int sc, int x, int y, int c)
{
	char score[64];

	sprintf(score, "%i", sc);
	GPrint(score, x, y, c - 4, 0); // TEMP

/*
SUB PutScore (sc, xPos, yPos, sCol)
' Puts a score object on the screen
sc$ = LTRIM$(STR$(sc))
xPos = xPos + ((15 - (LEN(sc$) * 4)) \ 2) + (LEN(sc$) * 4)
' Draws each digit of the score number with the game font
FOR e = LEN(sc$) TO 1 STEP -1
  Code = ASC(MID$(sc$, e, 1)) + 11
  DEF SEG = VARSEG(FontData)
  FOR ee = 0 TO 4
    byte = PEEK(VARPTR(FontData) + (Code * 8) + ee)
    IF byte AND 8 THEN BlastPset VARSEG(Buffer(0)), VARPTR(Buffer(0)), (xPos), (yPos + ee), (sCol - ee - 4)
    IF byte AND 16 THEN BlastPset VARSEG(Buffer(0)), VARPTR(Buffer(0)), (xPos - 1), (yPos + ee), (sCol - ee - 3)
    IF byte AND 32 THEN BlastPset VARSEG(Buffer(0)), VARPTR(Buffer(0)), (xPos - 2), (yPos + ee), (sCol - ee - 2)
    IF byte AND 64 THEN BlastPset VARSEG(Buffer(0)), VARPTR(Buffer(0)), (xPos - 3), (yPos + ee), (sCol - ee - 1)
    IF byte AND 128 THEN BlastPset VARSEG(Buffer(0)), VARPTR(Buffer(0)), (xPos - 4), (yPos + ee), (sCol - ee)
  NEXT ee
  xPos = xPos - 4
NEXT e

END SUB
*/
}

void LoadFont(char *name)
{
	FILE *f;
	//char tfont[1792];

	f = fopen(name, "rb");
	fread(tfont, 1, 1792, f);
	fclose(f);

	//font = SDL_CreateRGBSurface(SDL_SWSURFACE, 224*8, 8, 8, 0, 0, 0, 0);
}
