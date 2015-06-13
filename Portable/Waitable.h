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

