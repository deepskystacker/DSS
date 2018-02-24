/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ParseStyles.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ParseStyles.h"

struct StructStyle
{
	StructStyle( LPCTSTR pcszName, Style	style )
		: m_strName( pcszName ), m_style( style ) {}
	CStaticString m_strName;
	Style	m_style;
private:
	StructStyle();
};

const StructStyle g_tokens[] =
{
	 StructStyle( _T("font-weight"),				sFontWeight )
	,StructStyle( _T("font-size"),					sFontsize )
	,StructStyle( _T("font-family"),				sFontFamily )
	,StructStyle( _T("color"),							sColor )
	,StructStyle( _T("background-color"),		sBackgroundColor )
	,StructStyle( _T("background-image"),		sBackgroundImage )	
};


MapClass< CStaticString, Style > g_mapStyle( 397 );


bool ExtractStyle( LPCTSTR pcszStyle, size_t uLength, Style &style, CStaticString &strValue )
{
	LPCTSTR pcszStyleEnd = _tcschr( pcszStyle, _T(':') );
	if( pcszStyleEnd )
	{
		CStaticString strStyleName( pcszStyle, pcszStyleEnd - pcszStyle );
		strStyleName.TrimBoth();
		//TRACE( _T("Style Name: %s\n"), (LPCTSTR)CSimpleString( strStyleName, strStyleName.GetLength() ) );
		Style *pStyle = g_mapStyle.Lookup( strStyleName );
		if( pStyle )
		{
			style = *pStyle;
			pcszStyleEnd++;
			strValue.Set( pcszStyleEnd, uLength - ( pcszStyleEnd - pcszStyle ) );
			strValue.TrimBoth();
			//TRACE( _T("Style Value: %s\n"), (LPCTSTR)CSimpleString( strValue, strValue.GetLength() ) );
			return true;
		}
	}
	return false;
}


void ParseStyles( const CStaticString &strStyle, CStyles &styles )
{
	if( g_mapStyle.GetSize() == 0 )
	{
		for( UINT n = 0; n < countof( g_tokens ); n++ )
		{
			g_mapStyle.SetAt( g_tokens[n].m_strName, g_tokens[n].m_style );
		}
	}

	Style style;
	CStaticString strValue;

	LPCTSTR pcszStart = strStyle;
	LPCTSTR pcszCurrent = strStyle;
	size_t uLength = strStyle.GetLength();
	while( pcszCurrent && uLength )
	{
		if( *pcszCurrent == _T(';') )
		{
			if( ExtractStyle( pcszStart, pcszCurrent - pcszStart, style, strValue ) )
			{
				CStylePair &sp = styles.Add();
				sp.m_strValue = strValue;
				sp.m_style = style;
			}
			pcszStart = pcszCurrent + 1;
		}
		pcszCurrent++;
		uLength--;
	}

	if( ExtractStyle( pcszStart, pcszCurrent - pcszStart, style, strValue ) )
	{
		CStylePair &sp = styles.Add();
		sp.m_strValue = strValue;
		sp.m_style = style;
	}


	
}