/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	cursor.cpp
Owner:	russf@gipsysoft.com
Purpose:	Cursor abstraction.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "Cursor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCursor::CCursor()
	: m_cursor( knArrow )
{

}

CCursor::~CCursor()
{

}


void CCursor::Load( CursorShape c )
{
	m_cursor = c;
}


void CCursor::Set()
{
	//	REVIEW - russf - OS specific code
	HCURSOR cur = NULL;
	switch( m_cursor )
	{
	case knArrow:
		cur = ::LoadCursor( NULL, IDC_ARROW );
		break;

	case knHand:
		cur = ::LoadCursor( NULL, MAKEINTRESOURCE(/*IDC_HAND*/32649) );
		if (!cur)
		{
			cur = ::LoadCursor( g_hResourceInstance, MAKEINTRESOURCE( g_uHandCursorID ) );
		}
		break;

	case knMinimumCursor:
	case knMaximumCursor:
	default:
		QHTM_TRACE(_T("Invalid cursor shape used\n"));
		ASSERT( FALSE );
		break;
	}

	//	If this fires then you have not yet added the resources from QHTM (cursor, bitmap)		
	ASSERT( cur );
	SetCursor( cur );
}

