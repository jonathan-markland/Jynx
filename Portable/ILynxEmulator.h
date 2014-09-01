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

#include "ISerialiser.h"
#include "IFileOpener.h"
#include <vector>


namespace Jynx
{
	namespace LynxMachineType
	{
		enum Enum
		{
			LYNX_48K, LYNX_96K
		};
	}



	namespace LynxZ80Cycles
	{
		enum Enum
		{
			At50  =  40000,
			At100 =  70000,    // 80,000 * 50 timeslices per second = 4.00 Mhz
			At200 = 160000,
			At400 = 320000,
			At800 = 640000,
		};
	};



	class ILynxEmulator
	{
	public:

		// Services provided by the emulator to the emulator's host.

		virtual void CallMeBackToInvalidateRegions() = 0;
			// Host can ask for callbacks to invalidate screen areas, as needed.
			// Callbacks to the host are made through the function:
			// - IHostServicesForLynxEmulator::InvalidateAreaOfGuestScreen().

		virtual void NotifyKeyDown( int32_t guestKeyCode ) = 0;
			// Host must call this to tell emulator of key DOWN.

		virtual void NotifyKeyUp( int32_t guestKeyCode ) = 0;
			// Host must call this to tell emulator of key UP.

		virtual void NotifyAllKeysUp() = 0;
			// Host should call if its window is subject to switch-away, on the host window manager.
	
		virtual void SaveState( IFileOpener *fileOpener ) = 0;
			// Host calls this to ask emulator to save-state to the given file.

		virtual void LoadState( IFileOpener *fileOpener ) = 0;
			// Host calls this to ask emulator to load-state from the given file.
	
		virtual void ResetGuest( LynxMachineType::Enum machineType ) = 0;
			// Host calls this to do a hardware reset of the guest.

		virtual uint32_t GetCyclesPerTimeslice() const = 0;
		virtual void SetCyclesPerTimeslice( uint32_t numCycles ) = 0;
			// Set/get the number of Z80 cycles per AdvanceEmulation() timeslice.

		virtual void InsertBlankTape() = 0;
			// Host uses this to instruct emulator to insert a blank virtual cassette,
			// for saving purposes ("destination").

		virtual bool CanSaveTAPFile() const = 0;
			// Host uses this to query whether the emulator has recorded anything
			// at all onto the virtual "destination" cassette.

		virtual void SaveTape( IFileOpener *fileOpener ) = 0;
			// Host uses this to ask emulator to save the virtual "destination" cassette to a TAP file.

		virtual bool IsTapeModified() const = 0;
			// Host asks emulator whether the virtual "destination" cassette has been written
			// and is unsaved.

		virtual void LoadExistingTAPFile( IFileOpener *fileOpener ) = 0;
			// Host uses this to ask emulator to allow the Lynx to load from a TAP file.

		virtual bool CanRewindTape() const = 0;
			// Host uses this to ask whether the source cassette can be re-wound.
			// It can't if already re-wound.

		virtual void RewindTape() = 0;
			// Host instructs the emulator to re-wind the virtual source cassette.

		virtual void SetTapeSounds( bool tapeSounds ) = 0;
		virtual bool GetTapeSounds() const = 0;
			// Set/get whether the Lynx tape saving and loading waves can be heard by the user.
			// (These are subject to volume turned down).

		virtual void RecordSoundToFile( IFileOpener *fileOpener ) = 0;
			// Host instruction emulator to commence spooling sound to the given file.
			// WAV header will be written on closing the file.

		virtual void FinishRecordingSoundToFile() = 0;
			// Host calls this to stop the emulator recording sound to a WAV file.
			// The file header is written, and the file closed by this routine.

		virtual bool IsRecordingSoundToFile() const = 0;
			// The host can use this to ask the emulator if sound recording is in progress.

		virtual void RecordLynxTextToFile( IFileOpener *fileOpener ) = 0;
			// Host instruction emulator to commence spooling the Lynx's text output to a file.
			// This does not include text output by the Lynx during keyboard entry, since this
			// would capture the cursor flashing characters!
		
		virtual void FinishRecordingLynxTextToFile() = 0;
			// Host instruction emulator to finish spooling the Lynx's text output to a file.

		virtual bool IsRecordingLynxTextToFile() const = 0;
			// Host asks emulator whether the text is to be recorded to a file.

		virtual bool GetLynxRemCommandExtensionsEnabled() const = 0;
		virtual void SetLynxRemCommandExtensionsEnabled( bool enable ) = 0;
			// Set/get command extensions mode.
			// The emulator spies on the commands entered, and responds to special REM commands
			// typed at the basic prompt.
			// eg:    REM DISASM 85E 15
			//        (Disassemble from address 85E for 15 lines).

		virtual void TypeInTextFromFile( IFileOpener *fileOpener ) = 0;
			// The host can pass a file to the emulator to be "typed in" at the Lynx
			// prompt.  The emulator spies on the ROM key read routine and substitutes
			// the character stream from the file.
	};



} // end namespace Jynx
