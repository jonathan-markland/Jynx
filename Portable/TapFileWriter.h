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
#include "IFileOpener.h"


namespace Jynx
{
	namespace TapWriterState
	{
		// State enumeration for the TapFileWriter

		enum Enum
		{
			MotorOff, Synchronising, Await1, ReadA5, CollectBlockType, CollectData, BadTape
		};
	}	
	
	
	
	
	class TapFileWriter
	{
	public:

		// A multi-file TAP file saver.
		// Saves from the Lynx will just append.
		// Client must construct afresh for starting a new tape!

		TapFileWriter();

		bool HasFailed() const;
		bool IsTapeEmpty() const;
		bool IsModified() const;

		void NotifyCassetteMotorOn();
		void NotifyCassetteMotorOff();
		void WriteSignal( uint8_t speakerLevel, uint64_t z80CycleCounter );

		std::vector<uint8_t>  GetTapFileImage();  
			// This design allows the EMULATION thread to be suspended for a tiny
			// amount of time only, when saving a TAP file to the host.  This
			// completely avoids the disc access time.
			// Throws std::runtime_error exception if nothing to save.

	private:

		TapWriterState::Enum  _state;

		bool                  _startSeen;
		bool                  _failed;
		uint64_t              _startTime;
		int32_t               _lastSyncPulseWidth;
		uint32_t              _syncCounter;
		int32_t               _barrierCycles;
		uint32_t              _bitCounter;
		uint8_t               _byteAccumulator;
		uint8_t               _blockTypeIndicator;

		std::vector<uint8_t>  _dataVector;
	
		enum { NumSyncCyclesToLookFor = 300 };

	private:
	
		void StartSynchronising();
		void PulseSeen( int32_t pulseWidthCycles );
		void BitSeen( uint8_t bitValue );

	};	

} // end namespace Jynx
