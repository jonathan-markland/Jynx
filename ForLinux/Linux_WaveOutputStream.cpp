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



#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include <assert.h>

#include "WaveOutputStream.h"



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND OUTPUT FOR LINUX
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class HostOS_WaveOutputStream
{
public:

    HostOS_WaveOutputStream( uint32_t requestedRateHz, uint32_t channelCount, uint32_t requestedBufferSizeFrames, uint32_t numBuffersInRing );
    ~HostOS_WaveOutputStream();
    uint32_t GetRateHz() const;
    uint32_t GetBufferSizeInFrames() const;
    void Write( const void *soundDataBlock, uint32_t numFrames );
    void Flush();
    void Reset();

private:

    bool _handleConstructed;
    snd_pcm_t *_handle;
    snd_pcm_hw_params_t *_params;  // TODO: not sure if needed globally
    uint32_t  _actualRateHz;            // Because the driver may return different
    uint32_t  _actualBufferSizeFrames;  // Because the driver may return different

};



HostOS_WaveOutputStream::HostOS_WaveOutputStream( uint32_t requestedRateHz, uint32_t channelCount, uint32_t requestedBufferSizeFrames, uint32_t numBuffersInRing )
    : _handleConstructed( false )
    , _handle( nullptr )
    , _actualRateHz( 0 )
    , _actualBufferSizeFrames( 0 )
{
    // Open PCM device for playback
    if( snd_pcm_open( &_handle, "default", SND_PCM_STREAM_PLAYBACK, 0 ) != 0 )   // TODO: mode	Open mode (see SND_PCM_NONBLOCK, SND_PCM_ASYNC)
    {
        throw std::runtime_error( "Cannot open sound playback device." );
    }

    _handleConstructed = true;

    // Allocate a hardware parameters object.
    snd_pcm_hw_params_alloca( &_params ); // TODO: can this fail?

    // Fill it in with default values.
    snd_pcm_hw_params_any( _handle, _params ); // It doesn't say this fails.

    // Set the desired hardware parameters.

    // Interleaved mode
    if( snd_pcm_hw_params_set_access( _handle, _params, SND_PCM_ACCESS_RW_INTERLEAVED ) != 0 )
    {
        throw std::runtime_error( "Failed to set sound access mode." );
    }

    // Signed 16-bit little-endian format
    if( snd_pcm_hw_params_set_format( _handle, _params, SND_PCM_FORMAT_S16_LE ) != 0 )
    {
        throw std::runtime_error( "Failed to set sound format." );
    }

    // Number of channels
    if( snd_pcm_hw_params_set_channels( _handle, _params, channelCount ) != 0 )
    {
        throw std::runtime_error( "Failed to set number of sound channels." );
    }

    // Sampling rate
    unsigned int val = requestedRateHz;
    int dir = 0;
    if( snd_pcm_hw_params_set_rate_near( _handle, _params, &val, &dir ) != 0 )
    {
        throw std::runtime_error( "Failed to set rate for sound output." );
    }
    _actualRateHz = requestedRateHz;

    // Set period size in frames
    snd_pcm_uframes_t  frames = requestedBufferSizeFrames;
    if( snd_pcm_hw_params_set_period_size_near( _handle, _params, &frames, &dir ) != 0 )
    {
        throw std::runtime_error( "Failed to set desired buffer size for sound output." );
    }
    _actualBufferSizeFrames = frames;

    // Restrict total ring buffer size.
    auto desireRingBufferSize = (snd_pcm_uframes_t) (_actualBufferSizeFrames * numBuffersInRing);
    auto ringBufferSize = desireRingBufferSize;
    if( snd_pcm_hw_params_set_buffer_size_near( _handle, _params, &ringBufferSize ) != 0  ||  ringBufferSize != desireRingBufferSize )  // this must match the numBuffersInRing we asked for
    {
        throw std::runtime_error( "Failed to set sound driver ring buffer size." );
    }

    // Write the parameters to the driver
    if( snd_pcm_hw_params( _handle, _params ) != 0 )
    {
        throw std::runtime_error( "Failed to write settings to sound driver." );
    }

    // Sanity check buffer size for one period, against what caller will expect to use.  // TODO: unnecessary?
    if( snd_pcm_hw_params_get_period_size( _params, &frames, &dir ) != 0  ||  frames != _actualBufferSizeFrames )
    {
        throw std::runtime_error( "Failed to confirm sound driver settings." );
    }
}



uint32_t HostOS_WaveOutputStream::GetRateHz() const
{
    return _actualRateHz;
}



uint32_t HostOS_WaveOutputStream::GetBufferSizeInFrames() const
{
    return _actualBufferSizeFrames;
}



void HostOS_WaveOutputStream::Write( const void *soundDataBlock, uint32_t numFrames )
{
    auto rc = snd_pcm_writei( _handle, soundDataBlock, numFrames );
    if( rc == -EPIPE )
    {
        // Under-run
        snd_pcm_prepare( _handle );
    }
    else if( rc < 0 )
    {
        // Error
        assert(false);
    }
    else if( rc != (int) numFrames )
    {
        // Short write, rc is the number of frames.
        assert(false);
    }
}


/* Not used yet
void HostOS_WaveOutputStream::Flush()
{
    assert(false); // TODO: check
    snd_pcm_drain( _handle );
}



void HostOS_WaveOutputStream::Reset()
{
    assert(false); // TODO: check
    snd_pcm_close( _handle );
}
*/


HostOS_WaveOutputStream::~HostOS_WaveOutputStream()
{
    if( _handleConstructed )   // only the destructor needs this guard
    {
        snd_pcm_drain( _handle );
        snd_pcm_close( _handle );
    }
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


