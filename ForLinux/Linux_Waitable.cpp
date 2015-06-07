
#include <stdexcept>
#include <pthread.h>
#include "Waitable.h"



class EventOnLinux
{
public:

    EventOnLinux();
    ~EventOnLinux();

    void Reset();
    void Signal();
    void Wait();
    bool IsSignalled() const;

private:

    volatile bool    _signalled;
    pthread_mutex_t  _mutexObject;
    pthread_cond_t   _condition;

};



EventOnLinux::EventOnLinux()
    : _signalled(false) // This synchronisation object is constructed in the "non-signalled" state.
{
    if( pthread_mutex_init( &_mutexObject, nullptr ) != 0 )
    {
        throw std::runtime_error( "Failed to create event mutex object." );
    }

    if( pthread_cond_init( &_condition, nullptr ) != 0 )
    {
        pthread_mutex_destroy( &_mutexObject );
        throw std::runtime_error( "Failed to create event condition object." );
    }
}



EventOnLinux::~EventOnLinux()
{
    pthread_mutex_destroy( &_mutexObject );
    pthread_cond_destroy( &_condition );
}



void EventOnLinux::Reset()
{
    pthread_mutex_lock( &_mutexObject );
    _signalled = false;
    pthread_mutex_unlock( &_mutexObject );
}



void EventOnLinux::Signal()
{
    pthread_mutex_lock( &_mutexObject );
    _signalled = true;
    pthread_cond_broadcast( &_condition );
    pthread_mutex_unlock( &_mutexObject );
}



void EventOnLinux::Wait()
{
    pthread_mutex_lock( &_mutexObject );
    while( ! _signalled )
    {
        pthread_cond_wait( &_condition, &_mutexObject );
    }
    pthread_mutex_unlock( &_mutexObject );
}



bool EventOnLinux::IsSignalled() const
{
    return _signalled;
}





#define IMPLEMENTATION ((EventOnLinux *&) _platformSpecificImplementation)


Waitable::Waitable()
{
    // This synchronisation object is constructed in the "non-signalled" state.
    _platformSpecificImplementation = new EventOnLinux();
}



void Waitable::Reset()
{
    // Set this synchronisation object to the "non-signalled" state.
    // Then, anyone calling Wait() will block.
    if( _platformSpecificImplementation != nullptr )
    {
        IMPLEMENTATION->Reset();
        return;
    }
    throw std::runtime_error( "Failed to reset event object." );
}



void Waitable::Signal()
{
    // Set this synchronisation object to the "signalled" state.
    // - Then, anyone calling Wait() will not block.
    // - Then, anyone who has already called Wait() will be released.
    if( _platformSpecificImplementation != nullptr )
    {
        IMPLEMENTATION->Signal();
        return;
    }
    throw std::runtime_error( "Failed to set event object." );
}



void Waitable::Wait() const
{
    // Block calling thread until the object is "signalled".
    // No-operation if this object is already signalled.
    if( _platformSpecificImplementation != nullptr )
    {
        IMPLEMENTATION->Wait();
        return;
    }
    throw std::runtime_error( "Failed to wait on event object." );
}



bool Waitable::IsSignalled() const
{
    // Determines whether this object is signalled, without blocking caller.
    if( _platformSpecificImplementation != nullptr )
    {
        return IMPLEMENTATION->IsSignalled();
    }
    throw std::runtime_error( "Failed to test whether event object is signalled." );
}



void Waitable::CleanupResources()
{
    if( _platformSpecificImplementation != nullptr )
    {
        delete IMPLEMENTATION;
        _platformSpecificImplementation = nullptr;
    }
}



