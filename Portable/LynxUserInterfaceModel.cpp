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


namespace Jynx
{
	LynxUserInterfaceModel::LynxUserInterfaceModel( IHostServicesForLynxUserInterfaceModel *hostView, uint16_t *soundBuffer, size_t numSamples )
		: _renderStyle( RenderStyle::SquarePixels )
		, _machineType( LynxMachineType::LYNX_48K )
		, _lynxEmulator( nullptr )
		, _hostView( hostView )
		, _soundEnable( true )
	{
		JynxZ80::Z80::InitialiseGlobalTables();  // Not absolutely ideal place to put this.

		_lynxEmulator = new LynxEmulatorGuest( this );
		_lynxEmulator->SetSoundBufferForNextPeriod( soundBuffer, numSamples );
		_lynxEmulator->ResetGuest( _machineType );  // TODO: maybe have guest constructor do this?
	}




	void  LynxUserInterfaceModel::OnInitDialog()   
	{
		// The term "OnInitDialog" is borrowed from Windows - a second stage initialisation after construction.
		// The constructor is called BEFORE the window is created with the window manager (thus no window handle).
		// The OnInitDialog is called for extra initialisation as soon as the window is created with the window manager.
		UpdateUserInterfaceElementsOnView();
	}




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
		else 
		{
			// TODO: Really need "fit rectangle in rectangle" function to generalised for rectangles.  Is OK for now, since lynx screen is square.
			return GetSquarePixelsProjectionArea( r.right, r.bottom, LYNX_FRAMEBUF_HEIGHT ); 
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

		// CONTENT
		_hostView->StretchBlitTheGuestScreen( projectionRectangle.left, projectionRectangle.top, projWidth, projHeight );

		// ABOVE
		_hostView->FillBlackRectangle( 0, 0, clientRect.right, projectionRectangle.top );

		// LEFT
		_hostView->FillBlackRectangle( 0, projectionRectangle.top, projectionRectangle.left, projHeight );

		// RIGHT
		_hostView->FillBlackRectangle( projectionRectangle.right, projectionRectangle.top, clientRect.right - projectionRectangle.right, projHeight );

		// UNDER
		_hostView->FillBlackRectangle( 0, projectionRectangle.bottom, clientRect.right, clientRect.bottom - projectionRectangle.bottom );
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
		if( _lynxEmulator->IsTapeModified() )
		{
			return _hostView->AskYesNoQuestion( "This action will lose the unsaved tape!\n\nDo you wish to proceed?", "Warning" );
		}
		return true;  // Tape not modified anyway.
	}



	void LynxUserInterfaceModel::OnExit() 
	{
		// The View calls this to handle the case where the "exit" button/menu/close box has been selected in the UI.

		if( _hostView->AskYesNoQuestion( "Are you sure you want to close the Lynx emulator?", "Warning" ) )
		{
			if( CanRiskLosingModifiedTape() )
			{
				_lynxEmulator->FinishRecordingSoundToFile();
				_lynxEmulator->FinishRecordingLynxTextToFile();
				_hostView->CloseDownNow();
			}
		}
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



	void LynxUserInterfaceModel::OnOpenTAPFile()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( CanRiskLosingModifiedTape() )
		{
			auto fileOpener = _hostView->ShowOpenFileDialog( LoadableFileTypes::TAP );
			if( fileOpener )
			{
				EnsureUIUpdated uiUpdater(this);
				_lynxEmulator->LoadExistingTAPFile( fileOpener.get() ); // throws
			}
		}
	}



	void LynxUserInterfaceModel::OnSaveTAPFileAs()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::TAP );
		if( fileOpener )
		{
			_lynxEmulator->SaveTape( fileOpener.get() ); // throws
		}
	}



	void LynxUserInterfaceModel::OnRewindAudioTape()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		_lynxEmulator->RewindTape();
		_hostView->TellUser( "Tape rewound.", "Note" );
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



	void LynxUserInterfaceModel::OnResetEmulation()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		if( UserAllowsReset() )
		{
			SetMachineTypeAndReset( _machineType );  // ie: don't change machine type
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



	void LynxUserInterfaceModel::OnSaveStateSnapshot()
	{
		// The View calls this because an option has (somehow!) been selected in the UI (menu/button/icon/whatever).

		auto fileOpener = _hostView->ShowSaveFileDialog( SaveableFileTypes::Snapshot );
		if( fileOpener )
		{
			EnsureUIUpdated uiUpdater(this); // TODO: Is this really needed or was it a copy-paste error?
			_lynxEmulator->SaveState( fileOpener.get() );  // throws
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



	bool LynxUserInterfaceModel::UserAllowsReset()
	{
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









	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//        MODEL (HOST) SERVICES TO GUEST (EMULATOR)  (IHostServicesForLynxEmulator)
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void  LynxUserInterfaceModel::InvalidateAreaOfGuestScreen( int left, int top, int right, int bottom )
	{
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
		scaledRect.right  = right  + projectionRectangle.left;
		scaledRect.top    = top    + projectionRectangle.top;
		scaledRect.bottom = bottom + projectionRectangle.top;

		_hostView->InvalidateAreaOfHostScreen( scaledRect );
	}



	void  LynxUserInterfaceModel::NotifyOutputTapeAvailbilityChanged()
	{ 
		UpdateUserInterfaceElementsOnView();
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     EMULATION SPEEDS SUPPORTED BY THE USER INTERFACE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	enum { NumEmulationSpeeds = 5 };

	struct EmulationSpeed
	{
		TickableInterfaceElements::Enum  interfaceItem;
		uint32_t  z80CyclesPerTimeslice;
	};

	EmulationSpeed  CycleCountToMenuOptionLookupTable[NumEmulationSpeeds] =
	{
		TickableInterfaceElements::Speed50,     LynxZ80Cycles::At50,     //  2MHz
		TickableInterfaceElements::Speed100,    LynxZ80Cycles::At100,    //  4Mhz
		TickableInterfaceElements::Speed200,    LynxZ80Cycles::At200,    //  8Mhz
		TickableInterfaceElements::Speed400,    LynxZ80Cycles::At400,    // 16Mhz
		TickableInterfaceElements::Speed800,    LynxZ80Cycles::At800,    // 32Mhz
	};






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     INTERNAL FUNCTIONS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void LynxUserInterfaceModel::UpdateUserInterfaceElementsOnView()
	{
		//
		// Decide what we need the tick state to be for the menu items:
		//

		bool tick48K        = (_machineType == LynxMachineType::LYNX_48K);
		bool tick96K        = (_machineType == LynxMachineType::LYNX_96K);
		bool tickSquare     = (_renderStyle == RenderStyle::SquarePixels);
		bool tickFitWindow  = (_renderStyle == RenderStyle::FitToWindow);
		bool tickTapeSounds = _lynxEmulator->GetTapeSounds();
		bool tickRemExtensions = _lynxEmulator->GetLynxRemCommandExtensionsEnabled();
		bool tickSound      = _soundEnable;

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
		_hostView->SetTickBoxState( TickableInterfaceElements::ListenToTapeSounds, tickTapeSounds );
		_hostView->SetTickBoxState( TickableInterfaceElements::FitToWindow, tickFitWindow );
		_hostView->SetTickBoxState( TickableInterfaceElements::UseSquarePixels, tickSquare );
		_hostView->SetTickBoxState( TickableInterfaceElements::LynxBasicRemCommandExtensions, tickRemExtensions );
		_hostView->SetTickBoxState( TickableInterfaceElements::SoundEnableDisable, tickSound );
	
		_hostView->SetEnabledState( ButtonInterfaceElements::RewindTape, greyRewind );
		_hostView->SetEnabledState( ButtonInterfaceElements::FinishRecording, greyFinishRec );
		_hostView->SetEnabledState( ButtonInterfaceElements::SaveTape, greySaveTapAs );
		_hostView->SetEnabledState( ButtonInterfaceElements::FinishLynxTextRecording, greyFinishTextRecording );

		//
		// Set the speed tickbox by looking up the number of cycles in the lookup table:
		// - If the user tampers with the timeslice length in the serialisation, then no checks will show,
		//   but we'll honour the tampered speed setting!
		//

		auto numCycles = _lynxEmulator->GetCyclesPerTimeslice();

		for( int i=0; i < NumEmulationSpeeds; i++ )
		{
			auto menuOptionId  = CycleCountToMenuOptionLookupTable[i].interfaceItem;
			auto thisCycles    = CycleCountToMenuOptionLookupTable[i].z80CyclesPerTimeslice;
			bool foundThisTime = (numCycles == thisCycles);
			_hostView->SetTickBoxState( menuOptionId, foundThisTime );
		}
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
		_machineType = machineType;
		_lynxEmulator->ResetGuest( _machineType );
		UpdateUserInterfaceElementsOnView();
	}

} // end namespace Jynx
