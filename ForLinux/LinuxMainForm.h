
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

    MainForm( /*HWND hWndOwner,*/ const char *settingsFilePath, const char *snapshotFilePath, bool gamesMode, const char *tapFilePath, const char *exePath );
	~MainForm();

    void ShowAll();

/*

	// libWinApi::BaseForm overrides:
	virtual bool OnInitDialog() override;
	virtual void WindowProc( libWinApi::WindowProcArgs &e ) override;
	virtual bool PreProcessMessage( libWinApi::Message *pMsg ) override;
	virtual void OnCancel() override;
*/
	// Interface for model:
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
	virtual std::shared_ptr<Jynx::IFileOpener>  GetUserSettingsFilePath() override;
	virtual void WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread() override;

    // InterfaceForMenuItemClickHandling:
    virtual void NotifyMenuItemClicked( uint32_t menuItemID ) override;

private:

    void GtkConstruction();
    static gint GtkHandlerForCloseBoxDeleteEvent( GtkWidget *widget, GdkEvent *event, gpointer thisMainWindowObject ); // static member
    bool OnInitDialog();
    void OnAbout();
    void OnCancel();

private:

    // GTK stuff

    GtkWidget *_win;
    GtkWidget *_vbox;

    std::shared_ptr<LinuxGtkMenuBar>   _menuBar;

    std::shared_ptr<LinuxGtkMenu>  _menuFile;
    std::shared_ptr<LinuxGtkMenu>  _menuSpeed;
    std::shared_ptr<LinuxGtkMenu>  _menuEmulation;
    std::shared_ptr<LinuxGtkMenu>  _menuDisplay;
    std::shared_ptr<LinuxGtkMenu>  _menuSound;
    std::shared_ptr<LinuxGtkMenu>  _menuText;
    std::shared_ptr<LinuxGtkMenu>  _menuHelp;

private:


//	bool CanRiskLosingModifiedTape() const;
//	bool UserAllowsReset();
//	void LoadSnapshot( const wchar_t *pathName );
//	void SaveSnapshot( const wchar_t *pathName );
//	void SetCycles( Jynx::LynxZ80Cycles::Enum cyclesEnum );
//	void SelectTimingMechanism();
//	void OnPaint( HDC dc );
//	void OnAbout();
//	void OnSound();
//
//	HDC     _dc;         // Is only set when asking the model to paint.
//	HANDLE  _hbicon;
//	HANDLE  _hsicon;
//	int     _saveDC;
//
	std::unique_ptr<Jynx::LynxUserInterfaceModel> _lynxUIModel;  // Reminder - Emulator is within this.
//	libWinApi::FrameBufferInfo        _screenInfo;
//
//	HBITMAP                _guestScreenBitmap;
//	MMRESULT               _timeBeginPeriodResult;
//	MMRESULT               _timeSetEventResult;
//
    std::shared_ptr<WaveOutputStream>  _waveOutStream;
	std::vector<uint16_t>         _soundBuffer;
//
//	libWinApi::WindowStyleAndPositionInformation  _restorationAfterFullScreen;
//
	std::string  _settingsFilePath;
	std::string  _snapshotFilePath;
	std::string  _tapFilePath;
	std::string  _exePath;

	bool _gamesMode;

};


