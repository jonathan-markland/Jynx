//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
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