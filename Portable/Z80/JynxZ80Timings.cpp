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

//
// Z80 Instruction set timing tables.
//
// A nice, well-presented resource  (with thanks!):
//
//     http://clrhome.org/table/#
//
// Notes on how I handle instruction timings:
//
// - Standard Z80 documentation typically gives the timings *inclusive* of fetching of the prefix bytes.
// - For ease of comparison the following tables are in these standard times.
// - However DD and FD prefixes can be redundantly repeated (only the final one applies).
// - So I use different accountancy:
//     1. For the prefixed tables:  Subtract 4 from the standard timings to remove the "prefix effect".
//     2. In the emulator, the prefix bytes consume 4 cycles by themselves, not as part of the instruction
//        to which they are attached -- that is separately timed.
//     3. Handle the CB and ED prefixes the same way for consistency.
//

#include "JynxZ80Timings.h"

// The following macro wraps PREFIXED instruction timings only.  It subtracts the prefix time.

#define T(TIMING)  (TIMING - 4)


namespace JynxZ80
{
	//
	// Primary instruction set timings.
	//
	// NB:  CB, DD, ED, FD all have "4" the timing here, because that's the prefix overhead time.
	//

	uint8_t Z80MainTimings[256] =
	{
	//  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f 
		4, 10,  7,  6,  4,  4,  7,  4,  4, 11,  7,  6,  4,  4,  7,  4,  // 0x
		8, 10,  7,  6,  4,  4,  7,  4, 12, 11,  7,  6,  4,  4,  7,  4,  // 1x
		7, 10, 16,  6,  4,  4,  7,  4,  7, 11, 16,  6,  4,  4,  7,  4,  // 2x
		7, 10, 13,  6, 11, 11, 10,  4,  7, 11, 13,  6,  4,  4,  7,  4,  // 3x

		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 4x
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 5x
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 6x
		7,  7,  7,  7,  7,  7,  4,  7,  4,  4,  4,  4,  4,  4,  7,  4,  // 7x

		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 8x
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // 9x
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // Ax
		4,  4,  4,  4,  4,  4,  7,  4,  4,  4,  4,  4,  4,  4,  7,  4,  // Bx

		5, 10, 10, 10, 10, 11,  7, 11,  5, 10, 10,  4, 10, 17,  7, 11,  // Cx
		5, 10, 10, 11, 10, 11,  7, 11,  5,  4, 10, 11, 10,  4,  7, 11,  // Dx
		5, 10, 10, 19, 10, 11,  7, 11,  5,  4, 10,  4, 10,  4,  7, 11,  // Ex
		5, 10, 10,  4, 10, 11,  7, 11,  5,  6, 10,  4, 10,  4,  7, 11,  // Fx
	};



	//
	// Timings for DD and FD prefixes (applied to the main instruction set).
	//
	// - Slots marked 'COPY' are filled at initialisation with the value in the 
	//   corresponding slot in the main instruction set, see Z80MainTimings[].
	//

	#define COPY 0

	uint8_t Z80DDFDMainTimings[256] =
	{
	//     0      1      2      3      4      5      6      7      8      9      a      b      c      d      e      f 
		 COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, T(15),  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // 0x
		 COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, T(15),  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // 1x
		 COPY, T(14), T(20), T(10),  T(8),  T(8), T(11),  COPY,  COPY, T(15), T(20), T(10),  T(8),  T(8), T(11),  COPY, // 2x
		 COPY,  COPY,  COPY,  COPY, T(23), T(23), T(19),  COPY,  COPY, T(15),  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // 3x
	
		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // 4x
		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // 5x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(19),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(19),  T(8), // 6x
		T(19), T(19), T(19), T(19), T(19), T(19),  COPY, T(19),  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // 7x

		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // 8x
		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // 9x
		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // Ax
		 COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY,  COPY,  COPY,  COPY,  COPY,  T(8),  T(8), T(19),  COPY, // Bx
	
		 COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // Cx
		 COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // Dx
		 COPY, T(14),  COPY, T(23),  COPY, T(15),  COPY,  COPY,  COPY,  T(8),  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // Ex
		 COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, T(10),  COPY,  COPY,  COPY,  COPY,  COPY,  COPY, // Fx
	};

	#undef COPY


	//
	// CB opcode set timings.
	//
	// NOTE:  DD and FD prefixed CB instructions have timings handled in code (hence no table at all).
	//

	uint8_t Z80CBTimings[256] =
	{
	//     0      1      2      3      4      5      6      7      8      9      a      b      c      d      e      f 
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 0x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 1x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 2x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 3x

		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8), // 4x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8), // 5x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8), // 6x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(12),  T(8), // 7x

		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 8x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // 9x
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Ax
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Bx
	
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Cx
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Dx
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Ex
		 T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8),  T(8), T(15),  T(8), // Fx
	};



	//
	//  ED opcode set instruction timings -- except for undefineds.
	//
	//  The zeroes are overwritten with '8's during initialisation,
	//  since this is the basic timing for a two-byte instruction.
	//  All shall be treated as no-operation), but at least they take
	//  some time up.
	//

	#define UNDEF T(8)  // Let us at least give these undefined "instructions" the basic time taken to fetch a 2-byte instruction.

	uint8_t Z80EDTimings[256] =
	{
	//     0      1      2      3      4      5      6      7      8      9      a      b      c      d      e      f 
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 0x
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 1x
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 2x
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 3x

		T(12), T(12), T(15), T(20),  T(8), T(14),  T(8),  T(9), T(12), T(12), T(15), T(20),  T(8), T(14),  T(8),  T(9), // 4x
		T(12), T(12), T(15), T(20),  T(8), T(14),  T(8),  T(9), T(12), T(12), T(15), T(20),  T(8), T(14),  T(8),  T(9), // 5x
		T(12), T(12), T(15), T(20),  T(8), T(14),  T(8), T(18), T(12), T(12), T(15), T(20),  T(8), T(14),  T(8), T(18), // 6x
		T(12), T(12), T(15), T(20),  T(8), T(14),  T(8), UNDEF, T(12), T(12), T(15), T(20),  T(8), T(14),  T(8), UNDEF, // 7x

		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 8x
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // 9x
		T(16), T(16), T(16), T(16), UNDEF, UNDEF, UNDEF, UNDEF, T(16), T(16), T(16), T(16), UNDEF, UNDEF, UNDEF, UNDEF, // Ax
		T(16), T(16), T(16), T(16), UNDEF, UNDEF, UNDEF, UNDEF, T(16), T(16), T(16), T(16), UNDEF, UNDEF, UNDEF, UNDEF, // Bx

		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // Cx
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // Dx
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // Ex
		UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, UNDEF, // Fx
	};

} // end namespace



#undef UNDEF
#undef T
