/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	strremove.cpp
Owner:	russf@gipsysoft.com
Purpose:	remove a substring from a larger string.
----------------------------------------------------------------------*/
#include "stdafx.h"

extern bool strremove( LPTSTR pszString, LPCTSTR pcszSub );

bool strremove( LPTSTR pszString, LPCTSTR pcszSub )
//
//	Return true if any of pcszSub that have been removed from
//	our main string.
{
	bool bRetVal = false;
	LPTSTR pszSubFound;
	const size_t nSearchLength = _tcslen( pcszSub );
	while( ( pszSubFound = _tcsstr( pszString, pcszSub ) ) != NULL )
	{
		_tcscpy( pszSubFound, pszSubFound + nSearchLength );
		pszString = pszSubFound;
		bRetVal = true;
	}
	return bRetVal;

}
