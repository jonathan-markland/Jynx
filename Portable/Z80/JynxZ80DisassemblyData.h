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
// Z80 Disassembly Tables
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Special characters in the 'DisassembledForm' text are as follows:
//
//    *     8-bit immediate value.
//    #    16-bit immediate value.
//    !    16-bit immediate value used for an address.
//    $     8-bit relative branch displacement.
//    %     8-bit signed offset for indirect addressing.
//    .     Indicates blank line will be inserted AFTER this instruction.  Only appears as the very first character.  
//



//
// Main instruction Set
//


#pragma once


namespace JynxZ80
{

	namespace Z80InstructionClass
	{
		// TODO:  I might actually get around to using this information in the disassembler!

		enum Enum
		{
			Defin,    // defined
			Unoff,    // unofficial definition
			Undef     // undefined
		};
	}



	struct Disasm
	{
		// The record format of the following static data tables.

		const Z80InstructionClass::Enum  InstructionClass;
		const char *DisassembledForm;
	};




	Disasm  g_DisassemblyMainTable[] = {

	// 0x00
	{ Z80InstructionClass::Defin, "nop" },
	{ Z80InstructionClass::Defin, "ld bc,#" },
	{ Z80InstructionClass::Defin, "ld (bc),a" },
	{ Z80InstructionClass::Defin, "inc bc" },
	{ Z80InstructionClass::Defin, "inc b" },
	{ Z80InstructionClass::Defin, "dec b" },
	{ Z80InstructionClass::Defin, "ld b,*" },
	{ Z80InstructionClass::Defin, "rlca" },
	{ Z80InstructionClass::Defin, "ex af,af'" },
	{ Z80InstructionClass::Defin, "add hl,bc" },
	{ Z80InstructionClass::Defin, "ld a,(bc)" },
	{ Z80InstructionClass::Defin, "dec bc" },
	{ Z80InstructionClass::Defin, "inc c" },
	{ Z80InstructionClass::Defin, "dec c" },
	{ Z80InstructionClass::Defin, "ld c,*" },
	{ Z80InstructionClass::Defin, "rrca" },


	// 0x10
	{ Z80InstructionClass::Defin, "djnz $" },
	{ Z80InstructionClass::Defin, "ld de,#" },
	{ Z80InstructionClass::Defin, "ld (de),a" },
	{ Z80InstructionClass::Defin, "inc de" },
	{ Z80InstructionClass::Defin, "inc d" },
	{ Z80InstructionClass::Defin, "dec d" },
	{ Z80InstructionClass::Defin, "ld d,*" },
	{ Z80InstructionClass::Defin, "rla" },
	{ Z80InstructionClass::Defin, ".jr $" },
	{ Z80InstructionClass::Defin, "add hl,de" },
	{ Z80InstructionClass::Defin, "ld a,(de)" },
	{ Z80InstructionClass::Defin, "dec de" },
	{ Z80InstructionClass::Defin, "inc e" },
	{ Z80InstructionClass::Defin, "dec e" },
	{ Z80InstructionClass::Defin, "ld e,*" },
	{ Z80InstructionClass::Defin, "rra" },


	// 0x20
	{ Z80InstructionClass::Defin, "jr nz,$" },
	{ Z80InstructionClass::Defin, "ld hl,#" },
	{ Z80InstructionClass::Defin, "ld (!),hl" },
	{ Z80InstructionClass::Defin, "inc hl" },
	{ Z80InstructionClass::Defin, "inc h" },
	{ Z80InstructionClass::Defin, "dec h" },
	{ Z80InstructionClass::Defin, "ld h,*" },
	{ Z80InstructionClass::Defin, "daa" },
	{ Z80InstructionClass::Defin, "jr z,$" },
	{ Z80InstructionClass::Defin, "add hl,hl" },
	{ Z80InstructionClass::Defin, "ld hl,(!)" },
	{ Z80InstructionClass::Defin, "dec hl" },
	{ Z80InstructionClass::Defin, "inc l" },
	{ Z80InstructionClass::Defin, "dec l" },
	{ Z80InstructionClass::Defin, "ld l,*" },
	{ Z80InstructionClass::Defin, "cpl" },


	// 0x30
	{ Z80InstructionClass::Defin, "jr nc,$" },
	{ Z80InstructionClass::Defin, "ld sp,#" },
	{ Z80InstructionClass::Defin, "ld (!),a" },
	{ Z80InstructionClass::Defin, "inc sp" },
	{ Z80InstructionClass::Defin, "inc (hl)" },
	{ Z80InstructionClass::Defin, "dec (hl)" },
	{ Z80InstructionClass::Defin, "ld (hl),*" },
	{ Z80InstructionClass::Defin, "scf" },
	{ Z80InstructionClass::Defin, "jr c,$" },
	{ Z80InstructionClass::Defin, "add hl,sp" },
	{ Z80InstructionClass::Defin, "ld a,(!)" },
	{ Z80InstructionClass::Defin, "dec sp" },
	{ Z80InstructionClass::Defin, "inc a" },
	{ Z80InstructionClass::Defin, "dec a" },
	{ Z80InstructionClass::Defin, "ld a,*" },
	{ Z80InstructionClass::Defin, "ccf" },


	// 0x40
	{ Z80InstructionClass::Defin, "ld b,b" },
	{ Z80InstructionClass::Defin, "ld b,c" },
	{ Z80InstructionClass::Defin, "ld b,d" },
	{ Z80InstructionClass::Defin, "ld b,e" },
	{ Z80InstructionClass::Defin, "ld b,h" },
	{ Z80InstructionClass::Defin, "ld b,l" },
	{ Z80InstructionClass::Defin, "ld b,(hl)" },
	{ Z80InstructionClass::Defin, "ld b,a" },
	{ Z80InstructionClass::Defin, "ld c,b" },
	{ Z80InstructionClass::Defin, "ld c,c" },
	{ Z80InstructionClass::Defin, "ld c,d" },
	{ Z80InstructionClass::Defin, "ld c,e" },
	{ Z80InstructionClass::Defin, "ld c,h" },
	{ Z80InstructionClass::Defin, "ld c,l" },
	{ Z80InstructionClass::Defin, "ld c,(hl)" },
	{ Z80InstructionClass::Defin, "ld c,a" },


	// 0x50
	{ Z80InstructionClass::Defin, "ld d,b" },
	{ Z80InstructionClass::Defin, "ld d,c" },
	{ Z80InstructionClass::Defin, "ld d,d" },
	{ Z80InstructionClass::Defin, "ld d,e" },
	{ Z80InstructionClass::Defin, "ld d,h" },
	{ Z80InstructionClass::Defin, "ld d,l" },
	{ Z80InstructionClass::Defin, "ld d,(hl)" },
	{ Z80InstructionClass::Defin, "ld d,a" },
	{ Z80InstructionClass::Defin, "ld e,b" },
	{ Z80InstructionClass::Defin, "ld e,c" },
	{ Z80InstructionClass::Defin, "ld e,d" },
	{ Z80InstructionClass::Defin, "ld e,e" },
	{ Z80InstructionClass::Defin, "ld e,h" },
	{ Z80InstructionClass::Defin, "ld e,l" },
	{ Z80InstructionClass::Defin, "ld e,(hl)" },
	{ Z80InstructionClass::Defin, "ld e,a" },


	// 0x60
	{ Z80InstructionClass::Defin, "ld h,b" },
	{ Z80InstructionClass::Defin, "ld h,c" },
	{ Z80InstructionClass::Defin, "ld h,d" },
	{ Z80InstructionClass::Defin, "ld h,e" },
	{ Z80InstructionClass::Defin, "ld h,h" },
	{ Z80InstructionClass::Defin, "ld h,l" },
	{ Z80InstructionClass::Defin, "ld h,(hl)" },
	{ Z80InstructionClass::Defin, "ld h,a" },
	{ Z80InstructionClass::Defin, "ld l,b" },
	{ Z80InstructionClass::Defin, "ld l,c" },
	{ Z80InstructionClass::Defin, "ld l,d" },
	{ Z80InstructionClass::Defin, "ld l,e" },
	{ Z80InstructionClass::Defin, "ld l,h" },
	{ Z80InstructionClass::Defin, "ld l,l" },
	{ Z80InstructionClass::Defin, "ld l,(hl)" },
	{ Z80InstructionClass::Defin, "ld l,a" },


	// 0x70
	{ Z80InstructionClass::Defin, "ld (hl),b" },
	{ Z80InstructionClass::Defin, "ld (hl),c" },
	{ Z80InstructionClass::Defin, "ld (hl),d" },
	{ Z80InstructionClass::Defin, "ld (hl),e" },
	{ Z80InstructionClass::Defin, "ld (hl),h" },
	{ Z80InstructionClass::Defin, "ld (hl),l" },
	{ Z80InstructionClass::Defin, ".halt" },
	{ Z80InstructionClass::Defin, "ld (hl),a" },
	{ Z80InstructionClass::Defin, "ld a,b" },
	{ Z80InstructionClass::Defin, "ld a,c" },
	{ Z80InstructionClass::Defin, "ld a,d" },
	{ Z80InstructionClass::Defin, "ld a,e" },
	{ Z80InstructionClass::Defin, "ld a,h" },
	{ Z80InstructionClass::Defin, "ld a,l" },
	{ Z80InstructionClass::Defin, "ld a,(hl)" },
	{ Z80InstructionClass::Defin, "ld a,a" },


	// 0x80
	{ Z80InstructionClass::Defin, "add a,b" },
	{ Z80InstructionClass::Defin, "add a,c" },
	{ Z80InstructionClass::Defin, "add a,d" },
	{ Z80InstructionClass::Defin, "add a,e" },
	{ Z80InstructionClass::Defin, "add a,h" },
	{ Z80InstructionClass::Defin, "add a,l" },
	{ Z80InstructionClass::Defin, "add a,(hl)" },
	{ Z80InstructionClass::Defin, "add a,a" },
	{ Z80InstructionClass::Defin, "adc a,b" },
	{ Z80InstructionClass::Defin, "adc a,c" },
	{ Z80InstructionClass::Defin, "adc a,d" },
	{ Z80InstructionClass::Defin, "adc a,e" },
	{ Z80InstructionClass::Defin, "adc a,h" },
	{ Z80InstructionClass::Defin, "adc a,l" },
	{ Z80InstructionClass::Defin, "adc a,(hl)" },
	{ Z80InstructionClass::Defin, "adc a,a" },


	// 0x90
	{ Z80InstructionClass::Defin, "sub a,b" },
	{ Z80InstructionClass::Defin, "sub a,c" },
	{ Z80InstructionClass::Defin, "sub a,d" },
	{ Z80InstructionClass::Defin, "sub a,e" },
	{ Z80InstructionClass::Defin, "sub a,h" },
	{ Z80InstructionClass::Defin, "sub a,l" },
	{ Z80InstructionClass::Defin, "sub a,(hl)" },
	{ Z80InstructionClass::Defin, "sub a,a" },
	{ Z80InstructionClass::Defin, "sbc a,b" },
	{ Z80InstructionClass::Defin, "sbc a,c" },
	{ Z80InstructionClass::Defin, "sbc a,d" },
	{ Z80InstructionClass::Defin, "sbc a,e" },
	{ Z80InstructionClass::Defin, "sbc a,h" },
	{ Z80InstructionClass::Defin, "sbc a,l" },
	{ Z80InstructionClass::Defin, "sbc a,(hl)" },
	{ Z80InstructionClass::Defin, "sbc a,a" },


	// 0xA0
	{ Z80InstructionClass::Defin, "and a,b" },
	{ Z80InstructionClass::Defin, "and a,c" },
	{ Z80InstructionClass::Defin, "and a,d" },
	{ Z80InstructionClass::Defin, "and a,e" },
	{ Z80InstructionClass::Defin, "and a,h" },
	{ Z80InstructionClass::Defin, "and a,l" },
	{ Z80InstructionClass::Defin, "and a,(hl)" },
	{ Z80InstructionClass::Defin, "and a,a" },
	{ Z80InstructionClass::Defin, "xor a,b" },
	{ Z80InstructionClass::Defin, "xor a,c" },
	{ Z80InstructionClass::Defin, "xor a,d" },
	{ Z80InstructionClass::Defin, "xor a,e" },
	{ Z80InstructionClass::Defin, "xor a,h" },
	{ Z80InstructionClass::Defin, "xor a,l" },
	{ Z80InstructionClass::Defin, "xor a,(hl)" },
	{ Z80InstructionClass::Defin, "xor a,a" },


	// 0xB0
	{ Z80InstructionClass::Defin, "or a,b" },
	{ Z80InstructionClass::Defin, "or a,c" },
	{ Z80InstructionClass::Defin, "or a,d" },
	{ Z80InstructionClass::Defin, "or a,e" },
	{ Z80InstructionClass::Defin, "or a,h" },
	{ Z80InstructionClass::Defin, "or a,l" },
	{ Z80InstructionClass::Defin, "or a,(hl)" },
	{ Z80InstructionClass::Defin, "or a,a" },
	{ Z80InstructionClass::Defin, "cp a,b" },
	{ Z80InstructionClass::Defin, "cp a,c" },
	{ Z80InstructionClass::Defin, "cp a,d" },
	{ Z80InstructionClass::Defin, "cp a,e" },
	{ Z80InstructionClass::Defin, "cp a,h" },
	{ Z80InstructionClass::Defin, "cp a,l" },
	{ Z80InstructionClass::Defin, "cp a,(hl)" },
	{ Z80InstructionClass::Defin, "cp a,a" },


	// 0xC0
	{ Z80InstructionClass::Defin, "ret nz" },
	{ Z80InstructionClass::Defin, "pop bc" },
	{ Z80InstructionClass::Defin, "jp nz,!" },
	{ Z80InstructionClass::Defin, ".jp !" },
	{ Z80InstructionClass::Defin, "call nz,!" },
	{ Z80InstructionClass::Defin, "push bc" },
	{ Z80InstructionClass::Defin, "add a,*" },
	{ Z80InstructionClass::Defin, "rst 00h" },
	{ Z80InstructionClass::Defin, "ret z" },
	{ Z80InstructionClass::Defin, ".ret" },
	{ Z80InstructionClass::Defin, "jp z,!" },
	{ Z80InstructionClass::Defin, "CB" },
	{ Z80InstructionClass::Defin, "call z,!" },
	{ Z80InstructionClass::Defin, "call !" },
	{ Z80InstructionClass::Defin, "adc a,*" },
	{ Z80InstructionClass::Defin, "rst 08h" },


	// 0xD0
	{ Z80InstructionClass::Defin, "ret nc" },
	{ Z80InstructionClass::Defin, "pop de" },
	{ Z80InstructionClass::Defin, "jp nc,!" },
	{ Z80InstructionClass::Defin, "out (*),a" },   // TODO: The multiple formats that get printed for this case are overkill.  Hex only.
	{ Z80InstructionClass::Defin, "call nc,!" },
	{ Z80InstructionClass::Defin, "push de" },
	{ Z80InstructionClass::Defin, "sub a,*" },
	{ Z80InstructionClass::Defin, "rst 10h" },
	{ Z80InstructionClass::Defin, "ret c" },
	{ Z80InstructionClass::Defin, "exx" },
	{ Z80InstructionClass::Defin, "jp c,!" },
	{ Z80InstructionClass::Defin, "in a,(*)" },    // TODO: The multiple formats that get printed for this case are overkill.  Hex only.
	{ Z80InstructionClass::Defin, "call c,!" },
	{ Z80InstructionClass::Defin, "DD" },
	{ Z80InstructionClass::Defin, "sbc a,*" },
	{ Z80InstructionClass::Defin, "rst 18h" },


	// 0xE0
	{ Z80InstructionClass::Defin, "ret po" },
	{ Z80InstructionClass::Defin, "pop hl" },
	{ Z80InstructionClass::Defin, "jp po,!" },
	{ Z80InstructionClass::Defin, "ex (sp),hl" },
	{ Z80InstructionClass::Defin, "call po,!" },
	{ Z80InstructionClass::Defin, "push hl" },
	{ Z80InstructionClass::Defin, "and a,*" },
	{ Z80InstructionClass::Defin, "rst 20h" },
	{ Z80InstructionClass::Defin, "ret pe" },
	{ Z80InstructionClass::Defin, "jp (hl)" },
	{ Z80InstructionClass::Defin, "jp pe,!" },
	{ Z80InstructionClass::Defin, "ex de,hl" },
	{ Z80InstructionClass::Defin, "call pe,!" },
	{ Z80InstructionClass::Defin, "ED" },
	{ Z80InstructionClass::Defin, "xor a,*" },
	{ Z80InstructionClass::Defin, "rst 28h" },


	// 0xF0
	{ Z80InstructionClass::Defin, "ret p" },
	{ Z80InstructionClass::Defin, "pop af" },
	{ Z80InstructionClass::Defin, "jp p,!" },
	{ Z80InstructionClass::Defin, "di" },
	{ Z80InstructionClass::Defin, "call p,!" },
	{ Z80InstructionClass::Defin, "push af" },
	{ Z80InstructionClass::Defin, "or a,*" },
	{ Z80InstructionClass::Defin, "rst 30h" },
	{ Z80InstructionClass::Defin, "ret m" },
	{ Z80InstructionClass::Defin, "ld sp,hl" },
	{ Z80InstructionClass::Defin, "jp m,!" },
	{ Z80InstructionClass::Defin, "ei" },
	{ Z80InstructionClass::Defin, "call m,!" },
	{ Z80InstructionClass::Defin, "FD" },
	{ Z80InstructionClass::Defin, "cp a,*" },
	{ Z80InstructionClass::Defin, "rst 38h" },

	};





	// INSTRUCTION SET SECTION: Extended instructions (ED)

	Disasm  g_DisassemblyED[] = {

	// 0x00
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x10
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x20
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },



	// 0x30
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x40
	{ Z80InstructionClass::Defin, "in b,(c)" },
	{ Z80InstructionClass::Defin, "out (c),b" },
	{ Z80InstructionClass::Defin, "sbc hl,bc" },
	{ Z80InstructionClass::Defin, "ld (!),bc" },
	{ Z80InstructionClass::Defin, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 0" },
	{ Z80InstructionClass::Defin, "ld i,a" },
	{ Z80InstructionClass::Defin, "in c,(c)" },
	{ Z80InstructionClass::Defin, "out (c),c" },
	{ Z80InstructionClass::Defin, "adc hl,bc" },
	{ Z80InstructionClass::Defin, "ld bc,(!)" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".reti" },
	{ Z80InstructionClass::Unoff, "im 0" },  // TODO: Is it 0?
	{ Z80InstructionClass::Defin, "ld r,a" },


	// 0x50
	{ Z80InstructionClass::Defin, "in d,(c)" },
	{ Z80InstructionClass::Defin, "out (c),d" },
	{ Z80InstructionClass::Defin, "sbc hl,de" },
	{ Z80InstructionClass::Defin, "ld (!),de" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 1" },
	{ Z80InstructionClass::Defin, "ld a,i" },
	{ Z80InstructionClass::Defin, "in e,(c)" },
	{ Z80InstructionClass::Defin, "out (c),e" },
	{ Z80InstructionClass::Defin, "adc hl,de" },
	{ Z80InstructionClass::Defin, "ld de,(!)" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 2" },
	{ Z80InstructionClass::Defin, "ld a,r" },


	// 0x60
	{ Z80InstructionClass::Defin, "in h,(c)" },
	{ Z80InstructionClass::Defin, "out (c),h" },
	{ Z80InstructionClass::Defin, "sbc hl,hl" },
	{ Z80InstructionClass::Unoff, "ld (!),hl" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 0" },
	{ Z80InstructionClass::Defin, "rrd" },
	{ Z80InstructionClass::Defin, "in l,(c)" },
	{ Z80InstructionClass::Defin, "out (c),l" },
	{ Z80InstructionClass::Defin, "adc hl,hl" },
	{ Z80InstructionClass::Unoff, "ld hl,(!)" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Unoff, "im 0" },  // TODO: Is it 0?
	{ Z80InstructionClass::Defin, "rld" },


	// 0x70
	{ Z80InstructionClass::Unoff, "in (c)" },
	{ Z80InstructionClass::Unoff, "out (c),0" },
	{ Z80InstructionClass::Defin, "sbc hl,sp" },
	{ Z80InstructionClass::Defin, "ld (!),sp" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 1" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "in a,(c)" },
	{ Z80InstructionClass::Defin, "out (c),a" },
	{ Z80InstructionClass::Defin, "adc hl,sp" },
	{ Z80InstructionClass::Defin, "ld sp,(!)" },
	{ Z80InstructionClass::Unoff, "neg" },
	{ Z80InstructionClass::Defin, ".retn" },
	{ Z80InstructionClass::Defin, "im 2" },
	{ Z80InstructionClass::Undef, "" },


	// 0x80
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x90
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xA0
	{ Z80InstructionClass::Defin, "ldi" },
	{ Z80InstructionClass::Defin, "cpi" },
	{ Z80InstructionClass::Defin, "ini" },
	{ Z80InstructionClass::Defin, "outi" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ldd" },
	{ Z80InstructionClass::Defin, "cpd" },
	{ Z80InstructionClass::Defin, "ind" },
	{ Z80InstructionClass::Defin, "outd" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xB0
	{ Z80InstructionClass::Defin, "ldir" },
	{ Z80InstructionClass::Defin, "cpir" },
	{ Z80InstructionClass::Defin, "inir" },
	{ Z80InstructionClass::Defin, "otir" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "lddr" },
	{ Z80InstructionClass::Defin, "cpdr" },
	{ Z80InstructionClass::Defin, "indr" },
	{ Z80InstructionClass::Defin, "otdr" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xC0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xD0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xE0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xF0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },

	};




	// INSTRUCTION TABLE: Bit inst

	Disasm  g_DisassemblyCB[] = {

	// 0x00
	{ Z80InstructionClass::Defin, "rlc b" },
	{ Z80InstructionClass::Defin, "rlc c" },
	{ Z80InstructionClass::Defin, "rlc d" },
	{ Z80InstructionClass::Defin, "rlc e" },
	{ Z80InstructionClass::Defin, "rlc h" },
	{ Z80InstructionClass::Defin, "rlc l" },
	{ Z80InstructionClass::Defin, "rlc (hl)" },
	{ Z80InstructionClass::Defin, "rlc a" },
	{ Z80InstructionClass::Defin, "rrc b" },
	{ Z80InstructionClass::Defin, "rrc c" },
	{ Z80InstructionClass::Defin, "rrc d" },
	{ Z80InstructionClass::Defin, "rrc e" },
	{ Z80InstructionClass::Defin, "rrc h" },
	{ Z80InstructionClass::Defin, "rrc l" },
	{ Z80InstructionClass::Defin, "rrc (hl)" },
	{ Z80InstructionClass::Defin, "rrc a" },


	// 0x10
	{ Z80InstructionClass::Defin, "rl b" },
	{ Z80InstructionClass::Defin, "rl c" },
	{ Z80InstructionClass::Defin, "rl d" },
	{ Z80InstructionClass::Defin, "rl e" },
	{ Z80InstructionClass::Defin, "rl h" },
	{ Z80InstructionClass::Defin, "rl l" },
	{ Z80InstructionClass::Defin, "rl (hl)" },
	{ Z80InstructionClass::Defin, "rl a" },
	{ Z80InstructionClass::Defin, "rr b" },
	{ Z80InstructionClass::Defin, "rr c" },
	{ Z80InstructionClass::Defin, "rr d" },
	{ Z80InstructionClass::Defin, "rr e" },
	{ Z80InstructionClass::Defin, "rr h" },
	{ Z80InstructionClass::Defin, "rr l" },
	{ Z80InstructionClass::Defin, "rr (hl)" },
	{ Z80InstructionClass::Defin, "rr a" },


	// 0x20
	{ Z80InstructionClass::Defin, "sla b" },
	{ Z80InstructionClass::Defin, "sla c" },
	{ Z80InstructionClass::Defin, "sla d" },
	{ Z80InstructionClass::Defin, "sla e" },
	{ Z80InstructionClass::Defin, "sla h" },
	{ Z80InstructionClass::Defin, "sla l" },
	{ Z80InstructionClass::Defin, "sla (hl)" },
	{ Z80InstructionClass::Defin, "sla a" },
	{ Z80InstructionClass::Defin, "sra b" },
	{ Z80InstructionClass::Defin, "sra c" },
	{ Z80InstructionClass::Defin, "sra d" },
	{ Z80InstructionClass::Defin, "sra e" },
	{ Z80InstructionClass::Defin, "sra h" },
	{ Z80InstructionClass::Defin, "sra l" },
	{ Z80InstructionClass::Defin, "sra (hl)" },
	{ Z80InstructionClass::Defin, "sra a" },


	// 0x30
	{ Z80InstructionClass::Unoff, "sll b" },
	{ Z80InstructionClass::Unoff, "sll c" },
	{ Z80InstructionClass::Unoff, "sll d" },
	{ Z80InstructionClass::Unoff, "sll e" },
	{ Z80InstructionClass::Unoff, "sll h" },
	{ Z80InstructionClass::Unoff, "sll l" },
	{ Z80InstructionClass::Unoff, "sll (hl)" },
	{ Z80InstructionClass::Unoff, "sll a" },
	{ Z80InstructionClass::Defin, "srl b" },
	{ Z80InstructionClass::Defin, "srl c" },
	{ Z80InstructionClass::Defin, "srl d" },
	{ Z80InstructionClass::Defin, "srl e" },
	{ Z80InstructionClass::Defin, "srl h" },
	{ Z80InstructionClass::Defin, "srl l" },
	{ Z80InstructionClass::Defin, "srl (hl)" },
	{ Z80InstructionClass::Defin, "srl a" },


	// 0x40
	{ Z80InstructionClass::Defin, "bit 0,b" },
	{ Z80InstructionClass::Defin, "bit 0,c" },
	{ Z80InstructionClass::Defin, "bit 0,d" },
	{ Z80InstructionClass::Defin, "bit 0,e" },
	{ Z80InstructionClass::Defin, "bit 0,h" },
	{ Z80InstructionClass::Defin, "bit 0,l" },
	{ Z80InstructionClass::Defin, "bit 0,(hl)" },
	{ Z80InstructionClass::Defin, "bit 0,a" },
	{ Z80InstructionClass::Defin, "bit 1,b" },
	{ Z80InstructionClass::Defin, "bit 1,c" },
	{ Z80InstructionClass::Defin, "bit 1,d" },
	{ Z80InstructionClass::Defin, "bit 1,e" },
	{ Z80InstructionClass::Defin, "bit 1,h" },
	{ Z80InstructionClass::Defin, "bit 1,l" },
	{ Z80InstructionClass::Defin, "bit 1,(hl)" },
	{ Z80InstructionClass::Defin, "bit 1,a" },


	// 0x50
	{ Z80InstructionClass::Defin, "bit 2,b" },
	{ Z80InstructionClass::Defin, "bit 2,c" },
	{ Z80InstructionClass::Defin, "bit 2,d" },
	{ Z80InstructionClass::Defin, "bit 2,e" },
	{ Z80InstructionClass::Defin, "bit 2,h" },
	{ Z80InstructionClass::Defin, "bit 2,l" },
	{ Z80InstructionClass::Defin, "bit 2,(hl)" },
	{ Z80InstructionClass::Defin, "bit 2,a" },
	{ Z80InstructionClass::Defin, "bit 3,b" },
	{ Z80InstructionClass::Defin, "bit 3,c" },
	{ Z80InstructionClass::Defin, "bit 3,d" },
	{ Z80InstructionClass::Defin, "bit 3,e" },
	{ Z80InstructionClass::Defin, "bit 3,h" },
	{ Z80InstructionClass::Defin, "bit 3,l" },
	{ Z80InstructionClass::Defin, "bit 3,(hl)" },
	{ Z80InstructionClass::Defin, "bit 3,a" },


	// 0x60
	{ Z80InstructionClass::Defin, "bit 4,b" },
	{ Z80InstructionClass::Defin, "bit 4,c" },
	{ Z80InstructionClass::Defin, "bit 4,d" },
	{ Z80InstructionClass::Defin, "bit 4,e" },
	{ Z80InstructionClass::Defin, "bit 4,h" },
	{ Z80InstructionClass::Defin, "bit 4,l" },
	{ Z80InstructionClass::Defin, "bit 4,(hl)" },
	{ Z80InstructionClass::Defin, "bit 4,a" },
	{ Z80InstructionClass::Defin, "bit 5,b" },
	{ Z80InstructionClass::Defin, "bit 5,c" },
	{ Z80InstructionClass::Defin, "bit 5,d" },
	{ Z80InstructionClass::Defin, "bit 5,e" },
	{ Z80InstructionClass::Defin, "bit 5,h" },
	{ Z80InstructionClass::Defin, "bit 5,l" },
	{ Z80InstructionClass::Defin, "bit 5,(hl)" },
	{ Z80InstructionClass::Defin, "bit 5,a" },


	// 0x70
	{ Z80InstructionClass::Defin, "bit 6,b" },
	{ Z80InstructionClass::Defin, "bit 6,c" },
	{ Z80InstructionClass::Defin, "bit 6,d" },
	{ Z80InstructionClass::Defin, "bit 6,e" },
	{ Z80InstructionClass::Defin, "bit 6,h" },
	{ Z80InstructionClass::Defin, "bit 6,l" },
	{ Z80InstructionClass::Defin, "bit 6,(hl)" },
	{ Z80InstructionClass::Defin, "bit 6,a" },
	{ Z80InstructionClass::Defin, "bit 7,b" },
	{ Z80InstructionClass::Defin, "bit 7,c" },
	{ Z80InstructionClass::Defin, "bit 7,d" },
	{ Z80InstructionClass::Defin, "bit 7,e" },
	{ Z80InstructionClass::Defin, "bit 7,h" },
	{ Z80InstructionClass::Defin, "bit 7,l" },
	{ Z80InstructionClass::Defin, "bit 7,(hl)" },
	{ Z80InstructionClass::Defin, "bit 7,a" },


	// 0x80
	{ Z80InstructionClass::Defin, "res 0,b" },
	{ Z80InstructionClass::Defin, "res 0,c" },
	{ Z80InstructionClass::Defin, "res 0,d" },
	{ Z80InstructionClass::Defin, "res 0,e" },
	{ Z80InstructionClass::Defin, "res 0,h" },
	{ Z80InstructionClass::Defin, "res 0,l" },
	{ Z80InstructionClass::Defin, "res 0,(hl)" },
	{ Z80InstructionClass::Defin, "res 0,a" },
	{ Z80InstructionClass::Defin, "res 1,b" },
	{ Z80InstructionClass::Defin, "res 1,c" },
	{ Z80InstructionClass::Defin, "res 1,d" },
	{ Z80InstructionClass::Defin, "res 1,e" },
	{ Z80InstructionClass::Defin, "res 1,h" },
	{ Z80InstructionClass::Defin, "res 1,l" },
	{ Z80InstructionClass::Defin, "res 1,(hl)" },
	{ Z80InstructionClass::Defin, "res 1,a" },


	// 0x90
	{ Z80InstructionClass::Defin, "res 2,b" },
	{ Z80InstructionClass::Defin, "res 2,c" },
	{ Z80InstructionClass::Defin, "res 2,d" },
	{ Z80InstructionClass::Defin, "res 2,e" },
	{ Z80InstructionClass::Defin, "res 2,h" },
	{ Z80InstructionClass::Defin, "res 2,l" },
	{ Z80InstructionClass::Defin, "res 2,(hl)" },
	{ Z80InstructionClass::Defin, "res 2,a" },
	{ Z80InstructionClass::Defin, "res 3,b" },
	{ Z80InstructionClass::Defin, "res 3,c" },
	{ Z80InstructionClass::Defin, "res 3,d" },
	{ Z80InstructionClass::Defin, "res 3,e" },
	{ Z80InstructionClass::Defin, "res 3,h" },
	{ Z80InstructionClass::Defin, "res 3,l" },
	{ Z80InstructionClass::Defin, "res 3,(hl)" },
	{ Z80InstructionClass::Defin, "res 3,a" },


	// 0xA0
	{ Z80InstructionClass::Defin, "res 4,b" },
	{ Z80InstructionClass::Defin, "res 4,c" },
	{ Z80InstructionClass::Defin, "res 4,d" },
	{ Z80InstructionClass::Defin, "res 4,e" },
	{ Z80InstructionClass::Defin, "res 4,h" },
	{ Z80InstructionClass::Defin, "res 4,l" },
	{ Z80InstructionClass::Defin, "res 4,(hl)" },
	{ Z80InstructionClass::Defin, "res 4,a" },
	{ Z80InstructionClass::Defin, "res 5,b" },
	{ Z80InstructionClass::Defin, "res 5,c" },
	{ Z80InstructionClass::Defin, "res 5,d" },
	{ Z80InstructionClass::Defin, "res 5,e" },
	{ Z80InstructionClass::Defin, "res 5,h" },
	{ Z80InstructionClass::Defin, "res 5,l" },
	{ Z80InstructionClass::Defin, "res 5,(hl)" },
	{ Z80InstructionClass::Defin, "res 5,a" },


	// 0xB0
	{ Z80InstructionClass::Defin, "res 6,b" },
	{ Z80InstructionClass::Defin, "res 6,c" },
	{ Z80InstructionClass::Defin, "res 6,d" },
	{ Z80InstructionClass::Defin, "res 6,e" },
	{ Z80InstructionClass::Defin, "res 6,h" },
	{ Z80InstructionClass::Defin, "res 6,l" },
	{ Z80InstructionClass::Defin, "res 6,(hl)" },
	{ Z80InstructionClass::Defin, "res 6,a" },
	{ Z80InstructionClass::Defin, "res 7,b" },
	{ Z80InstructionClass::Defin, "res 7,c" },
	{ Z80InstructionClass::Defin, "res 7,d" },
	{ Z80InstructionClass::Defin, "res 7,e" },
	{ Z80InstructionClass::Defin, "res 7,h" },
	{ Z80InstructionClass::Defin, "res 7,l" },
	{ Z80InstructionClass::Defin, "res 7,(hl)" },
	{ Z80InstructionClass::Defin, "res 7,a" },


	// 0xC0
	{ Z80InstructionClass::Defin, "set 0,b" },
	{ Z80InstructionClass::Defin, "set 0,c" },
	{ Z80InstructionClass::Defin, "set 0,d" },
	{ Z80InstructionClass::Defin, "set 0,e" },
	{ Z80InstructionClass::Defin, "set 0,h" },
	{ Z80InstructionClass::Defin, "set 0,l" },
	{ Z80InstructionClass::Defin, "set 0,(hl)" },
	{ Z80InstructionClass::Defin, "set 0,a" },
	{ Z80InstructionClass::Defin, "set 1,b" },
	{ Z80InstructionClass::Defin, "set 1,c" },
	{ Z80InstructionClass::Defin, "set 1,d" },
	{ Z80InstructionClass::Defin, "set 1,e" },
	{ Z80InstructionClass::Defin, "set 1,h" },
	{ Z80InstructionClass::Defin, "set 1,l" },
	{ Z80InstructionClass::Defin, "set 1,(hl)" },
	{ Z80InstructionClass::Defin, "set 1,a" },


	// 0xD0
	{ Z80InstructionClass::Defin, "set 2,b" },
	{ Z80InstructionClass::Defin, "set 2,c" },
	{ Z80InstructionClass::Defin, "set 2,d" },
	{ Z80InstructionClass::Defin, "set 2,e" },
	{ Z80InstructionClass::Defin, "set 2,h" },
	{ Z80InstructionClass::Defin, "set 2,l" },
	{ Z80InstructionClass::Defin, "set 2,(hl)" },
	{ Z80InstructionClass::Defin, "set 2,a" },
	{ Z80InstructionClass::Defin, "set 3,b" },
	{ Z80InstructionClass::Defin, "set 3,c" },
	{ Z80InstructionClass::Defin, "set 3,d" },
	{ Z80InstructionClass::Defin, "set 3,e" },
	{ Z80InstructionClass::Defin, "set 3,h" },
	{ Z80InstructionClass::Defin, "set 3,l" },
	{ Z80InstructionClass::Defin, "set 3,(hl)" },
	{ Z80InstructionClass::Defin, "set 3,a" },


	// 0xE0
	{ Z80InstructionClass::Defin, "set 4,b" },
	{ Z80InstructionClass::Defin, "set 4,c" },
	{ Z80InstructionClass::Defin, "set 4,d" },
	{ Z80InstructionClass::Defin, "set 4,e" },
	{ Z80InstructionClass::Defin, "set 4,h" },
	{ Z80InstructionClass::Defin, "set 4,l" },
	{ Z80InstructionClass::Defin, "set 4,(hl)" },
	{ Z80InstructionClass::Defin, "set 4,a" },
	{ Z80InstructionClass::Defin, "set 5,b" },
	{ Z80InstructionClass::Defin, "set 5,c" },
	{ Z80InstructionClass::Defin, "set 5,d" },
	{ Z80InstructionClass::Defin, "set 5,e" },
	{ Z80InstructionClass::Defin, "set 5,h" },
	{ Z80InstructionClass::Defin, "set 5,l" },
	{ Z80InstructionClass::Defin, "set 5,(hl)" },
	{ Z80InstructionClass::Defin, "set 5,a" },


	// 0xF0
	{ Z80InstructionClass::Defin, "set 6,b" },
	{ Z80InstructionClass::Defin, "set 6,c" },
	{ Z80InstructionClass::Defin, "set 6,d" },
	{ Z80InstructionClass::Defin, "set 6,e" },
	{ Z80InstructionClass::Defin, "set 6,h" },
	{ Z80InstructionClass::Defin, "set 6,l" },
	{ Z80InstructionClass::Defin, "set 6,(hl)" },
	{ Z80InstructionClass::Defin, "set 6,a" },
	{ Z80InstructionClass::Defin, "set 7,b" },
	{ Z80InstructionClass::Defin, "set 7,c" },
	{ Z80InstructionClass::Defin, "set 7,d" },
	{ Z80InstructionClass::Defin, "set 7,e" },
	{ Z80InstructionClass::Defin, "set 7,h" },
	{ Z80InstructionClass::Defin, "set 7,l" },
	{ Z80InstructionClass::Defin, "set 7,(hl)" },
	{ Z80InstructionClass::Defin, "set 7,a" },

	};


	//
	// IX instructions (DD)
	// (In code the disassembler will have to handle falling back to the main set).
	//

	Disasm  g_DisassemblyDD[] = {

	// 0x00
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add ix,bc" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x10
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add ix,de" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x20
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld ix,#" },
	{ Z80InstructionClass::Defin, "ld (!),ix" },
	{ Z80InstructionClass::Defin, "inc ix" },
	{ Z80InstructionClass::Unoff, "inc ixh" },
	{ Z80InstructionClass::Unoff, "dec ixh" },
	{ Z80InstructionClass::Unoff, "ld ixh,*" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add ix,ix" },
	{ Z80InstructionClass::Defin, "ld ix,(!)" },
	{ Z80InstructionClass::Defin, "dec ix" },
	{ Z80InstructionClass::Unoff, "inc ixl" },
	{ Z80InstructionClass::Unoff, "dec ixl" },
	{ Z80InstructionClass::Unoff, "ld ixl,*" },
	{ Z80InstructionClass::Undef, "" },


	// 0x30
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "inc (ix%)" },
	{ Z80InstructionClass::Defin, "dec (ix%)" },
	{ Z80InstructionClass::Defin, "ld (ix%),*" },  // Haha, luckily the order of the encoding matches the order of the special characters!
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add ix,sp" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x40
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld b,ixh" },
	{ Z80InstructionClass::Unoff, "ld b,ixl" },
	{ Z80InstructionClass::Defin, "ld b,(ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld c,ixh" },
	{ Z80InstructionClass::Unoff, "ld c,ixl" },
	{ Z80InstructionClass::Defin, "ld c,(ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x50
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld d,ixh" },
	{ Z80InstructionClass::Unoff, "ld d,ixl" },
	{ Z80InstructionClass::Defin, "ld d,(ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld e,ixh" },
	{ Z80InstructionClass::Unoff, "ld e,ixl" },
	{ Z80InstructionClass::Defin, "ld e,(ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x60
	{ Z80InstructionClass::Unoff, "ld ixh,b" },
	{ Z80InstructionClass::Unoff, "ld ixh,c" },
	{ Z80InstructionClass::Unoff, "ld ixh,d" },
	{ Z80InstructionClass::Unoff, "ld ixh,e" },
	{ Z80InstructionClass::Unoff, "ld ixh,ixh" },
	{ Z80InstructionClass::Unoff, "ld ixh,ixl" },
	{ Z80InstructionClass::Defin, "ld h,(ix%)" },
	{ Z80InstructionClass::Unoff, "ld ixh,a" },
	{ Z80InstructionClass::Unoff, "ld ixl,b" },
	{ Z80InstructionClass::Unoff, "ld ixl,c" },
	{ Z80InstructionClass::Unoff, "ld ixl,d" },
	{ Z80InstructionClass::Unoff, "ld ixl,e" },
	{ Z80InstructionClass::Unoff, "ld ixl,ixh" },
	{ Z80InstructionClass::Unoff, "ld ixl,ixl" },
	{ Z80InstructionClass::Defin, "ld l,(ix%)" },
	{ Z80InstructionClass::Unoff, "ld ixl,a" },


	// 0x70
	{ Z80InstructionClass::Defin, "ld (ix%),b" },
	{ Z80InstructionClass::Defin, "ld (ix%),c" },
	{ Z80InstructionClass::Defin, "ld (ix%),d" },
	{ Z80InstructionClass::Defin, "ld (ix%),e" },
	{ Z80InstructionClass::Defin, "ld (ix%),h" },
	{ Z80InstructionClass::Defin, "ld (ix%),l" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld (ix%),a" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld a,ixh" },
	{ Z80InstructionClass::Unoff, "ld a,ixl" },
	{ Z80InstructionClass::Defin, "ld a,(ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x80
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "add a,ixh" },
	{ Z80InstructionClass::Unoff, "add a,ixl" },
	{ Z80InstructionClass::Defin, "add a,(ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "adc a,ixh" },
	{ Z80InstructionClass::Unoff, "adc a,ixl" },
	{ Z80InstructionClass::Defin, "adc a,(ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x90
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "sub ixh" },
	{ Z80InstructionClass::Unoff, "sub ixl" },
	{ Z80InstructionClass::Defin, "sub (ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "sbc a,ixh" },
	{ Z80InstructionClass::Unoff, "sbc a,ixl" },
	{ Z80InstructionClass::Defin, "sbc a,(ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xA0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "and ixh" },
	{ Z80InstructionClass::Unoff, "and ixl" },
	{ Z80InstructionClass::Defin, "and (ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "xor ixh" },
	{ Z80InstructionClass::Unoff, "xor ixl" },
	{ Z80InstructionClass::Defin, "xor (ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xB0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "or ixh" },
	{ Z80InstructionClass::Unoff, "or ixl" },
	{ Z80InstructionClass::Defin, "or (ix%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "cp ixh" },
	{ Z80InstructionClass::Unoff, "cp ixl" },
	{ Z80InstructionClass::Defin, "cp (ix%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xC0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "CB" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xD0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xE0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "pop ix" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ex (sp),ix" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "push ix" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "jp (ix)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xF0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld sp,ix" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },

	};


	// IX bit instructions (DDCB)

	Disasm  g_DisassemblyDDCB[] = {

	// 0x00
	{ Z80InstructionClass::Unoff, "rlc (ix%),b" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),c" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),d" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),e" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),h" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),l" },
	{ Z80InstructionClass::Defin, "rlc (ix%)" },
	{ Z80InstructionClass::Unoff, "rlc (ix%),a" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),b" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),c" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),d" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),e" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),h" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),l" },
	{ Z80InstructionClass::Defin, "rrc (ix%)" },
	{ Z80InstructionClass::Unoff, "rrc (ix%),a" },


	// 0x10
	{ Z80InstructionClass::Unoff, "rl (ix%),b" },
	{ Z80InstructionClass::Unoff, "rl (ix%),c" },
	{ Z80InstructionClass::Unoff, "rl (ix%),d" },
	{ Z80InstructionClass::Unoff, "rl (ix%),e" },
	{ Z80InstructionClass::Unoff, "rl (ix%),h" },
	{ Z80InstructionClass::Unoff, "rl (ix%),l" },
	{ Z80InstructionClass::Defin, "rl (ix%)" },
	{ Z80InstructionClass::Unoff, "rl (ix%),a" },
	{ Z80InstructionClass::Unoff, "rr (ix%),b" },
	{ Z80InstructionClass::Unoff, "rr (ix%),c" },
	{ Z80InstructionClass::Unoff, "rr (ix%),d" },
	{ Z80InstructionClass::Unoff, "rr (ix%),e" },
	{ Z80InstructionClass::Unoff, "rr (ix%),h" },
	{ Z80InstructionClass::Unoff, "rr (ix%),l" },
	{ Z80InstructionClass::Defin, "rr (ix%)" },
	{ Z80InstructionClass::Unoff, "rr (ix%),a" },


	// 0x20
	{ Z80InstructionClass::Unoff, "sla (ix%),b" },
	{ Z80InstructionClass::Unoff, "sla (ix%),c" },
	{ Z80InstructionClass::Unoff, "sla (ix%),d" },
	{ Z80InstructionClass::Unoff, "sla (ix%),e" },
	{ Z80InstructionClass::Unoff, "sla (ix%),h" },
	{ Z80InstructionClass::Unoff, "sla (ix%),l" },
	{ Z80InstructionClass::Defin, "sla (ix%)" },
	{ Z80InstructionClass::Unoff, "sla (ix%),a" },
	{ Z80InstructionClass::Unoff, "sra (ix%),b" },
	{ Z80InstructionClass::Unoff, "sra (ix%),c" },
	{ Z80InstructionClass::Unoff, "sra (ix%),d" },
	{ Z80InstructionClass::Unoff, "sra (ix%),e" },
	{ Z80InstructionClass::Unoff, "sra (ix%),h" },
	{ Z80InstructionClass::Unoff, "sra (ix%),l" },
	{ Z80InstructionClass::Defin, "sra (ix%)" },
	{ Z80InstructionClass::Unoff, "sra (ix%),a" },


	// 0x30
	{ Z80InstructionClass::Unoff, "sll (ix%),b" },
	{ Z80InstructionClass::Unoff, "sll (ix%),c" },
	{ Z80InstructionClass::Unoff, "sll (ix%),d" },
	{ Z80InstructionClass::Unoff, "sll (ix%),e" },
	{ Z80InstructionClass::Unoff, "sll (ix%),h" },
	{ Z80InstructionClass::Unoff, "sll (ix%),l" },
	{ Z80InstructionClass::Unoff, "sll (ix%)" },
	{ Z80InstructionClass::Unoff, "sll (ix%),a" },
	{ Z80InstructionClass::Unoff, "srl (ix%),b" },
	{ Z80InstructionClass::Unoff, "srl (ix%),c" },
	{ Z80InstructionClass::Unoff, "srl (ix%),d" },
	{ Z80InstructionClass::Unoff, "srl (ix%),e" },
	{ Z80InstructionClass::Unoff, "srl (ix%),h" },
	{ Z80InstructionClass::Unoff, "srl (ix%),l" },
	{ Z80InstructionClass::Defin, "srl (ix%)" },
	{ Z80InstructionClass::Unoff, "srl (ix%),a" },


	// 0x40
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(ix%)" },


	// 0x50
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(ix%)" },


	// 0x60
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(ix%)" },


	// 0x70
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },
	{ Z80InstructionClass::Defin, "bit 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(ix%)" },


	// 0x80
	{ Z80InstructionClass::Unoff, "res 0,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 0,(ix%),a" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 1,(ix%),a" },


	// 0x90
	{ Z80InstructionClass::Unoff, "res 2,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 2,(ix%),a" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 3,(ix%),a" },


	// 0xA0
	{ Z80InstructionClass::Unoff, "res 4,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 4,(ix%),a" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 5,(ix%),a" },


	// 0xB0
	{ Z80InstructionClass::Unoff, "res 6,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 6,(ix%),a" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),b" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),c" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),d" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),e" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),h" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),l" },
	{ Z80InstructionClass::Defin, "res 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "res 7,(ix%),a" },


	// 0xC0
	{ Z80InstructionClass::Unoff, "set 0,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 0,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 0,(ix%),a" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 1,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 1,(ix%),a" },


	// 0xD0
	{ Z80InstructionClass::Unoff, "set 2,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 2,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 2,(ix%),a" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 3,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 3,(ix%),a" },


	// 0xE0
	{ Z80InstructionClass::Unoff, "set 4,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 4,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 4,(ix%),a" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 5,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 5,(ix%),a" },


	// 0xF0
	{ Z80InstructionClass::Unoff, "set 6,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 6,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 6,(ix%),a" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),b" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),c" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),d" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),e" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),h" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),l" },
	{ Z80InstructionClass::Defin, "set 7,(ix%)" },
	{ Z80InstructionClass::Unoff, "set 7,(ix%),a" },

	};






	//
	// IY instructions (FD)
	// (In code the disassembler will have to handle falling back to the main set).
	//

	Disasm  g_DisassemblyFD[] = {

	// 0x00
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add iy,bc" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x10
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add iy,de" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x20
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld iy,#" },
	{ Z80InstructionClass::Defin, "ld (!),iy" },
	{ Z80InstructionClass::Defin, "inc iy" },
	{ Z80InstructionClass::Unoff, "inc iyh" },
	{ Z80InstructionClass::Unoff, "dec iyh" },
	{ Z80InstructionClass::Unoff, "ld iyh,*" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add iy,iy" },
	{ Z80InstructionClass::Defin, "ld iy,(!)" },
	{ Z80InstructionClass::Defin, "dec iy" },
	{ Z80InstructionClass::Unoff, "inc iyl" },
	{ Z80InstructionClass::Unoff, "dec iyl" },
	{ Z80InstructionClass::Unoff, "ld iyl,*" },
	{ Z80InstructionClass::Undef, "" },


	// 0x30
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "inc (iy%)" },
	{ Z80InstructionClass::Defin, "dec (iy%)" },
	{ Z80InstructionClass::Defin, "ld (iy%),*" },  // Haha, luckily the order of the encoding matches the order of the special characters!
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "add iy,sp" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0x40
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld b,iyh" },
	{ Z80InstructionClass::Unoff, "ld b,iyl" },
	{ Z80InstructionClass::Defin, "ld b,(iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld c,iyh" },
	{ Z80InstructionClass::Unoff, "ld c,iyl" },
	{ Z80InstructionClass::Defin, "ld c,(iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x50
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld d,iyh" },
	{ Z80InstructionClass::Unoff, "ld d,iyl" },
	{ Z80InstructionClass::Defin, "ld d,(iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld e,iyh" },
	{ Z80InstructionClass::Unoff, "ld e,iyl" },
	{ Z80InstructionClass::Defin, "ld e,(iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x60
	{ Z80InstructionClass::Unoff, "ld iyh,b" },
	{ Z80InstructionClass::Unoff, "ld iyh,c" },
	{ Z80InstructionClass::Unoff, "ld iyh,d" },
	{ Z80InstructionClass::Unoff, "ld iyh,e" },
	{ Z80InstructionClass::Unoff, "ld iyh,iyh" },
	{ Z80InstructionClass::Unoff, "ld iyh,iyl" },
	{ Z80InstructionClass::Defin, "ld h,(iy%)" },
	{ Z80InstructionClass::Unoff, "ld iyh,a" },
	{ Z80InstructionClass::Unoff, "ld iyl,b" },
	{ Z80InstructionClass::Unoff, "ld iyl,c" },
	{ Z80InstructionClass::Unoff, "ld iyl,d" },
	{ Z80InstructionClass::Unoff, "ld iyl,e" },
	{ Z80InstructionClass::Unoff, "ld iyl,iyh" },
	{ Z80InstructionClass::Unoff, "ld iyl,iyl" },
	{ Z80InstructionClass::Defin, "ld l,(iy%)" },
	{ Z80InstructionClass::Unoff, "ld iyl,a" },


	// 0x70
	{ Z80InstructionClass::Defin, "ld (iy%),b" },
	{ Z80InstructionClass::Defin, "ld (iy%),c" },
	{ Z80InstructionClass::Defin, "ld (iy%),d" },
	{ Z80InstructionClass::Defin, "ld (iy%),e" },
	{ Z80InstructionClass::Defin, "ld (iy%),h" },
	{ Z80InstructionClass::Defin, "ld (iy%),l" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld (iy%),a" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "ld a,iyh" },
	{ Z80InstructionClass::Unoff, "ld a,iyl" },
	{ Z80InstructionClass::Defin, "ld a,(iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x80
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "add a,iyh" },
	{ Z80InstructionClass::Unoff, "add a,iyl" },
	{ Z80InstructionClass::Defin, "add a,(iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "adc a,iyh" },
	{ Z80InstructionClass::Unoff, "adc a,iyl" },
	{ Z80InstructionClass::Defin, "adc a,(iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0x90
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "sub iyh" },
	{ Z80InstructionClass::Unoff, "sub iyl" },
	{ Z80InstructionClass::Defin, "sub (iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "sbc a,iyh" },
	{ Z80InstructionClass::Unoff, "sbc a,iyl" },
	{ Z80InstructionClass::Defin, "sbc a,(iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xA0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "and iyh" },
	{ Z80InstructionClass::Unoff, "and iyl" },
	{ Z80InstructionClass::Defin, "and (iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "xor iyh" },
	{ Z80InstructionClass::Unoff, "xor iyl" },
	{ Z80InstructionClass::Defin, "xor (iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xB0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "or iyh" },
	{ Z80InstructionClass::Unoff, "or iyl" },
	{ Z80InstructionClass::Defin, "or (iy%)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Unoff, "cp iyh" },
	{ Z80InstructionClass::Unoff, "cp iyl" },
	{ Z80InstructionClass::Defin, "cp (iy%)" },
	{ Z80InstructionClass::Undef, "" },


	// 0xC0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "CB" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xD0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xE0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "pop iy" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ex (sp),iy" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "push iy" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "jp (iy)" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },


	// 0xF0
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Defin, "ld sp,iy" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },
	{ Z80InstructionClass::Undef, "" },

	};



	//
	// IY bit instructions (FDCB)
	//

	Disasm  g_DisassemblyFDCB[] = {

	// 0x00
	{ Z80InstructionClass::Unoff, "rlc (iy%),b" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),c" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),d" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),e" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),h" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),l" },
	{ Z80InstructionClass::Defin, "rlc (iy%)" },
	{ Z80InstructionClass::Unoff, "rlc (iy%),a" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),b" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),c" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),d" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),e" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),h" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),l" },
	{ Z80InstructionClass::Defin, "rrc (iy%)" },
	{ Z80InstructionClass::Unoff, "rrc (iy%),a" },


	// 0x10
	{ Z80InstructionClass::Unoff, "rl (iy%),b" },
	{ Z80InstructionClass::Unoff, "rl (iy%),c" },
	{ Z80InstructionClass::Unoff, "rl (iy%),d" },
	{ Z80InstructionClass::Unoff, "rl (iy%),e" },
	{ Z80InstructionClass::Unoff, "rl (iy%),h" },
	{ Z80InstructionClass::Unoff, "rl (iy%),l" },
	{ Z80InstructionClass::Defin, "rl (iy%)" },
	{ Z80InstructionClass::Unoff, "rl (iy%),a" },
	{ Z80InstructionClass::Unoff, "rr (iy%),b" },
	{ Z80InstructionClass::Unoff, "rr (iy%),c" },
	{ Z80InstructionClass::Unoff, "rr (iy%),d" },
	{ Z80InstructionClass::Unoff, "rr (iy%),e" },
	{ Z80InstructionClass::Unoff, "rr (iy%),h" },
	{ Z80InstructionClass::Unoff, "rr (iy%),l" },
	{ Z80InstructionClass::Defin, "rr (iy%)" },
	{ Z80InstructionClass::Unoff, "rr (iy%),a" },


	// 0x20
	{ Z80InstructionClass::Unoff, "sla (iy%),b" },
	{ Z80InstructionClass::Unoff, "sla (iy%),c" },
	{ Z80InstructionClass::Unoff, "sla (iy%),d" },
	{ Z80InstructionClass::Unoff, "sla (iy%),e" },
	{ Z80InstructionClass::Unoff, "sla (iy%),h" },
	{ Z80InstructionClass::Unoff, "sla (iy%),l" },
	{ Z80InstructionClass::Defin, "sla (iy%)" },
	{ Z80InstructionClass::Unoff, "sla (iy%),a" },
	{ Z80InstructionClass::Unoff, "sra (iy%),b" },
	{ Z80InstructionClass::Unoff, "sra (iy%),c" },
	{ Z80InstructionClass::Unoff, "sra (iy%),d" },
	{ Z80InstructionClass::Unoff, "sra (iy%),e" },
	{ Z80InstructionClass::Unoff, "sra (iy%),h" },
	{ Z80InstructionClass::Unoff, "sra (iy%),l" },
	{ Z80InstructionClass::Defin, "sra (iy%)" },
	{ Z80InstructionClass::Unoff, "sra (iy%),a" },


	// 0x30
	{ Z80InstructionClass::Unoff, "sll (iy%),b" },
	{ Z80InstructionClass::Unoff, "sll (iy%),c" },
	{ Z80InstructionClass::Unoff, "sll (iy%),d" },
	{ Z80InstructionClass::Unoff, "sll (iy%),e" },
	{ Z80InstructionClass::Unoff, "sll (iy%),h" },
	{ Z80InstructionClass::Unoff, "sll (iy%),l" },
	{ Z80InstructionClass::Unoff, "sll (iy%)" },
	{ Z80InstructionClass::Unoff, "sll (iy%),a" },
	{ Z80InstructionClass::Unoff, "srl (iy%),b" },
	{ Z80InstructionClass::Unoff, "srl (iy%),c" },
	{ Z80InstructionClass::Unoff, "srl (iy%),d" },
	{ Z80InstructionClass::Unoff, "srl (iy%),e" },
	{ Z80InstructionClass::Unoff, "srl (iy%),h" },
	{ Z80InstructionClass::Unoff, "srl (iy%),l" },
	{ Z80InstructionClass::Defin, "srl (iy%)" },
	{ Z80InstructionClass::Unoff, "srl (iy%),a" },


	// 0x40
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 1,(iy%)" },


	// 0x50
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 3,(iy%)" },


	// 0x60
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 5,(iy%)" },


	// 0x70
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },
	{ Z80InstructionClass::Defin, "bit 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "bit 7,(iy%)" },


	// 0x80
	{ Z80InstructionClass::Unoff, "res 0,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 0,(iy%),a" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 1,(iy%),a" },


	// 0x90
	{ Z80InstructionClass::Unoff, "res 2,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 2,(iy%),a" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 3,(iy%),a" },


	// 0xA0
	{ Z80InstructionClass::Unoff, "res 4,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 4,(iy%),a" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 5,(iy%),a" },


	// 0xB0
	{ Z80InstructionClass::Unoff, "res 6,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 6,(iy%),a" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),b" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),c" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),d" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),e" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),h" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),l" },
	{ Z80InstructionClass::Defin, "res 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "res 7,(iy%),a" },


	// 0xC0
	{ Z80InstructionClass::Unoff, "set 0,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 0,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 0,(iy%),a" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 1,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 1,(iy%),a" },


	// 0xD0
	{ Z80InstructionClass::Unoff, "set 2,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 2,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 2,(iy%),a" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 3,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 3,(iy%),a" },


	// 0xE0
	{ Z80InstructionClass::Unoff, "set 4,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 4,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 4,(iy%),a" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 5,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 5,(iy%),a" },


	// 0xF0
	{ Z80InstructionClass::Unoff, "set 6,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 6,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 6,(iy%),a" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),b" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),c" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),d" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),e" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),h" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),l" },
	{ Z80InstructionClass::Defin, "set 7,(iy%)" },
	{ Z80InstructionClass::Unoff, "set 7,(iy%),a" },

	};


};  // end namespace