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

// This is a minimal midi core library which rolls through midi events and
// does some magical stuff. :)

#include <string.h>
#include <stdio.h>

#include "mid.h"
#include "mid_core.h"
#include "mid_drv.h"

#undef FALSE
#undef TRUE
#define FALSE 0
#define TRUE 1

int mid_playing = 0;
int mid_type;
int mid_num_of_tracks;
int mid_DTperQN;
int mid_Tempo;
int mid_Freq;
//int mid_PercMode; // 0 - melo, 1 - perc
int mid_Mode; // 0 - normal midi (perc is on), 1 - mdi (melo or perc), 2 - cmf (melo or perc)

MIDITRACK MidiTracks[MAX_MIDI_TRACKS]; // array of midi tracks, more than 32 is useless, i think
MIDICHANNEL MidiChannels[MIDI_CHANNELS];

// read variable-length value and adjust pointer
static unsigned int ReadVarLen(int i)
{
	unsigned int value = 0;
	unsigned char c;

	// LATER PUT HERE READING OF .MUS var-len value
	if((value = *MidiTracks[i].pMidi++) & 0x80) {
		value &= 0x7F;
		do {
			value = (value << 7) | ((c = *MidiTracks[i].pMidi++) & 0x7F);
		} while (c & 0x80);
	}

	return value;
}

extern unsigned char *pMid;

void DoEvent(int i)
{
	unsigned char event, data;

	// take new midi event
	event = *MidiTracks[i].pMidi;

	// running status
	if((event & 0x80) == 0) {
		// remember the old event
		event = MidiTracks[i].LastEvent;
	} else {
		// store the event to remember it in case of running status
		MidiTracks[i].LastEvent = event;
		MidiTracks[i].pMidi++;
	}

	// check for ordinary events
	switch(event & 0xf0) {
	case 0x80: // note off
		mid_NoteOff(event & 0x0f, *MidiTracks[i].pMidi, *(MidiTracks[i].pMidi + 1));
		MidiTracks[i].pMidi += 2;
		return;

	case 0x90: // note on
		mid_NoteOn(event & 0x0f, *MidiTracks[i].pMidi, *(MidiTracks[i].pMidi + 1));
		MidiTracks[i].pMidi += 2;
		return;

	case 0xa0: // polyphonic after touch
		MidiTracks[i].pMidi += 2;
		return;

	case 0xb0: // controller change
		// controller types
		switch(*MidiTracks[i].pMidi) {
		case 1: // modulation
			break;
		case 6: // data entry MSB (for RPN) - semitones
			// check only for pitchwheel sensivity
			if((MidiChannels[event & 0x0f].RPN_MSB | MidiChannels[event & 0x0f].RPN_LSB) == 0) {
				MidiChannels[event & 0x0f].PitchRange &= 0x7f;
				MidiChannels[event & 0x0f].PitchRange |= (*(MidiTracks[i].pMidi + 1) << 7);
			}
			break;
		case 7: // channel volume
			MidiChannels[event & 0x0f].Volume = *(MidiTracks[i].pMidi + 1);
			mid_SetChannelVolume(event & 0x0f, *(MidiTracks[i].pMidi + 1));
			break;
		case 0x0a: // 10 panning
			MidiChannels[event & 0x0f].Panning = *(MidiTracks[i].pMidi + 1);
			// TO BE IMPLEMENTED
			// mid_SetChannelPanning
			break;
		case 0x0b: // 11 expression
			break;
		case 0x26: // 38 data entry LSB (for RPN) - cents
			if((MidiChannels[event & 0x0f].RPN_MSB | MidiChannels[event & 0x0f].RPN_LSB) == 0) {
				MidiChannels[event & 0x0f].PitchRange &= (0x7f << 7);
				MidiChannels[event & 0x0f].PitchRange |= *(MidiTracks[i].pMidi + 1);
			}
			break;
		case 0x40: // 64 sustain
			break;
		case 0x63: // 99 CMF AM+VIB depth control
			break;
		case 0x64: // 100 Registered Parameter Number (RPN) - LSB
			MidiChannels[event & 0x0f].RPN_LSB = *(MidiTracks[i].pMidi + 1);
			break;
		case 0x65: // 101 Registered Parameter Number (RPN) - MSB
			MidiChannels[event & 0x0f].RPN_MSB = *(MidiTracks[i].pMidi + 1);
			break;
		case 0x66: // 102 CMF marker
		case 0x67: // 103 CMF rhythm mode on/off
		case 0x68: // 104 CMF pitch up
		case 0x69: // 105 CMF pitch down
			break;
		case 0x78: // 120 all sounds off
			// TO BE IMPLEMENTED
			FPRINTF(stdout, "ALL SOUNDS OFF\n");
			break;
		case 0x79: // 121 reset all controllers
			// TO BE IMPLEMENTED
			FPRINTF(stdout, "RESET ALL CONTROLLERS\n");
			break;
		case 0x7b: // 123 all notes off
			// TO BE IMPLEMENTED
			FPRINTF(stdout, "ALL NOTES OFF\n");
			break;
		}

		MidiTracks[i].pMidi += 2;
		return;

	case 0xc0: // instrument number change
		// take instrument number
		// noteon event will reload patch dynamically
		MidiChannels[event & 0x0f].Patch = *MidiTracks[i].pMidi;
		MidiTracks[i].pMidi += 1;
		return;

	case 0xd0: // pressure after touch
		MidiTracks[i].pMidi += 1;
		return;

	case 0xe0: // pitch wheel change
		MidiChannels[event & 0x0f].PitchWheel = (*(MidiTracks[i].pMidi + 1) << 7) | *(MidiTracks[i].pMidi + 0);
		//FPRINTF(stdout, "PITCHWHEEL - Track: %i, Voice: %i, Value: %i\n", i, event & 15, MidiChannels[event & 0x0f].PitchWheel);
		mid_SetChannelPitchWheel(event & 0xf);
		MidiTracks[i].pMidi += 2;
		return;
	}

	//if((event & 0xf0) != 0xf0) return;

	// check for meta & system exclusive
	switch(event) {
	case 0xf0: // system exclusive event - ends with F7 byte
	case 0xf7:
		// SYSEX "7F 00" - MUS tempo change
		if(*MidiTracks[i].pMidi == 0x7F && *(MidiTracks[i].pMidi + 1) == 0x00) {
			/*
			non-standard... this is a tempo multiplier:
			data format: <F0> <7F> <00> <integer> <frac> <F7>
			tempo = basicTempo * integerPart + basicTempo * fractionPart/128
			*/
			;
		}

		while(*MidiTracks[i].pMidi != 0xF7) MidiTracks[i].pMidi++;
		MidiTracks[i].pMidi++;
		return;
	case 0xf1:
		break;
	case 0xf2:
		MidiTracks[i].pMidi += 2;
		break;
	case 0xf3:
		MidiTracks[i].pMidi += 1;
		break;
	case 0xf4:
	case 0xf5:
		break;
	case 0xf6:
	case 0xf8:
	case 0xfa:
	case 0xfb:
	case 0xfc: // MUS - equivalent to end of track
		MidiTracks[i].FinishFlag = 1;
		//MidiTracks[i].pMidi += 1;
		return;
	case 0xfd:
		break;
	case 0xff: // meta events here
		data = *MidiTracks[i].pMidi;
		MidiTracks[i].pMidi += 1;

		// meta-event tag
		switch(data) {
		case 0x2f: // end of the track
			MidiTracks[i].FinishFlag = 1;
			MidiTracks[i].pMidi += 1;
			return;
		case 0x51: // tempo change
			mid_Tempo = (*(MidiTracks[i].pMidi + 1) << 16) +
						(*(MidiTracks[i].pMidi + 2) << 8) +
						(*(MidiTracks[i].pMidi + 3));
			mid_Freq = 60000000 / mid_Tempo * mid_DTperQN / 60;	// Hz = 60000000/MidiTempo*TicksPerQuaterNote/60
			mid_set_timer(mid_Freq);

			//FPRINTF(stdout, "TEMPO - Track: %i, Tempo: 0x%x, Hz: %i\n", i, mid_Tempo, mid_Freq);
			break;
		case 0x7f: // sequencer specific, adlib data will be there!!
			// Ad Lib midi ID is 00 00 3f
			// then 2 bytes of adlib event and then data
			if(*(MidiTracks[i].pMidi + 1) == 0 && *(MidiTracks[i].pMidi + 2) == 0 && *(MidiTracks[i].pMidi + 3) == 0x3f) {
				if(mid_Mode != MODE_MDI) { // switch to mdi mode, do it once
					mid_Mode = MODE_MDI;
					mid_MdiSetPercMode(0);
				}

				switch(*(MidiTracks[i].pMidi + 5)) {
				case 1: // instrument change (1 byte voice num + 28 bytes of data)
					mid_MdiPatchLoad(*(MidiTracks[i].pMidi + 6), MidiTracks[i].pMidi + 7);
					//FPRINTF(stdout, "MDI PATCH LOAD - Track: %i, Voice: %i\n", i, *(MidiTracks[i].pMidi + 6));
					break;
				case 2: // melo/perc mode (0 - melo, !0 - perc) 1 byte
					mid_MdiSetPercMode(*(MidiTracks[i].pMidi + 6));
					//FPRINTF(stdout, "MDI MELO/PERC MODE - Track: %i, Value: %i\n", i, *(MidiTracks[i].pMidi + 6));;
					break;
				case 3: // pitch range (1 byte)
					// global for all voices!
					for(int j = 0; j < 16; j++) MidiChannels[j].PitchRange = *(MidiTracks[i].pMidi + 6) << 7;
					//FPRINTF(stdout, "MDI PITCHWHEEL RANGE - All tracks, Value: %i\n", MidiChannels[event & 0x0f].PitchRange);
					break;
				}
			}
			break;
		}

		// length of the meta-event
		data = *MidiTracks[i].pMidi;
		MidiTracks[i].pMidi += data + 1;
		return;
	default: // all other unhandled shit
		FPRINTF(stdout, "SYSEX unhandled message 0x%x, Track: %i\n", event, i);
		MidiTracks[i].pMidi += 1;
	}

}

// do track events
void DoTrackEvents(int i) // track number here
{
	if(MidiTracks[i].pMidi == MidiTracks[i].pMidiStart) goto here;

	if(MidiTracks[i].dTime == 0) {
	again:
		DoEvent(i);
		if(MidiTracks[i].FinishFlag == 1) return;
	here:
		MidiTracks[i].dTime = ReadVarLen(i);

		if(MidiTracks[i].dTime == 0) goto again;
	}

	MidiTracks[i].dTime--;
}

int CheckEndAllTracks()
{
	for(int i = 0; i < mid_num_of_tracks; i++) {
		if(MidiTracks[i].FinishFlag == 0) return 0;
	}

	return 1;
}

void RestartMidiTracks()
{
	for(int i = 0; i < mid_num_of_tracks; i++) {
		MidiTracks[i].pMidi = MidiTracks[i].pMidiStart;
		MidiTracks[i].FinishFlag = 0;
		MidiTracks[i].dTime = 0;
		MidiTracks[i].LastEvent = 0;
	}
}

void ResetMidiChannels()
{
	memset(MidiChannels, 0, sizeof(MidiChannels));
	for(int i = 0; i < 16; i++) {
		MidiChannels[i].Volume = 127;
		MidiChannels[i].Panning = 64;
		MidiChannels[i].PitchWheel = 0x2000;
		MidiChannels[i].PitchRange = (2 << 7); // semitones << 7
	}

	mid_Reset(); // reset driver - shut all sound, free voices
}

void mid_update_frame()
{
	if(mid_playing == 1) {
		// do tracks
		for(int i = 0; i < mid_num_of_tracks; i++) {
			if(CheckEndAllTracks() == 1) {
				ResetMidiChannels();
				RestartMidiTracks();
				return;
			}

			// play if track is not over
			if(MidiTracks[i].FinishFlag == 0) DoTrackEvents(i);
		}
	}
}

static inline int get16(unsigned char *w)
{
	return (*w << 8) | *(w + 1);
}

static inline int get32(unsigned char *dw)
{
	return (*dw << 24) | (*(dw + 1) << 16) | (*(dw + 2) << 8) | *(dw + 3);
}

int mid_play(unsigned char *p)
{
	if(mid_playing == 1) mid_stop();

	if(get32(p) != 'MThd') { // if not 'MThd' signature - it's not midi
		return FALSE;
	}

	mid_Mode = MODE_MID; // normal mid
	mid_MdiSetPercMode(1); // always on for normal midi

	memset(MidiTracks, 0, sizeof(MidiTracks));
	ResetMidiChannels();

	// read header
	mid_type = get16(p + 8);
	if(mid_type > 1) return FALSE; // only support 0 or 1 midi type

	mid_num_of_tracks = get16(p + 10);
	if(mid_num_of_tracks > MAX_MIDI_TRACKS) mid_num_of_tracks = MAX_MIDI_TRACKS; // not more than 32 tracks, should be enough

	mid_DTperQN = get16(p + 12);

	FPRINTF(stdout, "Midi type: %i\n", mid_type);
	FPRINTF(stdout, "Number of tracks: %i\n", mid_num_of_tracks);
	FPRINTF(stdout, "Delta ticks per quater note: %i\n", mid_DTperQN);

	// roll thru all tracks and initialize track array
	p += 14;
	for(int i = 0; i < mid_num_of_tracks; i++) {
		if(get32(p) != 'MTrk') { // if error reading track id - exit
			FPRINTF(stdout, "Error reading MTrk, %i tracks will be played\n", i);
			mid_num_of_tracks = i;
			break;
		}

		MidiTracks[i].pMidiStart = MidiTracks[i].pMidi = p + 8;
		p += get32(p + 4) + 8;
	}

	mid_playing = 1;

	return TRUE;
}

void mid_stop()
{
	mid_playing = 0;
	ResetMidiChannels();
}