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

#include <stdint.h>
#include "IFileOpener.h"
#include "ISerialiser.h"
#include "LynxEmulatorEnums.h"


namespace Jynx
{
	namespace RenderStyle
	{
		enum Enum
		{
			FitToWindow, SquarePixels, FillWindow
		};
	}



	class UserSettings
	{
	public:

		// Load and save user settings.
		// - When loading it is desirable to have a separate store, in case of failures.
		//   We apply the store to the main system only when successfully loaded.
		// - Save may as well be in the same place for symmetry.

		explicit UserSettings( IFileOpener * );

		UserSettings(
			LynxMachineType::Enum  machineType,
			RenderStyle::Enum      renderStyle,
			bool                   soundEnable,
			bool                   fullScreenEnable,
			uint32_t               cyclesPerTimeslice,
			bool                   tapeSounds,
			bool                   remExtensions,
			bool                   maxSpeedCassette,
			bool                   maxSpeedConsole,
			bool                   maxSpeedAlways );

		void SaveToFile( IFileOpener *, const std::string endOfLineSequence );

		LynxMachineType::Enum  GetMachineType()        const { return _machineType; }
		RenderStyle::Enum      GetRenderStyle()        const { return _renderStyle; }
		uint32_t               GetCyclesPerTimeslice() const { return _cyclesPerTimeslice; }
		bool                   GetSoundEnable()        const { return _soundEnable; }
		bool                   GetFullScreenEnable()   const { return _fullScreenEnable; }
		bool                   GetTapeSounds()         const { return _tapeSounds; }
		bool                   GetRemExtensions()      const { return _remExtensions; }
		bool                   GetMaxSpeedCassette()   const { return _maxSpeedCassette; }
		bool                   GetMaxSpeedConsole()    const { return _maxSpeedConsole; }
		bool                   GetMaxSpeedAlways()     const { return _maxSpeedAlways; }

	private:

		LynxMachineType::Enum  _machineType;
		RenderStyle::Enum      _renderStyle;
		bool                   _soundEnable;
		bool                   _fullScreenEnable;
		uint32_t               _cyclesPerTimeslice;
		bool                   _tapeSounds;
		bool                   _remExtensions;
		bool                   _maxSpeedCassette;
		bool                   _maxSpeedConsole;
		bool                   _maxSpeedAlways;

		void SerialiseFields( ISerialiser *serialiser );

	};

}

