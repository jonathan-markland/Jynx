
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



