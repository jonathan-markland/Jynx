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

#include <cstring>

namespace Jynx
{
	template<typename T, size_t N>
	void ZeroInitialiseMemory( T (&arrayToInitialise)[N] )
	{
		memset( &arrayToInitialise, 0, sizeof(T[N]) );
	}



	template<typename T, size_t N>
	void CopyArrayMemory( T (&destinationArray)[N], const T (&sourceArray)[N] )
	{
		memcpy( destinationArray, sourceArray, sizeof(T[N]) );
	}



	template<typename T>
	void ZeroInitialiseMemory( T &itemToZeroInitialise )
	{
		memset( &itemToZeroInitialise, 0, sizeof(T) );
	}



	template<typename T, size_t N>
	void InitialiseAllArrayElements( T (&arrayToInitialise)[N], const T &value )
	{
		auto p = &arrayToInitialise[0];
		auto e = &arrayToInitialise[N];
		while( p < e )
		{
			*p = value;
			++p;
		}
	}

} // end namespace Jynx
