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

#include <algorithm>
#include "LynxRectangle.h"

namespace Jynx
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	//     RECTANGLES
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	LynxRectangle CentreRectangleInArea( int hostWidth, int hostHeight, int areaWidth, int areaHeight )
	{
		LynxRectangle resultRect;
		resultRect.left   = (hostWidth - areaWidth) / 2;
		resultRect.top    = (hostHeight - areaHeight) / 2;
		resultRect.right  = resultRect.left + areaWidth;
		resultRect.bottom = resultRect.top + areaHeight;
		return resultRect;
	}



	LynxRectangle GetSquarePixelsProjectionArea( int hostWidth, int hostHeight, int sourceSidePixels )
	{
		int smallestDimension = std::min( hostWidth, hostHeight );
		int zoomedSide = std::max(smallestDimension / sourceSidePixels, 1) * sourceSidePixels;
		return CentreRectangleInArea( hostWidth, hostHeight, zoomedSide, zoomedSide );
	}



	LynxRectangle GetBestFitProjectionArea( int hostWidth, int hostHeight )
	{
		int smallestDimension = std::min( hostWidth, hostHeight );
		return CentreRectangleInArea( hostWidth, hostHeight, smallestDimension, smallestDimension );
	}

} // end namespace Jynx
