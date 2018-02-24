/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Palette.cpp
Owner:	russf@gipsysoft.com
Purpose:	Standard paletter creation, using the netscape/ie web palette.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DebugHlp/DebugHlp.h"
#include "QHTM_Types.h"

static HPALETTE CreatePaletteFrom( const PALETTEENTRY pPalEntry[], const size_t nSize )
{
	ASSERT( pPalEntry );
	ASSERT( nSize > 0 );

	HPALETTE     hPalette = NULL;
	LPLOGPALETTE lpLogPal = reinterpret_cast<LPLOGPALETTE>( new char [ sizeof(LOGPALETTE)+(sizeof(PALETTEENTRY)*nSize) ] );
	if( lpLogPal )
	{
		lpLogPal->palNumEntries = (WORD)nSize;
		lpLogPal->palVersion    = (WORD)0x0300;
		for(size_t i = 0; i < nSize; i++)
		{
			lpLogPal->palPalEntry[i].peRed   = pPalEntry[i].peRed;
			lpLogPal->palPalEntry[i].peGreen = pPalEntry[i].peGreen;
			lpLogPal->palPalEntry[i].peBlue  = pPalEntry[i].peBlue;
			lpLogPal->palPalEntry[i].peFlags = pPalEntry[i].peFlags;
		}
		hPalette = CreatePalette( lpLogPal );
		delete[] (char*)lpLogPal;
	}

	return hPalette;
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class CHTMLPalette
//
//	Class to help maintain the palette
{
public:
	CHTMLPalette() : m_hPalette( NULL ) { CreateFromColours(); }
	~CHTMLPalette() { VAPI( ::DeleteObject( m_hPalette ) ); m_hPalette = NULL; }

	void CreateFromColours()
	{

		//	add a standard palette for the HTML.
		//	This code was taken from a palette GIF from which I found at lynda.com
		//	In her article she suggested that it was a mathematical palette rather than one chosen
		//	for aethetics.
		//
		//	Create an array and create a HPALETTE from that array
		ArrayClass< PALETTEENTRY > arrPals( 216 );
		int n = 0;
		for( int nGreen = 0; nGreen <= 255; nGreen+=51 )
		{
			for( int nRed = 0; nRed <= 255; nRed+=51 )
			{
				for( int nBlue = 0; nBlue <= 255; nBlue+=51 )
				{
					PALETTEENTRY &pal = arrPals[ n ];
					pal.peRed = (BYTE)nRed;
					pal.peGreen = (BYTE)nGreen;
					pal.peBlue = (BYTE)nBlue;
					pal.peFlags = 0;
					n++;
				}
			}
		}
		m_hPalette = CreatePaletteFrom( arrPals.GetData(), arrPals.GetSize() );
	}
	HPALETTE m_hPalette;
};

static CHTMLPalette g_Pal;


HPALETTE GetCurrentWindowsPalette()
{
	return g_Pal.m_hPalette;
}