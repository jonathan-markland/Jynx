
#pragma once

#include <stdexcept>
#include <stdint.h>
#include <memory>
#include <vector>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//     WAVE SOUND OUTPUT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


	// NOTE:  Operating system specific implementation must be linked in.


class WaveOutputStream
{
public:

    // TODO: Store constructed values and provide "reminder" interface?
    // TODO: Should/could this expose the address of a buffer to be filled?

    WaveOutputStream( uint32_t channelCount, uint32_t bufferSizeFrames, uint32_t numBuffersInRing );
        // 44,100Hz
        // 2 bytes per sample, 16-bit signed little-endian

    ~WaveOutputStream();
        // Need to ensure a destruction order.

    void *GetSoundBufferBaseAddress();
        // Retrieve base address of composition buffer.
        // This will not be played until the next PlayBufferWithWait() call.
        // The size of this is given by the constructor parameters.

    void PlayBufferWithWait();
        // The caller has filled the buffer with data.
        // This sends the buffer to the sound subsystem.
        // Once the ring of buffers is populated, this becomes a
        // blocking function useful for timed synchronisation.

    // Not used yet:  void Flush();
    // Not used yet:  void Reset();

private:

    uint32_t               _bufferSizeFrames;
    std::shared_ptr<class HostOS_WaveOutputStream>  _hostImplementation;
    std::vector<uint16_t>  _soundBuffer;  // TODO: Can we use the ring buffer rather than copy-in (on both linux and windows?)

};





