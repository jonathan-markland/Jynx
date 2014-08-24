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


namespace JynxZ80
{


	namespace Z80Flags
	{
		enum Enum
		{
			SF = 0x80,  // Sign
			ZF = 0x40,  // Zero
			XF = 0x20,  // Undocumented flag.
			HF = 0x10,  // Half-carry  (from bit 3 to bit 4, particularly used by DAA).
			YF = 0x08,  // Undocumented flag.
			PV = 0x04,  // Parity / Overflow (dual purpose depending on instruction that sets it).
			NF = 0x02,  // Negative flag (tells DAA instruction whether the previous instruction was ADD/SUB)
			CF = 0x01,  // Carry
		};
	}



	struct Z80SerialisableState
	{
		// Z80 main store (privately inherited).
		// Exposed for reading and writing by the serialiser.

		int32_t  _timesliceLength;
		int32_t  _remainingCycles;

		uint16_t  _BC;
		uint16_t  _DE;
		uint16_t  _HL;
		uint16_t  _AF;
		uint16_t  _IX;
		uint16_t  _IY;

		uint16_t  _alternateBC;
		uint16_t  _alternateDE;
		uint16_t  _alternateHL;
		uint16_t  _alternateAF;

		uint16_t  _programCounter;
		uint16_t  _stackPointer;

		uint8_t   _interruptMode;
		bool      _interruptsEnabled;     // TODO: EI and DI change this.  This will need to become IFF1 and IFF2.
		bool      _deferInterruptCheck;   // Used by EI only.
		bool      _cpuHalted;

		uint8_t   _I;
		uint8_t   _R;

	};



	struct ConditionTestingData
	{
		// Used to test a flag in the Z80 flags register.
		// We need a C-language compatible result: 
		// - If the result is 0, the condition is NOT SATISFIED.
		// - If the result is !0, the condition is SATISFIED.
		// To achieve the above, we take the Flags value, and apply the AND mask, then the Xor mask.

		const uint8_t  AndMask;      // The AND mask that selects the flag of interest from the F register.
		const uint8_t  XorMask;      // XOR mask to be applied after AND masking.  
	};



	inline  uint8_t &LoByte( uint16_t &reg )
	{
		// (This function is suitable for little-endian microprocessors only).
		// Return reference to the low byte of the register.
		return (uint8_t &) reg;
	}



	inline  uint8_t &HiByte( uint16_t &reg )
	{
		// (This function is suitable for little-endian microprocessors only).
		// Return reference to the high byte of the register.
		return *(&((uint8_t *) &reg)[1]);
	}



	inline  uint16_t  Make16( uint8_t hiByte, uint8_t loByte )
	{
		return loByte | ((uint16_t) hiByte) << 8;
	}


} // end namespace
