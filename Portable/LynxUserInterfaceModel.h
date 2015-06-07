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
			IViewServicesForLynxUserInterfaceModel *hostView, uint16_t *soundBuffer, size_t numSamples, const char *platformEndOfLineSequenceUTF8, bool gamesMode );

		// ILynxUserInterfaceModel:
		void OnInitDialog();
		void OnTimer();
		void NotifyAllKeysUp();
		void NotifyKeyDown( int32_t keyCode );
		void NotifyKeyUp( int32_t keyCode );
		void OnPaint();
		void OnLoadStateSnapshot();
		void OnSaveStateSnapshot();
		void OnRunTAPFile();
		void OnOpenTAPFile();
		void OnNewAudioTape();
		void OnSaveTAPFileAs();
		void OnRewindAudioTape();
		void OnExit();
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
		void ForceLoadSpecificSnapshot( IFileOpener *fileOpener ); // enable view to load the command-line default snapshot file.
		void ForceLoadSpecificTape( IFileOpener *fileOpener ); // enable view to load the command-line default TAP file.
		void OnTypeTapeDirectoryIntoLynx();
		void OnSpeedMaxCassette();
		void OnSpeedMaxConsoleCommands();
		void OnSpeedMaxPermanently();
		void OnChangeColourSet( LynxColourSet::Enum colourSet );

		// IHostServicesForLynxEmulator:
		// - THREADING NOTE:  The emulator object calls back into the Model on
		//   the EMULATOR thread, using this restricted interface.
		virtual  void  InvalidateAreaOfGuestScreen_OnMainThread( int32_t left, int32_t top, int32_t right, int32_t bottom ) override;
		virtual  void  OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) override;
		virtual  void  NotifyOutputTapeAvailbilityChanged_OnAnyThread() override;
		virtual  void  PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData ) override;
		virtual  void  WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread() override;
		virtual  void  TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues ) override;

	private:

		friend class EnsureUIUpdated;

		IViewServicesForLynxUserInterfaceModel  *_hostView;
		std::unique_ptr<LynxEmulatorGuest>       _lynxEmulator;
		RenderStyle::Enum      _renderStyle;
		bool                   _soundEnable;
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
            // throws std::ofstream::failure

		void LoadUserSettings();
            // throws const std::invalid_argument on parse error
            // throws const std::ifstream::failure

        void ReportWithPrefix( const char *messageToPrefix, const std::exception &e );

        template<typename FILE_OPERATION>
        void DoWithFileHandlingErrorReportsToUser( const char *messageToPrefix, FILE_OPERATION fileOperation )
        {
            try
            {
                fileOperation();
            }
            catch( std::invalid_argument &e )
            {
                ReportWithPrefix( messageToPrefix, e );
            }
            catch( std::ios_base::failure &e )
            {
                ReportWithPrefix( messageToPrefix, e );
            }
        }




	};

} // end namespace Jynx
