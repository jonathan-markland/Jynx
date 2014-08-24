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

#include "TextPlayer.h"


namespace Jynx
{
	TextPlayer::TextPlayer()
	{
		Reset();
	}



	void TextPlayer::Reset()
	{
		_copyOfSuppliedText.clear();
		_readPosition = "";
	}



	void TextPlayer::SetText( const char *textBytes )
	{
		_copyOfSuppliedText = textBytes;
		if( _copyOfSuppliedText.empty() )
		{
			Reset();
		}
		else
		{
			_readPosition = &_copyOfSuppliedText.front();
		}
	}



	char  TextPlayer::ReadChar()
	{
		// We must accept what the TextRecorder generated, but also:
		// - MAC or UNIX or DOS line ends -> map onto Lynx's end of line character, 13.
		// - Encoding: ASCII files, ignoring non-line-end values in range 1-31.
		// - Encoding: UTF-8 allowing code points 0..255, ignoring non-line-end values in range 1-31.
		// - Do not assume to skip unrecognised content, enter Reset() state, and return 0.

		for(;;)
		{
			auto byte = (uint8_t) (*_readPosition);   // Reminder - char is signed.

			// Look for likely host end-of-line character(s).  Pass as "13" to the Lynx:
			//    - 10 only
			//    - 13 only
			//    - 13 10

			if( byte == 10 ) // UNIX
			{
				++_readPosition; // consume the character
				return 13; // Lynx's preferred char
			}

			if( byte == 13 ) // MAC / WINDOWS
			{
				++_readPosition; // consume the character
				if( _readPosition[0] == 10 ) ++_readPosition; // It a WINDOWS line end
				return 13; // Lynx's preferred char
			}

			// Now, if it's not NUL then do UTF-8 decoding:

			if( byte != '\0' )
			{
				++_readPosition; // always consume the character

				if( byte >= 32 && byte < 0x80 )
				{
					return (char) byte;
				}
				else if( byte == 0xC2 || byte == 0xC3 )  // Subset UTF-8 support (we only need code points 0..255)
				{
					auto tailByte = (uint8_t) (*_readPosition);
					if( (tailByte & 0xC0) == 0x80 )
					{
						++_readPosition; // consume valid tail byte
						if( byte == 0xC3 ) tailByte += 0x40;
						return (char) tailByte;
					}
				}

				// Failed to read the subset of UTF-8 that we support.
				// Drop through.
			}

			// If we get to NUL, then let's free the memory.

			Reset();
			return '\0';
		}
	}

} // end namespace Jynx
