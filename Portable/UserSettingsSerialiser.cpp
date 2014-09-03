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
		uint32_t               cyclesPerTimeslice,
		bool                   tapeSounds,
		bool                   remExtensions )
			: _machineType( machineType )
			, _renderStyle( renderStyle )
			, _soundEnable( soundEnable )
			, _cyclesPerTimeslice( cyclesPerTimeslice )
			, _tapeSounds( tapeSounds )
			, _remExtensions( remExtensions )
	{
	}



	void UserSettings::SaveToFile( IFileOpener *fileOpener, const std::string endOfLineSequence )
	{
		OutputFileSerialiser  outputFile( fileOpener, endOfLineSequence );
		SerialiseFields( &outputFile );
	}



	void UserSettings::SerialiseFields( ISerialiser *serialiser )
	{
		int versionNumber = 1; // TODO: Sort out version on the next version!
		serialiser->Field(     "FileVersion",   versionNumber );
		serialiser->FieldEnum( "MachineType",   _machineType );
		serialiser->FieldEnum( "RenderStyle",   _renderStyle );
		serialiser->Field(     "SoundEnable",   _soundEnable );
		serialiser->FieldEnum( "CyclesPerTimeslice",   _cyclesPerTimeslice );
		serialiser->Field(     "TapeSounds",    _tapeSounds );
		serialiser->Field(     "RemExtensions", _remExtensions );
	}


}

