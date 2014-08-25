//
// Microsoft Windows Library
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//

#pragma once

#include <vector>
#include <assert.h>
#include "winnt.h"
#include <mmsystem.h>
#include "stdint.h"


extern "C" IMAGE_DOS_HEADER __ImageBase;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//      RECT and POINT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

inline SIZE MakeSIZE( int cx, int cy )
{
	SIZE s;
	s.cx = cx;
	s.cy = cy;
	return s;
}

inline POINT MakePOINT( int x, int y )
{
	POINT p;
	p.x = x;
	p.y = y;
	return p;
}

inline RECT MakeRECT( int l, int t, int r, int b )
{
	RECT rc;
	rc.left   = l;
	rc.top    = t;
	rc.right  = r;
	rc.bottom = b;
	return rc;
}

inline int Width(  RECT r )        { return r.right - r.left; }
inline int Height( RECT r )        { return r.bottom - r.top; }
inline SIZE MakeSIZE( RECT r )     { SIZE sz; sz.cx = Width(r); sz.cy = Height(r); return sz; }







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WINDOWS API ROUTINES
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define libWinApi_WindowClassName          L"libWinApi_Window"



namespace libWinApi
{
	struct Message: public MSG
	{
		Message();

		bool IsErase() const;
		bool IsPaint() const;

		bool IsKeyDown() const;
		bool IsKeyDown( UINT32 VkCode ) const;
		bool IsKeyDown( UINT32 *out_VkCode ) const;

		bool IsKeyUp() const;
		bool IsKeyUp( UINT32 VkCode ) const;
		bool IsKeyUp( UINT32 *out_VkCode ) const;

		bool IsMouseMove() const;
		bool IsMouseMove( POINT *out_CursorPos ) const;

		bool IsMouseLDown() const;
		bool IsMouseLDown( POINT *out_CursorPos ) const;
		bool IsMouseLUp() const;
		bool IsMouseLUp( POINT *out_CursorPos ) const;
		bool IsMouseLDoubleClick() const;
		bool IsMouseLDoubleClick( POINT *out_CursorPos ) const;

		bool IsMouseRDown() const;
		bool IsMouseRDown( POINT *out_CursorPos ) const;
		bool IsMouseRUp() const;
		bool IsMouseRUp( POINT *out_CursorPos ) const;
		bool IsMouseRDoubleClick() const;
		bool IsMouseRDoubleClick( POINT *out_CursorPos ) const;

		bool IsNotify() const;

		bool IsCommand() const;
		bool IsMenuCommand() const;
		bool IsMenuCommand( UINT16 *out_CommandID ) const;
		
		bool IsDeactivated() const;

		void Repost() const;
		void RepostTo( HWND hWnd ) const;
		void ResendTo( HWND hWnd ) const;

		bool IsGetMinMax() const;

		void ChangeToKeyDownMessage( UINT32 VkCode );
		void ChangeTheKey( UINT32 VkCode );
		bool TurnWheelMessageIntoKeyPressMessage();

	private:

		bool HandleMouseMessage( UINT32 MouseMsg, POINT *out_CursorPos ) const;
	};



	struct WindowProcArgs: public Message
	{
		WindowProcArgs() 
			: Result(0) 
		{
		}

		void RespondWithMinTrackSize( SIZE sz );

		LRESULT  Result;
	};

	POINT LParamToPoint( LPARAM lParam );    // for use with unpacking coordinates from mouse messages  (SIGNED coordinates!)

	void  CenterWindow( HWND hWnd, HWND hWndOwner );
	void  CenterWindowAboutScreenPoint( HWND hWnd, POINT p, bool LimitToScreen );
	void  CenterWindowPercent( HWND hWnd, INT32 HeightPercent, HWND hWndMeasureAgainst );
	
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     STACK PLUG and SOCKET
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	template<typename T>
	class StackSocket;  // below

		
	template<typename T>
	class StackPlug   // is anywhere
	{
	public:
		StackPlug()                    : _pTarget(NULL) {}
		~StackPlug()                   { assert(_pTarget==NULL); } // should have been unplugged by ~StackSocket() being called first?
		T &operator *()                { return *_pTarget; }
		const T &operator *() const    { return *_pTarget; }
		T *operator->()                { return _pTarget; }
		const T *operator->() const    { return _pTarget; }
		operator bool() const          { return _pTarget != NULL; }
	private:
		friend class StackSocket<T>;
		T *_pTarget;
	};


	template<typename T>
	class StackSocket  // is on the stack
	{
	public:
		StackSocket( StackPlug<T> &ThePlug, T *pTarget )
		{
			ThePlug._pTarget = pTarget;
			_pClient = &ThePlug;
		}
		~StackSocket()
		{
			if(_pClient)
			{
				_pClient->_pTarget = NULL;
				_pClient = NULL;
			}
		}
	private:
		StackPlug<T> *_pClient;
		StackSocket( const StackSocket & );
		void operator=( const StackSocket & );
	};
}

			








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     BASE DIALOG
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	class ModalLoop; // below


	void ToggleWindowBetweenMaximizeAndRestore( HWND hWnd );



	class BaseForm
	{
	public:

		// Base dialog class.
		// Supports MODAL and MODELESS operation.
		// Reminder: WM_DESTROY / WM_NCDESTROY doesn't come through the WindowProc.  Technique has been to override EndModal() then call the base.

		BaseForm( HWND hWndOwner, UINT nIDTemplate );

		void SetAllowFamilyResizing( bool b ) { m_AllowOlderFamilyResizing = b; } // default is true

		virtual ~BaseForm();
		virtual bool PreProcessMessage( libWinApi::Message *pMsg ); // You must call the base to have IsDialogMessage() done!   Override this to filter POSTED messages (eg: key / mouse).  Return false to drop the message, or true to dispatch it.
		virtual bool OnInitDialog();
		virtual void OnOK();                  // The base calls EndModal(IDOK).  If you override this, you must decide whether to end or not.
		virtual void OnCancel();              // The base calls EndModal(IDCANCEL).  If you override this, you must decide whether to end or not.
		virtual INT_PTR DoModal();            // Operate the dialog modally.  Note that this will call Create() internally, if that has not yet been done.
		virtual void EndModal( INT_PTR n );   // The base class function is only available within DoModal()'s loop.  Outside the loop, it is a no-operation.
		virtual void WindowProc( libWinApi::WindowProcArgs &e );

		bool Create();               // Create the dialog.  You can show it modelessly with ShowWindow().

		void ShowAndMaximise();

		void Invalidate();  // Invalidate the client area of the dialog.

		void SetBigAndSmallIcons( UINT iconResourceID ); // call in OnInitDialog
		void ReleaseIcons();

		operator HWND() const              { return m_hWnd; }
		HWND  GetHWND() const              { return m_hWnd; }
		HWND  GetOwner() const             { return m_hWndOwner; }

		void ProcessMessage( libWinApi::Message * );   // (Split out, so other parties can implement message loops if desired)

	private:

		static INT_PTR CALLBACK StaticDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );  // NB: DLGPROC
		static LRESULT CALLBACK StaticWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );  // NB: WNDPROC

		void DoVirtualCall_OnOK();
		void DoVirtualCall_OnCancel();
		bool HandleProblemWorkarounds( WindowProcArgs *pMsg );

	private:

		HWND      m_hWnd;
		HWND      m_hWndOwner;
		HICON     _bigIcon;
		HICON     _smallIcon;
		LPCTSTR   m_lpszTemplateName;
		INT_PTR   m_EndDialogCode;
		WNDPROC   m_pPrevWndProc;
		bool      m_EndCodeSetAlready;  // a debug facility
		bool      m_AllowOlderFamilyResizing;
		bool      m_MaximiseOnReshowReminder; // This restores non-backsheet windows to MAXIMIZED state, if they were maximised when the back sheet was minimised.  [corrects an arguable windows bug]

		StackPlug<ModalLoop>  m_pActiveModalLoop;

	};
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WM_COMMAND dispatcher
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	class WmCommandDispatcher
	{
	public:

		WmCommandDispatcher( libWinApi::WindowProcArgs &e ) 
			: wParam(e.wParam)
			, lParam(e.lParam)
			, CtrlID(LOWORD(e.wParam))
		{
		}

		inline bool Is( UINT32 CommandID )
		{
			return HIWORD(wParam) == CommandID && lParam != 0;
		}

		inline bool IsFromMenu()
		{
			return HIWORD(wParam) == 0 && lParam == 0;
		}

		template<typename LAMBDA>
		inline void Case( WORD DlgItemID, LAMBDA Lambda )
		{
			if( DlgItemID == CtrlID ) 
			{ 
				Lambda();
			}
		}

		template<typename FUNC, typename FORM_CLASS>
		inline bool Case( WORD DlgItemID, FORM_CLASS *pThis, FUNC HandlerFunction )
		{
			if( DlgItemID == CtrlID ) 
			{ 
				((*pThis).*HandlerFunction)(); 
				return true;
			}
			return false;
		}

		WORD  GetCtrlID() const { return CtrlID; }

	private:
		WPARAM wParam;
		LPARAM lParam;
		WORD  CtrlID;
	};
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WM_PAINT handler
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{	
	// NB: AreaIsEmpty() is used to filter Raymond chen's 0*0 message

	class WmPaintHandler
	{
	public:
		WmPaintHandler( HWND hWnd );
		WmPaintHandler( libWinApi::WindowProcArgs &e );
		~WmPaintHandler();
		bool AreaIsEmpty() const            { return ps.rcPaint.right <= ps.rcPaint.left || ps.rcPaint.bottom <= ps.rcPaint.top; }
		bool ClipToUpdateRect();  // NB: does AreaIsEmpty() test
		HDC dc;
		PAINTSTRUCT ps;
	private:
		void Init( HWND hWnd );
		HWND m_hWnd;
	};
}













// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FILE BROWSER STANDARD DIALOG
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	//
	// Open / Save As file browser common dialogs
	//
	// Filters must be an MFC-style specification string, eg: L"Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"
	// Filters can be NULL to show no filters.
	// inout_Choice can be the empty string on entry to both functions to have no default.
	//
	bool  ShowOpenFileDialog( HWND hWndOwner, const wchar_t *WindowTitle, const wchar_t *Filters, std::wstring *inout_Choice );
	bool  ShowSaveFileDialog( HWND hWndOwner, const wchar_t *WindowTitle, const wchar_t *Filters, const wchar_t *DefaultFileExtension, std::wstring *inout_Choice );
}











// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WINDOWS MESSAGING
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	void Dispatch( libWinApi::Message &msg );   // Avoid DispatchMessage() API's swallowing of exceptions.
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     MODAL LOOP
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	class ModalLoop   // allocate on the stack above the loop, not as a member variable of a form
	{
	public:

		// Provides the loop for a modal loop including standard GetMessage() checks
		// and WM_QUIT handling.
		//
		// Provides a termination flag so that partied called by the loop can 
		// set a flag that tells the loop to immediately exit without fetching 
		// another message.
		//
		// - The End() member function sets the 'end' flag.

		ModalLoop();

		bool EndedYet() const             { return ! _ModalLoopShouldRun; }

		void End()                        { _ModalLoopShouldRun = false; }
			// Asks the loop to exit, when it is returned to.
			// This is a sticky flag.



		template<typename MESSAGE_HANDLER>
		void DoMessageLoop( libWinApi::Message *out_Msg, MESSAGE_HANDLER &Handler ) // void Handler()
		{
			// Run a message processing loop which calls the Handler for all messages.
			// WM_QUIT is handled by this routine, as well as GetMessage() protocol.

			// Reminder: It is considered a legitimate case that End() may have been
			// called BEFORE this function is called, thus _ModalLoopShouldRun is 'false' already.

			while( _ModalLoopShouldRun )
			{ 
				auto getMessageResult = ::GetMessage( out_Msg, NULL, 0, 0 );
				if( getMessageResult == 0 || getMessageResult == -1 )
				{
					// Per Raymond Chen: Received WM_QUIT so GetMessage() returns 0.
					// GetMessage() returns -1 for an error case, which we do the same thing.
					// We exit now, and re-post WM_QUIT for the outer message loop.
					if( getMessageResult == -1 ) assert(false); // just to let you know this happened.
					::PostQuitMessage( (int) out_Msg->wParam );
					break;
				}
				else
				{
					Handler(); // Reminder:  Handler can do ModalLoop::End() to end.
				}
			}
		}

	private:

		bool _ModalLoopShouldRun;

	};

}







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     EXEC PROCESS + WAIT TERMINATION
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	std::wstring  GetMyExeFilePath();
	std::wstring  GetMyExeFolderPath();
	std::wstring  GetUserAppDataPath();
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FRAME BUFFER BITMAP
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	struct FrameBufferInfo
	{
		void             *BaseAddress;      // /* Address of top left row. (Is *still* visually top left if frame buffer upside down) */
		INT32            WidthPixels;       // /* Must be positive */
		INT32            HeightPixels;      // /* Must be positive */
		INT32            BytesPerScanLine;  // /* Can be negative if the frame buffer is up-side down */
		UINT32           ColourDepth;       // /* Colour depth, bits per pixel */
	};

	bool CreateDIBSectionFrameBuffer( INT32 Width, INT32 Height, FrameBufferInfo *out_ScreenInfo, HBITMAP *out_HBITMAP );
}









// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     BINARY IMAGE  /  STRING FILE OPS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libBasic
{
	bool PathRootLeafSplit( const wchar_t *Path, std::wstring *out_pRootStr, std::wstring *out_pLeafStr );
	bool PathRootLeafSplit( const wchar_t *Path, std::wstring *out_pRootStr, std::wstring *out_pLeafStr, std::wstring *out_pExtensionStr );
	bool RemoveExtensionIfPresent( const wchar_t *pDotAndExtension, std::wstring &inout_Str );

}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     COMMAND LINE APPS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libBasic
{
	bool SplitCommandLine( const wchar_t *Str, std::vector<std::wstring> *out_Strings );
		// The parsing is in the style of a Windows command line where filepaths
		// can have spaces, and thus are enclosed in double-quotes.  The double
		// quotes are stripped off in the list of strings that are returned.
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     WAVE SOUND OUTPUT
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	class WaveOutputStream 
	{
	public:

		WaveOutputStream( int sampleRateHz, int bytesPerSample, int channelCount, int bufferCount, int bufferSizeBytes );
		~WaveOutputStream();
		void Write( const void *soundBlock, int sizeBytes );
		void Flush();
		// void Wait();
		void Reset();

	private:

		const HANDLE   _semaphoreHandle;
		const int      _bufferCount;
		int            _currentBuffer;
		BOOL           _noBuffer;
		HWAVEOUT       _waveOutHandle;
		class WaveBuffer  *_waveBuffers;

		static void CALLBACK WaveCallback( HWAVEOUT hWave, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );

		void WaitOnTheSemaphore();

	};
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     THREAD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

namespace libWinApi
{
	class Thread
	{
	public:

		Thread();
		virtual ~Thread();
		
		void CreateAndRun();
			// Call this to create and run your thread.

		virtual int32_t ThreadMain() = 0;
			// Override this an implement your thread activity.
		
		virtual void RequestTermination();   
			// Base class sets the ShouldTerminate() flag.  Derived may 
			// need to perform extra tasks eg: signal internal events.
			// - If you override this, be sure to override the destructor
			//   and call your RequestTermination() from that.

		void WaitForTermination();
			// Thread Join.
			// Allows external party to halt until this thread terminates.
			// This takes no action if the thread is already terminated.

	protected:

		bool ShouldTerminate() const;

	private:

		HANDLE _threadHandle;
		volatile bool _shouldTerminate;
		static DWORD WINAPI ThreadProcedure( LPVOID parameter );
		enum { DefaultStackSizeBytes = 1048576 };

	};
}




