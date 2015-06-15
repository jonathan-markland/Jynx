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


// Exception wrapper to handle policy for un-handlable (certainly unexpected!) exceptions.
// Typically used to defend C function frames from being wound-back, as this is undefined behaviour.
// An example would be GTK event handlers.  Also now used in Windows, in the Model class, to handle both at once!

template<typename OPERATION>
void DoWithTerminationOnException( OPERATION operation )
{
    try
    {
        operation();
    }
    catch( ... )
    {
        exit(1);
        throw; // never executed
    }
}
