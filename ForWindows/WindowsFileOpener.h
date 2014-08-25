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

#include <string>
#include "../Portable/IFileOpener.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FILE PATH FORMAT ABSTRACTION ON WINDOWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class WindowsFileOpener: public Jynx::IFileOpener
{
public:

	// Encapsulate the nature and representation of a file path.
	// Assist opening or creating a file with the hidden path.

	// See IFileOpener

	WindowsFileOpener( const wchar_t *filePath );
	WindowsFileOpener( const std::wstring &filePath );
	virtual void OpenOutputStream( std::ofstream &stream, std::ios_base::openmode openModeRequired ) override;
	virtual void OpenInputStream(  std::ifstream &stream, std::ios_base::openmode openModeRequired ) override;

private:

	std::wstring  _filePath;

};
