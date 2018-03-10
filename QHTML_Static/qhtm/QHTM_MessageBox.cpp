/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_MessageBox.cpp
Owner:	russf@gipsysoft.com
Purpose:	Replacement for the boring old MessageBox function. Adds
					HTML instead of plain text.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLSection.h"
#include "QHTM.h"

#ifdef QHTM_ALLOW_HTML_MESSAGEBOX

static HHOOK g_hook;
static HINSTANCE g_hInstance;
static LPCTSTR g_pcszCaption = NULL;

extern void GetDisplayWorkArea( HWND hwnd, RECT &rc );

static void ChangeMessageBoxToHTML( HWND hwnd )
{
	//
	//	Find our static control
	HWND hwndStatic = GetWindow( hwnd, GW_CHILD );
	//
	//	Bit dependent really, but we search for a control that has -1 as the ID, this seems to be the static control
	//	with the text on it.
	//	NOTE: If this proves troublesome then we could search for a STATIC with the correct styles (missing the static with
	//	the icon).
	UINT uID = 0;
	while( hwndStatic )
	{
		uID = GetWindowLongPtr( hwndStatic, GWL_ID );
		if( uID >= 0xffff )
			break;
		hwndStatic = GetWindow( hwndStatic, GW_HWNDNEXT );
	}

	if( hwndStatic )
	{
		//
		//	Store this because we'll need it when moving the buttons
		WinHelper::CRect rcWindow;
		GetWindowRect( hwnd, rcWindow );

		WinHelper::CRect rcStaticWindow;
		GetWindowRect( hwndStatic, rcStaticWindow );

		//
		//	Use DisplayArea because we may have dual monitors of different resolutions and things look sick if we don't
		WinHelper::CRect rcScreen;
		GetDisplayWorkArea( hwnd, rcScreen );
		const int nMaxScreenWidth = rcScreen.Width() / 2;
		const int nMaxScreenHeight = rcScreen.Height() / 2;

		//
		//	Get the text from the window to give to our newly create QHTM window
		UINT uBufferLength = GetWindowTextLength( hwndStatic );
		LPTSTR pszBuffer = new TCHAR[ uBufferLength + 1 ];
		GetWindowText( hwndStatic, pszBuffer, uBufferLength + 1 );

		//
		//	Get the font so we can set it later.
		HFONT hfont = (HFONT)SendMessage( hwndStatic, WM_GETFONT, 0, 0 );

		//
		//	Discard the old window.
		DestroyWindow( hwndStatic );

		//
		//	Now we have our window we need to change some sizes...

		hwndStatic = CreateWindowEx( WS_EX_TRANSPARENT, QHTM_CLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, nMaxScreenWidth, nMaxScreenHeight, hwnd, (HMENU)uID, g_hInstance, NULL );
		SendMessage( hwndStatic, WM_SETFONT, (WPARAM)hfont, 0 );

		QHTM_SetUseColorStatic( hwndStatic, TRUE );

		WinHelper::CRect rcMargins( 0, 0, 0, 0 );
		QHTM_SetMargins( hwndStatic, rcMargins );

		SetWindowText( hwndStatic, pszBuffer );
		WinHelper::CSize size;
		QHTM_GetDrawnSize( hwndStatic, &size );

		ASSERT( hwndStatic );

		//
		//	Figure out the size of the QHTM window
		WinHelper::CRect rcNewStaticWindow( rcStaticWindow );
		//	OBO error fix.
		size.cx++;
		if( size.cy > nMaxScreenHeight )
			size.cy = nMaxScreenHeight;


		//
		//	Try to ensure that the new size isn't so small that the buttons spew out of the message box.
		{
			int nMinimum = 100000;
			int nMaximum = 0;
			HWND hwndButton = GetWindow( hwnd, GW_CHILD );
			static const TCHAR szButton[] = _T("BUTTON");
			TCHAR szTest[]                = _T("      ");
			while( hwndButton )
			{
				if( GetClassName( hwndButton, szTest, countof( szTest ) ) )
				{
					if( !_tcsnicmp( szTest, szButton, countof( szTest ) - 1 ) )
					{
						WinHelper::CRect rcButton;
						GetWindowRect( hwndButton, rcButton );
						nMinimum = min( nMinimum, rcButton.left );
						nMaximum = max( nMaximum, rcButton.right );
					}
				}
				hwndButton = GetWindow( hwndButton, GW_HWNDNEXT );
			}

			if( size.cx < nMaximum - nMinimum )
			{
				size.cx = nMaximum - nMinimum;
			}

		}

		rcNewStaticWindow.SetSize( size );
		MapWindowPoints( HWND_DESKTOP, hwnd, reinterpret_cast<LPPOINT>(&rcNewStaticWindow.left), 2 );
		SetWindowPos( hwndStatic, NULL, rcNewStaticWindow.left, rcNewStaticWindow.top, rcNewStaticWindow.Width(), rcNewStaticWindow.Height(), SWP_NOZORDER );


		//
		//	Adjust the size and position of the dialog due to the different size.
		const int nDeltaX = rcNewStaticWindow.Width() - rcStaticWindow.Width();
		const int nDeltaY = rcNewStaticWindow.Height() - rcStaticWindow.Height();
		rcWindow.right += nDeltaX;
		rcWindow.bottom += nDeltaY;
		SetWindowPos( hwnd, NULL, rcWindow.left - nDeltaX / 2, rcWindow.top - nDeltaY / 2, rcWindow.Width(), rcWindow.Height(), SWP_NOZORDER );


		//
		//	Move all buttons by the delta between the new and old size of the dialog
		{
			HWND hwndButton = GetWindow( hwnd, GW_CHILD );
			static const TCHAR szButton[] = _T("BUTTON");
			TCHAR szTest[]                = _T("      ");
			while( hwndButton )
			{
				if( GetClassName( hwndButton, szTest, countof( szTest ) ) )
				{
					if( !_tcsnicmp( szTest, szButton, countof( szTest ) - 1 ) )
					{
						WinHelper::CRect rcButton;
						GetWindowRect( hwndButton, rcButton );
						rcButton.Offset( nDeltaX / 2, nDeltaY );
						MapWindowPoints( HWND_DESKTOP, hwnd, reinterpret_cast<LPPOINT>(&rcButton.left), 2 );
						SetWindowPos( hwndButton, NULL, rcButton.left, rcButton.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOSENDCHANGING );
					}
				}
				hwndButton = GetWindow( hwndButton, GW_HWNDNEXT );
			}
		}

		delete[] pszBuffer;
	}
#ifdef _DEBUG
	else
	{
		QHTM_TRACE( _T("Couldn't find the static control\n") );
	}
#endif	//	_DEBUG

}


static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( nCode < 0 )
		return CallNextHookEx( g_hook, nCode, wParam, lParam );

	LPCWPRETSTRUCT lpmsg = (LPCWPRETSTRUCT)lParam;
	if( g_pcszCaption && lpmsg->message == WM_INITDIALOG )
	{
		UINT uBufferLength = GetWindowTextLength( lpmsg->hwnd );
		LPTSTR pszBuffer = new TCHAR[ uBufferLength + 1 ];
		GetWindowText( lpmsg->hwnd, pszBuffer, uBufferLength + 1 );
		if( !_tcscmp( pszBuffer, g_pcszCaption ) )
		{
			//
			//	NULL our caption so we don't get another popup coming along (unlikely but it could happen!)
			g_pcszCaption = NULL;

			ChangeMessageBoxToHTML( lpmsg->hwnd );

			//
			//	Could we unhook at this point? I'm not sure so I'll leave it until the dialog is finished.
		}
		delete[] pszBuffer;
	}

  return ::CallNextHookEx( g_hook, nCode, wParam, lParam );
}

#endif	//	QHTM_ALLOW_HTML_MESSAGEBOX


int WINAPI QHTM_MessageBox(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
{
#ifdef QHTM_ALLOW_HTML_MESSAGEBOX
	g_hInstance = (HINSTANCE)GetWindowLongPtr( hwnd, GWLP_HINSTANCE );

	g_pcszCaption = lpCaption;
	g_hook = SetWindowsHookEx( WH_CALLWNDPROCRET, HookProc, g_hInstance,  GetCurrentThreadId() );

	int result = MessageBox( hwnd, lpText, lpCaption, uType );
	UnhookWindowsHookEx( g_hook );
	return result;
#else	//	QHTM_ALLOW_HTML_MESSAGEBOX
	UNREF( hwnd );
	UNREF( lpText );
	UNREF( lpCaption );
	UNREF( uType );
	return 0;
#endif	//	QHTM_ALLOW_HTML_MESSAGEBOX
}

