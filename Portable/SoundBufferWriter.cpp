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

#include <assert.h>
#include "SoundBufferWriter.h"
#include "ZeroInitialiseMemory.h"


namespace Jynx
{
	SoundBufferWriter::SoundBufferWriter()
		: _soundBuffer( nullptr )
		, _soundBufferNumSamples( 0 )
		, _currentRecordingPos(0)
		, _lynxSpeakerLevel(0)
	{
	}




	void SoundBufferWriter::SetSoundBuffer( uint16_t *soundBuffer, size_t numSamples )
	{
		_soundBuffer = soundBuffer;
		_soundBufferNumSamples = (uint32_t) numSamples;
	}




	void SoundBufferWriter::WriteSample( uint8_t lynxSpeakerLevel, int32_t periodZ80Cycles, int32_t countdownZ80Cycles )
	{
		// Reminder: One Z80 timeslice length corresponds to a *single* buffer.

		if( SoundActive() )
		{
			if( countdownZ80Cycles < 0 )  countdownZ80Cycles = 0;  // Z80 overshot by a couple of cycles, trim off the overshoot so we don't address outside the buffer.

			if( countdownZ80Cycles < periodZ80Cycles )
			{
				// This is kind of "write behind":  Only NOW do we know how long the previous
				// level was held for!  So let's flush the previous level:

				auto fillToOffsetInCycles  = periodZ80Cycles - countdownZ80Cycles;
				auto fillToOffsetInSamples = (fillToOffsetInCycles * _soundBufferNumSamples) / periodZ80Cycles;
				FillTo( fillToOffsetInSamples );
			}
		}

		// Now remember the new level:

		_lynxSpeakerLevel = lynxSpeakerLevel;
	}



	void SoundBufferWriter::EndOfZ80PeriodNotification()
	{
		// Reminder: '_soundBufferNumSamples' gives the end of the CURRENT BUFFER.
		if( SoundActive() )
		{
			FillTo( _soundBufferNumSamples );  // use most recent speaker level
		}

		_currentRecordingPos = 0;
	}



	void SoundBufferWriter::FillTo( int32_t offsetInSamples )
	{
		// Reminder: 'offsetInSamples' is offset WITHIN the current buffer.

		if( _currentRecordingPos < offsetInSamples )
		{
			auto uintValue = (uint32_t) _lynxSpeakerLevel;
			auto valueToStore = uintValue << 9;  // Fit it into the +ve range of the SIGNED 16-bit samples.  [TODO:  Find out:  Audacity doesn't seem to support unsigned 16-bit as RAW import -- is it trying to tell us something?]

			auto currentPosition = _soundBuffer + _currentRecordingPos;
			auto endFillPosition = _soundBuffer + offsetInSamples;

			while( currentPosition < endFillPosition )
			{
				*currentPosition = valueToStore;
				++currentPosition;
			}

			_currentRecordingPos = offsetInSamples;   // rememeber for next time.
		}
	}



	void SoundBufferWriter::SerialiseSoundBufferContent( IBinarySerialiser *serialiser )
	{
		serialiser->Binary( _soundBuffer, _soundBufferNumSamples * 2 );
	}

} // end namespace Jynx
