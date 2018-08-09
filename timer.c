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
#include <time.h>
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
#include "world.h"
#include "timer.h"
#include "sound.h"

SDL_TimerID timer_id;
int TimerOn = FALSE;

// timer routine
Uint32 TimePass(Uint32 interval/*, void *param*/)
{
	if(TimerOn)
		Game.time--;

	return interval;
}

void CheckTime() // TimePass:
{
	float A;

	if(Game.status < 1) {
		//Game.time--;
		if(Game.time < 0) {
			for(int h = 0; h < Game.players; h++) {
				Death[h].speed += .02;
			}
		}
  
		if(Game.time == 0) {
			if(Game.status != -501) {
				PlaySound(5);
				srand(time(NULL));
				for(int h = 0; h < Game.players; h++) {
					A = rand() % 6 + (float)rand() / .28; // RND(1) * 6.28
					Death[h].x = 160 + (cos(A) * 200);
					Death[h].y = 100 + (sin(A) * 200);
					Death[h].frame = rand() % 3;
					Death[h].speed = .5;
				}
			} else {
				DrawObjects();
				SPrint("POTION BONUS", 112, 50, Game.textcol);
				BlastLine(110, 59, 208, 59, Game.textcol);
				SPrint("NO BONUS!", 124, 83, Game.textcol);
				BlastLine(110, 115, 208, 115, Game.textcol);
				BlitAndWait(200);
				Game.status = 501;
			}
		}

		if(Game.time == 15) {
			if(Game.status > -501 && Game.status < 1) {
				RedrawLevel();
				DrawObjects();
				PlaySound(4); //DMAplay EMSseg, 0, SoundLen(4), 11025&
				for(int o = 0; o < 4; o++) {
					SPrint("HURRY UP!!", 120, 90, Game.textcol);
					BlitAndWait(24);
					RedrawLevel();
					DrawObjects();
					BlitAndWait(24);
				}
				//if(Game.status > -1) ChangePal(0);
			}
		}

	}
}

void TimerInit()
{
	// make it tick every second
	timer_id = SDL_AddTimer(1000, (SDL_NewTimerCallback)&TimePass, NULL);
}

void TimerDeinit()
{
	SDL_RemoveTimer(timer_id);
}

char game_fps[16] = "    ";

void Wait()
{
	static int next_game_tick = 0;
	static int frame_start = 0;
	static int frame_end = 0;
	static int sleep_time  = 0;
	static int frames = 0; 

	next_game_tick += 17;
	frames++;
	frame_end = SDL_GetTicks();

	if(frame_end - frame_start >= 1000) {
		sprintf(game_fps, "%i", frames);
		frames = 0;
	}

	if(next_game_tick == 17) next_game_tick += frame_end;
	sleep_time = next_game_tick - frame_end;
	if(sleep_time > 0) SDL_Delay(sleep_time); else SDL_Delay(2);
	if(frames == 0) frame_start = next_game_tick;
}