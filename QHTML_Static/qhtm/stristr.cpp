/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	stristr.cpp
Owner:	russf@gipsysoft.com
Web site: http://www.gipsysoft.com
Purpose:	Case insensitive test to see if a string is contained
					within another.
					It will fault if either of the strings are NULL.
----------------------------------------------------------------------*/
#include "stdafx.h"

extern LPTSTR stristr( LPTSTR pszSource, LPCTSTR pcszSearch );

LPTSTR stristr( LPTSTR pszSource, LPCTSTR pcszSearch )
//
//	Return a pointer to the start of the search string
//	If pszSource does not contain pcszSearch then returns NULL.
{
	const size_t nLength =  _tcslen( pcszSearch );
	while( *pszSource )
	{
		if( !_tcsnicmp( pszSource, pcszSearch, nLength ) )
			break;
		pszSource++;
	}

	if( !( *pszSource ) )
	{
		pszSource = NULL;
	}
	return pszSource;
}
