
#pragma once


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WAITABLE SYNCHRONISATION OBJECT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

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

