
#include "LinuxGtkFileDialogs.h"

bool ShowOpenFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, std::string *out_filePathChosen )  // TODO: Filtering per MS Windows
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

	if( gtk_dialog_run( GTK_DIALOG (dialog) ) == GTK_RESPONSE_ACCEPT )
    {
        auto fileName = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER (dialog) );
        *out_filePathChosen = std::string(fileName);
        g_free( fileName );
        result = true;
    }

    gtk_widget_destroy( dialog );

	return result;
}



bool ShowSaveFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, const char *defaultExtension, std::string *out_filePathChosen )  // TODO: Filtering and default extension per MS Windows
{
    auto result = false;

    auto dialog = gtk_file_chooser_dialog_new (
        dialogTitle,
        parentWindow,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        GTK_STOCK_CANCEL,  GTK_RESPONSE_CANCEL,
        GTK_STOCK_SAVE,    GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER (dialog), TRUE );

/*
    if (TRUE) // user_edited_a_new_document) // TODO: fix
      {
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), "/home/jonathan/Documents");
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled document");
      }
    else
      gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), "filename_for_existing_document");
*/

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        auto fileName = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
        *out_filePathChosen = std::string(fileName);
        g_free( fileName );
        result = true;
    }

    gtk_widget_destroy( dialog );

	return result;
}



