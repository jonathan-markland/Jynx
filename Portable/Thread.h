//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2015  Jonathan Markland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//		jynx_emulator {at} yahoo {dot} com
//

#pragma once

#include <stdint.h>

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

		static void SleepMilliseconds( uint32_t countMilliseconds );

	public:

		void private_SetThreadExitResult( void *exitResult )     { _threadExitResult = exitResult; }  // TODO: Not ideal to have this public

	private:

		ThreadFunction _threadFunction;
		void *_userObjectForThreadFunction;
		void *_platformSpecificImplementation;
		void *_threadExitResult;
		volatile bool _shouldTerminate;

	};



