/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_SetHTMLButton.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "Defaults.h"
#include "HtmlSection.h"

#ifdef QHTM_ALLOW_HTML_BUTTON

#define QHTM_SUBCLASSED_BUTTON_WNDPROC_PROP	_T("QHTM_OldWindowProc")
#define QHTM_SUBCLASSED_BUTTON_QHTM_PROP	_T("QHTM_QHTM_Window")
#define QHTM_SUBCLASSED_QHTM_WNDPROC_PROP	_T("QHTM_WindowSubClass")


static void SizeHTML( HWND hwndButton, HWND hwndQHTM )
{
	bool bButtonPushed = SendMessage( hwndButton, BM_GETSTATE, 0, 0 ) & BST_PUSHED ? true : false;
	WinHelper::CRect rc, rcButton;
	GetWindowRect( hwndButton, &rcButton );
	rc = rcButton;

	const int nVerticalBorder = GetSystemMetrics( SM_CXEDGE );
	const int nHorizontalBorder = GetSystemMetrics( SM_CYEDGE );
	//	REVIEW - russf - need to find a better way of figuring out where to place the control
	rc.bottom -= ( nVerticalBorder + 3 );
	rc.top += (nVerticalBorder + 3);
	rc.left += (nHorizontalBorder + 4 );
	rc.right -= ( nHorizontalBorder + 5 );
	if( bButtonPushed )
	{
		rc.top++;
		rc.left++;
		rc.right++;
	}
	MapWindowPoints( HWND_DESKTOP, hwndButton, reinterpret_cast<LPPOINT>(&rc.left), 2 );
	SetWindowPos( hwndQHTM, NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOZORDER );
}


LRESULT FAR PASCAL QHTMSubClassFunc( HWND hwnd, WORD msg, WORD wParam, LONG lParam )
{
	WNDPROC old = reinterpret_cast< WNDPROC >( GetProp( hwnd, QHTM_SUBCLASSED_QHTM_WNDPROC_PROP ) );
	if( msg == WM_NCHITTEST )
		return HTTRANSPARENT;

	return CallWindowProc( old, hwnd, msg, wParam, lParam );
}


LRESULT FAR PASCAL QHTM_BUTTON_SubClassFunc( HWND hwnd, WORD msg, WORD wParam, LONG lParam )
{
	LRESULT lr = FALSE;
	WNDPROC old = reinterpret_cast< WNDPROC >( GetProp( hwnd, QHTM_SUBCLASSED_BUTTON_WNDPROC_PROP ) );
	switch( msg )
	{
	case WM_CTLCOLORSTATIC:
		return SendMessage( GetParent( hwnd ), msg, wParam, lParam );

	case WM_SETTEXT:
		{
			HWND hwndQHTM = reinterpret_cast< HWND >( GetProp( hwnd, QHTM_SUBCLASSED_BUTTON_QHTM_PROP ) );
			SendMessage( hwndQHTM, WM_SETTEXT, wParam, lParam );
		}
		break;

	case WM_LBUTTONDOWN:
		lr = CallWindowProc( old, hwnd, msg, wParam, lParam );
		{
			HWND hwndQHTM = reinterpret_cast< HWND >( GetProp( hwnd, QHTM_SUBCLASSED_BUTTON_QHTM_PROP ) );
			SizeHTML( hwnd, hwndQHTM );
		}
		break;		

	case WM_MOUSEMOVE:
		if( GetCapture() == hwnd )
		{
			HWND hwndQHTM = reinterpret_cast< HWND >( GetProp( hwnd, QHTM_SUBCLASSED_BUTTON_QHTM_PROP ) );
			SizeHTML( hwnd, hwndQHTM );
		}
		lr = CallWindowProc( old, hwnd, msg, wParam, lParam );
		break;

	case WM_SIZE:
		lr = CallWindowProc( old, hwnd, msg, wParam, lParam );
		{
			HWND hwndQHTM = reinterpret_cast< HWND >( GetProp( hwnd, QHTM_SUBCLASSED_BUTTON_QHTM_PROP ) );
			SizeHTML( hwnd, hwndQHTM );
		}
		break;

	default:
		lr = CallWindowProc( old, hwnd, msg, wParam, lParam );
	};

	return lr;
}

#endif	//	QHTM_ALLOW_HTML_BUTTON


extern "C" BOOL WINAPI QHTM_SetHTMLButton( HWND hwndButton )
{
#ifdef QHTM_ALLOW_HTML_BUTTON
	UINT uTextSize = ::GetWindowTextLength( hwndButton );
	LPTSTR pszText = new TCHAR [ uTextSize + 1 ];
	::GetWindowText( hwndButton, pszText, uTextSize + 1 );
	SetWindowText( hwndButton, _T(" ") );

	LONG_PTR lp = ::GetWindowLongPtr( hwndButton, GWLP_WNDPROC );
	::SetProp( hwndButton, QHTM_SUBCLASSED_BUTTON_WNDPROC_PROP, reinterpret_cast< HANDLE >( lp ) );
	::SetWindowLongPtr( hwndButton, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( QHTM_BUTTON_SubClassFunc ) );
	::SetWindowLong( hwndButton, GWL_STYLE, ::GetWindowLong( hwndButton, GWL_STYLE ) );
	

	HFONT hFont = (HFONT)::SendMessage( hwndButton, WM_GETFONT, 0, 0 );

	HWND hwndQHTM = CreateWindowEx( WS_EX_TRANSPARENT, QHTM_CLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwndButton, NULL, (HINSTANCE)GetWindowLongPtr( hwndButton, GWLP_HINSTANCE ), NULL );
	SetProp( hwndButton, QHTM_SUBCLASSED_BUTTON_QHTM_PROP, reinterpret_cast< HANDLE >( hwndQHTM ) );

	QHTM_EnableScrollbars( hwndQHTM, FALSE );
	WinHelper::CRect rcMargins( 0, 0, 0, 0 );
	QHTM_SetMargins( hwndQHTM, rcMargins );
	SizeHTML( hwndButton, hwndQHTM );

	SendMessage( hwndQHTM, WM_SETFONT, (WPARAM)hFont, 0 );
	SetWindowText( hwndQHTM, pszText );
	
	lp = ::GetWindowLongPtr( hwndQHTM, GWLP_WNDPROC );
	::SetProp( hwndQHTM, QHTM_SUBCLASSED_QHTM_WNDPROC_PROP, reinterpret_cast< HANDLE >( lp ) );
	::SetWindowLongPtr( hwndQHTM, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( QHTMSubClassFunc ) );

	delete pszText;
	pszText = NULL;
	return TRUE;

#else		//	QHTM_ALLOW_HTML_BUTTON
	UNREFERENCED_PARAMETER( hwndButton );
	return FALSE;
#endif	//	QHTM_ALLOW_HTML_BUTTON
}

