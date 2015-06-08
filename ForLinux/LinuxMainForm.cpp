
#include "../Portable/MenuItemIDs.h"

#include "LinuxMainForm.h"
#include "LinuxAboutBox.h"







void MainForm::NotifyMenuItemClicked( uint32_t menuItemID )
{
    // This gets the menu item clicks.

    // TODO: call model

    if( menuItemID == ID_HELP_ABOUT )
    {
        Jynx::ShowLinuxGtkAboutBox();
    }
}



gint MainForm::GtkHandlerForCloseBoxDeleteEvent( GtkWidget *widget, GdkEvent *event, gpointer thisMainWindowObject ) // static member
{
    // If you return FALSE GTK will emit the "destroy" signal.
    // Returning TRUE means you don't want the window to be destroyed.

    auto thisObject = (MainForm *) thisMainWindowObject;
    thisObject->NotifyMenuItemClicked( ID_FILE_EXIT );
    // TODO: Who calls gtk_main_quit now?gint delete_event( GtkWidget *widget,
    return TRUE; // do not automatically destroy window.  The handler above will have done that if it is required.
}



MainForm::MainForm()
    : _win( nullptr )
    , _vbox( nullptr )
{
    // Create the main window
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
}



void MainForm::ShowAll()
{
    gtk_widget_show_all( _win );
}




