/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeCharset.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DebugHlp/DebugHlp.h"
#include "QHTM_Types.h"
#include "smallstringhash.h"
#include "StaticString.h"

extern BYTE DecodeCharset( const CStaticString &strCharSet );

struct StructCharSet
{
	LPCTSTR m_pcszName;
	BYTE m_cCharSet;
};

static const StructCharSet g_arrCharSet[] =
{
	{ _T("windows-1250"), EASTEUROPE_CHARSET },
	{ _T("windows-1251"), RUSSIAN_CHARSET },
	{ _T("windows-1252"), ANSI_CHARSET },
	{ _T("windows-1253"), GREEK_CHARSET },
	{ _T("windows-1254"), TURKISH_CHARSET },
	{ _T("windows-1255"), HEBREW_CHARSET },
	{ _T("windows-1256"), ARABIC_CHARSET },
	{ _T("windows-1257"), BALTIC_CHARSET },
	{ _T("gb2312"), GB2312_CHARSET },
	{ _T("windows-OEM"),	OEM_CHARSET },
	{ _T("UTF-8"),				ANSI_CHARSET },	
	{ _T("big5"),					CHINESEBIG5_CHARSET },	
};


BYTE DecodeCharset( const CStaticString &strCharSet )
{
	for( UINT u = 0; u < countof( g_arrCharSet ); u++ )
	{
		if( !_tcsnicmp( g_arrCharSet[ u ].m_pcszName, strCharSet.GetData(), strCharSet.GetLength() ) )
		{
			return g_arrCharSet[ u ].m_cCharSet;
		}
	}

	QHTM_TRACE( _T("Unknown characters set\n") );
	return DEFAULT_CHARSET;
}