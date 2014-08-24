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
#include "IBinarySerialiser.h"


namespace Jynx
{
	class ISerialiser: public IBinarySerialiser
	{
	public:

		// Serialisation interface.
		// This supports the snapshot's serialise-to-fairly-simple-not-as-nasty-as-XML text.

		virtual void OpenTag(  const char *tagName ) = 0;
		virtual void CloseTag( const char *tagName ) = 0;
		virtual void Field(    const char *tagName, uint8_t  &field ) = 0;
		virtual void Field(    const char *tagName, uint16_t &field ) = 0;
		virtual void Field(    const char *tagName, int32_t  &field ) = 0;
		virtual void Binary(   void *baseAddress,   uintptr_t blockSize ) = 0;


		// TODO: The following is not really very pure design!

		inline void Field( const char *tagName, bool &field )  // TODO: make first class?
		{
			uint8_t translatedField = field ? 1 : 0;
			Field( tagName, translatedField );
			field = translatedField ? true : false;
		}

		// Very type-safe design for static array serialisation:

		template<typename T, size_t N>
		void Binary( T (&arrayToSerialise)[N] )
		{
			Binary( &arrayToSerialise, sizeof(arrayToSerialise) );
		}

		template<typename T, size_t N>
		void Binary( const char *tagName, T (&arrayToSerialise)[N] )
		{
			OpenTag( tagName );
			Binary( &arrayToSerialise, sizeof(arrayToSerialise) );
			CloseTag( tagName );
		}

	};

} // end namespace Jynx
