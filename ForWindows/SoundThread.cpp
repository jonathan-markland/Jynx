//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//


#include "stdafx.h"
#include "SoundThread.h"



SoundThread::SoundThread( uint32_t numSamplesPerBuffer )
	: _hWndEmulatorMainForm( NULL )  // filled by SetEmulatorWindowHandle(), as soon as the window handle is known.
{
	// (Called on main thread)

	_soundBuffer.reserve( numSamplesPerBuffer );

	for( uint32_t i=0; i < numSamplesPerBuffer; i++ )
	{
		_soundBuffer.push_back( 0 );
	}

	_emulatorIsFillingNextBufferEvent = ::CreateEvent( NULL, TRUE, TRUE, NULL );
	if( _emulatorIsFillingNextBufferEvent == NULL )
	{
		throw std::exception( "The system is out of resources -- cannot create an event object" );
	}
}



SoundThread::~SoundThread()
{
	// (Called on main thread)
	RequestTermination();
}



void SoundThread::SetEmulatorWindowHandle( HWND hWndEmulatorMainForm )
{
	// (Called on main thread)
	// Because it can't be passed to the constructor -- not known at the time.
	_hWndEmulatorMainForm = hWndEmulatorMainForm;
}



void SoundThread::RequestTermination()
{
	// (Callable on any thread)
	NotifyEmulatorFinishedFillingSoundBuffer();  // need this in case SoundThread is about to block!
	Thread::RequestTermination(); // call base 
}



uint16_t *SoundThread::GetBaseAddressOfSharedSoundBuffer()
{
	// (Callable on any thread)
	return &(*_soundBuffer.begin());
}



size_t SoundThread::GetSizeOfSharedSoundBufferInSamples()
{
	// (Callable on any thread)
	return _soundBuffer.size();
}



int32_t SoundThread::ThreadMain()
{
	// (Called on sound thread only)

	auto bufferSizeBytes = _soundBuffer.size() * 2;

	libWinApi::WaveOutputStream  waveOutStream( 44100, 2, 1, 3, (int) bufferSizeBytes );

	auto bufferAddress = (PBYTE) GetBaseAddressOfSharedSoundBuffer();

	while( ! ShouldTerminate() )
	{
		auto resetResult = ::ResetEvent( _emulatorIsFillingNextBufferEvent );
		assert( resetResult == TRUE );
		PostMessage( _hWndEmulatorMainForm, WM_ADVANCE_EMULATION, 0, 0 );
		::WaitForSingleObject( _emulatorIsFillingNextBufferEvent, INFINITE );
		if( ShouldTerminate() ) break;
		waveOutStream.Write( bufferAddress, (int) bufferSizeBytes );
	}

	return 0;
}



void SoundThread::NotifyEmulatorFinishedFillingSoundBuffer()
{
	// (Callable on any thread)
	auto setResult = ::SetEvent( _emulatorIsFillingNextBufferEvent );
	assert( setResult == TRUE );
}
