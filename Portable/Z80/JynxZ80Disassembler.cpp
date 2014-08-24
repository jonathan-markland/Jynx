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


#include "JynxZ80Disassembler.h"
#include "JynxZ80DisassemblyData.h"
#include <iomanip>


namespace JynxZ80
{

	Z80Disassembler::Z80Disassembler()
		: _sourceStream(nullptr)
		, _programCounter(0)
		, _insertBlankLine(false)
	{
	}



	void Z80Disassembler::SetStreamAndAddress( IZ80DisassemblerStream *sourceStream, uint16_t programCounter )
	{
		_sourceStream    = sourceStream;
		_programCounter  = programCounter;
		_insertBlankLine = false;
	}



	void Z80Disassembler::SetLinePrefix( const char *prefixString )
	{
		_prefixString = prefixString;
	}



	void Z80Disassembler::SetSeparator( const char *separatorString )
	{
		_separatorString = separatorString;
	}



	uint8_t  Z80Disassembler::Fetch()
	{
		auto opcode = _sourceStream->ReadByte();
		++_programCounter;
		return opcode;
	}



	std::string  Z80Disassembler::GetNextLine()
	{
		if( _insertBlankLine )
		{
			_insertBlankLine = false; // for next time
			return std::string("");
		}

		_thisRowString = std::stringstream();
		_thisRowString << std::setfill('0');
		_thisRowString << _prefixString << std::hex << std::setw(4) << _programCounter << std::setw(0) << _separatorString;

		auto opcode = Fetch();

	useless_prefix:

		if( opcode == 0xCB )
		{
			DisassembleCB();
		}
		else if( opcode == 0xED )
		{
			DisassembleED();
		}
		else if( opcode == 0xDD )
		{
			auto secondOpcode = Fetch();
			if( secondOpcode == 0xDD || secondOpcode == 0xFD )
			{
				opcode = secondOpcode;
				goto useless_prefix;  // Useless prefix detected.  NB: The final disassembly will be just the eventual instruction!
			}
			else if( secondOpcode == 0xCB )
			{
				DisassembleDDCB();
			}
			else
			{
				DisassembleDDMain( secondOpcode );
			}
		}
		else if( opcode == 0xFD )
		{
			auto secondOpcode = Fetch();
			if( secondOpcode == 0xDD || secondOpcode == 0xFD )
			{
				opcode = secondOpcode;
				goto useless_prefix;  // Useless prefix detected.  NB: The final disassembly will be just the eventual instruction!
			}
			else if( secondOpcode == 0xCB )
			{
				DisassembleFDCB();
			}
			else
			{
				DisassembleFDMain( secondOpcode );
			}
		}
		else
		{
			DisassembleMain( opcode );
		}

		return _thisRowString.str();
	}



	void Z80Disassembler::DisassembleMain( uint8_t opcode )
	{
		AppendInstructionDisassembly( g_DisassemblyMainTable[opcode], -1 );
	}



	void Z80Disassembler::DisassembleCB()
	{
		auto opcode = Fetch();
		AppendInstructionDisassembly( g_DisassemblyCB[opcode], -1 );
	}



	void Z80Disassembler::DisassembleED()
	{
		auto opcode = Fetch();
		AppendInstructionDisassembly( g_DisassemblyED[opcode], -1 );
	}



	void Z80Disassembler::DisassembleDDMain( uint8_t opcode )
	{
		AppendInstructionDisassembly( g_DisassemblyDD[opcode], -1 );
	}



	void Z80Disassembler::DisassembleFDMain( uint8_t opcode )
	{
		AppendInstructionDisassembly( g_DisassemblyFD[opcode], -1 );
	}


	void Z80Disassembler::DisassembleDDCB()
	{
		auto offset = Fetch();
		auto opcode = Fetch();
		AppendInstructionDisassembly( g_DisassemblyDDCB[opcode], offset );
	}



	void Z80Disassembler::DisassembleFDCB()
	{
		auto offset = Fetch();
		auto opcode = Fetch();
		AppendInstructionDisassembly( g_DisassemblyFDCB[opcode], offset );
	}



	void Z80Disassembler::AppendInstructionDisassembly( const struct Disasm &instructionRecord, int offsetOrMinusOne )
	{
		// Get the template string for this instruction:

		auto source = instructionRecord.DisassembledForm;

		// Expand the special characters in the disassembly string:

		if( *source == '.' )
		{
			_insertBlankLine = true;  // applies AFTER this instruction's line.
			++source;
		}

		for(;;)
		{
			auto ch = *source;
			if( ch == 0 )
			{
				break;
			}
			else if( ch == '*' )
			{
				FetchAndAppend8bitIntelligentFormatting();
			}
			else if( ch == '#' )
			{
				FetchAndAppend16bitIntelligentFormatting();
			}
			else if( ch == '!' )
			{
				FetchAndAppend16bitAddressFormatting();
			}
			else if( ch == '$' )
			{
				FetchAndAppend8BitRelativeBranchDisplacement();
			}
			else if( ch == '%' )
			{
				if( offsetOrMinusOne == -1 )
				{
					// Fetch the offset byte, because the caller isn't passing one in.
					offsetOrMinusOne = Fetch();
				}
				FetchAndAppend8bitSignedOffset( offsetOrMinusOne );
			}
			else 
			{
				_thisRowString << ch;
			}
			++source;
		}
	}



	void Z80Disassembler::AppendHex8( uint8_t value )
	{
		_thisRowString << "0x" << std::hex << std::setw(2) << ((uint32_t) value) << std::setw(0) ;
	}



	void Z80Disassembler::AppendHex16( uint16_t value )
	{
		_thisRowString << "0x" << std::hex << std::setw(4) << ((uint32_t) value) << std::setw(0);
	}



	//    *     8-bit immediate value.  Show intelligent.
	//    #    16-bit immediate value.  Show intelligent.
	//    !    16-bit immediate value used for an address.  Show as hex only to 16-bits.
	//    $     8-bit relative branch displacement.  Calculate and show as a hex address to 16 bits.
	//    %     8-bit signed offset for indirect addressing.  Show as decimal only, with prefixed '+' if 0 or positive.
	//    .     Only appears as the very first character.  Indicates blank line will be inserted AFTER this instruction.

	void Z80Disassembler::FetchAndAppend8bitIntelligentFormatting()
	{
		// For 8-bit:
		// - If 10..255 then show as hexadecimal with "0x" prefix.
		// - Then if 32..126 append 8-bit as a character, enclosed in single quotes.
		// - Then:
		//      Show as unsigned decimal.
		//      If 128..255 show as unsigned decimal first, then signed decimal value (which will be -ve) in brackets.

		auto value = Fetch();

		if( value >= 10 && value <= 255 )
		{
			// hex (only if worth it)
			AppendHex8( value ); 
			_thisRowString << ' ';  
		}

		if( value >= 32 && value <= 126 )
		{
			_thisRowString << "\'" << ((char) value) << "\' ";  // character representation
		}

		_thisRowString << std::dec << ((uint32_t) value) << ' ';   // unsigned decimal
	
		if( value >= 128 && value <= 255 )
		{
			_thisRowString << std::dec << ((int32_t) (int8_t) value) << ' ';   // signed (negative case)
		}
	}



	void Z80Disassembler::FetchAndAppend16bitIntelligentFormatting()
	{
		// For 16-bit:
		// - If 10..65535 show as hexadecimal with "0x" prefix.
		// - Then:
		//      Show as unsigned decimal.
		//      If 32768..65535 show as unsigned decimal first, then signed decimal value (which will be -ve) in brackets.

		auto value0 = Fetch();
		auto value1 = Fetch();

		uint16_t value = (value1 << 8) | value0;

		if( value >= 10 && value <= 65535 )
		{
			// hex (only if worth it)
			AppendHex16( value ); 
			_thisRowString << ' ';  
		}

		_thisRowString << std::dec << ((uint32_t) value) << ' ';   // unsigned decimal
	
		if( value >= 32768 && value <= 65535 )
		{
			_thisRowString << std::dec << ((int32_t) (int16_t) value) << ' ';   // signed (negative case)
		}
	}



	void Z80Disassembler::FetchAndAppend16bitAddressFormatting()
	{
		auto value0 = Fetch();
		auto value1 = Fetch();
		uint16_t value = (value1 << 8) | value0;
		AppendHex16( value );
	}



	void Z80Disassembler::FetchAndAppend8BitRelativeBranchDisplacement()
	{
		int8_t branchDisplacement = (int8_t) Fetch();
		auto targetAddress = _programCounter + ((int16_t) branchDisplacement);
		AppendHex16( targetAddress );
	}



	void Z80Disassembler::FetchAndAppend8bitSignedOffset( int8_t offset )
	{
		if( offset < 128 ) _thisRowString << '+';
		_thisRowString << std::dec << ((int32_t) (int8_t) offset);   // signed
	}



} // end namespace

