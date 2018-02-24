/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Guitooltip.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "GuiToolTip.h"
#include <commctrl.h>

#if !defined (_WIN32_WCE)
//#pragma comment( lib, "comctl32.lib" )

using namespace GS;

CGuiTooltip::CGuiTooltip()
	: m_hwnd( NULL )
{

}

CGuiTooltip::~CGuiTooltip()
{
	m_hwnd = NULL;
}


bool CGuiTooltip::Create( HWND hwndParent )
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr( hwndParent, GWLP_HINSTANCE );
	m_hwnd = CreateWindow(TOOLTIPS_CLASS, (LPTSTR) NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, (HMENU) NULL, hInst, NULL);
	if( m_hwnd )
	{
		return true;
	}
	return false;
}


void CGuiTooltip::RemoveAll()
{
	TOOLINFO ti;
	ZeroMemory( &ti, sizeof( ti ) );
	ti.cbSize = sizeof( ti );
	int nCount = static_cast<int>( ::SendMessage( m_hwnd, TTM_GETTOOLCOUNT, 0, 0 ) );
	for( int n = nCount - 1; n >= 0; n-- )
	{
		if( ::SendMessage( m_hwnd, TTM_ENUMTOOLS, n, (LPARAM)&ti ) )
		{
			::SendMessage( m_hwnd, TTM_DELTOOL, n, (LPARAM)&ti );
		}
	}
}


bool CGuiTooltip::AddTool( HWND hwnd, LPCTSTR pcszText )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.hwnd = m_hwnd;
	ti.uId = (UINT) hwnd;
	if( GetClientRect( hwnd, &ti.rect ) )
	{
		ti.lpszText = const_cast<LPTSTR>( pcszText );
		if( ::SendMessage( m_hwnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti)) 
		{
			return true;
		}
	}
	return false;
}


void CGuiTooltip::Activate( BOOL bActive )
{
	(void)::SendMessage( m_hwnd, TTM_ACTIVATE, static_cast<WPARAM>(bActive), 0);
}


void CGuiTooltip::UpdateTipText( HWND hwnd, LPCTSTR pcszText )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd = m_hwnd;
	ti.uId = (UINT) hwnd;
	ti.lpszText = const_cast<LPTSTR>( pcszText );
	(void)::SendMessage( m_hwnd, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
}


bool CGuiTooltip::AddTool( HWND hwnd, RECT &rc, LPCTSTR pcszText, UINT uID )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.uId = uID;
	ti.rect = rc;
	ti.lpszText = const_cast<LPTSTR>( pcszText );
	if( ::SendMessage( m_hwnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti) )
	{
		return true;
	}
	return false;
}


void CGuiTooltip::UpdateToolRect( HWND hwnd, UINT uID, const RECT &rc )
{
	TOOLINFO ti = {0};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = 0;
	ti.hwnd = hwnd;
	ti.uId = uID;
	ti.rect = rc;
	ti.lpszText = NULL;
	(void)::SendMessage( m_hwnd, TTM_NEWTOOLRECT, 0, (LPARAM) (LPTOOLINFO) &ti );
}

#endif	// !defined (_WIN32_WCE)

