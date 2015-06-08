
#pragma once

#include <string>
#include <gtk/gtk.h>

bool ShowOpenFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, std::string *out_filePathChosen );
bool ShowSaveFileDialog( GtkWindow *parentWindow, const char *dialogTitle, const char *fileSpecs, const char *defaultExtension, std::string *out_filePathChosen );
