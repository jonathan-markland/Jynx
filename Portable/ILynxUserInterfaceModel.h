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

#include "ILynxEmulator.h" // for enum

namespace Jynx
{
	class ILynxUserInterfaceModel
	{
	public:

		virtual void OnInitDialog() = 0;
		virtual void OnTimer() = 0;
		virtual void NotifyAllKeysUp() = 0;
		virtual void NotifyKeyDown( int32_t keyCode ) = 0;
		virtual void NotifyKeyUp( int32_t keyCode ) = 0;
		virtual void OnPaint() = 0;
		virtual void OnLoadStateSnapshot() = 0;
		virtual void OnSaveStateSnapshot() = 0;
		virtual void OnOpenTAPFile() = 0;
		virtual void OnNewAudioTape() = 0;
		virtual void OnSaveTAPFileAs() = 0;
		virtual void OnRewindAudioTape() = 0;
		virtual void OnExit() = 0;
		virtual void OnEmulation48K() = 0;
		virtual void OnEmulation96K() = 0;
		virtual void OnSetCycles( LynxZ80Cycles::Enum numCycles ) = 0;
		virtual void OnListenToTapeSounds() = 0;
		virtual void OnRecordToFile() = 0;
		virtual void OnFinishRecording() = 0;
		virtual void OnResetEmulation() = 0;
		virtual void OnFitToWindow() = 0;
		virtual void OnSquarePixels() = 0;
		virtual void OnRecordLynxTextToFile() = 0;
		virtual void OnFinishRecordingLynxText() = 0;
		virtual void OnLynxBasicRemCommandExtensions() = 0;
		virtual void OnEnableDisableSound() = 0;
		virtual bool IsSoundEnabled() = 0;
		virtual void OnTypeInTextFromFile() = 0;

	};

} // end namespace Jynx
