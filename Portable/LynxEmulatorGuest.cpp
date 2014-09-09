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

#include <assert.h>
#include <string>
#include "LynxEmulatorGuest.h"
#include "ZeroInitialiseMemory.h"
#include "FileSerialiser.h"
#include "FileLoader.h"
#include "Z80\JynxZ80Disassembler.h"

// TEST const char *g_DemoText = "10 CLS\r15 REPEAT\r16 INK INK+1\r20 PRINT \"Hello \xF3\xF4\xF5\xF6\xF7\xf8\xf9 here! \";\r30 UNTIL FALSE\rRUN\r";

namespace Jynx
{
	class EmulatorThreadInhibitor
	{
	public:

		// This is a thread synchronisation mechanism used when the MAIN thread
		// calls the public interface functions of LynxEmulatorGuest.
		//
		// It blocks the MAIN thread until the EMULATOR thread finishes its current
		// burst of processing.  The EMULATOR thread then blocks and releases the
		// MAIN thread.  The MAIN thread is then free to use the member variables
		// of the LynxEmulatorGuest class, theoretically for as long as it wants.
		//
		// - However don't take too long or the sound will break up!
		//
		// The destructor releases the EMULATOR thread, and revokes the MAIN thread's
		// right to access the member variables of LynxEmulatorGuest.
		//
		// - This object must be allocated on the stack.
		// - Sometimes volatile variables are used instead of this.
		
		explicit EmulatorThreadInhibitor( LynxEmulatorGuest * );
		~EmulatorThreadInhibitor();

	private:

		LynxEmulatorGuest * _guestObjectToLockAndUnlock;

	};



	EmulatorThreadInhibitor::EmulatorThreadInhibitor( LynxEmulatorGuest *guestObjectToLockAndUnlock ) 
		: _guestObjectToLockAndUnlock(guestObjectToLockAndUnlock) 
	{
		// (Constructor called on MAIN thread).
		if( _guestObjectToLockAndUnlock->_callWaiting == false ) // re-entrancy detection.
		{
			_guestObjectToLockAndUnlock->_callWaitingAcknowledge.Reset();
			_guestObjectToLockAndUnlock->_callWaiting = true;
			_guestObjectToLockAndUnlock->_callWaitingAcknowledge.Wait();
			// EMULATOR thread is now suspended, waiting for actions in our destructor.
		}
		else
		{
			_guestObjectToLockAndUnlock = nullptr; // disable destructor because this was a re-entrant usage of this class.
		}
	}



	EmulatorThreadInhibitor::~EmulatorThreadInhibitor() 
	{
		// (Destructor called on MAIN thread).
		// EMULATOR thread is currently blocked on '_callWaitingAcknowledge' going 'false'.
		if( _guestObjectToLockAndUnlock != nullptr )
		{
			_guestObjectToLockAndUnlock->_callWaiting = false; // but SET THIS FIRST!
			_guestObjectToLockAndUnlock->_resumeEmulatorThread.Signal(); // release this SECOND!
		}
	}


}



namespace Jynx
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     EMULATOR "GUEST" CLASS -- this is the Lynx hardware
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	LynxEmulatorGuest  *g_LynxEmulatorGuestSingleton = nullptr;



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     EMULATOR "GUEST" CLASS -- this is the Lynx hardware
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	LynxEmulatorGuest::LynxEmulatorGuest( IHostServicesForLynxEmulator *hostObject, uint16_t *soundBuffer, size_t numSamples, LynxMachineType::Enum initialMachineType, const char *platformEndOfLineSequenceUTF8 )
		: _z80CycleCounter(0)
		, _tapeMode( LynxTapeMode::SavingPermitted )
		, _hearTapeSounds(false)
		, _watchingCommands(false)
		, _inhibitTextRecorder(false)
		, _hostObject(hostObject)
		, _machineType(initialMachineType)
		, _platformEndOfLineSequenceUTF8(platformEndOfLineSequenceUTF8)
		, _callWaiting(false)
		, _pauseMode(false)
		, _pauseAfterTapLoadEnable(false)
		, _canEnableSpeedMaxModeWhenUsingCassette(true)  // default true for the HyperSpin guys I think.
		, _canEnableSpeedMaxModeWhenInBetweenConsoleCommands(false)
		, _speedMaxModeBecauseUserWantsItPermanently(false)
		, _speedMaxModeBecauseOfCassette(false)
		, _speedMaxModeBecauseWeAreInBetweenConsoleCommands(false)
		, _level(0)
		, _colourSet( LynxColourSet::NormalRGB )
	{
		// (Reminder - Called on the client thread).

		assert( g_LynxEmulatorGuestSingleton == nullptr );  // should only be one instance of this class.
		g_LynxEmulatorGuestSingleton = this; // establish this object as the singleton that the Z80 will call.

		_currentReadTape  = std::make_shared<TapFileReader>( this );
		_currentWriteTape = std::make_shared<TapFileWriter>();

		_processor.SetTimesliceLength( LynxZ80Cycles::At100 ); // 4.00 mhz
		_soundBufferWriter.SetSoundBuffer( soundBuffer, numSamples );

		LoadROMS();
		InitialiseLYNX();
		_emulationThread.CreateAndRun( &LynxEmulatorGuest::BootstrapRunThreadMainLoop, this );
	}



	LynxEmulatorGuest::~LynxEmulatorGuest()
	{
		// (Called on main thread)
		_emulationThread.RequestTermination();
		_emulationThread.WaitForTermination();
	}



	void LynxEmulatorGuest::LoadROMS()
	{
		// (Reminder - Called on the client thread).

		Load8KBChipFile( _lynxROM_48_1, LynxRoms::Lynx48_1 );
		Load8KBChipFile( _lynxROM_48_2, LynxRoms::Lynx48_2 );
		Load8KBChipFile( _lynxROM_96_1, LynxRoms::Lynx96_1 );
		Load8KBChipFile( _lynxROM_96_2, LynxRoms::Lynx96_2 );
		Load8KBChipFile( _lynxROM_96_3, LynxRoms::Lynx96_3 );
	}



	void LynxEmulatorGuest::Load8KBChipFile( uint8_t *chipToLoad, LynxRoms::Enum romRequired )
	{
		// (Reminder - Called on the client thread).

		try
		{
			std::ifstream  inStream;
			_hostObject->OpenChipFileStream_OnMainThread( inStream, std::ios::in | std::ios::binary | std::ios::ate, romRequired );
	
			if( inStream.is_open() )
			{
				uint64_t fileSize = inStream.tellg();
				if( fileSize == 8192 )
				{
					inStream.seekg (0, std::ios::beg);
					inStream.read( (char *) chipToLoad, 8192 );
					inStream.close();
					return;
				}
				inStream.close();
			}
		}
		catch( const std::ifstream::failure & )
		{
			// Drop to re-raise below, because otherwise a horrible message is raised to the user.
		}

		throw std::exception( "One or more of the ROM files are missing.  Please refer to the readme.htm file for information." );  // very base class std::exception should terminate program.
	}



	void LynxEmulatorGuest::SetGuestHardwareToResetState()
	{
		_z80CycleCounter = 0;  // may as well reset this

		//
		// Screen invalidation
		//

		_recompositeWholeScreen = false;
		InitialiseAllArrayElementsVolatile( _invalidateRow, true );

		//
		// Init Z80
		//

		_processor.Reset();
	
		//
		// Init Ports
		//

		_devicePort   = DEVICEPORT_INITIALISATION_VALUE;
		_bankPort     = BANKPORT_INITIALISATION_VALUE;
		_mc6845Select = 0;
		ZeroInitialiseMemory( _mc6845Regs );
		InitialiseAllArrayElementsVolatile( _keyboard, (uint8_t) 0xFF );  // -ve logic
		InitialiseAllArrayElements( _keyboardSweepDetect, false ); 
		_level = 0;

		//
		// Initialise bank switching selectors.
		//

		InitialiseAllArrayElements( _addressSpaceREAD,   (CHIP *) nullptr );
		InitialiseAllArrayElements( _addressSpaceWRITE1, (CHIP *) nullptr );
		InitialiseAllArrayElements( _addressSpaceWRITE2, (CHIP *) nullptr );
		InitialiseAllArrayElements( _addressSpaceWRITE3, (CHIP *) nullptr );

		//
		// Video image composition selectors
		//

		UpdatePalette();
		_sourceVideoRED   = nullptr;
		_sourceVideoGREEN = nullptr;
		_sourceVideoBLUE  = nullptr;

		//
		// The Lynx's chip set  (8K ROMs/RAMs)
		//

		ZeroInitialiseMemory( _lynxROM_0000 );
		ZeroInitialiseMemory( _lynxROM_2000 );
		ZeroInitialiseMemory( _lynxROM_4000 );

		ZeroInitialiseMemory( _lynxRAM_0000 );
		ZeroInitialiseMemory( _lynxRAM_2000 );
		ZeroInitialiseMemory( _lynxRAM_4000 );
		ZeroInitialiseMemory( _lynxRAM_6000 );
		ZeroInitialiseMemory( _lynxRAM_8000 );
		ZeroInitialiseMemory( _lynxRAM_A000 );
		ZeroInitialiseMemory( _lynxRAM_C000 );
		ZeroInitialiseMemory( _lynxRAM_E000 );

		ZeroInitialiseMemory( _lynxRedRAM );
		ZeroInitialiseMemory( _lynxBlueRAM );

		ZeroInitialiseMemory( _lynxAltGreenRAM );
		ZeroInitialiseMemory( _lynxGreenRAM );

		//
		// Copy the appropriate ROMs in according to the _machineType
		//

		if( _machineType == LynxMachineType::LYNX_48K )
		{
			CopyArrayMemory( _lynxROM_0000, _lynxROM_48_1 );
			CopyArrayMemory( _lynxROM_2000, _lynxROM_48_2 );
		}
		else if( _machineType == LynxMachineType::LYNX_96K )
		{
			CopyArrayMemory( _lynxROM_0000, _lynxROM_96_1 );
			CopyArrayMemory( _lynxROM_2000, _lynxROM_96_2 );
			CopyArrayMemory( _lynxROM_4000, _lynxROM_96_3 );
		}
		else assert(false);
	}





	void LynxEmulatorGuest::InitialiseLYNX()
	{
		SetGuestHardwareToResetState();
		UpdateAllOtherStateFromPortStateVariables();
		MarkWholeScreenInvalid();
		_recompositeWholeScreen = true;
	}




	uint64_t  LynxEmulatorGuest::GetElapsedZ80Cycles() const
	{
		return _z80CycleCounter + (_processor.GetTimesliceLength() - _processor.GetRemainingCycles());
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     LYNX BANK SWITCH HANDLING
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



	void ZeroBANK( ADDRESS_SPACE &addressSpace )
	{
		InitialiseAllArrayElements( addressSpace, (CHIP *) nullptr );
	}



	void MapReflections( ADDRESS_SPACE &addressSpace, CHIP *chipOne, CHIP *chipTwo )
	{
		addressSpace[0] = chipOne;
		addressSpace[1] = chipOne;
		addressSpace[2] = chipTwo;
		addressSpace[3] = chipTwo;
		addressSpace[4] = chipOne;
		addressSpace[5] = chipOne;
		addressSpace[6] = chipTwo;
		addressSpace[7] = chipTwo;
	}




	void LynxEmulatorGuest::UpdateAllOtherStateFromPortStateVariables()
	{
		UpdateVideoSources();
		UpdateBankSwitchFromPorts();
	}



	void LynxEmulatorGuest::UpdateVideoSources()
	{
		_sourceVideoGREEN = (_devicePort & DEVICEPORT_USE_ALT_GREEN) ? &_lynxAltGreenRAM : &_lynxGreenRAM;
		_sourceVideoBLUE  = &_lynxBlueRAM;
		_sourceVideoRED   = &_lynxRedRAM;
	}



	void LynxEmulatorGuest::UpdateBankSwitchFromPorts()
	{
		//
		// Updates the following bank switching variables from the state of _VideoPort and _bankPort:
		//
		// _addressSpaceREAD
		// _addressSpaceWRITE1
		// _addressSpaceWRITE2
		// _addressSpaceWRITE3
		//

		UpdateVideoSources();

		bool bCasEnBank2 = (_devicePort & DEVICEPORT_NOT_CASEN_BANK2) == 0;
		bool bCasEnBank3 = (_devicePort & DEVICEPORT_NOT_CASEN_BANK3) == 0;

		//
		// ADDRESS SPACE - WRITING
		//

		// Is Bank 1 going to be enabled for writes?

		if( (_bankPort & BANKPORT_NOT_WREN1) == 0 )
		{
			if( _machineType == LynxMachineType::LYNX_48K )
			{
				MapReflections( _addressSpaceWRITE1, &_lynxRAM_8000, &_lynxRAM_6000 );
			}
			else if( _machineType == LynxMachineType::LYNX_96K )
			{
				_addressSpaceWRITE1[0] = &_lynxRAM_0000;
				_addressSpaceWRITE1[1] = &_lynxRAM_2000;
				_addressSpaceWRITE1[2] = &_lynxRAM_4000;
				_addressSpaceWRITE1[3] = &_lynxRAM_6000;
				_addressSpaceWRITE1[4] = &_lynxRAM_8000;
				_addressSpaceWRITE1[5] = &_lynxRAM_A000;
				_addressSpaceWRITE1[6] = &_lynxRAM_C000;
				_addressSpaceWRITE1[7] = &_lynxRAM_E000;
			}
			else assert(false);
		}
		else
		{
			MapReflections( _addressSpaceWRITE1, nullptr, nullptr );
		}

		// Is Bank 2 going to be enabled for writes?

		if( _bankPort & BANKPORT_WREN2 && bCasEnBank2 )
		{
			MapReflections( _addressSpaceWRITE2, &_lynxBlueRAM, &_lynxRedRAM );
		}
		else
		{
			MapReflections( _addressSpaceWRITE2, nullptr, nullptr );
		}

		// Is Bank 3 going to be enabled for writes?

		if( _bankPort & BANKPORT_WREN3 && bCasEnBank3 )
		{
			MapReflections( _addressSpaceWRITE3, &_lynxAltGreenRAM, &_lynxGreenRAM );
		}
		else
		{
			MapReflections( _addressSpaceWRITE3, nullptr, nullptr );
		}

		//
		// ADDRESS SPACE - READING
		//

		uint8_t readCount = 0;

		if( (_bankPort & BANKPORT_NOT_RDEN1) == 0 )
		{
			// Bank1 is switched IN.

			if( _machineType == LynxMachineType::LYNX_48K )
			{
				MapReflections( _addressSpaceREAD, &_lynxRAM_8000, &_lynxRAM_6000 );
			}
			else if( _machineType == LynxMachineType::LYNX_96K )
			{
				_addressSpaceREAD[0] = &_lynxRAM_0000;
				_addressSpaceREAD[1] = &_lynxRAM_2000;
				_addressSpaceREAD[2] = &_lynxRAM_4000;
				_addressSpaceREAD[3] = &_lynxRAM_6000;
				_addressSpaceREAD[4] = &_lynxRAM_8000;
				_addressSpaceREAD[5] = &_lynxRAM_A000;
				_addressSpaceREAD[6] = &_lynxRAM_C000;
				_addressSpaceREAD[7] = &_lynxRAM_E000;
			}
			else assert(false);

			++readCount;
		}
	
		if( _bankPort & BANKPORT_RDEN2_3 )
		{
			if( bCasEnBank2 )
			{
				// Bank 2 is switched in
				MapReflections( _addressSpaceREAD, &_lynxBlueRAM, &_lynxRedRAM );
				++readCount;
			}
			if( bCasEnBank3 )
			{
				// Bank 3 is switched in
				MapReflections( _addressSpaceREAD, &_lynxAltGreenRAM, &_lynxGreenRAM );
				++readCount;
			}
		}

		if( readCount == 0 )
		{
			ZeroBANK( _addressSpaceREAD );
		}

		//
		// ROM -- If enabled, this appears in the READable address-space, overlaying the RAM.
		//

		if( (_bankPort & BANKPORT_NOT_RDEN0) == 0 )
		{
			// ROM is enabled for reading
		
			_addressSpaceREAD[0] = &_lynxROM_0000;
			_addressSpaceREAD[1] = &_lynxROM_2000;

			if( _machineType == LynxMachineType::LYNX_48K )
			{
				_addressSpaceREAD[2] = nullptr; // extended ROM not present, MUST return 0xFF for the region.
			}
			else if( _machineType == LynxMachineType::LYNX_96K )
			{
				_addressSpaceREAD[2] = &_lynxROM_4000;  // The 96K machine has an extended ROM.
			}
			else assert(false);
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     SCREEN
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::UpdatePalette()
	{
		// Fill the palette with RGBA values.
		// Then ask the host to translate the palette to target values.
		// We will quote the target values through the pixel rendering main interface.

		if( _colourSet == LynxColourSet::NormalRGB )
		{
			_colourPalette[0] = 0x000000;
			_colourPalette[1] = 0xFF0000;
			_colourPalette[2] = 0x0000FF;
			_colourPalette[3] = 0xFF00FF;
			_colourPalette[4] = 0x00FF00;
			_colourPalette[5] = 0xFFFF00;
			_colourPalette[6] = 0x00FFFF;
			_colourPalette[7] = 0xFFFFFF;
		}
		else if( _colourSet == LynxColourSet::BlackAndWhiteTV )
		{
			_colourPalette[0] = 0x000000;
			_colourPalette[1] = 0x444444;
			_colourPalette[2] = 0x666666;
			_colourPalette[3] = 0x888888;
			_colourPalette[4] = 0xAAAAAA;
			_colourPalette[5] = 0xCCCCCC;
			_colourPalette[6] = 0xDDDDDD;
			_colourPalette[7] = 0xFFFFFF;
		}
		else if( _colourSet == LynxColourSet::GreenScreenMonitor )
		{
			_colourPalette[0] = 0x000000;
			_colourPalette[1] = 0x004400;
			_colourPalette[2] = 0x006600;
			_colourPalette[3] = 0x008800;
			_colourPalette[4] = 0x00AA00;
			_colourPalette[5] = 0x00CC00;
			_colourPalette[6] = 0x00DD00;
			_colourPalette[7] = 0x00FF00;
		}
		else if( _colourSet == LynxColourSet::GreenOnly )
		{
			_colourPalette[0] = 0x000000;
			_colourPalette[1] = 0x000000;
			_colourPalette[2] = 0x000000;
			_colourPalette[3] = 0x000000;
			_colourPalette[4] = 0x00FF00;
			_colourPalette[5] = 0x00FF00;
			_colourPalette[6] = 0x00FF00;
			_colourPalette[7] = 0x00FF00;
		}
		else if( _colourSet == LynxColourSet::Level9 )
		{
			_colourPalette[0] = 0x000000;
			_colourPalette[1] = 0x200000;
			_colourPalette[2] = 0x000020;
			_colourPalette[3] = 0x200020;
			_colourPalette[4] = 0xCCFFFF;
			_colourPalette[5] = 0xDDFFFF;
			_colourPalette[6] = 0xEEFFFF;
			_colourPalette[7] = 0xFFFFFF;
		}

		// Establish default translations:
		for( uint32_t  i=0; i<8; i++ )
		{
			_translatedColourPalette[i] = _colourPalette[i];
		}

		// Allow hosts to translate the values to whatever they desire to use
		// directly as pixel values / indices:
		_hostObject->TranslateRGBXColourPaletteToHostValues( _colourPalette, _translatedColourPalette );
	}

	void LynxEmulatorGuest::ComposeHostBitmapPixelsForLynxScreenAddress( uint32_t addressOffset )
	{
		// (Called on Z80 thread).
		// Note: Volatile variable access to _invalidateRow[] -- no sync needed.

		// The Lynx's screen memory has just been written at address offset 'addressOffset'.
		// We compose the framebuffer equivalent from the sources, which can be NULL.

		// TODO: Consider: This overall design could cause multiple re-compositions as the banks are written to
		// *WITHIN* the time periods between repaints by the Host.  Just before invalidation
		// we could re-compose the invalid region.  If we did this, it would be desireable to
		// record HIGH RESOLUTION invalid regions, in case just a small section has changed.

		assert( addressOffset < 0x2000 );

		uint32_t  lynxRedByte   = (*_sourceVideoRED)[addressOffset];
		uint32_t  lynxGreenByte = (*_sourceVideoGREEN)[addressOffset];
		uint32_t  lynxBlueByte  = (*_sourceVideoBLUE)[addressOffset];

		uint32_t   pixelDataRGBA[8];

		auto pixelAddress = &pixelDataRGBA[0];
		uint8_t pixelMask = 0x80;
		while( pixelMask != 0 )
		{
			auto lynxInkNumber = 0;
			if( lynxBlueByte  & pixelMask )  lynxInkNumber |= 1;
			if( lynxRedByte   & pixelMask )  lynxInkNumber |= 2;
			if( lynxGreenByte & pixelMask )  lynxInkNumber |= 4;
			*pixelAddress = _translatedColourPalette[lynxInkNumber];
			pixelMask >>= 1;
			++pixelAddress;
		}

		_hostObject->PaintPixelsOnHostBitmap_OnEmulatorThread( addressOffset, pixelDataRGBA );

		assert( (addressOffset >> 8) < INV_ROWS );
		_invalidateRow[addressOffset >> 8] = true; // mark a row invalid
	}



	void LynxEmulatorGuest::MarkWholeScreenInvalid()
	{
		// (Called on Z80 thread and Main thread).
		// Volatile variable access -- no sync needed.

		InitialiseAllArrayElementsVolatile( _invalidateRow, true );
	}



	void LynxEmulatorGuest::RecompositeEntireLynxScreenOntoHostBitmap()
	{
		// (Called on Z80 thread).

		_recompositeWholeScreen = false;

		for( uint32_t guestScreenAddressOffset = 0x0000; guestScreenAddressOffset < 0x2000; ++guestScreenAddressOffset )
		{
			ComposeHostBitmapPixelsForLynxScreenAddress( guestScreenAddressOffset );
		}
	}



	void LynxEmulatorGuest::CallMeBackToInvalidateRegions()
	{
		// Called on MAIN thread.  (NOT Z80 thread!)
		// Volatile variable access to _invalidateRow[] -- no sync needed.

		for( uint32_t i=0; i < INV_ROWS; ++i )
		{
			if( _invalidateRow[i] == true )
			{
				int32_t x = 0;
				int32_t y = (i * 8);
				_hostObject->InvalidateAreaOfGuestScreen_OnMainThread( x, y, x+LYNX_FRAMEBUF_WIDTH, y+8 );
				_invalidateRow[i] = false;
			}
		}
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//      LYNX KEYBOARD
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


	uint8_t  LynxEmulatorGuest::ReadLynxKeyboard( uint16_t portNumber )
	{
		// Bits A11..A8 are the index into _keyboard[].
		// Bits A15..A12 are not decoded.
		assert( (portNumber & DEVICEPORT_DECODING_MASK) == 0x80 ); // Should have been checked by the caller.

		portNumber &= DEVICEPORT_KEYBOARD_DECODING_MASK;  // The Lynx doesn't decode all the bits.

		//
		// Handle port reading when text player active:
		//

		if( _textPlayer.HasText() )  
		{
			// When active, the _textPlayer disables direct keyboard reading, so we lie and say "result 0xFF".
			// If the user allows, we also assert speed-max mode with the text player:
			if( _canEnableSpeedMaxModeWhenInBetweenConsoleCommands == true )
			{
				_speedMaxModeBecauseWeAreInBetweenConsoleCommands = true;
			}
			return 0xFF; 
		}

		//
		// Handle normal port reading:
		//

		uint32_t portIndex = portNumber >> 8;
		if( portIndex < 10 )
		{
			auto result = _keyboard[portIndex];   // NB: volatile read

			auto cleanSweepSeen = DoKeyboardCleanSweepDetection( portIndex );
			if( cleanSweepSeen )
			{
				// The Lynx has just done a clean-sweep read of the keyboard ports
				// resulting in a NO KEYS HELD DOWN result.  This our signal that
				// a command-prompt has returned, so let's disable speed max mode:
				_speedMaxModeBecauseWeAreInBetweenConsoleCommands = false;
			}

			// If the user allows, then enable speed-max mode when RETURN is pressed
			// (as we assume this is entering a console command for which the user wants
			// host-speed performance).
			if( _canEnableSpeedMaxModeWhenInBetweenConsoleCommands == true )
			{
				if( portNumber == 0x0980 && ((_keyboard[9] & 0x08) == 0) )  // RETURN key DOWN?
				{
					_speedMaxModeBecauseWeAreInBetweenConsoleCommands = true;
				}
			}

			// Return the keyboard port byte:
			return result;
		}

		return 0xFF;  // Port out of range of keyboard.
	}



	bool LynxEmulatorGuest::DoKeyboardCleanSweepDetection( uint8_t portIndex )
	{
		assert( portIndex < 10 );

		if( _keyboardSweepDetect[portIndex] == true )
		{
			// Oops this isn't a clean sweep!  This port has already been read before we saw a clean sweep.  Start over:
			InitialiseAllArrayElements( _keyboardSweepDetect, false );
			_keyboardSweepDetect[portIndex] = true;
			return false;
		}

		// Looking clean so far:
		_keyboardSweepDetect[portIndex] = true;
		for( auto flag : _keyboardSweepDetect )
		{
			if( flag == false )
			{
				return false; // no clean sweep seen until all values are 'true'.
			}
		}

		return true;
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     EMULATING:  ADDRESS SPACE READ / WRITE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	uint8_t  LynxEmulatorGuest::Z80_AddressRead( uint16_t address )
	{
		// The Z80 calls this to read a byte from the address space.

		auto chipIndex = (address >> 13) & 7;
		auto chipToReadFrom = _addressSpaceREAD[chipIndex];

		if( chipToReadFrom != nullptr )
		{
			auto dataByte = (*chipToReadFrom)[address & 0x1FFF];
			return dataByte;
		}
		else
		{
			return 0xFF;  // Nobody decoded this address.
		}
	}



	void   LynxEmulatorGuest::Z80_AddressWrite( uint16_t address, uint8_t dataByte )
	{
		// The Z80 calls this to write a byte to the address space.
		// Writes can hit multiple chips.

		auto chipIndex = (address >> 13) & 7;
		address &= 0x1FFF;
		bool updateHostBitmap = false;

		// Is bank 1 enabled to decode this?

		auto pChipInBank1 = _addressSpaceWRITE1[chipIndex];
		if( pChipInBank1 )
		{
			(*pChipInBank1)[address] = dataByte;
		}

		// Is bank 2 enabled to decode this?

		auto pChipInBank2 = _addressSpaceWRITE2[chipIndex];
		if( pChipInBank2 )
		{
			auto ramLocation = (*pChipInBank2) + address;
			if( *ramLocation != dataByte ) // speed optimisation
			{
				*ramLocation = dataByte;
				updateHostBitmap = true;
			}
		}

		// Is bank 3 enabled to decode this?

		auto pChipInBank3 = _addressSpaceWRITE3[chipIndex];
		if( pChipInBank3 )
		{
			auto ramLocation = (*pChipInBank3) +address;
			if( *ramLocation != dataByte ) // speed optimisation
			{
				*ramLocation = dataByte;
				updateHostBitmap = true;
			}
		}

		// Do we compose host screen pixels for this write?

		if( updateHostBitmap )
		{
			ComposeHostBitmapPixelsForLynxScreenAddress( address );   // Speed optimisation:  We can call this ONCE ONLY.
		}
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     EMULATING:  I/O SPACE READ / WRITE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void   LynxEmulatorGuest::Z80_IOSpaceWrite( uint16_t portNumber, uint8_t dataByte )
	{
		// The Z80 calls this to write a uint8_t to the I/O space

		//
		// AUDIO-VISUAL CONTROL PORT
		//

		if( (portNumber & DEVICEPORT_DECODING_MASK) == 0x80 )
		{
			auto oldSetting = _devicePort;

			dataByte &= 0x3F; // Bit 7 kept zero according to spec, and let's wire the mono-stable (bit 6) to 0

			if( oldSetting != dataByte ) // optimise by ignoring repeated writes of same value.
			{
				// Update our record of the port status (which may simultaneously change several things):
				// Must do this first, so any routines we call out to (below) see the new port settings:
				_devicePort = dataByte;

				// Use XOR to detect *change* in the DEVICEPORT_USE_ALT_GREEN bit:
				if( (oldSetting ^ dataByte) & DEVICEPORT_USE_ALT_GREEN )
				{
					// We are changing the GREEN / ALTERNATIVE GREEN selector
					// so a re-construction and invalidate of the display is needed.
					// We don't do it right now, we flag for it at the next frame:
					_recompositeWholeScreen = true;
					UpdateVideoSources();
				}

				// Use XOR to detect *change* in either/both of the DEVICEPORT_NOT_CASEN_BANK3 or DEVICEPORT_NOT_CASEN_BANK2 bits:
				if( (oldSetting ^ dataByte) & (DEVICEPORT_NOT_CASEN_BANK3 | DEVICEPORT_NOT_CASEN_BANK2) )
				{
					// Bits "NOT CASEN BANK3" or "NOT CASEN BANK2" have changed.
					// This affects bank switching:
					UpdateBankSwitchFromPorts();
				}
			
				// Use XOR to detect *change* in CASSETTE MOTOR control bit:
				if( (oldSetting ^ dataByte) & DEVICEPORT_CASSETTE_MOTOR )
				{
					if( dataByte & DEVICEPORT_CASSETTE_MOTOR )
					{
						CassetteMotorOn();
					}
					else
					{
						CassetteMotorOff();
					}
				}
			}
		}

		//
		// BANK SWITCH ( "Port 80" )
		//

		else if( (portNumber & 0x207F) == 0x207F )
		{
			// It's the so-called "PORT FFFF":
			if( _bankPort != dataByte ) // optimise away repeated writes of same value
			{
				_bankPort = dataByte;
				UpdateBankSwitchFromPorts();
			}
		}

		//
		// 6845
		//

		else if( (portNumber & 0xC7) == 0x86 )
		{
			// 6845 display generator
			_mc6845Select = dataByte;   // TODO: 6845 doesn't have 256 registers.  Are top 3 bits ignored?
		}
		else if( (portNumber & 0xC7) == 0x87 )
		{
			// 6845 display generator
			_mc6845Regs[ _mc6845Select & 0x1F ] = dataByte;
		}

		//
		// SERIAL PORT
		//

		/*else if( (portNumber & DEVICEPORT_DECODING_MASK) == 0x82 )
		{
		}*/


		//
		// CASSETTE / LOUDSPEAKER - 6 bit D/A
		//

		else if( (portNumber & DEVICEPORT_DECODING_MASK) == 0x84 )
		{
			// If the casette motor is enabled, we are saving to tape:
			// The Lynx has a 6-bit D-A converter.
		
			// Bits 5..0 contain the level:
			auto level = dataByte & 0x3F;

			_level = level;

			if( _devicePort & DEVICEPORT_CASSETTE_MOTOR )
			{
				CassetteWrite( level );
				if( _hearTapeSounds )
				{
					// Listen to tape saving (quieten it a bit!):
					SpeakerWrite( level >> 2 ); 
				}
			}
			else if( _devicePort & DEVICEPORT_SPEAKER )
			{
				// Write to speaker.
				SpeakerWrite( level ); 
			}
		}
	}





	bool LynxEmulatorGuest::IsTapeInOperation() const
	{
		return ( _level != 0  // <-- added for crazy things that Level 9 do!  (Turn tape motor ON all the time when keyboard scanning).
				&& _devicePort & DEVICEPORT_CASSETTE_MOTOR );
	}





	uint8_t  LynxEmulatorGuest::Z80_IOSpaceRead( uint16_t portNumber )
	{
		if( (portNumber & DEVICEPORT_DECODING_MASK) == 0x80 )
		{
			// If the cassette motor is enabled, we are loading from tape.
			// Bit 0 is a "level sensor" which detects whether the level is below or above the middle.

			// Since the keyboard shares the same port as the cassette we must include the key states
			// -- although I have insufficient documentation on this!  I deduced when the supply the
			// cassette value in bit 0 of port 0x0080

			if( IsTapeInOperation() )
			{
				if( (portNumber & 0xFC6) == 0x0080 ) // <-- Mask per Lynx User Magazine Issue 1.  The lynx appears to only read from this port specifically, when reading tapes.
				{
					if( _canEnableSpeedMaxModeWhenUsingCassette )
					{
						_speedMaxModeBecauseOfCassette = true;
					}

					// (It seems cassette loading terminates immediately unless the key information is 
					// returned here.  Fixing the top 7 bits at "0"s wasn't a good idea!).
					auto cassetteBit0 = CassetteRead();
					if( _hearTapeSounds )
					{
						// Listen to tape loading (quieten it a bit):
						SpeakerWrite( cassetteBit0 << 3 ); 
					}
					return ReadLynxKeyboard(portNumber) & 0xFE | cassetteBit0;
				}
			}

			// Read of keyboard only (cassette motor not active):
			return ReadLynxKeyboard(portNumber);
		}
		else if( (portNumber & CRTCPORT_DECODING_MASK) == 0x86 )
		{
			// 6845 display generator
			return _mc6845Select; // spec says this isn't readable, but heck
		}
		else if( (portNumber & CRTCPORT_DECODING_MASK) == 0x87 )
		{
			// 6845 display generator
			return _mc6845Regs[ _mc6845Select & 0x1F ]; // spec says this isn't readable, but heck
		}
		else if( (portNumber & BANKPORT_DECODING_MASK) == BANKPORT_DECODING_MASK )
		{
			// It's the so-called "PORT FFFF":
			return _bankPort; // I think the spec says this isn't readable either.  This doesn't seem to get executed.
		}

		return 0xFF;   // Nobody decoded this I/O space address.
	}








	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     CASSETTE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::CassetteMotorOn()
	{
		// We shall examine the TAPE speed every time the Lynx switches the cassette motor on.
		// When the Lynx LOADs, we can then serve data at the speed it is expecting.  Of course,
		// this would never have happened on a real system!  The lynx would have ignored files
		// saved at unexpected speeds.  (See Lynx BASIC "TAPE" command).
		_currentReadTape->CassetteMotorOn();
		_currentWriteTape->NotifyCassetteMotorOn();
	}



	void LynxEmulatorGuest::CassetteMotorOff()
	{
		_speedMaxModeBecauseOfCassette = false;
		_currentReadTape->CassetteMotorOff();
		_currentWriteTape->NotifyCassetteMotorOff();
		_hostObject->NotifyOutputTapeAvailbilityChanged_OnAnyThread();
	}



	uint8_t LynxEmulatorGuest::CassetteRead()
	{
		if( _tapeMode == LynxTapeMode::LoadingPermitted )
		{
			// Reminder: Rewinding the _currentReadTape will cause it to resync the cycles on the first "read".
			auto elapsedCycles = GetElapsedZ80Cycles();
			return _currentReadTape->GetSampleAtTime( elapsedCycles );
		}
		return 0;
	}



	void LynxEmulatorGuest::CassetteWrite( uint8_t dataByte )
	{
		if( _tapeMode == LynxTapeMode::SavingPermitted )
		{
			// dataByte is the signal level 0..63
			_currentWriteTape->WriteSignal( dataByte, GetElapsedZ80Cycles() );
		}
	}



	bool LynxEmulatorGuest::GetPauseAfterTapLoadEnable()
	{
		return _pauseAfterTapLoadEnable;
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     LYNX "TAPE" speed conversion
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	uint32_t  TranslateCoarseAndFineToBitsPerSecond( uint8_t coarseSpeed, uint8_t fineSpeed )
	{
		if( coarseSpeed == 1 && fineSpeed == 1 ) return 900;  // TAPE 1
		if( coarseSpeed == 2 && fineSpeed == 4 ) return 1200; // TAPE 2
		if( coarseSpeed == 2 && fineSpeed == 2 ) return 1500; // TAPE 3
		if( coarseSpeed == 2 && fineSpeed == 1 ) return 1800; // TAPE 4
		if( coarseSpeed == 3 && fineSpeed == 2 ) return 2100; // TAPE 5
		return 600;  // TAPE 0 (default)
	}



	uint32_t  LynxEmulatorGuest::GetLynxTapeSpeedBitsPerSecond()
	{
		// (Called on the Z80 thread - no synchronization issues here)

		// The Lynx never did speed detection on reading tapes/
		// Let's give the Lynx the tape at the speed it wants.
		// 0x692D "COARSE" and 0x692E "FINE" contain two values that set cassette speed
		// which correspond to the BASIC "TAPE" speed-setting command:

		auto coarseSpeed = _lynxRAM_6000[0x29D];
		auto fineSpeed   = _lynxRAM_6000[0x29E];
		return TranslateCoarseAndFineToBitsPerSecond( coarseSpeed, fineSpeed );
	}



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    WRITE TO SPEAKER
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::SpeakerWrite( uint8_t dataByte )
	{
		_soundBufferWriter.WriteSample( dataByte, _processor.GetTimesliceLength(), _processor.GetRemainingCycles() );
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     SERIALISATION
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::Serialise( ISerialiser &serialiser )
	{
		// (Reminder - this does not need to support the cassette)

		int32_t version = 1; // For Writing
		int32_t machine = (_machineType == LynxMachineType::LYNX_48K) ? 48 : 96;  // For Writing

		serialiser.OpenTag( "camputers_lynx" );

		serialiser.Field( "version",         version );
		// TODO:  Check version
		assert( version == 1 );

		serialiser.Field( "machine",         machine );
		_machineType = (machine == 48) ? LynxMachineType::LYNX_48K : LynxMachineType::LYNX_96K; // for Reading

		//
		// Z80
		//

		auto cpuState = _processor.GetSerialisableVariables(); // Only really needed for writing.  Benign for reading.

		serialiser.Field( "Z80_AF",          cpuState._AF );
		serialiser.Field( "Z80_AF1",         cpuState._alternateAF );
		serialiser.Field( "Z80_BC",          cpuState._BC );
		serialiser.Field( "Z80_BC1",         cpuState._alternateBC );
		serialiser.Field( "Z80_DE",          cpuState._DE );
		serialiser.Field( "Z80_DE1",         cpuState._alternateDE );
		serialiser.Field( "Z80_HL",          cpuState._HL );
		serialiser.Field( "Z80_HL1",         cpuState._alternateHL );
		serialiser.Field( "Z80_I",           cpuState._I );
		serialiser.Field( "Z80_IX",          cpuState._IX );
		serialiser.Field( "Z80_IY",          cpuState._IY );
		serialiser.Field( "Z80_PC",          cpuState._programCounter );
		serialiser.Field( "Z80_SP",          cpuState._stackPointer );
		serialiser.Field( "Z80_IntEnable",   cpuState._interruptsEnabled );
		serialiser.Field( "Z80_DeferInt",    cpuState._deferInterruptCheck );
		serialiser.Field( "Z80_IM",          cpuState._interruptMode );
		serialiser.Field( "Z80_Halted",      cpuState._cpuHalted );
		serialiser.Field( "Z80_Remaining",   cpuState._remainingCycles );
		serialiser.Field( "Z80_Timeslice",   cpuState._timesliceLength );

		_processor.SetSerialisableVariables( cpuState ); // Only really needed for reading.  Benign for writing.

		//
		// Camputers Lynx
		//

		serialiser.Field( "VideoPort",       _devicePort );
		serialiser.Field( "BankPort",        _bankPort );
		serialiser.Field( "MC6845",          _mc6845Select );
		serialiser.Binary( _mc6845Regs );

		//
		// Yes, I serialise the ROMs.
		//

		serialiser.Binary( "LynxROM_0000",       _lynxROM_0000 );
		serialiser.Binary( "LynxROM_2000",       _lynxROM_2000 );
		if( _machineType == LynxMachineType::LYNX_96K )
		{
			serialiser.Binary( "LynxROM_4000",       _lynxROM_4000 );
		}

		//
		// RAM serialisation.
		//

		if( _machineType == LynxMachineType::LYNX_96K )
		{
			serialiser.Binary( "LynxRAM_0000",       _lynxRAM_0000 );
			serialiser.Binary( "LynxRAM_2000",       _lynxRAM_2000 );
			serialiser.Binary( "LynxRAM_4000",       _lynxRAM_4000 );
		}
		serialiser.Binary( "LynxRAM_6000",       _lynxRAM_6000 );
		serialiser.Binary( "LynxRAM_8000",       _lynxRAM_8000 );
		if( _machineType == LynxMachineType::LYNX_96K )
		{
			serialiser.Binary( "LynxRAM_A000",       _lynxRAM_A000 );
			serialiser.Binary( "LynxRAM_C000",       _lynxRAM_C000 );
			serialiser.Binary( "LynxRAM_E000",       _lynxRAM_E000 );
		}

		//
		// Screen RAM serialisation.
		//

		serialiser.Binary( "LynxRedRAM",      _lynxRedRAM );
		serialiser.Binary( "LynxBlueRAM",     _lynxBlueRAM );
		serialiser.Binary( "LynxAltGreenRAM", _lynxAltGreenRAM );
		serialiser.Binary( "LynxGreenRAM",    _lynxGreenRAM );

		serialiser.CloseTag( "camputers_lynx" );
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     SPYING ON LYNX OPERATION
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::Z80_OnAboutToBranch()
	{
		// Is the Lynx ROM switched in?
		// If not, the addresses won't apply!

		if( (_bankPort & 1) == 0 )
		{
			// The ROM routine at 0x203C outputs a character. (Both 48K and 96K).
			// The accumulator has the character value.

			auto programCounter = _processor.GetSerialisableVariables()._programCounter;

			//
			// 0x203C : Spy on the PRINT CHARACTER routine.  (Same on 48K and 96K lynxes).
			//

			if( programCounter == 0x203C )
			{
				auto vduCode = (uint8_t) (_processor.GetSerialisableVariables()._AF >> 8);

				//
				// If the text recorder is in operation consider this character:
				//

				if( _textRecorder.IsOpen() )
				{
					if( ! _inhibitTextRecorder )
					{
						_textRecorder.AddLynxCharacter( vduCode );
					}

					if( vduCode == 13 )
					{
						// Seen CR
						// Assume to re-enable recording on next character.
						// The 0x9BD case below will re-inhibit if needed.
						_inhibitTextRecorder = false;  
					}
				}

				//
				// Watch for lynx extension REM commands ("REM <cmd> <params>")
				//

				if( _watchingCommands )
				{
					if( vduCode == 13 )
					{
						ProcessLynxCommand();
					}
				}

			}

			//
			// 09BD : Spy on the key read routine's ENTRY POINT.  (Same on 48K and 96K lynxes).
			//

			else if( programCounter == 0x9BD )  
			{
				// The Lynx has branched to the "key read" routine.
				// ROM routine that returns key pressed in A.

				// As soon as the lynx asks to read a key, we inhibit the recorder, otherwise we capture all the cursor
				// flashing rubbish!
				_inhibitTextRecorder = true;

				// If the text player is in operation, we don't let the Lynx execute its key read routine,
				// and instead we poke a character directly into the Z80 A register, and force a subroutine return:
				if( _textPlayer.HasText() )
				{
					auto cpuRegisters = _processor.GetSerialisableVariables(); // take copy
	
					auto nextChar = _textPlayer.ReadChar();
					if( nextChar != 0 )
					{
						// First off, the _textRecorder may as well record this always:
						if( _textRecorder.IsOpen() )
						{
							_textRecorder.AddLynxCharacter( nextChar );
						}

						// Put key code into accumulator:
						cpuRegisters._AF &= 0xFF;
						cpuRegisters._AF |= nextChar << 8;

						// Change PC to the address of a known C9 (RET):
						cpuRegisters._programCounter = 0x9BC;

						// Update state
						_processor.SetSerialisableVariables( cpuRegisters );
					}
				}
			}

		}
	}



	void LynxEmulatorGuest::Z80_OnAboutToReturn()
	{
		// TODO: remove?
	}







	std::string  LynxEmulatorGuest::GetLynxCommandLine() const
	{
		char buffer[256];

		// Lynx input buffer is stored at 0x6000.
		// Take a copy:

		memcpy( buffer, _lynxRAM_6000, 256 );

		// In the copy, find the first 13 and replace with NUL character
		// in readiness for returning that as std::string:

		for( int i=0; i<256; i++ )
		{
			if( buffer[i] == 13 )
			{
				buffer[i] = 0;
				return std::string(buffer);
			}
		}

		return std::string();
	}



	class LynxRamDisassemblySource: public JynxZ80::IZ80DisassemblerStream
	{
	public:

		// Connects the disassembler to the Lynx RAM.

		LynxRamDisassemblySource( uint16_t programCounter )
			: _programCounter(programCounter)
		{
		}

		virtual uint8_t ReadByte() override
		{
			// We sort of assume that read can never have side effects!
			// On the Lynx it doesn't.
			return JynxZ80::Z80ImplementationBaseClass::GuestRead( _programCounter++ );
		}

	private:

		uint16_t _programCounter;

	};





	void LynxEmulatorGuest::ProcessLynxCommand()
	{
		// If the user enabled the extended REM statements in the UI, this routine
		// can get lines typed at the Lynx's prompt.

		auto command = GetLynxCommandLine();

		std::stringstream  ss( command );

		std::string  strRem;
		std::string  strCommand;

		ss >> strRem >> strCommand;

		if( ! ss.fail() )
		{
			// Is is the disassembly command?

			if( strRem == "REM" && strCommand == "DISASM" )
			{
				uint32_t  address = 0;
				uint32_t  lineCount = 0;
				ss >> std::hex >> address >> std::dec >> lineCount;
			
				if( ! ss.fail() )
				{
					// Establish disassembler, and run it on the address given, for the
					// given number of lines of disassembly:

					LynxRamDisassemblySource  disasmSource( address );
					JynxZ80::Z80Disassembler disasm;
					disasm.SetLinePrefix( "REM " );
					disasm.SetSeparator( "  " );
					disasm.SetStreamAndAddress( &disasmSource, address );

					// Concatenate disassembly into a string.
					// (For flexibility the disassembler doesn't do this as I might
					//  want to power a UI in future!).

					std::string total;

					for( uint32_t i=0; i < lineCount; i++ )
					{
						total += disasm.GetNextLine();
						total += "\n";
					}

					// Type the disassembly into the lynx's BASIC prompt.
					// (The above code has generated a sequence of REM statements).

					_textPlayer.SetText( total.c_str() );
				}
				else
				{
					_textPlayer.SetText( "REM Usage: DISASM 85E 15\r" );
				}
			}
		}
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     Z80 THREAD:  EMULATION MAIN LOOP
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::BootstrapRunThreadMainLoop( void *thisPointer )  // static   (Compatible with non-member function).
	{
		((LynxEmulatorGuest *) thisPointer)->RunThreadMainLoop();
	}

	void LynxEmulatorGuest::RunThreadMainLoop()
	{
		while( ! _emulationThread.ShouldTerminate() )
		{
			// Read the volatile bool variables any of which can enable speed max mode:
			auto speedMaxMode = 
				_speedMaxModeBecauseOfCassette
				|| _speedMaxModeBecauseWeAreInBetweenConsoleCommands
				|| _speedMaxModeBecauseUserWantsItPermanently;

			if( ! speedMaxMode )
			{
				_hostObject->WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread();
			}

			if( ! _pauseMode )
			{
				// Execute Z80 code for this timeslice, and accumulate
				// the precise number of cycles elapsed (which may not
				// precisely be what we asked for, but the design supports
				// correcting this in later timeslices):

				auto cycleCountBefore = _processor.GetRemainingCycles();
				_processor.RunForTimeslice();
				auto cycleCountAfter  = _processor.GetRemainingCycles();
				_z80CycleCounter += (cycleCountAfter - cycleCountBefore) + _processor.GetTimesliceLength();
			}

			// Sound:

			_soundBufferWriter.EndOfZ80PeriodNotification();

			// Are we recording the sound to a file?

			if( _soundRecorder.IsOpen() && ! _pauseMode )
			{
				_soundBufferWriter.SerialiseSoundBufferContent( &_soundRecorder );
			}

			// Do we need to re-composite the whole screen?  (Because of port change).

			if( _recompositeWholeScreen )
			{
				RecompositeEntireLynxScreenOntoHostBitmap();
			}

			// Now that we have just supplied the sound buffer we have time to consider
			// the needs of the MAIN thread (if any).  If it wants access to the member
			// variables of this class (through the public interface) we can now allow it.
			// (We hope for the best it will release us in a timely fashion).

			if( _callWaiting )
			{
				_resumeEmulatorThread.Reset();
				_callWaitingAcknowledge.Signal(); // Releases MAIN thread to access our member variables.
				_resumeEmulatorThread.Wait();
			}
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     HANDLING CALLS FROM HOST
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxEmulatorGuest::NotifyKeyDown( int32_t guestKeyCode )
	{
		if( guestKeyCode == 1 )
		{
			// The ESC key cancels the _textPlayer.
			EmulatorThreadInhibitor  handshake(this);   // (Warning: This function is called on the client's thread).
			_textPlayer.Reset();
		}

		// (Warning: Called on client's thread -- volatile accessing only here).
		uint32_t registerIndex = (guestKeyCode >> 3) & 15;
		if( registerIndex < 10 )
		{
			auto orMask = 0x80 >> (guestKeyCode & 7);
			_keyboard[registerIndex] &= ~orMask;
		}
	}



	void LynxEmulatorGuest::NotifyKeyUp( int32_t guestKeyCode )
	{
		// (Warning: Called on client's thread -- volatile accessing only here).

		uint32_t registerIndex = (guestKeyCode >> 3) & 15;
		if( registerIndex < 10 )
		{
			auto orMask = 0x80 >> (guestKeyCode & 7);
			_keyboard[registerIndex] |= orMask;
		}
	}



	void LynxEmulatorGuest::NotifyAllKeysUp()
	{
		// (Warning: Called on client's thread -- volatile accessing only here).

		// (This was done to support cancelling keys on focus-loss in Windowed systems).

		for( auto &thisKey : _keyboard )
		{
			thisKey = 0xFF;  // NB: "active low" hardware logic
		}
	}



	void LynxEmulatorGuest::SaveState( IFileOpener *fileOpener )
	{
		EmulatorThreadInhibitor  handshake(this);

		if( IsTapeInOperation() )
		{
			throw std::runtime_error( "Cannot save a snapshot while the cassette is in operation." );
		}

		OutputFileSerialiser  outputSerialiser( fileOpener, _platformEndOfLineSequenceUTF8 );
		Serialise( outputSerialiser );
	}



	void LynxEmulatorGuest::LoadState( IFileOpener *fileOpener )
	{
		// TODO: This function works, but breaks the principle of de-serialising
		// into a SPARE object, and abandoning that in the case of failure.
		// From a perspective of inhibiting the emulation thread for as LITTLE time
		// as possible, we could de-serialise entirely on the calling thread, and
		// handshake to pass the result in.  The state could then be copied across
		// which would easily be done in the time.

		// All this said, the "damage" to the user experience of a sound spit
		// while de-serialising is mimimal as the sound will be reset anyway!

		EmulatorThreadInhibitor  handshake(this);

		InitialiseLYNX();

		try
		{
			InputFileSerialiser  inputSerialiser( fileOpener );
			Serialise( inputSerialiser );
			UpdateAllOtherStateFromPortStateVariables();

			// TODO: Do we need to do both of the following?
			MarkWholeScreenInvalid();
			_recompositeWholeScreen = true;
		}
		catch( const std::ifstream::failure & )
		{
			InitialiseLYNX();
			throw;
		}
	}



	void LynxEmulatorGuest::ResetGuest( LynxMachineType::Enum machineType )
	{
		EmulatorThreadInhibitor  handshake(this);
		_machineType = machineType;
		InitialiseLYNX();
	}



	uint32_t LynxEmulatorGuest::GetCyclesPerTimeslice() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this));  // TODO: elminate by having a copy of the state, NOT by changing the Z80!
		return _processor.GetTimesliceLength();
	}



	void LynxEmulatorGuest::SetCyclesPerTimeslice( uint32_t numCycles )
	{
		EmulatorThreadInhibitor  handshake(this);
		_processor.SetTimesliceLength( numCycles );
	}



	void LynxEmulatorGuest::InsertBlankTape()
	{
		EmulatorThreadInhibitor  handshake(this);
		_tapeMode = LynxTapeMode::SavingPermitted;
		_currentWriteTape = std::make_shared<TapFileWriter>();
		_hostObject->NotifyOutputTapeAvailbilityChanged_OnAnyThread();
	}



	bool LynxEmulatorGuest::CanRewindTape() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this)); // TODO: ideally allow volatile querying
		// (Cannot rewind tape in SAVE mode, because that's meant for building a new tape!).
		return (_tapeMode == LynxTapeMode::LoadingPermitted);
	}



	void LynxEmulatorGuest::RewindTape()
	{
		EmulatorThreadInhibitor  handshake(this);
		_currentReadTape->RewindPlaybackPosition();
	}



	void LynxEmulatorGuest::RunExistingTAPFile( IFileOpener *fileOpener )
	{
		ResetGuest( _machineType );  // API -- thus no thread inhibit needed
		LoadExistingTAPFile( fileOpener );  // API -- thus no thread inhibit needed

		EmulatorThreadInhibitor  handshake(this);

		if( _currentReadTape != nullptr )
		{
			_textPlayer.SetText( _currentReadTape->GetTapeDirectory( TapeDirectoryStyle::LoadCommands ).c_str() );
		}
	}



	void LynxEmulatorGuest::LoadExistingTAPFile( IFileOpener *fileOpener )
	{
		EmulatorThreadInhibitor  handshake(this);
		auto newTape = std::make_shared<TapFileReader>( fileOpener, this );  // throws
		_tapeMode = LynxTapeMode::LoadingPermitted;
		_currentReadTape = newTape;
	}



	bool LynxEmulatorGuest::CanSaveTAPFile() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this));  // TODO: Make volatile
		return _currentWriteTape->IsModified();
	}



	void LynxEmulatorGuest::SaveTape( IFileOpener *fileOpener )
	{
		std::vector<uint8_t> tapFileImage;

		{
			// (We only need to block emulator thread for this scope only)
			EmulatorThreadInhibitor  handshake(this);
			tapFileImage = _currentWriteTape->GetTapFileImage();
		}

		// Main thread can now do all the saving:
		SaveFileFromVector( fileOpener, tapFileImage );
		
		// Revert to a blank tape after saving, and signal MAIN thread to update UI:
		InsertBlankTape();
	}



	bool LynxEmulatorGuest::CanSaveSnapshot() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this));  // TODO: Make volatile
		return ! IsTapeInOperation();
	}



	void LynxEmulatorGuest::SetTapeSounds( bool tapeSounds )
	{
		// (Warning: Called on client's thread -- volatile accessing only here).
		_hearTapeSounds = tapeSounds;
	}



	bool LynxEmulatorGuest::GetTapeSounds() const
	{
		// (Warning: Called on client's thread -- volatile accessing only here).
		return _hearTapeSounds;
	}



	void LynxEmulatorGuest::RecordSoundToFile( IFileOpener *fileOpener )
	{
		EmulatorThreadInhibitor  handshake(this);
		FinishRecordingSoundToFile(); // in case already writing a sound file!
		_soundRecorder.StartNewFile( fileOpener );
	}



	void LynxEmulatorGuest::FinishRecordingSoundToFile()
	{
		EmulatorThreadInhibitor  handshake(this);
		if( _soundRecorder.IsOpen() )
		{
			_soundRecorder.Close();
		}
	}



	bool LynxEmulatorGuest::IsRecordingSoundToFile() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this));  // TODO: Make volatile?
		return _soundRecorder.IsOpen();
	}



	void LynxEmulatorGuest::RecordLynxTextToFile( IFileOpener *fileOpener )
	{
		EmulatorThreadInhibitor  handshake(this);
		FinishRecordingLynxTextToFile(); // in case already writing a sound file!
		_textRecorder.StartNewFile( fileOpener, _platformEndOfLineSequenceUTF8 );
		_inhibitTextRecorder = true; // until the next CR
	}



	void LynxEmulatorGuest::FinishRecordingLynxTextToFile()
	{
		EmulatorThreadInhibitor  handshake(this);  // TODO: Make volatile?
		if( _textRecorder.IsOpen() )
		{
			_textRecorder.Close();
		}
	}



	bool LynxEmulatorGuest::IsRecordingLynxTextToFile() const
	{
		EmulatorThreadInhibitor  handshake(const_cast<LynxEmulatorGuest *>(this));  // TODO: Make volatile?
		return _textRecorder.IsOpen();
	}



	void LynxEmulatorGuest::TypeInTextFromFile( IFileOpener *fileOpener )
	{
		// (We can do a load of this on the calling CLIENT thread)

		// TODO: Probably need to look for, and translate UTF-16 + BOM -> UTF-8.  *OR* make _textPlayer a plain byte-stream, and convert all source formats to byte stream here.

		// Load the file image into memory and add an extra NUL terminator:
		std::vector<uint8_t>  fileImage;
		LoadFileIntoVector( fileOpener, fileImage );
		fileImage.push_back(0); // NUL terminator

		// Interpret as a string of char:
		auto loadedText = (const char *) &fileImage.front();

		// Synchronize from now on because we need member variable access with _textPlayer.SetText():
		EmulatorThreadInhibitor  handshake(this);

		// Look for (optional) UTF-8 BOM, because we don't want to misinterpret that
		// as characters!
		if( fileImage[0] == 0xEF 
			&& fileImage[1] == 0xBB
			&& fileImage[2] == 0xBF )   // Short-circuit logic on if() is safe because of NUL appended above.
		{
			// UTF-8 BOM
			_textPlayer.SetText( &loadedText[3] );
		}
		else
		{
			// Assume no UTF-8 BOM, which covers ASCII.
			_textPlayer.SetText( &loadedText[0] );
		}
	}



	void LynxEmulatorGuest::TypeTapeDirectoryIntoLynx()
	{
		EmulatorThreadInhibitor  handshake(this);
		if( _currentReadTape != nullptr )
		{
			_textPlayer.SetText( _currentReadTape->GetTapeDirectory( TapeDirectoryStyle::REMCommandListing ).c_str() );
		}
		else
		{
			_textPlayer.SetText( "REM There is no tape inserted.\r" );
		}
	}



	bool LynxEmulatorGuest::GetLynxRemCommandExtensionsEnabled() const
	{
		// (Volatile access)
		return _watchingCommands;
	}


	void LynxEmulatorGuest::SetLynxRemCommandExtensionsEnabled( bool enable )
	{
		// (Volatile access)
		_watchingCommands = enable;
	}


	bool LynxEmulatorGuest::GetPauseMode() const
	{
		// (Volatile access)
		return _pauseMode;
	}


	void LynxEmulatorGuest::SetPauseMode( bool pauseMode )
	{
		// (Volatile access)
		_pauseMode = pauseMode;
	}


	bool LynxEmulatorGuest::GetPauseAfterTapLoadEnable() const
	{
		// (Volatile access)
		return _pauseAfterTapLoadEnable;
	}


	void LynxEmulatorGuest::SetPauseAfterTapLoadEnable( bool pauseAfterTapLoadEnable )
	{
		// (Volatile access)
		_pauseAfterTapLoadEnable = pauseAfterTapLoadEnable;
	}


	bool LynxEmulatorGuest::GetEnableSpeedMaxModeWhenUsingCassette() const
	{
		// (Volatile access)
		return _canEnableSpeedMaxModeWhenUsingCassette;
	}


	void LynxEmulatorGuest::SetEnableSpeedMaxModeWhenUsingCassette( bool newSetting )
	{
		// (Volatile access)
		_canEnableSpeedMaxModeWhenUsingCassette = newSetting;
		if( newSetting == false )
		{
			// Immediate turn off may be necessary as other cases may not trigger!
			_speedMaxModeBecauseOfCassette = false; 
		}
	}


	bool LynxEmulatorGuest::GetEnableSpeedMaxModeWhenInBetweenConsoleCommands() const
	{
		// (Volatile access)
		return _canEnableSpeedMaxModeWhenInBetweenConsoleCommands;
	}


	void LynxEmulatorGuest::SetEnableSpeedMaxModeWhenInBetweenConsoleCommands( bool newSetting )
	{
		// (Volatile access)
		_canEnableSpeedMaxModeWhenInBetweenConsoleCommands = newSetting;
		if( newSetting == false )
		{
			// Immediate turn off may be necessary as other cases may not trigger!
			_speedMaxModeBecauseWeAreInBetweenConsoleCommands = false; 
		}
	}


	bool LynxEmulatorGuest::GetEnableSpeedMaxModeBecauseUserWantsItPermanently() const
	{
		// (Volatile access)
		return _speedMaxModeBecauseUserWantsItPermanently;
	}


	void LynxEmulatorGuest::SetEnableSpeedMaxModeBecauseUserWantsItPermanently( bool newSetting )
	{
		// (Volatile access)
		_speedMaxModeBecauseUserWantsItPermanently = newSetting;
	}


	LynxColourSet::Enum  LynxEmulatorGuest::GetLynxColourSet() const
	{
		// (Volatile access)
		return _colourSet;
	}


	void LynxEmulatorGuest::SetLynxColourSet( LynxColourSet::Enum screenRendering )
	{
		EmulatorThreadInhibitor  handshake(this);
		if( _colourSet != screenRendering )
		{
			_colourSet = screenRendering;
			UpdatePalette();
			_recompositeWholeScreen = true;
		}
	}


} // end namespace Jynx





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     Z80 LIBRARY : CONNECTION TO SINGLETON EMULATOR GUEST OBJECT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

uint8_t JynxZ80::Z80ImplementationBaseClass::GuestRead( uint16_t address )
{
	// The Z80 is doing a READ of an address in the ADDRESS SPACE.
	return Jynx::g_LynxEmulatorGuestSingleton->Z80_AddressRead( address );
}


void JynxZ80::Z80ImplementationBaseClass::GuestWrite( uint16_t address, uint8_t dataByte )
{
	// The Z80 is doing a WRITE to an address in the ADDRESS SPACE.
	Jynx::g_LynxEmulatorGuestSingleton->Z80_AddressWrite( address, dataByte );
}


void JynxZ80::Z80ImplementationBaseClass::GuestWriteIOSpace( uint16_t portNumber, uint8_t dataByte )
{
	// The Z80 is doing a WRITE to an address in the I/O SPACE.
	Jynx::g_LynxEmulatorGuestSingleton->Z80_IOSpaceWrite( portNumber, dataByte );
}


uint8_t JynxZ80::Z80ImplementationBaseClass::GuestReadIOSpace( uint16_t portNumber )
{
	// The Z80 is doing a READ of an address in the I/O SPACE.
	return Jynx::g_LynxEmulatorGuestSingleton->Z80_IOSpaceRead( portNumber );
}


void JynxZ80::Z80ImplementationBaseClass::OnAboutToBranch()
{
	// The Z80 is about to re-load the program counter as a result of
	// a branch of some kind (ie: not just run of the mill PC incrementing!)
	// This is our "spy hook" point.
	Jynx::g_LynxEmulatorGuestSingleton->Z80_OnAboutToBranch();
}

void JynxZ80::Z80ImplementationBaseClass::OnAboutToReturn()
{
	// The Z80 is about to re-load the program counter as a result of
	// a branch of some kind (ie: not just run of the mill PC incrementing!)
	// This is our "spy hook" point.
	Jynx::g_LynxEmulatorGuestSingleton->Z80_OnAboutToReturn();
}
