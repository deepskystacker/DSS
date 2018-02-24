/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	MessageReflector.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "MessageReflector.h"

#ifdef QHTM_ALLOW_FORMS

static LPCTSTR g_szMessageRelfection = _T("QHTM_MR-0");

void CMessageReflector::RemoveMessageReflector( HWND hwnd )
{
	(void)::RemoveProp( hwnd, g_szMessageRelfection );
}

CMessageReflector* CMessageReflector::IsMessageReflector( HWND hwnd )
{
	return reinterpret_cast<CMessageReflector*>( ::GetProp(hwnd, g_szMessageRelfection ) );
}

void CMessageReflector::SetMessageReflector( HWND hwnd )
{
	VERIFY( ::SetProp( hwnd, g_szMessageRelfection, reinterpret_cast<HANDLE>( this ) ) );
}

#endif		//	QHTM_ALLOW_FORMS
