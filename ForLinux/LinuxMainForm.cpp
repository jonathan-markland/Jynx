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

#include "MenuItemIDs.h"
#include "LynxHardware.h"
#include "UIStrings.h"

#include <assert.h>
#include <iostream>
#include <fstream>

#include "LinuxMainForm.h"
#include "LinuxAboutBox.h"
#include "LinuxFileOpener.h"
#include "LinuxGtkFileDialogs.h"

/*
#include "stdafx.h"
#include "resource.h"
#include "mmsystem.h"


#define TIMESLICE_PERIOD   16   // 16 bizarrely looks like 20 milliseconds (check the cursor flash rate).
#define WM_HI_RES_TIMER (WM_USER + 0x101)



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//        HOST WINDOW
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


volatile HWND  g_hWndToPostMessage = NULL;

void CALLBACK MainFormTimerProcedure(
	uint32_t uTimerID,
	uint32_t uMsg,
	DWORD_PTR dwUser,
	DWORD_PTR dw1,
	DWORD_PTR dw2)
{
	if( g_hWndToPostMessage != NULL )
	{
		::PostMessage( g_hWndToPostMessage, WM_HI_RES_TIMER, 0, 0 );
	}
}
*/



MainForm::MainForm( /*HWND hWndOwner, */ const char *settingsFilePath, const char *snapshotFilePath, bool gamesMode, const char *tapFilePath, const char *exePath )
    : _win( nullptr )
    , _vbox( nullptr )
	, _lynxUIModel( nullptr )
	, _settingsFilePath(settingsFilePath)
	, _snapshotFilePath(snapshotFilePath)
	, _tapFilePath(tapFilePath)
	, _exePath(exePath)
	, _gamesMode(gamesMode)
	//, _guestScreenBitmap(NULL)
	//, _timeBeginPeriodResult(0)
	//, _timeSetEventResult(0)
	//, _saveDC(0)
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

    auto numChannels = 1;
    auto numFramesPerBuffer = numSamplesPerBuffer * numChannels;  // clarifying the issue
	_waveOutStream = std::make_shared<WaveOutputStream>( numChannels, numFramesPerBuffer, 3 );

/*
	//
	// Create frame buffer bitmap which emulator can directly draw on.
	//

	if( ! CreateDIBSectionFrameBuffer( LYNX_FRAMEBUF_WIDTH, LYNX_FRAMEBUF_HEIGHT, &_screenInfo, &_guestScreenBitmap ) )
	{
		throw std::runtime_error( "Windows cannot create the screen bitmap for the emulation.\nThe emulation cannot continue." );
	}

	//
	// Ask for high resolution timers.
	//

	_timeBeginPeriodResult = timeBeginPeriod(1);
	if( _timeBeginPeriodResult == TIMERR_NOCANDO )
	{
		MessageBox( *this, L"Windows has not permitted the emulator to use the desired timer frequency.\nThe emulation speed may be affected.", L"Note", MB_OK | MB_ICONINFORMATION );
	}

	_timeSetEventResult	= timeSetEvent( 20, 20, &MainFormTimerProcedure, (DWORD_PTR) GetHWND(), TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS );
	if( _timeSetEventResult == NULL )
	{
		throw std::runtime_error( "Windows cannot create a multimedia timer.  Emulation cannot continue." );
	}
*/
	//
	// Create the model (this has the emulator inside, plus UI logic)
	//

	_lynxUIModel = std::unique_ptr<Jynx::LynxUserInterfaceModel>( new Jynx::LynxUserInterfaceModel(
		this,
		&_soundBuffer.front(),
		_soundBuffer.size(),
		"\n",   // The preferred end of line sequence on the LINUX platform.
		_gamesMode ) );

    //
    // User interface
    //

    GtkConstruction();
}



MainForm::~MainForm()
{
	// THREADING NOTE:
	// - Must destroy _lynxUIModel FIRST - to clean up threads, before
	//   we destroy what the threads are using!
	_lynxUIModel = nullptr;
/*
	//
	// Now the EMULATOR thread is gone, we can now clean up
	// everything that the EMULATOR thread was using:
	//

	g_hWndToPostMessage = NULL;

	if( _timeSetEventResult != NULL )
	{
		timeKillEvent( _timeSetEventResult );
		_timeSetEventResult = NULL;
	}

	if( _timeBeginPeriodResult != TIMERR_NOCANDO )
	{
		timeEndPeriod(1);
		_timeBeginPeriodResult = TIMERR_NOCANDO;
	}

	if( _guestScreenBitmap != NULL )
	{
		::DeleteObject(_guestScreenBitmap);
		_guestScreenBitmap = NULL;
	}
	*/
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     THE GTK SECTION
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MainForm::GtkConstruction()  // TODO: Do in OnInitDialog instead?
{
    // Create the main window:

    _win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_container_set_border_width (GTK_CONTAINER (_win), 0);
    gtk_window_set_default_size(GTK_WINDOW(_win), 800, 600);
    gtk_window_set_title (GTK_WINDOW (_win), "Jynx");
    gtk_window_set_position (GTK_WINDOW (_win), GTK_WIN_POS_CENTER);
    gtk_widget_realize (_win);
    gtk_signal_connect( GTK_OBJECT( _win ), "delete_event", GTK_SIGNAL_FUNC( &MainForm::GtkHandlerForCloseBoxDeleteEvent ), this );

    // Create vertical box:

    _vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER(_win), _vbox );

    // Create the menu bar:

    _menuBar = std::make_shared<LinuxGtkMenuBar>();

    // Create buttons on the menu bar itself:

    auto clickHandlerForMenuOptions = this;

    _menuFile      = _menuBar->CreateMenu( "&File"      , clickHandlerForMenuOptions );
    _menuSpeed     = _menuBar->CreateMenu( "S&peed"     , clickHandlerForMenuOptions );
    _menuEmulation = _menuBar->CreateMenu( "&Emulation" , clickHandlerForMenuOptions );
    _menuDisplay   = _menuBar->CreateMenu( "&Display"   , clickHandlerForMenuOptions );
    _menuSound     = _menuBar->CreateMenu( "&Sound"     , clickHandlerForMenuOptions );
    _menuText      = _menuBar->CreateMenu( "&Text"      , clickHandlerForMenuOptions );
    _menuHelp      = _menuBar->CreateMenu( "&Help"      , clickHandlerForMenuOptions );

    // POPUP "&File"

    _menuFile->AddItem( "&Run TAP file ...",           ID_FILE_RUNTAPFILE );
    _menuFile->AddSeparator();
    _menuFile->AddItem( "&Open TAP file ...",          ID_FILE_OPENTAPFILE );
    _menuFile->AddItem( "&Rewind tape",                ID_FILE_REWINDTAPE );
    _menuFile->AddItem( "Tape &directory (at BASIC prompt)", ID_FILE_DIRECTORY );
    _menuFile->AddSeparator();
    _menuFile->AddItem( "&New output tape",            ID_FILE_INSERTBLANKTAPE );
    _menuFile->AddItem( "&Save as TAP file...",        ID_FILE_SAVETAPE );
    _menuFile->AddSeparator();
    _menuFile->AddItem( "Load s&tate snapshot ...",    ID_FILE_LOADSTATESNAPSHOT );
    _menuFile->AddItem( "S&ave state snapshot ...",    ID_FILE_SAVESTATESNAPSHOT );
    _menuFile->AddSeparator();
    _menuFile->AddItem( "E&xit",                       ID_FILE_EXIT );

    // POPUP "S&peed"

    _menuSpeed->AddTick(  "Speed &50%",                  ID_SPEED_SPEED50 );
    _menuSpeed->AddTick(  "Speed &100%",                 ID_SPEED_SPEED100 );
    _menuSpeed->AddTick(  "Speed &200%",                 ID_SPEED_SPEED200 );
    _menuSpeed->AddTick(  "Speed &400%",                 ID_SPEED_SPEED400 );
    _menuSpeed->AddTick(  "Speed &800%",                 ID_SPEED_SPEED800 );
    _menuSpeed->AddSeparator();
    _menuSpeed->AddTick(  "Super speed &cassette",        ID_SPEED_MAXSPEEDCASSETTE );
    _menuSpeed->AddTick(  "Super speed c&onsole",         ID_SPEED_MAXSPEEDCONSOLE );
    _menuSpeed->AddTick(  "Super speed &always",          ID_SPEED_MAXSPEEDALWAYS );
    _menuSpeed->AddSeparator();
    _menuSpeed->AddTick(  "&Pause",                      ID_EMULATION_PAUSE );

    // POPUP "&Emulation"

    _menuEmulation->AddTick(  "Lynx 48&K",                   ID_EMULATION_LYNX48K );
    _menuEmulation->AddTick(  "Lynx 9&6K",                   ID_EMULATION_LYNX96K );
    _menuEmulation->AddTick(  "Lynx 96K (+ &Scorpion ROM)",  ID_EMULATION_LYNX96KSCORPION );
    _menuEmulation->AddSeparator();
    _menuEmulation->AddTick(  "&Pause after tape load",       ID_EMULATION_PAUSEAFTERTAPLOAD );
    _menuEmulation->AddSeparator();
    _menuEmulation->AddItem(  "&Reset guest machine",        ID_EMULATION_RESET );

    // POPUP "&Display"

    _menuDisplay->AddTick(  "Fit to &window",              ID_DISPLAY_FITTOWINDOW );
    _menuDisplay->AddTick(  "Use &square pixels",          ID_DISPLAY_SQUAREPIXELS );
    _menuDisplay->AddTick(  "&Fill window",                ID_DISPLAY_FILLWINDOW );
    _menuDisplay->AddSeparator();
    _menuDisplay->AddTick(  "Normal &Lynx colours",        ID_DISPLAY_COLOURSET_NORMALRGB );
    _menuDisplay->AddTick(  "&Green screen monitor",       ID_DISPLAY_COLOURSET_GREENSCREENMONITOR );
    _menuDisplay->AddTick(  "&Black and white TV",         ID_DISPLAY_COLOURSET_BLACKANDWHITETV );
    _menuDisplay->AddTick(  "Level &9 game colours",       ID_DISPLAY_COLOURSET_LEVEL9 );
    _menuDisplay->AddTick(  "Show green &channel only",    ID_DISPLAY_COLOURSET_GREENONLY );
    _menuDisplay->AddSeparator();
    _menuDisplay->AddTick(  "F&ull screen",                ID_DISPLAY_FULLSCREENENABLE );

    // POPUP "&Sound"

    _menuSound->AddItem(  "&Record sound to file ...",    ID_SOUND_RECORDTOFILE );
    _menuSound->AddItem(  "&Finish recording",            ID_SOUND_FINISHRECORDING );
    _menuSound->AddSeparator();
    _menuSound->AddTick(  "&Listen to tape sounds",       ID_SOUND_LISTENTOTAPESOUNDS );
    _menuSound->AddSeparator();
    _menuSound->AddTick(  "&Enable sound",                ID_SOUND_ENABLE );

    // POPUP "&Text"

    _menuText->AddItem(  "&Record Lynx text to file ...", ID_TEXT_RECORDLYNXTEXT );
    _menuText->AddItem(  "&Stop recording Lynx text",     ID_TEXT_STOPRECORDINGLYNXTEXT );
    _menuText->AddSeparator();
    _menuText->AddItem(  "&Type in text from file ...",   ID_TEXT_TYPEINFROMFILE );
    _menuText->AddSeparator();
    _menuText->AddTick(  "&Enable Lynx BASIC REM command extensions", ID_TEXT_LYNXBASICREMCOMMANDEXTENSIONS );

    // POPUP "&Help"

    _menuHelp->AddItem(  "&About ...",                  ID_HELP_ABOUT );



    // Establish content of primary vertical box:
    gtk_box_pack_start( GTK_BOX(_vbox), _menuBar->GetWidget(), FALSE, FALSE, 0 );

    // TODO: Add main bitmap here.

    OnInitDialog();
}



void MainForm::ShowAll()
{
    gtk_widget_show_all( _win );
}




void MainForm::NotifyMenuItemClicked( uint32_t menuItemID )
{
    // This gets the menu item clicks.

    if( ! _lynxUIModel->DispatchMenuComment( menuItemID ) )
    {
        if( menuItemID == ID_HELP_ABOUT ) // not handled by the model
        {
            OnAbout();
        }
    }
}



gint MainForm::GtkHandlerForCloseBoxDeleteEvent( GtkWidget *widget, GdkEvent *event, gpointer thisMainWindowObject ) // static member
{
    // If you return FALSE GTK will emit the "destroy" signal.
    // Returning TRUE means you don't want the window to be destroyed.

    auto thisObject = (MainForm *) thisMainWindowObject;
    thisObject->OnCancel(); // thisObject->NotifyMenuItemClicked( ID_FILE_EXIT );
    return TRUE; // do not automatically destroy window.  The handler above will have done that if it is required.
}












/*
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
*/



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     FRAMEWORK HANDLING
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool  MainForm::OnInitDialog()
{
    /*
	SetBigAndSmallIcons( IDR_MAINFRAME );

	g_hWndToPostMessage = GetHWND();

	// Centre window placement BEFORE calling model's OnInitDialog() as
	// that may cause go full screen as settings file is loaded!
	libWinApi::CenterWindowPercent( *this, 85, GetOwner() );
*/
	_lynxUIModel->OnInitDialog();

	if( ! _snapshotFilePath.empty() )
	{
		// Load the snapshot file that the user specified on the command line:
		LinuxFileOpener  fileOpener( _snapshotFilePath.c_str() );
		_lynxUIModel->ForceLoadSpecificSnapshot( &fileOpener );
	}
	else if( ! _tapFilePath.empty() )
	{
		// Load the cassette file that the user specified on the command line:
		LinuxFileOpener  fileOpener( _tapFilePath.c_str() );
		_lynxUIModel->ForceLoadSpecificTape( &fileOpener );
	}

	return true; // TODO: return not required in Linux
}


void  MainForm::OnCancel()
{
	_lynxUIModel->OnExit();
}


/*
void MainForm::WindowProc( libWinApi::WindowProcArgs &e )
{
	try
	{
		uint16_t  menuCommand = 0;

		//
		// WM_HI_RES_TIMER:
		// Periodic timer for the model to perform tasks.
		//

		if( e.message == WM_HI_RES_TIMER )
		{
			_lynxUIModel->OnTimer();
			e.Result = 0;
			return;
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


		if( e.IsDeactivated() )
		{
			_lynxUIModel->NotifyAllKeysUp();   // Let's make sure we don't have stuck keys, switching away from the app!
		}

		if( e.message == WM_LBUTTONDBLCLK )
		{
			_lynxUIModel->OnEnableDisableFullScreen();
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
	catch( const UserInterfaceException &nonFatalError )  // reminder: catches stream errors too.  TODO: This might be controversial, and a "UserInterfaceException" better.
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
*/





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     UI ELEMENT EVENT HANDLERS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MainForm::OnAbout()
{
    Jynx::ShowLinuxGtkAboutBox();
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//        VIEW SERVICES TO MODEL  (IHostServicesForLynxUserInterfaceModel)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MainForm::CloseDownNow()
{
	// Just exit with no questions asked -- the Model has done all checks + saves.
	gtk_main_quit();
}



std::shared_ptr<Jynx::IFileOpener> MainForm::ShowOpenFileDialog( Jynx::LoadableFileTypes::Enum fileType )
{
	std::string  filePathChosen;
	if( ::ShowOpenFileDialog( GTK_WINDOW(_win), OpenFileDialogTitlesNarrow[fileType], OpenFileDialogSpecsNarrow[fileType], &filePathChosen ) )
	{
		return std::make_shared<LinuxFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



std::shared_ptr<Jynx::IFileOpener> MainForm::ShowSaveFileDialog( Jynx::SaveableFileTypes::Enum fileType )
{
	std::string  filePathChosen;
	if( ::ShowSaveFileDialog( GTK_WINDOW(_win), SaveFileDialogTitlesNarrow[fileType], SaveFileDialogSpecsNarrow[fileType], SaveFileDialogExtnsNarrow[fileType], &filePathChosen ) )
	{
		return std::make_shared<LinuxFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



void MainForm::TellUser( const char *messageText, const char *captionText )
{
	// ::MessageBoxA( *this, messageText, captionText, MB_OK | MB_ICONINFORMATION );

    auto dialog = gtk_message_dialog_new(
        GTK_WINDOW(_win),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s",
        messageText);

    gtk_window_set_title( GTK_WINDOW(dialog), captionText );
    gtk_dialog_run( GTK_DIALOG(dialog) );
    gtk_widget_destroy( dialog );
}



bool MainForm::AskYesNoQuestion( const char *questionText, const char *captionText )
{
    auto dialog = gtk_message_dialog_new(
        GTK_WINDOW(_win),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "%s",
        questionText);

    gtk_window_set_title( GTK_WINDOW(dialog), captionText );
    auto result = gtk_dialog_run( GTK_DIALOG(dialog) );
    gtk_widget_destroy( dialog );

    return result == GTK_RESPONSE_YES;
}



void MainForm::SetTickBoxState( Jynx::TickableInterfaceElements::Enum itemToSet, bool tickState )
{
	assert( itemToSet >= 0 && itemToSet <= Jynx::TickableInterfaceElements::Count );
	assert( sizeof(MainFormTickableItems) == (sizeof(uint32_t) * Jynx::TickableInterfaceElements::Count) );

    _menuBar->CheckMenuItem( MainFormTickableItems[itemToSet], tickState );

/*
	if( itemToSet == Jynx::TickableInterfaceElements::ShowFullScreen )
	{
		auto previouslyFullScreen = libWinApi::IsWindowFullScreen( *this );
		if( tickState != previouslyFullScreen )
		{
			if( tickState )
			{
				_restorationAfterFullScreen = libWinApi::GoFullScreen( *this );
			}
			else
			{
				_restorationAfterFullScreen.Restore();
			}
		}
	}
	*/
}



void MainForm::SetEnabledState( Jynx::ButtonInterfaceElements::Enum itemToSet, bool enableState )
{
	assert( itemToSet >= 0 && itemToSet <= Jynx::ButtonInterfaceElements::Count );
	assert( sizeof(MainFormGreyableItems) == (sizeof(uint32_t) * Jynx::ButtonInterfaceElements::Count) );

    _menuBar->EnableMenuItem( MainFormGreyableItems[itemToSet], enableState );
}



Jynx::LynxRectangle  MainForm::GetClientRectangle()
{
	Jynx::LynxRectangle  area;
	assert(false); // TODO: complete this function
/*
	RECT r;
	::GetClientRect( *this, &r );

	area.left   = r.left;
	area.top    = r.top;
	area.right  = r.right;
	area.bottom = r.bottom;
*/
	return area;
}



void MainForm::SetViewport( int left, int top, int width, int height )
{
	assert(false);
    /*
	if( _dc != NULL ) // If this is NULL, program will be terminating because of posted quit message, anyway!
	{
		assert( _saveDC == 0 );
		_saveDC = ::SaveDC( _dc );
		::IntersectClipRect( _dc, left, top, left+width, top+height );
	}*/
}



void MainForm::CancelViewport()
{
	assert(false);
    /*
	if( _dc != NULL && _saveDC != 0 )
	{
		::RestoreDC( _dc, _saveDC );
		_saveDC = 0;
	}*/
}



void MainForm::StretchBlitTheGuestScreen( int left, int top, int width, int height )
{
	assert(false);
    /*
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
	}*/
}



void MainForm::FillBlackRectangle( int left, int top, int width, int height )
{
	assert(false);
    /*
	if( _dc != NULL )
	{
		::BitBlt( _dc, left, top, width, height, NULL, 0, 0, BLACKNESS );
	}*/
}



void MainForm::InvalidateAreaOfHostScreen( const Jynx::LynxRectangle &area )
{
	assert(false);
    /*
	RECT r;
	r.left   = area.left;
	r.top    = area.top;
	r.right  = area.right;
	r.bottom = area.bottom;
	::InvalidateRect( *this, &r, FALSE );
	*/
}





void  MainForm::OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, Jynx::LynxRoms::Enum romRequired )
{
	// (Reminder - called on the MAIN thread only).

	assert( uint32_t(romRequired) < Jynx::LynxRoms::Count ); // Should be

	// Determine path name, ROMS are in same folder as the Windows EXE:
	auto leafFileName = std::string( g_RomFileNamesNarrow[ romRequired ] );
	auto filePath = _exePath + "/" + leafFileName;

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




void MainForm::TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues )
{
	// Nothing to do here, the format this host requires is the same as the emulator uses.
}



void  MainForm::PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData )
{
	// (WARNING - Called on the EMULATOR thread, NOT the MAIN thread)

	// Multithreading note:  In theory, we may get "tearing" with this being unsynchronised.
	// This is deemed not to matter.  The EMULATOR thread CANNOT BE HELD UP without risking sound suffering!

/*	int32_t  destX = (addressOffset & 0x1F) << 3;
	int32_t  destY = (addressOffset >> 5);
	auto destinationPixelAddress = CalcFrameBufferPixelAddress( (uint32_t *) _screenInfo.BaseAddress, _screenInfo.BytesPerScanLine, destX, destY );
	auto endPixelAddress = destinationPixelAddress + 8;
	std::copy_n( eightPixelsData, 8, destinationPixelAddress );*/
}



std::string  GetJynxAppDataPath()
{
    // TODO:  Ensure it is a folder, make it if it does not exist.
    return "~/.jynx_emulator";

/*	auto pathRoot = "~/.jynx_emulator";
	if( ! pathRoot.empty() )
	{
		auto jynxFolderPath = pathRoot + L"JynxEmulator";
		if ( CreateDirectory( jynxFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError() )
		{
			return jynxFolderPath;
		}
	}
	return std::string();*/
}



std::shared_ptr<Jynx::IFileOpener>  MainForm::GetUserSettingsFilePath()
{
	if( _settingsFilePath.empty() )
	{
		// The user did not specify a settings file path on the command line.
		// Use the platform preferred location for settings files:

		auto jynxAppDataPath = GetJynxAppDataPath();
		if( ! jynxAppDataPath.empty() )
		{
			auto fullPath = jynxAppDataPath + "/JynxEmulatorSettings.config";
			return std::make_shared<LinuxFileOpener>( fullPath );
		}
	}
	else
	{
		// Use the path the user specified on the command line:
		return std::make_shared<LinuxFileOpener>( _settingsFilePath );
	}

	return nullptr;
}







void MainForm::WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread()
{
	// (Called on the EMULATOR thread, NOT the MAIN thread)

	if( _lynxUIModel->IsSoundEnabled() )
	{
		// NOTE: The sound card "forces" us back until it's ready.
		// This gives us a 20ms timer, on which the emulation is synchronised, when sound is ON.
		_waveOutStream->Write( &(*_soundBuffer.begin()), (uint32_t) _soundBuffer.size() );
	}
	else
	{
		// Sound is OFF, so we have to sleep for the 20 milliseconds instead.
		// The emulation burst processing is usually very small on a modern CPU
		// so this will suffice.  I don't care so much about realtime accuracy with
		// sound OFF.
		usleep( 20*1000 );
	}
}
