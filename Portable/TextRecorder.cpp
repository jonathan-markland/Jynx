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

#include "TextRecorder.h"
#include "ZeroInitialiseMemory.h"


namespace Jynx
{
	TextRecorder::TextRecorder()
	{
	}



	TextRecorder::~TextRecorder()
	{
		Close();
	}



	static uint8_t g_Utf8ByteOrderMark[3] = { 0xEF, 0xBB, 0xBF };



	void TextRecorder::StartNewFile( IFileOpener *fileOpener, const std::string &endOfLineSequence )
	{
		_endOfLineSequence = endOfLineSequence;

		fileOpener->OpenOutputStream( _outStream, std::ios::binary | std::ios::out );

		// Write the BOM for UTF-8:
		_outStream.write( (const char *) g_Utf8ByteOrderMark, 3 );
	}



	bool TextRecorder::IsOpen() const
	{
		return _outStream.is_open();
	}



	void TextRecorder::AddLynxCharacter( uint8_t vduCode )
	{
		// Let's rationalise the two codes the Lynx could use for new line:
		if( vduCode == 13 || vduCode == 31 )
		{
			_outStream.write( &_endOfLineSequence.front(), _endOfLineSequence.size() );
		}
		else if( vduCode >= 32 && vduCode <= 255 )
		{
			WriteCharUtf8( vduCode );
		}
	}



	void TextRecorder::WriteCharUtf8( uint8_t codePoint )
	{
		// Subset UTF-8 support (we only need code points 13, 32..255)

		if( codePoint < 128 )
		{
			WriteCharUnencoded( codePoint );
		}
		else if( codePoint < 0xC0 )
		{
			WriteCharUnencoded( 0xC2 );
			WriteCharUnencoded( codePoint );
		}
		else
		{
			WriteCharUnencoded( 0xC3 );
			WriteCharUnencoded( codePoint - 0x40 );
		}
	}



	void TextRecorder::WriteCharUnencoded( uint8_t byteValue )
	{
		_outStream.write( (const char *) &byteValue, 1 );
	}



	void TextRecorder::Close()
	{
		if( _outStream.is_open() )
		{
			_outStream.close();
		}
	}

} // end namespace Jynx
