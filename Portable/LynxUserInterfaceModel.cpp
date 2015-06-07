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

#include <assert.h>
#include "LynxUserInterfaceModel.h"
#include "LynxHardware.h"
#include "LynxEmulatorGuest.h"
#include "FileSerialiser.h"


namespace Jynx
{
	LynxUserInterfaceModel::LynxUserInterfaceModel( IViewServicesForLynxUserInterfaceModel *hostView, uint16_t *soundBuffer, size_t numSamples, const char *platformEndOfLineSequenceUTF8, bool gamesMode )
		: _renderStyle( RenderStyle::SquarePixels )
		, _hostView( hostView )
		, _soundEnable( true )
		, _showFullScreen( false )
		, _platformEndOfLineSequenceUTF8(platformEndOfLineSequenceUTF8)
		, _emulatorWantsUIStatusUpdate(false)
		, _gamesMode(gamesMode)
	{
		JynxZ80::Z80::InitialiseGlobalTables();  // Not absolutely ideal place to put this.

		// Reminder - Creating the LynxEmulatorGuest will create the EMULATOR thread.
		_lynxEmulator = std::unique_ptr<LynxEmulatorGuest>( new LynxEmulatorGuest( this, soundBuffer, numSamples, LynxMachineType::LYNX_48K, platformEndOfLineSequenceUTF8 ) );
	}



	void  LynxUserInterfaceModel::OnInitDialog()
	{
		// The term "OnInitDialog" is borrowed from Windows - a second stage initialisation after construction.
		// The constructor is called BEFORE the window is created with the window manager (thus no window handle).
		// The OnInitDialog is called for extra initialisation as soon as the window is created with the window manager.
		try
		{
			LoadUserSettings();
		}
		catch( const std::invalid_argument & ) // parse error
		{
			// TODO: reportable.
		}
		catch( std::ifstream::failure &e )
		{
			// TODO: check for file not found, and ignore this.  Anything else is reportable.
		}

		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::NotifyAllKeysUp()                 { _lynxEmulator->NotifyAllKeysUp(); }
	void LynxUserInterfaceModel::NotifyKeyDown( int32_t keyCode )  { _lynxEmulator->NotifyKeyDown( keyCode); }
	void LynxUserInterfaceModel::NotifyKeyUp( int32_t keyCode )    { _lynxEmulator->NotifyKeyUp( keyCode ); }




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     PAINT ASSISTANCE - CALCULATING LAYOUT
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



	LynxRectangle  LynxUserInterfaceModel::GetProjectionRectangle() const
	{
		//
		// Return the area into which the emulator screen should render.
		// This depends on whether the user has selected to force square pixels.
		//

		auto r = _hostView->GetClientRectangle();

		if( _renderStyle == RenderStyle::FitToWindow )
		{
			return GetBestFitProjectionArea( r.right, r.bottom );
		}
		else if( _renderStyle == RenderStyle::SquarePixels )
		{
			// TODO: Really need "fit rectangle in rectangle" function to generalised for rectangles.  Is OK for now, since lynx screen is square.
			return GetSquarePixelsProjectionArea( r.right, r.bottom, LYNX_FRAMEBUF_HEIGHT );
		}
		else
		{
			assert( _renderStyle == RenderStyle::FillWindow );
			return r;
		}
	}




	void LynxUserInterfaceModel::OnPaint()
	{
		// The model provides a paint algorithm, which really amounts to positioning assistance.
		// (Models don't usually paint, but heck all views will be using pixels!)

		auto clientRect = _hostView->GetClientRectangle();   // TODO: As it stands, this MUST have a top-left of (0,0)!  Which on an MS-Windows client area it does.  Other hosts may have to do translation.  Or we make this routine more general.
		assert( clientRect.left == 0 );  // TODO: See TODO comment above
		assert( clientRect.top == 0 );   // TODO: See TODO comment above

		auto projectionRectangle = GetProjectionRectangle();

		auto w = LYNX_FRAMEBUF_WIDTH;
		auto h = LYNX_FRAMEBUF_HEIGHT;

		auto projWidth  = projectionRectangle.right - projectionRectangle.left;
		auto projHeight = projectionRectangle.bottom - projectionRectangle.top;
		auto projOffsetX6845 = (projWidth * _lynxEmulator->Get6845OffsetPixelsX()) / w;
		auto projOffsetY6845 = (projHeight * _lynxEmulator->Get6845OffsetPixelsY()) / h;
		bool isShifted = projOffsetX6845 != 0 || projOffsetY6845 != 0;

		// CONTENT
		if( ! isShifted )
		{
			// FOR SPEED - This is the "straight on" display, 6845 R12 and R13 are both zero.
			_hostView->StretchBlitTheGuestScreen( projectionRectangle.left, projectionRectangle.top, projWidth, projHeight );
		}
		else
		{
			// THe 6845 address registers R12,R13 have shifted the display.

			_hostView->SetViewport( projectionRectangle.left, projectionRectangle.top, projWidth, projHeight );

			auto tileSlideUpAmount = (4 * projHeight) / h;  // 4 = 6845 num scanlines per char  TODO: sort out

			// Origin offsetting:
			auto ox = projectionRectangle.left - projOffsetX6845;
			auto oy = projectionRectangle.top - projOffsetY6845;

			// TOP LEFT TILE
			_hostView->StretchBlitTheGuestScreen( ox, oy, projWidth, projHeight );

			// TOP RIGHT TILE
			_hostView->StretchBlitTheGuestScreen( ox + projWidth, oy - tileSlideUpAmount, projWidth, projHeight );

			// BOTTOM LEFT TILE
			_hostView->StretchBlitTheGuestScreen( ox, oy + projHeight, projWidth, projHeight );

			// BOTTOM RIGHT TILE
			_hostView->StretchBlitTheGuestScreen( ox + projWidth, (oy - tileSlideUpAmount) + projHeight, projWidth, projHeight );

			_hostView->CancelViewport();
		}

		// ABOVE
		_hostView->FillBlackRectangle( 0, 0, clientRect.right, projectionRectangle.top );

		// LEFT
		_hostView->FillBlackRectangle( 0, projectionRectangle.top, projectionRectangle.left, projHeight );

		// RIGHT
		_hostView->FillBlackRectangle( projectionRectangle.right, projectionRectangle.top, clientRect.right - projectionRectangle.right, projHeight );

		// UNDER
		_hostView->FillBlackRectangle( 0, projectionRectangle.bottom, clientRect.right, clientRect.bottom - projectionRectangle.bottom );
	}



	void LynxUserInterfaceModel::OnTimer()
	{
		// (Called on the MAIN thread)

		_lynxEmulator->CallMeBackToInvalidateRegions();

		if( _emulatorWantsUIStatusUpdate )
		{
			_emulatorWantsUIStatusUpdate = false;
			UpdateUserInterfaceElementsOnView();
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     EXCEPTION SAFETY FOR UI
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	class EnsureUIUpdated
	{
	public:

		// Destructor ensures UI is refreshed in event of exception.

		EnsureUIUpdated( LynxUserInterfaceModel *model )   : _model(*model) {}
		~EnsureUIUpdated()                                 { _model.UpdateUserInterfaceElementsOnView(); }

	private:

		LynxUserInterfaceModel &_model;

	};







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     UI ELEMENT EVENT HANDLERS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	bool LynxUserInterfaceModel::CanRiskLosingModifiedTape() const
	{
		if( _lynxEmulator->CanSaveTAPFile() )
		{
			return _hostView->AskYesNoQuestion( "This action will lose the unsaved tape!\n\nDo you wish to proceed?", "Warning" );
		}
		return true;  // Tape not modified anyway.
	}



    void LynxUserInterfaceModel::ReportWithPrefix( const char *messageToPrefix, const std::exception &e )
    {
        std::string message;
        message += messageToPrefix;
        message += e.what();
        _hostView->TellUser( message.c_str(), "Error" );
    }



	void LynxUserInterfaceModel::OnExit()
	{
		// The View calls this to handle the case where the "exit" button/menu/close box has been selected in the UI.

		// If a snapshot file was issued on the command line, we assume its "games mode" so we do NOT pester
		// the user for shutdown confirmation:

		if( ! _gamesMode )
		{
			if( ! _hostView->AskYesNoQuestion( "Are you sure you want to close the Lynx emulator?", "Warning" ) ) return;
		}

		if( ! CanRiskLosingModifiedTape() ) return;

        DoWithFileHandlingErrorReportsToUser( "Failed to save the WAVE file!  The file may be incomplete.  ", [&]()
        {
			_lynxEmulator->FinishRecordingSoundToFile();
		} );

        DoWithFileHandlingErrorReportsToUser( "Failed to save the text file!  The file may be incomplete.  ", [&]()
        {
			_lynxEmulator->FinishRecordingLynxTextToFile();
		} );

        DoWithFileHandlingErrorReportsToUser( "Failed to save the user options file!  ", [&]()
        {
			SaveUserSettings();
		} );

		_hostView->CloseDownNow();
	}



	void LynxUserInterfaceModel::OnNewAudioTape()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( CanRiskLosingModifiedTape() )
		{
			_lynxEmulator->InsertBlankTape();
			UpdateUserInterfaceElementsOnView();
		}
	}



	void LynxUserInterfaceModel::OnRunTAPFile()
	{
		auto fileOpener = _hostView->ShowOpenFileDialog( LoadableFileTypes::TAP );
		if( fileOpener )
		{
			EnsureUIUpdated uiUpdater(this);
			DoWithFileHandlingErrorReportsToUser( "", [&]()
            {
                _lynxEmulator->RunExistingTAPFile( fileOpener.get() ); // throws
            } );
		}
	}



	void LynxUserInterfaceModel::OnOpenTAPFile()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( CanRiskLosingModifiedTape() )
		{
			auto fileOpener = _hostView->ShowOpenFileDialog( LoadableFileTypes::TAP );
			if( fileOpener )
			{
				EnsureUIUpdated uiUpdater(this);
                DoWithFileHandlingErrorReportsToUser( "", [&]()
                {
                    _lynxEmulator->LoadExistingTAPFile( fileOpener.get() ); // throws
                } );
			}
		}
	}



	void LynxUserInterfaceModel::OnSaveTAPFileAs()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::TAP );
		if( fileOpener )
		{
			if( ! _lynxEmulator->SaveTape( fileOpener.get() ) )
            {
                _hostView->TellUser( "The Lynx has not saved anything onto the tape yet.", "Note" );
            }
		}
	}



	void LynxUserInterfaceModel::OnRewindAudioTape()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->RewindTape();
		if( ! _gamesMode )
		{
			_hostView->TellUser( "Tape rewound.", "Note" );
		}
	}



	void LynxUserInterfaceModel::OnFitToWindow()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		SetRenderStyle( RenderStyle::FitToWindow );
	}



	void LynxUserInterfaceModel::OnSquarePixels()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		SetRenderStyle( RenderStyle::SquarePixels );
	}



	void LynxUserInterfaceModel::OnFillWindow()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		SetRenderStyle( RenderStyle::FillWindow );
	}



	void LynxUserInterfaceModel::OnEmulation48K()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( UserAllowsReset() )
		{
			SetMachineTypeAndReset( LynxMachineType::LYNX_48K );
		}
	}



	void LynxUserInterfaceModel::OnEmulation96K()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( UserAllowsReset() )
		{
			SetMachineTypeAndReset( LynxMachineType::LYNX_96K );
		}
	}



	void LynxUserInterfaceModel::OnEmulation96KScorpion()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( UserAllowsReset() )
		{
			SetMachineTypeAndReset( LynxMachineType::LYNX_96K_Scorpion );
		}
	}



	void LynxUserInterfaceModel::OnResetEmulation()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( UserAllowsReset() )
		{
			SetMachineTypeAndReset( _lynxEmulator->GetMachineType() );  // ie: don't change machine type
		}
	}



	void LynxUserInterfaceModel::OnLoadStateSnapshot()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowOpenFileDialog( LoadableFileTypes::Snapshot );
		if( fileOpener )
		{
			if( UserAllowsReset() )
			{
				EnsureUIUpdated uiUpdater(this);
				_lynxEmulator->LoadState( fileOpener.get() );  // throws
			}
		}
	}



	void LynxUserInterfaceModel::ForceLoadSpecificSnapshot( IFileOpener *fileOpener )
	{
		EnsureUIUpdated uiUpdater(this);
		_lynxEmulator->LoadState( fileOpener );  // throws
	}



	void LynxUserInterfaceModel::ForceLoadSpecificTape( IFileOpener *fileOpener )
	{
		EnsureUIUpdated uiUpdater(this);
		_lynxEmulator->RunExistingTAPFile( fileOpener );  // throws
	}



	void LynxUserInterfaceModel::OnSaveStateSnapshot()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( _lynxEmulator->CanSaveSnapshot() )
		{
			auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::Snapshot );
			if( fileOpener )
			{
				EnsureUIUpdated uiUpdater(this); // TODO: Is this really needed or was it a copy-paste error?
				if( ! _lynxEmulator->SaveState( fileOpener.get() ) )
                {
                    _hostView->TellUser( "Cannot save a snapshot while the cassette is in operation.", "Note" );
                }
			}
		}
		else
		{
			_hostView->TellUser( "Cannot save a snapshot while the tape is in operation!", "Note" );
		}
	}


	void LynxUserInterfaceModel::OnListenToTapeSounds()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->SetTapeSounds( ! _lynxEmulator->GetTapeSounds() );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnRecordToFile()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::Sound );
		if( fileOpener )
		{
			EnsureUIUpdated uiUpdater(this);
			_lynxEmulator->RecordSoundToFile( fileOpener.get() ); // throws
		}
	}



	void LynxUserInterfaceModel::OnFinishRecording()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->FinishRecordingSoundToFile();
		UpdateUserInterfaceElementsOnView();
	}




	void LynxUserInterfaceModel::OnRecordLynxTextToFile()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::LynxText );
		if( fileOpener )
		{
			EnsureUIUpdated uiUpdater(this);
			_lynxEmulator->RecordLynxTextToFile( fileOpener.get() ); // throws
		}
	}



	void LynxUserInterfaceModel::OnFinishRecordingLynxText()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->FinishRecordingLynxTextToFile();
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnTypeInTextFromFile()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowOpenFileDialog( LoadableFileTypes::Text );
		if( fileOpener )
		{
			EnsureUIUpdated uiUpdater(this);
			_lynxEmulator->TypeInTextFromFile( fileOpener.get() );  // throws
		}
	}



	void LynxUserInterfaceModel::OnLynxBasicRemCommandExtensions()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto currentState = _lynxEmulator->GetLynxRemCommandExtensionsEnabled();
		_lynxEmulator->SetLynxRemCommandExtensionsEnabled( ! currentState );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnPause()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto currentState = _lynxEmulator->GetPauseMode();
		_lynxEmulator->SetPauseMode( ! currentState );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnPauseAfterTapLoad()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto currentState = _lynxEmulator->GetPauseAfterTapLoadEnable();
		_lynxEmulator->SetPauseAfterTapLoadEnable( ! currentState );
		UpdateUserInterfaceElementsOnView();
	}



	bool LynxUserInterfaceModel::UserAllowsReset()
	{
		if( _gamesMode ) return true;
		return _hostView->AskYesNoQuestion( "This action will RESET the emulator!\n\nDo you wish to proceed?", "Warning" );
	}




	void LynxUserInterfaceModel::OnSetCycles( LynxZ80Cycles::Enum cyclesEnum )
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->SetCyclesPerTimeslice( cyclesEnum );
		UpdateUserInterfaceElementsOnView();
	}




	void LynxUserInterfaceModel::OnEnableDisableSound()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_soundEnable = ! _soundEnable;
		UpdateUserInterfaceElementsOnView();
	}



	bool LynxUserInterfaceModel::IsSoundEnabled()
	{
		// The host calls this when it wants to know if sound is enabled.
		// (I admit this is a trivial service for the model to be supplying, but it unifies all future hosts).
		return _soundEnable;
	}



	void LynxUserInterfaceModel::OnEnableDisableFullScreen()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_showFullScreen = ! _showFullScreen;
		UpdateUserInterfaceElementsOnView();
	}



	bool LynxUserInterfaceModel::IsFullScreenEnabled()
	{
		// The view calls this when it wants to know if full screen mode is enabled.
		// (I admit this is a trivial service for the model to be supplying, but it unifies all future hosts).
		return _showFullScreen;
	}


	void LynxUserInterfaceModel::OnTypeTapeDirectoryIntoLynx()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).
		_lynxEmulator->TypeTapeDirectoryIntoLynx();
	}



	void LynxUserInterfaceModel::OnSpeedMaxCassette()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).
		_lynxEmulator->SetEnableSpeedMaxModeWhenUsingCassette( ! _lynxEmulator->GetEnableSpeedMaxModeWhenUsingCassette() );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnSpeedMaxConsoleCommands()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).
		_lynxEmulator->SetEnableSpeedMaxModeWhenInBetweenConsoleCommands( ! _lynxEmulator->GetEnableSpeedMaxModeWhenInBetweenConsoleCommands() );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnSpeedMaxPermanently()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).
		_lynxEmulator->SetEnableSpeedMaxModeBecauseUserWantsItPermanently( ! _lynxEmulator->GetEnableSpeedMaxModeBecauseUserWantsItPermanently() );
		UpdateUserInterfaceElementsOnView();
	}



	void LynxUserInterfaceModel::OnChangeColourSet( LynxColourSet::Enum colourSet )
	{
		_lynxEmulator->SetLynxColourSet( colourSet );
		UpdateUserInterfaceElementsOnView();
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//        MODEL (HOST) SERVICES TO GUEST (EMULATOR)  (IHostServicesForLynxEmulator)
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void  LynxUserInterfaceModel::InvalidateAreaOfGuestScreen_OnMainThread( int left, int top, int right, int bottom )
	{
		// (Called on MAIN thread only)

		// The entry coordinates are on the Lynx's screen, and we translate to pixel
		// coordinates on the host display.  (All hosts will use square pixels these days!).

		// Inform the host that the area needs to be re-painted.  On MS-WIndows the "InvalidateRect"
		// function is used, and I suggest this technique be used on other platforms.

		auto projectionRectangle = GetProjectionRectangle();
		auto projectionWidth     = projectionRectangle.right - projectionRectangle.left;
		auto projectionHeight    = projectionRectangle.bottom - projectionRectangle.top;

		left   = (left   * projectionWidth)  / LYNX_FRAMEBUF_WIDTH;
		right  = (right  * projectionWidth)  / LYNX_FRAMEBUF_WIDTH;
		top    = (top    * projectionHeight) / LYNX_FRAMEBUF_HEIGHT;
		bottom = (bottom * projectionHeight) / LYNX_FRAMEBUF_HEIGHT;

		LynxRectangle  scaledRect;
		scaledRect.left   = left   + projectionRectangle.left;
		scaledRect.right  = right  + projectionRectangle.left + 1;
		scaledRect.top    = top    + projectionRectangle.top;
		scaledRect.bottom = bottom + projectionRectangle.top + 1;

		_hostView->InvalidateAreaOfHostScreen( scaledRect );
	}



	void  LynxUserInterfaceModel::OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired )
	{
		// (Called on MAIN thread only)

		// Just delegate this call.
		_hostView->OpenChipFileStream_OnMainThread( streamToBeOpened, openModeRequired, romRequired );
	}



	void  LynxUserInterfaceModel::NotifyOutputTapeAvailbilityChanged_OnAnyThread()
	{
		// (WARNING - Called on EITHER the emulator or main thread!)
		// (May be called with the EMULATOR suspended!)

		// - Reminder:  This cannot be done on the EMULATOR thread:  UpdateUserInterfaceElementsOnView();

		// So we have a mechanism to defer the UI refresh until the UI thread calls OnTimer().
		// We can afford to defer updating the menu grey-out status until then!
		_emulatorWantsUIStatusUpdate = true;
	}



	void  LynxUserInterfaceModel::PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData )
	{
		// (WARNING - Called on the EMULATOR thread, NOT the MAIN thread)

		// Just delegate this call.
		_hostView->PaintPixelsOnHostBitmap_OnEmulatorThread( addressOffset, eightPixelsData );
	}



	void LynxUserInterfaceModel::WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread()
	{
		_hostView->WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread();
	}



	void  LynxUserInterfaceModel::TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues )
	{
		_hostView->TranslateRGBXColourPaletteToHostValues( eightEntryColourPalette, eightEntryTranslatedValues );
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     EMULATION SPEEDS SUPPORTED BY THE USER INTERFACE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	template<typename VALUE>
	struct ValueToMenuItemMap
	{
		TickableInterfaceElements::Enum  interfaceItem;
		VALUE  enumerationValue;
	};

	template<typename VALUE, size_t N>
	void UpdateMenuExclusiveSelectionGroup( IViewServicesForLynxUserInterfaceModel *hostView, ValueToMenuItemMap<VALUE> (&mappingsArray)[N], VALUE memberToActivate )
	{
		for( int i=0; i < N; i++ )
		{
			bool foundThisTime = (memberToActivate == mappingsArray[i].enumerationValue);
			hostView->SetTickBoxState( mappingsArray[i].interfaceItem, foundThisTime );
		}
	}




	ValueToMenuItemMap<uint32_t>  CycleCountToMenuOptionLookupTable[5] =
	{
		TickableInterfaceElements::Speed50,     LynxZ80Cycles::At50,     //  2MHz
		TickableInterfaceElements::Speed100,    LynxZ80Cycles::At100,    //  4Mhz
		TickableInterfaceElements::Speed200,    LynxZ80Cycles::At200,    //  8Mhz
		TickableInterfaceElements::Speed400,    LynxZ80Cycles::At400,    // 16Mhz
		TickableInterfaceElements::Speed800,    LynxZ80Cycles::At800,    // 32Mhz
	};




	ValueToMenuItemMap<LynxColourSet::Enum>  ColourSetToMenuOptionLookupTable[5] =
	{
		TickableInterfaceElements::ColourSetNormalRGB,            LynxColourSet::NormalRGB,
		TickableInterfaceElements::ColourSetGreenOnly,            LynxColourSet::GreenOnly,
		TickableInterfaceElements::ColourSetLevel9,               LynxColourSet::Level9,
		TickableInterfaceElements::ColourSetBlackAndWhiteTV,      LynxColourSet::BlackAndWhiteTV,
		TickableInterfaceElements::ColourSetGreenScreenMonitor,   LynxColourSet::GreenScreenMonitor,
	};



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     INTERNAL FUNCTIONS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void LynxUserInterfaceModel::UpdateUserInterfaceElementsOnView()
	{
		//
		// Decide what we need the tick state to be for the menu items:
		//

		auto machineType = _lynxEmulator->GetMachineType();
		bool tick48K        = (machineType == LynxMachineType::LYNX_48K);
		bool tick96K        = (machineType == LynxMachineType::LYNX_96K);
		bool tick96KScorpion= (machineType == LynxMachineType::LYNX_96K_Scorpion);
		bool tickSquare     = (_renderStyle == RenderStyle::SquarePixels);
		bool tickFitWindow  = (_renderStyle == RenderStyle::FitToWindow);
		bool tickFillWindow = (_renderStyle == RenderStyle::FillWindow);
		bool tickTapeSounds = _lynxEmulator->GetTapeSounds();
		bool tickRemExtensions = _lynxEmulator->GetLynxRemCommandExtensionsEnabled();
		bool tickPaused        = _lynxEmulator->GetPauseMode();
		bool tickPauseAfterTap = _lynxEmulator->GetPauseAfterTapLoadEnable();
		bool tickMaxSpeedCassette = _lynxEmulator->GetEnableSpeedMaxModeWhenUsingCassette();
		bool tickMaxSpeedConsole  = _lynxEmulator->GetEnableSpeedMaxModeWhenInBetweenConsoleCommands();
		bool tickMaxSpeedAlways   = _lynxEmulator->GetEnableSpeedMaxModeBecauseUserWantsItPermanently();
		bool tickSound      = _soundEnable;
		bool tickFullScreen = _showFullScreen;

		//
		// Decide what menu items need to be greyed out:
		//

		bool greyRewind              = ! _lynxEmulator->CanRewindTape();
		bool greyFinishRec           = ! _lynxEmulator->IsRecordingSoundToFile();
		bool greySaveTapAs           = ! _lynxEmulator->CanSaveTAPFile();
		bool greyFinishTextRecording = ! _lynxEmulator->IsRecordingLynxTextToFile();

		//
		// Set the check mark against the menu items:
		//

		_hostView->SetTickBoxState( TickableInterfaceElements::Lynx48K, tick48K );
		_hostView->SetTickBoxState( TickableInterfaceElements::Lynx96K, tick96K );
		_hostView->SetTickBoxState( TickableInterfaceElements::Lynx96KScorpion, tick96KScorpion );
		_hostView->SetTickBoxState( TickableInterfaceElements::ListenToTapeSounds, tickTapeSounds );
		_hostView->SetTickBoxState( TickableInterfaceElements::FitToWindow, tickFitWindow );
		_hostView->SetTickBoxState( TickableInterfaceElements::FillWindow, tickFillWindow );
		_hostView->SetTickBoxState( TickableInterfaceElements::UseSquarePixels, tickSquare );
		_hostView->SetTickBoxState( TickableInterfaceElements::LynxBasicRemCommandExtensions, tickRemExtensions );
		_hostView->SetTickBoxState( TickableInterfaceElements::SoundEnableDisable, tickSound );
		_hostView->SetTickBoxState( TickableInterfaceElements::ShowFullScreen, tickFullScreen );
		_hostView->SetTickBoxState( TickableInterfaceElements::Paused, tickPaused );
		_hostView->SetTickBoxState( TickableInterfaceElements::PausedAfterTapLoad, tickPauseAfterTap );
		_hostView->SetTickBoxState( TickableInterfaceElements::MaxSpeedCassette, tickMaxSpeedCassette );
		_hostView->SetTickBoxState( TickableInterfaceElements::MaxSpeedConsole,  tickMaxSpeedConsole );
		_hostView->SetTickBoxState( TickableInterfaceElements::MaxSpeedAlways,   tickMaxSpeedAlways );

		_hostView->SetEnabledState( ButtonInterfaceElements::RewindTape, greyRewind );
		_hostView->SetEnabledState( ButtonInterfaceElements::FinishRecording, greyFinishRec );
		_hostView->SetEnabledState( ButtonInterfaceElements::SaveTape, greySaveTapAs );
		_hostView->SetEnabledState( ButtonInterfaceElements::FinishLynxTextRecording, greyFinishTextRecording );

		//
		// Set the speed tickbox by looking up the number of cycles in the lookup table:
		// - If the user tampers with the timeslice length in the serialisation, then no checks will show,
		//   but we'll honour the tampered speed setting!
		//

		UpdateMenuExclusiveSelectionGroup( _hostView, CycleCountToMenuOptionLookupTable, _lynxEmulator->GetCyclesPerTimeslice() );
		UpdateMenuExclusiveSelectionGroup( _hostView, ColourSetToMenuOptionLookupTable, _lynxEmulator->GetLynxColourSet() );
	}




	void LynxUserInterfaceModel::SetRenderStyle( RenderStyle::Enum newStyle )
	{
		if( newStyle != _renderStyle )
		{
			_renderStyle = newStyle;
			_hostView->InvalidateAreaOfHostScreen( _hostView->GetClientRectangle() );
			UpdateUserInterfaceElementsOnView();
		}
	}



	void LynxUserInterfaceModel::SetMachineTypeAndReset( LynxMachineType::Enum machineType )
	{
		_lynxEmulator->ResetGuest( machineType );
		UpdateUserInterfaceElementsOnView();
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//     SERIALISE USER SETTINGS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void LynxUserInterfaceModel::SaveUserSettings()
	{
		// TODO: Failure to save user settings must not disrupt anything else, eg: closedown
		// TODO: Can we save to a temporary and re-name on success?
		auto fileOpener = _hostView->GetUserSettingsFilePath();
		if( fileOpener != nullptr )
		{
			UserSettings userSettings(
				_lynxEmulator->GetMachineType(),
				_renderStyle,
				_soundEnable,
				_showFullScreen,
				_lynxEmulator->GetCyclesPerTimeslice(),
				_lynxEmulator->GetTapeSounds(),
				_lynxEmulator->GetLynxRemCommandExtensionsEnabled(),
				_lynxEmulator->GetEnableSpeedMaxModeWhenUsingCassette(),
				_lynxEmulator->GetEnableSpeedMaxModeWhenInBetweenConsoleCommands(),
				_lynxEmulator->GetEnableSpeedMaxModeBecauseUserWantsItPermanently(),
				_lynxEmulator->GetLynxColourSet() );

			userSettings.SaveToFile( &*fileOpener, _platformEndOfLineSequenceUTF8 );
		}
	}



	void LynxUserInterfaceModel::LoadUserSettings()
	{
		auto fileOpener = _hostView->GetUserSettingsFilePath();
		if( fileOpener != nullptr )
		{
            UserSettings userSettings( &*fileOpener );

            // Now that the file has loaded successfully, we know we can use the information in it!
            // (Or, of course, the defaults that the UserSettings object applies, if it's down-level version).
            _renderStyle = userSettings.GetRenderStyle();
            _soundEnable = userSettings.GetSoundEnable();
            _showFullScreen = userSettings.GetFullScreenEnable();
            _lynxEmulator->SetCyclesPerTimeslice( userSettings.GetCyclesPerTimeslice() );
            _lynxEmulator->SetTapeSounds( userSettings.GetTapeSounds() );
            _lynxEmulator->SetLynxRemCommandExtensionsEnabled( userSettings.GetRemExtensions() );
            _lynxEmulator->SetEnableSpeedMaxModeWhenUsingCassette( userSettings.GetMaxSpeedCassette() );
            _lynxEmulator->SetEnableSpeedMaxModeWhenInBetweenConsoleCommands( userSettings.GetMaxSpeedConsole() );
            _lynxEmulator->SetEnableSpeedMaxModeBecauseUserWantsItPermanently( userSettings.GetMaxSpeedAlways() );
            _lynxEmulator->SetLynxColourSet( userSettings.GetColourSet() );

			_lynxEmulator->ResetGuest( userSettings.GetMachineType() );

			// Update UI:
			_hostView->InvalidateAreaOfHostScreen( _hostView->GetClientRectangle() );
			UpdateUserInterfaceElementsOnView();
		}
	}


} // end namespace Jynx
