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

//
// Z80ImplementationBaseClass
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// The Z80 needs to call user-defined functions to perform regular 
// address-space READ / WRITE, and I/O space reads and writes ("IN" / "OUT").
//
//    ** The user must supply these functions **
//
// The most obvious(!) design solution is to have a Z80 base class with 
// pure virtual functions for the above.  The user derives his own class and
// implements the functions.
//
// However, it may be desireable to have choice over the performance 
// of the function call dispatch.
//
// C++ has two function call dispatch mechanisms:
//   - Statically dispatched call   : The function to be called is identified at compile-time.
//   - Dynamically dispatched call  : The function to be called is determined at run-time.  A "virtual" function call.
//
// Further to the above -
//
// Statically-dispatched calls may be implemented with:
//    - inlining     (no stack frame building overhead)
//    - non-inline   (a stack frame is built to perform the call)
//
// Statically-dispatched calls may also choose:
//    - to implement the singleton pattern:  The function has no "this" pointer operand (C++ "static" keyword).
//    - to be a "member" function:           The function is passed the address of an object as the "this" pointer operand.
//
// I wanted to have a way of allowing a user to decide.
//
// This is a bit grubby, but basically the strategy is:  ** The user provides the BASE CLASS **.
// You do this by changing this file only.
//

/*

// Example for inlining:
// (Your implementation of these functions may need to up-cast to "class Z80", unfortunately).

class Z80ImplementationBaseClass
{
protected:
	inline void GuestWrite(  uint16_t address, uint8_t value );
	inline void GuestWriteIOSpace( uint16_t portNumber, uint8_t value );
	inline uint8_t GuestRead( uint16_t address );
	inline uint8_t GuestReadIOSpace( uint16_t port );
};

// Example for static dispatch with 'this' pointer.
// (Your implementation of these functions may need to up-cast to "class Z80", unfortunately).

class Z80ImplementationBaseClass
{
protected:
	void GuestWrite(  uint16_t address, uint8_t value );
	void GuestWriteIOSpace( uint16_t portNumber, uint8_t value );
	uint8_t GuestRead( uint16_t address );
	uint8_t GuestReadIOSpace( uint16_t port );
};

// Example for singleton pattern

class Z80ImplementationBaseClass
{
protected:
	static void GuestWrite(  uint16_t address, uint8_t value );
	static void GuestWriteIOSpace( uint16_t portNumber, uint8_t value );
	static uint8_t GuestRead( uint16_t address );
	static uint8_t GuestReadIOSpace( uint16_t port );
};

// Example for pure virtual functions.
// Then you derive from "class Z80", and implement these.
// You can then have as many Z80 instances as you want.
// And as many emulator types as you want!

class Z80ImplementationBaseClass
{
protected:
	virtual void GuestWrite(  uint16_t address, uint8_t value ) = 0;
	virtual void GuestWriteIOSpace( uint16_t portNumber, uint8_t value ) = 0;
	virtual uint8_t GuestRead( uint16_t address ) = 0;
	virtual uint8_t GuestReadIOSpace( uint16_t port ) = 0;
};

*/



//
// For Jonathan's Camputer's Lynx emulator only:
//

#include <stdint.h>
#include <string>

// I was experimenting with these on the MSVC compiler:
#ifdef _MSC_VER
#define NO_INLINE        __declspec(noinline)
#else
#define NO_INLINE        
#endif


namespace JynxZ80
{

	class Z80ImplementationBaseClass
	{
	public:
		NO_INLINE  static void GuestWrite(  uint16_t address, uint8_t value );
		NO_INLINE  static void GuestWriteIOSpace( uint16_t portNumber, uint8_t value );
		NO_INLINE  static uint8_t GuestRead( uint16_t address );
		NO_INLINE  static uint8_t GuestReadIOSpace( uint16_t port );
		NO_INLINE  static void OnAboutToBranch();
	};

}

#undef NO_INLINE
