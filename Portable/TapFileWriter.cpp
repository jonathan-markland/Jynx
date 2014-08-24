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

#include "TapFileWriter.h"


namespace Jynx
{
	bool ApproxEquals( int32_t a, int32_t b )   // Used for signal length comparison (in Z80 cycles) because they're not precise.
	{
		auto diff = a-b;
		return (diff > -20) && (diff < 20);
	}



	TapFileWriter::TapFileWriter()
		: _barrierCycles(0)
		, _bitCounter(0)
		, _byteAccumulator(0)
		, _lastSyncPulseWidth(0)
		, _startSeen(false)
		, _startTime(0)
		, _state( TapWriterState::MotorOff )
		, _failed( false )
	{
	}



	bool TapFileWriter::IsTapeEmpty() const
	{
		return _dataVector.empty();
	}	



	bool TapFileWriter::IsModified() const
	{
		return ! HasFailed() && ! IsTapeEmpty();
	}



	bool TapFileWriter::HasFailed() const
	{
		return _failed;
	}
	
	
	
	void TapFileWriter::StartSynchronising()
	{
		_state = TapWriterState::Synchronising;
		_startSeen = false;
		_startTime = 0;
		_syncCounter = NumSyncCyclesToLookFor;
		_lastSyncPulseWidth = 0;
		_bitCounter = 0;
		_byteAccumulator = 0;
		_blockTypeIndicator = 0;
	}



	void TapFileWriter::NotifyCassetteMotorOn()
	{
		StartSynchronising();
	}	
	
	
	
	void TapFileWriter::NotifyCassetteMotorOff()
	{
		_state = TapWriterState::MotorOff;
	}	
	
	
	
	void TapFileWriter::WriteSignal( uint8_t speakerLevel, uint64_t z80CycleCounter )
	{
		if( _state != TapWriterState::MotorOff )
		{
			// The Lynx only measures the time "below the mid-line".
			// Time above is insignificant.
			// 1s are twice the time-length of 0s.
	
			if( speakerLevel < 32 && ! _startSeen )
			{
				_startSeen = true;
				_startTime = z80CycleCounter;
			}
			else if( speakerLevel >= 32 && _startSeen )
			{
				_startSeen = false;
				PulseSeen( (int32_t) (z80CycleCounter - _startTime) );
			}
		}	
	}



	void TapFileWriter::PulseSeen( int32_t pulseWidthCycles )
	{
		// On each "pulse" (time BELOW the mid-line) we come here to process it.

		// - Note: While 1 is twice the length of 0, the timing in z80 cycles is NOT 
		//   precise, and may waver +/- 20 Z80 cycles (about 10%).

		if( _state == TapWriterState::Synchronising )
		{
			// The SYNC is 768 x 0s.
			//
			// However, we don't require to read it all, just some of it to calculate the
			// time-length of a '0'.  '1' is supposedly double, but in practice it's slightly
			// less than double!
			//
			// Use most recent pulse as the yardstick, until we read enough adjacent
			// pulses of same(ish) size to be satisfied.
	
			if( ApproxEquals( _lastSyncPulseWidth, pulseWidthCycles ) )
			{
				--_syncCounter;
				if( _syncCounter == 0 )
				{
					// We've read enough SYNC bits.
					// - Calculate the barrier value used to discern 0 and 1, from now on:
					_barrierCycles = (_lastSyncPulseWidth * 11) / 10;  // allow a small margin extra

					// Time to await the first '1' bit of the 0xA5 marker.
					_state = TapWriterState::Await1;
					return;
				}
			}
			else
			{
				_syncCounter = NumSyncCyclesToLookFor;
			}
		
			_lastSyncPulseWidth = pulseWidthCycles;  // use most recent pulse as the yardstick
		}
		else 
		{
			// Now we have SYNC'd we can view the rest of the stream as
			// bit values:
			BitSeen( (pulseWidthCycles < _barrierCycles) ? 0 : 1 );
		}
	}


	
	void TapFileWriter::BitSeen( uint8_t bitValue )
	{
		// Shift this bit into the accumulator:
	
		_byteAccumulator = (_byteAccumulator << 1) | bitValue;
	
		// Process according to the state:

		if( _state == TapWriterState::Await1 )
		{
			// (We are probably still on the tail of the SYNC, so await the first '1')
		
			if( bitValue == 1 )
			{
				_state = TapWriterState::ReadA5;
				_bitCounter = 1;
			}
		}
		else if( _state == TapWriterState::ReadA5 )
		{
			++_bitCounter;
			if( _bitCounter == 8 )
			{
				if( _byteAccumulator == 0xA5 )
				{
					// We have seen the 0xA5 marker at the end of the SYNC.
					// Reset the bit counter, and allow byte-collection to commence:
					_bitCounter = 0;
					_state = TapWriterState::CollectBlockType;
				}
				else
				{
					_state = TapWriterState::BadTape;  // The caller can query this through the interface.
					_failed = true;
				}
			}
		}
		else if( _state == TapWriterState::CollectBlockType )
		{
			// Three block types:
			// -  "  Quotation mark: File name block.
			// -  B  Basic
			// -  M  Machine code
			++_bitCounter;
			if( _bitCounter == 8 )
			{
				_blockTypeIndicator = _byteAccumulator;
				_dataVector.push_back( _byteAccumulator );
				_bitCounter = 0;
				_state = TapWriterState::CollectData;
			}
		}
		else if( _state == TapWriterState::CollectData )
		{
			// Collect each byte into the vector:
			++_bitCounter;
			if( _bitCounter == 8 )
			{
				if( _blockTypeIndicator == '\"' && _byteAccumulator == 0x00 )
				{
					StartSynchronising();  // Second lot of SYNC (0x00 bytes) -- NOT recorded in TAP file!
				}
				else
				{
					_dataVector.push_back( _byteAccumulator );
					_bitCounter = 0;
				}
			}
		}
	}	


	
	void TapFileWriter::SaveToFile( IFileOpener *fileOpener )
	{
		if( _state == TapWriterState::MotorOff && ! HasFailed() && ! IsTapeEmpty() )  // TODO: maybe UI needs to know this instead:  CanSaveToTapFile()
		{
			std::ofstream  outStream;
			fileOpener->OpenOutputStream( outStream, std::ios::binary | std::ios::out );
			auto startAddress = &(*_dataVector.begin());
			outStream.write( (const char *) startAddress, _dataVector.size() );
			outStream.close();
		}
		else
		{
			throw std::runtime_error( "Cannot save TAP file.  There is no successfully generated tape to save." );
		}
	}

} // end namespace Jynx
