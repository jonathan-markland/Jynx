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

#include "UserSettingsSerialiser.h"
#include "FileSerialiser.h"
#include "ISerialiser.h"


namespace Jynx
{
	UserSettings::UserSettings( IFileOpener *fileOpener )
	{
		InputFileSerialiser  inputFile( fileOpener );
		SerialiseFields( &inputFile );
	}



	UserSettings::UserSettings(
		LynxMachineType::Enum  machineType,
		RenderStyle::Enum      renderStyle,
		bool                   soundEnable,
		bool                   fullScreenEnable,
		uint32_t               speedPercentage,
		bool                   tapeSounds,
		bool                   remExtensions,
		bool                   maxSpeedCassette,
		bool                   maxSpeedConsole,
		bool                   maxSpeedAlways,
		LynxColourSet::Enum    colourSet )
			: _machineType( machineType )
			, _renderStyle( renderStyle )
			, _soundEnable( soundEnable )
			, _fullScreenEnable( fullScreenEnable )
			, _speedPercentage( speedPercentage )
			, _tapeSounds( tapeSounds )
			, _remExtensions( remExtensions )
			, _maxSpeedCassette(maxSpeedCassette)
			, _maxSpeedConsole(maxSpeedConsole)
			, _maxSpeedAlways(maxSpeedAlways)
			, _colourSet(colourSet)
	{
	}



	void UserSettings::SaveToFile( IFileOpener *fileOpener, const std::string endOfLineSequence )
	{
		OutputFileSerialiser  outputFile( fileOpener, endOfLineSequence );
		SerialiseFields( &outputFile );
	}



	void UserSettings::SerialiseFields( ISerialiser *serialiser )
	{
		int versionNumber = 5; // Current Version Number (For when writing).

		serialiser->Field(     "FileVersion",   versionNumber );
		serialiser->FieldEnum( "MachineType",   _machineType );
		serialiser->FieldEnum( "RenderStyle",   _renderStyle );
		serialiser->Field(     "SoundEnable",   _soundEnable );

		if( versionNumber >= 2 )
		{
			serialiser->Field( "FullScreenEnable",   _fullScreenEnable );
		}
		else
		{
			// Only executed if reading downlevel version.
			_fullScreenEnable = false;
		}

		if( versionNumber >= 5 )
        {
            serialiser->FieldEnum( "SpeedPercentage", _speedPercentage );  // Serialising the cycle counter was a stupid idea!
        }
        else
        {
			// Only executed if reading downlevel version.
			uint32_t  cyclesPerTimeslice = 0;
            serialiser->FieldEnum( "CyclesPerTimeslice", cyclesPerTimeslice );

            // Initialise _speedPercentage from the old serialised values:
            switch( cyclesPerTimeslice )
            {
                case  35000: _speedPercentage =  50;  break;
                case 140000: _speedPercentage = 200;  break;
                case 280000: _speedPercentage = 400;  break;
                case 560000: _speedPercentage = 800;  break;
                default:     _speedPercentage = 100;  break;
            }
        }

		serialiser->Field(     "TapeSounds",    _tapeSounds );
		serialiser->Field(     "RemExtensions", _remExtensions );

		if( versionNumber >= 3 )
		{
			serialiser->Field( "MaxSpeedCassette", _maxSpeedCassette );
			serialiser->Field( "MaxSpeedConsole",  _maxSpeedConsole );
			serialiser->Field( "MaxSpeedAlways",   _maxSpeedAlways );
		}
		else
		{
			// Only executed if reading downlevel version.
			_maxSpeedCassette = true;
			_maxSpeedConsole  = false;
			_maxSpeedAlways   = false;
		}

		if( versionNumber >= 4 )
		{
			serialiser->FieldEnum( "ColourSet", _colourSet );
		}
		else
		{
			// Only executed if reading downlevel version.
			_colourSet = LynxColourSet::NormalRGB;
		}
	}

}

