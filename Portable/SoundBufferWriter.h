//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2014  Jonathan Markland
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
//		jynx_emulator {at} yahoo {dot} com
// 

#pragma once

#include "stdint.h"
#include "ISerialiser.h"   // TODO: only need IBinarySerialiser

namespace Jynx
{
	class SoundBufferWriter
	{
	public:

		// "Draws" the waveform of the sound that the Lynx is generating onto a 44,100Hz CD MONO PCM sound buffer.
		// - This is an on-going process.  The emulation runs in "bursts", each burst mixes sound samples for an
		//   entire sound buffer (it fits *precisely*).  In fact, with sound enabled, the host's sound provision
		//   TIMES the entire emulation!
		// - During a burst the Lynx may NEVER write to its speaker port to change the value of it's 6-bit DAC, so
		//   the previous known value would be "drawn" across the entire sound buffer (flat line).
		// - Otherwise, when the Lynx writes the 6-bit DAC register, we use the Z80 cycle counter to work out WHEN
		//   (in real time) it would have been written, and the output waveform in the sound buffer must reflect that.
		// - Bear in mind that a Z80 cycle MAY overshoot by a microscopic amount because the instructions are
		//   variable length.  The remainder reduces the length of the next cycle.  This COULD cause sound buffer
		//   "drawing" issues (ie: overrun array), but I handle this.

		SoundBufferWriter();
		void SetSoundBuffer( uint16_t *soundBuffer, size_t numSamples );
		void WriteSample( uint8_t lynxSpeakerLevel, int32_t periodZ80Cycles, int32_t countdownZ80Cycles );
		void EndOfZ80PeriodNotification();
		void SerialiseSoundBufferContent( IBinarySerialiser * );

	private:

		uint16_t  *_soundBuffer;
		uint32_t   _soundBufferNumSamples;
		uint8_t    _lynxSpeakerLevel;           // so we know what to back-fill when the level changes!
		int32_t    _currentRecordingPos;

		void FillTo( int32_t offsetInSamples );
		bool SoundActive() const { return _soundBufferNumSamples > 0; }

	};

} // end namespace Jynx
