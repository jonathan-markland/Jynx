
#pragma once

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     THREAD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	typedef void *(*ThreadFunction)( void *userObject );
		// The return value is the exit value of the thread, if you require such a thing.
		// The thread exit value can be read with WaitForTermination().



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

		void *WaitForTermination();
			// Allows any thread to halt until this thread terminates.
			// This takes no action if the thread is already terminated.
			// Otherwise known as "Thread Join".
			// Returns the value that the thread returns from its ThreadFunction.

		bool ShouldTerminate() const;
			// Asks whether this thread should terminate.
			// Intended to be called on the thread itself, if it wants to
			// support a synchronised 'exiting' feature.

		ThreadFunction GetThreadFunction() const       { return _threadFunction; }
		void *GetUserObjectForThreadFunction() const   { return _userObjectForThreadFunction; }

	public:

		void private_SetThreadExitResult( void *exitResult )     { _threadExitResult = exitResult; }  // TODO: Not ideal to have this public

	private:

		ThreadFunction _threadFunction;
		void *_userObjectForThreadFunction;
		void *_platformSpecificImplementation;
		void *_threadExitResult;
		volatile bool _shouldTerminate;

	};



