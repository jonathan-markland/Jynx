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

namespace Jynx
{
	namespace TickableInterfaceElements
	{
		enum Enum
		{
			Lynx48K, Lynx96K, Lynx96KScorpion, ListenToTapeSounds, FitToWindow, UseSquarePixels, FillWindow,
			Speed50, Speed100, Speed200, Speed400, Speed800, LynxBasicRemCommandExtensions,
			SoundEnableDisable, ShowFullScreen, Paused, PausedAfterTapLoad,
			MaxSpeedCassette, MaxSpeedConsole, MaxSpeedAlways,
			ColourSetNormalRGB,
			ColourSetGreenOnly,
			ColourSetLevel9,
			ColourSetBlackAndWhiteTV,
			ColourSetGreenScreenMonitor,

			Count // always last
		};
	};



	namespace ButtonInterfaceElements
	{
		// This enum allows the Model class to refer to conceptual objects on the View.
		// This is because the Model can help the View enable/disable UI elements.
		// "Button" is kind of a generic term here, it could mean "menu item".
		enum Enum
		{
			RewindTape, FinishRecording, SaveTape, FinishLynxTextRecording,

			Count // always last
		};
	}



	namespace SaveableFileTypes
	{
		enum Enum
		{
			TAP, Snapshot, Sound, LynxText,
			Count // always last
		};
	}



	namespace LoadableFileTypes
	{
		enum Enum
		{
			TAP, Snapshot, Text,
			Count // always last
		};
	}



}



