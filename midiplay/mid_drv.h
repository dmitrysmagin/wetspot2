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

void fm_write(int reg, int data);

void mid_set_timer(int value);
void mid_NoteOn(unsigned char channel, unsigned char note, unsigned char volume);
void mid_NoteOff(unsigned char channel, unsigned char note, unsigned char volume);
void mid_PatchLoad(unsigned char channel, unsigned char patch);
void mid_MdiPatchLoad(unsigned char channel, unsigned char *p);
void mid_MdiSetPercMode(unsigned char m);
void mid_Reset();
void mid_SetChannelVolume(int channel, int volume);
void mid_SetChannelPitchWheel(int channel);