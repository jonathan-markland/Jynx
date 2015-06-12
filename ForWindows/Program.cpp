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



#include "stdafx.h"
#include "libWinApi.h"
#include "assert.h"
#include <exception>
#include <memory>
#include <vector>
#include <string>
#include "MainForm.h"


int APIENTRY _tWinMain(
	_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
	try
	{
		// Command line parse:
		std::vector<std::wstring>  paramList;
		if( ! libWinApi::SplitCommandLine( lpCmdLine, &paramList ) )
		{
			SyntaxError();
		}

		// Show main form:
		auto mainForm = std::make_shared<MainForm>( (HWND) NULL, paramList );
		mainForm->ShowAndMaximise();
		mainForm->DoModal();

		return 0;
	}
	catch( const std::exception &e )
	{
		MessageBoxA( NULL, e.what(), "Program cannot continue running", MB_OK | MB_ICONERROR );
		return 1;
	}
}
