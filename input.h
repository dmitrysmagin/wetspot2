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

#define WKEY_FIRE	(1 << 0)
#define WKEY_DOWN	(1 << 1)
#define WKEY_LEFT	(1 << 2)
#define WKEY_UP		(1 << 3)
#define WKEY_RIGHT	(1 << 4)

int GetPlayerInput(int pnum);
void SetPlayerKeys(int pnum);

int SDL_Pressed();
void SDL_PurgeEvents();
