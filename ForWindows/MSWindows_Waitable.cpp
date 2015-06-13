
#include "stdafx.h"
#include <stdexcept>
#include "Waitable.h"

	Waitable::Waitable()
	{
		// This synchronisation object is constructed in the "non-signalled" state.
		_platformSpecificImplementation = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		if( _platformSpecificImplementation == NULL )
		{
			throw std::runtime_error( "Cannot create Event object -- program cannot run." );
		}
	}



	void Waitable::Reset()
	{
		// Set this synchronisation object to the "non-signalled" state.
		// Then, anyone calling Wait() will block.
		if( ::ResetEvent( (HANDLE) _platformSpecificImplementation ) != 0 ) return;
		throw std::runtime_error( "ResetEvent() call failed unexpectedly." );
	}



	void Waitable::Signal()
	{
		// Set this synchronisation object to the "signalled" state.
		// - Then, anyone calling Wait() will not block.
		// - Then, anyone who has already called Wait() will be released.
		if( ::SetEvent( (HANDLE) _platformSpecificImplementation ) != 0 ) return;
		throw std::runtime_error( "SetEvent() call failed unexpectedly." );
	}



	void Waitable::Wait() const
	{
		// Block calling thread until the object is "signalled".
		// No-operation if this object is already signalled.
		if( ::WaitForSingleObject( (HANDLE) _platformSpecificImplementation, INFINITE ) == WAIT_OBJECT_0 ) return;
		throw std::runtime_error( "Unexpected result from WaitForSingleObject() during Wait()." );
	}



	bool Waitable::IsSignalled() const
	{
		// Determines whether this object is signalled, without blocking caller.
		auto result = ::WaitForSingleObject( (HANDLE) _platformSpecificImplementation, 0 );  // tests state without wait
		if( result == WAIT_OBJECT_0 ) return true;
		if( result == WAIT_TIMEOUT ) return false;
		throw std::runtime_error( "Unexpected result from WaitForSingleObject() while testing IsSignalled()." );
	}



	void Waitable::CleanupResources()
	{
		::CloseHandle( (HANDLE) _platformSpecificImplementation );
		_platformSpecificImplementation = NULL;
	}



