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

#include <vector>
#include <iterator>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

#include "windowsx.h"
#include "shellapi.h"
#include "shlobj.h"
#include "CommCtrl.h"
#include "Commdlg.h"

#include "libWinApi.h"




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     SAFE MESSAGE UNPACKING
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	Message::Message()
	{
		memset( this, 0, sizeof(*this) ); 
	}

	bool Message::IsNotify() const
	{
		return message == WM_NOTIFY;
	}

	bool Message::IsCommand() const
	{
		return message == WM_COMMAND;
	}

	bool Message::IsMenuCommand() const
	{
		return message == WM_COMMAND  &&  HIWORD(wParam) == 0  &&  lParam == 0;
	}

	bool Message::IsMenuCommand( UINT16 *out_CommandID ) const
	{
		if( IsMenuCommand() )
		{
			*out_CommandID = LOWORD(wParam);
			return true;
		}
		return false;
	}

	bool Message::IsErase() const 
	{ 
		return message == WM_ERASEBKGND; 
	}

	bool Message::IsPaint() const 
	{ 
		return message == WM_PAINT; 
	}

	bool Message::IsKeyDown() const
	{
		return message == WM_KEYDOWN;
	}

	bool Message::IsKeyDown( UINT32 VkCode ) const
	{ 
		return message == WM_KEYDOWN && wParam == VkCode; 
	}

	bool Message::IsKeyDown( UINT32 *out_VkCode ) const
	{ 
		if( message == WM_KEYDOWN )
		{
			*out_VkCode = (UINT32) wParam;
			return true;
		}
		return false;
	}

	bool Message::IsDeactivated() const
	{
		return message == WM_ACTIVATE  &&  wParam == 0;
	}

	bool Message::IsKeyUp() const
	{
		return message == WM_KEYUP;
	}

	bool Message::IsKeyUp( UINT32 VkCode ) const
	{ 
		return message == WM_KEYUP && wParam == VkCode; 
	}

	bool Message::IsKeyUp( UINT32 *out_VkCode ) const
	{ 
		if( message == WM_KEYUP )
		{
			*out_VkCode = (UINT32) wParam;
			return true;
		}
		return false;
	}



	bool Message::IsMouseMove()  const  { return message == WM_MOUSEMOVE; }
	bool Message::IsMouseLDown() const  { return message == WM_LBUTTONDOWN; }
	bool Message::IsMouseLUp()   const  { return message == WM_LBUTTONUP; }
	bool Message::IsMouseRDown() const  { return message == WM_RBUTTONDOWN; }
	bool Message::IsMouseRUp()   const  { return message == WM_RBUTTONUP; }

	bool Message::IsMouseLDoubleClick() const  { return message == WM_LBUTTONDBLCLK; }
	bool Message::IsMouseRDoubleClick() const  { return message == WM_RBUTTONDBLCLK; }

	bool Message::IsMouseMove(  POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_MOUSEMOVE,   out_CursorPos ); }
	bool Message::IsMouseLDown( POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_LBUTTONDOWN, out_CursorPos ); }
	bool Message::IsMouseLUp(   POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_LBUTTONUP,   out_CursorPos ); }
	bool Message::IsMouseRDown( POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_RBUTTONDOWN, out_CursorPos ); }
	bool Message::IsMouseRUp(   POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_RBUTTONUP,   out_CursorPos ); }

	bool Message::IsMouseLDoubleClick( POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_LBUTTONDBLCLK, out_CursorPos ); }
	bool Message::IsMouseRDoubleClick( POINT *out_CursorPos ) const  { return HandleMouseMessage( WM_RBUTTONDBLCLK, out_CursorPos ); }

	bool Message::HandleMouseMessage( UINT32 MouseMsg, POINT *out_CursorPos ) const
	{
		if( message == MouseMsg )
		{
			*out_CursorPos = LParamToPoint( lParam );
			return true;
		}
		return false;
	}

	void Message::Repost() const
	{
		::PostMessage( hwnd, message, wParam, lParam );
	}

	void Message::RepostTo( HWND hWnd ) const
	{
		::PostMessage( hWnd, message, wParam, lParam );
	}

	void Message::ResendTo( HWND hWnd ) const
	{
		::SendMessage( hWnd, message, wParam, lParam );
	}

	bool Message::IsGetMinMax() const
	{
		return message == WM_GETMINMAXINFO;
	}

	void Message::ChangeTheKey( UINT32 VkCode )
	{
		if( message == WM_KEYDOWN || message == WM_KEYUP )
		{
			wParam = VkCode;
		}
		else assert(false); // message not supported
	}

	void Message::ChangeToKeyDownMessage( UINT32 VkCode )
	{
		message = WM_KEYDOWN;
		hwnd    = NULL;
		wParam  = VkCode;
		lParam  = 0;
	}

	bool Message::TurnWheelMessageIntoKeyPressMessage()
	{
		// (This was an assumption for the grid navigator)
		auto zDelta = GET_WHEEL_DELTA_WPARAM( wParam );
		if( zDelta <= -WHEEL_DELTA )
		{
			ChangeToKeyDownMessage( VK_DOWN );
			return true;
		}
		else if( zDelta >= WHEEL_DELTA )
		{
			ChangeToKeyDownMessage( VK_UP );
			return true;
		}
		return false;
	}

}



namespace libWinApi
{
	void WindowProcArgs::RespondWithMinTrackSize( SIZE sz )
	{
		if( IsGetMinMax() )
		{
			auto pMMI = reinterpret_cast<MINMAXINFO *>( lParam );
			if( pMMI )
			{
				pMMI->ptMinTrackSize = MakePOINT(sz.cx, sz.cy);
				Result = 0;
			}		
			else assert(false); // didn't expect this from API!
		}
		else assert(false);
	}
}




namespace libWinApi
{
	POINT LParamToPoint( LPARAM lParam )
	{
		// for use with unpacking coordinates from mouse messages  (SIGNED coordinates!)
		return MakePOINT( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
	}
}




namespace libWinApi
{
	WmPaintHandler::WmPaintHandler( libWinApi::WindowProcArgs &e )
	{
		Init( e.hwnd );
	}

	WmPaintHandler::WmPaintHandler( HWND hWnd )
	{
		Init( hWnd );
	}

	void WmPaintHandler::Init( HWND hWnd )
	{
		dc = ::BeginPaint( hWnd, &ps );
		m_hWnd = hWnd;
	}

	WmPaintHandler::~WmPaintHandler()
	{
		::EndPaint( m_hWnd, &ps );
	}

	bool WmPaintHandler::ClipToUpdateRect()
	{
		// The client can call this to set the clip zone to the update rectangle.
		// We don't want the client to do anything if the area is empty though.
		if( ! AreaIsEmpty() )
		{
			INT32 Type = ::IntersectClipRect( ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom );
			return Type == SIMPLEREGION || Type == COMPLEXREGION;
		}
		return false;
	}	
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//    MS WINDOWS API ROUTINES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	bool IsWindowClass( HWND h, const wchar_t *ClassName )
	{
		if(h)
		{
			wchar_t cn[256];
			if( ::GetClassNameW( h, cn, 250 ) )
			{
				return _wcsicmp( ClassName, cn ) == 0;
			}
		}
		return false;
	}
}





namespace libWinApi
{
	void ToggleWindowBetweenMaximizeAndRestore( HWND hWnd )
	{
		if( IsMaximized( hWnd ) )
		{
			::ShowWindow( hWnd, SW_RESTORE );
		}
		else
		{
			::ShowWindow( hWnd, SW_MAXIMIZE );
		}
	}
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     HandleHitWindowCaption()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	void HandleHitWindowCaption( HWND hWnd )
	{
		// LBUTTON is down on the window's caption.
		// Substitute our own window-move routine, to avoid stupid pause that interrupts emulation -- thanks Microsoft!

		::SetActiveWindow( hWnd );

		// Take a measurement of WHERE the mouse went down, in relation to the top left:

		POINT ptOrigin;
		::GetCursorPos( &ptOrigin );

		RECT rcBorder;
		::GetWindowRect( hWnd, &rcBorder );

		auto distanceFromLeft = ptOrigin.x - rcBorder.left;
		auto distanceFromTop  = ptOrigin.y - rcBorder.top;
		auto windowWidth      = rcBorder.right - rcBorder.left;
		auto windowHeight     = rcBorder.bottom - rcBorder.top;

		bool isMoveable = ! IsMaximized( hWnd );

		libWinApi::ModalLoop  subLoop;
		libWinApi::Message    message;

		auto ptPrevious = ptOrigin;
	
		subLoop.DoMessageLoop( &message, [&]()
		{
			// [ ] Only move if it's in the RESTORED state.

			if( isMoveable )
			{
				// Get live mouse position, and check for movement change.
				// Let's not bother with messages for this, SetCapture() isn't reliable, as something else may take capture (potentially!)

				POINT ptCurrent;
				::GetCursorPos( &ptCurrent );

				// If position changed since last loop, move the window:

				if( ptCurrent.x != ptPrevious.x || ptCurrent.y != ptPrevious.y )
				{
					::MoveWindow( hWnd, 
						ptCurrent.x - distanceFromLeft,
						ptCurrent.y - distanceFromTop,
						windowWidth,
						windowHeight,
						TRUE );

					ptPrevious = ptCurrent;
				}
			}

			// Otherwise, regular message dispatching, except for some we stub out:

			if( message.message >= WM_NCMOUSEMOVE && message.message <= WM_NCXBUTTONDBLCLK && message.message != WM_NCLBUTTONUP )
			{
				// Don't dispatch anything NC-mousey.
			}
			else if( message.message >= WM_MOUSEFIRST && message.message <= WM_MOUSELAST )
			{
				// Don't dispatch anything mousey.
			}
			else
			{
				::TranslateMessage( &message );
				libWinApi::Dispatch( message );
			}

			// Test for left button release:

			if( (::GetAsyncKeyState( VK_LBUTTON ) & 0x8000) == 0 )
			{
				subLoop.End();
			}
		} );
	}
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     Windows DispatchMessage() replacement.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	void Dispatch( libWinApi::Message &msg )
	{
		// Avoid DispatchMessage() API's swallowing of exceptions.
		// We do this by whitelisting messages we know about and dispatching them ourselves.

		auto message = msg.message;

		if( message == WM_TIMER && msg.lParam != NULL )
		{
			((TIMERPROC) msg.lParam)( msg.hwnd, WM_TIMER, msg.wParam, GetTickCount() );
		}
		else if( msg.IsPaint()
			|| (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) 
			|| (message >= WM_NCMOUSEMOVE && message <= WM_NCXBUTTONDBLCLK) 
			|| (message >= WM_INITDIALOG && message <= WM_SYSCOMMAND) 
			|| (message == WM_TIMER && msg.lParam == NULL) )
		{
			// Dispatch this message without using Windows.
			// This way we avoid the irritating exception swallowing "catch and assume to continue" that DispatchMessage() API does.
			auto p = ::GetWindowLongPtr( msg.hwnd, GWLP_WNDPROC );
			if( p )
			{
				((WNDPROC) p)( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			}
			else
			{
				::DispatchMessage( &msg ); // Get windows to handle it.  Other programs would just have done this even through there's no apparent WNDPROC!
			}
		}
		else
		{
			::DispatchMessage( &msg ); // Get windows to handle it.
		}
	}
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     ASSISTANCE CLASSES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{

	// These ensure exception safe cleanup


	namespace Internal
	{
		class WindowShowAndHider  // assistance for BaseForm::DoModal
		{
		public:

			explicit WindowShowAndHider( HWND hWnd ) 
				: hWnd(hWnd) 
			{
				::ShowWindow( hWnd, SW_SHOW );
			}

			~WindowShowAndHider() 
			{ 
				::ShowWindow( hWnd, SW_HIDE ); 
			}

		private:

			HWND  hWnd;

		};
	}





	namespace Internal
	{
		class WindowDisablerAndReEnabler  // assistance for BaseForm::DoModal
		{
		public:

			explicit WindowDisablerAndReEnabler( HWND hWnd ) 
				: _hWnd(hWnd) 
			{
				_previousEnableState = ::IsWindowEnabled( hWnd );
				::EnableWindow( hWnd, FALSE );
			}

			~WindowDisablerAndReEnabler() 
			{ 
				::EnableWindow( _hWnd, _previousEnableState ); 
			}

		private:

			HWND  _hWnd;
			BOOL  _previousEnableState;

		};
	}
}









// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     BASE DIALOG
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	BaseForm::BaseForm( HWND hWndOwner, UINT nIDTemplate )
		: m_EndDialogCode(0)         // Tidyness
		, m_hWnd(NULL)               // Until dialog created
		, m_pPrevWndProc(NULL)
		, m_AllowOlderFamilyResizing(true)
		, m_hWndOwner(hWndOwner)     // we support NULL
		, m_lpszTemplateName(MAKEINTRESOURCE(nIDTemplate))
		, m_MaximiseOnReshowReminder(false)
		, _bigIcon(NULL)
		, _smallIcon(NULL)
	{
	}



	BaseForm::~BaseForm()
	{
		ReleaseIcons();

		if( m_hWnd )
		{
			assert( ::IsWindow(m_hWnd) ); // This class expects to manage the lifetime of the handle!
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}
	}



	bool BaseForm::Create()
	{
		if( m_hWnd == NULL )
		{
			assert( this != NULL ); // should be
			HINSTANCE  hInst = reinterpret_cast<HINSTANCE>(&__ImageBase); // from Raymond Chen
			m_hWnd = ::CreateDialogParam( hInst, m_lpszTemplateName, m_hWndOwner, &StaticDialogProc, (LPARAM) this );
			assert( m_hWnd != NULL );
			return m_hWnd != NULL; // return true if success.
		}
		else
		{
			assert(false); // a reminder that the dialog is already created.
			return true;
		}
	}



	void BaseForm::SetBigAndSmallIcons( UINT iconResourceID )
	{
		assert( m_hWnd != NULL );

		ReleaseIcons();

		_bigIcon = (HICON) ::LoadImage(
			::GetModuleHandle(0),
			MAKEINTRESOURCE(iconResourceID),
			IMAGE_ICON,
			::GetSystemMetrics(SM_CXICON),
			::GetSystemMetrics(SM_CYICON),
			0);
	
		_smallIcon = (HICON) ::LoadImage(
			::GetModuleHandle(0),
			MAKEINTRESOURCE(iconResourceID),
			IMAGE_ICON,
			::GetSystemMetrics(SM_CXSMICON),
			::GetSystemMetrics(SM_CYSMICON),
			0);

		::SendMessage( *this, WM_SETICON, ICON_BIG, (LPARAM) _bigIcon );
		::SendMessage( *this, WM_SETICON, ICON_SMALL, (LPARAM) _smallIcon );
	}



	void BaseForm::ReleaseIcons()
	{
		if( m_hWnd != NULL )
		{
			::SendMessage( *this, WM_SETICON, ICON_BIG, (LPARAM) NULL );
			::SendMessage( *this, WM_SETICON, ICON_SMALL, (LPARAM) NULL );
		}

		if( _bigIcon )
		{
			::DestroyIcon( _bigIcon );
			_bigIcon = NULL;
		}

		if( _smallIcon )
		{
			::DestroyIcon( _smallIcon );
			_smallIcon = NULL;
		}
	}



	void BaseForm::ShowAndMaximise()
	{
		::ShowWindow( *this, SW_MAXIMIZE );
	}



	void BaseForm::WindowProc( libWinApi::WindowProcArgs &e )
	{
		assert( e.hwnd == m_hWnd ); // Should be the same!!

		//
		// WM_COMMAND
		//
		
		if( e.IsCommand() )
		{
			libWinApi::WmCommandDispatcher  disp( e ); 
			if( disp.Is(BN_CLICKED) )
			{
				// On receiving WM_COMMAND with BN_CLICKED and these standard IDs, we call the virtual functions:
				// We shall support these no matter whether the dialog is modeless or modal.
				disp.Case( IDOK,     this, &BaseForm::DoVirtualCall_OnOK );
				disp.Case( IDCANCEL, this, &BaseForm::DoVirtualCall_OnCancel );
			}
			if( disp.IsFromMenu() )
			{
				// This makes the close box do the cancel action, for those dialogs without a button with ID=2
				disp.Case( IDCANCEL, this, &BaseForm::DoVirtualCall_OnCancel );
			}
		}

		//
		// WM_INITDIALOG
		//

		else if( e.message == WM_INITDIALOG )
		{
			e.Result = OnInitDialog() ? TRUE : FALSE;
			return;
		}

		//
		// WM_WINDOWPOSCHANGING  |  WM_WINDOWPOSCHANGED  
		//
		// This attempts to better Windows's standard efforts for the non-backsheet windows.
		// They are hidden when the back sheet is minimised, but are NOT ever restored-to-maximised
		// if they were maximised (Windows forgets).
		//

		else if( e.message == WM_WINDOWPOSCHANGING || e.message == WM_WINDOWPOSCHANGED )
		{
			auto pWP = reinterpret_cast<WINDOWPOS *>( e.lParam );
			if( pWP )
			{
				auto minflags = (SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
				if( e.message == WM_WINDOWPOSCHANGING && (pWP->flags & minflags) == minflags )
				{
					// We are being hidden, let's remember if we were maxmised, for if we get shown again:
					m_MaximiseOnReshowReminder = IsMaximized( e.hwnd ) ? true : false;
				}
				else if( e.message == WM_WINDOWPOSCHANGED && pWP->flags & SWP_SHOWWINDOW )
				{
					if( m_MaximiseOnReshowReminder )
					{
						m_MaximiseOnReshowReminder = false; // for next time. Also let's set this flag before this:
						if( ! IsMaximized( e.hwnd ) )
						{
							::ShowWindow( e.hwnd, SW_MAXIMIZE );
						}
					}
				}
			}

			// Let caller see this message too.
		}

		//
		// Special handling to work around MS-Windows glitches (if enabled)
		//

		else if( HandleProblemWorkarounds( &e ) )
		{
			e.Result = 0;
			return;
		}

		//
		// Call the original WNDPROC that we sub-classed in WM_INITDIALOG:
		//

		e.Result = ::CallWindowProc( m_pPrevWndProc, m_hWnd, e.message, e.wParam, e.lParam );
	}
	


	bool BaseForm::OnInitDialog()
	{
		return true;
	}
	


	void BaseForm::DoVirtualCall_OnOK()        
	{ 
		OnOK();	// virtual function call
	}



	void BaseForm::DoVirtualCall_OnCancel()
	{ 
		OnCancel(); // virtual function call
	}    



	void BaseForm::OnOK()
	{
		if( m_pActiveModalLoop )
		{
			EndModal( IDOK ); // modal
		}
		else
		{
			::ShowWindow( *this, SW_HIDE ); // modeless
		}
	}



	void BaseForm::OnCancel()
	{
		if( m_pActiveModalLoop )
		{
			EndModal( IDCANCEL ); // modal
		}
		else
		{
			::ShowWindow( *this, SW_HIDE ); // modeless
		}
	}



	bool BaseForm::PreProcessMessage( libWinApi::Message *pMsg )
	{
		assert( ::IsWindow(m_hWnd) ); // You have destroyed the window handle during the DoModal() loop?

		// - I do not want ENTER on any of my dialogs to do the "default button" search.
		//   So, for class "BUTTON" we fake it that the SPACE BAR was being pressed instead,
		//   and for all others we make VK_RETURN *bypass* IsDialogMessage() altogether.
		// - Also, ENTER on single-line EDIT controls wants to TAB out of the control.

		UINT32 Key;
		if( pMsg->IsKeyDown(&Key) || pMsg->IsKeyUp(&Key) )
		{
			if( Key == VK_RETURN )
			{
				if( libWinApi::IsWindowClass( pMsg->hwnd, L"BUTTON" ) )
				{
					pMsg->ChangeTheKey( VK_SPACE ); // fake it
				}
				else if( libWinApi::IsWindowClass( pMsg->hwnd, L"EDIT" ) )
				{
					if( ! (GetWindowStyle( pMsg->hwnd ) & ES_MULTILINE) ) // Multi line edits must start a new line on ENTER
					{
						pMsg->ChangeTheKey( VK_TAB ); // change enter -> tab  ** FIX: This was for the Analysis screen so Enter can action the change.
					}
					else return false;  // Caller translates & dispatches, *BUT* we're skipping IsDialogMessage() here!
				}
				else
				{
					return false; // Caller translates & dispatches, *BUT* we're skipping IsDialogMessage() here!
				}
			}
			else if( Key == VK_ESCAPE ) // trying to stop VK_ESCAPE closing important screens.
			{
				return false;
			}
		} 

		// Regular action for dialogs:

		if( ::IsDialogMessage( m_hWnd, pMsg ) )
		{
			return true; // skip
		}

		return false; // The default is to process all posted messages (ie: false == do *not* skip)
	}



	INT_PTR BaseForm::DoModal()
	{
		m_EndCodeSetAlready = false;
		m_EndDialogCode = IDCANCEL;

		// The dialog template must be specified:
		if( m_lpszTemplateName == NULL )
		{
			assert(false);
			return 0;
		}

		if( m_hWndOwner )
		{
			// If the owner is non-NULL, it must still exist:
			if( ! ::IsWindow( m_hWndOwner ) ) // this is the best we can do!
			{
				assert(false); // Owner window handle is known to no longer be valid.
				return 0;
			}
		}

		// Let's limit the scope of variable 'loop'.
		{
			ModalLoop  loop;
			StackSocket<ModalLoop>  Socket( m_pActiveModalLoop, &loop ); // <-- this line enables EndModal() to find our "loop" variable.

			if( m_hWnd == NULL )
			{
				// The dialog is not created already, so do that now.
				// We shall the leave the dialog in the 'created' state on return.
				// Reminder: Create() sends WM_INITDIALOG, which calls OnInitDialog() which may want to EndModal() right away.
				if( ! Create() )
				{
					assert(false);   // Dialog creation failed
					return 0;
				}
			}

			assert( m_hWnd != NULL ); // handle should be known now.

			if( ! loop.EndedYet() ) // must not show dialog if OnInitDialog already ended it!
			{
				// Show the window, if the caller hasn't already done so:
				Internal::WindowShowAndHider  wsh( m_hWnd ); // destructor hides it again so it won't operate on any outer message loop!
				auto winWithFocus = ::GetFocus();
				if( winWithFocus != m_hWnd && ! IsChild( m_hWnd, winWithFocus ) )
				{
					::SetFocus( m_hWnd ); // if we've just created the window it'll already be focussed, but not otherwise.
				}

				{
					Internal::WindowDisablerAndReEnabler  windowDisablerAndReEnabler( m_hWndOwner );  // disable owner window

					//
					// MESSAGE LOOP:
					//

					libWinApi::Message msg; // ** FIX:  Enhancement:  We could make exit reason available to the caller?
					loop.DoMessageLoop( &msg, [&]()
					{
						assert( ::IsWindow(m_hWnd) );
						ProcessMessage( &msg );
					} );

				} // end scope to re-enable disabled owner

				// Ensure the owner window is activated:
				if( m_hWndOwner )
				{
					assert( ::IsWindow(m_hWndOwner) );
					if( ::IsWindowVisible(m_hWndOwner) )
					{
						::SetActiveWindow( m_hWndOwner );
					}
				}
					
			}

		} // end scope - StackSocket destructor "unplugs" EndModal(), since our loop is gone.

		return m_EndDialogCode;
	}



	void BaseForm::ProcessMessage( libWinApi::Message *message ) 
	{
		// (Split out, so other parties can implement message loops if desired)

		if( ! PreProcessMessage( message ) ) // Reminder: Base class BaseForm::PreProcessMessage() does IsDialogMessage()
		{
			::TranslateMessage( message );
			libWinApi::Dispatch( *message );
		}
	}



	void BaseForm::EndModal( INT_PTR n )
	{
		if( m_pActiveModalLoop )
		{
			assert( m_EndCodeSetAlready == false ); // DEBUG: fires if you attempt to EndModal() more than once, which could indicate a coding error.
			m_EndDialogCode = n;
			m_EndCodeSetAlready = true;
			m_pActiveModalLoop->End(); // ends our loop
		}
		else assert(false); // The DoModal() loop is not active!
	}



	INT_PTR CALLBACK  NoOperationDlgProc( HWND, UINT, WPARAM, LPARAM )
	{
		// We switch over to this DLGPROC once BaseForm::StaticDialogProc is done.
		return FALSE;
	}
	


	INT_PTR CALLBACK  BaseForm::StaticDialogProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) // static
	{
		// The DLGPROC for a BaseForm during the initial phase until WM_INITDIALOG processed
		// then we switch over to StaticDialogProc_PostInit().

		if( uMsg == WM_INITDIALOG )
		{
			// lParam is the 'this' pointer (BaseForm *)
			assert( lParam != NULL ); // should be -- lParam is the 'this' pointer.
			::SetWindowLongPtr( hWndDlg, DWLP_USER, (LONG_PTR) lParam );

			// Get the 'this' pointer:
			BaseForm *pDlg = reinterpret_cast<BaseForm *>(lParam);

			// Subclass the WNDPROC:
			pDlg->m_hWnd = hWndDlg; // so m_hWnd is guaranteed set for all calls to virtual WindowProc()
			pDlg->m_pPrevWndProc = (WNDPROC) (LONG_PTR) ::GetWindowLongPtr( hWndDlg, GWLP_WNDPROC );
			::SetWindowLongPtr( hWndDlg, GWLP_WNDPROC, (LONG_PTR) &BaseForm::StaticWindowProc );

			// Now that the 'this' pointer is recorded, switch over to the null DLGPROC
			// (Note that WNDPROC processes all the messages).
			::SetWindowLongPtr( hWndDlg, DWLP_DLGPROC, (LONG_PTR) &NoOperationDlgProc );

			// Pass the WM_INITDIALOG to the C++ app:
			return (INT_PTR) BaseForm::StaticWindowProc( hWndDlg, uMsg, wParam, lParam );
		}

		return FALSE; // We did not process this message, ask DefDlgProc to process it.
	}



	LRESULT CALLBACK  BaseForm::StaticWindowProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) // static
	{
		// The WNDPROC for a BaseForm (we swap to this hander post WM_INITDIALOG). 

		// The 'this' pointer is always set by this time.
		// The 'this' pointer is always valid, as the object outlives the window.

		auto pDlg = reinterpret_cast<BaseForm *>( (LONG_PTR) ::GetWindowLongPtr( hWndDlg, DWLP_USER ) );
		assert( pDlg != NULL ); // it should have been set
		assert( hWndDlg == pDlg->GetHWND() ); // should match

		libWinApi::WindowProcArgs e;
		e.hwnd    = hWndDlg;
		e.message = uMsg;
		e.wParam  = wParam;
		e.lParam  = lParam;

		pDlg->WindowProc( e );
		return e.Result;
	}



	void BaseForm::Invalidate()
	{
		if( m_hWnd )
		{
			::InvalidateRect( m_hWnd, NULL, FALSE );
		}
		else assert(false);
	}
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     MS-WINDOWS GLITCH WORKAROUND
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	bool BaseForm::HandleProblemWorkarounds( WindowProcArgs *pMsg )
	{
		// The "Windows pauses my program when certain Non-client-area mouse actions take place" problem
		// Let's start by stubbing out things we definately don't need.
		// R-BUTTON DOWN on NC area is particularly bad!  Pauses message dispatch until the button is released!

		if( pMsg->message >= WM_NCRBUTTONDOWN && pMsg->message <= WM_NCMBUTTONDBLCLK )
		{
			return true;
		}
		else if( pMsg->message == WM_NCLBUTTONDOWN )
		{
			auto hitTestValue = pMsg->wParam;
			auto mousePos = libWinApi::LParamToPoint(pMsg->lParam);

			switch( hitTestValue )
			{
			case HTMINBUTTON:
				::ShowWindow( *this, SW_MINIMIZE );
				return true;

			case HTMAXBUTTON:
				ToggleWindowBetweenMaximizeAndRestore( *this );
				return true;

			case HTCLOSE:
				OnCancel();
				return true;

			case HTCAPTION:
				libWinApi::HandleHitWindowCaption( *this );
				return true;
			}
		}
		else if( pMsg->message == WM_NCLBUTTONDBLCLK )
		{
			if( GetWindowStyle( *this ) & WS_MAXIMIZEBOX )
			{
				ToggleWindowBetweenMaximizeAndRestore( *this );
			}
			return true;
		}

		return false; // let caller handle
	}
}















namespace libWinApi
{
	ModalLoop::ModalLoop()
		: _ModalLoopShouldRun(true) 
	{
	}
}






namespace libWinApi
{
	HMONITOR  MonitorFromWindow( HWND hWnd )
	{
		return ::MonitorFromWindow( hWnd, MONITOR_DEFAULTTONEAREST );
	}



	void BringRectangleOntoMonitorWorkArea( HMONITOR hMonitor, int *pLeft, int *pTop, int Width, int Height )
	{
		MONITORINFO mi;
		memset( &mi, 0, sizeof(mi) );
		mi.cbSize = sizeof(MONITORINFO);
		if( ::GetMonitorInfo( hMonitor, &mi ) )
		{
			auto mw = ::Width(mi.rcWork);
			auto mh = ::Height(mi.rcWork);

			// Update caller's Left and Top as necessary to re-position the window:
			*pLeft = max( mi.rcWork.left, *pLeft );
			*pTop  = max( mi.rcWork.top, *pTop );

			auto Right = *pLeft + Width;
			if( Right > mi.rcWork.right )
			{
				*pLeft = mi.rcWork.right - Width;
			}

			auto Bottom = *pTop + Height;
			if( Bottom > mi.rcWork.bottom )
			{
				*pTop = mi.rcWork.bottom - Height;
			}
		}
	}



	void MoveWindowButNotOffScreen( HMONITOR hMonitor, HWND hWnd, int Left, int Top, int Width, int Height )
	{
		BringRectangleOntoMonitorWorkArea( hMonitor, &Left, &Top, Width, Height );
		::MoveWindow( hWnd, Left, Top, Width, Height, TRUE );
	}


	void PositionWindowAboutScreenPoint( HWND hWnd, POINT p, INT32 mdx, INT32 mdy, bool LimitToScreen )
	{
		assert( hWnd != NULL );
		if( hWnd != NULL )
		{
			RECT r;
			if( ::GetWindowRect( hWnd, &r ) )
			{
				INT32  w = r.right - r.left;
				INT32  h = r.bottom - r.top;
				INT32  l = p.x + (mdx * (w / 2));
				INT32  t = p.y + (mdy * (h / 2));
				if( LimitToScreen )
				{
					auto hMonitor = ::MonitorFromPoint( p, MONITOR_DEFAULTTONEAREST );
					if( hMonitor )
					{
						MoveWindowButNotOffScreen( hMonitor, hWnd, l, t, w, h );
					}
				}
				else
				{
					::MoveWindow( hWnd, l, t, w, h, TRUE );
				}
			}
		}
	}


	void CenterWindowAboutScreenPoint( HWND hWnd, POINT p, bool LimitToScreen )
	{
		PositionWindowAboutScreenPoint( hWnd, p, -1, -1, LimitToScreen );
	}



	void CenterWindow( HWND hWnd, HWND hWndOwner )
	{
		// Center within parent
		assert( hWnd != NULL );
		assert( hWndOwner != NULL );
		if( hWnd != NULL && hWndOwner != NULL )
		{
			RECT rOwner;
			if( ::GetWindowRect( hWndOwner, &rOwner ) )
			{
				// Get parent centre:
				POINT cen;
				cen.x = (rOwner.left + rOwner.right) / 2;
				cen.y = (rOwner.top + rOwner.bottom) / 2;

				CenterWindowAboutScreenPoint( hWnd, cen, false );
			}
		}
	}



	void  CenterWindowPercent( HWND hWnd, INT32 HeightPercent, HWND hWndMeasureAgainst )
	{
		if( HeightPercent >= 0 && HeightPercent <= 100 ) {} else { assert(false); return; }

		RECT r;
		if( ! ::GetWindowRect( hWnd, &r ) ) { assert(false); return; }

		RECT rBack;
		if( hWndMeasureAgainst == NULL || ! ::GetWindowRect( hWndMeasureAgainst, &rBack ) ) 
		{ 
			// There isn't a parent to measure against, so let's try and get the monitor:
			MONITORINFO mi;
			memset( &mi, 0, sizeof(mi) );
			mi.cbSize = sizeof(MONITORINFO);

			auto hMonitor = libWinApi::MonitorFromWindow( hWnd );
			if( hMonitor && ::GetMonitorInfo( hMonitor, &mi ) )
			{
				rBack = mi.rcWork;
			}
			else 
			{
				assert( false ); 
				return;
			}
		}

		auto ow = Width(rBack);
		auto oh = Height(rBack);

		auto oldw = Width(r);
		auto oldh = Height(r);

		auto h = MulDiv( oh, HeightPercent, 100 );
		auto w = MulDiv( h, oldw, oldh );
		if( h < oldh || w < oldw ) 
		{
			// safety, can only expand size
			h = oldh;
			w = oldw;
		}

		// w,h are the new desired dimensions
		auto NewLeft = rBack.left + ((ow - w) / 2);
		auto NewTop  = rBack.top  + ((oh - h) / 2);
		::MoveWindow( hWnd, NewLeft, NewTop, w, h, TRUE );
	}
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FILE BROWSER STANDARD DIALOG
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


namespace libWinApi
{
	namespace Internal
	{
		enum { FileNameBufferSize = 4096 };



		void PrepareFiltersString( const wchar_t *Filters, std::vector<wchar_t> *out_pvecFilters )
		{
			std::vector<wchar_t> &vecFilters = *out_pvecFilters; // alias
			std::copy( Filters, Filters + wcslen(Filters), std::back_inserter(vecFilters) );
			vecFilters.push_back( L'|' ); // ensure terminated
			vecFilters.push_back( L'|' ); // ensure terminated
			std::replace_if( vecFilters.begin(), vecFilters.end(), []( wchar_t ch ) { return ch == L'|'; }, L'\0' );
		}



		void AssignStringToBuffer( const std::wstring &Str, std::vector<wchar_t> *out_pvecFilenameBuffer )
		{
			if( ! out_pvecFilenameBuffer->empty() )
			{
				if( (Str.size() + 1) <= out_pvecFilenameBuffer->size() )   // +1 = include NUL
				{
					const wchar_t *c = Str.c_str();
					std::copy( c, c + Str.size() + 1, out_pvecFilenameBuffer->begin() );
				}
				else
				{
					out_pvecFilenameBuffer->at(0) = L'\0'; // ensure null terminated
				}
			}
		}
	}




	bool  ShowOpenFileDialog( HWND hWndOwner, const wchar_t *WindowTitle, const wchar_t *Filters, std::wstring *out_Choice )
	{
		// Prepare the path string store:

		std::vector<wchar_t> vecFilenameBuffer;
		vecFilenameBuffer.resize( Internal::FileNameBufferSize, L'\0' );
		Internal::AssignStringToBuffer( *out_Choice, &vecFilenameBuffer ); // leaves empty string if can't fit.

		// Prepare the filters string:

		std::vector<wchar_t> vecFilters;
		Internal::PrepareFiltersString( Filters, &vecFilters );

		// Prepare struct:

		OPENFILENAMEW ofn;
		memset( &ofn, 0, sizeof(ofn) );
		ofn.lStructSize    = sizeof(ofn);
		ofn.hwndOwner      = hWndOwner;
		ofn.lpstrFile      = (LPWSTR) &vecFilenameBuffer.front();
		ofn.nMaxFile       = (DWORD) vecFilenameBuffer.size();
		ofn.lpstrTitle     = (LPWSTR) WindowTitle;
		ofn.Flags          = OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES;
		ofn.lpstrFilter    = (LPWSTR) &vecFilters.front();

		// Operate dialog:

		BOOL bResult = ::GetOpenFileNameW( &ofn );
		*out_Choice = bResult ? ofn.lpstrFile : L"";
		return bResult ? true : false;
	}




	bool  ShowSaveFileDialog( HWND hWndOwner, const wchar_t *WindowTitle, const wchar_t *Filters, const wchar_t *DefaultFileExtension, std::wstring *out_Choice )
	{
		// Prepare the path string store:

		std::vector<wchar_t> vecFilenameBuffer;
		vecFilenameBuffer.resize( Internal::FileNameBufferSize, L'\0' );
		Internal::AssignStringToBuffer( *out_Choice, &vecFilenameBuffer ); // leaves empty string if can't fit.

		// Prepare the filters string:

		std::vector<wchar_t> vecFilters;
		Internal::PrepareFiltersString( Filters, &vecFilters );

		// Prepare struct:

		OPENFILENAMEW ofn;
		memset( &ofn, 0, sizeof(ofn) );
		ofn.lStructSize    = sizeof(ofn);
		ofn.hwndOwner      = hWndOwner;
		ofn.lpstrFile      = (LPWSTR) &vecFilenameBuffer.front();
		ofn.nMaxFile       = (DWORD) vecFilenameBuffer.size();
		ofn.lpstrTitle     = (LPWSTR) WindowTitle;
		ofn.Flags          = OFN_ENABLESIZING | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_OVERWRITEPROMPT;
		ofn.lpstrFilter    = (LPWSTR) &vecFilters.front();
		ofn.lpstrDefExt    = DefaultFileExtension;

		// Operate dialog:

		BOOL bResult = ::GetSaveFileNameW( &ofn );
		*out_Choice = bResult ? ofn.lpstrFile : L"";
		return bResult ? true : false;
	}
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     EXEC PROCESS + WAIT TERMINATION
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{

	std::wstring  GetMyExeFilePath()
	{
		std::wstring  CmdLine( ::GetCommandLine() );
		std::vector<std::wstring>  Strings;
		if( libBasic::SplitCommandLine( CmdLine.c_str(), &Strings ) )
		{
			auto NumCmds = Strings.size();
			if( NumCmds >= 1 )
			{
				return Strings[0];
			}
		}
		return L"";
	}




	std::wstring  GetMyExeFolderPath()
	{
		auto str = GetMyExeFilePath();
		std::wstring  RootStr;
		std::wstring  LeafStr;
		if( libBasic::PathRootLeafSplit( str.c_str(), &RootStr, &LeafStr ) )
		{
			return RootStr;
		}
		return L"";
	}

}










// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FRAME BUFFER BITMAP
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	bool CreateDIBSectionFrameBuffer( INT32 frameBufferWidth, INT32 frameBufferHeight, FrameBufferInfo *out_ScreenInfo, HBITMAP *out_HBITMAP )
	{
		if( ! out_ScreenInfo || ! out_HBITMAP ) return false;

		// Create DIB section bitmap for the virtual screen 

		UINT32 bitsPerPixel = 32;

		// Compute widthInBytes for the bitmap (which will round UP to UINT_PTR boundary):
		// Reminder: This is for performance and, in fact, Win32 demands it:

		UINT32 widthInBytes = (bitsPerPixel >> 3) * frameBufferWidth;
		if(widthInBytes & 3) widthInBytes = (widthInBytes | 3) + 1;
		UINT32 sizeInBytes = frameBufferHeight * widthInBytes;

		// Create BITMAPINFOHEADER record:

		BITMAPV4HEADER bitmapStruct;
		memset( &bitmapStruct, 0, sizeof(bitmapStruct) );

		bitmapStruct.bV4Size          = sizeof(bitmapStruct);
		bitmapStruct.bV4Width         = frameBufferWidth;
		bitmapStruct.bV4Height        = frameBufferHeight;
		bitmapStruct.bV4Planes        = 1;
		bitmapStruct.bV4BitCount      = bitsPerPixel;
		bitmapStruct.bV4V4Compression = BI_BITFIELDS;
		bitmapStruct.bV4SizeImage     = 0;
		bitmapStruct.bV4XPelsPerMeter = 1000;
		bitmapStruct.bV4YPelsPerMeter = 1000;
		bitmapStruct.bV4ClrUsed       = 0;
		bitmapStruct.bV4ClrImportant  = 0;
		bitmapStruct.bV4BlueMask      = 0xFF0000;
		bitmapStruct.bV4GreenMask     = 0x00FF00;
		bitmapStruct.bV4RedMask       = 0x0000FF;
		bitmapStruct.bV4AlphaMask     = 0xFF000000;

		void *addressOfBits = 0;

		auto bitmapHandle = ::CreateDIBSection(
			NULL,                         // handle to device context 
			(CONST BITMAPINFO *) &bitmapStruct,    // pointer to structure containing bitmap size, format, and color data 
			DIB_RGB_COLORS,               // color data type indicator: RGB values or palette indices 
			reinterpret_cast<void **>(&addressOfBits), // pointer to variable to receive a pointer to the bitmap’s bit values 
			NULL,                         // optional handle to a file mapping object 
			0);                           // offset to the bitmap bit values within the file mapping object 

		if( bitmapHandle == NULL )
		{
			return false;
		}

		// Return screen stats:
		// We need to account for the DIBSection frame buffer being upside-down.

		auto correctedBase = UINT_PTR(addressOfBits) + (widthInBytes * (frameBufferHeight-1));

		*out_HBITMAP                     = bitmapHandle;
		out_ScreenInfo->BaseAddress      = reinterpret_cast<void *>(correctedBase);
		out_ScreenInfo->BytesPerScanLine = -INT32(widthInBytes);
		out_ScreenInfo->ColourDepth      = bitsPerPixel;
		out_ScreenInfo->WidthPixels      = frameBufferWidth;
		out_ScreenInfo->HeightPixels     = frameBufferHeight;

		return true;
	}

}












// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FOLDERS / PATHS SUPPORT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libBasic
{
	bool PathRootLeafSplit( const wchar_t *Path, std::wstring *out_pRootStr, std::wstring *out_pLeafStr )
	{
		out_pRootStr->clear();
		out_pLeafStr->clear();

		// Find last backslash:
		std::wstring  TmpStr( Path );
		size_t  SlashPos = TmpStr.find_last_of( L'\\' );
		if( SlashPos >= TmpStr.size() ) return false;

		// Truncate the path at the backslash and append the folder name:
		TmpStr.erase( SlashPos + 1 );
		std::swap( TmpStr, *out_pRootStr );

		// Return the leaf:
		const wchar_t *LeafName = Path + SlashPos + 1;
		*out_pLeafStr = std::wstring( LeafName );

		// Success:
		return true;
	}



	bool PathRootLeafSplit( const wchar_t *Path, std::wstring *out_pRootStr, std::wstring *out_pLeafStr, std::wstring *out_pExtensionStr )
	{
		if( PathRootLeafSplit( Path, out_pRootStr, out_pLeafStr ) )
		{
			auto DotPos = out_pLeafStr->find_first_of( L'.' );
			if( DotPos >= out_pLeafStr->size() )
			{
				// No extension (legit)
				out_pExtensionStr->clear();
			}
			else
			{
				// Extension found
				*out_pExtensionStr = std::wstring( out_pLeafStr->begin() + DotPos, out_pLeafStr->end() );
				out_pLeafStr->erase( out_pLeafStr->begin() + DotPos, out_pLeafStr->end() );
			}
			return true;
		}
		return false;
	}



	bool RemoveExtensionIfPresent( const wchar_t *pDotAndExtension, std::wstring &inout_Str )
	{
		std::wstring  tmp(pDotAndExtension);
		if( inout_Str.size() >= tmp.size() )
		{
			std::wstring  rightpart = inout_Str.substr( inout_Str.size() - tmp.size(), tmp.size() );
			if( wcscmp( rightpart.c_str(), tmp.c_str() ) == 0 )
			{ 
				inout_Str.erase( inout_Str.end() - tmp.size(), inout_Str.end() );
				return true;
			}
		}
		return false;
	}

}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     COMMAND LINE APPS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libBasic
{
	bool SplitCommandLine( const wchar_t *Str, std::vector<std::wstring> *out_Strings )
	{
		// The parsing is in the style of a Windows command line where filepaths
		// can have spaces, and thus are enclosed in double-quotes.  The double
		// quotes are stripped off in the list of strings that are returned.

		std::vector<std::wstring>  Strings;
		std::wstring  tmp;
		bool InQuote = false;
		for(;;)
		{
			auto ch = *Str;
			if( ch == 0 )
			{
				if( ! InQuote )
				{
					if( ! tmp.empty() )
					{
						Strings.push_back(tmp); // flush final
					}
					out_Strings->swap( Strings );
					return true;
				}
				return false;
			}
			else if( ch == L' ' && ! InQuote )
			{
				if( ! tmp.empty() )
				{
					Strings.push_back(tmp);
					tmp.clear();
				}
			}
			else if( ch != L'\"' )
			{
				tmp.push_back(ch);
			}
			else
			{
				InQuote = ! InQuote;
			}
			++Str;
		}
	}
}










// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WAVE SOUND OUTPUT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


namespace libWinApi
{
	class WaveBuffer 
	{
	public:
    
		~WaveBuffer();
		bool Init( HWAVEOUT waveHandle, int sizeBytes );
		bool Write( const void *soundBlock, int sizeBytes, int &bytesWritten );
		void Flush();

	private:

		WAVEHDR      _waveHeader;
		HWAVEOUT     _waveOutHandle;
		int          _bytesCount;

	};
}






namespace libWinApi
{
	bool WaveBuffer::Init( HWAVEOUT waveHandle, int sizeBytes )
	{
		_waveOutHandle  = waveHandle;
		_bytesCount = 0;

		// Allocate a buffer and initialize the header.
		_waveHeader.lpData = (LPSTR) LocalAlloc( LMEM_FIXED, sizeBytes );
		if( _waveHeader.lpData == nullptr ) 
		{
			return false;
		}
		_waveHeader.dwBufferLength  = sizeBytes;
		_waveHeader.dwBytesRecorded = 0;
		_waveHeader.dwUser = 0;
		_waveHeader.dwFlags = 0;
		_waveHeader.dwLoops = 0;
		_waveHeader.lpNext = 0;
		_waveHeader.reserved = 0;

		// Prepare it.
		waveOutPrepareHeader( waveHandle, &_waveHeader, sizeof(WAVEHDR) );
		return true;
	}



	WaveBuffer::~WaveBuffer() 
	{
		if( _waveHeader.lpData ) 
		{
			waveOutUnprepareHeader( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
			LocalFree( _waveHeader.lpData );
		}
	}



	void WaveBuffer::Flush()
	{
		// ASSERT(_bytesCount != 0);
		_bytesCount = 0;
		waveOutWrite( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
	}



	bool WaveBuffer::Write( const void *soundBlock, int sizeBytes, int &bytesWritten)
	{
		// ASSERT((DWORD)_bytesCount != _waveHeader.dwBufferLength);
		bytesWritten = min( (int)_waveHeader.dwBufferLength - _bytesCount, sizeBytes );
		CopyMemory( (PVOID)(_waveHeader.lpData + _bytesCount), soundBlock, bytesWritten );
		_bytesCount += bytesWritten;
		if( _bytesCount == (int)_waveHeader.dwBufferLength ) 
		{
			// Write it!
			_bytesCount = 0;
			waveOutWrite( _waveOutHandle, &_waveHeader, sizeof(WAVEHDR) );
			return true;
		}
		return false;
	}

}













namespace libWinApi
{
	// With thanks to MSDN for wave play example.  (Since somewhat tuned)

	WaveOutputStream::WaveOutputStream( int sampleRateHz, int bytesPerSample, int channelCount, int bufferCount, int bufferSizeBytes ) 
		: _bufferCount( bufferCount )
		, _currentBuffer(0)
		, _noBuffer(true)
		, _semaphoreHandle( CreateSemaphore( NULL, bufferCount, bufferCount, NULL ) )
		, _waveBuffers( new WaveBuffer[bufferCount] )
		, _waveOutHandle( NULL )
	{
		WAVEFORMATEX  format;
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels  = channelCount;
		format.nSamplesPerSec = sampleRateHz;
		format.wBitsPerSample = bytesPerSample * 8;
		format.nBlockAlign = bytesPerSample;
		format.cbSize = 0;

		// Create wave device.
		waveOutOpen( &_waveOutHandle,
					WAVE_MAPPER,
					&format,
					(DWORD_PTR) WaveOutputStream::WaveCallback,
					(DWORD_PTR) _semaphoreHandle,
					CALLBACK_FUNCTION);

		// Initialize the wave buffers.
		for (int i = 0; i < bufferCount; i++) 
		{
			_waveBuffers[i].Init( _waveOutHandle, bufferSizeBytes );
		}
	}



	void CALLBACK WaveOutputStream::WaveCallback( HWAVEOUT hWave, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 ) // static member function
	{
		if( uMsg == WOM_DONE ) 
		{
			ReleaseSemaphore( (HANDLE) dwUser, 1, NULL );
		}
	}



	WaveOutputStream::~WaveOutputStream()
	{
		// First, get the buffers back.
		waveOutReset(_waveOutHandle);

		// Free the buffers.
		delete [] _waveBuffers;

		// Close the wave device.
		waveOutClose(_waveOutHandle);

		// Free the semaphore.
		CloseHandle(_semaphoreHandle);
	}



	void WaveOutputStream::Flush()
	{
		if( ! _noBuffer ) 
		{
			_waveBuffers[_currentBuffer].Flush();
			_noBuffer = true;
			_currentBuffer = (_currentBuffer + 1) % _bufferCount;
		}
	}



	void WaveOutputStream::Reset()
	{
		waveOutReset(_waveOutHandle);
	}



	void WaveOutputStream::Write( const void *soundBlock, int sizeBytes )
	{
		while( sizeBytes != 0 ) 
		{
			// Get a buffer if necessary.
			if ( _noBuffer ) 
			{
				WaitOnTheSemaphore();
				_noBuffer = false;
			}

			// Write into a buffer.
			int nWritten;
			if (_waveBuffers[_currentBuffer].Write( soundBlock, sizeBytes, nWritten) ) 
			{
				_noBuffer = true;
				_currentBuffer = (_currentBuffer + 1) % _bufferCount;
				sizeBytes -= nWritten;
				((const uint8_t *&) soundBlock) += nWritten;
			} 
			else 
			{
				// ASSERT(nWritten == nBytes);
				break;
			}
		}
	}


	/* not used
	void WaveOutputStream::Wait()
	{
		// Send any remaining buffers.
		Flush();

		// Wait for the buffers back.
		for (int i = 0; i < _bufferCount; i++) 
		{
			WaitOnTheSemaphore();
		}

		LONG previousCount;
		ReleaseSemaphore( _semaphoreHandle, _bufferCount, &previousCount );
	}
	*/



	void WaveOutputStream::WaitOnTheSemaphore()
	{
		WaitForSingleObject( _semaphoreHandle, INFINITE );
	}



}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     THREAD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	Thread::Thread()
	{
		_shouldTerminate = false;
		_threadHandle = NULL;
	}



	Thread::~Thread()
	{
		if( _threadHandle != NULL )
		{
			RequestTermination(); // Reminder - calls THIS CLASS ONLY (derived is gone at this point)
			WaitForTermination();
			CloseHandle( _threadHandle );
			_threadHandle = NULL;
		}
	}



	void Thread::CreateAndRun()
	{
		if( _threadHandle == NULL )
		{
			DWORD threadId = 0; // not massively important to store this
			_threadHandle = ::CreateThread( NULL, DefaultStackSizeBytes, &ThreadProcedure, this, STACK_SIZE_PARAM_IS_A_RESERVATION, &threadId );
			if( _threadHandle == NULL )
			{
				throw std::exception( "The system is out of resources, cannot create a new thread." );
			}
		}
	}



	void Thread::RequestTermination()
	{
		_shouldTerminate = true;
	}
	
	
	
	void Thread::WaitForTermination()
	{
		if( _threadHandle != NULL )
		{
			::WaitForSingleObject( _threadHandle, INFINITE );
		}
	}



	bool Thread::ShouldTerminate() const
	{
		return _shouldTerminate;
	}


	DWORD WINAPI Thread::ThreadProcedure( LPVOID parameter ) // static member function
	{
		// Call derived class to perform thread tasks.
		auto thisThread = reinterpret_cast<Thread *>( parameter );
		return (DWORD) thisThread->ThreadMain();
	}
}







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     USER'S APPLICATION DATA PATH
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	std::wstring  GetUserAppDataPath()
	{
		wchar_t  Path[MAX_PATH + 1];
		HRESULT hr = ::SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, Path );
		if( SUCCEEDED(hr) )
		{
			return std::wstring(Path) + L"\\";
		}

		assert(FALSE);
		return std::wstring( L"C:\\" );
	}
}


