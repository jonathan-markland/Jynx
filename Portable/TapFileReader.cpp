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

#include "stdint.h"
#include "TapFileReader.h"
#include <assert.h>


namespace Jynx
{
	TapFileReader::TapFileReader( ITapeSpeedSupplier *tapeSpeedSupplier )
		: _tapeSpeedSupplier( tapeSpeedSupplier )
		, _playbackResyncWithCycleCounterOnNextRead(true)
		, _bitsPerSecond(600)  // Initialisation not used as it happens!
		, _playbackCycleCounterOrigin(0)
	{
		RewindPlaybackPosition();
	}



	TapFileReader::TapFileReader( IFileOpener *tapFileOpener, ITapeSpeedSupplier *tapeSpeedSupplier )
		: _tapFileSplitter( tapFileOpener )
		, _tapeSpeedSupplier( tapeSpeedSupplier )
		, _playbackResyncWithCycleCounterOnNextRead(true)
		, _bitsPerSecond(600)  // Initialisation not used as it happens!
		, _playbackCycleCounterOrigin(0)
	{
		RewindPlaybackPosition();
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     CASSETTE MOTOR
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void TapFileReader::CassetteMotorOn()
	{
		_playbackResyncWithCycleCounterOnNextRead = true;
	}



	void TapFileReader::CassetteMotorOff()
	{
		// In order to support the "Pause Emulator after TAP load" feature, we do this:

		size_t allowance = 17;
			// The Lynx doesn't seem to consume the whole TAP necessarily, this is how many _waveData entries it doesn't consume.
			// This is used as a flexible "allowance" when deciding whether the Lynx has consumed the tape or not.

		if( _tapeSpeedSupplier->GetPauseAfterTapLoadEnable()
			&& _waveData.size() > allowance
			&& _playbackWaveDataIndex >= (_waveData.size() - allowance) )
		{
			// We decided that the Lynx did consume this tape, so pause the emulator:
			_tapeSpeedSupplier->SetPauseMode( true );
		}
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     PLAYBACK
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	std::string  TapFileReader::GetTapeDirectory( TapeDirectoryStyle::Enum styleRequired ) const
	{
		return _tapFileSplitter.GetTapeDirectory( styleRequired );
	}




	void TapFileReader::RewindPlaybackPosition()
	{
		SetPositionToFile(0);
	}



	void TapFileReader::SetPositionToFile( size_t fileIndex )
	{
		_playbackFileIndex = fileIndex;
		_playbackTimeOffset = 0;
		_playbackWaveDataIndex = 0;
		_playbackResyncWithCycleCounterOnNextRead = true;
		_waveData.clear(); // force re-constituting file (in case re-wind changed the _playbackFileIndex)
	}



	uint8_t TapFileReader::GetSampleAtTime( uint64_t cycleCounterNow )
	{
		// Files loop...

		while( _playbackFileIndex < _tapFileSplitter.GetNumberOfFiles() )
		{
			// Asked to resync?
			// This happens every time the cassette motor is turned ON by the lynx.
			// Which is before every SYNC.

			if( _playbackResyncWithCycleCounterOnNextRead )
			{
				_playbackResyncWithCycleCounterOnNextRead = false;
				_playbackCycleCounterOrigin = cycleCounterNow - _playbackTimeOffset;
			}

			// Do we need to generate the wave data?  (Multi-file TAPs are supported).
			// This happens if the wave data has run out.  (Always the case on the initial time).

			if( _waveData.empty() )
			{
				_bitsPerSecond = _tapeSpeedSupplier->GetLynxTapeSpeedBitsPerSecond();
				_waveData = _tapFileSplitter.GenerateWaveformForFile( _playbackFileIndex, _bitsPerSecond );
				assert( ! _waveData.empty() );
			}

			// Take wave measurement at current time, until wave data runs out:

			auto offsetIntoTapeInCycles = cycleCounterNow - _playbackCycleCounterOrigin;

			while( _playbackWaveDataIndex < _waveData.size() )
			{
				auto thisDatum = _waveData[_playbackWaveDataIndex];
				auto thisDuration = thisDatum & 0x7FFF;
				if( offsetIntoTapeInCycles < (_playbackTimeOffset + thisDuration) )
				{
					return (thisDatum >> 15);
				}
				_playbackTimeOffset += thisDuration;
				_playbackWaveDataIndex += 1;
			}

			// This waveform has expired.  Move to next file (if exists):

			SetPositionToFile( ++_playbackFileIndex );
		}

		// All files expired.

		return 0;
	}


} // end namespace Jynx
