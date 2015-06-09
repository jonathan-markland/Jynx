
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

    HostOS_WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames, uint32_t numBuffersInRing );
    ~HostOS_WaveOutputStream();
    void Write( const void *soundDataBlock, uint32_t numFrames );
    void Flush();
    void Reset();

private:

    bool _handleConstructed;
    snd_pcm_t *_handle;
    snd_pcm_hw_params_t *_params;  // TODO: not sure if needed globally

};



HostOS_WaveOutputStream::HostOS_WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames, uint32_t numBuffersInRing )
    : _handleConstructed( false )
    , _handle( nullptr )
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

    // 44100 bits/second sampling rate (CD quality) */
    unsigned int val = 44100;
    int dir = 0;
    if( snd_pcm_hw_params_set_rate_near( _handle, _params, &val, &dir ) != 0  ||  val != 44100 )
    {
        throw std::runtime_error( "Failed to set 44100Hz for sound output." );
    }

    // Set period size in frames
    snd_pcm_uframes_t  frames = bufferSizeFrames;
    if( snd_pcm_hw_params_set_period_size_near( _handle, _params, &frames, &dir ) != 0  ||  frames != bufferSizeFrames )
    {
        throw std::runtime_error( "Failed to set desired buffer size for sound output." );
    }

    // Restrict total ring buffer size.
    auto desireRingBufferSize = (snd_pcm_uframes_t) (bufferSizeFrames * numBuffersInRing);
    auto ringBufferSize = desireRingBufferSize;
    if( snd_pcm_hw_params_set_buffer_size_near( _handle, _params, &ringBufferSize ) != 0  ||  ringBufferSize != desireRingBufferSize )
    {
        throw std::runtime_error( "Failed to set sound driver ring buffer size." );
    }

    // Write the parameters to the driver
    if( snd_pcm_hw_params( _handle, _params ) != 0 )
    {
        throw std::runtime_error( "Failed to write settings to sound driver." );
    }

    // Sanity check buffer size for one period, against what caller will expect to use.  // TODO: unnecessary?
    if( snd_pcm_hw_params_get_period_size( _params, &frames, &dir ) != 0  ||  frames != bufferSizeFrames )
    {
        throw std::runtime_error( "Failed to confirm sound driver settings." );
    }
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
//     WAVE SOUND OUTPUT - INTERFACE DELEGATION
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

WaveOutputStream::WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames, uint32_t numBuffersInRing )
{
    _hostImplementation = std::make_shared<HostOS_WaveOutputStream>( channelCount, bufferSizeFrames, numBuffersInRing );
}

void WaveOutputStream::Write( const void *soundDataBlock, uint32_t numFrames )
{
    _hostImplementation->Write( soundDataBlock, numFrames );
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


