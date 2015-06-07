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

#include "SignalWriter.h"



namespace Jynx
{
	SignalWriter::SignalWriter( std::vector<uint16_t> &dataVector, const SignalLengthInfo &forZeroes, const SignalLengthInfo &forOnes )
		: _dataVector( dataVector )
		, _signalLengthsForZeroes( forZeroes )
		, _signalLengthsForOnes( forOnes )
	{
	}



	void SignalWriter::WriteSyncAndA5()
	{
		for( int i=0; i<768; i++ )
		{
			WriteByte( 0x00 );
		}
		WriteExtraHighCycles( 0x15 );
		WriteByte( 0xA5 );
	}



	void SignalWriter::WriteByte( uint8_t byteValue )
	{
		// Loop for bits 7..1 inclusive:
		uint8_t bitMask = 0x80;
		while( bitMask != 1 )
		{
			if( byteValue & bitMask )
			{
				WriteBit( _signalLengthsForOnes.LowCycles, _signalLengthsForOnes.HighCyclesAfterBits7to1 );
			}
			else
			{
				WriteBit( _signalLengthsForZeroes.LowCycles, _signalLengthsForZeroes.HighCyclesAfterBits7to1 );
			}
			bitMask >>= 1;
		}
		// Finally do bit 0:
		if( byteValue & bitMask )
		{
			WriteBit( _signalLengthsForOnes.LowCycles, _signalLengthsForOnes.HighCyclesAfterBit0 );
		}
		else
		{
			WriteBit( _signalLengthsForZeroes.LowCycles, _signalLengthsForZeroes.HighCyclesAfterBit0 );
		}
	}



	void SignalWriter::WriteBytes( const uint8_t *start, uintptr_t numBytes )
	{
		auto pos = start;
		auto end = start + numBytes;
		while( pos < end )
		{
			WriteByte( *pos );
			++pos;
		}
	}



	void SignalWriter::WriteExtraHighCycles( uint16_t repeatCount )
	{
		_dataVector.push_back( 0x8000 | (repeatCount & 0x7FFF) );
	}



	void SignalWriter::WriteBit( uint16_t cycleCountLow, uint16_t cycleCountHigh )
	{
		// Tape is a square wave.
		// The level is in bit #15 (1=high, 0=low).  Bits 14..0 are the duration in cycles.
		// (The duration encodes whether its a 0 or a 1 being recorded on tape.  1s are longer than 0s).
		_dataVector.push_back( 0x0000 | (cycleCountLow & 0x7FFF) );
		_dataVector.push_back( 0x8000 | (cycleCountHigh & 0x7FFF) );
	}


} // end namespace Jynx
