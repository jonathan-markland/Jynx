
#pragma once

namespace Jynx
{
	typedef void (*IHostServicesForLynxEmulatorThreadFunction)( void *userObject );


	class IHostThread
	{
	public:
		virtual  void SignalToTerminateAndJoin() = 0;
		virtual  bool CanKeepRunning() = 0;
	};

}