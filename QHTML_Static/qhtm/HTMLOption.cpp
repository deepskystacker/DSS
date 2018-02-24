/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLOption.cpp
Owner:	russf@gipsysoft.com
Purpose:

	A single item in a listbox or combo (or tree I guess)

----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM_Types.h"
#include "HTMLOption.h"


CHTMLOption::CHTMLOption()
	: m_bSelected( false )
{
}


CHTMLOption::~CHTMLOption()
{
}

#ifdef _DEBUG
void CHTMLOption::Dump() const
{
	TRACENL( _T("  Option\n") );
	TRACENL( _T("   Selected %s\n"), m_bSelected ? _T("true") : _T("false") );
	TRACENL( _T("   Value (%s)\n"), (LPCTSTR)m_strValue );
	TRACENL( _T("   Label (%s)\n"), (LPCTSTR)m_strLabel );
	TRACENL( _T("   Text (%s)\n"), (LPCTSTR)m_strText );	
}
#endif	//	_DEBUG
