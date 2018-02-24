/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	qhtm_wndproc.cpp
Owner:	russf@gipsysoft.com
	This is the main window procedure for QHTM windows.

----------------------------------------------------------------------*/
#include "stdafx.h"
#include <guitools/guitools.h>
#include "QHTM.h"
#include "QHTMControlSection.h"
#include "utils.h"
#include "MessageReflector.h"
#include "UnicodeHelpers.h"

//
//	Where the class information is stored in the class data area, gets set to knDeadWindow when the window has gone.
#define WINDOW_DATA	0
enum {knDeadWindow = 1};

#if !defined (_WIN32_WCE)
static UINT nMsgMouseWheel =
   (((::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 4)) ||
	 (!(::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 3)))
	 ? ::RegisterWindowMessage(MSH_MOUSEWHEEL) : 0;

extern LRESULT FASTCALL HandleMouseWheelRegisteredMessage( HWND hwnd, WPARAM wParam, LPARAM lParam );

#endif	//	if !defined (_WIN32_WCE)

//
//	Some globals
extern CSectionABC *g_pSectHighlight;
extern void CancelHighlight();

CSectionABC *g_pSectMouseDowned = NULL;

HINSTANCE g_hQHTMInstance = NULL;


bool IsMouseDown()
{
	return g_pSectMouseDowned != NULL;
}


void CancelMouseDowns()
{
	if( g_pSectMouseDowned )
	{
		g_pSectMouseDowned->OnStopMouseDown();
		g_pSectMouseDowned = NULL;
	}

}


#if !defined (_WIN32_WCE)
static bool IsbeforeInZorder( HWND hwndThis, HWND hwnd, UINT uDirection )
{
	HWND hwndNext = GetNextWindow( hwndThis, uDirection );
	while( hwndNext )
	{
		if( hwndNext == hwnd )
		{
			return true;
		}
		hwndNext = GetNextWindow( hwndNext, uDirection );
	}
	return false;
}
#endif	//	#if !defined (_WIN32_WCE)

LRESULT CALLBACK CQHTMControlSection::WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LPARAM lparam = GetWindowLong( hwnd, WINDOW_DATA );

	if( lparam == knDeadWindow )
	{
		return DefWindowProc( hwnd, message, wParam, lParam );
	}

	CQHTMControlSection *pWnd = reinterpret_cast<CQHTMControlSection*>( lparam );

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

				if( pWnd->IsTransparent() )
				{
					pWnd->OnDraw( dc );
				}
				else
				{
					GS::CBufferedDC dcBuffer( dc );
					pWnd->OnDraw( dcBuffer );
				}
			}
			EndPaint( hwnd, &ps );
		}
		break;


#if !defined (_WIN32_WCE)
	case WM_PRINTCLIENT:
		{
			if( !(lParam & PRF_CHECKVISIBLE) ||  IsWindowVisible( hwnd ) )
			{
				HDC hdc = (HDC)wParam;
				if( lParam & PRF_ERASEBKGND )
				{
					if( pWnd->IsTransparent() )
					{
						SendMessage( ::GetParent( hwnd ), message, wParam, lParam );
					}
				}

				WinHelper::CRect rcPaint;
				GetClientRect( hwnd, &rcPaint );
				{
					GS::CDrawContext dc( &rcPaint, hdc );
					SelectPalette( hdc, GS::GetCurrentWindowsPalette(), TRUE );
					RealizePalette( hdc );
					pWnd->OnDraw( dc );
				}

				if( lParam & PRF_CHILDREN )
				{
					//
					//	Print our children.
					WinHelper::CRect rcWindow;
					GetWindowRect( hwnd, rcWindow );
					HWND hwndChild = GetWindow( hwnd, GW_CHILD );
					
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
#endif	//	#if !defined (_WIN32_WCE)

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


	case WM_ERASEBKGND:
		if( pWnd->IsTransparent() )
		{
			SendMessage( ::GetParent( hwnd ), message, wParam, lParam );//return FALSE;
		}

		//	richg - 19990224 - Changed from break to return 1; Prevents
		//	redrawing of background, and eliminates flicker.
		return TRUE;


	case WM_SIZE:
		if( wParam != SIZE_MINIMIZED )
		{
			if( !pWnd->IsLayingOut() )
			{
				WinHelper::CRect rc( 0, 0, LOWORD( lParam ), HIWORD( lParam ) );
				if( rc.Width() && rc.Height() )
				{
					pWnd->OnLayout( rc );
					InvalidateRect( hwnd, NULL, FALSE );
				}
			}
		}
		break;


#ifndef UNDER_CE
	case WM_NCCREATE:
		if( pWnd == NULL )
		{
			pWnd = new CQHTMControlSection( hwnd );
			SetWindowLong( hwnd, WINDOW_DATA, reinterpret_cast<long>( pWnd ) );
			SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | WS_VSCROLL | WS_HSCROLL );
		}
		return TRUE;
#endif	//	UNDER_CE


	case WM_CREATE:
		{
#ifdef UNDER_CE
		pWnd = new CQHTMControlSection( hwnd );
		SetWindowLong( hwnd, WINDOW_DATA, reinterpret_cast<long>( pWnd ) );
		SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE ) | WS_VSCROLL | WS_HSCROLL );
#endif	//	UNDER_CE


			LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
#ifndef UNDER_CE
			if( lpcs->dwExStyle & WS_EX_TRANSPARENT )
			{
				pWnd->Transparent( true );
			}	
#endif	//	UNDER_CE
			if ( lpcs->lpszName && *lpcs->lpszName )
				pWnd->SetText( lpcs->lpszName );

			//	TODO - 
//			if( !( lpcs->style & WS_CLIPCHILDREN ) )
//			{
//				SetWindowLong( hwnd, GWL_STYLE, lpcs->style | WS_CLIPCHILDREN );
//			}
		}
	break;



	case WM_SETFONT:
		pWnd->SetFont( (HFONT)wParam );
		break;

	case WM_SETTEXT:
		{
			CancelHighlight();
			CancelMouseDowns();
			CSectionABC::KillTip();
			pWnd->SetText( (LPCTSTR)lParam );
		}
		return TRUE;

	case WM_GETTEXT:
		{
			size_t uToCopy = min( pWnd->GetText().GetLength(), wParam );
			_tcsncpy( (LPTSTR)lParam, (LPCTSTR)pWnd->GetText(), uToCopy );
			return uToCopy;
		}
		

	case WM_GETTEXTLENGTH:
		if( GetWindowLong( hwnd, WINDOW_DATA ) == 0
			|| GetWindowLong( hwnd, WINDOW_DATA ) == knDeadWindow )
		{
			return 0;
		}
		return pWnd->GetText().GetLength();

	case WM_MOUSEMOVE:
		{
			WinHelper::CPoint pt( lParam );
			pWnd->OnMouseMove( pt );
		}
		break;

	case WM_SYSKEYDOWN:
		CancelMouseDowns();
		CSectionABC::KillTip();
		return DefWindowProc( hwnd, message, wParam, lParam );

	case WM_CANCELMODE:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_CHAR:
		CancelMouseDowns();
		CSectionABC::KillTip();
		break;

	case WM_TIMER:
		if( (int)wParam == pWnd->m_nMouseMoveTimerID )
		{
			WinHelper::CPoint pt;
			GetCursorPos( pt );
			HWND hwndCursor = WindowFromPoint( pt );
			if( hwndCursor != pWnd->GetHwnd() )
			{
				CancelHighlight();
				pWnd->UnregisterTimerEvent( pWnd->m_nMouseMoveTimerID );
				pWnd->m_nMouseMoveTimerID = knNullTimerId;
			}
		}
		else
		{
			pWnd->OnTimer( static_cast< int >( wParam ) );
		}
		break;

	case WM_MOUSEWHEEL:
		{
			CancelHighlight();
			CancelMouseDowns();
			CSectionABC::KillTip();

			WinHelper::CPoint pt( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
			int nDelta = GET_WHEEL_DELTA_WPARAM( wParam );
			ScreenToClient( hwnd, &pt );
			CSectionABC *psect = pWnd->FindSectionFromPoint( pt );
			if( psect )
			{
				pWnd->OnMouseWheel( nDelta );
			}
		}
		break;


#ifdef QHTM_ALLOW_FORMS
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
		{
			CMessageReflector *pMsg = CMessageReflector::IsMessageReflector( (HWND)lParam );
			if( pMsg )
			{
				return pMsg->OnWindowMessage( message, wParam, lParam );
			}			
			else
			{
				return DefWindowProc( hwnd, message, wParam, lParam );
			}
		}
		break;


	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT di = reinterpret_cast< LPDRAWITEMSTRUCT >( lParam );
			CMessageReflector *pMsg = CMessageReflector::IsMessageReflector( di->hwndItem );
			if( pMsg )
			{
				return pMsg->OnWindowMessage( message, wParam, lParam );
			}			
			else
			{
				return DefWindowProc( hwnd, message, wParam, lParam );
			}
		}
		break;

	case WM_COMMAND:
		{
			HWND hwndItem = (HWND)lParam;
			CMessageReflector *pMsg = CMessageReflector::IsMessageReflector( hwndItem );
			if( pMsg )
			{
				return pMsg->OnWindowMessage( message, wParam, lParam );
			}			
		}
		break;

#endif//	QHTM_ALLOW_FORMS

	case WM_KILLFOCUS:
		CancelMouseDowns();
		CSectionABC::KillTip();
		if( wParam )
		{
			HWND hwndChild = (HWND)wParam;
			if( ::GetParent( hwndChild ) != hwnd )
			{
				pWnd->SetFocus( false );
			}
		}
		break;


	case WM_SETFOCUS:
#ifndef UNDER_CE
		if( wParam )
		{
			if( IsbeforeInZorder( hwnd, (HWND)wParam, GW_HWNDPREV ) )
			{
				pWnd->SetFocusControl( true );
			}
			else if( IsbeforeInZorder( hwnd, (HWND)wParam, GW_HWNDNEXT ) )
			{
				pWnd->SetFocusControl( false );
			}
		}
#endif	//	UNDER_CE
		pWnd->SetFocus( true );
		break;


#ifndef UNDER_CE
	case WM_MOUSEACTIVATE:
		{
			WinHelper::CPoint pt;
			GetCursorPos( pt );
			ScreenToClient( hwnd, pt );
			WinHelper::CRect rc;
			GetClientRect( hwnd, rc );
			if( rc.PtInRect( pt ) )
			{
				//pWnd->SelectNearestLink( pt );
				::SetFocus( hwnd );
			}
		}
			
		return MA_ACTIVATE;
#endif	//	UNDER_CE

	case WM_GETDLGCODE:
		if( pWnd->HasControls() )
		{
			LPMSG lpMsg = (LPMSG)lParam;
			if( lpMsg && lpMsg->message == WM_KEYDOWN )
			{
				if( lpMsg->wParam == VK_TAB )
				{
					BOOL bPrevious = FALSE;
					if( WinHelper::IsShiftPressed() )
					{
						if( pWnd->CanTabBackward() )
						{
							return DLGC_WANTTAB | DLGC_WANTARROWS;
						}
						bPrevious = TRUE;
					}
					else
					{
						if( pWnd->CanTabForward() )
						{
							return DLGC_WANTTAB | DLGC_WANTARROWS;
						}
					}

					HWND hwndNext = GetNextDlgTabItem( ::GetParent( hwnd ), hwnd, bPrevious );
					::SetFocus( hwndNext );
					return DLGC_WANTTAB | DLGC_WANTARROWS;
				}
			}
			else
			{
				return DLGC_WANTTAB | DLGC_WANTARROWS;
			}
		}
		return DLGC_WANTARROWS;


	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_TAB:
			if( !WinHelper::IsControlPressed() )
			{
				if( WinHelper::IsShiftPressed() )
				{
					if( pWnd->CanTabBackward() )
					{
						pWnd->TabToPreviousControl();
					}
					else
					{
						HWND hwndNext = GetNextDlgTabItem( ::GetParent( hwnd ), hwnd, TRUE );
						::SetFocus( hwndNext );
						if( hwndNext == hwnd )
						{
							pWnd->SetFocusControl( false );
						}
					}
				}
				else
				{
					if( pWnd->CanTabForward() )
					{
						pWnd->TabToNextControl();
					}
					else
					{
						HWND hwndNext = GetNextDlgTabItem( ::GetParent( hwnd ), hwnd, FALSE );
						::SetFocus( hwndNext );
						if( hwndNext == hwnd )
						{
							pWnd->SetFocusControl( true );
						}
					}
				}
				return 0;
			}
			break;

		case VK_SPACE:
			pWnd->ActivateFocusedItem();
			break;

		case VK_LEFT:
			pWnd->SelectPreviousInGroup();
			break;

		case VK_RIGHT:
			pWnd->SelectNextInGroup();
			break;

		case VK_UP:
			if( !pWnd->SelectPreviousInGroup() )
			{
				pWnd->OnVScroll( SB_LINEUP );
			}
			break;

		case VK_DOWN:
			if( !pWnd->SelectNextInGroup() )
			{
				pWnd->OnVScroll( SB_LINEDOWN );
			}
			break;

		case VK_PRIOR:
			pWnd->OnVScroll( SB_PAGEUP );
			break;
		
		case VK_NEXT:
			pWnd->OnVScroll( SB_PAGEDOWN );
			break;

		case VK_END:
			pWnd->OnVScroll( SB_BOTTOM );
			break;
		
		case VK_HOME:
			pWnd->OnVScroll( SB_TOP );
			break;
		}
		return DefWindowProc( hwnd, message, wParam, lParam );

#ifndef UNDER_CE
	case WM_NCLBUTTONDOWN:
		CSectionABC::KillTip();
		return DefWindowProc( hwnd, message, wParam, lParam );
#endif	//	UNDER_CE

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



	case WM_DESTROY:
		pWnd->OnDestroy();

		SetWindowLong( hwnd, WINDOW_DATA, knDeadWindow );
		delete pWnd;
		break;


	case WM_STYLECHANGED:
		if( !pWnd->IsLayingOut() )
		{
			if( wParam == GWL_EXSTYLE )
			{
				LPSTYLESTRUCT pstyles = reinterpret_cast<LPSTYLESTRUCT>( lParam );
#ifndef UNDER_CE
				if( pstyles->styleNew  & WS_EX_TRANSPARENT )
				{
					pWnd->Transparent( true );
				}
				else
#endif	//	UNDER_CE
				{
					pWnd->Transparent( false );
				}
			}

			SetWindowPos( hwnd, NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER );
			pWnd->OnLayout( *pWnd );
			InvalidateRect( hwnd, NULL, FALSE );
		}
		return DefWindowProc( hwnd, message, wParam, lParam );

	case WM_PALETTECHANGED:
		if ((HWND)wParam == hwnd)       // Responding to own message.
			break;									// Nothing to do.

	case WM_QUERYNEWPALETTE:
		{
			HDC hdc = GetDC( hwnd );
			HPALETTE hOldPal = SelectPalette( hdc, GS::GetCurrentWindowsPalette(), FALSE );
			int i = RealizePalette( hdc );							// Realize drawing palette.
			if (i)																	// Did the realization change?
				InvalidateRect(hwnd, NULL, TRUE);			// Yes, so force a repaint.
			SelectPalette( hdc, hOldPal, TRUE);
			RealizePalette( hdc );
			ReleaseDC( hwnd, hdc );
			return(i);
		}

	case WM_VSCROLL:
		pWnd->OnVScroll( LOWORD( wParam ) );
		break;

	case WM_HSCROLL:
		pWnd->OnHScroll( LOWORD( wParam ) );
		break;
		
	case QHTM_LOAD_FROM_RESOURCE:
		CancelHighlight();
		CancelMouseDowns();
		CSectionABC::KillTip();
		return pWnd->LoadFromResource( (HINSTANCE)wParam, (LPCTSTR)lParam );

	case QHTM_LOAD_FROM_FILE:
		CancelHighlight();
		CancelMouseDowns();
		CSectionABC::KillTip();
		return pWnd->LoadFromFile( (LPCTSTR)lParam );

	case QHTM_GET_OPTION:
		return pWnd->GetOption( static_cast< UINT >( wParam ), lParam );

	case QHTM_SET_OPTION:
		return pWnd->SetOption( static_cast< UINT >( wParam ), lParam );

	case QHTM_GOTO_LINK:
		pWnd->GotoLink( (LPCTSTR)lParam );
		break;

	case QHTM_GET_HTML_TITLE_LENGTH:
		return pWnd->GetTitleLength();

	case QHTM_GET_HTML_TITLE:
		return pWnd->GetTitle( wParam, (LPTSTR)lParam );

	case QHTM_GET_DRAWN_SIZE:
		{
			LPSIZE lpSize = reinterpret_cast<LPSIZE>( lParam );
			if( !::IsBadReadPtr( lpSize, sizeof( SIZE ) ) )
			{
				const WinHelper::CSize &size = pWnd->GetSize();
				lpSize->cx = size.cx;
				lpSize->cy = size.cy;
				return TRUE;
			}
			return FALSE;
		}

	case QHTM_GET_SCROLL_POS:
		return pWnd->GetScrollPos();

	case QHTM_SET_SCROLL_POS:
		pWnd->SetScrollPos( static_cast< int >( wParam ) );
		break;

#ifdef QHTM_ALLOW_ADD_HTML


	case QHTM_ADD_HTMLA:
		{
#ifdef _UNICODE
			LPCSTR p = (LPCSTR)lParam;
			CAnsiToUnicode str( p, strlen( p ) );
			pWnd->OnAddHTML( str, static_cast< UINT >( wParam ) );
#else	//	_UNICODE
			pWnd->OnAddHTML( (LPCTSTR)lParam, static_cast< UINT >( wParam ) );
#endif	//	//	_UNICODE
		}
		break;

	case QHTM_ADD_HTMLW:
		{
#ifdef _UNICODE
			pWnd->OnAddHTML( (LPCWSTR)lParam, static_cast< UINT >( wParam ) );
#else	//	_UNICODE
			LPCWSTR p = (LPCWSTR)lParam;
			CUnicodeToAnsi str( p, wcslen( p ) );
			pWnd->OnAddHTML( str, static_cast< UINT >( wParam ) );
#endif	//	//	_UNICODE
		}
		break;

#endif	//	QHTM_ALLOW_ADD_HTML

	
	case QHTM_GET_LINK_FROM_POINT:
		return pWnd->GetLinkFromPoint( (LPPOINT)wParam, (LPQHTM_LINK_INFO)lParam );

	case QHTM_SUBMIT_FORM:
		return pWnd->SubmitForm( (LPCTSTR)lParam, _T("submit") );

	case QHTM_RESET_FORM:
		return pWnd->ResetForm( (LPCTSTR)lParam );

	case QHTM_SET_DEFAULT_RESOURCE_HANDLE:	
		pWnd->SetDefaultResourceHandle( (HINSTANCE)lParam );
		break;

	case QHTM_GET_LINK_REGION:
		return (LRESULT)pWnd->GetLinkRegion( (LPCTSTR)lParam );

	case QHTM_SET_LPARAM:
		pWnd->SetLParam( lParam );
		break;

	case QHTM_SET_DEFAULTS:
		return g_defaults.Set( (LPQHTM_DEFAULTS)lParam );

	default:
#if !defined (_WIN32_WCE)
		if( message == nMsgMouseWheel && nMsgMouseWheel )
		{
			return HandleMouseWheelRegisteredMessage( hwnd, wParam, lParam );
		}
#endif	//	#if !defined (_WIN32_WCE)
		return DefWindowProc( hwnd, message, wParam, lParam );
	}
	return 0;
}
