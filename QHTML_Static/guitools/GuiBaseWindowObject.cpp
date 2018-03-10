/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiBaseWindowObject.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <Windowsx.h>
#include <WinUser.h>
#include "GuiBaseWindowObject.h"
#if !defined (_WIN32_WCE)
	#include <ZMouse.h>
#endif

#pragma warning( disable: 4355 )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace GS;

CGuiBaseWindowObject::CGuiBaseWindowObject( CGuiBaseObject * pgboParent, HWND hwnd )
	: CGuiBaseObject( pgboParent )
	, m_hwnd( hwnd )
	, m_uNextTimerID( 1 )
	, m_timerEnterLeave( this )
	, m_pgboHighlighted( NULL )
	, m_pgboMouseDowned( NULL )
{
}


CGuiBaseWindowObject::~CGuiBaseWindowObject()
{
}


LRESULT CGuiBaseWindowObject::OnWindowsMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ERASEBKGND:
		return TRUE;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint( GetSafeHwnd(), &ps );
			{
				WinHelper::CRect rc( ps.rcPaint );
				CDrawContext dc( &rc, ps.hdc );
				//TRACE( _T("WM_PAINT RECT %d, %d, %d, %d\n"), rc.left, rc.top, rc.right, rc.bottom );
				SelectPalette( ps.hdc, GetCurrentWindowsPalette(), TRUE );
				RealizePalette( ps.hdc );
				{
#ifndef UNDER_CE
					CBufferedDC dcBuffer( dc );
#else	//	UNDER_CE
					CDrawContext &dcBuffer = dc;
#endif
					OnDraw( dcBuffer );
					//	Simple test to draw a rect around what needs to be drawn.
					//dcBuffer.Rectangle( rc, RGB( 255, 0, 0 ) );
				}
			}
			EndPaint( GetSafeHwnd(), &ps );
		}
		break;


	case WM_SETCURSOR:
		{		
			WinHelper::CPoint pt;
			GetCursorPos( &pt );

			ScreenToClient( GetSafeHwnd(), &pt );
			CGuiBaseObject *pgbo = FindObjectFromPoint( pt, true );
			if( pgbo )
			{
				pgbo->SetCursor();
			}
		}
		break;


	case WM_TIMER:
		{
			CGuiBaseObject **pgbo = m_mapTimerEvents.Lookup( (UINT)wParam );
			if( pgbo )
			{
				ASSERT_VALID_WRITEOBJPTR( pgbo );
				(*pgbo)->OnTimer( (UINT)wParam );
			}
			else
			{
				::KillTimer( GetSafeHwnd(), (UINT)wParam );
			}
		}		
		break;


	case WM_MOUSEMOVE:
		{
			WinHelper::CPoint pt( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
			if( !m_timerEnterLeave.IsSet() )
			{
				m_timerEnterLeave.Start( 50 );
			}

			CGuiBaseObject *pgbo = FindObjectFromPoint( pt, true );
			if( pgbo != m_pgboHighlighted )
			{
				if( m_pgboHighlighted )
				{
					m_pgboHighlighted->OnMouseLeave();
				}
				m_pgboHighlighted = pgbo;

				if( m_pgboHighlighted )
				{
					m_pgboHighlighted->OnMouseEnter();
				}
			}
		}
		break;

	case WM_CLOSE:
		OnCloseWindow();
		break;


	case WM_SIZE:
		{
			WinHelper::CRect rc;
			GetClientRect( GetSafeHwnd(), rc );
			CDrawContext dc;
			Set( rc );
			OnLayout( dc );
			InvalidateRect( GetSafeHwnd(), NULL, TRUE );
		}
		break;

	case WM_MOUSEWHEEL:
		{
			CancelHighlight();
			CancelMouseDown();

			WinHelper::CPoint pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
			int nDelta = GET_WHEEL_DELTA_WPARAM( wParam );
			ScreenToClient( GetSafeHwnd(), &pt );
			CGuiBaseObject *pgbo = FindObjectFromPoint( pt, true );
			if( pgbo )
			{
				pgbo->OnMouseWheel( nDelta );
			}
			else
			{
				
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			CancelMouseDown();
			WinHelper::CPoint pt( lParam );

			m_pgboMouseDowned = FindObjectFromPoint( pt, true );
			if( m_pgboMouseDowned )
			{
				m_pgboMouseDowned->OnMouseLeftDown( pt );
			}
		}
		break;


	case WM_LBUTTONUP:
		if( m_pgboMouseDowned )
		{
			WinHelper::CPoint pt( lParam );
			CGuiBaseObject *pgbo = FindObjectFromPoint( pt, true );
			if( pgbo && pgbo == m_pgboMouseDowned )
			{
				pgbo->OnMouseLeftUp( pt );
			}

			m_pgboMouseDowned = NULL;
		}
		break;


	case WM_LBUTTONDBLCLK:
		{
			WinHelper::CPoint pt( lParam );
			CGuiBaseObject *pgbo = FindObjectFromPoint( pt, true );
			if( pgbo )
			{
				pgbo->OnMouseLeftDoubleclick( pt );
			}
		}
		break;

	case WM_CREATE:
		if( OnCreateWindow() )
		{
			return 0;
		}
		return 1;

	#if !defined (_WIN32_WCE)
	case WM_PRINTCLIENT:
		{
			if( !(lParam & PRF_CHECKVISIBLE) ||  IsWindowVisible( m_hwnd ) )
			{
				HDC hdc = (HDC)wParam;
				if( lParam & PRF_ERASEBKGND )
				{
//					if( pWnd->IsTransparent() )
//					{
//						SendMessage( ::GetParent( hwnd ), message, wParam, lParam );
//					}
				}

				WinHelper::CRect rcPaint;
				GetClientRect( m_hwnd, &rcPaint );
				{
					CDrawContext dc( &rcPaint, hdc );
					SelectPalette( hdc, GetCurrentWindowsPalette(), TRUE );
					RealizePalette( hdc );
					OnDraw( dc );
				}

				if( lParam & PRF_CHILDREN )
				{
					//
					//	Print our children.
					WinHelper::CRect rcWindow;
					GetWindowRect( m_hwnd, rcWindow );
					HWND hwndChild = GetWindow( m_hwnd, GW_CHILD );
					
					//
					//	It seems like we need to offset the origin of each child so it draws
					//	in just the right place
					POINT ptOldOrg;
					while( hwndChild )
					{
						WinHelper::CRect rc;
						GetWindowRect( hwndChild, rc );
						VERIFY( SetWindowOrgEx( hdc, rc.left - rcWindow.left, rc.top - rcWindow.top, &ptOldOrg ) );
						//TRACE("%d %d %d %d\n", rc.left, rc.top, rc.right, rc.bottom );

						SendMessage( hwndChild, WM_PRINTCLIENT, wParam, PRF_NONCLIENT | PRF_ERASEBKGND | PRF_CLIENT | PRF_CHILDREN );

						VERIFY( SetWindowOrgEx( hdc, ptOldOrg.x, ptOldOrg.y, NULL ) );
						hwndChild = GetWindow( hwndChild, GW_HWNDNEXT );
					}
				}
			}
		}
		break;
#endif	// !defined (_WIN32_WCE)

	default:

		return DefWindowProc( GetSafeHwnd(), message, wParam, lParam );
	}
	return 0;
}


void CGuiBaseWindowObject::CancelMouseDown()
{
	if( m_pgboMouseDowned )
	{
		m_pgboMouseDowned->OnMouseCancelLeftMouse();
	}
}


void CGuiBaseWindowObject::OnTimer( UINT uTimerID )
{
	if( m_timerEnterLeave == uTimerID )
	{
		WinHelper::CPoint pt;
		GetCursorPos( pt );
		HWND hwndCursor = WindowFromPoint( pt );
		if( hwndCursor != GetSafeHwnd() )
		{
			m_timerEnterLeave.Stop();

			if( m_pgboHighlighted )
			{
				m_pgboHighlighted->OnMouseLeave();
				m_pgboHighlighted = NULL;
			}
		}	
	}
}


UINT CGuiBaseWindowObject::StartTimer( CGuiBaseObject *pgbo, UINT nInterval )
{
	ASSERT_VALID_HWND( GetSafeHwnd() );
	ASSERT_VALID_WRITEOBJPTR( pgbo );
	ASSERT( nInterval > 0 );

	VAPI( ::SetTimer( GetSafeHwnd(), m_uNextTimerID, nInterval, 0 ) );
	m_mapTimerEvents.SetAt( m_uNextTimerID, pgbo );

	//TRACE( _T("Started timer %d\n"), m_uNextTimerID );
	return m_uNextTimerID++;
}


void CGuiBaseWindowObject::StopTimer( UINT uTimerID )
{
	//TRACE( _T("Removed timer id %d\n"), nTimerEventID );
	ASSERT( uTimerID  );
	if( m_mapTimerEvents.Lookup( uTimerID ) )
	{
		(void)::KillTimer( GetSafeHwnd(), uTimerID );
		m_mapTimerEvents.RemoveAt( uTimerID );
		//TRACE( _T("Stopped timer %d\n"), uTimerID );
	}
	else
	{
		//	Trying to unregister an event twice
		//ASSERT( FALSE );
	}
}

void CGuiBaseWindowObject::ForceRedraw( const WinHelper::CRect &rc )
{
	InvalidateRect( GetSafeHwnd(), rc, TRUE );
}


bool CGuiBaseWindowObject::OnCreateWindow()
{
	return true;
}


void CGuiBaseWindowObject::CancelHighlight()
{
	if( m_pgboHighlighted )
	{
		m_pgboHighlighted->OnMouseLeave();
	}
	m_pgboHighlighted = NULL;
}


void CGuiBaseWindowObject::OnCloseWindow()
{
	
}