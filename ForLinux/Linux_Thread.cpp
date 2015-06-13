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



#include <stdexcept>
#include <mutex>
#include <pthread.h>
#include "Thread.h"
#include <assert.h>
#include "unistd.h"


// The _platformSpecificImplementation variable shall be a pointer to a LinuxThreadImplementation  (may revise)


struct LinuxThreadImplementation
{
    LinuxThreadImplementation() : OsThreadID(0) {}
    pthread_t       OsThreadID;
    std::once_flag  JoinOnceFlag;
};



// enum { DefaultStackSizeBytes = 1048576 };  // TODO: Use this?  Need to set attributes.



void *MainFunctionOfNewThread( void *parameter )
{
    // Call derived class to perform thread tasks.
    auto thisThread = reinterpret_cast<Thread *>( parameter );
    thisThread->private_SetThreadExitResult(
        thisThread->GetThreadFunction()(
            thisThread->GetUserObjectForThreadFunction() ) );
    return nullptr; // not used
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
    if( _platformSpecificImplementation != nullptr )
    {
        RequestTermination();
        WaitForTermination();
        delete (LinuxThreadImplementation *) _platformSpecificImplementation;
        _platformSpecificImplementation = nullptr;
    }
}



void Thread::CreateAndRun( ThreadFunction threadFunction, void *userObject )
{
    if( _platformSpecificImplementation == nullptr )
    {
        _threadFunction = threadFunction;
        _userObjectForThreadFunction = userObject;

        auto newImplementation = new LinuxThreadImplementation();

        auto result1 = pthread_create( &newImplementation->OsThreadID, NULL, &MainFunctionOfNewThread, this );
        if( result1 != 0 )
        {
            delete newImplementation;
            assert( _platformSpecificImplementation == nullptr );
            throw std::runtime_error( "The system is out of resources, cannot create a new thread." );
        }

        _platformSpecificImplementation = newImplementation;
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



/*
   After a successful call to pthread_join(), the caller is guaranteed
   that the target thread has terminated.

   Joining with a thread that has previously been joined results in
   undefined behavior.

   Failure to join with a thread that is joinable (i.e., one that is not
   detached), produces a "zombie thread".  Avoid doing this, since each
   zombie thread consumes some system resources, and when enough zombie
   threads have accumulated, it will no longer be possible to create new
   threads (or processes).
*/

void *Thread::WaitForTermination()
{
    if( _platformSpecificImplementation != nullptr )
    {
        //First thread into here must do the JOIN call, and thus the exit status will be available.
        //Any others must block until first thread has done this.
        //All subsequent threads in will never attempt the JOIN but will just return the previously determined exit status.

        auto implementationObject = (LinuxThreadImplementation *) _platformSpecificImplementation;

        void *threadResult = nullptr; // not used - thread result returned by different mechanism
        int result1 = 0; // ie: the pthread_join() success status

        std::call_once( implementationObject->JoinOnceFlag, [&]()
        {
            result1 = pthread_join( implementationObject->OsThreadID, &threadResult );
        } );

        if( result1 != 0 )
        {
            throw std::runtime_error( "Failed to wait for termination of other thread." );
        }
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
    usleep( countMilliseconds * 1000 );
}






