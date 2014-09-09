
#pragma once

namespace Jynx
{
	namespace LynxTapeMode
	{
		enum Enum
		{
			SavingPermitted, LoadingPermitted
		};
	}



	namespace LynxMachineType
	{
		enum Enum
		{
			LYNX_48K, LYNX_96K
		};
	}



	namespace LynxZ80Cycles
	{
		enum Enum
		{
			// The following should be 40,000  80,000  160,000  320,000  640,000   resp.
			// Where: 80,000 * 50 timeslices per second = 4.00 Mhz.
			// ** HOWEVER ** Switching in the Lynx ROM causes slow-down of about this much.
			// I ascertained this (by ear!) with a Youtube video of a real Lynx, listening to the power-on BEEP!!
			// I am trying to determine the correct values for this. 
			// TODO:  ROM slow-down only occurs with the ROM switched in anyway.  We need the speeds mention above with the ROM switched OUT.

			At50  =  35000,
			At100 =  70000,    
			At200 = 140000,
			At400 = 280000,
			At800 = 560000,
		};
	};



	namespace LynxColourSet
	{
		enum Enum
		{
			NormalRGB, GreenOnly, Level9, BlackAndWhiteTV, GreenScreenMonitor
		};
	}

}