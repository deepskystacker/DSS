/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLGetAlignmentFromString.cpp
Owner:	russf@gipsysoft.com
Purpose:	Read a string and get the alignment value from it.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"

struct StructAlignment
{
	StructAlignment( LPCTSTR pcsz, CStyle::Align alg )
		: m_str( pcsz ), m_alg( alg ) {}
	CStaticString m_str;
	CStyle::Align m_alg;
};

static const StructAlignment g_arrAligns[] = {
	StructAlignment( _T("left"), CStyle::algLeft )
	, StructAlignment( _T("center"), CStyle::algCentre )
	, StructAlignment( _T("right"), CStyle::algRight )
	, StructAlignment( _T("top"),CStyle::algTop )
	, StructAlignment( _T("middle"), CStyle::algMiddle )
	, StructAlignment( _T("bottom"), CStyle::algBottom )
	, StructAlignment( _T("justify"), CStyle::algJustify )
};


CStyle::Align GetAlignmentFromString( const CStaticString &str, CStyle::Align algDefault )
{
	if( str.GetLength() )
	{
		for( int n = 0; n < countof( g_arrAligns ); n++ )
		{
			if( !_tcsnicmp( g_arrAligns[ n ].m_str.GetData(), str.GetData(), str.GetLength() ) )
			{
				return g_arrAligns[ n].m_alg;
			}
		}
	}
	return algDefault;
}


LPCTSTR GetStringFromAlignment( CStyle::Align alg )
{
	for( int n = 0; n < countof( g_arrAligns ); n++ )
	{
		if( g_arrAligns[ n].m_alg == alg )
		{
			return g_arrAligns[ n ].m_str.GetData();
		}
	}

	return _T("Unknown");
}