
#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

#include "WaveOutputStream.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND OUTPUT FOR WINDOWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	// With thanks to MSDN for wave play example.  (Since somewhat tuned)





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND BUFFERS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



class WaveBuffer
{
public:

	~WaveBuffer();
	bool Init( HWAVEOUT waveHandle, int sizeBytes );
	bool Write( const void *soundBlock, int sizeBytes, int &bytesWritten );
	void Flush();

private:

	WAVEHDR      _waveHeader;
	HWAVEOUT     _waveOutHandle;
	int          _bytesCount;

};








bool WaveBuffer::Init( HWAVEOUT waveHandle, int sizeBytes )
{
	_waveOutHandle  = waveHandle;
	_bytesCount = 0;

	// Allocate a buffer and initialize the header.
	_waveHeader.lpData = (LPSTR) LocalAlloc( LMEM_FIXED, sizeBytes );
	if( _waveHeader.lpData == nullptr )
	{
		return false;
	}
	_waveHeader.dwBufferLength  = sizeBytes;
	_waveHeader.dwBytesRecorded = 0;
	_waveHeader.dwUser = 0;
	_waveHeader.dwFlags = 0;
	_waveHeader.dwLoops = 0;
	_waveHeader.lpNext = 0;
	_waveHeader.reserved = 0;

	// Prepare it.
	waveOutPrepareHeader( waveHandle, &_waveHeader, sizeof(WAVEHDR) );
	return true;
}



WaveBuffer::~WaveBuffer()
{
	if( _waveHeader.lpData )
	{
		waveOutUnprepareHeader( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
		LocalFree( _waveHeader.lpData );
	}
}



void WaveBuffer::Flush()
{
	// ASSERT(_bytesCount != 0);
	_bytesCount = 0;
	waveOutWrite( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
}



bool WaveBuffer::Write( const void *soundBlock, int sizeBytes, int &bytesWritten)
{
	// ASSERT((DWORD)_bytesCount != _waveHeader.dwBufferLength);
	bytesWritten = min( (int)_waveHeader.dwBufferLength - _bytesCount, sizeBytes );
	CopyMemory( (PVOID)(_waveHeader.lpData + _bytesCount), soundBlock, bytesWritten );
	_bytesCount += bytesWritten;
	if( _bytesCount == (int)_waveHeader.dwBufferLength )
	{
		// Write it!
		_bytesCount = 0;
		waveOutWrite( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
		return true;
	}
	return false;
}













// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND HOST OS IMPLEMENTATION -- MS WINDOWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


class HostOS_WaveOutputStream
{
public:

    HostOS_WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames, uint32_t numBuffersInRing );
    ~HostOS_WaveOutputStream();
    void Write( const void *soundDataBlock, uint32_t numFrames );
    void Flush();
    void Reset();

private:

	const HANDLE   _semaphoreHandle;
	const int      _bufferCount;
	int            _currentBuffer;
	BOOL           _noBuffer;
	HWAVEOUT       _waveOutHandle;
	class WaveBuffer  *_waveBuffers;
	uint32_t       _channelCount;
	uint32_t       _bufferSizeFrames;

	static void CALLBACK WaveCallback( HWAVEOUT hWave, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );

	void WaitOnTheSemaphore();

};



HostOS_WaveOutputStream::HostOS_WaveOutputStream( uint32_t requestedRateHz, uint32_t channelCount, uint32_t requestedBufferSizeFrames, uint32_t numBuffersInRing )
	: _bufferCount( numBuffersInRing )
	, _currentBuffer(0)
	, _noBuffer(true)
	, _semaphoreHandle( CreateSemaphore( NULL, numBuffersInRing, numBuffersInRing, NULL ) )
	, _waveBuffers( new WaveBuffer[numBuffersInRing] )
	, _waveOutHandle( NULL )
	, _channelCount(channelCount)
	, _bufferSizeFrames(0)
	, _rateHz(0)
{
	auto bytesPerSample = 2; // not yet parameterisable

	WAVEFORMATEX  format;
	format.wFormatTag     = WAVE_FORMAT_PCM;
	format.nChannels      = channelCount;
	format.nSamplesPerSec = requestedRateHz;
	format.wBitsPerSample = bytesPerSample * 8;
	format.nBlockAlign    = bytesPerSample;
	format.cbSize         = 0;

	// Create wave device.
	auto openResult = waveOutOpen( &_waveOutHandle,
				WAVE_MAPPER,
				&format,
				(DWORD_PTR) HostOS_WaveOutputStream::WaveCallback,
				(DWORD_PTR) _semaphoreHandle,
				CALLBACK_FUNCTION);

    if( openResult != MMSYSERR_NOERROR )
    {
        throw std::runtime_error( "Failed to open the WAVE output with the desired parameters." );
    }

	// Initialize the wave buffers.
	auto bufferSizeBytes = requestedBufferSizeFrames * channelCount * bytesPerSample;
	for (uint32_t i = 0; i < numBuffersInRing; i++)
	{
		_waveBuffers[i].Init( _waveOutHandle, bufferSizeBytes );
	}

	_bufferSizeFrames = requestedBufferSizeFrames;
	_rateHz = requestedRateHz;
}



uint32_t HostOS_WaveOutputStream::GetRateHz() const
{
    return _rateHz;
}



uint32_t HostOS_WaveOutputStream::GetBufferSizeInFrames() const
{
    return _bufferSizeFrames;
}



void HostOS_WaveOutputStream::Write( const void *soundDataBlock, uint32_t numFrames )
{
	auto sizeBytes = numFrames * _channelCount * 2; // TODO : *2 is specific to 16-bits per sample

	while( sizeBytes != 0 )
	{
		// Get a buffer if necessary.
		if ( _noBuffer )
		{
			WaitOnTheSemaphore();
			_noBuffer = false;
		}

		// Write into a buffer.
		int nWritten;
		if (_waveBuffers[_currentBuffer].Write( soundDataBlock, sizeBytes, nWritten) )
		{
			_noBuffer = true;
			_currentBuffer = (_currentBuffer + 1) % _bufferCount;
			sizeBytes -= nWritten;
			((const uint8_t *&) soundDataBlock) += nWritten;
		}
		else
		{
			// ASSERT(nWritten == nBytes);
			break;
		}
	}
}



void CALLBACK HostOS_WaveOutputStream::WaveCallback( HWAVEOUT hWave, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 ) // static member function
{
	if( uMsg == WOM_DONE )
	{
		ReleaseSemaphore( (HANDLE) dwUser, 1, NULL );
	}
}



void HostOS_WaveOutputStream::Flush()
{
	if( ! _noBuffer )
	{
		_waveBuffers[_currentBuffer].Flush();
		_noBuffer = true;
		_currentBuffer = (_currentBuffer + 1) % _bufferCount;
	}
}



void HostOS_WaveOutputStream::Reset()
{
	waveOutReset(_waveOutHandle);
}






/* not used
void HostOS_WaveOutputStream::Wait()
{
	// Send any remaining buffers.
	Flush();

	// Wait for the buffers back.
	for (int i = 0; i < _bufferCount; i++)
	{
		WaitOnTheSemaphore();
	}

	LONG previousCount;
	ReleaseSemaphore( _semaphoreHandle, _bufferCount, &previousCount );
}
*/



void HostOS_WaveOutputStream::WaitOnTheSemaphore()
{
	WaitForSingleObject( _semaphoreHandle, INFINITE );
}



HostOS_WaveOutputStream::~HostOS_WaveOutputStream()
{
	// First, get the buffers back.
	waveOutReset(_waveOutHandle);

	// Free the buffers.
	delete [] _waveBuffers;

	// Close the wave device.
	waveOutClose(_waveOutHandle);

	// Free the semaphore.
	CloseHandle(_semaphoreHandle);
}







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND OUTPUT - INTERFACE DELEGATION - Same on all platforms, except for content of class HostOS_WaveOutputStream
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

WaveOutputStream::WaveOutputStream( uint32_t requestedRateHz, uint32_t channelCount, uint32_t requestedBufferSizeFrames, uint32_t numBuffersInRing )
{
    _hostImplementation = std::make_shared<HostOS_WaveOutputStream>( requestedRateHz, channelCount, requestedBufferSizeFrames, numBuffersInRing );

    auto numSamplesPerBuffer = channelCount * _hostImplementation->GetBufferSizeInFrames();

	_soundBuffer.reserve( numSamplesPerBuffer );

	for( uint32_t i=0; i < numSamplesPerBuffer; i++ )
	{
		_soundBuffer.push_back( 0 );
	}
}

WaveOutputStream::~WaveOutputStream()
{
    _hostImplementation = nullptr;  // Destroy this first because it has had the _soundBuffer's address revealed to it.
    // Language will now safely apply destructors to everything.
}

uint32_t WaveOutputStream::GetRateHz() const
{
    return _hostImplementation->GetRateHz();
}

uint32_t WaveOutputStream::GetBufferSizeInFrames() const
{
    return _hostImplementation->GetBufferSizeInFrames();
}

void *WaveOutputStream::GetSoundBufferBaseAddress()
{
    return &_soundBuffer[0];
}

void WaveOutputStream::PlayBufferWithWait()
{
    _hostImplementation->Write( &_soundBuffer[0], GetBufferSizeInFrames() );
}

/* Not used yet
void WaveOutputStream::Flush()
{
    _hostImplementation->Flush();
}

void WaveOutputStream::Reset()
{
    _hostImplementation->Reset();
}
*/


