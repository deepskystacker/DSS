/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	GetColourFromString.cpp
Owner:	russf@gipsysoft.com
Purpose:	Parse a string for it's colour value, used by the HTML code
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <reuse/WinHelper.h>
#include "QHTM_Types.h"
#include "StaticString.h"
#include <stdlib.h>

#define COLOR_HOTLIGHT          26
#define COLOR_GRADIENTACTIVECAPTION 27
#define COLOR_GRADIENTINACTIVECAPTION 28
#define COLOR_MENUHILIGHT       29
#define COLOR_MENUBAR           30

struct StructColour
{
	StructColour( LPCTSTR pcszName, COLORREF cr )
		: m_strName( pcszName ), m_cr( cr ) {}
	CStaticString m_strName;
	COLORREF m_cr;
};

static const StructColour g_colours[] =
{
	StructColour( _T("aliceblue"),		RGB( 240, 248, 255 ) )
	, StructColour( _T("antiquewhite"),		RGB( 250, 235, 215 ) )
	, StructColour( _T("aqua"),				RGB(  0, 255, 255 ) )
	, StructColour( _T("aquamarine"),		RGB( 127, 255, 212 ) )
	, StructColour( _T("azure"),			RGB( 240, 255, 255 ) )
	, StructColour( _T("beige"),			RGB( 245, 245, 220 ) )
	, StructColour( _T("bisque"),			RGB( 255, 228, 196 ) )
	, StructColour( _T("black"),			RGB(  0, 0, 0 ) )
	, StructColour( _T("blanchedalmond"),	RGB( 255, 235, 205 ) )
	, StructColour( _T("blue"),				RGB(  0, 0, 255 ) )
	, StructColour( _T("blueviolet"),		RGB( 138, 43, 226 ) )
	, StructColour( _T("brown"),			RGB( 165, 42, 42 ) )
	, StructColour( _T("burlywood"),		RGB( 222, 184, 135 ) )
	, StructColour( _T("cadetblue"),		RGB(  95, 158, 160 ) )
	, StructColour( _T("chartreuse"),		RGB( 127, 255, 0 ) )
	, StructColour( _T("chocolate"),		RGB( 210, 105, 30 ) )
	, StructColour( _T("coral"),			RGB( 255, 127, 80 ) )
	, StructColour( _T("cornflowerblue"),	RGB( 100, 149, 237 ) )
	, StructColour( _T("cornsilk"),			RGB( 255, 248, 220 ) )
	, StructColour( _T("crimson"),			RGB( 220, 20, 60 ) )
	, StructColour( _T("cyan"),				RGB(  0, 255, 255 ) )
	, StructColour( _T("darkblue"),			RGB(  0, 0, 139 ) )
	, StructColour( _T("darkcyan"),			RGB(  0, 139, 139 ) )
	, StructColour( _T("darkgoldenrod"),	RGB( 184, 134, 11 ) )
	, StructColour( _T("darkgray"),			RGB( 169, 169, 169 ) )
	, StructColour( _T("darkgreen"),		RGB(  0, 100, 0 ) )
	, StructColour( _T("darkgrey"),			RGB( 169, 169, 169 ) )
	, StructColour( _T("darkkhaki"),		RGB( 189, 183, 107 ) )
	, StructColour( _T("darkmagenta"),		RGB( 139, 0, 139 ) )
	, StructColour( _T("darkolivegreen"),	RGB(  85, 107, 47 ) )
	, StructColour( _T("darkorange"),		RGB( 255, 140, 0 ) )
	, StructColour( _T("darkorchid"),		RGB( 153, 50, 204 ) )
	, StructColour( _T("darkred"),			RGB( 139, 0, 0 ) )
	, StructColour( _T("darksalmon"),		RGB( 233, 150, 122 ) )
	, StructColour( _T("darkseagreen"),		RGB( 143, 188, 143 ) )
	, StructColour( _T("darkslateblue"),	RGB(  72, 61, 139 ) )
	, StructColour( _T("darkslategray"),	RGB(  47, 79, 79 ) )
	, StructColour( _T("darkslategrey"),	RGB(  47, 79, 79 ) )
	, StructColour( _T("darkturquoise"),	RGB(  0, 206, 209 ) )
	, StructColour( _T("darkviolet"),		RGB( 148, 0, 211 ) )
	, StructColour( _T("deeppink"),			RGB( 255, 20, 147 ) )
	, StructColour( _T("deepskyblue"),		RGB(  0, 191, 255 ) )
	, StructColour( _T("dimgray"),			RGB( 105, 105, 105 ) )
	, StructColour( _T("dimgrey"),			RGB( 105, 105, 105 ) )
	, StructColour( _T("dodgerblue"),		RGB(  30, 144, 255 ) )
	, StructColour( _T("firebrick"),		RGB( 178, 34, 34 ) )
	, StructColour( _T("floralwhite"),		RGB( 255, 250, 240 ) )
	, StructColour( _T("forestgreen"),		RGB(  34, 139, 34 ) )
	, StructColour( _T("fuchsia"),			RGB( 255, 0, 255 ) )
	, StructColour( _T("gainsboro"),		RGB( 220, 220, 220 ) )
	, StructColour( _T("ghostwhite"),		RGB( 248, 248, 255 ) )
	, StructColour( _T("gold"),				RGB( 255, 215, 0 ) )
	, StructColour( _T("goldenrod"),		RGB( 218, 165, 32 ) )
	, StructColour( _T("gray"),				RGB( 128, 128, 128 ) )
	, StructColour( _T("grey"),				RGB( 128, 128, 128 ) )
	, StructColour( _T("green"),			RGB(  0, 128, 0 ) )
	, StructColour( _T("greenyellow"),		RGB( 173, 255, 47 ) )
	, StructColour( _T("honeydew"),			RGB( 240, 255, 240 ) )
	, StructColour( _T("hotpink"),			RGB( 255, 105, 180 ) )
	, StructColour( _T("indianred"),		RGB( 205, 92, 92 ) )
	, StructColour( _T("indigo"),			RGB(  75, 0, 130 ) )
	, StructColour( _T("ivory"),			RGB( 255, 255, 240 ) )
	, StructColour( _T("khaki"),			RGB( 240, 230, 140 ) )
	, StructColour( _T("lavender"),			RGB( 230, 230, 250 ) )
	, StructColour( _T("lavenderblush"),	RGB( 255, 240, 245 ) )
	, StructColour( _T("lawngreen"),		RGB( 124, 252, 0 ) )
	, StructColour( _T("lemonchiffon"),		RGB( 255, 250, 205 ) )
	, StructColour( _T("lightblue"),		RGB( 173, 216, 230 ) )
	, StructColour( _T("lightcoral"),		RGB( 240, 128, 128 ) )
	, StructColour( _T("lightcyan"),		RGB( 224, 255, 255 ) )
	, StructColour( _T("lightgoldenrodyellow"),			RGB( 250, 250, 210 ) )
	, StructColour( _T("lightgray"),		RGB( 211, 211, 211 ) )
	, StructColour( _T("lightgreen"),		RGB( 144, 238, 144 ) )
	, StructColour( _T("lightgrey"),		RGB( 211, 211, 211 ) )
	, StructColour( _T("lightpink"),		RGB( 255, 182, 193 ) )
	, StructColour( _T("lightsalmon"),		RGB( 255, 160, 122 ) )
	, StructColour( _T("lightseagreen"),	RGB(  32, 178, 170 ) )
	, StructColour( _T("lightskyblue"),		RGB( 135, 206, 250 ) )
	, StructColour( _T("lightslategray"),	RGB( 119, 136, 153 ) )
	, StructColour( _T("lightslategrey"),	RGB( 119, 136, 153 ) )
	, StructColour( _T("lightsteelblue"),	RGB( 176, 196, 222 ) )
	, StructColour( _T("lightyellow"),		RGB( 255, 255, 224 ) )
	, StructColour( _T("lime"),				RGB(  0, 255, 0 ) )
	, StructColour( _T("limegreen"),		RGB(  50, 205, 50 ) )
	, StructColour( _T("linen"),			RGB( 250, 240, 230 ) )
	, StructColour( _T("magenta"),			RGB( 255, 0, 255 ) )
	, StructColour( _T("maroon"),			RGB( 128, 0, 0 ) )
	, StructColour( _T("mediumaquamarine"),	RGB( 102, 205, 170 ) )
	, StructColour( _T("mediumblue"),		RGB(  0, 0, 205 ) )
	, StructColour( _T("mediumorchid"),		RGB( 186, 85, 211 ) )
	, StructColour( _T("mediumpurple"),		RGB( 147, 112, 219 ) )
	, StructColour( _T("mediumseagreen"),	RGB(  60, 179, 113 ) )
	, StructColour( _T("mediumslateblue"),	RGB( 123, 104, 238 ) )
	, StructColour( _T("mediumspringgreen"),RGB(  0, 250, 154 ) )
	, StructColour( _T("mediumturquoise"),	RGB(  72, 209, 204 ) )
	, StructColour( _T("mediumvioletred"),	RGB( 199, 21, 133 ) )
	, StructColour( _T("midnightblue"),		RGB(  25, 25, 112 ) )
	, StructColour( _T("mintcream"),		RGB( 245, 255, 250 ) )
	, StructColour( _T("mistyrose"),		RGB( 255, 228, 225 ) )
	, StructColour( _T("moccasin"),			RGB( 255, 228, 181 ) )
	, StructColour( _T("navajowhite"),		RGB( 255, 222, 173 ) )
	, StructColour( _T("navy"),				RGB(  0, 0, 128 ) )
	, StructColour( _T("oldlace"),			RGB( 253, 245, 230 ) )
	, StructColour( _T("olive"),			RGB( 128, 128, 0 ) )
	, StructColour( _T("olivedrab"),		RGB( 107, 142, 35 ) )
	, StructColour( _T("orange"),			RGB( 255, 165, 0 ) )
	, StructColour( _T("orangered"),		RGB( 255, 69, 0 ) )
	, StructColour( _T("orchid"),			RGB( 218, 112, 214 ) )
	, StructColour( _T("palegoldenrod"),	RGB( 238, 232, 170 ) )
	, StructColour( _T("palegreen"),		RGB( 152, 251, 152 ) )
	, StructColour( _T("paleturquoise"),	RGB( 175, 238, 238 ) )
	, StructColour( _T("palevioletred"),	RGB( 219, 112, 147 ) )
	, StructColour( _T("papayawhip"),		RGB( 255, 239, 213 ) )
	, StructColour( _T("peachpuff"),		RGB( 255, 218, 185 ) )
	, StructColour( _T("peru"),				RGB( 205, 133, 63 ) )
	, StructColour( _T("pink"),				RGB( 255, 192, 203 ) )
	, StructColour( _T("plum"),				RGB( 221, 160, 221 ) )
	, StructColour( _T("powderblue"),		RGB( 176, 224, 230 ) )
	, StructColour( _T("purple"),			RGB( 128, 0, 128 ) )
	, StructColour( _T("red"),				RGB( 255, 0, 0 ) )
	, StructColour( _T("rosybrown"),		RGB( 188, 143, 143 ) )
	, StructColour( _T("royalblue"),		RGB(  65, 105, 225 ) )
	, StructColour( _T("saddlebrown"),		RGB( 139, 69, 19 ) )
	, StructColour( _T("salmon"),			RGB( 250, 128, 114 ) )
	, StructColour( _T("sandybrown"),		RGB( 244, 164, 96 ) )
	, StructColour( _T("seagreen"),			RGB(  46, 139, 87 ) )
	, StructColour( _T("seashell"),			RGB( 255, 245, 238 ) )
	, StructColour( _T("sienna"),			RGB( 160, 82, 45 ) )
	, StructColour( _T("silver"),			RGB( 192, 192, 192 ) )
	, StructColour( _T("skyblue"),			RGB( 135, 206, 235 ) )
	, StructColour( _T("slateblue"),		RGB( 106, 90, 205 ) )
	, StructColour( _T("slategray"),		RGB( 112, 128, 144 ) )
	, StructColour( _T("slategrey"),		RGB( 112, 128, 144 ) )
	, StructColour( _T("snow"),				RGB( 255, 250, 250 ) )
	, StructColour( _T("springgreen"),		RGB(  0, 255, 127 ) )
	, StructColour( _T("steelblue"),		RGB(  70, 130, 180 ) )
	, StructColour( _T("tan"),				RGB( 210, 180, 140 ) )
	, StructColour( _T("teal"),				RGB(  0, 128, 128 ) )
	, StructColour( _T("thistle"),			RGB( 216, 191, 216 ) )
	, StructColour( _T("tomato"),			RGB( 255, 99, 71 ) )
	, StructColour( _T("turquoise"),		RGB(  64, 224, 208 ) )
	, StructColour( _T("violet"),			RGB( 238, 130, 238 ) )
	, StructColour( _T("wheat"),			RGB( 245, 222, 179 ) )
	, StructColour( _T("white"),			RGB( 255, 255, 255 ) )
	, StructColour( _T("whitesmoke"),		RGB( 245, 245, 245 ) )
	, StructColour( _T("yellow"),			RGB( 255, 255, 0 ) )
	, StructColour( _T("yellowgreen"),		RGB( 154, 205, 50 ) )
};


struct StructSystemColour
{
	LPCTSTR pcszName;
	UINT uID;
};

#define SYSCOLOR_ITEM( id ) { _T(#id), id },

static const StructSystemColour g_SysColour[] =
{
	SYSCOLOR_ITEM( COLOR_3DDKSHADOW )
	SYSCOLOR_ITEM( COLOR_3DSHADOW )
	SYSCOLOR_ITEM( COLOR_3DFACE )
	SYSCOLOR_ITEM( COLOR_3DHIGHLIGHT )
	SYSCOLOR_ITEM( COLOR_3DHILIGHT )
	SYSCOLOR_ITEM( COLOR_3DLIGHT )
	SYSCOLOR_ITEM( COLOR_BTNFACE )
	SYSCOLOR_ITEM( COLOR_BTNSHADOW )
	SYSCOLOR_ITEM( COLOR_BTNTEXT )
#if !defined (_WIN32_WCE)
	SYSCOLOR_ITEM( COLOR_BTNHILIGHT )
#endif	//	!defined (_WIN32_WCE)
	SYSCOLOR_ITEM( COLOR_BTNHIGHLIGHT )
	SYSCOLOR_ITEM( COLOR_BACKGROUND )
	SYSCOLOR_ITEM( COLOR_ACTIVECAPTION )
	SYSCOLOR_ITEM( COLOR_INACTIVECAPTION )
	SYSCOLOR_ITEM( COLOR_CAPTIONTEXT )
	SYSCOLOR_ITEM( COLOR_ACTIVEBORDER )
	SYSCOLOR_ITEM( COLOR_INACTIVEBORDER )
	SYSCOLOR_ITEM( COLOR_APPWORKSPACE )
	SYSCOLOR_ITEM( COLOR_HIGHLIGHT )
	SYSCOLOR_ITEM( COLOR_HIGHLIGHTTEXT )
	SYSCOLOR_ITEM( COLOR_GRAYTEXT )
	SYSCOLOR_ITEM( COLOR_INACTIVECAPTIONTEXT )
	SYSCOLOR_ITEM( COLOR_INFOTEXT )
	SYSCOLOR_ITEM( COLOR_INFOBK )
	SYSCOLOR_ITEM( COLOR_HOTLIGHT )
	SYSCOLOR_ITEM( COLOR_GRADIENTACTIVECAPTION )
	SYSCOLOR_ITEM( COLOR_GRADIENTINACTIVECAPTION )
	SYSCOLOR_ITEM( COLOR_MENU )
	SYSCOLOR_ITEM( COLOR_MENUBAR )
	SYSCOLOR_ITEM( COLOR_MENUTEXT )
	SYSCOLOR_ITEM( COLOR_MENUHILIGHT )
	SYSCOLOR_ITEM( COLOR_DESKTOP )
	SYSCOLOR_ITEM( COLOR_SCROLLBAR )
	SYSCOLOR_ITEM( COLOR_WINDOW )
	SYSCOLOR_ITEM( COLOR_WINDOWFRAME )
	SYSCOLOR_ITEM( COLOR_WINDOWTEXT )
};


static MapClass< CStaticString , COLORREF > g_mapNamedColour;
static MapClass< CStaticString , UINT > g_mapSysColour;




void QHTM_ColoursStartup()
{
	if( g_mapNamedColour.GetSize() == 0 )
	{
		UINT n;
		for( n = 0; n < countof( g_colours ); n++ )
		{
			g_mapNamedColour.SetAt( g_colours[n].m_strName, g_colours[n].m_cr );
		}

		for( n = 0; n < countof( g_SysColour ); n++ )
		{
			g_mapSysColour.SetAt( g_SysColour[n].pcszName, g_SysColour[n].uID );
		}
		
	}
}


void QHTM_ColoursShutdown()
{
	g_mapNamedColour.RemoveAll();
	g_mapSysColour.RemoveAll();
}


COLORREF GetColourFromString( const CStaticString &strColour, COLORREF crDefault)
//
//	Return the colour from the string passed.
//
//	Does a quick lookup to see if the string is oe of the standard colours
//	if not it simple assumes the colour is in hex.
{
	if( !strColour.GetLength() )
		return crDefault;	// black

	LPCTSTR pcszColour = strColour.GetData();

	if( *pcszColour == '#' )
	{
		pcszColour++;

		//
		//	This bit of code adapted from the mozilla source for the same purpose.
		const size_t nLength = strColour.GetLength() - 1;
		size_t nBytesPerColour = min( nLength / 3, 2 );
		int rgb[3] = { 0, 0, 0 };
		for( size_t nColour = 0; nColour < 3; nColour++ )
		{
			int val = 0;
			for( size_t nByte = 0; nByte < nBytesPerColour; nByte++ )
			{
				int c = 0;
				if( *pcszColour )
				{
					c = tolower( (TCHAR) *pcszColour );
					if( (c >= '0') && (c <= '9'))
					{
						c = c - '0';
					}
					else if( (c >= 'a') && ( c <= 'f') )
					{
						c = c - 'a' + 10;
					}
					else
					{
						c = 0;
					}
					val = (val << 4) + c;
					pcszColour++;
				}
			}
			rgb[ nColour ] = val;
		}

		return RGB( rgb[0], rgb[1], rgb[2] );
	}


	COLORREF *pcr = g_mapNamedColour.Lookup( strColour );
	if( pcr )
	{
		return *pcr;
	}

	UINT *puID = g_mapSysColour.Lookup( strColour );
	if( puID )
	{
		return GetSysColor( *puID );
	}
	


	if( *pcszColour  )
	{
		LPTSTR endptr;
		if( _tcschr( pcszColour, ',' ) )
		{
			int rgb[3] = { 0, 0, 0 };
			rgb[ 0 ] = _tcstoul( pcszColour, &endptr, 10 );
			endptr = WinHelper::SkipWhiteSpace( endptr );
			if( *endptr == ',' )
			{
				rgb[ 1 ] = _tcstoul( endptr + 1, &endptr, 10 );
			}
			endptr = WinHelper::SkipWhiteSpace( endptr );
			if( *endptr == ',' )
			{
				rgb[ 2 ] = _tcstoul( endptr + 1, &endptr, 10 );
			}
			return RGB( rgb[0], rgb[1], rgb[2] );
		}

		//
		//	It's safe to use _tcstoul because the assumption that the data originally passed to us is zero terminated.
		//	That way even if the colour is *the* last item in the file _tcstoul will properly terminate!
		return _tcstoul( pcszColour, &endptr, 16 );
	}
	return crDefault;
}