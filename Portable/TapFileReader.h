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

#include <vector>
#include "ISerialiser.h"
#include "IFileOpener.h"
#include "TapFileSplitter.h"

namespace Jynx
{
	class TapFileReader
	{
	public:

		// A TAP file reader.
		// - TAP files seem to have just one Lynx file in them, as far as I can see.  (No Spec!)
		// Must construct afresh for each file to load!
		// Can rewind though!

		TapFileReader(); 
			// The constructor for an empty tape.

		explicit TapFileReader( IFileOpener *tapFileOpener );
			// Throws std:: file exceptions.
			// Throws a std::runtime_error if parse fails.

		// Motor
		void CassetteMotorOn( const class LynxTapeSpeed &lynxTapeSpeed );
		void CassetteMotorOff();

		// Playback
		void RewindPlaybackPosition();
		uint8_t GetSampleAtTime( uint64_t cycleCounterNow );

	private:

		TapFileSplitter _tapFileSplitter;

		// Wave data representation for the *current* file:
		std::vector<uint16_t>  _waveData;  // Bit 15 gives the data bit.  Bits 14..0 give the sustain period in Z80 cycles.

		// Playback:
		bool      _playbackResyncWithCycleCounterOnNextRead;
		uint32_t  _bitsPerSecond;
		size_t    _playbackFileIndex;
		size_t    _playbackWaveDataIndex;
		uint64_t  _playbackTimeOffset;
		uint64_t  _playbackCycleCounterOrigin;

		void SetPositionToFile( size_t fileIndex );

	};



	class LynxTapeSpeed
	{
	public:
		LynxTapeSpeed( uint8_t coarseSpeed, uint8_t fineSpeed ); // At read from the Lynx's RAM  ("COARSE" and "FINE" system variables).
		const uint32_t BitsPerSecond;   // as Camputers documentation has it, but we only care about ratios anyway!
	};

} // end namespace Jynx
