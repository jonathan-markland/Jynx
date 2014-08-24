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
#include <stdint.h>


namespace Jynx
{
	class TextPlayer
	{
	public:

		// TextPlayer is for "re-playing" streams of characters into the Lynx, so 
		// its "read key" ROM routine sees these as real key presses.
		// - The Text is UTF-8 encoded (NO BOM).

		TextPlayer();

		void Reset();
		void SetText( const char *textBytes );
		char ReadChar();
		inline bool HasText() const                  { return *_readPosition != '\0'; };

	private:

		std::string  _copyOfSuppliedText;
		const char  *_readPosition;

	};

} // end namespace Jynx
