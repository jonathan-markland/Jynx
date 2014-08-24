//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
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
	return BaseForm::OnInitDialog();
}








