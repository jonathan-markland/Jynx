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
#include "resource.h"
#include "AboutBoxForm.h"


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     ABOUT BOX
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

AboutBoxForm::AboutBoxForm( HWND hWndOwner ) 
	: BaseForm( hWndOwner, AboutBoxForm::IDD )
{
}



void AboutBoxForm::WindowProc( libWinApi::WindowProcArgs &e )
{
	return BaseForm::WindowProc( e );
}



bool AboutBoxForm::OnInitDialog()
{
	libWinApi::CenterWindow( *this, GetOwner() );

	auto hWndEditGitHub = ::GetDlgItem( *this, IDC_EDIT_GITHUB );
	auto hWndEditBlog   = ::GetDlgItem( *this, IDC_EDIT_BLOG );
	::SetWindowText( hWndEditGitHub, L"http://www.github.com/jonathan-markland/jynx" );
	::SetWindowText( hWndEditBlog,   L"http://jynx-emulator.tumblr.com/" );

	return BaseForm::OnInitDialog();
}








