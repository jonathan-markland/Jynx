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

#include "LinuxFileOpener.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     FILE PATH FORMAT ABSTRACTION ON LINUX
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

LinuxFileOpener::LinuxFileOpener( const char *filePath )
	: _filePath( filePath )
{
}



LinuxFileOpener::LinuxFileOpener( const std::string &filePath )
	: _filePath( filePath )
{
}



void LinuxFileOpener::OpenOutputStream( std::ofstream &stream, std::ios_base::openmode openModeRequired )
{
	stream.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
	stream.open( _filePath.c_str(), openModeRequired );
}



void LinuxFileOpener::OpenInputStream(  std::ifstream &stream, std::ios_base::openmode openModeRequired )
{
	stream.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
	stream.open( _filePath.c_str(), openModeRequired );
}
