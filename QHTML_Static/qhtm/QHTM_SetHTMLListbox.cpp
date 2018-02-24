/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_SetHTMLListbox.cpp
Owner:	russf@gipsysoft.com

Purpose:

	Enable Arbitrary HTML for listbox items.

----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "SimpleString.h"
#include <guitools/guitools.h>

#define QHTM_SUBCLASSED_PARENT_WNDPROC_PROP	_T("QHTM_OldParentWindowProc")
#define QHTM_LISTBOX_WANTS_HTML _T("QHTM_LIST_WANTS_HTML")
#define QHTM_LISTBOX_CX _T("QHTM_LIST_CX")
#define QHTM_SUBCLASSED_WNDPROC_PROP _T("QHTMListBoxSC")

#ifdef QHTM_ALLOW_HTML_LISTBOX


static LRESULT CALLBACK QHTM_PARENT_SubclassFunc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	WNDPROC old = reinterpret_cast< WNDPROC >( GetProp( hwnd, QHTM_SUBCLASSED_PARENT_WNDPROC_PROP ) );
	switch( msg )
	{
	case WM_DRAWITEM:
		{
			HWND hwndListBox = GetDlgItem( hwnd, static_cast< int >( wParam ) );
			if( ::GetProp( hwndListBox, QHTM_LISTBOX_WANTS_HTML ) )
			{
				LPDRAWITEMSTRUCT lpDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;
				CSimpleString strItem( reinterpret_cast< LPCTSTR >( lpDrawItemStruct->itemData ) );

				WinHelper::CRect rcItem( lpDrawItemStruct->rcItem );
				GS::CDrawContext dc( &rcItem, lpDrawItemStruct->hDC );
				GS::CBufferedDC dcBuffer( dc );

				WinHelper::CRect rcList;
				GetClientRect( hwndListBox, rcList );

				dcBuffer.SetClipRect( rcList );

				{
					COLORREF crBackground = GetSysColor( COLOR_WINDOW );
					bool bChangeTextColor = false;
					COLORREF crText = 0;
					if( lpDrawItemStruct->itemState & ODS_SELECTED )
					{
						crBackground = GetSysColor( COLOR_HIGHLIGHT );
						crText = GetSysColor( COLOR_HIGHLIGHTTEXT );
						bChangeTextColor = true;
					}

					if( lpDrawItemStruct->itemState & ODS_DISABLED )
					{
						crText = GetSysColor( COLOR_GRAYTEXT );
						bChangeTextColor = true;
					}

					if( bChangeTextColor )
					{
						TCHAR szBuffer[ 128 ];
						wsprintf( szBuffer, _T("<font color='%d,%d,%d'>"), GetRValue( crText ), GetGValue( crText ), GetBValue( crText ) );
						CSimpleString str( szBuffer );
						str += strItem;
						str += _T("</font>");
						strItem = str;
					}

					dcBuffer.FillRect( rcItem, crBackground );
				}


				QHTM_RenderHTMLRect( dcBuffer.GetSafeHdc(), strItem, NULL, QHTM_RENDER_FLAG_TEXT | QHTM_RENDER_FLAG_TRANSPARENT, &lpDrawItemStruct->rcItem );
				
				if( lpDrawItemStruct->itemState & ODS_FOCUS )
				{
					DrawFocusRect( dcBuffer.GetSafeHdc(), &lpDrawItemStruct->rcItem );
				}

				return TRUE;
			}
			else
			{
				return CallWindowProc( old, hwnd, msg, wParam, lParam );
			}
		}
		break;


	case WM_MEASUREITEM:
		{
			HWND hwndListBox = GetDlgItem( hwnd, static_cast< int >( wParam ) );
			if( ::GetProp( hwndListBox, QHTM_LISTBOX_WANTS_HTML ) )
			{
				LPMEASUREITEMSTRUCT lpMeasureItemStruct = (LPMEASUREITEMSTRUCT)lParam;

				WinHelper::CRect rc;
				GetClientRect( hwndListBox, rc );

				LPCTSTR pcszText = reinterpret_cast< LPCTSTR >( lpMeasureItemStruct->itemData );
				
				WinHelper::CWindowDC hdc( hwndListBox );
				UINT uHeight = 0;
				QHTM_GetHTMLHeight( hdc, pcszText, NULL, QHTM_RENDER_FLAG_TEXT, rc.Width(), &uHeight );

				lpMeasureItemStruct->itemHeight  = uHeight;
				lpMeasureItemStruct->itemWidth = rc.Width();
				return TRUE;
			}
			else
			{
				return CallWindowProc( old, hwnd, msg, wParam, lParam );
			}
		}
		break;


	/*
		This has to be one of the worst hacks ever. Listbox doesn't send WM_MEASUREITEM
		when the size of the listbox changes (why in heavens not) and so the items that
		could be shorter (due to wrapping) don't ever get shorter. This look rubbish

		So, what we do is remeasure all of teh list items and if anything is different
		we tell the listbox about it.
	*/
	case WM_CTLCOLORLISTBOX:
		if( ::GetProp( (HWND)lParam, QHTM_LISTBOX_WANTS_HTML ) )
		{
			HWND hwndListBox = (HWND)lParam;

			WinHelper::CRect rc;
			GetClientRect( hwndListBox, rc );
			const UINT uCX = (UINT)GetProp( hwnd, QHTM_LISTBOX_CX );
			if( uCX != (UINT)rc.Width()  )
			{
				SetProp( hwnd, QHTM_LISTBOX_CX, (HANDLE)rc.Width() );
				bool bSomeChanged = true;

				SendMessage( hwndListBox, WM_SETREDRAW, FALSE, 0 );

				WinHelper::CWindowDC hdc( hwndListBox );

				const UINT uItems = static_cast<UINT>( SendMessage( hwndListBox, LB_GETCOUNT, 0, 0 ) );
				for( UINT u = 0; u < uItems; u++ )
				{
					LPCTSTR pcszText = reinterpret_cast< LPCTSTR >( SendMessage( hwndListBox, LB_GETITEMDATA, u, 0 ) );
					UINT uHeight = 0;
					QHTM_GetHTMLHeight( hdc, pcszText, NULL, QHTM_RENDER_FLAG_TEXT, rc.Width(), &uHeight );
					if( (UINT)SendMessage( hwndListBox, LB_GETITEMHEIGHT, u, 0 ) != uHeight )
					{
						SendMessage( hwndListBox, LB_SETITEMHEIGHT, u, uHeight );
						bSomeChanged = true;
					}
				}
				SendMessage( hwndListBox, WM_SETREDRAW, TRUE, 0 );


				if( bSomeChanged )
				{
					InvalidateRect( hwndListBox, NULL, FALSE );
				}
			}
			return CallWindowProc( old, hwnd, msg, wParam, lParam );
		}
		else
		{
			return CallWindowProc( old, hwnd, msg, wParam, lParam );
		}
		break;
	}

	return CallWindowProc( old, hwnd, msg, wParam, lParam );
}

#endif	//	QHTM_ALLOW_HTML_LISTBOX


extern "C" BOOL WINAPI QHTM_SetHTMLListbox( HWND hwndListBox )
{
#ifdef QHTM_ALLOW_HTML_LISTBOX

	//
	//	Subclass the parent so we can catch the WM_DRAWITEM etc, messages
	HWND hwndParent = GetParent( hwndListBox );

	if( !::GetProp( hwndListBox, QHTM_SUBCLASSED_PARENT_WNDPROC_PROP ) )
	{
		//
		//	If we're a dialog box then we subclass based on that. If we're a normal window then
		//	 we subclass the main window procedure.

		WNDPROC proc = QHTM_PARENT_SubclassFunc;
		
		LONG_PTR l = ::GetWindowLongPtr( hwndParent, DWLP_DLGPROC );
		if( l )
		{
			::SetProp( hwndParent, QHTM_SUBCLASSED_PARENT_WNDPROC_PROP, reinterpret_cast< HANDLE >( l ) );
			::SetWindowLongPtr( hwndParent, DWLP_DLGPROC, reinterpret_cast< LONG_PTR >( proc ) );
		}
		else
		{
			l = ::GetWindowLongPtr( hwndParent, GWLP_WNDPROC );

			::SetProp( hwndParent, QHTM_SUBCLASSED_PARENT_WNDPROC_PROP, reinterpret_cast< HANDLE >( l ) );
			::SetWindowLongPtr( hwndParent, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( proc ) );
		}

	}

	//
	//	Set this listbox as wanting to have HTML...
	::SetProp( hwndListBox, QHTM_LISTBOX_WANTS_HTML, (HANDLE)1 );
	
	return TRUE;
#else		//	QHTM_ALLOW_HTML_LISTBOX
	UNREFERENCED_PARAMETER( hwndListBox );
	return FALSE;
#endif	//	QHTM_ALLOW_HTML_LISTBOX
}