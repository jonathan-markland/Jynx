
#include <stdlib.h>
#include <stdint.h>
#include <string>

#include <gtk/gtk.h>

#include "LinuxMainForm.h"









int main (int argc, char *argv[])
{
    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    auto mainWindow = std::make_shared<MainForm>();
    mainWindow->ShowAll();

//  button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
//  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (helloWorld), (gpointer) win);
//  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

//  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
//  g_signal_connect (button, "clicked", gtk_main_quit, NULL);
//  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

    /* Enter the main loop */
    gtk_main();
    return 0;
}
