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

namespace Jynx
{
	class TextRecorder
	{
	public:

		// Used to record the key presses (as seen by the Lynx).
		// These are spooled to a file.
		// UTF-8 is used because the Lynx emits character codes 128-255 (with Control-1),
		// and Lynx program listings can contain these characters.

		TextRecorder();
		~TextRecorder();

		void StartNewFile( IFileOpener *, const std::string &endOfLineSequence );
		bool IsOpen() const;
		void AddLynxCharacter( uint8_t vduCode );
		void Close();

	private:
		
		void WriteCharUtf8( uint8_t codePoint );
		void WriteCharUnencoded( uint8_t byteValue );

		std::ofstream  _outStream;
		std::string    _endOfLineSequence;

	};

} // end namespace Jynx
