//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//


#include "stdafx.h"
#include "libWinApi.h"
#include "assert.h"
#include <exception>
#include <memory>
#include "MainForm.h"


int APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	try
	{
		auto mainForm = std::make_shared<MainForm>( (HWND) NULL );
		mainForm->ShowAndMaximise();
		mainForm->DoModal();
	}
	catch( const std::exception &e )
	{
		MessageBoxA( NULL, e.what(), "Program cannot continue running", MB_OK | MB_ICONERROR );
	}

	return 0;
}
