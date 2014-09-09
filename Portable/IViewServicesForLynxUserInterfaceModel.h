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

#include <memory>
#include "IFileOpener.h"
#include "LynxRectangle.h"
#include "LynxRoms.h"


namespace Jynx
{
	namespace TickableInterfaceElements
	{
		enum Enum
		{
			Lynx48K, Lynx96K, ListenToTapeSounds, FitToWindow, UseSquarePixels, FillWindow,
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



	class IViewServicesForLynxUserInterfaceModel
	{
	public:

		// The host (view) must provide these IF it is using the (optional) user interface model class.
		// This allows the model to request things of the host.

		// - Threading note:  These are all called on the MAIN thread, unless the function name noted otherwise.

		virtual void CloseDownNow() = 0;
			// The Model tells the View to close down now.
			// It is always appropriate for the View to perform this without question.

		virtual std::shared_ptr<IFileOpener> ShowOpenFileDialog( LoadableFileTypes::Enum ) = 0;  
			// The Model is asking the View to show an "open file" dialog for the given file type.
			// View must return nullptr if cancelled, else return IFileOpener for the file selected.

		virtual std::shared_ptr<IFileOpener> ShowSaveFileDialog( SaveableFileTypes::Enum ) = 0;
			// The Model is asking the View to show a "save file" dialog for the given file type.
			// View must return nullptr if cancelled, else return IFileOpener for the file selected.

		virtual void TellUser( const char *messageText, const char *captionText ) = 0;
			// The Model is asking the View to tell the user something has happened.
			// The View might, for example, show a message box with an OK button.

		virtual bool AskYesNoQuestion( const char *questionText, const char *captionText ) = 0;
			// The Model is asking the View to prompt the user as to the action to take.
			// The View might, for example, show a message box with YES/NO buttons.

		virtual void SetTickBoxState( TickableInterfaceElements::Enum itemToSet, bool tickState ) = 0;
			// The Model assists the View by telling it to change the selection (tick) status
			// of a conceptual UI option.  The View might show/hide a tick symbol next to a menu item
			// or check box control, as a result of this, for example.

		virtual void SetEnabledState( ButtonInterfaceElements::Enum itemToSet, bool enableState ) = 0;
			// The Model assists the View by telling it to change the enable / disable status
			// of a conceptual UI option.  The View might grey out, or re-activate, a menu item
			// or a button/icon, as a result of this, for example.

		virtual LynxRectangle GetClientRectangle() = 0;
			// The Model assists the View by calculating positioning for the Lynx screen within
			// a rectangular area.  The View must return the pixel-area into which the Lynx screen
			// (and associated black border) can be rendered.

		virtual void StretchBlitTheGuestScreen( int left, int top, int width, int height ) = 0;
			// The Model assists the View with calculating position for painting the 
			// Lynx screen bitmap.  The View must render the Lynx bitmap into the area given.
			// This should be done with something like StretchBlt() on Windows.  The View
			// must have created the 256*256 bitmap for the Lynx screen, on its initialisation.

		virtual void FillBlackRectangle( int left, int top, int width, int height ) = 0;
			// The Model assists the View with calculating position for painting black
			// border rectangles around the Lynx screen bitmap, to the edges of the area
			// that the View previously returned in GetClientRectangle().
			// The View must fill the rectangle in black.

		virtual void InvalidateAreaOfHostScreen( const LynxRectangle &area ) = 0;
			// The Model provides a notification to the View that the given pixel area
			// should be marked for re-painting with the View's window manager.
			// This area lies within the rectangle that the View previously returned in GetClientRectangle().

		virtual  void  OpenChipFileStream_OnMainThread( std::ifstream &streamToBeOpened, std::ios_base::openmode openModeRequired, LynxRoms::Enum romRequired ) = 0;
			// (See same function comments in IHostServicesForLynxEmulator).

		virtual  std::shared_ptr<IFileOpener>  GetUserSettingsFilePath() = 0;
			// Obtains a file opener that holds the path to the user settings file (or where it would be if it doesn't exist yet).

		virtual  void TranslateRGBXColourPaletteToHostValues( const uint32_t *eightEntryColourPalette, uint32_t *eightEntryTranslatedValues ) = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// (See IHostServicesForLynxEmulator)

		virtual  void  PaintPixelsOnHostBitmap_OnEmulatorThread( uint32_t addressOffset, const uint32_t *eightPixelsData ) = 0;
			// (WARNING - Called on the EMULATOR thread, NOT the main thread)
			// (See IHostServicesForLynxEmulator)

		virtual  void WriteSoundBufferToSoundCardOrSleep_OnEmulatorThread() = 0;
			// (See IHostServicesForLynxEmulator)

	};


} // end namespace Jynx
