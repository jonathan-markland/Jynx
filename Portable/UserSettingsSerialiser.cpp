
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

