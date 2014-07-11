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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

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

void DrawTheEndBg()
{
	// Clear screen and buffer
	SDL_FillRect(gamescreen, NULL, 0);

	// Draw a "tunnel" like structure with the last area tiles
	for(int y = 0; y < 20; y++) {
		for(int x = 0; x < 3; x++) {
			PutShape(237, y * 16, 76 + x * 16);
			PutShape(x == 1 ? 233 : 235, y * 16, 76 + x * 16);
		}
	}
}

// Shows the end of the game (the same for all worlds, sorry!)
void TheEnd()
{
	struct {
		float speed; // speed of each falling enemy
		int x;       // x coordinate
		int y;       // y coordinate
		int typ;     // type of enemy...
		int frame;   // ...and its current frame
	} DeadEnemy[16];
	int frame[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	int aframe[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	int x[7] = {0, 0, 0, 0, 0, 0, 0};

	// Loads and plays the ending song
	LoadMIDI("./world/THEEND.MID");
	PlayMIDI();

	// Makes the crabs and the enemies to walk throught this tunnel
	// The crabs are faster than the enemies, that walk all with the same speed
	for(int i = 0; i <= 340; i++) {
		// Updates the background
		DrawTheEndBg();

		for(int ii = 0; ii <= 8; ii++) {
			if(ii > 1 && frame[ii] > 9) {
				// The enemy has been hit by the block and it's flying away
				frame[ii] += 12;
				if(102 - frame[ii] > -16)
					PutShape((136 + (((frame[ii] / 10) % 4) * 7) + aframe[ii]), x[ii - 2], (102 - frame[ii]));
			} else {
				// Handles both players and enemies movements and frames
				switch(ii) {
				case 0:
				case 1:
				case 4:
				case 6:
				case 7:
				case 8:
					frame[ii] += aframe[ii];
					if(frame[ii] == 0 || frame[ii] == 5) aframe[ii] = -aframe[ii];
					break;
				case 2:
				case 5:
					frame[ii] += aframe[ii];
					if(frame[ii] == 0 || frame[ii] == 8) aframe[ii] = -aframe[ii];
					break;
				case 3:
					frame[ii] -= 1;
					if(frame[ii] < 0) frame[ii] = 7;
					break;
				}

				// Draws each sprite on the buffer
				switch(ii) {
				case 0:
				case 1:
					PutShape((4 + (ii * 20) + (frame[ii] / 2)), (320 + (ii * 24) - (i * 2)), 92);
					break;
				case 2:
					PutShape((43 + (frame[ii] % 3)), (392 - i), (91 + (frame[ii] / 3)));
					break;
				case 3:
					PutShape((60 + frame[ii]), (416 - i), 92);
					break;
				case 4:
					PutShape((68 + (frame[ii] / 2)), (440 - i), 92);
					break;
				case 5:
					PutShape(83, (464 - i), (91 + (frame[ii] / 3)));
					break;
				case 6:
					PutShape((95 + (frame[ii] / 2)), (488 - i), 92);
					break;
				case 7:
					PutShape((111 + (frame[ii] / 2)), (512 - i), 92);
					break;
				case 8:
					PutShape((123 + (frame[ii] / 2)), (536 - i), 92);
					break;
				}
			}
		}

		if(i > 290) {
			// If enough time has passed, a block starts moving from left to right
			PutShape(236, ((i - 291) * 16), 92);
			for(int ii = 2; ii <= 8; ii++) {
				if(frame[ii] < 10) {
					if(((i - 291) * 16) > ((392 + ((ii - 2) * 24)) - i)) {
						// The block has hit an enemy; it begins flying away
						frame[ii] = 10;
						aframe[ii] = ii - 2;
						x[ii - 2] = ((392 + ((ii - 2) * 24)) - i);
						PlaySound(11);
					}
				}
			}
		}

		// Copies the buffer to the screen
		BlitAndWait(2);
	}

	// Makes the crabs come back to the center of the tunnel...
	for(int i = -40; i <= 140; i += 2) {
		DrawTheEndBg();

		frame[0] += aframe[0];
		if(frame[0] == 0 || frame[0] == 5) aframe[0] = -aframe[0];
		PutShape((12 + (frame[0] / 2)), i, 92);
		PutShape((32 + (frame[0] / 2)), (i + 24), 92);

		BlitAndWait(2);
	}

	// ...and do their victory gesture
	PlaySound(13);
	for(int i = 0; i <= 2; i++) {
		DrawTheEndBg();

		PutShape((16 + i), 140, 92);
		PutShape((36 + i), 164, 92);

		BlitAndWait(2);
	}

	BlitAndWait(40);

	// Initializes the falling enemies with random variables
	srand(time(NULL));

	SDL_PurgeEvents();

	for(int count = 0; count < 32000; count++) {
		// Updates the buffer
		SDL_BlitSurface(theend, NULL, gamescreen, NULL);

		// Gets the world credits and prints them on the buffer
		for(int i = 0; i < 20; i++) {
			SPrint(wwd->credits[i],
				(320 - strlen(wwd->credits[i]) * 8) / 2,
				20 + i * 8, 208);
		}

		// Handles the falling enemies
		for(int i = 0; i <= 15; i++) {
			PutShape((136 + ((DeadEnemy[i].frame / 2) * 7) + DeadEnemy[i].typ),
				 DeadEnemy[i].x, DeadEnemy[i].y);
			DeadEnemy[i].y += DeadEnemy[i].speed;
			DeadEnemy[i].frame = (DeadEnemy[i].frame + 1) % 8;
			if(count == 0 || DeadEnemy[i].y > 199) {
				DeadEnemy[i].typ = rand() % 7; //INT(RND(1) * 7)
				DeadEnemy[i].speed = rand() % 2 + 1; //(RND * 2) + 1
				DeadEnemy[i].x = rand() % 336 - 16; //INT(RND(1) * 336) - 16
				DeadEnemy[i].y = rand() % 216 - 16; //INT(RND(1) * 216) - 16
				DeadEnemy[i].frame = rand() % 4; //INT(RND(1) * 4)
			}
		}

		// Updates the screen
		if(SDL_Pressed()) break;
		BlitAndWait(2);
	}
}
