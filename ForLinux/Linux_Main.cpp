//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2015  Jonathan Markland
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

#include <stdlib.h>
#include <stdint.h>
#include <string>

#include <gtk/gtk.h>

#include "ParameterParsing.h"
#include "Linux_MainForm.h"



int main( int argc, char *argv[] )
{
    //
    // Initialize GTK+
    //

    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init( NULL, NULL );
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    std::string exceptionMessage;

    try
    {
        auto paramsList = MakeParamsListFromArgcAndArgv( argc, argv, 1 );
        auto exePath = RemoveLeafFromPath( argv[0] );
        auto mainWindow = std::make_shared<MainForm>( paramsList, exePath.c_str() );
        gtk_main();
    }
    catch( std::exception &e )
    {
        exceptionMessage = e.what();
    }

    //
    // MainForm now destructed, so emulator is no longer running.
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
        return 1;
    }

    return 0;
}
