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

#include "IViewServicesForLynxUserInterfaceModel.h"
#include "IHostServicesForLynxEmulator.h"
#include "LynxEmulatorGuest.h"
#include "UserSettingsSerialiser.h"

#include "Lexer.h" // for exception type
#include "FileLoader.h" // for exception type

namespace Jynx
{
	class LynxUserInterfaceModel:
		public IHostServicesForLynxEmulator
	{
	public:

		// An OPTIONAL user interface model.
		// - If used, this creates the emulator internally.
		// - If you don't want to use this, make your own object that exposes IHostServicesForLynxEmulator
		//   and create the LynxEmulatorGuest yourself.
		// - Multithreading note:  Consider the model to be called on the MAIN thread.

		LynxUserInterfaceModel(
			IViewServicesForLynxUserInterfaceModel *hostView, const char *platformEndOfLineSequenceUTF8, bool gamesMode );

		// IHostServicesForLynxEmulator:
		// - THREADING NOTE:  The emulator object calls back into the Model on
		//   the EMULATOR thread, using this restricted interface.
		virtual  void  InvalidateAreaOfGuestScreen_OnMainThread( int32_t left, int32_t top, int32_t right, int32_t bottom ) override;
		virtual  void  OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) override;
		virtual  void  NotifyOutputTapeAvailbilityChanged_OnAnyThread() override;
		virtual  void  PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData ) override;
		virtual  void  TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues ) override;

		// Event handler implementations.
		// All of these have exception guards for stray exceptions (program termination)
		// This protects C language event raisers from being wound-back.
		bool OnMenuCommand( uint32_t menuCommandID );
		void OnInitDialog();
		void OnTimer();
		void OnAllKeysUp();
		void OnKeyDown( int32_t keyCode );
		void OnKeyUp( int32_t keyCode );
		void OnPaint();

		// These throw:
		void ForceLoadSpecificSnapshot( IFileOpener *fileOpener ); // enable view to load the command-line default snapshot file.
		void ForceLoadSpecificTape( IFileOpener *fileOpener ); // enable view to load the command-line default TAP file.

    private:

        // (Used to be public before we had the menu dispatcher).

        void OnShowTheAboutBox();
		void OnExit();
		void OnLoadStateSnapshot();
		void OnSaveStateSnapshot();
		void OnRunTAPFile();
		void OnOpenTAPFile();
		void OnNewAudioTape();
		void OnSaveTAPFileAs();
		void OnRewindAudioTape();
		void OnEmulation48K();
		void OnEmulation96K();
		void OnEmulation96KScorpion();
		void OnSetCycles( LynxZ80Cycles::Enum numCycles );
		void OnListenToTapeSounds();
		void OnRecordToFile();
		void OnFinishRecording();
		void OnResetEmulation();
		void OnFitToWindow();
		void OnSquarePixels();
		void OnFillWindow();
		void OnRecordLynxTextToFile();
		void OnFinishRecordingLynxText();
		void OnLynxBasicRemCommandExtensions();
		void OnEnableDisableSound();
		bool IsSoundEnabled();
		void OnEnableDisableFullScreen();
		bool IsFullScreenEnabled();
		void OnTypeInTextFromFile();
		void OnPause();
		void OnPauseAfterTapLoad();
		void OnTypeTapeDirectoryIntoLynx();
		void OnSpeedMaxCassette();
		void OnSpeedMaxConsoleCommands();
		void OnSpeedMaxPermanently();
		void OnChangeColourSet( LynxColourSet::Enum colourSet );

	private:

		friend class EnsureUIUpdated;

		IViewServicesForLynxUserInterfaceModel  *_hostView;
		std::unique_ptr<LynxEmulatorGuest>       _lynxEmulator;
		RenderStyle::Enum      _renderStyle;
		bool                   _showFullScreen;
		bool                   _gamesMode;
		volatile bool          _emulatorWantsUIStatusUpdate;

		// Host platform's preferred UTF8 end of line sequence:
		const std::string  _platformEndOfLineSequenceUTF8;

    private:

		LynxRectangle  GetProjectionRectangle() const;

		void UpdateUserInterfaceElementsOnView();
		bool CanRiskLosingModifiedTape() const;
		void SetRenderStyle( RenderStyle::Enum newStyle );
		void SetMachineTypeAndReset( LynxMachineType::Enum machineType );
		bool UserAllowsReset();

		void SaveUserSettings();

		void LoadUserSettings();

        void ReportWithPrefix( const char *messageToPrefix, const std::exception &e );

        template<typename FILE_OPERATION>
        void DoWithFileHandlingErrorReportsToUser( const char *messageToPrefix, FILE_OPERATION fileOperation )
        {
            try
            {
                fileOperation();
            }
            catch( Jynx::TapFileLexerException &e )
            {
                ReportWithPrefix( "Failed while reading a TAP file.", e );
            }
            catch( Jynx::LexerException &e )
            {
                ReportWithPrefix( "Failed with a lexical error while reading a text file.", e );
            }
            catch( VectorLoadSaveException &e )
            {
                ReportWithPrefix( "Failed while attempting to load or save a file image.", e );
            }
            catch( std::ios_base::failure &e )
            {
                ReportWithPrefix( messageToPrefix, e );
            }
        }




	};

} // end namespace Jynx
