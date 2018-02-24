/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ParseWindowStyle.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <DebugHlp/DebugHlp.h>
#include "staticstring.h"
#include "QHTM_Types.h"

#ifdef QHTM_WINDOWS_CONTROL

static inline LPCTSTR SkipWhitespace( LPCTSTR pcsz )
//
//	Skip past any whitespace
{
#ifdef UNICODE
	while( *pcsz && _istspace( *pcsz ) ) pcsz++;
#else	//	UNICODE
	while( *pcsz && isspace( *pcsz ) ) pcsz++;
#endif	//	UNICODE
	return pcsz;
}

static Container::CMap< CStaticString, UINT > g_mapStyle( 997 );

#if (_WIN32_IE < 0x0300)
#define PBS_SMOOTH              0x01
#define PBS_VERTICAL            0x04
#endif

static void InitStyles()
{
#define MKSTYLE( n )	g_mapStyle.SetAt( _T(#n), n );

	MKSTYLE( DS_MODALFRAME )
	MKSTYLE( DS_ABSALIGN )
	MKSTYLE( DS_SYSMODAL )
	MKSTYLE( DS_LOCALEDIT )
	MKSTYLE( DS_SETFONT )
	MKSTYLE( DS_MODALFRAME )
	MKSTYLE( DS_NOIDLEMSG )
	MKSTYLE( DS_SETFOREGROUND )
	MKSTYLE( DS_3DLOOK )
	MKSTYLE( DS_FIXEDSYS )
	MKSTYLE( DS_NOFAILCREATE )
	MKSTYLE( DS_CONTROL )
	MKSTYLE( DS_CENTER )
	MKSTYLE( DS_CENTERMOUSE )
	MKSTYLE( DS_CONTEXTHELP )

#ifdef DS_SHELLFONT
	MKSTYLE( DS_SHELLFONT )
#endif	//	DS_SHELLFONT
	
	MKSTYLE( WS_OVERLAPPED )
	MKSTYLE( WS_POPUP )
	MKSTYLE( WS_CHILD )
	MKSTYLE( WS_MINIMIZE )
	MKSTYLE( WS_VISIBLE )
	MKSTYLE( WS_DISABLED )
	MKSTYLE( WS_CLIPSIBLINGS )
	MKSTYLE( WS_CLIPCHILDREN )
	MKSTYLE( WS_MAXIMIZE )
	MKSTYLE( WS_CAPTION )
	MKSTYLE( WS_BORDER )
	MKSTYLE( WS_DLGFRAME )
	MKSTYLE( WS_VSCROLL )
	MKSTYLE( WS_HSCROLL )
	MKSTYLE( WS_SYSMENU )
	MKSTYLE( WS_THICKFRAME )
	MKSTYLE( WS_GROUP )
	MKSTYLE( WS_TABSTOP )
	MKSTYLE( WS_MINIMIZEBOX )
	MKSTYLE( WS_MAXIMIZEBOX )
	MKSTYLE( WS_TILED )
	MKSTYLE( WS_ICONIC )
	MKSTYLE( WS_SIZEBOX )
	MKSTYLE( WS_TILEDWINDOW )

	/*
	 * Common Window Styles
	 */
	MKSTYLE( WS_OVERLAPPEDWINDOW )

	MKSTYLE( WS_POPUPWINDOW )

	MKSTYLE( WS_CHILDWINDOW )

	/*
	 * Extended Window Styles
	 */
	MKSTYLE( WS_EX_DLGMODALFRAME )
	MKSTYLE( WS_EX_NOPARENTNOTIFY )
	MKSTYLE( WS_EX_TOPMOST )
	MKSTYLE( WS_EX_ACCEPTFILES )
	MKSTYLE( WS_EX_TRANSPARENT )
	MKSTYLE( WS_EX_MDICHILD )
	MKSTYLE( WS_EX_TOOLWINDOW )
	MKSTYLE( WS_EX_WINDOWEDGE )
	MKSTYLE( WS_EX_CLIENTEDGE )
	MKSTYLE( WS_EX_CONTEXTHELP )
	MKSTYLE( WS_EX_RIGHT )
	MKSTYLE( WS_EX_LEFT )
	MKSTYLE( WS_EX_RTLREADING )
	MKSTYLE( WS_EX_LTRREADING )
	MKSTYLE( WS_EX_LEFTSCROLLBAR )
	MKSTYLE( WS_EX_RIGHTSCROLLBAR )
	MKSTYLE( WS_EX_CONTROLPARENT )
	MKSTYLE( WS_EX_STATICEDGE )
	MKSTYLE( WS_EX_APPWINDOW )
	MKSTYLE( WS_EX_OVERLAPPEDWINDOW )
	MKSTYLE( WS_EX_PALETTEWINDOW )
#if(_WIN32_WINNT >= 0x0500)
	MKSTYLE( WS_EX_LAYERED )
	MKSTYLE( WS_EX_NOINHERITLAYOUT )
	MKSTYLE( WS_EX_LAYOUTRTL )
	MKSTYLE( WS_EX_NOACTIVATE )
#endif	//	(_WIN32_WINNT >= 0x0500)

	//
	//	Edit styles
	MKSTYLE( ES_LEFT )
	MKSTYLE( ES_CENTER )
	MKSTYLE( ES_RIGHT )
	MKSTYLE( ES_MULTILINE )
	MKSTYLE( ES_UPPERCASE )
	MKSTYLE( ES_LOWERCASE )
	MKSTYLE( ES_PASSWORD )
	MKSTYLE( ES_AUTOVSCROLL )
	MKSTYLE( ES_AUTOHSCROLL )
	MKSTYLE( ES_NOHIDESEL )
	MKSTYLE( ES_OEMCONVERT )
	MKSTYLE( ES_READONLY )
	MKSTYLE( ES_WANTRETURN )
#if(WINVER >= 0x0400)
	MKSTYLE( ES_NUMBER )
#endif /* WINVER >= 0x0400 */

	//
	//	Button styles
	MKSTYLE( BS_PUSHBUTTON )
	MKSTYLE( BS_DEFPUSHBUTTON )
	MKSTYLE( BS_CHECKBOX )
	MKSTYLE( BS_AUTOCHECKBOX )
	MKSTYLE( BS_RADIOBUTTON )
	MKSTYLE( BS_3STATE )
	MKSTYLE( BS_AUTO3STATE )
	MKSTYLE( BS_GROUPBOX )
	MKSTYLE( BS_USERBUTTON )
	MKSTYLE( BS_AUTORADIOBUTTON )
	MKSTYLE( BS_OWNERDRAW )
	MKSTYLE( BS_LEFTTEXT )
#if(WINVER >= 0x0400)
	MKSTYLE( BS_TEXT )
	MKSTYLE( BS_ICON )
	MKSTYLE( BS_BITMAP )
	MKSTYLE( BS_LEFT )
	MKSTYLE( BS_RIGHT )
	MKSTYLE( BS_CENTER )
	MKSTYLE( BS_TOP )
	MKSTYLE( BS_BOTTOM )
	MKSTYLE( BS_VCENTER )
	MKSTYLE( BS_PUSHLIKE )
	MKSTYLE( BS_MULTILINE )
	MKSTYLE( BS_NOTIFY )
	MKSTYLE( BS_FLAT )
	MKSTYLE( BS_RIGHTBUTTON )
#endif /* WINVER >= 0x0400 */

	MKSTYLE( PBS_SMOOTH )
	MKSTYLE( PBS_VERTICAL )
}

void QHTM_StylesStartup()
{
	InitStyles();
}

void QHTM_StylesShutdown()
{
	g_mapStyle.RemoveAll();
}



static bool GetStyle( LPCTSTR pcszStart, LPCTSTR pcszCurrent, UINT &uStyle )
{
	//TRACE("Testing style %s\n", CSmallString( pcszStart, pcszCurrent - pcszStart ) );
	pcszStart = SkipWhitespace( pcszStart );
	LPCTSTR pcszEnd = pcszCurrent;
	while( pcszEnd > pcszStart && isspace( *(pcszEnd-1) ) )
	{
		pcszEnd--;
	}

	if( *pcszStart == _T('0') )
	{
	}
	else
	{
		CStaticString str( pcszStart, pcszEnd - pcszStart );
		UINT *pu = g_mapStyle.Lookup( str );
		if( pu )
		{
			//TRACE("Got style %s - 0x%x\n", CSmallString( str, str.GetLength() ), *pu );
			uStyle |= *pu;
			return true;
		}
		else
		{
			//TRACE( _T("WARNING: Unknown style '%s'\n"), StringClass( str, str.GetLength() ) );
		}
	}
	return false;
}


UINT ParseWindowStyle( const CTextABC &str )
{
	UINT uStyle = 0;
	UINT uTempStyle = 0;
	size_t uLength = str.GetLength();
	if( uLength )
	{
		LPCTSTR pcszStart = str;
		LPCTSTR pcszCurrent = str;
		pcszStart = SkipWhitespace( pcszStart );

		while( uLength )
		{
			if( *pcszCurrent == _T('|') )
			{
				if( GetStyle( pcszStart, pcszCurrent, uTempStyle ) )
				{
					uStyle |= uTempStyle;
				}
				pcszStart = pcszCurrent + 1;
			}
			pcszCurrent++;
			uLength--;
		}

		if( GetStyle( pcszStart, pcszCurrent, uTempStyle ) )
		{
			uStyle |= uTempStyle;
		}
	}

	return uStyle;
}

#endif	//	QHTM_WINDOWS_CONTROL
