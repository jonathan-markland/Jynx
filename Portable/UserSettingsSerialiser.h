
#pragma once

#include "IFileOpener.h"
#include "ILynxEmulator.h" // TODO:  only really need an enum


namespace Jynx
{
	namespace RenderStyle
	{
		enum Enum
		{
			FitToWindow, SquarePixels
		};
	}



	class UserSettings
	{
	public:

		// Load and save user settings.
		// - When loading it is desirable to have a separate store, in case of failures.
		//   We apply the store to the main system only when successfully loaded.
		// - Save may as well be in the same place for symmetry.

		explicit UserSettings( IFileOpener * );

		UserSettings(
			LynxMachineType::Enum  machineType,
			RenderStyle::Enum      renderStyle,
			bool                   soundEnable,
			uint32_t               cyclesPerTimeslice,
			bool                   tapeSounds,
			bool                   remExtensions );

		void SaveToFile( IFileOpener *, const std::string endOfLineSequence );

		LynxMachineType::Enum  GetMachineType()   const { return _machineType; }
		RenderStyle::Enum      GetRenderStyle()   const { return _renderStyle; }
		uint32_t               GetCyclesPerTimeslice() const { return _cyclesPerTimeslice; }
		bool                   GetSoundEnable()   const { return _soundEnable; }
		bool                   GetTapeSounds()    const { return _tapeSounds; }
		bool                   GetRemExtensions() const { return _remExtensions; }

	private:

		LynxMachineType::Enum  _machineType;
		RenderStyle::Enum      _renderStyle;
		bool                   _soundEnable;
		uint32_t               _cyclesPerTimeslice;
		bool                   _tapeSounds;
		bool                   _remExtensions;

		void SerialiseFields( ISerialiser *serialiser );

	};

}

