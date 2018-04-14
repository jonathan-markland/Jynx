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

#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "stdint.h"

#include "JynxZ80Timings.h"
#include "JynxZ80.h"
#include "JynxZ80Shared.h"

namespace JynxZ80
{

	// TODO: We could do with testing DAA with the "after subtract" scenario (Z80Flags::NF is set)
	// TODO: Simplify rotate and SLA etc by having an inline handler function to call.
	// TODO: Strategy change: (IX+dd) forms should work by storing the computed address in a temporary 16-bit register,
	//       then re-directing the (HL) operations of the main set / CB set to use the temporary address register instead.
	// TODO: Clearly indicate opcode bytes(s) against all cases




	//
	// Decoding notes
	// ~~~~~~~~~~~~~~
	//
	// The 256 byte opcode map is considered as an 8*32 arrangement,
	// which is further divided into areas known as "quarters".
	// This supports what patterns the Z80 does have!
	// - A "row" refers to a row of 8, within a quarter.  Members of a row are found by AND-masking the opcode with 7.
	// - A "column" refers to a vertical column of 8 bytes.  Members of a column are found by AND-masking the opcode with 0x38.
	// Some example columns and rows are illustrated below.
	//
	//
	//  "Quarter 0" :
	//
	//   Q0 Column 0  (includes 00, 08, 10, 18, 20, 28, 30, 38)
	//   |                    
	//   |                           Q0 Column 7  (includes 07, 0F, 17, 1F, 27, 2F, 37, 3F)
	//   |                           |
	//   v                           v
	//   00  01  02  03  04  05  06  07   <-- row 0  (includes 00..07)
	//   08  09  0A  0B  0C  0D  0E  0F
	//   10  11  12  13  14  15  16  17
	//   18  19  1A  1B  1C  1D  1E  1F
	//   20  21  22  23  24  25  26  27
	//   28  29  2A  2B  2C  2D  2E  2F
	//   30  31  32  33  34  35  36  37
	//   38  39  3A  3B  3C  3D  3E  3F   <-- row 7  (includes 38..3F)
	//
	//  "Quarter 1" :
	//   40  41  42  43  44  45  46  47   <-- row 0
	//   48  49  4A  4B  4C  4D  4E  4F
	//   50  51  52  53  54  55  56  57
	//   58  59  5A  5B  5C  5D  5E  5F
	//   60  61  62  63  64  65  66  67
	//   68  69  6A  6B  6C  6D  6E  6F
	//   70  71  72  73  74  75  76  77
	//   78  79  7A  7B  7C  7D  7E  7F   <-- row 7
	//
	//  "Quarter 2" :
	//   80  81  82  83  84  85  86  87   <-- row 0
	//   88  89  8A  8B  8C  8D  8E  8F
	//   90  91  92  93  94  95  96  97
	//   98  99  9A  9B  9C  9D  9E  9F
	//   A0  A1  A2  A3  A4  A5  A6  A7
	//   A8  A9  AA  AB  AC  AD  AE  AF
	//   B0  B1  B2  B3  B4  B5  B6  B7
	//   B8  B9  BA  BB  BC  BD  BE  BF   <-- row 7
	//
	//  "Quarter 3" :
	//   C0  C1  C2  C3  C4  C5  C6  C7   <-- row 0
	//   C8  C9  CA  CB  CC  CD  CE  CF
	//   D0  D1  D2  D3  D4  D5  D6  D7
	//   D8  D9  DA  DB  DC  DD  DE  DF
	//   E0  E1  E2  E3  E4  E5  E6  E7
	//   E8  E9  EA  EB  EC  ED  EE  EF
	//   F0  F1  F2  F3  F4  F5  F6  F7
	//   F8  F9  FA  FB  FC  FD  FE  FF   <-- row 7
	//   ^                           ^
	//   |                           |
	//   |                           Q3 Column 7  (includes C7, CF, D7, DF, E7, EF, F7, FF)
	//   |                    
	//   Q3 Column 0  (includes C0, C8, D0, D8, E0, E8, F0, F8)
	//






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     Z80 member functions
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


	static bool g_Z80GlobalInitDone = false;



	Z80::Z80()
	{
		assert( g_Z80GlobalInitDone ); // Assert if you didn't call InitialiseGlobalTables().
		_timesliceLength = 0;
		_currentOpcode = 0; // Not essential
		Reset();
	}



	void Z80::Reset()
	{
		_programCounter     = 0;
		_stackPointer       = 0;
	
		_AF     = 0;
		_BC     = 0;
		_DE     = 0;
		_HL     = 0;
		_IX     = 0;
		_IY     = 0;

		_alternateAF    = 0;
		_alternateBC    = 0;
		_alternateDE    = 0;
		_alternateHL    = 0;

		_interruptMode = 0;
		_I         = 0;
		_R         = 0;

		_cpuHalted = false;
		_interruptsEnabled = false;
		_deferInterruptCheck = false;

		_remainingCycles = _timesliceLength;

		_hiddenRegisterSix = 0;

		_addressesOf8BitRegisters_NoRedirection[0] = &HiByte(_BC);         // B
		_addressesOf8BitRegisters_NoRedirection[1] = &LoByte(_BC);         // C
		_addressesOf8BitRegisters_NoRedirection[2] = &HiByte(_DE);         // D
		_addressesOf8BitRegisters_NoRedirection[3] = &LoByte(_DE);         // E
		_addressesOf8BitRegisters_NoRedirection[4] = &HiByte(_HL);         // H  (always H, never IXH / IYH)
		_addressesOf8BitRegisters_NoRedirection[5] = &LoByte(_HL);         // L  (always L, never IXL / IYL)
		_addressesOf8BitRegisters_NoRedirection[6] = &_hiddenRegisterSix;  // The holding register.
		_addressesOf8BitRegisters_NoRedirection[7] = &HiByte(_AF);         // A

		_addressesOf8BitRegisters[0] = &HiByte(_BC);         // B
		_addressesOf8BitRegisters[1] = &LoByte(_BC);         // C
		_addressesOf8BitRegisters[2] = &HiByte(_DE);         // D
		_addressesOf8BitRegisters[3] = &LoByte(_DE);         // E
		_addressesOf8BitRegisters[4] = &HiByte(_HL);         // H / IXH (when DD) / IYH (when DD)
		_addressesOf8BitRegisters[5] = &LoByte(_HL);         // L / IXL (when FD) / IYL (when FD)
		_addressesOf8BitRegisters[6] = &_hiddenRegisterSix;  // The holding register.
		_addressesOf8BitRegisters[7] = &HiByte(_AF);         // A

		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[0] = &_BC;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[1] = &_DE;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[2] = &_HL;    // HL / IX (with DD) / IY (with FD)
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[3] = &_stackPointer;

		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[0] = &_BC;
		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[1] = &_DE;
		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[2] = &_HL;    // HL / IX (with DD) / IY (with FD)
		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[3] = &_AF;
	}





	void Z80::UndefinedInstruction()
	{
		assert(false);  // TODO: sort out
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    MAIN LOOP
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::RunForTimeslice()
	{
		while(_remainingCycles > 0)
		{
			//
			// Fetch - Execute
			//

			PrimaryOpcodeFetch();
			Spend( Z80MainTimings[ _currentOpcode ] );
			ExecuteOpcodeMainSet();

			//
			// Interrupt handling -- TODO: sort of work in progress, as the Lynx implementation didn't emulate anything that used interrupts.
			//

			if( _deferInterruptCheck )
			{
				// An instruction was executed that defers interrupt acceptance until AFTER the next instruction.
				_deferInterruptCheck = false;
				// NB: Do NOT accept pending interrupt this time!
			}
			else if( _interruptsEnabled && _interruptSignalled )
			{
				// Ensure CPU is not in the HALT state:
				_cpuHalted = false;

				// TODO: We can accept this interrupt now.
				// (TODO: call interrupt handler actions)
				// TODO: _interruptSignalled = false;
			}
			else if( _cpuHalted )
			{
				// No interrupt, and still in HALT state.
				// Let's not waste host time on this.
				_remainingCycles = 0; // flush all remaining cycles in this timeslice.
				break;
			}
		}

		_remainingCycles += _timesliceLength;  // for next time.
	}




	void Z80::SetTimesliceLength( int32_t numCycles )
	{ 
		auto difference = numCycles - _timesliceLength;
		_timesliceLength = numCycles; 
		_remainingCycles += difference;  // So we don't inadvertently lie about passage of time for interested parties.
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    SUPPORT FOR SERIALISATION
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	const Z80SerialisableState &Z80::GetSerialisableVariables() const
	{
		return *static_cast<const Z80SerialisableState *>(this);
	}



	void Z80::SetSerialisableVariables( const Z80SerialisableState &state )
	{
		// We just accept this in from the serialiser:
		*static_cast<Z80SerialisableState *>(this) = state;
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     FLAGS
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



	uint8_t Z80::_signParityAndZeroTable[256];



	uint32_t Parity( uint8_t value8 )
	{
		return (0x9669 >> (((value8 >> 4) ^ (value8)) & 0x0f)) & 1;
	}



	void Z80::CalculateSignParityAndZeroTable() // static
	{
		// Calculated once, on construction.

		for( uint32_t i=0; i<256; i++ )
		{
			_signParityAndZeroTable[i] = Parity(i) ? Z80Flags::PV : 0;
		}

		for( uint32_t i=128; i<256; i++ )
		{
			_signParityAndZeroTable[i] |= Z80Flags::SF;
		}

		_signParityAndZeroTable[0] |= Z80Flags::ZF;
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//       FIX UP TIMINGS TABLES
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void FillMissingTimings()
	{
		//
		// Fill missing slots in Z80DDFDMainTimings table  (idempotent):
		//

		for( int i=0; i<256; i++ )
		{
			auto &thisTiming = Z80DDFDMainTimings[i];
			if( thisTiming == 0 )
			{
				// This instruction is a redundantly-prefixed DD or FD instruction.
				// Copy the timing from the corresponding index of the main instruction set timing table:
				thisTiming = Z80MainTimings[i];
			}
		}
	}



	void Z80::InitialiseGlobalTables()  // static
	{
		if( ! g_Z80GlobalInitDone )
		{
			g_Z80GlobalInitDone = true;
			CalculateSignParityAndZeroTable();
			FillMissingTimings();
		}
	}


} // end namespace


