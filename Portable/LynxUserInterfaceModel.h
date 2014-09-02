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

#include "ILynxUserInterfaceModel.h"
#include "IHostServicesForLynxUserInterfaceModel.h"
#include "IHostServicesForLynxEmulator.h"
#include "LynxEmulatorGuest.h"
#include "UserSettingsSerialiser.h"


namespace Jynx
{
	class LynxUserInterfaceModel: 
		public ILynxUserInterfaceModel, 
		public IHostServicesForLynxEmulator
	{
	public:

		// An OPTIONAL user interface model.
		// - If used, this creates the emulator internally.
		// - If you don't want to use this, make your own object that exposes IHostServicesForLynxEmulator
		//   and create the LynxEmulatorGuest yourself.
		// - Multithreading note:  Consider the model to be called on the MAIN thread.

		LynxUserInterfaceModel( IHostServicesForLynxUserInterfaceModel *hostView, uint16_t *soundBuffer, size_t numSamples, const char *platformEndOfLineSequenceUTF8 );

		// ILynxUserInterfaceModel:
		virtual void OnInitDialog() override;
		virtual void OnTimer() override;
		virtual void NotifyAllKeysUp() override;
		virtual void NotifyKeyDown( int32_t keyCode ) override;
		virtual void NotifyKeyUp( int32_t keyCode ) override;
		virtual void OnPaint() override;
		virtual void OnLoadStateSnapshot() override;
		virtual void OnSaveStateSnapshot() override;
		virtual void OnOpenTAPFile() override;
		virtual void OnNewAudioTape() override;
		virtual void OnSaveTAPFileAs() override;
		virtual void OnRewindAudioTape() override;
		virtual void OnExit() override;
		virtual void OnEmulation48K() override;
		virtual void OnEmulation96K() override;
		virtual void OnSetCycles( LynxZ80Cycles::Enum numCycles ) override;
		virtual void OnListenToTapeSounds() override;
		virtual void OnRecordToFile() override;
		virtual void OnFinishRecording() override;
		virtual void OnResetEmulation() override;
		virtual void OnFitToWindow() override;
		virtual void OnSquarePixels() override;
		virtual void OnRecordLynxTextToFile() override;
		virtual void OnFinishRecordingLynxText() override;
		virtual void OnLynxBasicRemCommandExtensions() override;
		virtual void OnEnableDisableSound() override;
		virtual bool IsSoundEnabled() override;
		virtual void OnTypeInTextFromFile() override;

		// IHostServicesForLynxEmulator:
		virtual  void  InvalidateAreaOfGuestScreen( int32_t left, int32_t top, int32_t right, int32_t bottom ) override;
		virtual  void  OpenChipFileStream( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) override;
		virtual  void  NotifyOutputTapeAvailbilityChanged() override;  // Hint: call host CanSaveTAPFile() to discover state at any time.
		virtual  void  PaintPixelsOnHostBitmapForLynxScreenByte( uint32_t addressOffset, uint32_t lynxRedByte, uint32_t lynxGreenByte, uint32_t lynxBlueByte ) override;
		virtual  IHostThread *CreateThread( IHostServicesForLynxEmulatorThreadFunction threadFunction, void *userObject ) override;
		virtual  void ThreadSleep( uint32_t milliseconds ) override;
		virtual  void ThreadWaitForSound() override;

	private:

		friend class EnsureUIUpdated;

		IHostServicesForLynxUserInterfaceModel  *_hostView;
		std::unique_ptr<LynxEmulatorGuest>       _lynxEmulator;
		LynxMachineType::Enum  _machineType;
		RenderStyle::Enum      _renderStyle;
		bool                   _soundEnable;
		volatile bool          _emulatorWantsUIStatusUpdate;

		// Host platform's preferred UTF8 end of line sequence:
		const std::string  _platformEndOfLineSequenceUTF8;

		LynxRectangle  GetProjectionRectangle() const;

		void UpdateUserInterfaceElementsOnView();
		bool CanRiskLosingModifiedTape() const;
		void SetRenderStyle( RenderStyle::Enum newStyle );
		void SetMachineTypeAndReset( LynxMachineType::Enum machineType );
		bool UserAllowsReset();

		void SaveUserSettings();
		void LoadUserSettings();

	};

} // end namespace Jynx
