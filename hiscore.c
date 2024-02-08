/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS for(int A PARTICULAR PURPOSE.  See the
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
#include <SDL.h>
#include <SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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
#include "theend.h"
#include "hiscore.h"

typedef struct {          // Record data type for the top 10
  char nam[16];           // player's name
  int score;              // high score
  int area;               // area reached
  int level;              // level reached
} RECORDTYPE;

typedef struct {          // Used only in the top 10
  float x;                // x position
  float y;                // y position
  float ax;               // x increase/decrease value
  float ay;               // y increase/decrease value
  int time;               // firework countdown
  int col;                // firework color index
} FIREWORKTYPE;

FIREWORKTYPE FireWork[MAXFIREWORKS];
RECORDTYPE Record[10];
float fx[50], fy[50];

#ifndef __EMSCRIPTEN__
char homepath[256] = "./";
#else
char homepath[256] = "/WETSPOT2";
#endif

void PathInit()
{
#ifdef __linux__
	char *home = getenv("HOME");
	if(home) sprintf(homepath, "%s/.wetspot2", home);
	mkdir(homepath, 0777);
#endif
}


void DefaultRecord()
{
	for(int i = 0; i < 10; i++) {
		strcpy(Record[i].nam, "NOBODY");
		Record[i].score = 10000;
		Record[i].area = 1;
		Record[i].level = 1;
	}
}

void SaveRecord()
{
	FILE *f;
	char filename[256];

	sprintf(filename, "%s/%s.REC", homepath, wwd->name);

	f = fopen(filename, "w");
	if(f) {
		for(int i = 0; i < 10; i++) {
			fprintf(f, "%s %d %d %d\n",
				   Record[i].nam,
				   Record[i].score,
		   		   Record[i].area,
				   Record[i].level);
		}
		fclose(f);
	}
}

void LoadRecord()
{
	FILE *f;
	char filename[256];
	char line[256];

	sprintf(filename, "%s/%s.REC", homepath, wwd->name);

	f = fopen(filename, "r");
	if(f) {
		for(int i = 0; i < 10; i++) {
			fgets(line, sizeof(line), f);
			sscanf(line, "%s %d %d %d", 
				   Record[i].nam,
				   &Record[i].score,
		   		   &Record[i].area,
				   &Record[i].level);
		}
		fclose(f);
	} else {
		DefaultRecord();
		SaveRecord();
	}
}

void DrawFireworks()
{
	int NumFW = 0;

	// Draws fireworks
	if(rand() % (MAXFIREWORKS * 220) < 70 - (NumFW * 10) &&
	   NumFW < MAXFIREWORKS) {
		// Creates a new firework object
		NumFW++;
		for(int i = 0; i < MAXFIREWORKS; i++) {
			if(FireWork[i].time == 0) {
				// Initializes the firework with random variables
				FireWork[i].x = rand() % 100 + 110;
				FireWork[i].y = 230;
				FireWork[i].ax = fmod((float)rand() / 1000.0, 2.0) - 1.0;
				FireWork[i].ay = -fmod((float)rand() / 1000.0, 1.0) - 3.0;
				FireWork[i].time = 140 + rand() % 60;
				FireWork[i].col = 240 + i;
				// Chooses its color
				/*SELECT CASE INT(RND(1) * 6)
				CASE 0: PalSet (240 + i), 63, 0, 0
				CASE 1: PalSet (240 + i), 0, 63, 0
				CASE 2: PalSet (240 + i), 0, 0, 63
				CASE 3: PalSet (240 + i), 63, 63, 0
				CASE 4: PalSet (240 + i), 0, 63, 63
				CASE 5: PalSet (240 + i), 63, 0, 63
				END SELECT*/
				break;
			}
		}
	}
#
	for(int i = 0; i < MAXFIREWORKS; i++) {
		if(FireWork[i].time > 0) {
			// Move firework
			FireWork[i].x += FireWork[i].ax;
			FireWork[i].y += FireWork[i].ay;
			FireWork[i].ay += 0.03;
			FireWork[i].time--;

			// The firework exists no more
			if(FireWork[i].time == 0) {
				NumFW--;
			}

			if(FireWork[i].time > 32) {
				// The firework is not exploded yet
				if(FireWork[i].x > 0 && FireWork[i].x < 318 &&
				   FireWork[i].y > 0 && FireWork[i].y < 198) {
					// It's still on the screen, so we can draw it
					//IF PEEK(VARPTR(Buffer(0)) + (INT(FireWork(i).y) * 320&) + INT(FireWork(i).x)) = 0 THEN
					// Draws it only if it's not over the text
					BlastPset(FireWork[i].x, FireWork[i].y, FireWork[i].col);
					BlastPset(FireWork[i].x + 1, FireWork[i].y, FireWork[i].col);
					BlastPset(FireWork[i].x, FireWork[i].y + 1, FireWork[i].col);
					BlastPset(FireWork[i].x + 1, FireWork[i].y + 1, FireWork[i].col);
					//END IF
				}
			} else {
				// The firework is exploding; fades its color to black
				/*PalGet FireWork(i).col, r, g, b
				IF r > 0 THEN r = r - 1
				IF g > 0 THEN g = g - 1
				IF b > 0 THEN b = b - 1
				PalSet FireWork(i).col, r, g, b*/
				// Makes four rings of pixels as the expanding explosion
				for(int ii = 0; ii < 4; ii++) {
					int s;
					float F;

					// Chooses expanding factor and the number of pixels for each ring
					switch(ii) {
					case 0: s = 5; F = .5; break;
					case 1: s = 2; F = 1.2; break;
					case 2: s = 1; F = 1.7; break;
					case 3: s = 1; F = 2.0; break;
					}
					
					for(int iii = 0; iii < 40; iii += s) {
						int x, y;
						// Calculates the coordinates of each pixel
						x = (int)(FireWork[i].x + (fx[iii] * ((32 - FireWork[i].time) * F)));
						y = (int)(FireWork[i].y + (fy[iii] * ((32 - FireWork[i].time) * F)));
						if(x > 0 && x < 319 &&
						   y > 0 && y < 199) {
							// The pixel is on the screen; draws it
							BlastPset(x, y, FireWork[i].col);
						}
					}
				}
			}
		}
	}
}

void DrawRecordTable()
{
	//Title$ = CHR$(34) + Title$ + CHR$(34)
	SPrint(wwd->title, (320 - (strlen(wwd->title) * 8)) / 2, 2, 56);

	SPrint("TOP 10 BEST PLAYERS", 84, 10, 56);
	SPrint("NAME                SCORE  AREA", 36, 24, 22);

	// Prints each record data
	for(int i = 0; i < 10; i++) {
		char line[48];
		char spaces[18] = "                  ";
		spaces[18 - strlen(Record[i].nam)] = 0;

		sprintf(line, (Record[i].area > Game.numareas ?
			"%s%s%07d   END" :
			"%s%s%07d   %d-%d"),
			Record[i].nam,
			spaces,
			Record[i].score,
			Record[i].area,
			Record[i].level);

		SPrint(line, 36, 40 + i * 16, (152 - i / 2));
	}

}

// Displays the current world top 10
void ShowTop10(int Blink1, int Blink2)
{
	memset(FireWork, 0, sizeof(FireWork));

	// Prepares SIN/COS table for more speed
	for(int i = 0; i < 40; i++) {
		fx[i] = cos((6.28 / 40.0) * (float)i);
		fy[i] = sin((6.28 / 40.0) * (float)i);
	}

	SDL_PurgeEvents();

	//Fade 1

	// Set the default game palette
	SDL_SetPalette(gamescreen, SDL_LOGPAL, gamepal, 0, 256);

	// Opens the records file
	LoadRecord();

	for(int TimeLeft = 32000; TimeLeft > 0; TimeLeft--) {
		// Draws the screen
		SDL_FillRect(gamescreen, NULL, 0);
		DrawFireworks();
		DrawRecordTable();

		if(TimeLeft / 20 % 2 == 0) {
			// If the players have made high scores, their crabs blink near their names
			if(Blink1 != -1) PutShape(18, 18, (36 + (Blink1 * 16)));
			if(Blink2 != -1) PutShape(38, 18, (36 + (Blink2 * 16)));
		}

		// Updates the screen
		if(SDL_Pressed()) break;
		BlitAndWait(1);
	}

	//Fade 0
	// Restores menu palette
	SDL_SetPalette(gamescreen, SDL_LOGPAL, syspal, 0, 256);
	//CLS
}

// Checks if a player has made a high score
void CheckForRecord()
{
	int Blink[2] = {-1, -1};
	extern PLAYERTYPE Player[2]; // FIXME

	// Opens the records file
	LoadRecord();

	// Find if players have made a high score
	for(int i = 0; i < Game.players; i++)
		for(int ii = 0; ii <= 9; ii++)
			if(Player[i].score > Record[ii].score) {
				Blink[i] = ii;
				break;
			}

	if(Game.mode != NORMAL) {
		Blink[0] = -1;
		Blink[1] = -1;
	}

	// Adjusts players positions in the high score list
	for(int i = 0; i < Game.players; i++) {
		if(Blink[i] > -1) {
			// Player 'i' has made an high score; gets his name for the top 10!
			// LATER: enter player name here
			//s$ = GetText$("PLAYER" + STR$(i + 1) + ", YOU MADE A HIT! YOUR NAME:", 16)

			// Shifts the other records...
			if(Blink[i] != 9) {
				for(int ii = 9; ii > Blink[i]; ii--) {
					strcpy(Record[ii].nam, Record[ii - 1].nam);
					Record[ii].score = Record[ii - 1].score;
					Record[ii].area = Record[ii - 1].area;
					Record[ii].level = Record[ii - 1].level;
				}
			}
			// ...and inserts the new champion
			//Record[Blink[i]].nam = s$;
			sprintf(Record[Blink[i]].nam, "PLAYER-%d", i + 1);
			Record[Blink[i]].score = Player[i].score;
			Record[Blink[i]].area =  Player[i].levelreached / 5 + 1;
			Record[Blink[i]].level = Player[i].levelreached % 5 + 1;
		}
	}

	SaveRecord();

#ifdef __EMSCRIPTEN__
	EM_ASM(
		// Persist local copy to IndexedDB
		FS.syncfs(false, _ => { });
	);
#endif

	// Shows the updated top 10 players list
	ShowTop10(Blink[0], Blink[1]);
}
