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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "wetspot2.h"
#include "font.h"
#include "palette.h"
#include "timer.h"
#include "logo.h"
#include "sprites.h"
#include "world.h"
#include "menu.h"
#include "input.h"
#include "sound.h"
#include "hiscore.h"

SDL_Surface *title;

typedef struct {
	char *text;
	int y, col;
	void (*action)();
} MENUENTRY;

typedef struct {
	char *header;
	int y, col;
	MENUENTRY *menu;
	int num;
	int sel; // current selection
} MENU;

void ac_StartGame();
void ac_ChooseWorld();
void ac_EnterPassword();
void ac_RunOptionsMenu();
void ac_ShowCredits();
void ac_RunQuitMenu();

void ac_Quit();
void ac_GoBack();

MENUENTRY MainMenuEntries[] = {
	{ "START ONE PLAYER GAME", 125, 254, &ac_StartGame },
	{ "START TWO PLAYERS GAME", 135, 254, &ac_StartGame },
	{ "ENTER PASSWORD", 145, 254, &ac_EnterPassword },
	{ "LOAD EXTERNAL WORLD", 155, 254, &ac_ChooseWorld },
	//{ "OPTIONS", 165, 254, &ac_RunOptionsMenu },
	{ "SHOW CREDITS", 175-10, 254, &ac_ShowCredits },
	{ "QUIT", 185-10, 254, &ac_RunQuitMenu }
};

MENUENTRY QuitMenuEntries[] = {
	{ "LET ME QUIT NOW!", 140, 254, &ac_Quit },
	{ "DON'T QUIT", 150, 254, &ac_GoBack },
};

MENUENTRY OptionsMenuEntries[] = {
	{ "CUSTOMIZE KEYS", 150, 254, NULL },
	{ "MUSIC/FX VOLUME", 160, 254, NULL },
	{ "EXIT MENU", 170, 254, &ac_GoBack },
};

MENU MainMenu = {
	"rr MAIN MENU ss",
	110, 255,
	MainMenuEntries,
	6 /* 7 */, 0
};

MENU QuitMenu = {
	"rr QUIT GAME ss",
	110, 255,
	QuitMenuEntries,
	2, 0
};

MENU OptionsMenu = {
	"rr OPTIONS MENU ss",
	110, 255,
	OptionsMenuEntries,
	3, 0
};

MENU PasswordMenu = {
	"rr INSERT PASSWORD ss",
	110, 255,
	NULL,
	0, 0
};

MENU WorldMenu = {
	"rr LOAD EXTERNAL WORLD ss",
	110, 255,
	NULL,
	0, 0
};

int done = 0;

// Draws the game menu together with the selection box
void RedrawMenu(MENU *m)
{
	// Redraw menu
	SDL_BlitSurface(title, NULL, gamescreen, NULL);

	MPrint(m->header, m->y, m->col, 252);

	if(!m->menu) return;

	for(int i = 0; i < m->num; i++)
		MPrint((m->menu + i)->text, (m->menu + i)->y, (m->menu + i)->col, 252);

	int x1, y1, x2, y2;

	// Put selection box
	x1 = (320 - (strlen((m->menu + m->sel)->text) * 8)) / 2 - 8;
	y1 = m->menu->y + m->sel * 10 - 1;
	x2 = x1 + (strlen((m->menu + m->sel)->text) * 8) + 15;
	y2 = y1 + 8;

	PutBox(x1, y1, x2, y2, 83);
}

void RunMenu(MENU *m)
{
	int cnt = 0, TimeToDemo = 0;
	SDL_Event event;

	done = 0;

	while(!done) {
		while(SDL_PollEvent(&event))
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_UP && m->sel > 0) {
					cnt = 10;
					m->sel--;
					TimeToDemo = 0;
					PlaySound(0);
				} else if(event.key.keysym.sym ==  SDLK_DOWN && m->sel < m->num - 1) {
					cnt = 10;
					m->sel++;
					TimeToDemo = 0;
					PlaySound(0);
				} else if(event.key.keysym.sym == SDLK_RETURN ||
					  event.key.keysym.sym == SDLK_LCTRL) {
					PlaySound(1);
					if((m->menu + m->sel)->action) (*(m->menu + m->sel)->action)();
				} else if(event.key.keysym.sym == SDLK_ESCAPE) {
					cnt = 10;
					done = 1;
				}
			}

		TimeToDemo++;

		RedrawMenu(m);
		BlitAndWait(1);
  
		if(TimeToDemo == 1000) {
			// If no keys are pressed for a while, starts the demo
			srand(time(NULL));
			PlayGame(DEMO, rand() % wwd->numofareas, rand() % 5, (rand() % 2) + 1);
			//CLS : Fade 1
			//ShowCredits();
			ShowTop10(-1, -1);
			TimeToDemo = 0;
		}
	}
}

void ac_StartGame()
{
	PlayGame(NORMAL, 0, 0, MainMenu.sel + 1);
	CheckForRecord();
}

// 51 available characters
char PassChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'!\"$%&/()-+#@e ";

void DrawLetters(int cur_letter)
{
	char text[256];

	memset(text, 0, sizeof(text));

	if(cur_letter < 16) {
		strncat(text, PassChars + cur_letter + 35, 16 - cur_letter);
		strncat(text, PassChars, 16 + cur_letter);
	} else if (cur_letter > 35) {
		strncat(text, PassChars + cur_letter - 16, 51 - cur_letter + 16);
		strncat(text, PassChars, 16 - 51 + cur_letter);
	} else {
		strncat(text, PassChars + cur_letter - 16, 32);
	}

	MPrint(text, 125, 255, 252);
}

int check_pass(char *s)
{
	int i, maxi;

	for(i = 0, maxi = wwd->numofareas * 4; i < maxi; i++) {
		if(strncmp(s, (char *)wwd->pass + i * 4, 4) == 0)
			return i;
	}

	return -1;
}

void ac_EnterPassword()
{
	SDL_Event event;
	MENU *m = &PasswordMenu;
	char pass[16];

	m->num = 51;
	m->sel = 0;
	memset(pass, 0, sizeof(pass));

	done = 0;
	while(!done) {
		while(SDL_PollEvent(&event))
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_LEFT) {
					if(m->sel > 0) {
						m->sel--;
						PlaySound(0);
					} else {
						m->sel = m->num - 1;
					}
				} else if(event.key.keysym.sym == SDLK_RIGHT) {
					if(m->sel < m->num - 1) {
						m->sel++;
						PlaySound(0);
					} else {
						m->sel = 0;
					}
				} else if(event.key.keysym.sym == SDLK_LCTRL) {
					if(strlen(pass) < 5) {
						strncat(pass, PassChars + m->sel, 1);
					}
				} else if(event.key.keysym.sym == SDLK_LALT) {
					if(strlen(pass) != 0) {
						pass[strlen(pass) - 1] = 0;
					}
				} else if(event.key.keysym.sym == SDLK_RETURN) {
					int i;

					if((i = check_pass(pass)) != -1 &&
					   (pass[4] == '1' || pass[4] == '2')) {
						PlayGame(NORMAL, i, 0, pass[4] & 3);
						CheckForRecord();
					} else {
						PlaySound(1);
						MPrint("INVALID PASSWORD!!", 190, 255, 252);
						BlitAndWait(50);
					}
					done = 1;
				} else if(event.key.keysym.sym == SDLK_ESCAPE) {
					done = 1;
				}
			}

		SDL_BlitSurface(title, NULL, gamescreen, NULL);
		MPrint(m->header, m->y, m->col, 252);

		DrawLetters(m->sel);
		MPrint(pass, 150, 255, 252);
		PutBox(158, 123, 168, 135, 83);
		PutBox(134, 148, 184, 160, 83);

		MPrint("PRESS LEFT/RIGHT TO SELECT", 170, 255, 252);
		MPrint("<START> TO CONFIRM", 180, 255, 252);

		BlitAndWait(1);
	}

	done = 0;
}

WORLDINFO world_info[256]; // 256 worlds should be enough
int num_files;

// just to shut up mingw complaining about implicit definition
int strcasecmp(const char *, const char *);

void scan_for_wwd(char *path)
{
	char current_dir_name[MAX_PATH];
	DIR *current_dir = NULL;
	struct dirent *current_file = NULL;
	struct stat file_info;

	strcpy(current_dir_name, path);
	current_dir = opendir(current_dir_name);

	do {
		if(current_dir) current_file = readdir(current_dir); else current_file = NULL;

		if(current_file) {
			char *file_name = current_file->d_name;
			char full_name[MAX_PATH];
			int file_name_length = strlen(file_name);

			strcpy(full_name, current_dir_name);
			strcat(full_name, "/");
			strcat(full_name, file_name);

			if((stat(full_name, &file_info) >= 0) && ((file_name[0] != '.') || (file_name[1] == '.'))) {
				// if dir
				if(S_ISDIR(file_info.st_mode) && strcmp(file_name, "..")) {
					// do recursive scan
					scan_for_wwd(full_name);
				// if regular file
				} else if(S_ISREG(file_info.st_mode) && file_name_length >= 4) {
					int ext_pos;

					if(file_name[file_name_length - 4] == '.') ext_pos = file_name_length - 4;
					else if(file_name[file_name_length - 3] == '.') ext_pos = file_name_length - 3;
					else ext_pos = 0;

					if(!strcasecmp((file_name + ext_pos), ".WWD")) {
						if(num_files < 256) {
							strcpy(world_info[num_files].fullname, full_name);
							strcpy(world_info[num_files].filename, file_name);

							// read title and author from world file
							fill_world_info(full_name, &world_info[num_files]);
							num_files++;
						}
					}
				} 
			}
		}
	} while(current_file);

	closedir(current_dir);
}

void ac_ChooseWorld()
{
	num_files = 0;
	memset(world_info, 0, sizeof(world_info));

	scan_for_wwd("./world");
#ifdef linux
	// LATER: scan in $HOME
#endif

	// Now choose the world
	SDL_Event event;
	MENU *m = &WorldMenu;
	m->num = num_files;
	m->sel = 0;

	done = 0;
	while(!done) {
		while(SDL_PollEvent(&event))
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_LEFT) {
					if(m->sel > 0) {
						m->sel--;
						PlaySound(0);
					}
				} else if(event.key.keysym.sym == SDLK_RIGHT) {
					if(m->sel < m->num - 1) {
						m->sel++;
						PlaySound(0);
					}
				} else if(event.key.keysym.sym == SDLK_RETURN ||
					  event.key.keysym.sym == SDLK_LCTRL) {
					// reload new brave world
					FreeWorld(wwd);
					wwd = LoadWorld(world_info[m->sel].fullname);
					PlaySound(1);
					done = 1;
				} else if(event.key.keysym.sym == SDLK_ESCAPE) {
					done = 1;
				}
			}

		RedrawMenu(m);

		MPrint(world_info[m->sel].title, 130, 254, 252);
		MPrint(world_info[m->sel].author, 140, 254, 252);
		MPrint(world_info[m->sel].areas, 150, 254, 252);
		MPrint("PRESS LEFT/RIGHT TO SELECT", 170, 255, 252);
		MPrint("<START> TO CONFIRM", 180, 255, 252);

		BlitAndWait(1);
	}

	done = 0;
}

void ac_RunOptionsMenu()
{
	RunMenu(&OptionsMenu);
	done = 0;
}

void ac_ShowCredits()
{
	SDL_Event event;

	done = 0;

	while(!done) {
		while(SDL_PollEvent(&event))
			if(event.type == SDL_KEYDOWN) {
				done = 1;
			}

		SDL_BlitSurface(title, NULL, gamescreen, NULL);

		MPrint("WETSPOT 2 REMAKE", 108, 255, 252);
		MPrint("PORTED TO C/SDL", 122, 255, 252);
		MPrint("BY DMITRY SMAGIN IN 2013-14", 132, 255, 252);
		MPrint("ORIGINALLY CODED IN QUICKBASIC 4.5", 146, 255, 252);
		MPrint("BY ANGELO MOTTOLA IN 1997-98", 156, 255, 252);

		MPrint("GRAPHICS AND SFX BY ANGELO MOTTOLA", 170, 255, 252);
		MPrint("MIDI MUSIC IS (C) OF RESPECTIVE OWNERS", 180, 255, 252);

		BlitAndWait(1);
	}

	done = 0;
}

void ac_RunQuitMenu()
{
	static char *quit_text[6] = {
		"LET ME QUIT NOW!",
		"LOOSERS CAN QUIT...",
		"THAT'S ENOUGH FOR NOW",
		"I'LL TRY THE NEXT TIME...",
		"FLEE AWAY",
		"ARRGGGHHHH!!!",
};

	QuitMenu.menu->text = quit_text[rand() % 6];
	QuitMenu.sel = 0;

	RunMenu(&QuitMenu);
	done = 0;
}

void ac_Quit()
{
	exit(1);
}

void ac_GoBack()
{
	done = 1;
}

void Menu()
{
	// Restore system palette
	SDL_SetPalette(gamescreen, SDL_LOGPAL, syspal, 0, 256);

	RunMenu(&MainMenu);
}