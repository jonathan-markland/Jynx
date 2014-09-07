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
#include <string>
#include <vector>
#include "IFileOpener.h"

namespace Jynx
{
	class TapFileSplitter
	{
	public:

		TapFileSplitter();  
			// The constructor for an empty tape.

		explicit TapFileSplitter( IFileOpener *tapFile );
			// If the above fails, to parse the TAP file, we throw a std::runtime_error.

		size_t GetNumberOfFiles() const;
			// Return number of files containing in this TAP.

		std::vector<uint16_t>  GenerateWaveformForFile( size_t fileIndex, uint32_t bitsPerSecond );
			// Returns the wave data for the file at the given index.
			// The return wave comprises:
			// - The initial SYNC
			// - The file name
			// - The second SYNC
			// - The main data block
			// ... as the Lynx uses.
			// - bitsPerSecond should be the "TAPE" speed the Lynx is currently 
			//   expecting: { 600, 900, 1200, 1500, 1800, 2100 } for TAPE 0-5 resp.

		std::string  GetTapeDirectory() const;
			// Retrieves text giving tape content and file types (LOAD / MLOAD).

	private:

		std::vector<std::string>  _fileNames;  // In the block after the FIRST sync, the Lynx stores the file name, in quotes.
		std::vector< std::vector<uint8_t> >  _contentImages;  // In the block after the second sync, the type-stamp, data-length and raw data are encoded.

		void LoadAndParseTapFile( IFileOpener *tapFileOpener );

	};

} // end namespace Jynx
