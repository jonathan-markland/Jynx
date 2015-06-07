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

#include <stdint.h>
#include <vector>



namespace Jynx
{
	class SignalLengthInfo
	{
	public:

		// Used by SignalWriter, not really of interest elsewhere.

		SignalLengthInfo( uint16_t lowCycles, uint16_t highCyclesAfterBits7to1, uint16_t highCyclesAfterBit0 )
			: LowCycles(lowCycles)
			, HighCyclesAfterBits7to1(highCyclesAfterBits7to1)
			, HighCyclesAfterBit0(highCyclesAfterBit0)
		{
		}

		const uint16_t  LowCycles;                   // Start by writing this many cycles LOW VALUE.
		const uint16_t  HighCyclesAfterBits7to1;     // If we are writing bits 7..1 of the byte, write this many cycles HIGH.
		const uint16_t  HighCyclesAfterBit0;         // If we are writing bit 0 of the byte, write this many cycles HIGH.
	};




	class SignalWriter
	{
	public:

		// Used for generation of a "wave" sequence for a Lynx cassette tape.

		// The implementation is, essentially, a run-length-encoded square-wave, where bit 15 has the 0/1 value.
		// Bits 14..0 have the duration in Z80 cycles.

		// - The emulator needs to interpret and synthesize Lynx tape sound waves, in order to
		//   support the very simple TAP file format.  This class is part of that, but it has
		//   been a labour to do tapes!

		SignalWriter( std::vector<uint16_t> &dataVector, const SignalLengthInfo &forZeroes, const SignalLengthInfo &forOnes );
		void WriteSyncAndA5();
		void WriteByte( uint8_t byteValue );
		void WriteBytes( const uint8_t *start, uintptr_t numBytes );
		void WriteExtraHighCycles( uint16_t repeatCount );

	private:

		std::vector<uint16_t> &_dataVector;
		const SignalLengthInfo _signalLengthsForZeroes;
		const SignalLengthInfo _signalLengthsForOnes;

		void WriteBit( uint16_t cycleCountLow, uint16_t cycleCountHigh );

	};


} // end namespace Jynx
