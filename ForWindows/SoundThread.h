//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2014  Jonathan Markland
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

#include "stdint.h"
#include "libWinApi.h"

#define WM_ADVANCE_EMULATION (WM_USER + 0x100)


class SoundThread: public libWinApi::Thread
{
public:
	
	explicit SoundThread( uint32_t numSamplesPerBuffer );
	virtual ~SoundThread() override;

	void SetEmulatorWindowHandle( HWND hWndEmulatorMainForm );

	virtual int32_t ThreadMain() override;
	virtual void RequestTermination() override;

	uint16_t  *GetBaseAddressOfSharedSoundBuffer();    // in this model, this need only be queried once by the host
	size_t     GetSizeOfSharedSoundBufferInSamples();  // in this model, this need only be queried once by the host

	void NotifyEmulatorFinishedFillingSoundBuffer();

private:

	HANDLE _emulatorIsFillingNextBufferEvent;  // Emulator signals this when it's done, and ready to play
	HWND   _hWndEmulatorMainForm;
	std::vector<uint16_t>  _soundBuffer;       // Shared with the main program


};