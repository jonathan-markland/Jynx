
#include "stdafx.h"
#include <stdexcept>
#include "Thread.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     THREAD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	// The _platformSpecificImplementation variable shall hold the operating system handle directly.


	enum { DefaultStackSizeBytes = 1048576 };



	DWORD WINAPI MainFunctionOfNewThread( LPVOID parameter ) // static member function
	{
		// Call derived class to perform thread tasks.
		auto thisThread = reinterpret_cast<Thread *>( parameter );
		thisThread->private_SetThreadExitResult(
			thisThread->GetThreadFunction()(
				thisThread->GetUserObjectForThreadFunction() ) );
		return 0; // not used
	}



	Thread::Thread()
		: _threadFunction(nullptr)
		, _userObjectForThreadFunction(nullptr)
		, _platformSpecificImplementation(nullptr)
		, _threadExitResult(nullptr)
		, _shouldTerminate(false)
	{
	}



	Thread::~Thread()
	{
		if( _platformSpecificImplementation != NULL )
		{
			RequestTermination();
			WaitForTermination();
			CloseHandle( (HANDLE) _platformSpecificImplementation );
			_platformSpecificImplementation = NULL;
		}
	}



	void Thread::CreateAndRun( ThreadFunction threadFunction, void *userObject )
	{
		if( _platformSpecificImplementation == NULL )
		{
			_threadFunction = threadFunction;
			_userObjectForThreadFunction = userObject;

			DWORD threadId = 0; // not massively important to store this

			_platformSpecificImplementation = ::CreateThread(  // TODO: _beginthreadex() here would avoid an 80-byte leak with the MSVCRT library
				NULL,
				DefaultStackSizeBytes,
				&MainFunctionOfNewThread,
				this,
				STACK_SIZE_PARAM_IS_A_RESERVATION,
				&threadId );

			if( _platformSpecificImplementation == NULL )
			{
				throw std::runtime_error( "The system is out of resources, cannot create a new thread." );
			}
		}
		else
		{
			throw std::runtime_error( "Thread object is already created." );
		}
	}



	void Thread::RequestTermination()
	{
		_shouldTerminate = true;
	}



	void *Thread::WaitForTermination()
	{
		if( _platformSpecificImplementation != NULL )
		{
			auto waitResult = ::WaitForSingleObject( (HANDLE) _platformSpecificImplementation, INFINITE );
			if( waitResult == WAIT_FAILED )
			{
				throw std::runtime_error( "Failed to wait for termination of other thread." );
			}
			// WAIT_ABANDONED won't happen because we are not waiting on a MUTEX object here.
		}
		else
		{
			throw std::runtime_error( "No thread created." );
		}
		return _threadExitResult;
	}



	bool Thread::ShouldTerminate() const
	{
		return _shouldTerminate;
	}



    void Thread::SleepMilliseconds( uint32_t countMilliseconds ) // static
    {
        ::Sleep( countMilliseconds );  // Assumes externally configured increase in resolution.
    }







