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
#include "JynxZ80Timings.h"
#include "JynxZ80Shared.h"

namespace JynxZ80
{



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    CB
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteOpcodeCB()
	{
		// (Unprefixed) CB group

		PrimaryOpcodeFetch();
		Spend( Z80CBTimings[ _currentOpcode ] );

		auto columnNumber = GetColumnNumber( _currentOpcode );
		if( columnNumber == 6 )
		{
			// (HL) case:  Fetch value to the Z80 temporary register:
			_hiddenRegisterSix	= GuestRead( _HL );

			if( (_currentOpcode & 0xC0) == 0x40 )
			{
				// The BIT will not WRITE back, so disable the write-back if() below:
				columnNumber = 0;
			}
		}

		ExecuteCB_OnSpecificSlot( GetReferenceToReg8_FromBits2to0OfOpcode() );

		if( columnNumber == 6 )
		{
			// Write-back:  Only QUARTER 0, 2, and 3 write back.
			GuestWrite( _HL, _hiddenRegisterSix );
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    Entry points for prefixed-CB operations
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


	void Z80::ExecuteOpcodeDDCB()
	{
		// DD CB entry point

		Execute_DDCB_FDCB( _IX );
	}



	void Z80::ExecuteOpcodeFDCB()
	{
		// FD CB entry point

		Execute_DDCB_FDCB( _IY );
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    CB implementation
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteCB_OnSpecificSlot( uint8_t &targetRegister )
	{
		// NOTE - The register / temporary register to operate on is given by "targetRegister".
		//        So the register portion of the opcode is ignored by this routine!
		// NOTE - Called by DDCB and FDCB as well as just CB!
		// NOTE - This routine should not adjust the cycle counter.

		auto opcodeByte = _currentOpcode;
		auto bitNumber  = GetRowIndexWithinColumnFromOpcode( opcodeByte );  // also the instruction index for "quarter == 0x00" case.

		switch( opcodeByte >> 6 )
		{
		case 0:  // Quarter 0
		{
			auto workingRegister = targetRegister;  // Lets avoid repeated indirect fetches through the reference.
			uint8_t carryOut;

			switch( bitNumber )
			{
			case 0:  // RLC -- register rotated left where bit 7 goes into bit 0, original bit 7 goes to CF
				carryOut = workingRegister >> 7;  // carryOut == originalBit7MovedToBit0
				workingRegister = (workingRegister << 1) | carryOut;
				break;

			case 1:  // RRC -- register rotated right where bit 0 goes into bit 7, original bit 0 goes to CF
				carryOut = workingRegister & 1;  // carry == originalBit0
				workingRegister = (workingRegister >> 1) | (carryOut << 7);
				break;

			case 2:  // RL -- rotate THROUGH the carry
				carryOut = workingRegister >> 7;  // carry == originalBit7InBit0
				workingRegister = (workingRegister << 1) | CurrentCarry(); // originalCarryFlagInBit0
				break;

			case 3:  // RR -- rotate THROUGH the carry
				carryOut = workingRegister & 1;  // carryOut = originalBit0
				workingRegister = (workingRegister >> 1) | (CurrentCarry() << 7);      // originalCarryFlagInBit0 << 7
				break;

			case 4: // SLA -- 0 shifted in, original bit 7 goes out to CF
				carryOut = workingRegister >> 7;  // carryOut == originalBit7InBit0
				workingRegister <<= 1;
				break;

			case 5: // SRA -- arithmetic right, sign bit repeats itself, original bit 0 out to CF.
				carryOut = workingRegister & 1;  // carryOut == original bit 0
				workingRegister = (uint8_t)  (((int8_t) workingRegister) >> 1);  // coerce signed shift
				break;

			case 6: // SLL -- 1 shifted in, original bit 7 goes out to CF
				// (This is not an official instruction).
				carryOut = workingRegister >> 7;   // carryOut = originalBit7InBit0
				workingRegister = (workingRegister << 1) | 0x01;  // bizzarely a 1 comes in!
				break;

			case 7: // SRL -- 0 shifted in, original bit 0 goes out to CF
				carryOut = workingRegister & 1;   // carryOut = original bit 0
				workingRegister >>= 1;
				break;
			}

			// Record new state of flags, and include the carry flag determined above:
			SetFlags( ZeroSignAndParity8(workingRegister) | carryOut );

			// Let's have ONE write to the target register variable (through the reference):
			targetRegister = workingRegister;  
			break;
		}
	
		case 1:  // Quarter 1
			// BIT
			// Parity is "unknown", but let's set it anyway.
			SetFlags( ZeroSignAndParity8( targetRegister & (1 << bitNumber) ) | Z80Flags::HF | CurrentCarry() );
			break;

		case 2:  // Quarter 2
			// RES
			// No flags affects at all.
			targetRegister &= ~(1 << bitNumber);
			break;

		case 3:  // Quarter 3
			// SET
			// No flags affects at all.
			targetRegister |= 1 << bitNumber;
			break;
		}
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    DDCB and FDCB implementation
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::Execute_DDCB_FDCB( uint16_t &regIXIY )
	{
		// Reminder:  The DDCB and FDCB handling doesn't handle the IX and IY registers
		// the same way as the main set -- the "re-direction" features / flags in the Z80
		// struct are not used in this implementation, because ExecuteCB_OnSpecificSlot()
		// can simply be told what to operate on.

		// Remember:  ** No timings table for prefixed CB **  Timing *non-inclusive* of prefixes.

		auto offsetByte = CodeStreamFetch();
		PrimaryOpcodeFetch();

		auto operandAddress = regIXIY + (int8_t) offsetByte;
		auto quarter = (_currentOpcode & 0xC0);
		if( quarter != 0x40 )
		{
			// Quarter 0:  RLC RRC RL RR SLA SRA SLL SRL -- only (IX+dd) form supported, rest unofficial anyway
			// Quarter 2:  RES -- only (IX+dd) form supported, rest unofficial anyway
			// Quarter 3:  SET -- only (IX+dd) form supported, rest unofficial anyway
			if( GetColumnNumber(_currentOpcode) == 6 )
			{
				// (IX+dd) form
				Spend( 19 );    // ie: Standard timing less prefix (4)
				auto tempRegister = GuestRead( operandAddress );
				ExecuteCB_OnSpecificSlot( tempRegister );
				GuestWrite( operandAddress, tempRegister );  // QUARTER 0, 2, 3 write back
			}
			else
			{
				UndefinedInstruction(); // TODO: undocumented opcode -- maybe support known behaviours, probably look at using _hiddenRegisterSix
			}
		}
		else
		{
			// BIT
			// quarter == 0x40 is the weird case where all rows of 8 do the (IX+nn) form!
			Spend( 16 );  // ie: Standard timing less prefix (4)
			auto tempRegister = GuestRead( operandAddress );
			ExecuteCB_OnSpecificSlot( tempRegister );
		}
	}


} // end namespace

