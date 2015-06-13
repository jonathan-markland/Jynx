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




#include "UIStrings.h"
#include "MenuItemIDs.h"


// TODO: This perhaps need revisiting, but at least it's all together.


const wchar_t *OpenFileDialogTitles[Jynx::LoadableFileTypes::Count] =
{
	L"Open Lynx TAP file",
	L"Open emulator state snapshot",
	L"Open Text file",
};

const char *OpenFileDialogTitlesNarrow[Jynx::LoadableFileTypes::Count] =
{
	"Open Lynx TAP file",
	"Open emulator state snapshot",
	"Open Text file",
};




const wchar_t *OpenFileDialogSpecs[Jynx::LoadableFileTypes::Count]  =
{
	L"TAP files (*.tap)|*.tap",
	L"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	L"Text files (*.txt)|*.txt",
};

const char *OpenFileDialogSpecsNarrow[Jynx::LoadableFileTypes::Count]  =
{
	"TAP files (*.tap)|*.tap",
	"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	"Text files (*.txt)|*.txt",
};




const wchar_t *SaveFileDialogTitles[Jynx::SaveableFileTypes::Count] =
{
	L"Save Lynx Audio Tape file",
	L"Save emulator state snapshot",
	L"Record sound to file",
	L"Record lynx text to file"
};

const char *SaveFileDialogTitlesNarrow[Jynx::SaveableFileTypes::Count] =
{
	"Save Lynx Audio Tape file",
	"Save emulator state snapshot",
	"Record sound to file",
	"Record lynx text to file"
};



const wchar_t *SaveFileDialogSpecs[Jynx::SaveableFileTypes::Count] =
{
	L"Lynx TAP files (*.tap)|*.tap",
	L"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	L"Sound files (*.wav)|*.wav",
	L"Text files (*.txt)|*.txt"
};

const char *SaveFileDialogSpecsNarrow[Jynx::SaveableFileTypes::Count] =
{
	"Lynx TAP files (*.tap)|*.tap",
	"Snapshots (*.lynxsnapshot)|*.lynxsnapshot",
	"Sound files (*.wav)|*.wav",
	"Text files (*.txt)|*.txt"
};




const wchar_t *SaveFileDialogExtns[Jynx::SaveableFileTypes::Count] =
{
	L"tap",
	L"lynxsnapshot",
	L"wav",
	L"txt"
};

const char *SaveFileDialogExtnsNarrow[Jynx::SaveableFileTypes::Count] =
{
	"tap",
	"lynxsnapshot",
	"wav",
	"txt"
};



const wchar_t *g_RomFileNames[Jynx::LynxRoms::Count] =
{
	L"lynx48-1.rom",
	L"lynx48-2.rom",
	L"lynx96-1.rom",
	L"lynx96-2.rom",
	L"lynx96-3.rom",
	L"lynx96-3-scorpion.rom",
};

const char *g_RomFileNamesNarrow[Jynx::LynxRoms::Count] =
{
	"lynx48-1.rom",
	"lynx48-2.rom",
	"lynx96-1.rom",
	"lynx96-2.rom",
	"lynx96-3.rom",
	"lynx96-3-scorpion.rom",
};




const uint32_t MainFormTickableItems[Jynx::TickableInterfaceElements::Count] =
{
	ID_EMULATION_LYNX48K,
	ID_EMULATION_LYNX96K,
	ID_EMULATION_LYNX96KSCORPION,
	ID_SOUND_LISTENTOTAPESOUNDS,
	ID_DISPLAY_FITTOWINDOW,
	ID_DISPLAY_SQUAREPIXELS,
	ID_DISPLAY_FILLWINDOW,
	ID_SPEED_SPEED50,
	ID_SPEED_SPEED100,
	ID_SPEED_SPEED200,
	ID_SPEED_SPEED400,
	ID_SPEED_SPEED800,
	ID_TEXT_LYNXBASICREMCOMMANDEXTENSIONS,
	ID_SOUND_ENABLE,
	ID_DISPLAY_FULLSCREENENABLE,
	ID_EMULATION_PAUSE,
	ID_EMULATION_PAUSEAFTERTAPLOAD,
	ID_SPEED_MAXSPEEDCASSETTE,
	ID_SPEED_MAXSPEEDCONSOLE,
	ID_SPEED_MAXSPEEDALWAYS,
	ID_DISPLAY_COLOURSET_NORMALRGB,
	ID_DISPLAY_COLOURSET_GREENONLY,
	ID_DISPLAY_COLOURSET_LEVEL9,
	ID_DISPLAY_COLOURSET_BLACKANDWHITETV,
	ID_DISPLAY_COLOURSET_GREENSCREENMONITOR,
};

const uint32_t MainFormGreyableItems[Jynx::ButtonInterfaceElements::Count] =
{
	ID_FILE_REWINDTAPE,
	ID_SOUND_FINISHRECORDING,
	ID_FILE_SAVETAPE,
	ID_TEXT_STOPRECORDINGLYNXTEXT
};

