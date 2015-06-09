
#pragma once

#include <stdexcept>
#include <stdint.h>
#include <memory>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND OUTPUT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	// NOTE:  Operating system specific implementation must be linked in.


class WaveOutputStream
{
public:

    // TODO: Store constructed values and provide "reminder" interface?
    // TODO: Should/could this expose the address of a buffer to be filled?

    WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames );
        // 44,100Hz
        // 2 bytes per sample, 16-bit signed little-endian

    void Write( const void *soundDataBlock, uint32_t numFrames );
        // A frame is (bytesPerSample * channelCount)
        // This *copies* the supplied data.

    // Not used yet:  void Flush();
    // Not used yet:  void Reset();

private:

    std::shared_ptr<class HostOS_WaveOutputStream>  _hostImplementation;

};






