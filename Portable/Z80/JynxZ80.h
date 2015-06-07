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

// - See Z80ExternalImplementation.h for user tasks.
//
// - This was made for the Camputers Lynx emulator, but does NOT have Lynx-specific code!
//
// - The Lynx had almost no use for interrupts anyway, and no use at all for them in the
//   parts of the Lynx that I emulate!  So, the implementation of interrupts is
//   work-in-progress.  I will need to substitute into another oper-source emulator to
//   properly develop interrupts.  Sorry!
//
// - This is restricted to little-endian machines at present.  TheLoByte() and HiByte()
//   functions are responsible for this restriction.  I cannot test big-endian.
//

#pragma once

#include <stdint.h>
#include "JynxZ80Declarations.h"
#include "JynxZ80ExternalImplementation.h"



// I was experimenting with these on the MSVC compiler.
// Other than this #define I intend no system specific code in this source listing.
// (This should be the case already).
#ifdef _MSC_VER
#define INLINE_FUNCTION  __forceinline
#else
#define INLINE_FUNCTION  inline
#endif

namespace JynxZ80
{



	class Z80: public Z80ImplementationBaseClass, private Z80SerialisableState
	{
	public:

		// Main Z80 emulator.

		static void InitialiseGlobalTables();
			// Must be called ONCE before any instances created.
			// (Exposing this side-steps any multi-threading issues).

		Z80();

		void Reset();
		void RunForTimeslice();

		//
		// Timeslice support.
		// You can change the timeslice length during run, but that will
		// only take effect on the expiration of the current remainder.
		//

		int32_t GetRemainingCycles() const              { return _remainingCycles; }
		int32_t GetTimesliceLength() const              { return _timesliceLength; }
		void SetTimesliceLength( int32_t numCycles );

		//
		// Serialisation support.
		// This does set / get everything pertinent, but this interface
		// dissuades general fiddling.
		//

		const Z80SerialisableState &GetSerialisableVariables() const;
		void SetSerialisableVariables( const Z80SerialisableState & );

	private: // (No serialisation required).

		enum { _interruptSignalled = false };  // TODO: Until interrupt support perhaps!

		uint8_t _currentOpcode;
		uint8_t _hiddenRegisterSix;   // Unifies the handling of temporaries fetched from (HL) / (IX+nn) / (IY+nn)

		uint8_t   *_addressesOf8BitRegisters[8];                   // Access to register file, supporting re-direction H->IXH, L->IXL, or H->IYH, L->IYL, if prefixed.
		uint8_t   *_addressesOf8BitRegisters_NoRedirection[8];     // Access to register file, H and L are never re-directed to IXH/IXL IYH/IYL.
		uint16_t  *_addressesOf16BitRegisterPairs_BC_DE_HL_SP[4];  // Access to register file as register pairs.  Indexed based on bits from the opcode.
		uint16_t  *_addressesOf16BitRegisterPairs_BC_DE_HL_AF[4];  // Access to register file as register pairs.  Indexed based on bits from the opcode.

		static uint8_t _signParityAndZeroTable[256];  // Calculated on construction.

	private:

		static void CalculateSignParityAndZeroTable();

		//
		// Execution of opcodes
		//

		void ExecuteOpcodeMainSet();

		void MainSet_Quarter0_Column0();
		void MainSet_Quarter0_Column1();
		void MainSet_Quarter0_Column2();
		void MainSet_Quarter0_Column3();
		void MainSet_Quarter0_Column4();
		void MainSet_Quarter0_Column5();
		void MainSet_Quarter0_Column6();
		void MainSet_Quarter0_Column7();

		void MainSet_Quarter1();
		void MainSet_Quarter2();

		void MainSet_Quarter3_Column0();
		void MainSet_Quarter3_Column1();
		void MainSet_Quarter3_Column2();
		void MainSet_Quarter3_Column3();
		void MainSet_Quarter3_Column4();
		void MainSet_Quarter3_Column5();
		void MainSet_Quarter3_Column6();
		void MainSet_Quarter3_Column7();

		void ExecuteOpcodeED();
		void ExecuteED_Quarter1();
		void ExecuteED_Quarter2();
		void BlockCopy( int16_t directionDelta );
		void BlockCompare( int16_t directionDelta );
		void BlockReadIoSpace( int16_t directionDelta );
		void BlockWriteIoSpace( int16_t directionDelta );

		void ExecuteOpcodeCB();
		void ExecuteCB_OnSpecificSlot( uint8_t &targetRegister );

		void ExecuteOpcodeDD();
		void ExecuteOpcodeFD();

		void ExecuteOpcodeDDCB();
		void ExecuteOpcodeFDCB();
		void Execute_DDCB_FDCB( uint16_t &regIXIY );

		void UndefinedInstruction();

		//
		// Addressing of the register file.
		//
		// - Notice I infer the existence of a temporary register (encoding 110) that
		//   holds the value for the (HL) cases of otherwise regular-register instructions.
		//

		uint16_t &GetReferenceTo_HL_IX_IY();
		uint16_t &GetReferenceTo_BC_DE_HLIXIY_SP_FromOpcodeBits5and4();
		uint16_t &GetReferenceToRegisterForPushPopGroupFromOpcode();
		uint8_t  &GetReferenceToReg8_FromBits2to0OfOpcode();
		uint8_t  &GetReferenceToReg8_NoRedirection_FromBits2to0OfOpcode();
		uint8_t  &GetReferenceToReg8_FromBits5to3();
		uint8_t  &GetReferenceToReg8_NoRedirection_FromBits5to3();

		uint16_t  GetIndirectTargetAddress();

		//
		// Condition testing, avoiding C++ "bool".
		//

		uint8_t  IsConditionSatisfied( const ConditionTestingData &conditionTestingData );
		uint8_t  IsConditionSatisfiedBasedOnOpcode();

		//
		// Support subroutines for instruction execution.
		//

		INLINE_FUNCTION   void     PrimaryOpcodeFetch()                  { _currentOpcode = CodeStreamFetch(); }
		INLINE_FUNCTION   uint8_t  CodeStreamFetch()                     { return GuestRead( _programCounter++ ); }

		INLINE_FUNCTION   void     Spend( int32_t z80Cycles )            { _remainingCycles -= z80Cycles; }

		INLINE_FUNCTION   void     JumpTo( uint16_t address )            { _programCounter = address; OnAboutToBranch(); }
		INLINE_FUNCTION   void     JumpRelative( int16_t displacement )  { _programCounter += displacement;  }

		INLINE_FUNCTION   void     Countdown8()                          { --HiByte(_BC); }
		INLINE_FUNCTION   void     Countdown16()                         { --_BC; }

		uint16_t ReadSixteenBitsFromInstructionStream();
		static void DoSixteenBitLoadConstant( uint16_t &PC, uint16_t &targetRegister );
		static uint16_t DoFetchSixteenBits( uint16_t address );
		static void DoStoreSixteenBits( uint16_t address, uint16_t dataRegister );

		void DoPushSixteenBits( uint16_t valueToPush );
		void DoPopSixteenBitsIntoRegister( uint16_t &targetRegister );
		void DoSubroutineCall( uint16_t address );
		void DoSubroutineReturn();
		void DoArithLogicCompareWithAccumulatorAndValue( uint8_t rhsValue );
		void DoSixteenBitADD( uint16_t &leftOperand, const uint32_t rightOperand );
		void DoDAA();
		void DoRLDOrRRD( int32_t value1, int32_t value2, int32_t value3 );

		void AccumulatorAdd( uint8_t rhsValue );
		void AccumulatorAdc( uint8_t rhsValue );
		void AccumulatorSub( uint8_t rhsValue );
		void AccumulatorSbc( uint8_t rhsValue );
		void AccumulatorAnd( uint8_t rhsValue );
		void AccumulatorOr(  uint8_t rhsValue );
		void AccumulatorXor( uint8_t rhsValue );
		void AccumulatorCp(  uint8_t rhsValue );

		void DestructiveAdd8(      uint8_t &theRegister, uint8_t rhsValue, uint8_t carry );
		void DestructiveSubtract8( uint8_t &theRegister, uint8_t rhsValue, uint8_t carry );

		//
		// Flags reading
		//

		INLINE_FUNCTION   uint8_t  Flags()                               { return LoByte(_AF); }
		INLINE_FUNCTION   uint8_t  CurrentCarry()                        { return Flags() & Z80Flags::CF; }
		INLINE_FUNCTION   uint8_t  HalfCarryBasedOnCurrentCarry()        { return CurrentCarry() << 4; }

		//
		// Flags writing
		//

		INLINE_FUNCTION   void SetFlags( uint8_t value )                                 { LoByte(_AF) = value; }
		INLINE_FUNCTION   void ClearFlagsThenMerge( uint8_t toClear, uint8_t toMerge )   { SetFlags( (Flags() & (~toClear)) | toMerge ); }

		//
		// Accumulator read and write
		//

		INLINE_FUNCTION   uint8_t Accumulator()                                          { return HiByte(_AF); }
		INLINE_FUNCTION   void SetAccumulator( uint8_t value )                           { HiByte(_AF) = value; }
		INLINE_FUNCTION   void SetAccumulatorFromIOrR( uint8_t value );

		//
		// A lot of instruction need the Zero, Sign and Parity flags setting based on the value.
		//

		INLINE_FUNCTION   static uint8_t ZeroSignAndParity8( uint8_t value );

	};

} // end namespace
