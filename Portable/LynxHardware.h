//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
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

#include <stdint.h>

#define LYNX_FRAMEBUF_WIDTH            256   // NOTE: These are not really changeable at all!  If we ever did the 128K machine, these would need to be variables anyway...
#define LYNX_FRAMEBUF_HEIGHT           256   // NOTE: These are not really changeable at all!

//
// DEVICE PORT
// ~~~~~~~~~~~
// "PORT 80" : Actual I/O space decoding is:  A15..A0 resp:  [xxxx xxxx 10xx x00x]   x=don't care
//

#define DEVICEPORT_INITIALISATION_VALUE  0x0C

#define DEVICEPORT_FRAME_BLANKING  0x80  // Must be kept zeroed
#define DEVICEPORT_LINE_BLANKING   0x40  // If set high, Z80 will be frozen until next line blanking period  [TODO: Or not in the case of this emulator, but then it doesn't emulate snow, either!]
#define DEVICEPORT_CPU_ACCESS      0x20  // Display sharing sync: Halt Z80 until end of scan line.  CPU gets access to video RAM when high.
#define DEVICEPORT_USE_ALT_GREEN   0x10  // Basic returns this to GREEN after a screen access
#define DEVICEPORT_NOT_CASEN_BANK3 0x08  // When high, reading and writing to GREEN and ALT GREEN is impossible.  GREEN & ALT GREEN are not shown
#define DEVICEPORT_NOT_CASEN_BANK2 0x04  // When high, reading and writing to RED and BLUE is impossible.  RED & BLUE are not shown
#define DEVICEPORT_CASSETTE_MOTOR  0x02  // Casette motor on/off
#define DEVICEPORT_SPEAKER         0x01  // Speaker on/off

#define DEVICEPORT_DECODING_MASK   0x00C6
#define DEVICEPORT_KEYBOARD_DECODING_MASK   0x0FC6

//
// BANK SELECT PORT
// ~~~~~~~~~~~~~~~~
// "PORT FFFF" : Actual I/O space decoding is:  A15..A0 resp:  [xx1x xxxx x111 1111]   x=don't care
//

#define BANKPORT_INITIALISATION_VALUE  0x00

#define BANKPORT_RDEN4       0x80  // D7 =  RDEN4
#define BANKPORT_RDEN2_3     0x40  // D6 =  RDEN2_3
#define BANKPORT_NOT_RDEN1   0x20  // D5 = !RDEN1
#define BANKPORT_NOT_RDEN0   0x10  // D4 = !RDEN0
#define BANKPORT_WREN4       0x08  // D3 =  WREN4
#define BANKPORT_WREN3       0x04  // D2 =  WREN3
#define BANKPORT_WREN2       0x02  // D1 =  WREN2
#define BANKPORT_NOT_WREN1   0x01  // D0 = !WREN1

#define BANKPORT_DECODING_MASK   0x207F

//
// CRTC PORT
// ~~~~~~~~~
// 0x86 and 0x87    // TODO:  CRTC is not emulated.
//

#define CRTCPORT_DECODING_MASK   0x00C7

namespace Jynx
{
	typedef uint8_t CHIP[8192];
	typedef CHIP *CHIP_PTR;
	typedef CHIP_PTR ADDRESS_SPACE[8];

} // end namespace Jynx
