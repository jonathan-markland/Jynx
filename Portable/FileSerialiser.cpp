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
#include <string>
#include "FileSerialiser.h"

namespace Jynx
{
	const char *g_HexChars = "0123456789ABCDEF";



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    OutputFileSerialiser
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	OutputFileSerialiser::OutputFileSerialiser( IFileOpener *fileOpener, const std::string endOfLineSequence )
	{
		_endOfLineSequence = endOfLineSequence;
		fileOpener->OpenOutputStream( _outStream, std::ios::binary | std::ios::out );
	}



	void OutputFileSerialiser::OpenTag( const char *tagName )
	{
		_outStream << ">" << tagName; EndLine();
	}



	void OutputFileSerialiser::CloseTag( const char *tagName )
	{
		_outStream << "<" << tagName; EndLine();
	}



	void OutputFileSerialiser::Field( const char *tagName, unsigned char &field )
	{
		_outStream << tagName << " " << std::to_string(field); EndLine();
	}



	void OutputFileSerialiser::Field( const char *tagName, unsigned short &field )
	{
		_outStream << tagName << " " << std::to_string(field); EndLine();
	}



	void OutputFileSerialiser::Field( const char *tagName, int &field )
	{
		_outStream << tagName << " " << std::to_string((unsigned int) field); EndLine();
	}



	void OutputFileSerialiser::Binary( void *baseAddress, uintptr_t blockSize )
	{
		auto source = (unsigned char *) baseAddress;;
		auto end = source + blockSize;
		size_t count = 0;
		while( source < end )
		{
			if( count == 32 )
			{
				count = 0;
				_outStream; EndLine();
			}
			auto byteValue = *source;
			_outStream << g_HexChars[(byteValue >> 4) & 15] << g_HexChars[byteValue & 15];
			++count;
			++source;
		}
		_outStream << "."; EndLine();
	}



	void OutputFileSerialiser::EndLine()
	{
		_outStream.write( &_endOfLineSequence.front(), _endOfLineSequence.size() );
	}


	void OutputFileSerialiser::Close()
	{
		_outStream.close();
	}











	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    InputFileSerialiser
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	InputFileSerialiser::InputFileSerialiser( IFileOpener *fileOpener )
	{
		fileOpener->OpenInputStream( _inStream, std::ios::in | std::ios::binary | std::ios::ate );

		uint64_t fileSize = _inStream.tellg();

		// Reserve space for loaded file, and bi-directionally NUL terminate the file once loaded:
		_fileImage = std::vector<char>( (size_t) (fileSize + 2), '\0' );
		auto positionOfFirstCharacter = &(*_fileImage.begin()) + 1;  // Load file image 1 char into the block.

		_lexer.SetPos( positionOfFirstCharacter );

		// Load whole file at an offset of 1 char into the buffer we reserved:
		_inStream.seekg (0, std::ios::beg);
		_inStream.read( (char *) positionOfFirstCharacter, fileSize );
		_inStream.close();
	}



	void InputFileSerialiser::OpenTag( const char *tagName )
	{
		_lexer.ExpectChar( '>' );
		_lexer.ExpectMatch( tagName );
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::CloseTag( const char *tagName )
	{
		_lexer.ExpectChar( '<' );
		_lexer.ExpectMatch( tagName );
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::Field( const char *tagName, unsigned char &field )
	{
		_lexer.ExpectMatch( tagName );
		_lexer.ExpectChar( ' ' );
		field = _lexer.ExpectInteger();
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::Field( const char *tagName, unsigned short &field )
	{
		_lexer.ExpectMatch( tagName );
		_lexer.ExpectChar( ' ' );
		field = _lexer.ExpectInteger();
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::Field( const char *tagName, int &field )
	{
		_lexer.ExpectMatch( tagName );
		_lexer.ExpectChar( ' ' );
		field = _lexer.ExpectInteger();
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::Binary( void *baseAddress, uintptr_t blockSize )
	{
		auto dest = (unsigned char *) baseAddress;
		auto end  = dest + blockSize;
		size_t count = 0;
		while( dest < end )
		{
			if( count == 32 )
			{
				count = 0;
				_lexer.ExpectEndLine();
			}
			*dest = _lexer.ExpectHexByte();
			++count;
			++dest;
		}
		_lexer.ExpectChar( '.' );
		_lexer.ExpectEndLine();
	}



	void InputFileSerialiser::Close()
	{
		_fileImage.clear();
	}





} // end namespace Jynx
