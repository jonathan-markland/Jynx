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

#include "Linux_GtkFileDialogs.h"






void AddMicrosoftStyleFilters( GtkFileChooser *file_chooser, const gchar *fileSpecs )
{
    // Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||

    auto patterns = g_strsplit( fileSpecs, "|", -1 );

    for( gint i = 0; patterns[i] != NULL && patterns[i+1] != NULL; i += 2 )
    {
        auto f = gtk_file_filter_new();
        gtk_file_filter_set_name( f, patterns[i] );       // name
        gtk_file_filter_add_pattern( f, patterns[i+1] );  // spec
        gtk_file_chooser_add_filter( file_chooser, f );
    }

    g_strfreev( patterns );
}



std::string  AppendIfExtensionMissing( const std::string &filePath, const char *defaultExtensionWithoutDot )
{
    // If there isn't a dot in the string, the user has not specified an extension, so add one.
    if( filePath.find('.') == std::string::npos )
    {
        return filePath + '.' + defaultExtensionWithoutDot;
    }
    return filePath;
}



bool ShowOpenFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, std::string *out_filePathChosen )
{
	// OPEN FILE DIALOG

    auto result = false;

	auto dialog = gtk_file_chooser_dialog_new(
        dialogTitle,
        parentWindow,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL,   GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN,     GTK_RESPONSE_ACCEPT,
        NULL );

    auto gtkFileChooser = GTK_FILE_CHOOSER( dialog );

    AddMicrosoftStyleFilters( gtkFileChooser, fileSpecs );

	if( gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT )
    {
        auto fileName = gtk_file_chooser_get_filename( gtkFileChooser );
        *out_filePathChosen = std::string( fileName );
        g_free( fileName );
        result = true;
    }

    gtk_widget_destroy( dialog );

	return result;
}



bool ShowSaveFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, const char *defaultExtension, std::string *out_filePathChosen )
{
    auto result = false;

    auto dialog = gtk_file_chooser_dialog_new (
        dialogTitle,
        parentWindow,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL,  GTK_RESPONSE_CANCEL,
        GTK_STOCK_SAVE,    GTK_RESPONSE_ACCEPT,
        NULL);

    auto gtkFileChooser = GTK_FILE_CHOOSER( dialog );

    AddMicrosoftStyleFilters( gtkFileChooser, fileSpecs );

    gtk_file_chooser_set_do_overwrite_confirmation( gtkFileChooser, TRUE );

    if (gtk_dialog_run( GTK_DIALOG( dialog ) ) == GTK_RESPONSE_ACCEPT )
    {
        auto fileName = gtk_file_chooser_get_filename( gtkFileChooser );
        *out_filePathChosen = AppendIfExtensionMissing( std::string(fileName), defaultExtension );
        g_free( fileName );
        result = true;
    }

    gtk_widget_destroy( dialog );

	return result;
}



