
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
