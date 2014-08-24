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
	//    ED
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteOpcodeED()
	{
		PrimaryOpcodeFetch();
		Spend( Z80EDTimings[ _currentOpcode ] );
		switch( _currentOpcode >> 6 )
		{
		case 0: 	UndefinedInstruction(); break;
		case 1:		ExecuteED_Quarter1();   break;
		case 2:		ExecuteED_Quarter2();   break;
		case 3:		UndefinedInstruction(); break;
		};
	}




	void Z80::ExecuteED_Quarter1()
	{
		switch( _currentOpcode & 7 )
		{
		case 0:  // Column 0  (IN)
			{
				auto valueRead = GuestReadIOSpace( _BC );
				GetReferenceToReg8_FromBits5to3() = valueRead;
				SetFlags( ZeroSignAndParity8(valueRead) | CurrentCarry() );
			}
			break;

		case 1:  // Column 1  (OUT)
			// No flags effects.
			_hiddenRegisterSix = 0;  // in case it's ED 71 (undocumented "out (c),0")
			GuestWriteIOSpace( _BC, GetReferenceToReg8_FromBits5to3() );
			break;

		case 2:  // Column 2  (SBC HL,)  (ADC HL,)   { BC DE HL SP }   // reminder: ED never affected by DD/FD prefixing
			{
				auto &rhsRegister = GetReferenceTo_BC_DE_HLIXIY_SP_FromOpcodeBits5and4();
				auto initialCarry = CurrentCarry();
				if( _currentOpcode & 8 )
				{
					// ADC 16

					uint32_t  resultValue = (_HL + rhsRegister + initialCarry);

					SetFlags(
						CarryFlagFromBit16( resultValue )
						| OverflowFlagAfterAddition16( _HL, rhsRegister, resultValue )
						| HalfCarryAfterAddOrSubtract16( _HL, rhsRegister, resultValue )
						| SignAndZeroFlagFrom16BitNumber( resultValue ) );

					_HL = resultValue;
				}
				else
				{
					// SBC 16

					uint32_t  resultValue = (_HL - rhsRegister - initialCarry);

					SetFlags( Z80Flags::NF
						| CarryFlagFromBit16( resultValue )
						| OverflowFlagAfterSubtraction16( _HL, rhsRegister, resultValue )
						| HalfCarryAfterAddOrSubtract16( _HL, rhsRegister, resultValue )
						| SignAndZeroFlagFrom16BitNumber( resultValue ) );

					_HL = resultValue;
				}
			}
			break;

		case 3:  // Column 3 : Fetch and store { BC DE HL SP } to / from immediate address
			{
				auto address = ReadSixteenBitsFromInstructionStream();
				auto &targetRegister = GetReferenceTo_BC_DE_HLIXIY_SP_FromOpcodeBits5and4();
				if( _currentOpcode & 8 )
				{
					// Fetch 16 from address
					targetRegister = DoFetchSixteenBits( address );
				}
				else
				{
					// Store 16 to address
					DoStoreSixteenBits( address, targetRegister );
				}
				// No flags effects.
			}
			break;

		case 4:  // Column 4:  NEG  (44 is official, 54, 64, 74, 4C, 5C, 6C, 7C unofficial NEGs).
			{
				auto currentAccumulatorValue = Accumulator();
				SetAccumulator( 0 );
				AccumulatorSub( currentAccumulatorValue );
				// Flags set per subtract.
			}
			break;

		case 5:  // Column 5
			switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) ) // TODO: dispatch on shifted mask
			{
			case 0: // 0x45:  // RETN   (official)
			case 2: // 0x55:  // RETN   (unofficial)
			case 4: // 0x65:  // RETN   (unofficial)
			case 6: // 0x75:  // RETN   (unofficial)
			case 1: // 0x4D:  // RETI   // TODO: Separate out when interrupts implemented properly.  (Lynx doesn't need yet)
			case 3: // 0x5D:  // RETI   (unofficial)
			case 5: // 0x6D:  // RETI   (unofficial)
			case 7: // 0x7D:  // RETI   (unofficial)
				// TODO: Sort out when interrupts needed by the Lynx project.
				_interruptsEnabled = true;
				DoSubroutineReturn();
				break;
			}

		case 6:  // Column 6
			// No flags affected here.
			switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) )
			{
			case 0: // 0x46:    // (official)
			case 1: // 0x4E:    // (unofficial)
			case 4: // 0x66:    // (unofficial)  (world of spectrum says so!)
			case 5: // 0x6E:    // (unofficial)  (world of spectrum says so!)
				_interruptMode = 0;  break; // IM 0

			case 2: // 0x56:    // (official)
			case 6: // 0x76:    // (unofficial)
				_interruptMode = 1;  break; // IM 1

			case 3: // 0x5E:    // (official)
			case 7: // 0x7E:    // (unofficial)
				_interruptMode = 2;  break; // IM 2
			}

		case 7:  // Column 7
			switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) )
			{
			case 0: // 0x47: // LD I,A
				_I = Accumulator();
				break;

			case 1: // 0x4F: // LD R,A
				_R = Accumulator();
				break;

			case 2: // 0x57: // LD A,I
				SetAccumulatorFromIOrR( _I );
				break;

			case 3: // 0x5F: // LD A,R
				SetAccumulatorFromIOrR( (uint8_t)(_R - _remainingCycles / 4) );  // approximate the number of instructions.
				break;

			case 4: // 0x67: // RRD
				{
					auto const valueRead = GuestRead( _HL );
					DoRLDOrRRD( valueRead & 0x0F,  Accumulator() << 4,  valueRead >> 4 );
				}
				break;

			case 5: // 0x6F: // RLD
				{
					auto const valueRead = GuestRead( _HL );
					DoRLDOrRRD( valueRead >> 4,  valueRead << 4,  Accumulator() & 0x0F );
				}
				break;

			case 6: // 0x77
			case 7: // 0x7F
				UndefinedInstruction();
			}
		}
	}




	void Z80::ExecuteED_Quarter2()
	{
		// This quarter only has the block operation set.

		switch( _currentOpcode )
		{
		case 0xA0: // LDI
		case 0xB0: BlockCopy(+1); break; // LDIR

		case 0xA8: // LDD
		case 0xB8: BlockCopy(-1); break; // LDDR 

		case 0xA1: // CPI
		case 0xB1: BlockCompare(+1); break; // CPIR

		case 0xA9: // CPD
		case 0xB9: BlockCompare(-1); break; // CPDR

		case 0xA2: // INI
		case 0xB2: BlockReadIoSpace(+1); break; // INIR

		case 0xAA: // IND
		case 0xBA: BlockReadIoSpace(-1); break; // INDR

		case 0xA3: // OUTI
		case 0xB3: BlockWriteIoSpace(+1); break; // OTIR

		case 0xAB: // OUTD
		case 0xBB: BlockWriteIoSpace(-1); break; // OTDR

		default:
			UndefinedInstruction();
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    ED instruction implementation
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::DoRLDOrRRD( int32_t value1, int32_t value2, int32_t value3 )
	{
		// Core handler for RLD / RRD.
		auto const resultValue = (Accumulator() & 0xF0) | value1;
		SetAccumulator( resultValue );
		SetFlags( ZeroSignAndParity8(resultValue) | CurrentCarry() );
		GuestWrite( _HL, value2 | value3 );
	}




	INLINE_FUNCTION   void Z80::SetAccumulatorFromIOrR( uint8_t value )
	{
		// TODO: check setting of PV when doing proper interrupt model
		SetAccumulator( value );
		SetFlags( CurrentCarry() | CalcZeroAndSignFlags(Accumulator()) | (_interruptsEnabled ? Z80Flags::PV : 0) );
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    Z80 Block operations
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

			// The flag setting here is very interesting.
			// World Of Spectrum has all sorts for this!
			// http://clrhome.org/table/ has most flags undefined, or even unaffected.
			// http://www.z80.info/z80sflag.htm has P set based on BC.
			// While this Z80 emulator is not "for the Lynx" as such, the Lynx doesn't use these instructions.
			// The IN/OUT instructions are useless on any machine that decodes 16-bits of I/O space, as far as I can see,
			// as they put the countdown-counter as the high byte of the address!



	INLINE_FUNCTION  uint8_t IsRepeated( uint8_t opcode ) 
	{
		return opcode & 0x10;
	}



	void Z80::BlockCopy( int16_t directionDelta )
	{
		// LDIR / LDDR / LDI / LDD
	
		auto data = GuestRead( _HL );
		GuestWrite( _DE, data );
		_HL += directionDelta;
		_DE += directionDelta;
		Countdown16();

		ClearFlagsThenMerge(
			Z80Flags::NF | Z80Flags::HF | Z80Flags::PV,
			(_BC ? Z80Flags::PV : 0) );
		
		if( IsRepeated(_currentOpcode) )
		{
			// LDIR / LDDR
			if( _BC != 0 )
			{
				Spend(5);
				JumpRelative(-2);
			}
		}
	}



	void Z80::BlockCompare( int16_t directionDelta )
	{
		// CPIR / CPDR / CPI / CPD
	
		auto data = GuestRead( _HL );
		_HL += directionDelta;
		auto result = Accumulator() - data;
		Countdown16();
	
		SetFlags(
			Z80Flags::NF 
			| CurrentCarry()
			| CalcZeroAndSignFlags(result)
			| ((Accumulator() ^ data ^ result) & Z80Flags::HF)
			| (_BC ? Z80Flags::PV : 0) );

		if( IsRepeated(_currentOpcode) )
		{
			// CPIR / CPDR
			if( ! (_BC == 0 || result == 0) )
			{
				Spend(5);
				JumpRelative(-2);
			}
		}
	}



	void Z80::BlockReadIoSpace( int16_t directionDelta )
	{
		// INI / IND / INIR / INDR

		auto valueRead = GuestReadIOSpace( _BC );
		GuestWrite( _HL, valueRead );
		_HL += directionDelta;
		Countdown8();
		SetFlags( (HiByte(_BC) ? 0 : Z80Flags::ZF) | Z80Flags::NF );

		if( IsRepeated(_currentOpcode) )
		{
			// INIR / INDR
			if( HiByte(_BC) )
			{
				Spend(5);
				JumpRelative(-2);
			}
		}
	}



	void Z80::BlockWriteIoSpace( int16_t directionDelta )
	{
		auto valueRead = GuestRead( _HL );
		GuestWriteIOSpace( _BC, valueRead );
		_HL += directionDelta;
		Countdown8();
		SetFlags( (HiByte(_BC) ? 0 : Z80Flags::ZF) | Z80Flags::NF );

		if( IsRepeated(_currentOpcode) )
		{
			// OTIR / OTDR
			if( HiByte(_BC) )
			{
				Spend(5);
				JumpRelative(-2);
			}
		}
	}



} // end namespace

