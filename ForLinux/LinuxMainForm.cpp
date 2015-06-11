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
#include <algorithm>

#include "gdk/gdkkeysyms.h"

#include "LinuxMainForm.h"
#include "LinuxAboutBox.h"
#include "LinuxFileOpener.h"
#include "LinuxGtkFileDialogs.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//        HOST WINDOW
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

MainForm::MainForm( const char *settingsFilePath, const char *snapshotFilePath, bool gamesMode, const char *tapFilePath, const char *exePath )
    : _settingsFilePath(settingsFilePath)
	, _snapshotFilePath(snapshotFilePath)
	, _tapFilePath(tapFilePath)
	, _exePath(exePath)
	, _gamesMode(gamesMode)
	//, _guestScreenBitmap(NULL)
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

	//
	// Create frame buffer bitmap which emulator can directly draw on.
	//

	_pixBuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB, TRUE, 8, LYNX_FRAMEBUF_WIDTH, LYNX_FRAMEBUF_HEIGHT );  // Reminder: buffer is not cleared.
	if( ! _pixBuf )
	{
		throw std::runtime_error( "Cannot create the screen bitmap for the emulation.\nThe emulation cannot continue." );
	}

	auto pixelBuffer   = gdk_pixbuf_get_pixels( _pixBuf );
	auto numChannels2  = gdk_pixbuf_get_n_channels( _pixBuf );
	auto bitsPerSample = gdk_pixbuf_get_bits_per_sample( _pixBuf );
	auto rowStride     = gdk_pixbuf_get_rowstride( _pixBuf );

    _pixBufBaseAddress      = pixelBuffer;
    _pixBufBytesPerScanLine = rowStride;

/*GdkColorspace gdk_pixbuf_get_colorspace      (const GdkPixbuf *pixbuf);
int           gdk_pixbuf_get_n_channels      (const GdkPixbuf *pixbuf);
gboolean      gdk_pixbuf_get_has_alpha       (const GdkPixbuf *pixbuf);
int           gdk_pixbuf_get_bits_per_sample (const GdkPixbuf *pixbuf);
guchar       *gdk_pixbuf_get_pixels          (const GdkPixbuf *pixbuf);
int           gdk_pixbuf_get_width           (const GdkPixbuf *pixbuf);
int           gdk_pixbuf_get_height          (const GdkPixbuf *pixbuf);
int           gdk_pixbuf_get_rowstride       (const GdkPixbuf *pixbuf);
gsize         gdk_pixbuf_get_byte_length     (const GdkPixbuf *pixbuf);
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

	if( _guestScreenBitmap != NULL )
	{
		::DeleteObject(_guestScreenBitmap);GdkPixbuf *         gdk_pixbuf_new                      (GdkColorspace colorspace,
                                                         gboolean has_alpha,
                                                         int bits_per_sample,
                                                         int width,
                                                         int height);
		_guestScreenBitmap = NULL;
	}
	*/

    // TODO: unref the pixbuf
	// TODO:  To be clean, what GTK destruction needs to be done?  Most of the examples just returned from main().
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     THE GTK SECTION
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MainForm::GtkConstruction()  // TODO: Do in OnInitDialog instead?
{
    // Create the main window:

    _gtkWindow = GTK_WINDOW( gtk_window_new( GTK_WINDOW_TOPLEVEL ) );
    _gtkWindowAsWidget = GTK_WIDGET( _gtkWindow );  // avoid loads of casting

    gtk_container_set_border_width( GTK_CONTAINER( _gtkWindow ), 0 );
    gtk_window_set_default_size(    _gtkWindow, 800, 600 );
    gtk_window_set_title(           _gtkWindow, "Jynx" );
    gtk_window_set_position(        _gtkWindow, GTK_WIN_POS_CENTER );
    gtk_widget_realize(             _gtkWindowAsWidget );
    gtk_signal_connect(             GTK_OBJECT( _gtkWindow ), "delete_event", GTK_SIGNAL_FUNC( &MainForm::GtkHandlerForCloseBoxDeleteEvent ), this );

    // Create vertical box:

    _vbox = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER(_gtkWindow), _vbox );

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

    //
    // Add a generic drawable widget, to which we'll add event hooks to draw the screen.
    //

    _gtkDrawingArea = gtk_drawing_area_new();
    // gtk_drawing_area_size( GTK_DRAWING_AREA(_gtkDrawingArea), 200, 200 );
    auto gtkDrawingAreaAsObject = GTK_OBJECT(_gtkDrawingArea);
    gtk_signal_connect( gtkDrawingAreaAsObject, "expose_event",        (GtkSignalFunc) &MainForm::GtkHandlerForDrawingAreaExposeEvent,       this );
    gtk_signal_connect( gtkDrawingAreaAsObject, "configure_event",     (GtkSignalFunc) &MainForm::GtkHandlerForDrawingAreaConfigureEvent,    this );
    gtk_signal_connect( gtkDrawingAreaAsObject, "motion_notify_event", (GtkSignalFunc) &MainForm::GtkHandlerForDrawingAreaMotionNotifyEvent, this );
    gtk_signal_connect( gtkDrawingAreaAsObject, "button_press_event",  (GtkSignalFunc) &MainForm::GtkHandlerForDrawingAreaButtonPressEvent,  this );
    gtk_signal_connect( gtkDrawingAreaAsObject, "key_press_event",     (GtkSignalFunc) &MainForm::GtkHandlerForKeyPress,    this );
    gtk_signal_connect( gtkDrawingAreaAsObject, "key_release_event",   (GtkSignalFunc) &MainForm::GtkHandlerForKeyRelease,  this );
    gtk_widget_set_events(
        _gtkDrawingArea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
        | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK );  // TODO: Do I need all of these?
    gtk_widget_set_extension_events( _gtkDrawingArea, GDK_EXTENSION_EVENTS_CURSOR ); // TODO: What does this do?
    gtk_widget_set_can_focus( _gtkDrawingArea, TRUE );
    gtk_widget_show( _gtkDrawingArea ); // TODO: needed?

    //
    // Establish content of primary vertical box:
    //

    auto vboxAsBox = GTK_BOX(_vbox);
    gtk_box_pack_start( vboxAsBox, _menuBar->GetWidget(), FALSE, FALSE, 0 );
    gtk_box_pack_start( vboxAsBox, _gtkDrawingArea, TRUE, TRUE, 0 );

    //
    // The emulation thread provides timer-like functionality to the main thread.
    // This saves us setting up an additional timer with the library.
    //

    // g_idle_add( (GSourceFunc) &MainForm::GtkHandlerForIdleTasks, this );
    g_timeout_add( 20, (GSourceFunc) &MainForm::GtkHandlerForIdleTasks, this );

    OnInitDialog();
}



void MainForm::ShowAll()
{
    gtk_widget_show_all( _gtkWindowAsWidget );
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







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     HOST KEY CODE to LYNX KEY INDEX translation
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


		// Key codes are in order of the ports Bits A11..A8, then in order bit D7..D0 :


#define NUMBER_OF_KEYS (8*11)

#define VK_ESCAPE     9
#define VK_LSHIFT    50
#define VK_RSHIFT    62
#define VK_LCONTROL  37
#define VK_RCONTROL 105
#define VK_DOWN     116
#define VK_UP       111
#define VK_CAPITAL   66
#define VK_SPACE     65
#define VK_OEM_COMMA 59
#define VK_OEM_1      47 // PC :;
#define VK_OEM_PERIOD 60
#define VK_OEM_PLUS   21 // PC + =
#define VK_OEM_2      61 // PC / ?
#define VK_OEM_4      34 // PC { [
#define VK_OEM_7      48 // PC ' @
#define VK_OEM_MINUS  20 // PC - _
#define VK_RIGHT    114
#define VK_RETURN    36
#define VK_LEFT     113
#define VK_OEM_6      35 // PC } ]
#define VK_BACK      22
#define VK_HOME     110
#define VK_END      115


uint8_t  KeysInLynxKeyOrder[NUMBER_OF_KEYS] =
{
	VK_LSHIFT, VK_ESCAPE,   VK_DOWN,     VK_UP,     VK_CAPITAL,    0,            0,            10/*1*/,
	0,         0,           54/*C*/,     40/*D*/,   53/*X*/,       26/*E*/,      13/*4*/,      12/*3*/,
	0,         VK_LCONTROL, 38/*A*/,     39/*S*/,   52/*Z*/,       25/*W*/,      24/*Q*/,      11/*2*/,
	0,         0,           41/*F*/,     42/*G*/,   55/*V*/,       28/*T*/,      27/*R*/,      14/*5*/,
	0,         0,           56/*B*/,     57/*N*/,   VK_SPACE,      43/*H*/,      29/*Y*/,      15/*6*/,
	0,         0,           44/*J*/,     0,         58/*M*/,       30/*U*/,      17/*8*/,      16/*7*/,
	0,         0,           45/*K*/,     0,     VK_OEM_COMMA,      32/*O*/,      31/*I*/,      18/*9*/,
	0,         0,           VK_OEM_1,    0,     VK_OEM_PERIOD,     46/*L*/,      33/*P*/,      19/*0*/,
	0,         0,           VK_OEM_PLUS, 0,     VK_OEM_2,      VK_OEM_4, VK_OEM_7, VK_OEM_MINUS,
	0,         0,           VK_RIGHT,    0,     VK_RETURN,     VK_LEFT,  VK_OEM_6, VK_BACK,
};


int32_t GdkHardwareKeyCodeToLynxKeyIndex( uint8_t keyVkCode )
{
	// Wire these keys to the ones that will be found in the table:

	     if( keyVkCode == VK_RSHIFT )     keyVkCode = VK_LSHIFT;
	else if( keyVkCode == VK_RCONTROL )   keyVkCode = VK_LCONTROL;
	else if( keyVkCode == VK_HOME )       keyVkCode = VK_UP;   // Works well with Lynx line editor
	else if( keyVkCode == VK_END)         keyVkCode = VK_DOWN; // Works well with Lynx line editor

	for( int32_t n=0; n < NUMBER_OF_KEYS; ++n )
	{
		if( KeysInLynxKeyOrder[n] == keyVkCode ) return n;
	}

	return -1;
}


#undef VK_ESCAPE
#undef VK_LSHIFT
#undef VK_RSHIFT
#undef VK_LCONTROL
#undef VK_RCONTROL
#undef VK_DOWN
#undef VK_UP
#undef VK_CAPITAL
#undef VK_SPACE
#undef VK_OEM_COMMA
#undef VK_OEM_1
#undef VK_OEM_PERIOD
#undef VK_OEM_PLUS
#undef VK_OEM_2
#undef VK_OEM_4
#undef VK_OEM_7
#undef VK_OEM_MINUS
#undef VK_RIGHT
#undef VK_RETURN
#undef VK_LEFT
#undef VK_OEM_6
#undef VK_BACK
#undef VK_HOME
#undef VK_END





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
			auto lynxKeyIndex = GdkHardwareKeyCodeToLynxKeyIndex( keyCode );
			if( lynxKeyIndex != -1 )
			{
				_lynxUIModel->NotifyKeyDown( lynxKeyIndex );
				return true;
			}
		}
		else if( pMsg->IsKeyUp( &keyCode ) )
		{
			auto lynxKeyIndex = GdkHardwareKeyCodeToLynxKeyIndex( keyCode );
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
//     GTK event handlers
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



gint MainForm::GtkHandlerForCloseBoxDeleteEvent( GtkWidget *widget, GdkEvent *event, gpointer userObject ) // static member
{
    // If you return FALSE GTK will emit the "destroy" signal.
    // Returning TRUE means you don't want the window to be destroyed.

    auto thisObject = (MainForm *) userObject;
    thisObject->OnCancel(); // thisObject->NotifyMenuItemClicked( ID_FILE_EXIT );
    return TRUE; // do not automatically destroy window.  The handler above will have done that if it is required.
}



gint MainForm::GtkHandlerForDrawingAreaConfigureEvent( GtkWidget *widget, GdkEventConfigure *event, gpointer userObject ) // static member   configure_event
{
    // CONFIGURE
    auto thisObject = (MainForm *) userObject;
    return TRUE;
}



gint MainForm::GtkHandlerForDrawingAreaExposeEvent( GtkWidget *widget, GdkEventExpose *event, gpointer userObject )    // static member   expose_event
{
    auto thisObject = (MainForm *) userObject;

    if( thisObject->_gtkDrawingArea != nullptr )
    {
        auto gtkDrawble = gtk_widget_get_window( thisObject->_gtkDrawingArea );
        thisObject->_cairoContext = gdk_cairo_create( gtkDrawble );
        if( thisObject->_cairoContext != nullptr )
        {
            thisObject->_lynxUIModel->OnPaint();
            cairo_destroy( thisObject->_cairoContext );
            thisObject->_cairoContext = nullptr;
        }
    }

    return FALSE;
}



gint MainForm::GtkHandlerForDrawingAreaMotionNotifyEvent( GtkWidget *widget, GdkEventMotion *event, gpointer userObject )    // static member   motion_notify_event
{
    auto thisObject = (MainForm *) userObject;
    return TRUE;
}



gint MainForm::GtkHandlerForDrawingAreaButtonPressEvent(  GtkWidget *widget, GdkEventButton *event, gpointer userObject )    // static member   button_press_event
{
    auto thisObject = (MainForm *) userObject;
    gtk_widget_grab_focus( thisObject->_gtkDrawingArea );
    return TRUE;
}



gboolean  MainForm::GtkHandlerForKeyPress( GtkWidget *widget, GdkEvent *event, gpointer userObject ) // static member     key_press_event
{
    auto thisObject = (MainForm *) userObject;
    auto keyEvent = (GdkEventKey *) event;
    auto lynxKeyIndex = GdkHardwareKeyCodeToLynxKeyIndex( keyEvent->hardware_keycode );
    if( lynxKeyIndex != -1 )
    {
        thisObject->_lynxUIModel->NotifyKeyDown( lynxKeyIndex );
        return TRUE;
    }
    return FALSE;
}



gboolean  MainForm::GtkHandlerForKeyRelease( GtkWidget *widget, GdkEvent *event, gpointer userObject ) // static member     key_release_event
{
    auto thisObject = (MainForm *) userObject;
    auto keyEvent = (GdkEventKey *) event;
    auto lynxKeyIndex = GdkHardwareKeyCodeToLynxKeyIndex( keyEvent->hardware_keycode );
    if( lynxKeyIndex != -1 )
    {
        thisObject->_lynxUIModel->NotifyKeyUp( lynxKeyIndex );
        return TRUE;
    }
    return FALSE;
}



gboolean MainForm::GtkHandlerForIdleTasks( gpointer userObject )    // static member   button_press_event
{
    auto thisObject = (MainForm *) userObject;
    //if( thisObject->_mainThreadShouldDoPeriodicTasks )
    {
        thisObject->_lynxUIModel->OnTimer();
        //thisObject->_mainThreadShouldDoPeriodicTasks = false;
    }
    return TRUE;  // FALSE would remove us from the to do list.
}








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
	if( ::ShowOpenFileDialog( GTK_WINDOW(_gtkWindow), OpenFileDialogTitlesNarrow[fileType], OpenFileDialogSpecsNarrow[fileType], &filePathChosen ) )
	{
		return std::make_shared<LinuxFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



std::shared_ptr<Jynx::IFileOpener> MainForm::ShowSaveFileDialog( Jynx::SaveableFileTypes::Enum fileType )
{
	std::string  filePathChosen;
	if( ::ShowSaveFileDialog( GTK_WINDOW(_gtkWindow), SaveFileDialogTitlesNarrow[fileType], SaveFileDialogSpecsNarrow[fileType], SaveFileDialogExtnsNarrow[fileType], &filePathChosen ) )
	{
		return std::make_shared<LinuxFileOpener>( filePathChosen ); // throws
	}
	return nullptr;  // User cancelled
}



void MainForm::TellUser( const char *messageText, const char *captionText )
{
	// ::MessageBoxA( *this, messageText, captionText, MB_OK | MB_ICONINFORMATION );

    auto dialog = gtk_message_dialog_new(
        GTK_WINDOW(_gtkWindow),
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
        GTK_WINDOW(_gtkWindow),
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

	if( itemToSet == Jynx::TickableInterfaceElements::ShowFullScreen )
	{
        if( tickState == true )
        {
            gtk_window_fullscreen( GTK_WINDOW(_gtkWindow) );
        }
        else
        {
            gtk_window_unfullscreen( GTK_WINDOW(_gtkWindow) );
        }
	}
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

	if( _gtkDrawingArea != nullptr )
    {
        int width   = _gtkDrawingArea->allocation.width;
        int height  = _gtkDrawingArea->allocation.height;
        area.left   = 0;
        area.top    = 0;
        area.right  = width;
        area.bottom = height;
    }

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
    if( _gtkDrawingArea != nullptr && _cairoContext != nullptr )
    {
        cairo_matrix_t  mat;
        cairo_matrix_init( &mat, double(width) / LYNX_FRAMEBUF_WIDTH, 0, 0, double(height) / LYNX_FRAMEBUF_HEIGHT, left, top);
        cairo_transform( _cairoContext, &mat );
        cairo_rectangle( _cairoContext, 0, 0, LYNX_FRAMEBUF_WIDTH, LYNX_FRAMEBUF_HEIGHT ); // left, top, width, height );
        gdk_cairo_set_source_pixbuf( _cairoContext, _pixBuf, 0, 0 );
        cairo_set_antialias( _cairoContext, CAIRO_ANTIALIAS_NONE );
        cairo_pattern_set_filter( cairo_get_source(_cairoContext), CAIRO_FILTER_NEAREST );
        cairo_fill( _cairoContext );
    }
}



void MainForm::FillBlackRectangle( int left, int top, int width, int height )
{
    auto gtkDrawble = gtk_widget_get_window( _gtkDrawingArea );
    gdk_draw_rectangle( gtkDrawble, _gtkDrawingArea->style->black_gc, TRUE, left, top, width, height );
}



void MainForm::InvalidateAreaOfHostScreen( const Jynx::LynxRectangle &area )
{
    GdkRectangle  rect;
    rect.x      = area.left;
    rect.y      = area.top;
    rect.width  = area.right - area.left;
    rect.height = area.bottom - area.top;
    auto gtkDrawble = gtk_widget_get_window( _gtkDrawingArea );
	gdk_window_invalidate_rect( gtkDrawble, &rect, FALSE ); // FALSE = there are no children to invalidate anyway.
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
	for( int i=0; i<8; i++ )
    {
        eightEntryTranslatedValues[i] = eightEntryColourPalette[i] | 0xFF000000; // Set ALPHA to 255 always.
    }
}



void  MainForm::PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData )
{
	// (WARNING - Called on the EMULATOR thread, NOT the MAIN thread)

	// Multithreading note:  In theory, we may get "tearing" with this being unsynchronised.
	// This is deemed not to matter.  The EMULATOR thread CANNOT BE HELD UP without risking sound suffering!

	int32_t  destX = (addressOffset & 0x1F) << 3;
	int32_t  destY = (addressOffset >> 5);
	auto destinationPixelAddress = CalcFrameBufferPixelAddress( (uint32_t *) _pixBufBaseAddress, _pixBufBytesPerScanLine, destX, destY );
	auto endPixelAddress = destinationPixelAddress + 8;
	std::copy_n( eightPixelsData, 8, destinationPixelAddress );
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

	_mainThreadShouldDoPeriodicTasks = true;
}
