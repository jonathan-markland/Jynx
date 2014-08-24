//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//

#pragma once

#include "resource.h"
#include "libWinApi.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     ABOUT BOX
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class AboutBoxForm : public libWinApi::BaseForm
{
public:

	AboutBoxForm( HWND hWndOwner );
	enum { IDD = IDD_ABOUTBOX };
	virtual void WindowProc( libWinApi::WindowProcArgs &e );
	virtual bool OnInitDialog();

};

