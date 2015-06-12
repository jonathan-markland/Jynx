
#include <stdlib.h>
#include <stdint.h>
#include <string>

#include <gtk/gtk.h>

#include "ParameterParsing.h"
#include "LinuxMainForm.h"


template<typename CHAR_TYPE>
std::basic_string<CHAR_TYPE>  RemoveLeafFromPath( const CHAR_TYPE *path )   // TODO: Move to library
{
    std::basic_string<CHAR_TYPE>  result(path);
    auto pos = result.rfind('/');
    if( pos != std::basic_string<CHAR_TYPE>::npos )
    {
        return std::basic_string<CHAR_TYPE>( result.begin(), result.begin() + pos );
    }
    throw std::runtime_error( "Cannot determine path!" );
}



int main( int argc, char *argv[] )
{
    //
    // Initialize GTK+
    //

    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    std::string exceptionMessage;

    try
    {
        auto paramsList = MakeParamsListFromArgcAndArgv( argc, argv, 1 );

        //
        // Create main window
        //

/*        auto settingsFilePath = "~/.jynx_emulator/settings"; // TODO: fix
        auto snapshotFilePath = ""; // not loading a snapshot
        auto gamesMode = false; // TODO: fix
        auto tapFilePath = "";  // not loading a tap
        */
        auto exePath = RemoveLeafFromPath( argv[0] );

        auto mainWindow = std::make_shared<MainForm>( paramsList, exePath.c_str() );
        mainWindow->ShowAll();

    //  button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
    //  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (helloWorld), (gpointer) win);
    //  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

    //  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    //  g_signal_connect (button, "clicked", gtk_main_quit, NULL);
    //  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

        /* Enter the main loop */
        gtk_main();
    }
    catch( std::exception &e )
    {
        exceptionMessage = e.what();
    }

    //
    // MainForm now destructed, so game is no longer running.
    //

    if( ! exceptionMessage.empty() )
    {
        auto dialog = gtk_message_dialog_new(
            NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "The program cannot continue running because of an internal error:\n'%s'",
            exceptionMessage.c_str() );

        gtk_window_set_title( GTK_WINDOW(dialog), "Program error" );
        gtk_dialog_run( GTK_DIALOG(dialog) );
        gtk_widget_destroy( dialog );
    }

    return 0;
}
