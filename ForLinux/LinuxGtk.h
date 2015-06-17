//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2015  Jonathan Markland
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

#include <stdexcept>
#include "gtk/gtk.h"


        // These were added to check return statuses from the GTK "C" API.
        // Not much of that API is used, so I am happy to do this.


template<typename OBJECT>
OBJECT *ThrowIfNull( OBJECT *objectAddress, const char *errorMessage )
{
    if( objectAddress != nullptr )
    {
        return objectAddress;
    }
    throw std::runtime_error( errorMessage );
}



template<typename OBJECT>
OBJECT *ThrowIfNull( OBJECT *objectAddress )
{
    return ThrowIfNull( objectAddress, "Out of memory" );
}



template<typename INTEGER_TYPE>
INTEGER_TYPE  ThrowLEZ( INTEGER_TYPE value, const char *errorMessage )
{
    if( value > 0 )
    {
        return value;
    }
    throw std::runtime_error( errorMessage );
}



template<typename INTEGER_TYPE>
INTEGER_TYPE  ThrowLEZ( INTEGER_TYPE value )
{
    return ThrowLEZ( value, "Function call failed with error code less than or equal to zero." );
}



