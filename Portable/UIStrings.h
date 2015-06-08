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

#include <stdint.h>
#include "Enumerations.h"
#include "LynxRoms.h"

// WIDE STRING versions

extern const wchar_t *OpenFileDialogTitles[Jynx::LoadableFileTypes::Count];
extern const wchar_t *OpenFileDialogSpecs[Jynx::LoadableFileTypes::Count];
extern const wchar_t *SaveFileDialogTitles[Jynx::SaveableFileTypes::Count];
extern const wchar_t *SaveFileDialogSpecs[Jynx::SaveableFileTypes::Count];
extern const wchar_t *SaveFileDialogExtns[Jynx::SaveableFileTypes::Count];
extern const wchar_t *g_RomFileNames[Jynx::LynxRoms::Count];

// NARROW STRING versions

extern const char *OpenFileDialogTitlesNarrow[Jynx::LoadableFileTypes::Count];
extern const char *OpenFileDialogSpecsNarrow[Jynx::LoadableFileTypes::Count];
extern const char *SaveFileDialogTitlesNarrow[Jynx::SaveableFileTypes::Count];
extern const char *SaveFileDialogSpecsNarrow[Jynx::SaveableFileTypes::Count];
extern const char *SaveFileDialogExtnsNarrow[Jynx::SaveableFileTypes::Count];
extern const char *g_RomFileNamesNarrow[Jynx::LynxRoms::Count];

// MENU ITEM IDs

extern const uint32_t MainFormTickableItems[Jynx::TickableInterfaceElements::Count];
extern const uint32_t MainFormGreyableItems[Jynx::ButtonInterfaceElements::Count];
