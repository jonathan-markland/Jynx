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
#include "ILynxEmulator.h"
#include "IHostServicesForLynxEmulator.h"
#include "ISerialiser.h"
#include "Z80\JynxZ80.h"
#include "LynxHardware.h"
#include "SoundBufferWriter.h"
#include "SoundRecorder.h"
#include "TextRecorder.h"
#include "TextPlayer.h"
#include "TapFileReader.h"
#include "TapFileWriter.h"

namespace Jynx
{
	namespace LynxTapeMode
	{
		enum Enum
		{
			SavingPermitted, LoadingPermitted
		};
	}





	class LynxEmulatorGuest: public ILynxEmulator, public ITapeSpeedSupplier
	{
	public:

		// The Camputers Lynx hardware emulator.
		
		// - This creates a thread to run the emulation.
		// - This is a singleton class at the moment, see use of g_LynxEmulatorGuestSingleton.

		LynxEmulatorGuest( IHostServicesForLynxEmulator *hostObject );
			// Reminder: Constructor called on the CLIENT's thread.

		~LynxEmulatorGuest();
			// Reminder: Destructor called on the CLIENT's thread.

		void StartThread();
		void StopThread();

		// ILynxEmulator interface called by host, on the host's thread.
		// 
		virtual void AdvanceEmulation() override; // TODO: remove from interface
		virtual void NotifyKeyDown( int32_t guestKeyCode ) override;
		virtual void NotifyKeyUp( int32_t guestKeyCode ) override;
		virtual void NotifyAllKeysUp() override;
		virtual void SaveState( IFileOpener *fileOpener ) override;
		virtual void LoadState( IFileOpener *fileOpener ) override;
		virtual void ResetGuest( LynxMachineType::Enum machineType ) override;
		virtual uint32_t GetCyclesPerTimeslice() const override;
		virtual void SetCyclesPerTimeslice( uint32_t numCycles ) override;
		virtual void InsertBlankTape() override;
		virtual bool CanRewindTape() const override;
		virtual void RewindTape() override;
		virtual void LoadExistingTAPFile( IFileOpener *fileOpener ) override;
		virtual bool CanSaveTAPFile() const override;
		virtual void SaveTape( IFileOpener *fileOpener ) override;
		virtual bool IsTapeModified() const override;
		virtual void SetSoundBufferForNextPeriod( uint16_t *soundBuffer, size_t numSamples ) override;
		virtual void SetTapeSounds( bool tapeSounds ) override;
		virtual bool GetTapeSounds() const override;
		virtual void RecordSoundToFile( IFileOpener *fileOpener ) override;
		virtual void FinishRecordingSoundToFile() override;
		virtual bool IsRecordingSoundToFile() const override;
		virtual void RecordLynxTextToFile( IFileOpener *fileOpener ) override;
		virtual void FinishRecordingLynxTextToFile() override;
		virtual bool IsRecordingLynxTextToFile() const override;
		virtual bool GetLynxRemCommandExtensionsEnabled() const override;
		virtual void SetLynxRemCommandExtensionsEnabled( bool enable ) override;
		virtual void TypeInTextFromFile( IFileOpener *fileOpener ) override;

		// Implementing ITapeSpeedSupplier
		virtual uint32_t  GetLynxTapeSpeedBitsPerSecond() override;

	private:

		// This is where thread-safeness trumps the inadequate nature of "private"!
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

		void UpdateAllOtherStateFromPortStateVariables();
		void UpdateBankSwitchFromPorts();
		void UpdateVideoSources();
		void ComposeHostBitmapPixelsForLynxScreenAddress( uint32_t addressOffset );
		void MarkWholeScreenInvalid();
		void InvalidateDirtyRegionsOnHostScreen();
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

		IHostServicesForLynxEmulator * const _hostObject;  // Safe to read on ANY thread (not changed once constructed).

		// Machine type being emulated
		LynxMachineType::Enum _machineType;

		// Tape mode (not serialised)
		LynxTapeMode::Enum  _tapeMode;
		std::shared_ptr<TapFileReader>  _currentReadTape;
		std::shared_ptr<TapFileWriter>  _currentWriteTape;
	
		// Screen area invalidation recording system (guest-side):
		enum { INV_ROWS = 32 };          // The vertical height of the screen is divided into this many rows for invalidation recording.
		bool  _invalidateRow[INV_ROWS];  // Set true when an individual bit is drawn
		bool  _recompositeWholeScreen;   // Do we need to recomposite whole screen because of change in Lynx screen register that affects the whole display?

		// Main LYNX hardware state:
		JynxZ80::Z80      _processor;          // Z80 + Registers
		uint8_t  _devicePort;         // Port '0x80' see DEVICEPORT_ #defines
		uint8_t  _bankPort;           // Port '0xFFFF' see BANKPORT_ #defines
		uint8_t  _mc6845Select;       // 6845 register selector
		uint8_t  _mc6845Regs[32];     // 6845 register values store
		uint8_t  _keyboard[16];       // Lynx keyboard ports (not persistent)
		uint64_t _z80CycleCounter;    // Total cycle counter // TODO: serialise -- but only the cassette creation relies on it, and we don't & can't easily serialise the state of that.
	
		// Tape sound monitoring (ie: wire to speakers)
		volatile bool     _hearTapeSounds;

		// Lynx text recording (snooping) on host:
		TextRecorder         _textRecorder;
		bool                 _inhibitTextRecorder;

		// Lynx text playback (to automate key presses on the Lynx).
		TextPlayer           _textPlayer;

		volatile bool        _watchingCommands;

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
