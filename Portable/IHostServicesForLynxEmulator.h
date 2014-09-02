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
#include "LynxRoms.h"
#include "IHostThread.h"

namespace Jynx
{
	class IHostServicesForLynxEmulator  
	{
	public:

		// Services provided by host to the emulator.

		virtual  void  PaintPixelsOnHostBitmapForLynxScreenByte( uint32_t addressOffset, uint32_t lynxRedByte, uint32_t lynxGreenByte, uint32_t lynxBlueByte ) = 0;
			// (WARNING - Called on the Z80 thread, NOT the main thread)
			// The emulator is telling the host that the Lynx has written any of the colour banks
			// at the given addressOffset into the bank.  The host should translate the Lynx bytes
			// and plot them on the bitmap it is used to represent the lynx screen.  The bitmap
			// should NOT be painted with the host's Window Manager with this function!

		virtual  void  InvalidateAreaOfGuestScreen( int32_t left, int32_t top, int32_t right, int32_t bottom ) = 0;
			// (Called on the MAIN thread only)
			// The emulator is telling the host that the given region of the Lynx's screen should
			// be updated with the host's window manager.  The area is in Lynx screen coordinates.
			// - Note - The main thread must call LynxEmulatorGuest::CallMeBackToInvalidateRegions() 
			//   to get this callback.

		virtual  void  OpenChipFileStream( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) = 0;  
			// (Called on the MAIN thread only)
			// Host must open the file indicated by romRequired with the stream object
			// passed, using the open mode passed.  This way, the host can decide where
			// to put the ROM files.

		virtual  void  NotifyOutputTapeAvailbilityChanged() = 0;  
			// (WARNING - Called on the Z80 thread, NOT the main thread)
			// Emulator tells the host if the tape is no longer available, so host can adjust state of menu option.

		virtual  IHostThread *CreateThread( IHostServicesForLynxEmulatorThreadFunction threadFunction, void *userObject ) = 0;
			// (Called on the MAIN thread only)
			// Called to create a thread on the host platform, which supports our IHostThread interface.

		virtual  void ThreadSleep( uint32_t milliseconds ) = 0;
			// (WARNING - Called on the Z80 thread, NOT the main thread)
			// Called when sound is NOT ENABLED, thus sound is NOT timing the emulation.
			// Allows emulation thread to sleep for 20 ms.

		virtual  void WriteSoundBufferToSoundCardOrSleep() = 0;
			// (WARNING - Called on the Z80 thread, NOT the main thread)
			// The Z80 thread wants to be suspended until sound is ready.

	};

} // end namespace Jynx
