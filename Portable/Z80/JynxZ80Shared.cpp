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


#include "JynxZ80.h"


namespace JynxZ80
{


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     CONDITION TESTING TABLE
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	ConditionTestingData  ConditionalExecutionMasksAndExpectations[8] = 
	{ 
		// In order of instruction encoding:

		//    AND            XOR
		{ Z80Flags::ZF, Z80Flags::ZF },   // NZ
		{ Z80Flags::ZF,       0      },   // Z
		{ Z80Flags::CF, Z80Flags::CF },   // NC
		{ Z80Flags::CF,       0      },   // C
		{ Z80Flags::PV, Z80Flags::PV },   // PO
		{ Z80Flags::PV,       0      },   // PE
		{ Z80Flags::SF, Z80Flags::SF },   // P
		{ Z80Flags::SF,       0      },   // M

		// EG:
		// - We desire to test if "Z".
		//   F register's Z is 1 when satisfied.      So: (F AND Z80Flags::ZF) XOR 0 = Z80Flags::ZF (ie: non-zero result so TRUE in C)
		//   F register's Z is 0 when not satisfied.  So: (F AND Z80Flags::ZF) XOR 0 =      0       (ie: zero result so FALSE in C)
	};



} // end namespace

