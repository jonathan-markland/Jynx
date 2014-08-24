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

#include "Lexer.h"


namespace Jynx
{
	Lexer::Lexer()
		: _position(nullptr)
	{
	}


	void Lexer::SetPos( const char *pos )
	{
		_position = pos;
	}


	void Lexer::ExpectChar( char ch )
	{
		if( *_position == ch )
		{
			++_position;
		}
		else Error();
	}


	void Lexer::ExpectMatch( const char *tagName )
	{
		while( *tagName && *_position == *tagName )
		{
			++_position;
			++tagName;
		}
		if( *tagName != '\0' ) Error();
	}


	void Lexer::ExpectEndLine()
	{
		auto start = _position;
		if( *_position == 13 )
		{
			++_position;
		}
		if( *_position == 10 )
		{
			++_position;
		}
		if( _position == start ) Error();
	}


	uint8_t Lexer::NibbleToValue( char ch )
	{
		if( ch >= '0' && ch <= '9' ) return (ch - '0');
		if( ch >= 'A' && ch <= 'F' ) return (ch - ('A' - 10));
		Error();
		return 0; // never executed
	}


	uint8_t Lexer::ExpectHexByte()
	{
		auto result = (NibbleToValue( _position[0] ) << 4)  |  NibbleToValue( _position[1] );
		_position += 2;
		return (uint8_t) result;
	}


	uint32_t Lexer::ExpectInteger()
	{
		if( isdigit(*_position) )
		{
			uint32_t value = 0;
			while( isdigit(*_position) )
			{
				auto thisDigitValue = (*_position) - '0';
				value *= 10;
				value += thisDigitValue;
				++_position;
			}
			return value;
		}
		Error();
		return 0; // never executed
	}


	void Lexer::Error()
	{
		// TODO: include _position in the message
		throw std::runtime_error( "File parsing failed because of a syntax error." );
	}

} // end namespace Jynx
