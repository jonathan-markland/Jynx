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
// Z80 Instruction set timing tables -- see source file.
//

#pragma once

#include <stdint.h>

namespace JynxZ80
{

	extern uint8_t Z80MainTimings[256];
	extern uint8_t Z80DDFDMainTimings[256];
	extern uint8_t Z80CBTimings[256];
	extern uint8_t Z80EDTimings[256];

} // end namespace
