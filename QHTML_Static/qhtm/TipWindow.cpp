/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	TipWindow.cpp
Owner:	russf@gipsysoft.com
Purpose:	Tool tips window
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <guitools/guitools.h>
#include <versionhelpers.h>
#include "Defaults.h"
#include "TipWindow.h"
#include "utils.h"
#include "QHTM.h"

#ifdef QHTM_ALLOW_TOOLTIPS

//	: 'this' : used in base member initializer list
//	Ignored because we want to pass the 'this' pointer...
#pragma warning( disable: 4355 )

extern void GetDisplayWorkArea( POINT pt, RECT &rc );
extern void CancelMouseDowns();
extern CSectionABC *g_pSectMouseDowned;

//
//	Where the class information is stored in the class data area
#define WINDOW_DATA	0

#define QHTM_TIP_WINDOW_CLASS _T("QHTM_Window_Class_001-Tip")

BOOL CTipWindow::Register( HINSTANCE hInst )
{
	//	Needs an instance handle!
	ASSERT( hInst );
	WNDCLASS wc = { 0 };
	if( !GetClassInfo( hInst, QHTM_TIP_WINDOW_CLASS, &wc ) )
	{

		memset(&wc, 0, sizeof(WNDCLASS));
		wc.style			= CS_BYTEALIGNCLIENT;

		if(IsWindowsXPOrGreater())
		{
			wc.style |= CS_BYTEALIGNWINDOW;	// Use the correct named style not a *magic number*
		}


		wc.lpfnWndProc	= (WNDPROC)CTipWindow::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= sizeof( CTipWindow * );
		wc.hInstance		= hInst;
		wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= (HBRUSH)COLOR_WINDOW + 1;
		wc.lpszMenuName	= NULL;
		wc.lpszClassName	= QHTM_TIP_WINDOW_CLASS;

		return RegisterClass( &wc ) != 0;	
	}
	return true;
}


enum {knDeadWindow = 1};

LRESULT CALLBACK CTipWindow::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LPARAM lparam = GetWindowLongPtr( hwnd, WINDOW_DATA );

	if( lparam == knDeadWindow )
	{
		//	Should never happen. It implies a message ghas been sent *after* we have been destroyed.
		ASSERT( FALSE );
		return DefWindowProc( hwnd, message, wParam, lParam );
	}

	CTipWindow *pWnd = reinterpret_cast<CTipWindow*>( lparam );

	switch( message )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint( hwnd, &ps );
			{
				WinHelper::CRect rcPaint( ps.rcPaint );
				if( rcPaint.IsEmpty() )
				{
					GetClientRect( hwnd, &rcPaint );
				}
				GS::CDrawContext dc( &rcPaint, ps.hdc );
				SelectPalette( ps.hdc, GS::GetCurrentWindowsPalette(), TRUE );
				RealizePalette( ps.hdc );
				pWnd->OnDraw( dc );
			}
			EndPaint( hwnd, &ps );
		}
		break;


	case WM_SIZE:
		{
			WinHelper::CRect rc( 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
			pWnd->OnLayout( rc );
			InvalidateRect( hwnd, NULL, NULL );
		}
		break;


	case WM_SETCURSOR:
		{
			if( LOWORD( lParam ) == HTCLIENT )
			{
				if( pWnd->UpdateCursor() )
				{
					return 0;
				}
			}
		}
		return DefWindowProc( hwnd, message, wParam, lParam );


	case WM_LBUTTONDOWN:
		{
			CSectionABC::KillTip();
			CancelMouseDowns();

			WinHelper::CPoint pt( lParam );
			g_pSectMouseDowned = pWnd->FindSectionFromPoint( pt );
			pWnd->OnMouseLeftDown( pt );
		}
		break;



	case WM_LBUTTONUP:
		if( g_pSectMouseDowned )
		{
			WinHelper::CPoint pt( lParam );
			CSectionABC *psect = pWnd->FindSectionFromPoint( pt );
			if( psect && psect == g_pSectMouseDowned )
			{
				g_pSectMouseDowned->OnMouseLeftUp( pt );
			}
			CancelMouseDowns();
		}
		break;


	case WM_NCDESTROY:
		pWnd->OnWindowDestroyed();
#if defined(_WIN64)
		SetWindowLongPtr(hwnd, WINDOW_DATA, (LONG_PTR)(knDeadWindow));
#else
		SetWindowLong(hwnd, WINDOW_DATA, (LONG)(knDeadWindow));
#endif
		break;

	case WM_NCCREATE:
		if( pWnd == NULL )
		{
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
#if defined(_WIN64)
			SetWindowLongPtr(hwnd, WINDOW_DATA, reinterpret_cast<LONG_PTR>(lpcs->lpCreateParams));
#else
			SetWindowLong(hwnd, WINDOW_DATA, reinterpret_cast<LONG>(lpcs->lpCreateParams));
#endif
		}
		return TRUE;

	default:
		return DefWindowProc( hwnd, message, wParam, lParam );
	}
	return 0;
}


enum { g_knTipClearanceX = 4, g_knTipClearanceY = 22, g_knMaxTipWidth = 400, g_knMaxHeight = 400 };

DWORD	CTipWindow::m_nLastTipCreated = 0;

static bool HeightExceedsTipMax( StringClass &strHTML, int nMaxHeight )
{
	HDC hdcScreen = GetDC( NULL );
	int nHeaderHeight = QHTM_PrintGetHTMLHeight( hdcScreen, strHTML, g_knMaxTipWidth, QHTM_ZOOM_DEFAULT );
	ReleaseDC( NULL, hdcScreen );
	return nHeaderHeight > nMaxHeight;
}


CTipWindow::CTipWindow( CSectionABC *pParent, LPCTSTR pcszTip, WinHelper::CPoint &pt, UINT uCharacterSet )
	: m_defaults( g_defaults )
	, m_htmlSection( &m_defaults )
{
	m_defaults.m_cCharSet = static_cast< BYTE > ( uCharacterSet );
	AddSection( &m_htmlSection );

	HWND hwndParent = pParent->GetHwnd();

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof( ncm );
	if( SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0 ) )
	{
		m_defaults.SetFont( ncm.lfStatusFont );
	}
	ASSERT( pcszTip );
	m_strTip = pcszTip;

	WinHelper::CRect rcDesktop;
	GetDisplayWorkArea( pt, rcDesktop );
	m_htmlSection.SetZoomLevel( QHTM_ZOOM_DEFAULT );

	//
	//	Stop tip exceeding 'g_knMaxHeight' pixels
	bool bDeleted = false;
	while( HeightExceedsTipMax( m_strTip, g_knMaxHeight ) )
	{	
		size_t iDeleteAt = m_strTip.GetLength() - m_strTip.GetLength() / 100 * 15; // Knock 15% off
		m_strTip.Delete( iDeleteAt, m_strTip.GetLength() - iDeleteAt );

		size_t iCheckHTML = m_strTip.GetLength() - 1;
		while( iCheckHTML + 5 > m_strTip.GetLength() )
		{
			if( m_strTip[iCheckHTML] == '<' )
			{
				m_strTip.Delete( iCheckHTML, m_strTip.GetLength() - iCheckHTML );
				break;
			}
			iCheckHTML--;
		}
		bDeleted = true;
	}

	if( bDeleted )
	{
		m_strTip += _T("...<br><b>more</b>");
	}

	m_defaults.m_crBackground = GetSysColor( COLOR_INFOBK );
	m_defaults.m_crDefaultForeColour = GetSysColor( COLOR_INFOTEXT );

	m_htmlSection.SetHTML( m_strTip, m_strTip.GetLength(), NULL );
	WinHelper::CRect rcMargins( 2, 2, 2, 5 );
	m_htmlSection.SetDefaultMargins( rcMargins );
	WinHelper::CRect rcTip( 0, 0, g_knMaxTipWidth, 30 );
	GS::CDrawContext dc;
	m_htmlSection.OnLayout( rcTip, dc );
	const WinHelper::CSize size( m_htmlSection.GetSize() );

	//
	//	Adjust for mouse size
	pt.x += g_knTipClearanceX;
	pt.y += g_knTipClearanceY;

	rcTip.Set( pt.x, pt.y , pt.x + size.cx + 3, pt.y + size.cy );

	//
	//	Move rect around based on desktop rect
	if( rcTip.right > rcDesktop.right )
	{
		rcTip.Offset( -(rcTip.right - rcDesktop.right - g_knTipClearanceX), 0 );
	}

	if( rcTip.bottom > rcDesktop.bottom )
	{
		rcTip.Offset( 0, -rcTip.Height() - g_knTipClearanceY );
	}

	m_hwnd = CreateWindowEx( WS_EX_TOPMOST| WS_EX_TOOLWINDOW, QHTM_TIP_WINDOW_CLASS, NULL, WS_POPUP | WS_BORDER, rcTip.left, rcTip.top, rcTip.Width(), rcTip.Height(), hwndParent, NULL, (HINSTANCE)GetWindowLongPtr( hwndParent, GWLP_HINSTANCE ) , this );
	VAPI( m_hwnd );
	m_nLastTipCreated = GetTickCount();

	SetWindowPos( m_hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW );
}


CTipWindow::~CTipWindow()
{
	m_pCurrentTip = NULL;
}


void CTipWindow::OnWindowDestroyed()
{
	delete this;
}


void CTipWindow::OnMouseMove( const WinHelper::CPoint & )
{
	KillTip();
}


void CTipWindow::OnLayout( const WinHelper::CRect &rc )
{
	CParentSection::OnLayout( rc );
	GS::CDrawContext dc;
	m_htmlSection.OnLayout( rc, dc );
}


void CTipWindow::DestroyWindow()
{
	ASSERT_VALID_HWND( m_hwnd );
	VERIFY( ::DestroyWindow( m_hwnd ) );
}


bool CTipWindow::UpdateCursor()
{
	WinHelper::CPoint pt;
	GetCursorPos( &pt );

	ScreenToClient( m_hwnd, &pt );

	if( OnSetMouseCursor( pt ) )
	{
		return true;
	}

	return false;
}

#endif	//	QHTM_ALLOW_TOOLTIPS