
#pragma once

#include <stdint.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WAITABLE SYNCHRONISATION OBJECT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace Jynx
{
	class Waitable   // Note:  Implementation is in the machine-specific source file folders!
	{
	public:

		Waitable();
			// This synchronisation object is constructed in the "non-signalled" state.
			// Throws std::exception if failed to create.

		inline ~Waitable() { Signal(); CleanupResources(); }
		
		void Reset();
			// Set this synchronisation object to the "non-signalled" state.
			// Then, anyone calling Wait() will block.
			// Throws std::exception if failed.

		void Signal();
			// Set this synchronisation object to the "signalled" state.
			// - Then, anyone calling Wait() will not block.
			// - Then, anyone who has already called Wait() will be released.
			// Throws std::exception if failed.

		void Wait() const;
			// Block calling thread until the object is "signalled".
			// No-operation if this object is already signalled.
			// Throws std::exception if failed.

		bool IsSignalled() const;
			// Determines whether this object is signalled, without blocking caller.
			// Throws std::exception if failed.

	private:

		void CleanupResources();

		void *_platformSpecificImplementation;

	};
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     THREAD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace Jynx
{
	typedef void (*ThreadFunction)( void *userObject );



	class Thread   // Note:  Implementation is in the machine-specific source file folders!
	{
	public:

		Thread();

		virtual ~Thread();
			// If thread was created, calls RequestTermination() then WaitForTermination()
		
		void CreateAndRun( ThreadFunction threadFunction, void *userObject );
			// Call this to create and run your thread.

		virtual void RequestTermination();   
			// Base class sets the ShouldTerminate() flag.  Derived may 
			// need to perform extra tasks eg: signal internal events.
			// - If you override this, be sure to override the destructor
			//   and call your RequestTermination() from that.

		void WaitForTermination();
			// Thread Join.
			// Allows external party to halt until this thread terminates.
			// This takes no action if the thread is already terminated.

		bool ShouldTerminate() const;
			// Asks whether this thread should terminate.
			// Intended to be called on the thread itself, if it wants to
			// support a synchronised 'exiting' feature.

		ThreadFunction GetThreadFunction() const { return _threadFunction; }
		void *GetUserObjectForThreadFunction() const { return _userObjectForThreadFunction; }

	private:

		ThreadFunction _threadFunction;
		void *_userObjectForThreadFunction;
		void *_platformSpecificImplementation;
		volatile bool _shouldTerminate;

	};
}




