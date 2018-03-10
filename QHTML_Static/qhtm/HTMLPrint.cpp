/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	
Owner:	rich@woodbridgeinternalmed.com
Purpose:	Quick HTM print interface
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <LIMITS.H>
#include "Defaults.h"
#include "QHTM.h"
#include "HtmlSection.h"

#pragma warning( disable : 4100 )	//	unreferenced formal parameter

//	The actual structure referred to by the QHTMCONTEXT is private to this
//	module. We should be checking that the context truly exists. They can be stored
//	in an map for this purpose.

#ifdef QHTM_ALLOW_PRINT

struct CQHTMPrintContext
{
	CHTMLSection	m_htmlSection;	//	The section that does the work
	WinHelper::CRect			m_rcLayout;	//	The rect used to layout the data
	UINT			m_nPages;	// Number of pages 
	UINT			m_uZoomLevel;		//	HTML zoom level
	int	m_cxDeviceScaleNumer, m_cxDeviceScaleDenom;
	int	m_cyDeviceScaleNumer, m_cyDeviceScaleDenom;

	CQHTMPrintContext( UINT uZoomLevel ) : m_htmlSection( &g_defaults) ,m_uZoomLevel(uZoomLevel) { m_htmlSection.SetZoomLevel( m_uZoomLevel ); }

private:
	CQHTMPrintContext(const CQHTMPrintContext& );
	CQHTMPrintContext &operator =( const CQHTMPrintContext &);

};

static MapClass< QHTMCONTEXT, CQHTMPrintContext* > g_mapPrintContext;

#endif	//	QHTM_ALLOW_PRINT



//	Call this to create a new print context
QHTMCONTEXT WINAPI QHTM_PrintCreateContext( UINT uZoomLevel )
{
	if( uZoomLevel > QHTM_ZOOM_MAX )
		uZoomLevel = QHTM_ZOOM_MAX;
	if( uZoomLevel < QHTM_ZOOM_MIN )
		uZoomLevel = QHTM_ZOOM_MIN;

#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext* ctxNew = new CQHTMPrintContext( uZoomLevel );
	if (ctxNew)
	{
		QHTMCONTEXT ctx = (QHTMCONTEXT)ctxNew;
		g_mapPrintContext.SetAt(ctx, ctxNew);
		return ctx;
	}
	else
#endif	//QHTM_ALLOW_PRINT
		return (QHTMCONTEXT)0;
}

//	Call this to destroy a print context
BOOL WINAPI QHTM_PrintDestroyContext( QHTMCONTEXT ctx )
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		delete *ppctx;
		g_mapPrintContext.RemoveAt( ctx );
		return TRUE;
	}
	else
#endif	//	QHTM_ALLOW_PRINT
		return FALSE;
}

//	Call this to set the HTML
BOOL WINAPI QHTM_PrintSetText( QHTMCONTEXT ctx, LPCTSTR pcszText )
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		(*ppctx)->m_htmlSection.SetHTML( pcszText, lstrlen( pcszText ), NULL );
		return TRUE;
	}
	else
#endif	//	QHTM_ALLOW_PRINT
		return FALSE;
}


//	Call this to layout the HTML. Returns the number of pages in nPages
BOOL WINAPI QHTM_PrintLayout( QHTMCONTEXT ctx, HDC hDC, LPCRECT pRect, LPINT nPages )
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		CQHTMPrintContext & printCtx = *(*ppctx);
		printCtx.m_rcLayout = *pRect;
		printCtx.m_rcLayout.Offset( -printCtx.m_rcLayout.left, -printCtx.m_rcLayout.top );

		HDC screenDC = ::GetDC( NULL );
		printCtx.m_cxDeviceScaleNumer = ::GetDeviceCaps(hDC, LOGPIXELSX);
		printCtx.m_cxDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSX);
		printCtx.m_cyDeviceScaleNumer = ::GetDeviceCaps(hDC, LOGPIXELSY);
		printCtx.m_cyDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSY);
		::ReleaseDC( NULL, screenDC );

		GS::CDrawContext dc( NULL, hDC, true );
		dc.SetScaling( printCtx.m_cxDeviceScaleNumer, printCtx.m_cxDeviceScaleDenom, printCtx.m_cyDeviceScaleNumer, printCtx.m_cyDeviceScaleDenom );

		printCtx.m_htmlSection.OnLayout( printCtx.m_rcLayout,  dc );
		printCtx.m_nPages = *nPages = static_cast< UINT >( printCtx.m_htmlSection.Paginate( printCtx.m_rcLayout ) );
		return TRUE;
	}
	else
#endif	//	QHTM_ALLOW_PRINT
		return FALSE;
}


BOOL WINAPI QHTM_PrintSetTextResource( QHTMCONTEXT ctx, HINSTANCE hInst, LPCTSTR pcszName )
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		if( (*ppctx)->m_htmlSection.SetHTML( hInst, pcszName ) )
		{
			return TRUE;
		}
	}
#endif	//	QHTM_ALLOW_PRINT
	return FALSE;
}


BOOL WINAPI QHTM_PrintSetTextFile( QHTMCONTEXT ctx, LPCTSTR pcszFilename )
{
	BOOL bRetVal = FALSE;
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		CHTMLSection* pSection = &(*ppctx)->m_htmlSection;		

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];
		_tsplitpath( pcszFilename, drive, dir, fname, ext );
		TCHAR path_buffer[_MAX_PATH];
		_tmakepath( path_buffer, drive, dir, NULL, NULL );

		FILE *pFile = _tfopen( pcszFilename, _T("rb") );
		if( pFile )
		{
			fpos_t pos;
			if( !fseek( pFile, 0, SEEK_END ) && !fgetpos( pFile, &pos ) && !fseek( pFile, 0, SEEK_SET ) )
			{
				UINT uLength = static_cast<UINT>( pos );
				char * pszHTML = reinterpret_cast<char *>( malloc( uLength + 1 ) );
				if( pszHTML )
				{
					if( fread( reinterpret_cast<void*>(pszHTML), uLength, 1, pFile ) )
					{
						pszHTML[ uLength ] = _T('\000');

#ifdef _UNICODE
						LPTSTR pszHTML2 = reinterpret_cast<LPTSTR>( malloc( uLength * sizeof( TCHAR ) + 1 ) );
						MultiByteToWideChar(CP_ACP, 0, pszHTML, uLength, pszHTML2, uLength * sizeof( TCHAR ) );
						LPTSTR pszText = pszHTML2;
#else	//	_UNICODE
						LPTSTR pszText = pszHTML;
#endif	//	_UNICODE

						pSection->SetHTML( pszText, uLength, path_buffer );

#ifdef _UNICODE
						free( pszHTML2 );
#endif	//	_UNICODE
						bRetVal = TRUE;
					}
					free( pszHTML );
				}
			}
			fclose( pFile );
		}
	}
#endif	//	QHTM_ALLOW_PRINT
	return bRetVal;
}


BOOL WINAPI QHTM_PrintPage( QHTMCONTEXT ctx, HDC hDC, UINT nPage, LPCRECT rDest)
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		CQHTMPrintContext & printCtx = *(*ppctx);

		//	Validate page number
		if( nPage >= printCtx.m_nPages )
			return FALSE;

		//	Prevent background painting.
		//printCtx.m_htmlSection.Transparent( true );

		//	Get the page rect (clip rect)
		WinHelper::CRect rcPage = printCtx.m_htmlSection.GetPageRect( nPage );

		//	The dest rect maps to the rect rcPage. 
		//	Ensure that the cliprect matches the minimum of rcPage, rcDest
		//	In this mode, we are always printing from top,left
		WinHelper::CRect rcClip( rcPage );

		// If this rect is taller or wider than rcPage, trim it
		if( rcClip.Height() > rcPage.Height() )
			rcClip.bottom = rcClip.top + rcPage.Height();

		if( rcClip.Width() > rcPage.Width() )
			rcClip.right = rcClip.left + rcPage.Width();

		//	Scroll the document so that 0,0 points to the top left
		//	of the clipRect
		// Scroll the document...
		printCtx.m_htmlSection.SetPosAbsolute(rcPage.left, rcPage.top);

		//	Handle the case where the destination is not top/left
		POINT oldWindowOrg;
		VAPI( ::SetWindowOrgEx( hDC, -rDest->left, -rDest->top, &oldWindowOrg ) );
		rcClip.Offset(-rcClip.left, -rcClip.top);	// Move to 0,0

		GS::CDrawContext dc( &rcClip, hDC, true );
		dc.SetScaling( printCtx.m_cxDeviceScaleNumer, printCtx.m_cxDeviceScaleDenom, printCtx.m_cyDeviceScaleNumer, printCtx.m_cyDeviceScaleDenom );

		//	Do the printing
		printCtx.m_htmlSection.OnDraw( dc );

		// Restore context
		VAPI( ::SetWindowOrgEx( hDC, oldWindowOrg.x, oldWindowOrg.y, NULL ) );

		//
		return TRUE;
	}
	else
#endif	//	QHTM_ALLOW_PRINT
		return FALSE;
}


int WINAPI QHTM_PrintGetHTMLHeight( HDC hDC, LPCTSTR pcszText, int nMaxWidth, UINT uZoomLevel )
{
	if( uZoomLevel > QHTM_ZOOM_MAX )
		uZoomLevel = QHTM_ZOOM_MAX;

	if( uZoomLevel < QHTM_ZOOM_MIN )
		uZoomLevel = QHTM_ZOOM_MIN;

	int nHeight = -1;
#ifdef QHTM_ALLOW_PRINT
	if( hDC && pcszText && nMaxWidth )
	{
		CHTMLSection	htmlSection( &g_defaults );
		htmlSection.SetZoomLevel( uZoomLevel );
		htmlSection.SetHTML( pcszText, lstrlen( pcszText ), NULL );
		WinHelper::CRect rc( 0, 0, nMaxWidth, INT_MAX );

		HDC screenDC = ::GetDC(NULL);
		int cxDeviceScaleNumer = ::GetDeviceCaps(hDC, LOGPIXELSX);
		int cxDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSX);
		int cyDeviceScaleNumer = ::GetDeviceCaps(hDC, LOGPIXELSY);
		int cyDeviceScaleDenom = ::GetDeviceCaps(screenDC, LOGPIXELSY);
		::ReleaseDC(NULL, screenDC);

		GS::CDrawContext dc( NULL, hDC, true );
		dc.SetScaling( cxDeviceScaleNumer, cxDeviceScaleDenom, cyDeviceScaleNumer, cyDeviceScaleDenom );

		htmlSection.OnLayout( rc,  dc );
		nHeight = htmlSection.GetSize().cy;
	}
#endif	//	QHTM_ALLOW_PRINT
	return nHeight;
}

//	Call this to set the HTML
BOOL WINAPI QHTM_PrintSetImageCallback( QHTMCONTEXT ctx, funcQHTMImageCallback pfunc )
{
#ifdef QHTM_ALLOW_PRINT
	CQHTMPrintContext** ppctx = g_mapPrintContext.Lookup( ctx );
	if (ppctx)
	{
		(*ppctx)->m_htmlSection.SetImageCallback( pfunc );
		return TRUE;
	}
	else
#endif	//	QHTM_ALLOW_PRINT
		return FALSE;
}
