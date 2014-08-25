//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2014  Jonathan Markland
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

#include "mmsystem.h"
#include "resource.h"
#include "libWinApi.h"
#include "SoundThread.h"

#include "../Portable/ILynxUserInterfaceModel.h"
#include "../Portable/IHostServicesForLynxUserInterfaceModel.h"





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     MAIN FORM
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class MainForm: public libWinApi::BaseForm
	, public Jynx::IHostServicesForLynxUserInterfaceModel
{
public:

	MainForm( HWND hWndOwner );
	enum { IDD = IDD_MAINFORM };
	virtual ~MainForm() override;

	// libWinApi::BaseForm overrides:
	virtual bool OnInitDialog() override;
	virtual void WindowProc( libWinApi::WindowProcArgs &e ) override;
	virtual bool PreProcessMessage( libWinApi::Message *pMsg ) override;
	virtual void OnCancel() override;

	// Interface for model:
	virtual void CloseDownNow() override;
	virtual std::shared_ptr<Jynx::IFileOpener> ShowOpenFileDialog( Jynx::LoadableFileTypes::Enum ) override;  // return nullptr if cancelled, else return IFileOpener for the file selected.
	virtual std::shared_ptr<Jynx::IFileOpener> ShowSaveFileDialog( Jynx::SaveableFileTypes::Enum ) override;  // return nullptr if cancelled, else return IFileOpener for the file selected.
	virtual void TellUser( const char *messageText, const char *captionText ) override;
	virtual bool AskYesNoQuestion( const char *questionText, const char *captionText ) override;
	virtual void SetTickBoxState( Jynx::TickableInterfaceElements::Enum itemToSet, bool tickState ) override;
	virtual void SetEnabledState( Jynx::ButtonInterfaceElements::Enum itemToSet, bool enableState ) override;
	virtual Jynx::LynxRectangle GetClientRectangle() override;
	virtual void StretchBlitTheGuestScreen( int left, int top, int width, int height ) override;
	virtual void FillBlackRectangle( int left, int top, int width, int height ) override;
	virtual void InvalidateAreaOfHostScreen( const Jynx::LynxRectangle &area ) override;
	virtual void  OpenChipFileStream( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, Jynx::LynxRoms::Enum romRequired ) override;  // Host must open the INPUT stream for BINARY, position at BEGINNING.
	virtual void  LynxScreenAddressUpdated( uint32_t addressOffset, uint32_t lynxRedByte, uint32_t lynxGreenByte, uint32_t lynxBlueByte ) override;
	virtual std::string  GetPlatformEndOfLineSequence() override;
	virtual std::shared_ptr<Jynx::IFileOpener>  GetUserSettingsFilePath() override;

protected:

	void OnPaint( HDC dc );
	void OnLoadStateSnapshot();
	void OnSaveStateSnapshot();
	void OnEmulation48K();
	void OnEmulation96K();
	void OnResetEmulation();
	void OnFitToWindow();
	void OnSquarePixels();
	void OnRewindAudioTape();
	void OnNewAudioTape();
	void OnOpenTAPFile();
	void OnSaveTAPFileAs();
	void OnAbout();
	void OnListenToTapeSounds();
	void OnRecordToFile();
	void OnFinishRecording();
	void OnLynxBasicRemCommandExtensions();
	void OnEnableDisableSound();

private:

	bool CanRiskLosingModifiedTape() const;
	bool UserAllowsReset();
	void LoadSnapshot( const wchar_t *pathName );
	void SaveSnapshot( const wchar_t *pathName );
	void SetCycles( Jynx::LynxZ80Cycles::Enum cyclesEnum );
	void SelectTimingMechanism();
	void OnSound();

	HDC     _dc;         // Is only set when asking the model to paint.
	HANDLE  _hbicon;
	HANDLE  _hsicon;

	Jynx::ILynxUserInterfaceModel    *_lynxUIModel;  // Reminder - Emulator is within this.
	libWinApi::FrameBufferInfo        _screenInfo;

	HBITMAP                _guestScreenBitmap;
	MMRESULT               _timeBeginPeriodResult;
	SoundThread            _soundThread;
	UINT_PTR               _timerID;

};

