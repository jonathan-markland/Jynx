//
// JynxZ80 - Jonathan's Z80 Emulator - Initially for Camputers Lynx Emulation Project.
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
#include <sstream>
#include "IZ80DisassemblerStream.h"


namespace JynxZ80
{

	class Z80Disassembler
	{
	public:

		// Disassembler for the Z80.
		// This requires a byte stream to source fetching the bytes sequentially.  Random-access is not required.

		Z80Disassembler();

		void SetStreamAndAddress( IZ80DisassemblerStream *sourceStream, uint16_t programCounter );
			// The next byte fetched from the stream will be at address 'programCounter'.

		void SetLinePrefix( const char *prefixString );
			// This optional string is output at the left side of each line.

		void SetSeparator( const char *separatorString );
			// This string is output between the address and the instruction.

		std::string  GetNextLine();
			// Retrieves the next instruction, in disassembled form.
			// The return string does NOT contain any line ending characters.
			// - This is a state-machine, multiple calls to this function will continue
			//   disassembly.

	private:

		uint8_t  Fetch();

		void DisassembleMain( uint8_t opcode );
		void DisassembleCB();
		void DisassembleED();
		void DisassembleDDMain( uint8_t opcode );
		void DisassembleFDMain( uint8_t opcode );
		void DisassembleDDCB();
		void DisassembleFDCB();

		void AppendInstructionDisassembly( const struct Disasm &, int offsetOrMinusOne );

		void FetchAndAppend8bitIntelligentFormatting();
		void FetchAndAppend16bitIntelligentFormatting();
		void FetchAndAppend16bitAddressFormatting();
		void FetchAndAppend8BitRelativeBranchDisplacement();
		void FetchAndAppend8bitSignedOffset( int8_t offset );

		void AppendHex8( uint8_t value );
		void AppendHex16( uint16_t value );

		IZ80DisassemblerStream *_sourceStream;
		uint16_t _programCounter;
		bool     _insertBlankLine;
		std::stringstream _thisRowString;
		std::string  _prefixString;
		std::string  _separatorString;

	};


} // end namespace
