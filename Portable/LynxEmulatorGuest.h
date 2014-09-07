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

#include <memory>
#include "IHostServicesForLynxEmulator.h"
#include "ISerialiser.h"
#include "Z80\JynxZ80.h"
#include "LynxHardware.h"
#include "LynxEmulatorEnums.h"
#include "SoundBufferWriter.h"
#include "SoundRecorder.h"
#include "TextRecorder.h"
#include "TextPlayer.h"
#include "TapFileReader.h"
#include "TapFileWriter.h"
#include "JynxPlatformInterfacing.h"

namespace Jynx
{
	class LynxEmulatorGuest: public ITapeSpeedSupplier
	{
	public:

		// The Camputers Lynx hardware emulator.
		
		// - This creates a thread to run the emulation.
		// - This is a singleton class at the moment, see use of g_LynxEmulatorGuestSingleton.

		LynxEmulatorGuest( IHostServicesForLynxEmulator *hostObject, uint16_t *soundBuffer, size_t numSamples, LynxMachineType::Enum initialMachineType, const char *platformEndOfLineSequenceUTF8 );
			// Reminder: Constructor called on the CLIENT's thread.
			// Client tells guest where the sound buffer is.
			// The format is fixed at 44,100Hz CD MONO sound.

		~LynxEmulatorGuest();
			// Reminder: Destructor called on the CLIENT's thread.

		// ILynxEmulator interface called by the client, on the MAIN thread.
		// (These functions are threadsafe).
		void CallMeBackToInvalidateRegions();
		void NotifyKeyDown( int32_t guestKeyCode );
		void NotifyKeyUp( int32_t guestKeyCode );
		void NotifyAllKeysUp();
		void SaveState( IFileOpener *fileOpener );
		void LoadState( IFileOpener *fileOpener );
		void ResetGuest( LynxMachineType::Enum machineType );
		uint32_t GetCyclesPerTimeslice() const;
		void SetCyclesPerTimeslice( uint32_t numCycles );
		void InsertBlankTape();
		bool CanRewindTape() const;
		void RewindTape();
		void LoadExistingTAPFile( IFileOpener *fileOpener );
		bool CanSaveTAPFile() const;
		void SaveTape( IFileOpener *fileOpener );
		bool CanSaveSnapshot() const;
		void SetTapeSounds( bool tapeSounds );
		bool GetTapeSounds() const;
		void RecordSoundToFile( IFileOpener *fileOpener );
		void FinishRecordingSoundToFile();
		bool IsRecordingSoundToFile() const;
		void RecordLynxTextToFile( IFileOpener *fileOpener );
		void FinishRecordingLynxTextToFile();
		bool IsRecordingLynxTextToFile() const;
		bool GetLynxRemCommandExtensionsEnabled() const;
		void SetLynxRemCommandExtensionsEnabled( bool enable );
		void TypeInTextFromFile( IFileOpener *fileOpener );
		void SetPauseMode( bool pauseMode );
		bool GetPauseMode() const;
		void SetPauseAfterTapLoadEnable( bool pauseAfterTapLoadEnable );
		bool GetPauseAfterTapLoadEnable() const;
		void TypeTapeDirectoryIntoLynx();

		// Implementing ITapeSpeedSupplier
		virtual uint32_t  GetLynxTapeSpeedBitsPerSecond() override;
		virtual bool GetPauseAfterTapLoadEnable() override;

	private:

		// This is where proof of thread-safeness trumps the inadequate nature of "private"!
		// We want to say that these 5 can call the 5 functions below.
		friend uint8_t JynxZ80::Z80ImplementationBaseClass::GuestRead( uint16_t address );
		friend void    JynxZ80::Z80ImplementationBaseClass::GuestWrite( uint16_t address, uint8_t dataByte );
		friend void    JynxZ80::Z80ImplementationBaseClass::GuestWriteIOSpace( uint16_t portNumber, uint8_t dataByte );
		friend uint8_t JynxZ80::Z80ImplementationBaseClass::GuestReadIOSpace( uint16_t portNumber );
		friend void    JynxZ80::Z80ImplementationBaseClass::OnAboutToBranch();

		// Interface called by the Z80 emulator:
		uint8_t  Z80_AddressRead(  uint16_t address );
		uint8_t  Z80_IOSpaceRead(  uint16_t portNumber );
		void     Z80_AddressWrite( uint16_t address,    uint8_t dataByte );
		void     Z80_IOSpaceWrite( uint16_t portNumber, uint8_t dataByte );
		void     Z80_OnAboutToBranch();

	private:

		void SetGuestHardwareToResetState();
		void InitialiseLYNX();
		void LoadROMS();
		void Load8KBChipFile( uint8_t *chipToLoad, LynxRoms::Enum romRequired );
		void RunThreadMainLoop();
		static void BootstrapRunThreadMainLoop( void *thisPointer );

		void UpdateAllOtherStateFromPortStateVariables();
		void UpdateBankSwitchFromPorts();
		void UpdateVideoSources();
		void ComposeHostBitmapPixelsForLynxScreenAddress( uint32_t addressOffset );
		void MarkWholeScreenInvalid();
		void RecompositeEntireLynxScreenOntoHostBitmap();

		uint8_t  ReadLynxKeyboard( uint16_t portNumber ) const;

		void CassetteMotorOn();
		void CassetteMotorOff();
		void CassetteWrite( uint8_t dataByte );
		uint8_t CassetteRead();

		void SpeakerWrite( uint8_t dataByte );

		void Serialise( ISerialiser &serialiser );

		uint64_t  GetElapsedZ80Cycles() const;

		std::string  GetLynxCommandLine() const;
		void ProcessLynxCommand();

	private:

		friend class EmulatorThreadInhibitor;

		IHostServicesForLynxEmulator * const _hostObject;     // Safe to read on ANY thread (not changed once constructed).
		Jynx::Thread _emulationThread;                        // The "EMULATOR thread".

		// Machine type being emulated
		LynxMachineType::Enum _machineType;

		// Tape mode (not serialised)
		LynxTapeMode::Enum  _tapeMode;
		std::shared_ptr<TapFileReader>  _currentReadTape;
		std::shared_ptr<TapFileWriter>  _currentWriteTape;
	
		// Screen area invalidation recording system (guest-side):
		enum { INV_ROWS = 32 };          // The vertical height of the screen is divided into this many rows for invalidation recording.
		volatile bool  _invalidateRow[INV_ROWS];  // Set true when an individual bit is drawn
		bool  _recompositeWholeScreen;   // Do we need to recomposite whole screen because of change in Lynx screen register that affects the whole display?

		// Main LYNX hardware state:
		JynxZ80::Z80      _processor;          // Z80 + Registers
		uint8_t  _devicePort;         // Port '0x80' see DEVICEPORT_ #defines
		uint8_t  _bankPort;           // Port '0xFFFF' see BANKPORT_ #defines
		uint8_t  _mc6845Select;       // 6845 register selector
		uint8_t  _mc6845Regs[32];     // 6845 register values store
		uint64_t _z80CycleCounter;    // Total cycle counter // TODO: serialise -- but only the cassette creation relies on it, and we don't & can't easily serialise the state of that.
		volatile uint8_t  _keyboard[16];       // Lynx keyboard ports (not persistent)
	
		// Host platform's preferred UTF8 end of line sequence:
		const std::string  _platformEndOfLineSequenceUTF8;

		volatile bool     _hearTapeSounds;    // Tape sound monitoring (ie: wire to speakers)
		volatile bool     _pauseMode;
		volatile bool     _pauseAfterTapLoadEnable;

		// Lynx text recording (snooping) on host:
		TextRecorder         _textRecorder;
		bool                 _inhibitTextRecorder;

		// Lynx text playback (to automate key presses on the Lynx).
		TextPlayer           _textPlayer;

		volatile bool        _watchingCommands;

		// Thread sync.  Allow MAIN thread to ask EMULATOR thread to suspend.
		volatile bool        _callWaiting;             // false normally, until MAIN thread call comes in.
		Jynx::Waitable       _callWaitingAcknowledge;  // false normally, until EMULATOR thread realises call is waiting, then sets this true, and awaits MAIN thread returning this to false.
		Jynx::Waitable       _resumeEmulatorThread;

		//
		// SOUND BUFFER (represents ONE Z80 period precisely)
		//

		SoundBufferWriter  _soundBufferWriter;
		SoundRecorder      _soundRecorder;

		//
		// BANK SWITCHING SELECTORS.
		//
		// - The following arrays select what the Z80 sees in the address space.
		//
		//   Index [0] is 0000 .. 1FFF
		//   Index [1] is 2000 .. 3FFF
		//   Index [2] is 4000 .. 5FFF
		//   Index [3] is 6000 .. 7FFF
		//   Index [4] is 8000 .. 9FFF
		//   Index [5] is A000 .. BFFF
		//   Index [6] is C000 .. DFFF
		//   Index [7] is E000 .. FFFF
		//
		// - An address-space READ will read from one CHIP,
		//   or return 0xFF if the array holds a NULL pointer.
		//
		// - An address-space WRITE will consider all three
		//   destinations, and write to the CHIPs referred to
		//   if the pointers are non-NULL.
		//
		// The code *never* lets the ROMs appear in the WRITE arrays!
		//

		ADDRESS_SPACE _addressSpaceREAD;
		ADDRESS_SPACE _addressSpaceWRITE1; // First place to write to
		ADDRESS_SPACE _addressSpaceWRITE2; // Additional place to write to if non-NULL
		ADDRESS_SPACE _addressSpaceWRITE3; // Additional place to write to if non-NULL

		//
		// Video image composition selectors
		//
		// These specify the source data for RED GREEN and BLUE image composition.
		// We support these being NULL to hide that colour from the display.
		//

		CHIP *_sourceVideoRED;
		CHIP *_sourceVideoGREEN;
		CHIP *_sourceVideoBLUE;

		//
		// The *Original* ROM images.
		//
		// - The snapshot format permits the user to fiddle the ROM images,
		//   these are the original ones, loaded by the constructor, then 
		//   used as sources when resetting the machine, or changing the type.
		//

		CHIP  _lynxROM_48_1;
		CHIP  _lynxROM_48_2;
		CHIP  _lynxROM_96_1;
		CHIP  _lynxROM_96_2;
		CHIP  _lynxROM_96_3;

		//
		// The Lynx's chip set  (8K ROMs/RAMs)
		//

		///
		/// Lynx's Bank #0
		///
		/// Bank 0 contains the ROMs and is a special case. 
		/// When enabled, this overlays everything else for READS 
		/// between 0000..5FFF and E000..FFFF.
		///

		CHIP            _lynxROM_0000;         // 0000 .. 1FFF
		CHIP            _lynxROM_2000;         // 2000 .. 3FFF
		CHIP            _lynxROM_4000;         // 4000 .. 5FFF   (LYNX 96K) Basic extension -- not on 48K machine.
											   // E000 .. FFFF   (Any extension rom -- not used on this emulator, default address decoding returns 0xFF).
									   
		///
		/// Lynx's bank #1   ("User RAM")
		///
		/// Bank 1 contains the user RAM (read / write)
		///

		CHIP            _lynxRAM_0000;    // 0000 .. 1FFF  (LYNX 96K only)
		CHIP            _lynxRAM_2000;    // 2000 .. 3FFF  (LYNX 96K only)
		CHIP            _lynxRAM_4000;    // 4000 .. 5FFF  (LYNX 96K only)
		CHIP            _lynxRAM_6000;    // 6000 .. 7FFF  (LYNX 48K and 96K)
		CHIP            _lynxRAM_8000;    // 8000 .. 9FFF  (LYNX 48K and 96K)
		CHIP            _lynxRAM_A000;    // A000 .. BFFF  (LYNX 96K only)
		CHIP            _lynxRAM_C000;    // C000 .. DFFF  (LYNX 96K only)
		CHIP            _lynxRAM_E000;    // E000 .. FFFF  (LYNX 96K only)

		///
		/// Lynx's bank #2   (Video RAM RED and BLUE banks)
		///

		CHIP            _lynxRedRAM;         // A000 .. BFFF
		CHIP            _lynxBlueRAM;        // C000 .. DFFF

		///
		/// Lynx's bank #3   (Video RAM ALTERNATE GREEN and GREEN)
		///

		CHIP            _lynxAltGreenRAM;    // A000 .. BFFF
		CHIP            _lynxGreenRAM;       // C000 .. DFFF

	};

} // end namespace Jynx
