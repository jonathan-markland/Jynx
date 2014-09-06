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



void SyntaxError()
{
	throw std::runtime_error( "Syntax error in command line string" );
}			


void MissingOperand()
{
	throw std::runtime_error( "Command line parameter is missing an operand." );
}



bool ParseParamAndValue( const std::vector<std::wstring> &paramList, size_t &i, const wchar_t *switchString, std::wstring *out_variable )
{
	if( i < paramList.size() )
	{
		if( paramList[i] == switchString )
		{
			++i;
			if( i >= paramList.size() ) MissingOperand();
			if( paramList[i][0] == L'-' ) MissingOperand();
			*out_variable = paramList[i];
			++i;
			return true;
		}
	}
	return false;
}



bool ParseParam( const std::vector<std::wstring> &paramList, size_t &i, const wchar_t *switchString, bool *out_variable )
{
	if( i < paramList.size() )
	{
		if( paramList[i] == switchString )
		{
			++i;
			*out_variable = true;
			return true;
		}
	}
	return false;
}






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

		// Parsed Parameters
		std::wstring  settingsFilePath;
		std::wstring  snapshotFilePath;
		bool gamesMode = false;
		size_t i=0;
		while( i < paramList.size() )
		{
			     if( ParseParamAndValue( paramList, i, L"--settings", &settingsFilePath ) ) {}
			else if( ParseParamAndValue( paramList, i, L"--snapshot", &snapshotFilePath ) ) {}
			else if( ParseParam( paramList, i, L"--games", &gamesMode ) ) {}
			else throw std::runtime_error( "Unrecognised content on command line." );
		}

		// Show main form:
		auto mainForm = std::make_shared<MainForm>( (HWND) NULL, settingsFilePath.c_str(), snapshotFilePath.c_str(), gamesMode );
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
