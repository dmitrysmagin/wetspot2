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

#define DEBUG
#ifdef DEBUG
#define FPRINTF fprintf
#else
#define FPRINTF(...)
#endif

#define MAX_MIDI_TRACKS 48
#define MIDI_CHANNELS 16

#define MODE_MID 0
#define MODE_MDI 1
#define MODE_CMF 2

typedef struct {
	unsigned char *pMidiStart;
	unsigned char *pMidi;
	unsigned int dTime;
	unsigned char LastEvent;
	unsigned char FinishFlag;
} MIDITRACK;

typedef struct {
	unsigned char Patch;
	unsigned char Volume; // channel volume, 0..127, 90 by default
	unsigned char Panning; // 64 by default
	unsigned char Transpose; // may be not used
	unsigned short PitchWheel;
	unsigned short PitchRange;
	unsigned char RPN_LSB;
	unsigned char RPN_MSB;
} MIDICHANNEL;

extern int mid_Mode; // 0 - normal midi (perc is on), 1 - mdi (melo or perc), 2 - cmf (melo or perc)
extern MIDICHANNEL MidiChannels[MIDI_CHANNELS];

void mid_update_frame();
