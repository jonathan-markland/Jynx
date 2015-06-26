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

    WaveOutputStream( uint32_t requestedRateHz, uint32_t channelCount, uint32_t requestedBufferSizeFrames, uint32_t numBuffersInRing );
        // Note: Format is fixed at 2 bytes per sample, 16-bit signed little-endian
        // Note: You must honour GetRateHz()  GetBufferSizeInFrames() which may be different from requested

    ~WaveOutputStream();
        // Need to ensure a destruction order.

    uint32_t GetRateHz() const;
        // Retrieves the rate that was granted by the hardware driver.
        // WARNING: This may be different to that requested, but as near as possible.

    uint32_t GetBufferSizeInFrames() const;
        // Retrieves the frame count that was granted by the hardware driver.
        // WARNING: This may be different to that requested, but as near as possible.

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

    std::shared_ptr<class HostOS_WaveOutputStream>  _hostImplementation;
    std::vector<uint16_t>  _soundBuffer;  // TODO: Can we use the ring buffer rather than copy-in (on both linux and windows?)

};





