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

#pragma once

#include "../Portable/LynxUserInterfaceModel.h"
#include "../Portable/IViewServicesForLynxUserInterfaceModel.h"
#include "../Portable/WaveOutputStream.h"

#include <gtk/gtk.h>

#include "LinuxGtkMenu.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     MAIN FORM
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class MainForm: public Jynx::IViewServicesForLynxUserInterfaceModel, public InterfaceForMenuItemClickHandling
{
public:

    MainForm( const std::vector<std::string> &paramsList, const char *exePath );
	~MainForm();

    void ShowAll();

	// Interface for model:
	virtual void ShowTheAboutBox() override;
	virtual void CloseDownNow() override;
	virtual std::shared_ptr<Jynx::IFileOpener> ShowOpenFileDialog( Jynx::LoadableFileTypes::Enum ) override;  // return nullptr if cancelled, else return IFileOpener for the file selected.
	virtual std::shared_ptr<Jynx::IFileOpener> ShowSaveFileDialog( Jynx::SaveableFileTypes::Enum ) override;  // return nullptr if cancelled, else return IFileOpener for the file selected.
	virtual void TellUser( const char *messageText, const char *captionText ) override;
	virtual bool AskYesNoQuestion( const char *questionText, const char *captionText ) override;
	virtual void SetTickBoxState( Jynx::TickableInterfaceElements::Enum itemToSet, bool tickState ) override;
	virtual void SetEnabledState( Jynx::ButtonInterfaceElements::Enum itemToSet, bool enableState ) override;
	virtual Jynx::LynxRectangle GetClientRectangle() override;
	virtual void SetViewport( int left, int top, int width, int height ) override;
	virtual void CancelViewport() override;
	virtual void StretchBlitTheGuestScreen( int left, int top, int width, int height ) override;
	virtual void FillBlackRectangle( int left, int top, int width, int height ) override;
	virtual void InvalidateAreaOfHostScreen( const Jynx::LynxRectangle &area ) override;
	virtual void OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, Jynx::LynxRoms::Enum romRequired ) override;  // Host must open the INPUT stream for BINARY, position at BEGINNING.
	virtual void TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues ) override;
	virtual void PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData ) override;
	virtual std::shared_ptr<Jynx::IFileOpener>  GetUserSettingsFileOpener() override;

    // InterfaceForMenuItemClickHandling:
    virtual void NotifyMenuItemClicked( uint32_t menuItemID ) override;

private:

    void Cleanup();

    static gint GtkHandlerForCloseBoxDeleteEvent(          GtkWidget *widget, GdkEvent *event, gpointer userObject ); // static member

    static gint GtkHandlerForDrawingAreaExposeEvent(       GtkWidget *widget, GdkEventExpose *event, gpointer userObject );    // static member   expose_event
    static gint GtkHandlerForDrawingAreaButtonPressEvent(  GtkWidget *widget, GdkEventButton *event, gpointer userObject );    // static member   button_press_event

    static gboolean GtkHandlerForTheTimer( gpointer userObject );    // static member

    static gboolean GtkHandlerForKeyPress(   GtkWidget *widget, GdkEvent *event, gpointer user_data ); // static member
    static gboolean GtkHandlerForKeyRelease( GtkWidget *widget, GdkEvent *event, gpointer user_data ); // static member

private:

    template<typename RETURN_TYPE, typename OPERATION>
    RETURN_TYPE  DoWithTerminationOnStdException( OPERATION operation )
    {
        try
        {
            return operation();
        }
        catch( std::exception &e )
        {
            exit(1);
            throw; // never executed
        }
    }

private:

	std::string  _settingsFilePath;
	std::string  _exePath;

	std::unique_ptr<Jynx::LynxUserInterfaceModel> _lynxUIModel;  // Reminder - Emulator is within this.

private:

    // GTK stuff

    GtkWindow *_gtkWindow         = nullptr;
    GtkWidget *_gtkWindowAsWidget = nullptr;
    GtkWidget *_gtkDrawingArea    = nullptr;
    GtkWidget *_vbox              = nullptr;

    std::shared_ptr<LinuxGtkMenuBar>   _menuBar;

    std::shared_ptr<LinuxGtkMenu>  _menuFile;
    std::shared_ptr<LinuxGtkMenu>  _menuSpeed;
    std::shared_ptr<LinuxGtkMenu>  _menuEmulation;
    std::shared_ptr<LinuxGtkMenu>  _menuDisplay;
    std::shared_ptr<LinuxGtkMenu>  _menuSound;
    std::shared_ptr<LinuxGtkMenu>  _menuText;
    std::shared_ptr<LinuxGtkMenu>  _menuHelp;

    GdkPixbuf *_pixBuf;  // The Jynx screen
    void     *_pixBufBaseAddress      = nullptr;
    uint32_t  _pixBufBytesPerScanLine = 0;

    cairo_t  *_cairoContext              = nullptr;  // only set within lifetime of expose event
    bool      _originalCairoContextSaved = false;

    guint     _gtkTimerId                = 0;

};


