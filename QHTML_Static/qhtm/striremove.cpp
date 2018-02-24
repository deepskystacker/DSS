/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	strremove.cpp
Owner:	russf@gipsysoft.com
Purpose:	remove a substring from a larger string.
					Search is case insensitive

					See strremove for a case sensitive version
----------------------------------------------------------------------*/
#include "stdafx.h"

//	Resuse function.
extern LPTSTR stristr( LPTSTR pszSource, LPCTSTR pcszSearch );

extern bool striremove( LPTSTR pszString, LPCTSTR pcszSub );

bool striremove( LPTSTR pszString, LPCTSTR pcszSub )
//
//	Return true if any of pcszSub that have been removed from
//	our main string.
{
	bool bRetVal = false;
	LPTSTR pszSubFound;
	const size_t nSearchLength = _tcslen( pcszSub );
	while( ( pszSubFound = stristr( pszString, pcszSub ) ) != NULL )
	{
		_tcscpy( pszSubFound, pszSubFound + nSearchLength );
		pszString = pszSubFound;
		bRetVal = true;
	}
	return bRetVal;
}
