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

#ifndef MAX_PATH
#define MAX_PATH (256)
#endif 

typedef struct {
	int rd : 8;
	int nd : 8;
	int st : 8;
} CELL;

typedef struct {
	CELL cell[12][20];
	int time;
	ENEMYTYPE enemy[16];
	int x1, y1, x2, y2;
} LEVEL;

typedef struct {
	char title[32];
	SDL_Color pal[16]; // upper 16 colors
	SDL_Surface *sprites; // 5 sprites 16x16 each
	char midifile[64];
	int textcol;
	LEVEL level[5];
} AREA;

typedef struct {
	int numofareas;
	char title[32];
	char author[32];
	char pass[20][4];
	AREA *area; // pointer to area array
	char fullpath[256]; // fullpath to .WWD
} WORLD;

typedef struct {
	char fullname[MAX_PATH];
	char filename[MAX_PATH];
	char title[64];
	char author[64];
	char areas[64];
} WORLDINFO;

extern WORLD *wwd;

WORLD *LoadWorld(char *name);
void FreeWorld(WORLD *ww);

void fill_world_info(char *name, WORLDINFO *wi);
