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

// Flame-like unbeatable enemies data type
typedef struct {
	float x;			// x position
	float y;			// y position
	int frame;			// sprite frame number
	float speed;			// speed: it's increases at each cycle
} DEATHTYPE;

// Data type of the shots fired by the enemies
typedef struct {
	int x;			// x position
	int y;			// y position
	int ax;			// x increase/decrease value
	int ay;			// y increase/decrease value
	int typ;		// shot type
	int time;		// multiple purposes variable
} SHOTTYPE;

// Moving block data type
typedef struct {
	int x;			// x position
	int y;			// y position
	int ax;			// x increase/decrease value
	int ay;			// y increase/decrease value
	int by;			// which player hit the block?
	int hitscore;		// increases each time the block hits an enemy
	int bonus;		// bonus hidden within the block
} BLOCKTYPE;

// Object data type
typedef struct {
	int x;			// x position
	int y;			// y position
	int typ;		// object type
	int time;		// time left before the object is erased
} OBJECTTYPE;

// Holds all the main game variables
typedef struct {
	int players;	// number of players of current game
	int mode;	// game mode (NORMAL, DEMO or POTIONBONUS)
	int time;	// level time left
	int monsters;	// number of monsters still alive on the level
	int status;	// game status (has different purposes)
	int objects;	// number of objects on the screen
	int special;	// present bonus flag
	int numareas;	// number of total areas of the current world
	int area;	// current area
	int level;	// current level
	int textcol;	// area text color
	int delay;	// game delay; different on each computer
	int soundon;	// TRUE if sounds are on, otherwise FALSE
	int musicon;	// TRUE if musics are on, otherwise FALSE
	int framecount;	// framerate internal counter
	int framerate;	// framerate
} GAMETYPE;

// Data type to store the variables of each player
typedef struct {
	int dead;			// is the player dead?
	int control;			// player control method
	int x;				// crab x coordinate
	int y;				// crab y coordinate
	int dir;			// crab direction (0=down, 1=left, 2=up, 3=right)
	int action;			// current action (0=stop, 1=moving, >1=firing)
	int frame;			// sprite frame number
	int aframe;			// frame increase/decrease flag
	int speed;			// crab speed
	int status;			// player status
	int score;			// player score
	int nextextra;			// points needed for the next extra life
	int lives;			// number of lives
	char bonus[6];			// BONUS letters data
	int potion;			// number of bonuses taken in the potion bonus mode
	int levelreached;		// last level reached by the player
} PLAYERTYPE;

// Data type to store the variables of each enemy
typedef struct {
	int x;			// x position
	int y;			// y position
	int ox;			// old x position (shark uses ox for movements)
	int oy;			// old y position (shark uses oy for movements)
	int z;			// z position for flying enemies
	int az;			// z increase/decrease flag
	int typ;		// enemy type
	int dir;		// enemy direction (look at player's direction)
	int action;		// current action (depending on the enemy type)
	int frame;		// sprite frame number
	int aframe;		// frame increase/decrease flag
	int change;		// enemy change flag
} ENEMYTYPE;

// Game constants
#define FALSE		0
#define TRUE		1

#define NORMAL		0
#define DEMO		1
#define POTIONBONUS	2

#define KEYBOARD1	0
#define KEYBOARD2	1
#define JOY1		2
#define JOY2		3

#define MAXENEMIES	16	//15
#define MAXSHOTS	16	//15
#define MAXBLOCKS	8	//7
#define MAXOBJS		48	//47
#define MAXSOUNDS	19	//18
#define MAXFIREWORKS	4	//3

#define VERSION = "1.3"

extern Uint8 *keys;

extern GAMETYPE Game;
extern DEATHTYPE Death[2];

extern SDL_Surface *screen;
extern SDL_Surface *gamescreen;

void RedrawLevel();
void DrawScreen();

void PlayGame();
