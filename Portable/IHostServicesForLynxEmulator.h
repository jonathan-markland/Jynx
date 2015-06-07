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

#include <ios>
#include <stdint.h>
#include "LynxRoms.h"

namespace Jynx
{
	class IHostServicesForLynxEmulator
	{
	public:

		// Services that must be provided by host, to the LynxEmulatorGuest object.

		//
		// Services called on the MAIN thread only:
		//

		virtual  void  OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) = 0;
			// (Called on the MAIN thread only)
			// Host must open the file indicated by romRequired with the stream object
			// passed, using the open mode passed.  This way, the host can decide where
			// to put the ROM files.

		virtual  void  InvalidateAreaOfGuestScreen_OnMainThread( int32_t left, int32_t top, int32_t right, int32_t bottom ) = 0;
			// (Called on the MAIN thread only)
			// The emulator is telling the host that the given region of the Lynx's screen should
			// be updated with the host's window manager.  The area is in Lynx screen coordinates.
			// - Note - The main thread must call LynxEmulatorGuest::CallMeBackToInvalidateRegions()
			//   to get this callback.

		//
		// Services called on the EMULATOR thread  (and NOT on the MAIN thread):
		//

		virtual  void TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues ) = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// Host has the opportunity to change the RGBX values in the 8-entry array
			// to something more convenient for the host.
			// By default, the translated pixel format is the same as a the original values format.

		virtual  void  PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData ) = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// The emulator is telling the host that the Lynx has written any of the colour banks
			// at the given addressOffset into the bank.  The host should accept the 8 adjacent
			// pixels in the array passed, and write them to its bitmap.  The pixel format is the
			// format that host itself indicated in the translation.  The bitmap
			// should NOT be painted with the host's Window Manager with this function!

		virtual  void WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread() = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// The Z80 thread wants to be suspended until sound is ready.

		virtual  void  NotifyOutputTapeAvailbilityChanged_OnAnyThread() = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// Emulator tells the host if the tape is no longer available, so host can adjust state of menu option.

	};

} // end namespace Jynx

