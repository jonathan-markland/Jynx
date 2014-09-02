
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
			At50  =  40000,
			At100 =  70000,    // 80,000 * 50 timeslices per second = 4.00 Mhz
			At200 = 160000,
			At400 = 320000,
			At800 = 640000,
		};
	};



}