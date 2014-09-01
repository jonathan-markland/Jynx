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



#include "stdafx.h"
#include <assert.h>
#include "resource.h"
#include "mmsystem.h"
#include <iostream>
#include <fstream>
#include "MainForm.h"
#include "AboutBoxForm.h"
#include "WindowsFileOpener.h"

#include "../Portable/LynxHardware.h"
#include "../Portable/LynxUserInterfaceModel.h"

// TODO: The emulation runs very well with the emulation "bursts" being processed on the UI thread.  However, Open file dialogs cause interruption
//       in the sound because of non-timely servicing of the sound.  This little glitch can be fixed by putting the UI on a different thread, or
//       opening the file dialogs on a different thread even!

// TODO: Shame the WM_TIMER is inaccurate even with timeBeginPeriod(1) -- try the creating a MM timer and posting from that thread.
//	     Maybe TIMESLICE_PERIOD can really be set to 20 then!


#define TIMER_EVENT_ID      1   // WM_TIMER ID for MainForm.

#define TIMESLICE_PERIOD   16   // 16 bizarrely looks like 20 milliseconds (check the cursor flash rate).



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//        HOST WINDOW
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


MainForm::MainForm( HWND hWndOwner )
	: BaseForm( hWndOwner, MainForm::IDD )
	, _lynxUIModel( nullptr )
	, _guestScreenBitmap(NULL)
	, _timeBeginPeriodResult(0)
	, _timerID(0)
	, _waveOutStream(nullptr)
{
	//
	// Sound
	//

	uint32_t numSamplesPerBuffer = 882;

	_soundBuffer.reserve( numSamplesPerBuffer );

	for( uint32_t i=0; i < numSamplesPerBuffer; i++ )
	{
		_soundBuffer.push_back( 0 );
	}

	auto bufferSizeBytes = numSamplesPerBuffer * 2;
	_waveOutStream = new libWinApi::WaveOutputStream( 44100, 2, 1, 3, (int) bufferSizeBytes );

	//
	// Create frame buffer bitmap which emulator can directly draw on.
	//

	if( ! CreateDIBSectionFrameBuffer( LYNX_FRAMEBUF_WIDTH, LYNX_FRAMEBUF_HEIGHT, &_screenInfo, &_guestScreenBitmap ) )
	{
		throw std::exception( "Windows cannot create the screen bitmap for the emulation.\nThe emulation cannot continue." );
	}

	//
	// Ask for high resolution timers.
	//

	_timeBeginPeriodResult = timeBeginPeriod(1);
	if( _timeBeginPeriodResult == TIMERR_NOCANDO )
	{
		MessageBox( *this, L"Windows has not permitted the emulator to use the desired timer frequency.\nThe emulation speed may be affected.", L"Note", MB_OK | MB_ICONINFORMATION );
	}

	//
	// Create the model (this has the emulator inside, plus UI logic)
	//

	_lynxUIModel = new Jynx::LynxUserInterfaceModel( 
		this, 
		&_soundBuffer.front(), 
		_soundBuffer.size(), 
		"\r\n" );  // The preferred end of line sequence on the WINDOWS platform.  (Think: Notepad.exe!)
}



MainForm::~MainForm()
{
	// Must destroy _lynxUIModel FIRST - to clean up threads, before
	// we destroy what the threads are using!
	if( _lynxUIModel != nullptr )
	{
		delete (Jynx::LynxUserInterfaceModel *) _lynxUIModel;  // TODO: not ideal upcast
		_lynxUIModel = nullptr;
	}

	if( _waveOutStream != nullptr )
	{
		delete _waveOutStream;
		_waveOutStream = nullptr;
	}

	if( _timerID != 0 )
	{
		::KillTimer( *this, TIMER_EVENT_ID );
		_timerID = 0;
	}

	if( _timeBeginPeriodResult != TIMERR_NOCANDO )
	{
		timeEndPeriod(1);
	}

	if( _guestScreenBitmap != NULL )
	{
		::DeleteObject(_guestScreenBitmap);
		_guestScreenBitmap = NULL;
	}
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     HOST KEY CODE to LYNX KEY INDEX translation
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


		// Key codes are in order of the ports Bits A11..A8, then in order bit D7..D0 :


#define NUMBER_OF_KEYS (8*11)

uint8_t  KeysInLynxKeyOrder[NUMBER_OF_KEYS] = 
{
	VK_LSHIFT, VK_ESCAPE,   VK_DOWN,     VK_UP, VK_CAPITAL,    0,        0,        '1',
	0,         0,           'C',         'D',   'X',           'E',      '4',      '3',
	0,         VK_LCONTROL, 'A',         'S',   'Z',           'W',      'Q',      '2',
	0,         0,           'F',         'G',   'V',           'T',      'R',      '5',
	0,         0,           'B',         'N',   VK_SPACE,      'H',      'Y',      '6',
	0,         0,           'J',         0,     'M',           'U',      '8',      '7',
	0,         0,           'K',         0,     VK_OEM_COMMA,  'O',      'I',      '9',
	0,         0,           VK_OEM_1,    0,     VK_OEM_PERIOD, 'L',      'P',      '0',
	0,         0,           VK_OEM_PLUS, 0,     VK_OEM_2,      VK_OEM_4, VK_OEM_7, VK_OEM_MINUS,
	0,         0,           VK_RIGHT,    0,     VK_RETURN,     VK_LEFT,  VK_OEM_6, VK_BACK,
};


int32_t MicrosoftWindowsVkCodeToLynxKeyIndex( uint8_t keyVkCode )
{
	// Wire these keys to the ones that will be found in the table:

	if( keyVkCode == VK_SHIFT )      keyVkCode = VK_LSHIFT;
	else if( keyVkCode == VK_RSHIFT )     keyVkCode = VK_LSHIFT;
	else if( keyVkCode == VK_CONTROL )    keyVkCode = VK_LCONTROL;
	else if( keyVkCode == VK_RCONTROL )   keyVkCode = VK_LCONTROL;
	else if( keyVkCode == VK_HOME )       keyVkCode = VK_UP;   // Works well with Lynx line editor
	else if( keyVkCode == VK_END)         keyVkCode = VK_DOWN; // Works well with Lynx line editor

	for( int32_t n=0; n < NUMBER_OF_KEYS; ++n )
	{
		if( KeysInLynxKeyOrder[n] == keyVkCode ) return n;
	}

	return -1;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FRAMEWORK HANDLING
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

bool  MainForm::OnInitDialog()
{
	SetBigAndSmallIcons( IDR_MAINFRAME );

	_timerID = ::SetTimer( *this, TIMER_EVENT_ID, TIMESLICE_PERIOD, NULL );

	libWinApi::CenterWindowPercent( *this, 85, GetOwner() );
	_lynxUIModel->OnInitDialog();
	return BaseForm::OnInitDialog();
}



void  MainForm::OnCancel()
{
	_lynxUIModel->OnExit();
}



void MainForm::WindowProc( libWinApi::WindowProcArgs &e )
{
	try
	{
		uint16_t  menuCommand = 0;

		//
		// WM_TIMER:  If a timer is being used to time the emulation, advance it now.
		//            (If sound is enabled, the timer is ignored because the sound 
		//            playback also times the emulation).
		//

		if( e.message == WM_TIMER )
		{
			if( e.wParam == TIMER_EVENT_ID )
			{
				/*if( ! _lynxUIModel->IsSoundEnabled() )
				{
					_lynxUIModel->AdvanceEmulation();
				}*/
				_lynxUIModel->CallMeBackToInvalidateRegions();
				e.Result = 0;
				return;
			}
		}

		if( e.IsPaint() )
		{
			libWinApi::WmPaintHandler ph( *this );
			if( ! ph.AreaIsEmpty() )
			{
				if( ph.ClipToUpdateRect() )
				{
					_dc = ph.dc;
					_lynxUIModel->OnPaint();
					_dc = NULL;
				}
			}
			e.Result = 1;
			return;
		}

		if( e.IsErase() )
		{
			e.Result = 1;
			return;
		}

		if( e.IsMenuCommand( &menuCommand ) )
		{
			switch( menuCommand )
			{
				case ID_FILE_LOADSTATESNAPSHOT:   _lynxUIModel->OnLoadStateSnapshot(); break;
				case ID_FILE_SAVESTATESNAPSHOT:   _lynxUIModel->OnSaveStateSnapshot(); break;
				case ID_FILE_OPENTAPFILE:         _lynxUIModel->OnOpenTAPFile(); break;
				case ID_FILE_INSERTBLANKTAPE:     _lynxUIModel->OnNewAudioTape(); break;
				case ID_FILE_SAVETAPE:            _lynxUIModel->OnSaveTAPFileAs(); break;
				case ID_FILE_REWINDTAPE:          _lynxUIModel->OnRewindAudioTape(); break;
				case ID_FILE_EXIT:                _lynxUIModel->OnExit(); break;
				case ID_EMULATION_LYNX48K:        _lynxUIModel->OnEmulation48K(); break;
				case ID_EMULATION_LYNX96K:        _lynxUIModel->OnEmulation96K(); break;
				case ID_EMULATION_SPEED50:        _lynxUIModel->OnSetCycles( Jynx::LynxZ80Cycles::At50 ); break;
				case ID_EMULATION_SPEED100:       _lynxUIModel->OnSetCycles( Jynx::LynxZ80Cycles::At100 ); break;
				case ID_EMULATION_SPEED200:       _lynxUIModel->OnSetCycles( Jynx::LynxZ80Cycles::At200 ); break;
				case ID_EMULATION_SPEED400:       _lynxUIModel->OnSetCycles( Jynx::LynxZ80Cycles::At400 ); break;
				case ID_EMULATION_SPEED800:       _lynxUIModel->OnSetCycles( Jynx::LynxZ80Cycles::At800 ); break;
				case ID_SOUND_LISTENTOTAPESOUNDS: _lynxUIModel->OnListenToTapeSounds(); break;
				case ID_SOUND_RECORDTOFILE:       _lynxUIModel->OnRecordToFile(); break;
				case ID_SOUND_FINISHRECORDING:    _lynxUIModel->OnFinishRecording(); break;
				case ID_TEXT_RECORDLYNXTEXT:      _lynxUIModel->OnRecordLynxTextToFile(); break;
				case ID_TEXT_STOPRECORDINGLYNXTEXT:          _lynxUIModel->OnFinishRecordingLynxText(); break;
				case ID_TEXT_TYPEINFROMFILE:                 _lynxUIModel->OnTypeInTextFromFile(); break;
				case ID_TEXT_LYNXBASICREMCOMMANDEXTENSIONS:  _lynxUIModel->OnLynxBasicRemCommandExtensions(); break;
				case ID_EMULATION_RESET:          _lynxUIModel->OnResetEmulation(); break;
				case ID_DISPLAY_FITTOWINDOW:      _lynxUIModel->OnFitToWindow(); break;
				case ID_DISPLAY_SQUAREPIXELS:     _lynxUIModel->OnSquarePixels(); break;
				case ID_SOUND_ENABLE:             OnSound(); break; // not handled by the model
				case ID_HELP_ABOUT:               OnAbout(); break; // not handled by the model
				default:                          return BaseForm::WindowProc( e );
			}
			e.Result = 1;
			return;  // Processed.
		}

		if( e.IsDeactivated() )
		{
			_lynxUIModel->NotifyAllKeysUp();   // Let's make sure we don't have stuck keys, switching away from the app!
		}

		//
		// WM_WINDOWPOSCHANGED
		//

		if( e.message == WM_WINDOWPOSCHANGED )
		{
			auto pWP = reinterpret_cast<WINDOWPOS *>(e.lParam);
			if( pWP )
			{
				int Mask = SWP_NOSIZE; // irritating negative logic here
				if( (pWP->flags & Mask) != Mask ) 
				{
					// Called because of size of this window.
					RECT r;
					if( GetClientRect( *this, &r ) )
					{
						InvalidateRect( *this, &r, FALSE );
					}
				}
			}

			// Let caller see this message too.
		}

	}
	catch( const std::runtime_error &nonFatalError )  // reminder: catches stream errors too.  TODO: This might be controversial, and a "UserInterfaceException" better.
	{
		MessageBoxA( *this, nonFatalError.what(), "Error", MB_OK | MB_ICONERROR );
	}

	return BaseForm::WindowProc( e );
}










bool  MainForm::PreProcessMessage( libWinApi::Message *pMsg )
{
	if( _lynxUIModel != nullptr )
	{
		uint32_t  keyCode = 0;

		if( pMsg->IsKeyDown( &keyCode ) )
		{
			auto lynxKeyIndex = MicrosoftWindowsVkCodeToLynxKeyIndex( keyCode );
			if( lynxKeyIndex != -1 )
			{
				_lynxUIModel->NotifyKeyDown( lynxKeyIndex );
				return true;
			}
		}
		else if( pMsg->IsKeyUp( &keyCode ) )
		{
			auto lynxKeyIndex = MicrosoftWindowsVkCodeToLynxKeyIndex( keyCode );
			if( lynxKeyIndex != -1 )
			{
				_lynxUIModel->NotifyKeyUp( lynxKeyIndex );
				return true;
			}
		}
	}

	return false;
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     UI ELEMENT EVENT HANDLERS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void MainForm::OnAbout()
{
	AboutBoxForm  aboutForm( *this );
	aboutForm.DoModal();
}



void MainForm::OnSound()
{
	auto hMenu = ::GetMenu( *this );
	if( hMenu )
	{
		auto state = ::GetMenuState( hMenu, ID_SOUND_ENABLE, MF_BYCOMMAND );

		_lynxUIModel->OnEnableDisableSound(); // do in-between

		if( state != MF_CHECKED )
		{
			// Sound is being turned ON.
			// PostMessage( *this, WM_ADVANCE_EMULATION, 0, 0 );
			// TODO: What?
		}
	}
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//        VIEW SERVICES TO MODEL  (IHostServicesForLynxUserInterfaceModel)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void MainForm::CloseDownNow()
{
	// Just exit with no questions asked -- the Model has done all checks + saves.
	BaseForm::OnCancel();
}



const wchar_t *OpenFileDialogTitles[Jynx::LoadableFileTypes::Count] = 
{ 
	L"Open Lynx TAP file", 
	L"Open emulator state snapshot",
	L"Open Text file",
};

const wchar_t *OpenFileDialogSpecs[Jynx::LoadableFileTypes::Count]  = 
{ 
	L"TAP files (*.TAP)|*.TAP", 
	L"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	L"Text files (*.txt)|*.txt",
};

std::shared_ptr<Jynx::IFileOpener> MainForm::ShowOpenFileDialog( Jynx::LoadableFileTypes::Enum fileType )
{
	std::wstring  filePathChosen;

	if( libWinApi::ShowOpenFileDialog( *this, OpenFileDialogTitles[fileType], OpenFileDialogSpecs[fileType], &filePathChosen ) )
	{
		return std::make_shared<WindowsFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



const wchar_t *SaveFileDialogTitles[Jynx::SaveableFileTypes::Count] = 
{
	L"Save Lynx Audio Tape file",     
	L"Save emulator state snapshot",
	L"Record sound to file",
	L"Record lynx text to file"
};

const wchar_t *SaveFileDialogSpecs[Jynx::SaveableFileTypes::Count] = 
{ 
	L"Lynx TAP files (*.TAP)|*.TAP",
	L"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	L"Sound files (*.wav)|*.wav",
	L"Text files (*.txt)|*.txt"
};

const wchar_t *SaveFileDialogExtns[Jynx::SaveableFileTypes::Count] = 
{ 
	L"TAP",
	L"lynxsnapshot",
	L"wav",
	L"txt"
};

std::shared_ptr<Jynx::IFileOpener> MainForm::ShowSaveFileDialog( Jynx::SaveableFileTypes::Enum fileType )
{
	std::wstring  filePathChosen;

	if( libWinApi::ShowSaveFileDialog( *this, SaveFileDialogTitles[fileType], SaveFileDialogSpecs[fileType], SaveFileDialogExtns[fileType], &filePathChosen ) )
	{
		return std::make_shared<WindowsFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



void MainForm::TellUser( const char *messageText, const char *captionText )
{
	::MessageBoxA( *this, messageText, captionText, MB_OK | MB_ICONINFORMATION );
}



bool MainForm::AskYesNoQuestion( const char *questionText, const char *captionText )
{
	return ::MessageBoxA( *this, questionText, captionText, MB_YESNO | MB_ICONQUESTION ) == IDYES;
}



const UINT MainFormTickableItems[Jynx::TickableInterfaceElements::Count] =
{
	ID_EMULATION_LYNX48K,
	ID_EMULATION_LYNX96K,
	ID_SOUND_LISTENTOTAPESOUNDS,
	ID_DISPLAY_FITTOWINDOW,
	ID_DISPLAY_SQUAREPIXELS,
	ID_EMULATION_SPEED50,
	ID_EMULATION_SPEED100,
	ID_EMULATION_SPEED200,
	ID_EMULATION_SPEED400,
	ID_EMULATION_SPEED800,
	ID_TEXT_LYNXBASICREMCOMMANDEXTENSIONS,
	ID_SOUND_ENABLE,
};

void MainForm::SetTickBoxState( Jynx::TickableInterfaceElements::Enum itemToSet, bool tickState )
{
	auto hMenu = ::GetMenu( *this );
	if( hMenu )
	{
		CheckMenuItem( hMenu, MainFormTickableItems[itemToSet], tickState ? MF_CHECKED : 0 );
	}
}



const UINT MainFormGreyableItems[Jynx::ButtonInterfaceElements::Count] =
{
	ID_FILE_REWINDTAPE,
	ID_SOUND_FINISHRECORDING,
	ID_FILE_SAVETAPE,
	ID_TEXT_STOPRECORDINGLYNXTEXT
};

void MainForm::SetEnabledState( Jynx::ButtonInterfaceElements::Enum itemToSet, bool enableState )
{
	auto hMenu = ::GetMenu( *this );
	if( hMenu )
	{
		EnableMenuItem( hMenu, MainFormGreyableItems[itemToSet], enableState ? MF_GRAYED  : MF_ENABLED );
	}
}



Jynx::LynxRectangle  MainForm::GetClientRectangle()
{
	Jynx::LynxRectangle  area;

	RECT r;
	::GetClientRect( *this, &r );

	area.left   = r.left;
	area.top    = r.top;
	area.right  = r.right;
	area.bottom = r.bottom;

	return area;
}



void MainForm::StretchBlitTheGuestScreen( int left, int top, int width, int height )
{
	if( _dc != NULL && _guestScreenBitmap != NULL ) // If this is NULL, program will be terminating because of posted quit message, anyway!
	{
		auto bitmapDC = ::CreateCompatibleDC( _dc );
		if( bitmapDC != NULL )
		{
			auto previousBitmapHandle = (HANDLE) ::SelectObject( bitmapDC, _guestScreenBitmap );

			::StretchBlt( 
				_dc, 
				left, top, width, height,
				bitmapDC, 
				0, 0, LYNX_FRAMEBUF_WIDTH, LYNX_FRAMEBUF_HEIGHT,
				SRCCOPY );

			::SelectObject( bitmapDC, previousBitmapHandle );
			::DeleteDC( bitmapDC );
		}
	}
}



void MainForm::FillBlackRectangle( int left, int top, int width, int height )
{
	if( _dc != NULL )
	{
		::BitBlt( _dc, left, top, width, height, NULL, 0, 0, BLACKNESS ); 
	}
}



void MainForm::InvalidateAreaOfHostScreen( const Jynx::LynxRectangle &area )
{
	RECT r;
	r.left   = area.left;
	r.top    = area.top;
	r.right  = area.right;
	r.bottom = area.bottom;
	::InvalidateRect( *this, &r, FALSE );
}





const wchar_t *g_RomFileNames[Jynx::LynxRoms::Count] =
{
	L"lynx48-1.rom",
	L"lynx48-2.rom",
	L"lynx96-1.rom",
	L"lynx96-2.rom",
	L"lynx96-3.rom",
};

void  MainForm::OpenChipFileStream( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, Jynx::LynxRoms::Enum romRequired )
{
	// (Reminder - called on the main thread only).

	assert( uint32_t(romRequired) < Jynx::LynxRoms::Count ); // Should be

	// Determine path name, ROMS are in same folder as the Windows EXE:
	auto wideLeafFileName = std::wstring( g_RomFileNames[ romRequired ] );
	auto folderPath = libWinApi::GetMyExeFolderPath();
	auto filePath = folderPath + wideLeafFileName;

	// Open the stream for the emulator:
	streamToBeOpened.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
	streamToBeOpened.open( filePath.c_str(), openModeRequired );
}



template<typename PIXEL_TYPE>
inline PIXEL_TYPE *CalcFrameBufferPixelAddress( PIXEL_TYPE *frameBufferTopLeftAddress, int32_t frameBufferRowStepBytes, int32_t pixelX, int32_t pixelY )
{
	auto pixelAddress = ((intptr_t) frameBufferTopLeftAddress) + (pixelY * frameBufferRowStepBytes) + (pixelX * sizeof(PIXEL_TYPE));
	return (PIXEL_TYPE *) pixelAddress;
}




void  MainForm::PaintPixelsOnHostBitmapForLynxScreenByte( uint32_t addressOffset, uint32_t lynxRedByte, uint32_t lynxGreenByte, uint32_t lynxBlueByte )
{
	// (WARNING - Called on the Z80 thread, NOT the main thread)

	// Multithreading note:  In theory, we may get "tearing" with this being unsynchronised.
	// This is deemed not to matter.  The Z80 thread CANNOT BE HELD UP without risking sound suffering!

	int32_t  destX = (addressOffset & 0x1F) << 3;
	int32_t  destY = (addressOffset >> 5);

	auto pixelAddress = CalcFrameBufferPixelAddress( (uint32_t *) _screenInfo.BaseAddress, _screenInfo.BytesPerScanLine, destX, destY );
	uint8_t pixelMask = 0x80;
	while( pixelMask != 0 )
	{
		uint32_t hostPixelValue = 0;
		if( lynxRedByte & pixelMask ) 
		{
			hostPixelValue |= RGB(255,0,0);
		}
		if( lynxGreenByte & pixelMask )
		{
			hostPixelValue |= RGB(0,255,0);
		}
		if( lynxBlueByte & pixelMask )
		{
			hostPixelValue |= RGB(0,0,255);
		}
		pixelMask >>= 1;
		*pixelAddress = hostPixelValue;
		++pixelAddress;
	}
}



std::wstring  GetJynxAppDataPath()
{
	auto pathRoot = libWinApi::GetUserAppDataPath();
	if( ! pathRoot.empty() )
	{
		auto jynxFolderPath = pathRoot + L"JynxEmulator";
		if ( CreateDirectory( jynxFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError() )
		{
			return jynxFolderPath;
		}
	}
	return std::wstring();
}



std::shared_ptr<Jynx::IFileOpener>  MainForm::GetUserSettingsFilePath()
{
	auto jynxAppDataPath = GetJynxAppDataPath();
	if( ! jynxAppDataPath.empty() )
	{
		auto fullPath = jynxAppDataPath + L"\\JynxEmulatorSettings.config";
		return std::make_shared<WindowsFileOpener>( fullPath );
	}
	return nullptr;
}


class ThreadAdapterToMicrosoftWindows: public libWinApi::Thread, public Jynx::IHostThread
{
public:
	ThreadAdapterToMicrosoftWindows( Jynx::IHostServicesForLynxEmulatorThreadFunction threadFunction, void *userObject );
	virtual int32_t ThreadMain() override;
	virtual void SignalToTerminateAndJoin() override;
	virtual bool CanKeepRunning() override;
private:
	Jynx::IHostServicesForLynxEmulatorThreadFunction _threadFunction;
	void *_userObject;
};



ThreadAdapterToMicrosoftWindows::ThreadAdapterToMicrosoftWindows( Jynx::IHostServicesForLynxEmulatorThreadFunction threadFunction, void *userObject )
	: _threadFunction(threadFunction)
	, _userObject(userObject)
{
	CreateAndRun();
}

int32_t ThreadAdapterToMicrosoftWindows::ThreadMain()
{
	_threadFunction(_userObject);
	return 0;
}

void ThreadAdapterToMicrosoftWindows::SignalToTerminateAndJoin()
{
	this->RequestTermination();
	this->WaitForTermination();
}

bool ThreadAdapterToMicrosoftWindows::CanKeepRunning()
{
	return ! ShouldTerminate();
}



Jynx::IHostThread *MainForm::CreateThread( Jynx::IHostServicesForLynxEmulatorThreadFunction threadFunction, void *userObject )
{
	return new ThreadAdapterToMicrosoftWindows( threadFunction, userObject );
}



void MainForm::ThreadSleep( uint32_t milliseconds )
{
	::Sleep( milliseconds );
}



void MainForm::ThreadWaitForSound()
{
	// (Called on the Z80 thread, not the MAIN thread)
	if( _lynxUIModel->IsSoundEnabled() )
	{
		_waveOutStream->Write( &(*_soundBuffer.begin()), (int) _soundBuffer.size() * 2 );
	}
	else
	{
		::Sleep(20);  // Not using sound.
	}
}

