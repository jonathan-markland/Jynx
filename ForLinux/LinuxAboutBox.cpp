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

#include <gtk/gtk.h>

namespace Jynx
{
    void ShowLinuxGtkAboutBox()
    {
        // auto pixbuf = gdk_pixbuf_new_from_file( "battery.png", NULL );  // TODO: Statically link
        auto dialog = gtk_about_dialog_new();

        gtk_about_dialog_set_name(      GTK_ABOUT_DIALOG(dialog), "Jynx Emulator" );
        gtk_about_dialog_set_version(   GTK_ABOUT_DIALOG(dialog), "1.0.9" );
        gtk_about_dialog_set_copyright( GTK_ABOUT_DIALOG(dialog), "(c) 2015 Jonathan Markland" );
        gtk_about_dialog_set_comments(  GTK_ABOUT_DIALOG(dialog), "Emulator for the Camputers Lynx 48K/96K" );
        gtk_about_dialog_set_website(   GTK_ABOUT_DIALOG(dialog), "http://jynxemulator.wordpress.com" );

        // gtk_about_dialog_set_logo(      GTK_ABOUT_DIALOG(dialog), pixbuf);
        // g_object_unref(pixbuf);
        // pixbuf = NULL;

        gtk_dialog_run( GTK_DIALOG(dialog) );
        gtk_widget_destroy(dialog);
    }




    void ShowLinuxGtkAboutBoxEventHandler( GtkWidget *widget, gpointer data )
    {
        ShowLinuxGtkAboutBox();
    }
}
