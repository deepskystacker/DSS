/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SizeAttribute.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "SizeAttribute.h"
#include "HTMLSectionCreator.h"

static const int arrHTMLFontSizePixels[ 7 ][5] = 
		{//	0		1		2		3		4		
				 6  ,7	,7	,9	,12	//	1 
				,8	,9	,9	,12	,14	//	2 
				,9	,9 	,12	,14	,16	//	3 
				,9	,12	,14	,16	,18	//	4 
				,12 ,16	,18	,20	,24	//	5 
				,16	,20	,24	,27	,31	//	6 
				,24	,29	,36	,41	,47	//	7 
		};


int CSizeAttribute::ConvertToPixels( class CHTMLSectionCreator *psc ) const
{
	return ConvertToPixels( psc->GetDC() );
}


int CSizeAttribute::ConvertToPixels( GS::CDrawContext &dc ) const
{
	if( m_nSize )
	{
		switch( m_Type )
		{
		case knLine:
			return m_nSize * dc.GetCurrentFontHeight() / 100;

		case knPercent:
			return m_nSize * dc.GetCurrentFontHeight() / 100 / 100 ;

		case knInch:
			return MulDiv( m_nSize, GetDeviceCaps( dc.GetSafeHdc(), LOGPIXELSX), 1000 );

		case knPixel:
			return m_nSize / 100;

		case knEM:
			return m_nSize * dc.GetCurrentFontHeight() / 100;

		case knPoint:
			{
				int	nLogPixelsY = 0;
				if( ::GetObjectType( dc.GetSafeHdc() ) == OBJ_METADC )
				{
					HDC		hScreenDC = GetDC( NULL );
					nLogPixelsY = GetDeviceCaps( hScreenDC, LOGPIXELSY );
					ReleaseDC(NULL, hScreenDC);
				}
				else
				{
					nLogPixelsY = GetDeviceCaps( dc.GetSafeHdc(), LOGPIXELSY );
				}

				//
				//	Convert the point size to pixel size for the destination device and we are away!
				const int nSize = -MulDiv( m_nSize / 100, nLogPixelsY, 72 );
				return nSize;
			}

		case knHTML:
			{
				int nZoomLevel = 2;

				int nSize = max( 0, min( m_nSize / 100, sizeof( arrHTMLFontSizePixels ) / sizeof( arrHTMLFontSizePixels[ 0 ] ) ) );
				nSize--;

				//
				//	Look up a HTML psuedo-size
				nSize = arrHTMLFontSizePixels[ nSize ][nZoomLevel];

				int	nLogPixelsY = 0;
				if( ::GetObjectType( dc.GetSafeHdc() ) == OBJ_METADC )
				{
					HDC		hScreenDC = GetDC( NULL );
					nLogPixelsY = GetDeviceCaps( hScreenDC, LOGPIXELSY );
					ReleaseDC(NULL, hScreenDC);
				}
				else
				{
					nLogPixelsY = GetDeviceCaps( dc.GetSafeHdc(), LOGPIXELSY );
				}

				//
				//	Convert the point size to pixel size for the destination device and we are away!
				const int nFontSize = -MulDiv( nSize, nLogPixelsY, 72 );
				return nFontSize;
			}
			
		}
	}
	return 0;
}