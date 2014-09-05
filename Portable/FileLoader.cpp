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

#include "FileLoader.h"

namespace Jynx
{

	void LoadFileIntoVector( IFileOpener *fileOpener, std::vector<uint8_t> &result )
	{
		std::ifstream  inStream;
		fileOpener->OpenInputStream( inStream, std::ios::in | std::ios::binary | std::ios::ate );

		uint64_t fileSize = inStream.tellg();
		if( fileSize < 0x80000000ui64 )
		{
			result = std::vector<uint8_t>( (uint32_t) fileSize, uint8_t(0) );
			if( ! result.empty() )
			{
				inStream.seekg (0, std::ios::beg);
				inStream.read( (char *) &(*result.begin()), fileSize );
			}
			inStream.close();
			return;
		}

		inStream.close();
		throw std::runtime_error( "File is too large." );
	}



	void SaveFileFromVector( IFileOpener *fileOpener, const std::vector<uint8_t> &fileImage )
	{
		std::ofstream  outStream;

		fileOpener->OpenOutputStream( outStream, std::ios::binary | std::ios::out );

		if( ! fileImage.empty() ) // begin() on an empty vector raise debug assert on Windows
		{
			auto startAddress = &(*fileImage.begin());
			outStream.write( (const char *) startAddress, fileImage.size() );
		}

		outStream.close();
	}


} // end namespace Jynx
