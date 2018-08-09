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

#include "wetspot2.h"
#include "font.h"
#include "palette.h"
#include "timer.h"
#include "logo.h"
#include "sprites.h"
#include "menu.h"
#include "world.h"

WORLD wetspot2;
WORLD *wwd = &wetspot2;

void rtrim(char *p)
{
	int c = strlen(p) - 1;

	for(; c >= 0; c--)
		if(*(p + c) == 0x20) *(p + c) = 0;
		else return;
}

// load old Wetspot 2 world
WORLD *LoadWorld(char *name)
{
	FILE *f;
	uint32_t tmp32;
	//int16_t tmp16;
	uint8_t tmp8;

	f = fopen(name, "rb");
	if(!f) return NULL;

	fread(&tmp32, sizeof(int32_t), 1, f);
	fread(&tmp8, sizeof(int8_t), 1, f);
	if(tmp32 == 0x44573257 && tmp8 == 0xFE) { // "W2WD"
		//printf("%s: found W2WD signature\n", name);
	} else {
		printf("Unknown file format\n");
		fclose(f);
		return NULL;
	}

	// allocate WORLD
	wwd = malloc(sizeof(WORLD));
	memset(wwd, 0, sizeof(WORLD));

	// set fullpath
	strcpy(wwd->fullpath, name);
	char *p = strrchr(wwd->fullpath, '/');
	strcpy(wwd->name, p + 1);
	printf("World filename: %s\n", wwd->name);
	if(p) *p = 0;
	printf("Full path to world: %s\n", wwd->fullpath);

	fread(&wwd->numofareas, sizeof(int16_t), 1, f);
	//printf("Areas: %i\n", wwd->numofareas);

	// allocate AREAs
	wwd->area = malloc(sizeof(AREA) * wwd->numofareas);
	memset(wwd->area, 0, sizeof(AREA) * wwd->numofareas);

	// world title
	memset(wwd->title, 0, sizeof(wwd->title));
	fread(wwd->title, 1, 30, f);
	rtrim(wwd->title);
	//printf("World name: %s\n", wwd->title);

	// world author
	memset(wwd->author, 0, sizeof(wwd->author));
	fread(wwd->author, 1, 20, f);
	rtrim(wwd->author);
	//printf("Author: %s\n", wwd->author);

	// 20 passwords
	fread(wwd->pass, 1, 20 * 4, f);

	// credits, 20 strings * 30 chars
	memset(wwd->credits, 0, 20*30);
	for(int i = 0; i < 20; i++) {
		fread(wwd->credits[i], 1, 30, f);
		rtrim(wwd->credits[i]);
	}

	// roll thru all areas
	for(int j = 0; j < wwd->numofareas; j++) {
		fseek(f, 0x02e1 + j * 0x166A, SEEK_SET);

		fread((wwd->area + j)->title, 1, 30, f);
		rtrim((wwd->area + j)->title);
		//printf("Area %i: %s\n", j, (wwd->area + j)->title);

		// area palette
		int r = 0, g = 0, b = 0;
		for(int i = 0; i < 16; i++) {
			fread(&r, sizeof(int8_t), 1, f);
			fread(&g, sizeof(int8_t), 1, f);
			fread(&b, sizeof(int8_t), 1, f);

			(wwd->area + j)->pal[i].r = r << 2;
			(wwd->area + j)->pal[i].g = g << 2;
			(wwd->area + j)->pal[i].b = b << 2;
		}

		// area sprites - 5 16x15
		(wwd->area + j)->sprites =
			SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 80, 8, 0, 0, 0, 0);

		fread((wwd->area + j)->sprites->pixels, 1, 1280, f);

		SDL_SetPalette((wwd->area + j)->sprites, SDL_LOGPAL, (wwd->area + j)->pal, 240, 16);

		// midi filename without extension
		memset((wwd->area + j)->midifile, 0, sizeof((wwd->area + j)->midifile));
		fread((wwd->area + j)->midifile, 1, 8, f);
		rtrim((wwd->area + j)->midifile);

		// if 'none' there's no midi
		if(!strcmp((wwd->area + j)->midifile, "none"))
			(wwd->area + j)->midifile[0] = 0;

		// append .MID
		if(strlen((wwd->area + j)->midifile) > 0)
			strcat((wwd->area + j)->midifile, ".MID");
			//printf("        %s\n", (wwd->area + j)->midifile);

		// default text color for area
		fread(&(wwd->area + j)->textcol, sizeof(int16_t), 1, f);

		// roll thru 5 levels
		for(int i = 0; i < 5; i++) {
			for(int y = 0; y < 12; y++)
				for(int x = 0; x < 20; x++) {
					tmp32 = 0;
					fread(&tmp32, sizeof(int16_t), 1, f);
					(wwd->area + j)->level[i].cell[y][x].st = tmp32 / 1000;
					(wwd->area + j)->level[i].cell[y][x].nd = (tmp32 % 1000) / 100;
					(wwd->area + j)->level[i].cell[y][x].rd = tmp32 % 100;
				}

			// get level time
			fread(&(wwd->area + j)->level[i].time, sizeof(int16_t), 1, f);

			// get enemies data (sign extension word16 -> word32)
			for(int e = 0; e < 16; e++) {
				int16_t tmp16[12];

				fread(tmp16, sizeof(int16_t), 12, f);

				(wwd->area + j)->level[i].enemy[e].x = tmp16[0];
				(wwd->area + j)->level[i].enemy[e].y = tmp16[1];
				(wwd->area + j)->level[i].enemy[e].ox = tmp16[2];
				(wwd->area + j)->level[i].enemy[e].oy = tmp16[3];
				(wwd->area + j)->level[i].enemy[e].z = tmp16[4];
				(wwd->area + j)->level[i].enemy[e].az = tmp16[5];
				(wwd->area + j)->level[i].enemy[e].typ = tmp16[6];
				(wwd->area + j)->level[i].enemy[e].dir = tmp16[7];
				(wwd->area + j)->level[i].enemy[e].action = tmp16[8];
				(wwd->area + j)->level[i].enemy[e].frame = tmp16[9];
				(wwd->area + j)->level[i].enemy[e].aframe = tmp16[10];
				(wwd->area + j)->level[i].enemy[e].change = tmp16[11];
			}

			// read starting x, y for two players
			int16_t tmp16[4];

			fread(tmp16, sizeof(int16_t), 4, f);
			(wwd->area + j)->level[i].x1 = tmp16[0];
			(wwd->area + j)->level[i].y1 = tmp16[1];
			(wwd->area + j)->level[i].x2 = tmp16[2];
			(wwd->area + j)->level[i].y2 = tmp16[3];
		}

		
	}

	fclose(f);

	return wwd;
}

// free all memory occupied by the world
void FreeWorld(WORLD *ww)
{
	// roll thru all areas
	for(int j = 0; j < ww->numofareas; j++) {
		SDL_FreeSurface((ww->area + j)->sprites);
	}

	// free areas
	free(ww->area);

	// free world itself
	free(ww);
}

void fill_world_info(char *name, WORLDINFO *wi)
{
	FILE *f;
	uint32_t tmp32;
	int16_t tmp16;
	uint8_t tmp8;

	f = fopen(name, "rb");
	if(!f) return;

	fread(&tmp32, sizeof(int32_t), 1, f);
	fread(&tmp8, sizeof(int8_t), 1, f);
	if(tmp32 == 0x44573257 && tmp8 == 0xFE) { // "W2WD"
		//printf("%s: found W2WD signature\n", name);
	} else {
		printf("Unknown file format\n");
		fclose(f);
		return;
	}

	fread(&tmp16, sizeof(int16_t), 1, f);
	sprintf(wi->areas, "NUMBER OF AREAS: %i", tmp16);

	strcpy(wi->title, "TITLE: ");
	fread(wi->title + 7, 1, 30, f);
	rtrim(wi->title);

	strcpy(wi->author, "AUTHOR: ");
	fread(wi->author + 8, 1, 20, f);
	rtrim(wi->author);

	fclose(f);
}
