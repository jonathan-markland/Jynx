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

#include <algorithm>
#include "JynxZ80.h"
#include "JynxZ80Timings.h"
#include "JynxZ80Shared.h"

namespace JynxZ80
{



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    MAIN SET -- DISPATCHER
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteOpcodeMainSet()
	{
		// Called to handle the main set, and also the DD and FD prefixed main set.
		// Does NOT handle the DDCB or FDCB -- they have separate handling.
		// - The opcode is already in _currentOpcode!
		// Called from three places, so should NOT handle cycle count reduction here.

		switch( _currentOpcode >> 6 )
		{
		case 0:  // Opcodes: 00..3F
			switch( _currentOpcode & 7 )
			{
			case 0: MainSet_Quarter0_Column0(); break;
			case 1: MainSet_Quarter0_Column1(); break;
			case 2: MainSet_Quarter0_Column2(); break;
			case 3: MainSet_Quarter0_Column3(); break;
			case 4: MainSet_Quarter0_Column4(); break;
			case 5: MainSet_Quarter0_Column5(); break;
			case 6: MainSet_Quarter0_Column6(); break;
			case 7: MainSet_Quarter0_Column7(); break;
			}
			break;

		case 1:  // Opcodes: 40..7F
			MainSet_Quarter1(); 
			break;

		case 2:  // Opcodes: 80..BF
			MainSet_Quarter2(); 
			break;

		case 3:  // Opcodes: C0..FF
			switch( _currentOpcode & 7 )
			{
			case 0: MainSet_Quarter3_Column0(); break;
			case 1: MainSet_Quarter3_Column1(); break;
			case 2: MainSet_Quarter3_Column2(); break;
			case 3: MainSet_Quarter3_Column3(); break;
			case 4: MainSet_Quarter3_Column4(); break;
			case 5: MainSet_Quarter3_Column5(); break;
			case 6: MainSet_Quarter3_Column6(); break;
			case 7: MainSet_Quarter3_Column7(); break;
			}
			break;
		}
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    DD
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteOpcodeDD()
	{
		PrimaryOpcodeFetch();
		Spend( Z80DDFDMainTimings[_currentOpcode] );

		// Filter for the things that the main set isn't going to handle:

		if( _currentOpcode == 0xDD || _currentOpcode == 0xFD )
		{
			// Redundant prefix.  But: Don't treat this as an instruction, and check interrupts after this!
			_deferInterruptCheck = true;
			JumpRelative(-1);
			return;
		}
	
		if( _currentOpcode == 0xCB )  // this is needed because I implemented CB and DDCB/FDCB first, and it all handles itself in a different manner to prefixing the Main set.
		{
			ExecuteOpcodeDDCB();
			return;
		}

		if( _currentOpcode == 0xED )  // Needed because DD or FD prefixing NEVER applied to ED opcodes -- NO RE-DIRECTIONS!
		{
			ExecuteOpcodeED();
			return;
		}

		// Re-direct the H / L / HL accesses to IX:

		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[2] = &_IX;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[2] = &_IX;
		_addressesOf8BitRegisters[4] = &HiByte(_IX); // What was H -> IXH
		_addressesOf8BitRegisters[5] = &LoByte(_IX); // What was L -> IXL

		// (The main set's (HL) forms become (IX+dd), and the above almost totally works, apart 
		// for the need to fetch and add the offset using a temporary store -- but the (HL) handlers
		// in the Main set code know about this).  Other instructions in the Main Set also know
		// to use the re-direction instead of just assuming direct access to HL.

		ExecuteOpcodeMainSet();

		// Restore re-directions:

		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[2] = &_HL;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[2] = &_HL;
		_addressesOf8BitRegisters[4] = &HiByte(_HL);
		_addressesOf8BitRegisters[5] = &LoByte(_HL);
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//    FD
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::ExecuteOpcodeFD()
	{
		PrimaryOpcodeFetch();
		Spend( Z80DDFDMainTimings[_currentOpcode] );

		// Filter for the things that the main set isn't going to handle:

		if( _currentOpcode == 0xDD || _currentOpcode == 0xFD )  // Pointless prefix chaining.
		{
			// Redundant prefix.  But: Don't treat this as an instruction, and check interrupts after this!
			_deferInterruptCheck = true;
			JumpRelative(-1);
			return;
		}
	
		if( _currentOpcode == 0xCB )  // this is needed because I implemented CB and DDCB/FDCB first, and it all handles itself in a different manner to prefixing the Main set.
		{
			ExecuteOpcodeFDCB();
			return;
		}

		if( _currentOpcode == 0xED )  // Needed because DD or FD prefixing NEVER applied to ED opcodes -- NO RE-DIRECTIONS!
		{
			ExecuteOpcodeED();
			return;
		}

		// Re-direct the H / L / HL accesses to IY:

		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[2] = &_IY;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[2] = &_IY;
		_addressesOf8BitRegisters[4] = &HiByte(_IY); // What was H -> IYH
		_addressesOf8BitRegisters[5] = &LoByte(_IY); // What was L -> IYL

		// (The main set's (HL) forms become (IX+dd), and the above almost totally works, apart 
		// for the need to fetch and add the offset using a temporary store -- but the (HL) handlers
		// in the Main set code know about this).  Other instructions in the Main Set also know
		// to use the re-direction instead of just assuming direct access to HL.

		ExecuteOpcodeMainSet();

		// Restore re-directions:

		_addressesOf16BitRegisterPairs_BC_DE_HL_AF[2] = &_HL;
		_addressesOf16BitRegisterPairs_BC_DE_HL_SP[2] = &_HL;
		_addressesOf8BitRegisters[4] = &HiByte(_HL);
		_addressesOf8BitRegisters[5] = &LoByte(_HL);
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//      MAIN SET IMPLEMENTATION
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column0()
	{
		if( _currentOpcode == 0x00 )
		{
			// NOP
		}
		else if( _currentOpcode == 0x08 )
		{
			// EX AF, AF'
			std::swap( _AF, _alternateAF );
		}
		else
		{
			// DJNZ, JR, and the 4 conditional JRs

			uint8_t  branchTaken;

			if( _currentOpcode == 0x10 )
			{
				// DJNZ
				branchTaken = --HiByte(_BC);
			}
			else if( _currentOpcode == 0x18 )
			{
				// JR
				goto do_branch_taken;
			}
			else
			{
				// The 4 conditional JRs
 				auto index = (_currentOpcode >> 3) & 3;  // Intentionally only using the first four, hence "& 3".
				branchTaken = IsConditionSatisfied( ConditionalExecutionMasksAndExpectations[ index ] );
			}

			if( branchTaken )
			{
			do_branch_taken:
				Spend(5);
				auto branchOffset = (int16_t) ((int8_t) CodeStreamFetch());
				JumpRelative( branchOffset );
				OnAboutToBranch();
			}
			else
			{
				++_programCounter; // branch not taken
			}
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column1()
	{
		auto &operandRegister = GetReferenceTo_BC_DE_HLIXIY_SP_FromOpcodeBits5and4();
		if( _currentOpcode & 8 )
		{
			// ADD { HL / IX / IY }, { BC / DE / HL / IX / IY / SP }
			DoSixteenBitADD( GetReferenceTo_HL_IX_IY(), operandRegister );
		}
		else
		{
			// Load 16-bit constant into  BC / DE / HL / IX / IY / SP
			DoSixteenBitLoadConstant( _programCounter, operandRegister );
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column2()
	{
		uint16_t address;

		if( _currentOpcode & 0x20 )
		{
			address = ReadSixteenBitsFromInstructionStream();

			if( _currentOpcode == 0x22 )
			{
				// LD (nnnn),HL/IX/IY
				auto &targetRegister = GetReferenceTo_HL_IX_IY();
				DoStoreSixteenBits( address, targetRegister );
				return; // early out
			}
			else if( _currentOpcode == 0x2A )
			{
				// LD HL/IX/IY,(nnnn)
				auto &targetRegister = GetReferenceTo_HL_IX_IY();
				targetRegister = DoFetchSixteenBits( address );
				return; // early out
			}
		}
		else
		{
			// The address is either BC or DE
			address = (_currentOpcode & 0x10) ? _DE : _BC;
		}

		// ** Only the 6 accumulator instructions get here **

		// LD (nnnn),A
		// LD A,(nnnn)
		// LD (BC),A
		// LD A,(BC)
		// LD (DE),A
		// LD A,(DE)

		if( _currentOpcode & 8 )
		{
			SetAccumulator( GuestRead( address ) ); // fetch A
		}
		else
		{
			GuestWrite( address, Accumulator() );  // store A
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column3()
	{
		auto &targetRegister = GetReferenceTo_BC_DE_HLIXIY_SP_FromOpcodeBits5and4();
		if( _currentOpcode & 8 )
		{
			--targetRegister;  // 0B 1B 2B 3B: DEC BC DE HL/IX/IY SP
		}
		else
		{
			++targetRegister;  // 03 13 23 33: INC BC DE HL/IX/IY SP
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION uint8_t Calculate_HF_PV_PostIncrementAndPreDecrement( uint8_t value )
	{
		return (value == 0x80 ? Z80Flags::PV : 0) | ((value & 0x0F) ? 0 : Z80Flags::HF);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column4()
	{
		// 04 14 24 34 0C 1C 2C 3C
		// INC 8-bit registers 
		// INC (HL) / (IX+dd) / (IY+dd)

		uint16_t targetAddress;

		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			targetAddress = GetIndirectTargetAddress();
			_hiddenRegisterSix = GuestRead( targetAddress );
		}
				
		auto &reg = GetReferenceToReg8_FromBits5to3();
		++reg;
		SetFlags(
			CurrentCarry()
			| CalcZeroAndSignFlags(reg)
			| Calculate_HF_PV_PostIncrementAndPreDecrement(reg) );

		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			GuestWrite( targetAddress, _hiddenRegisterSix );
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column5()
	{
		// 05 15 25 35 0D 1D 2D 3D
		// DEC 8-bit register
		// DEC (HL) / (IX+dd) / (IY+dd)

		uint16_t targetAddress;

		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			targetAddress = GetIndirectTargetAddress();
			_hiddenRegisterSix = GuestRead( targetAddress );
		}

		auto &reg = GetReferenceToReg8_FromBits5to3();
		auto PVHF = Calculate_HF_PV_PostIncrementAndPreDecrement(reg);
		--reg;
		SetFlags( CurrentCarry() | CalcZeroAndSignFlags(reg) | PVHF | Z80Flags::NF );

		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			GuestWrite( targetAddress, _hiddenRegisterSix );
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column6()
	{
		// Load constant into 8-bit register, or 
		// Store constant to (HL) / (IX+dd) / (IY+dd).
	
		uint16_t targetAddress;
		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			targetAddress =  GetIndirectTargetAddress();
		}
	
		GetReferenceToReg8_FromBits5to3() = CodeStreamFetch();
	
		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			GuestWrite( targetAddress, _hiddenRegisterSix ); // Store the temporary register back to memory.
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter0_Column7()
	{
		// RLCA RRCA RLA RRA DAA CPL SCF CCF
		switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) )
		{
			case 0: // RLCA:
				{
					uint8_t carryOut = Accumulator() >> 7;
					SetAccumulator( (Accumulator() << 1) | carryOut );
					ClearFlagsThenMerge( Z80Flags::CF | Z80Flags::NF | Z80Flags::HF, carryOut );
				}
				break;

			case 1: // RRCA:
				{
					uint8_t carryOut = Accumulator() & 1;
					SetAccumulator( (Accumulator() >> 1) | (carryOut << 7) );
					ClearFlagsThenMerge( Z80Flags::CF | Z80Flags::NF | Z80Flags::HF, carryOut );
				}
				break;

			case 2: // RLA:
				{
					uint8_t carryOut = Accumulator() >> 7;
					SetAccumulator( (Accumulator() << 1) | CurrentCarry() );
					ClearFlagsThenMerge( Z80Flags::CF | Z80Flags::NF | Z80Flags::HF, carryOut );
				}
				break;

			case 3: // RRA:
				{
					uint8_t carryOut = Accumulator() & 1;
					SetAccumulator( (Accumulator() >> 1) | (Flags() << 7) );
					ClearFlagsThenMerge( Z80Flags::CF | Z80Flags::NF | Z80Flags::HF, carryOut );
				}
				break;

			case 4: // DAA
				DoDAA();
				break;

			case 5: // CPL
				SetAccumulator( ~Accumulator() );
				SetFlags( Flags() | Z80Flags::NF | Z80Flags::HF );
				break;

			case 6: // SCF
				ClearFlagsThenMerge( Z80Flags::NF | Z80Flags::HF, Z80Flags::CF );
				break;

			case 7: // CCF
				// Condition Bits Affected: S is not affected Z is not affected H, 
				// previous carry is copied 
				// P/V is not affected 
				// N is reset 
				// C is set if CY was 0 before operation; reset otherwise
				{
					auto oldFlags  = Flags();
					auto flags  = oldFlags ^ Z80Flags::CF;   // do the CCF
					auto masked = flags & (Z80Flags::SF | Z80Flags::ZF | Z80Flags::CF | Z80Flags::PV);  // keep these, clear others
					auto halfCarry = (oldFlags << 4) & Z80Flags::HF; // HF takes the old CF value
					SetFlags( masked | halfCarry );
				}
				break;
		};
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter1()
	{
		// 0x40 .. 0x7F

		// REG to REG copy
		// Fetch from (HL)
		// Store to (HL)
		// HALT

		if( GetRowMask( _currentOpcode ) == RowMasks::Row6 )
		{
			if( _currentOpcode == 0x76 )
			{
				// HALT
				// The real Z80 re-fetches the HALT instruction.  We do not.
				_cpuHalted = true;
				_deferInterruptCheck = false; // This should be false, but let's ensure it, otherwise it looks like the main loop COULD execute 1 instruction in the HALT state!!
			}
			else
			{
				// Store 8-bit register to (HL) / (IX+dd) / (IY+dd)
				auto targetAddress = GetIndirectTargetAddress();
				GuestWrite( targetAddress, GetReferenceToReg8_NoRedirection_FromBits2to0OfOpcode() );
			}
		}
		else
		{
			if( GetColumnNumber(_currentOpcode) == 6 )
			{
				// Fetch from (HL) / (IX+dd) / (IY+dd) case.  Fetch into the Z80 temporary register:
				auto targetAddress = GetIndirectTargetAddress();
				GetReferenceToReg8_NoRedirection_FromBits5to3() = GuestRead( targetAddress );  // Indirect form NOT re-directed
			}
			else
			{
				// (The majority) Register to register copy.
				GetReferenceToReg8_FromBits5to3() = GetReferenceToReg8_FromBits2to0OfOpcode();
			}
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter2()
	{
		// 0x80 .. 0xBF
		// ADD ADC SUB SBC AND XOR OR CP

		if( GetColumnNumber(_currentOpcode) == 6 )
		{
			// This the (HL) / (IX+dd) / (IY+dd) form.  Pre-load the Z80's temporary register with the value at the address:
			auto targetAddress = GetIndirectTargetAddress();
			_hiddenRegisterSix = GuestRead( targetAddress );
		}

		DoArithLogicCompareWithAccumulatorAndValue( GetReferenceToReg8_FromBits2to0OfOpcode() );
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column0()
	{
		if( IsConditionSatisfiedBasedOnOpcode() )
		{
			DoSubroutineReturn();  // Conditional RET
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column1()
	{
		if( (_currentOpcode & 0x0F) == 0x01 )
		{
			// POP BC DE HL/IX/IY AF
			DoPopSixteenBitsIntoRegister( GetReferenceToRegisterForPushPopGroupFromOpcode() );
		}
		else if( _currentOpcode == 0xC9 )
		{
			DoSubroutineReturn();  // RET
		}
		else if( _currentOpcode == 0xD9 )
		{
			// EXX
			std::swap( _BC, _alternateBC );
			std::swap( _DE, _alternateDE );
			std::swap( _HL, _alternateHL );  // always swaps HL, never IX/IY !
		}
		else if( _currentOpcode == 0xE9 )
		{
			// JP HL / IX / IY
			JumpTo( GetReferenceTo_HL_IX_IY() );
		}
		else if( _currentOpcode == 0xF9 )
		{
			// LD SP,HL / IX / IY
			_stackPointer = GetReferenceTo_HL_IX_IY();
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column2()
	{
		if( IsConditionSatisfiedBasedOnOpcode() )
		{
			JumpTo( ReadSixteenBitsFromInstructionStream() );   // Conditional JP
		}
		else
		{
			JumpRelative(2);  // Not taken
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column3()
	{
		switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) )
		{
		case 0:  // C3: Unconditional JP
			JumpTo( ReadSixteenBitsFromInstructionStream() );
			break;

		case 1:  // CB  (reminder -- the DD and FD prefixed CB never gets here, as it's handled elsewhere).
			ExecuteOpcodeCB();
			break;
			
		case 2:  // D3: OUTA
			{
				auto tempValue = CodeStreamFetch();
				GuestWriteIOSpace( (_AF & 0xFF00) | tempValue, Accumulator() );
			}
			break;

		case 3:  // DB: INA
			{
				auto tempValue = CodeStreamFetch();
				SetAccumulator( GuestReadIOSpace( (_AF & 0xFF00) | tempValue ) );
			}
			break;
			
		case 4:  // E3: EX HL / IX / IY,(SP)
			{
				auto tempValue = DoFetchSixteenBits( _stackPointer );
				DoStoreSixteenBits( _stackPointer, GetReferenceTo_HL_IX_IY() );
				GetReferenceTo_HL_IX_IY() = tempValue;
			}
			break;

		case 5:  // EB: EX DE,HL   (never re-directed to swap with IX or IY)
			std::swap( _DE, _HL );
			break;

		case 6:  // F3: DI
			_interruptsEnabled = false;
			break;
			
		case 7:  // FB: EI
			_interruptsEnabled = true;
			_deferInterruptCheck = true;
			break;
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column4()
	{
		if( IsConditionSatisfiedBasedOnOpcode() )
		{
			Spend(7);
			DoSubroutineCall( ReadSixteenBitsFromInstructionStream() );  // Conditional CALL
		}
		else
		{
			JumpRelative(2);  // Not taken
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column5()
	{
		switch( GetRowIndexWithinColumnFromOpcode( _currentOpcode ) )
		{
		case 0:  // PUSH BC
		case 2:  // PUSH DE
		case 4:  // PUSH HL / IX / IY
		case 6:  // PUSH AF
			DoPushSixteenBits( GetReferenceToRegisterForPushPopGroupFromOpcode() );
			break;

		case 1:  // CD:  CALL
			DoSubroutineCall( ReadSixteenBitsFromInstructionStream() );
			break;

		case 3:  // DD:  IX REGISTER SELECT PREFIX
			ExecuteOpcodeDD();
			break;

		case 5:  // ED:  PREFIX
			ExecuteOpcodeED();
			break;

		case 7:  // FD:  IY REGISTER SELECT PREFIX
			ExecuteOpcodeFD();
			break;
		}
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column6()
	{
		// ADD(etc) immediate constant to accumulator
		auto immediateConstant = CodeStreamFetch();
		DoArithLogicCompareWithAccumulatorAndValue( immediateConstant );
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::MainSet_Quarter3_Column7()
	{
		// RST
		DoPushSixteenBits( _programCounter );
		JumpTo( _currentOpcode & 0x38 );
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     8-bit arithmetic and logical
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::DestructiveAdd8( uint8_t &theRegister, uint8_t rhsValue, uint8_t carry )
	{
		auto lhsValue = theRegister;
		uint32_t  resultValue = lhsValue + rhsValue + carry;

		SetFlags( 
			CalcZeroAndSignFlags(resultValue) 
			| OverflowFlagAfterAddition8( lhsValue, rhsValue, resultValue )
			| HalfCarryAfterAddOrSubtract8( lhsValue, rhsValue, resultValue )
			| CarryAfterAddOrSubtract8( lhsValue, rhsValue, resultValue ) );

		theRegister = resultValue;
	}



	void Z80::DestructiveSubtract8( uint8_t &theRegister, uint8_t rhsValue, uint8_t carry )
	{
		auto lhsValue = theRegister;
		uint32_t  resultValue = lhsValue - rhsValue - carry;

		SetFlags( 
			CalcZeroAndSignFlags(resultValue) 
			| OverflowFlagAfterSubtraction8( lhsValue, rhsValue, resultValue )
			| HalfCarryAfterAddOrSubtract8( lhsValue, rhsValue, resultValue )
			| CarryAfterAddOrSubtract8( lhsValue, rhsValue, resultValue )
			| Z80Flags::NF );

		theRegister = resultValue;
	}



	void Z80::AccumulatorAdd( uint8_t rhsValue )
	{
		DestructiveAdd8( HiByte(_AF), rhsValue, 0 );
	}



	void Z80::AccumulatorSub( uint8_t rhsValue )
	{
		DestructiveSubtract8( HiByte(_AF), rhsValue, 0 );
	}



	void Z80::AccumulatorAdc( uint8_t rhsValue )
	{
		DestructiveAdd8( HiByte(_AF), rhsValue, CurrentCarry() );
	}



	void Z80::AccumulatorSbc( uint8_t rhsValue )
	{
		DestructiveSubtract8( HiByte(_AF), rhsValue, CurrentCarry() );
	}



	void Z80::AccumulatorCp( uint8_t rhsValue )
	{
		auto temp = Accumulator(); // take copy
		DestructiveSubtract8( temp, rhsValue, 0 );  // bin result
	}



	void Z80::AccumulatorAnd( uint8_t rhsValue )
	{
		uint8_t result = Accumulator() & rhsValue;
		SetAccumulator( result );
		SetFlags( ZeroSignAndParity8(result) | Z80Flags::HF );
	}



	void Z80::AccumulatorOr( uint8_t rhsValue )
	{
		uint8_t result = Accumulator() | rhsValue;
		SetAccumulator( result );
		SetFlags( ZeroSignAndParity8(result) );
	}



	void Z80::AccumulatorXor( uint8_t rhsValue )
	{
		uint8_t result = Accumulator() ^ rhsValue;
		SetAccumulator( result );
		SetFlags( ZeroSignAndParity8(result) );
	}



	void Z80::DoArithLogicCompareWithAccumulatorAndValue( uint8_t rhsValue )
	{
		switch( GetRowIndexWithinColumnFromOpcode(_currentOpcode) )
		{
		case 0: AccumulatorAdd( rhsValue ); break;
		case 1: AccumulatorAdc( rhsValue ); break;
		case 2: AccumulatorSub( rhsValue ); break;
		case 3: AccumulatorSbc( rhsValue ); break;
		case 4: AccumulatorAnd( rhsValue ); break;
		case 5: AccumulatorXor( rhsValue ); break;
		case 6: AccumulatorOr(  rhsValue ); break;
		case 7: AccumulatorCp(  rhsValue ); break;
		};
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     16-bit arithmetic
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	void Z80::DoSixteenBitADD( uint16_t &leftOperand, const uint32_t rightOperand )
	{
		uint32_t  leftCopy = leftOperand;  // optimisation: ensure fetch once only. Take opportunity to zero-extend.
		uint32_t  result32 = leftCopy + rightOperand;  // The desired result, also allowing us to sample carry flag from bit 16.
	
		ClearFlagsThenMerge(
			Z80Flags::HF | Z80Flags::NF | Z80Flags::CF, // flags to clear!
			HalfCarryAfterAddOrSubtract16( leftCopy, rightOperand, result32 ) | CarryFlagFromBit16( result32 ) );
	
		leftOperand = (uint16_t) result32;
	}






	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     DAA
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

			// DAA subtroutine - with thanks:
			// http://www.worldofspectrum.org/faq/reference/z80reference.htm

	INLINE_FUNCTION void Z80::DoDAA()
	{
		auto A = Accumulator();
		auto F = Flags();
		uint8_t correctionFactor = 0x00;
		uint8_t carryOut = 0;

		if( A > 0x99 || F & Z80Flags::CF )
		{
			correctionFactor = 0x60;
			carryOut = Z80Flags::CF;
		}

		if( (A & 0x0F) > 9 || F & Z80Flags::HF )
		{
			correctionFactor |= 0x06;
		}

		auto newAccumulator = (F & Z80Flags::NF)
			? A - correctionFactor
			: A + correctionFactor;

		SetFlags( 
			carryOut 
			| ((A ^ newAccumulator) & Z80Flags::HF)
			| ZeroSignAndParity8(newAccumulator)
			| (F & Z80Flags::NF) );
		
		SetAccumulator( newAccumulator );
	}







	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     Z80 REGISTER SELECTION
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	uint16_t  Z80::GetIndirectTargetAddress()
	{
		// TODO: It would be nice if the (HL) case in the main set didn't have to execute this IF statement!
		//       That would require leaving AddressesOf16BitRegisterPairs_BDEHLAF alone.  We would then
		//       etablish a separate re-direction register to point to the HL slot for the (HL) case, or 
		//       the address of a temporary 16-bit address register that has
		//       been primed with the IX+dd value or IY+dd value, by code elsewhere.

		auto targetRegister = _addressesOf16BitRegisterPairs_BC_DE_HL_AF[2];
		if( targetRegister == &_HL )
		{
			// (HL)
			return _HL;
		}
		else
		{
			// (IX+dd)  or  (IY+dd)
			auto offsetByte = CodeStreamFetch();
			return (*targetRegister) + (int8_t) offsetByte;
		}
	}




	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     16-bit operation assistance
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	uint16_t Z80::ReadSixteenBitsFromInstructionStream()
	{
		auto loByte = CodeStreamFetch();
		auto hiByte = CodeStreamFetch();
		return Make16( hiByte, loByte );
	}


	void Z80::DoSixteenBitLoadConstant( uint16_t &addressRegister, uint16_t &targetRegister )  // static 
	{
		auto loByte = GuestRead( addressRegister++ );
		auto hiByte = GuestRead( addressRegister++ );
		targetRegister = Make16( hiByte, loByte );
	}


	uint16_t Z80::DoFetchSixteenBits( uint16_t address ) // static 
	{
		auto loByte = GuestRead( address );
		auto hiByte = GuestRead( address + 1 );
		return Make16( hiByte, loByte );
	}


	void Z80::DoStoreSixteenBits( uint16_t address, uint16_t dataRegister )  // static 
	{
		GuestWrite( address,     (uint8_t) dataRegister );
		GuestWrite( address + 1, (uint8_t) (dataRegister >> 8) );
	}


	void Z80::DoPushSixteenBits( uint16_t valueToPush )
	{
		GuestWrite( --_stackPointer, (uint8_t) (valueToPush >> 8) );
		GuestWrite( --_stackPointer, (uint8_t) valueToPush );
	}


	void Z80::DoPopSixteenBitsIntoRegister( uint16_t &targetRegister )
	{
		auto loByte = GuestRead( _stackPointer++ );
		auto hiByte = GuestRead( _stackPointer++ );
		targetRegister = Make16( hiByte, loByte );
	}





	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     Subroutine call and return assistance
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	INLINE_FUNCTION void Z80::DoSubroutineCall( uint16_t address )
	{
		DoPushSixteenBits( _programCounter );
		JumpTo( address );
	}



	INLINE_FUNCTION void Z80::DoSubroutineReturn()
	{
		Spend(6);
		OnAboutToReturn();
		DoPopSixteenBitsIntoRegister( _programCounter );
	}



} // end namespace




