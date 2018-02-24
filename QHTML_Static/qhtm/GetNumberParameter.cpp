/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	GetNumberParameter.cpp
Owner:	russf@gipsysoft.com
Purpose:	Get a parameter as a number, can be positive to reflect pixels
					or negative to represent a percentage.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdlib.h>
#include "StaticString.h"


int GetNumberParameterPercent( const CStaticString &strParam, int nDefault )
{
  int nTotal = 0;
	if( strParam.GetLength() )
	{
		LPCTSTR p = strParam.GetData();
		LPCTSTR pEnd = strParam.GetEndPointer();
		while( p < pEnd && isspace( *p ))
		{
			p++;
		}
  
		while( p < pEnd && isdigit( *p) )
		{
			nTotal = 10 * nTotal + (*p - _T('0') );     /* accumulate digit */
			p++;
		}

		if( p < pEnd && *p == _T('%') )
			nTotal = -nTotal;
	}
	else
	{
		nTotal = nDefault;
	}
	
	return nTotal;
}


int GetNumberParameter( const CStaticString &strParam, int nDefault )
{
  int nTotal = 0;
	if( strParam.GetLength() )
	{
		LPCTSTR p = strParam.GetData();
		LPCTSTR pEnd = strParam.GetEndPointer();
		while( p < pEnd && isspace( *p ))
		{
			p++;
		}
  
		TCHAR cSign = *p;
		if( cSign == _T('-') || cSign == _T('+') )
			p++;

		while( p < pEnd && isdigit( *p) )
		{
			nTotal = 10 * nTotal + (*p - _T('0') );     /* accumulate digit */
			p++;
		}

		if( cSign == _T('-') )
			nTotal = -nTotal;
	}
	else
	{
		nTotal = nDefault;
	}
	
	return nTotal;
}
