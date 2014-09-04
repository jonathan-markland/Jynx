
#pragma once

namespace Jynx
{
	class Waitable   // Note:  Implementation is in the machine-specific source file folders!
	{
	public:

		Waitable();
			// This synchronisation object is constructed in the "non-signalled" state.

		inline ~Waitable() { Signal(); CleanupResources(); }
		
		void Reset();
			// Set this synchronisation object to the "non-signalled" state.
			// Then, anyone calling Wait() will block.

		void Signal();
			// Set this synchronisation object to the "signalled" state.
			// - Then, anyone calling Wait() will not block.
			// - Then, anyone who has already called Wait() will be released.

		void Wait() const;
			// Block calling thread until the object is "signalled".
			// No-operation if this object is already signalled.

		bool IsSignalled() const;
			// Determines whether this object is signalled, without blocking caller.

	private:

		void CleanupResources();

		void *_platformSpecificImplementation;

	};
}
