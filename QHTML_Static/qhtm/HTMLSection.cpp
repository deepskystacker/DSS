/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLSection.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Display section.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <SHELLAPI.H>	//	For ShellExecute
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include "qhtm.h"
#include "defaults.h"
#include "HTMLSectionABC.h"
#include "HTMLSection.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "ResourceLoader.h"
#include "smallstringhash.h"



extern bool LoadTextFile( LPCTSTR pcszFilename, LPTSTR &pszBuffer, UINT &uLength );
extern void DrawTiledBitmap( int x, int y, int cx, int cy, HDC hdc, const CQHTMImageABC *pImage, UINT uFrame );
extern void RemoveFilename( LPTSTR pcszFilename );

LONG g_lHTMLSectionCount = 0;

CHTMLSection::CHTMLSection( CDefaults *pDefaults )
	: m_pDocument( NULL )
	, m_uZoomLevel( pDefaults->m_nZoomLevel )
	, m_pDefaults( pDefaults )
	, m_nBackgroundTimerID( knNullTimerId )
	, m_nBackgroundFrame( 0 )
	, m_pFocus( NULL )
	, m_bFocused( false )
	, m_nLineHeight( 0 )
	, m_hDefaultInstance( GetModuleHandle( NULL ) )
{
	InterlockedIncrement( &g_lHTMLSectionCount );
}


CHTMLSection::~CHTMLSection()
{
	DestroyDocument();
	InterlockedDecrement( &g_lHTMLSectionCount );
	if( g_lHTMLSectionCount == 0 )
	{
		GS::CDrawContext::ClearAllObjects();
	}
}



bool CHTMLSection::SetHTMLFile( LPCTSTR pcszFilename )
{
	bool bRetVal = false;

	LPTSTR pcszHTML = NULL;
	UINT uLength = 0;
	if( LoadTextFile( pcszFilename, pcszHTML, uLength ) )
	{
		TCHAR path_buffer[_MAX_PATH];
		_tcscpy( path_buffer, pcszFilename );
		RemoveFilename( path_buffer );

		SetHTML( pcszHTML, uLength, path_buffer );


		bRetVal = true;

		delete[] pcszHTML;
	}
	return bRetVal;
}


void CHTMLSection::SetHTML( LPCTSTR pcszHTMLText, size_t uLength, LPCTSTR pcszPathToFile )
{
	m_strHTML.Set( pcszHTMLText, uLength );

	CHTMLParse html( pcszHTMLText, uLength, m_hDefaultInstance, pcszPathToFile, m_pDefaults );
	CHTMLDocument *pDoc = html.Parse();

	if( pDoc )
	{
#ifdef _DEBUG
//	pDoc->Dump();
#endif
		DestroyDocument();
		m_pDocument = pDoc;
	}

	OnLoadedDocument();
}


void CHTMLSection::AddHTML( LPCTSTR pcsz )
{
	m_strHTML += pcsz;

	CHTMLParse html( pcsz, static_cast<UINT>( _tcslen( pcsz ) ), m_hDefaultInstance, NULL, m_pDefaults );
	CHTMLDocument *pDoc = html.Parse( m_pDocument );
	if( pDoc )
	{
		//
		//	If there is no current document then simply use the new one
		if( !m_pDocument )
		{
			m_pDocument = pDoc;
		}

		OnLoadedDocument();
	}
}



bool CHTMLSection::SetHTML( HINSTANCE hInst, LPCTSTR pcszName )
{
	CResourceLoader rsrc( hInst );

	if( rsrc.Load( pcszName, RT_RCDATA ) || rsrc.Load( pcszName, RT_HTML ) )
	{
		CHTMLDocument *pDoc = NULL;

#ifdef _UNICODE
		LPTSTR pszHTML = reinterpret_cast<LPTSTR>( malloc( rsrc.GetSize() * sizeof( TCHAR ) + 1 ) );
		{
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)rsrc.GetData(), rsrc.GetSize(), pszHTML, rsrc.GetSize() * sizeof( TCHAR ) );
			CHTMLParse html( pszHTML, rsrc.GetSize(), hInst, NULL, m_pDefaults );
			m_strHTML.Set( pszHTML, rsrc.GetSize() );
#else	//	_UNICODE
			CHTMLParse html( (LPCTSTR)rsrc.GetData(), rsrc.GetSize(), hInst, NULL, m_pDefaults );
			m_strHTML.Set( (LPCTSTR)rsrc.GetData(), rsrc.GetSize() );
#endif	//	_UNICODE
			pDoc = html.Parse();

#ifdef _UNICODE
			free( pszHTML );
		}
#endif	//	_UNICODE

		if( pDoc )
		{
			DestroyDocument();
			m_pDocument = pDoc;

			OnLoadedDocument();

			return true;
		}
	}
	return false;
}


void CHTMLSection::OnLayout( const WinHelper::CRect &rc, GS::CDrawContext &dc )
{
	RemoveAllChildSections( false );

	sizeHTML.cx = sizeHTML.cy = 0;
	if( m_pDocument && rc.Width() )
	{
		sizeHTML = LayoutDocument( dc, m_pDocument
			, rc.top + m_pDocument->m_nTopMargin		//	Start YPos
			, rc.left + m_pDocument->m_nLeftMargin		//	Left
			, rc.right - m_pDocument->m_nRightMargin - 1 );	//	Right
		sizeHTML.cy -= rc.top;
		sizeHTML.cx -= rc.left;
		sizeHTML.cx += m_pDocument->m_nRightMargin;
		sizeHTML.cy += m_pDocument->m_nBottomMargin;
	}

	int nScrollPos = GetScrollPos();
	int nScrollPosH = GetScrollPosH();
	Reset( sizeHTML.cx, sizeHTML.cy );
	CScrollContainer::OnLayout( rc );

	SetPos( nScrollPos );
	SetPosH( nScrollPosH );
}


void CHTMLSection::OnDraw( GS::CDrawContext &dc )
{
	HBRUSH hbr = NULL;
	if( GetBackgroundColours( dc.GetSafeHdc(), hbr ) )
	{
		FillRect( dc.GetSafeHdc(), *this, hbr );
	}
	else if( !IsTransparent() )
	{
		if( m_pDocument )
		{
			if( !m_pDocument->m_back.Draw( dc, *this ) )
			{
				dc.FillRect( *this, m_pDefaults->m_crBackground );
			}

			if( m_pDocument->m_pimgBackground )
			{
				const int nPosX = left - GetScrollPosH();
				const int nPosY = top - GetScrollPos();
				DrawTiledBitmap( nPosX, nPosY, max( GetMaxWidth(), (UINT)Width() ), max( GetMaxHeight(), (UINT)Height() ), dc.GetSafeHdc(), m_pDocument->m_pimgBackground, m_nBackgroundFrame );
			}
		}
		else
		{
			dc.FillRect( *this, m_pDefaults->m_crBackground );
		}
	}
	CScrollContainer::OnDraw( dc );
}


void CHTMLSection::DestroyDocument()
//
//	Remove the child sections and destroy the document
{
	KillBackgroundImageTimer();

	RemoveAllChildSections( true );

	if( m_pDocument )
	{
		delete m_pDocument;
		m_pDocument = NULL;
	}
}


void CHTMLSection::RemoveAllChildSections( bool bRemoveKeepSectionToo )
//
//	Remove all child sections prior to either recreating them or destroying them
//
//	Cleanup all other control objects that are related to the visuals of the control.

{
	(void)m_arrFocus.SetSize( 0 );
	m_pFocus = NULL;

	RemoveAllSections( bRemoveKeepSectionToo );

	// Empty the list of links.
	for (UINT i = 0; i < m_arrLinks.GetSize(); ++i)
		delete m_arrLinks[i];
	//	For safety...
	m_arrLinks.RemoveAll();

	// Clear the map
	m_mapNames.RemoveAll();
	//	Clear the pagination data
	m_arrBreakSections.RemoveAll();
	m_arrPageRects.RemoveAll();
}


void CHTMLSection::LoadFromResource( UINT uID )
{
	CResourceLoader rsrc( m_hDefaultInstance );

	VERIFY( rsrc.Load( uID, RT_RCDATA ) );
	
	CHTMLDocument *pDoc = NULL;

#ifdef _UNICODE
	LPTSTR pszHTML = reinterpret_cast<LPTSTR>( malloc( rsrc.GetSize() * sizeof( TCHAR ) + 1 ) );
	{
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)rsrc.GetData(), rsrc.GetSize(), pszHTML, rsrc.GetSize() * sizeof( TCHAR ) );
		CHTMLParse html( pszHTML, rsrc.GetSize(), m_hDefaultInstance, NULL, m_pDefaults );
#else	//	_UNICODE
		CHTMLParse html( (LPCTSTR)rsrc.GetData(), rsrc.GetSize(), m_hDefaultInstance, NULL, m_pDefaults );
#endif	//	_UNICODE
		pDoc = html.Parse();

#ifdef _UNICODE
		free( pszHTML );
	}
#endif	//	_UNICODE

	if( pDoc )
	{
		DestroyDocument();
		m_pDocument = pDoc;
	}
}


void CHTMLSection::SetDefaultResourceHandle( HINSTANCE hInstance )
{
	m_hDefaultInstance = hInstance;
}


void CHTMLSection::OnExecuteHyperlink( CHTMLSectionLink *pHtmlLink )
{
#if !defined (_WIN32_WCE)
	ShellExecute( GetHwnd(), _T("open"), pHtmlLink->m_strLinkTarget, NULL, NULL, SW_SHOW  );
#else
	ASSERT( FALSE );
#endif	//	#if !defined (_WIN32_WCE)
}


void CHTMLSection::GotoLink( CHTMLSectionLink *pLink )
{
	LPCTSTR pcszLinkTarget = pLink->m_strLinkTarget;
	//
	//	If a link has not been resolved and it is a local link then
	//	we should search the page for a matching link, when found we should
	//	scroll our position to .
	if( *pcszLinkTarget == '#' )
	{
		pcszLinkTarget++;
		GotoLink( pcszLinkTarget );
	}
	else
	{
		OnExecuteHyperlink( pLink );
	}
}


void CHTMLSection::GotoLink( LPCTSTR pcszLinkTarget )
{
	if( *pcszLinkTarget == '#' )
	{
		pcszLinkTarget++;
	}

	// Locate it in the map...
	WinHelper::CPoint* pPoint = m_mapNames.Lookup( StringClass( pcszLinkTarget ) );
	if( pPoint )
	{
		SetPos( pPoint->y - top );
		SetPosH( pPoint->x - left );

		ForceRedraw();
		NotifyParent( 1 );
	}
}


bool CHTMLSection::OnNotify( const CSectionABC *pChild, const int nEventID )
{
	if( NotifyParent( nEventID, pChild ) )
		return true;

	return true;
}



WinHelper::CSize CHTMLSection::LayoutDocument( GS::CDrawContext &dc, CHTMLDocument *pDocument, int nYPos, int nLeft, int nRight )
{
	StringClass string( _T("top") );
	WinHelper::CPoint point( 0, 0 );
	m_mapNames.SetAt( string, point );

	CHTMLSectionCreator htCreate( this, dc, nYPos, nLeft, nRight, m_pDocument->m_back.m_crBack, false, m_uZoomLevel, NULL, NULL );
	htCreate.AddDocument( pDocument );
	m_nLineHeight = dc.GetCurrentFontHeight();
	return htCreate.GetSize();
}


void CHTMLSection::EnableTooltips( bool bEnable ) const
{
	CHTMLSectionABC::EnableTooltips( bEnable );
}


bool CHTMLSection::IsTooltipsEnabled() const
{
	return CHTMLSectionABC::IsTooltipsEnabled();
}


CHTMLSectionLink* CHTMLSection::AddLink(LPCTSTR pcszLinkTarget, LPCTSTR pcszLinkTitle, LPCTSTR pcszID, COLORREF crLink, COLORREF crHover, bool bTitleSet )
{
	CHTMLSectionLink* pLink = new CHTMLSectionLink( pcszLinkTarget, pcszLinkTitle, pcszID, crLink, crHover, bTitleSet, this );
	m_arrLinks.Add( pLink );
	m_arrFocus.Add( pLink );
	return pLink;
}


void CHTMLSection::AddFocusObject( CFocusControlABC *pObject )
{
	m_arrFocus.Add( pObject );
}


void CHTMLSection::AddNamedSection(LPCTSTR name, const WinHelper::CPoint& point)
{
	StringClass string(name);
	m_mapNames.SetAt(string, point);
}

void CHTMLSection::AddBreakSection(size_t i)
{
	// Don't allow duplicates...
	const size_t nArrSize = m_arrBreakSections.GetSize();
	if (nArrSize > 0)
	{
		const size_t nLastSection = m_arrBreakSections[nArrSize - 1];
		if (nLastSection == i)
			return;
	}
	m_arrBreakSections.Add(i);
}


void CHTMLSection::GetDefaultMargins( LPRECT lprect ) const
{
	if( m_pDocument )
	{
		lprect->left = m_pDocument->m_nLeftMargin;
		lprect->top = m_pDocument->m_nTopMargin;
		lprect->right = m_pDocument->m_nRightMargin;
		lprect->bottom= m_pDocument->m_nBottomMargin;
	}
	else
	{
		*lprect = m_pDefaults->m_rcMargins;
	}
}

void CHTMLSection::SetDefaultMargins( LPCRECT lprect )
{
	m_pDefaults->m_rcMargins = *lprect;

	if( m_pDocument )
	{
		//	Too early, need to have set HTML first.
		ASSERT( m_pDocument );

		m_pDocument->m_nLeftMargin = lprect->left;
		m_pDocument->m_nTopMargin = lprect->top;
		m_pDocument->m_nRightMargin = lprect->right;
		m_pDocument->m_nBottomMargin = lprect->bottom;
	}
}

// Returns number of pages
size_t CHTMLSection::Paginate(const WinHelper::CRect& rcPage)
{
	/*	Strategy:
			Using the page break data, attempt to place
			as much as possible on each page, and determine
			the extrema of the rectangle for each page.
			If an object does fit on the remainder of the page,
			AND it is larger than a page, split it onto the current
			page, otherwise, wait until the next page.
	*/
	//	Empty the pagination data
	m_arrPageRects.RemoveAll();
	const int nPageHeight = rcPage.Height();
	const size_t nPageBreaks = m_arrBreakSections.GetSize();

	// Handle the simple case where the whole thing fits on the first page
	if (GetHeight() <= nPageHeight)
	{
		m_arrPageRects.Add(rcPage);
	}
	else
	{
		size_t nBreakIndex;
		UINT nPageTop = 0, nPageBottom = 0;

		for (nBreakIndex = 0; nBreakIndex < nPageBreaks; ++nBreakIndex)
		{
			// See if the next break will fit on this page
			const int nSectionIndex = static_cast< int >( m_arrBreakSections[nBreakIndex] ) - 1;
			if (nSectionIndex < 0)
				continue;
			if ((UINT)nSectionIndex >= GetSectionCount())
				break;
			CSectionABC* psect = GetSectionAt(nSectionIndex);
			const UINT nNextY = psect->bottom;
			// See if it will fit. 
			if (nNextY < nPageTop + nPageHeight)
			{
				// It fits... keep going...
				nPageBottom = max(nPageBottom, nNextY);
			}
			else
			{
				// If the page is empty, or the object is taller
				// than the page height, split the object!
				if (nPageBottom == nPageTop || psect->Height() > nPageHeight)
				{
					// maximum page:
					nPageBottom = nPageTop + nPageHeight;
				}
				// Create a new page
				WinHelper::CRect rect(rcPage.left, nPageTop, rcPage.right, nPageBottom);
				m_arrPageRects.Add(rect);
				nPageBottom++;
				nPageTop = nPageBottom;
			}
		}
		// Handle the remainder...
		while (nPageBottom < (UINT)GetHeight())	// bottom of this
		{
			nPageBottom = nPageTop + nPageHeight;
			WinHelper::CRect rect(rcPage.left, nPageTop, rcPage.right, nPageBottom);
			m_arrPageRects.Add(rect);
			nPageBottom++;
			nPageTop = nPageBottom;
		}
	}
	return m_arrPageRects.GetSize();
}

WinHelper::CRect CHTMLSection::GetPageRect(UINT nPage) const
{
	ASSERT(nPage < m_arrPageRects.GetSize());

	return m_arrPageRects[nPage];
}


const StringClass & CHTMLSection::GetTitle() const
{
	static const StringClass g_strNoTitle;

	if( m_pDocument )
		return m_pDocument->m_strTitle;
	return g_strNoTitle;
}


void CHTMLSection::ResetMeasuringKludge()
{
	if( m_pDocument )
		m_pDocument->ResetMeasuringKludge();
}


void CHTMLSection::SetZoomLevel( UINT uZoomLevel )
{
	ResetMeasuringKludge();
	m_uZoomLevel = uZoomLevel;
}


void CHTMLSection::KillBackgroundImageTimer()
{
	if( m_nBackgroundTimerID !=  knNullTimerId )
	{
		UnregisterTimerEvent( m_nBackgroundTimerID );
		m_nBackgroundTimerID = knNullTimerId;
		m_nBackgroundFrame = 0;
	}	
}


void CHTMLSection::OnLoadedDocument()
{
	m_nBackgroundFrame = 0;

	if( m_pDocument->m_pimgBackground && m_pDocument->m_pimgBackground->GetFrameCount() && m_pDocument->m_pimgBackground->GetFrameTime( 0 ) )
	{
		m_nBackgroundTimerID = RegisterTimerEvent( this, m_pDocument->m_pimgBackground->GetFrameTime( 0 ) );
	}

	m_pFocus = NULL;
	(void)m_arrFocus.SetSize( 0 );
}


void CHTMLSection::OnTimer( int nTimerID )
{
	if( nTimerID == m_nBackgroundTimerID && !IsTransparent() )
	{
		m_nBackgroundFrame++;
		if( m_nBackgroundFrame == m_pDocument->m_pimgBackground->GetFrameCount() )
			m_nBackgroundFrame = 0;

		UnregisterTimerEvent( m_nBackgroundTimerID );
		m_nBackgroundTimerID = RegisterTimerEvent( this, m_pDocument->m_pimgBackground->GetFrameTime( m_nBackgroundFrame ) );
		ForceRedraw();
	}
	else
	{
		CScrollContainer::OnTimer( nTimerID );
	}
}


void CHTMLSection::SetImageCallback( funcQHTMImageCallback pfuncQHTMImageCallback )
{
#ifdef QHTM_ALLOW_IMAGECALLBACK
	m_pDefaults->m_funcQHTMImageCallback = pfuncQHTMImageCallback;
#else
	UNREF( pfuncQHTMImageCallback );
#endif	//	QHTM_ALLOW_IMAGECALLBACK
}


void CHTMLSection::SetBitmapCallback( funcQHTMBitmapCallback pfuncBitmapCallback )
{
#ifdef QHTM_ALLOW_IMAGECALLBACK
	m_pDefaults->m_funcBitmapCallback = pfuncBitmapCallback;
#else
	UNREF( pfuncBitmapCallback );
#endif	//	QHTM_ALLOW_IMAGECALLBACK
}


void CHTMLSection::SetResourceCallback( funcQHTMResourceCallback pfuncQHTMResourceCallback )
{
#ifdef QHTM_ALLOW_RESOURCECALLBACK
	m_pDefaults->m_funcQHTMResourceCallback = pfuncQHTMResourceCallback;
#else
	UNREF( pfuncQHTMResourceCallback );
#endif	//	QHTM_ALLOW_IMAGECALLBACK
}

void CHTMLSection::SetFormCallback( funcQHTMFORMCallback pfuncQHTMFORMCallback )
{
#ifdef QHTM_ALLOW_RESOURCECALLBACK
	m_pDefaults->m_funcQHTMFORMCallback = pfuncQHTMFORMCallback;
#else
	UNREF( pfuncQHTMFORMCallback );
#endif	//	QHTM_ALLOW_IMAGECALLBACK
}


void CHTMLSection::SetEventCallback( funcQHTMEventCallback pfuncQHTMEventCallback )
{
	m_pDefaults->m_funcQHTMEventCallback = pfuncQHTMEventCallback;
}




void CHTMLSection::SetCurrentFocus( CFocusControlABC *pFocus, bool bScrollIntoView )
{
	if( m_pFocus )
	{
		m_pFocus->SetFocus( false );
	}

	m_pFocus = pFocus;

	if( m_pFocus )
	{
		m_pFocus->SetFocus( true );

		if( bScrollIntoView )
		{
			//
			//	Now we need to ensure that our focused section is visible
			WinHelper::CRect rcBounds;
			m_pFocus->GetObjectRect( rcBounds );
			bool bChanged = false;
			if( rcBounds.Height() > Height() )
			{
				SetPos( rcBounds.top );
				bChanged = true;
			}
			else
			{
				rcBounds.Offset( GetScrollPosH(), GetScrollPos() );

				int nPos = GetScrollPos();
				int nBottom = nPos + Height();

				if( rcBounds.top < nPos )
				{
					ScrollV( rcBounds.top - nPos );
					bChanged = true;
				}
				else if( rcBounds.bottom > nBottom )
				{
					ScrollV( rcBounds.bottom - nBottom );
					bChanged = true;
				}
			}

			if( bChanged )
			{
				ForceRedraw();
				(void)NotifyParent( 1 );
			}
		}
	}	
}


size_t CHTMLSection::FindOurControl( CFocusControlABC *pFocusControl ) const
{
	const size_t uSize = m_arrFocus.GetSize();
	for( size_t u = 0; u < uSize; u++ )
	{
		if( pFocusControl == m_arrFocus[ u ] )
		{
			return u;
		}
	}
	return 0;
}


CFocusControlABC *CHTMLSection::FindSelectedFromGroup( CFocusControlABC *pFocusNext )
{
	const size_t uSize = m_arrFocus.GetSize();

	//
	//	Find the start of our group....
	UINT uStart = 0;
	UINT u;
	for( u = 0; u < uSize; u++ )
	{
		CFocusControlABC *pTest = m_arrFocus[ u ];
		if( pFocusNext->IsSameName( pTest->GetName() ) )
		{
			uStart = u;
			break;
		}
	}

	for( u = uStart; u < uSize; u++ )
	{
		CFocusControlABC *pTest = m_arrFocus[ u ];
		if( pFocusNext->IsSameName( pTest->GetName() ) && pTest->IsSelected() )
		{
			pFocusNext = pTest;
			break;
		}
	}
	
	return pFocusNext;
}


bool CHTMLSection::SelectPreviousInGroup()
{
	if( m_pFocus )
	{
		size_t uStart = FindOurControl( m_pFocus );
		for( int n = static_cast< int >( uStart ) - 1; n  >= 0; n-- )
		{
			CFocusControlABC *pTest = m_arrFocus[ n ];
			if( m_pFocus->IsSameName( pTest->GetName() ) )
			{
				SetCurrentFocus( pTest, true );
				return true;
			}
		}
	}
	return false;
}


bool CHTMLSection::SelectNextInGroup()
{
	if( m_pFocus )
	{
		const size_t uSize = m_arrFocus.GetSize();
		size_t uStart = FindOurControl( m_pFocus );
		for( size_t u = uStart + 1; u < uSize; u++ )
		{
			CFocusControlABC *pTest = m_arrFocus[ u ];
			if( m_pFocus->IsSameName( pTest->GetName() ) )
			{
				SetCurrentFocus( pTest, true );
				return true;
			}
		}
	}
	return false;
}


bool CHTMLSection::TabToNextControl()
{
#ifdef QHTM_ALLOW_FOCUS
	CFocusControlABC *pFocusNext = NULL;

	const size_t uSize = m_arrFocus.GetSize();
	if( m_pFocus )
	{
		size_t uStart = FindOurControl( m_pFocus );
		for( size_t u = uStart + 1; u < uSize; u++ )
		{
			CFocusControlABC *pTest = m_arrFocus[ u ];
			if( !m_pFocus->IsSameName( pTest->GetName() ) )
			{
				pFocusNext = pTest;
				break;
			}
		}
	}

	if( !pFocusNext && uSize )
	{
		pFocusNext = m_arrFocus[ 0 ];
	}

	pFocusNext = FindSelectedFromGroup( pFocusNext );
	
	SetCurrentFocus( pFocusNext, true );

	if( pFocusNext )
	{
		return true;
	}
#endif	//	QHTM_ALLOW_FOCUS
	return false;
}


bool CHTMLSection::TabToPreviousControl()
{
#ifdef QHTM_ALLOW_FOCUS
	CFocusControlABC *pFocusPrevious = NULL;

	const size_t uSize = m_arrFocus.GetSize();
	if( m_pFocus )
	{
		size_t uStart = FindOurControl( m_pFocus );
		for( int n = static_cast< int >( uStart ) - 1; n  >= 0; n-- )
		{
			CFocusControlABC *pTest = m_arrFocus[ n ];
			if( !m_pFocus->IsSameName( pTest->GetName() ) )
			{
				pFocusPrevious = pTest;
				break;
			}
		}
	}

	if( !pFocusPrevious && uSize )
	{
		pFocusPrevious = m_arrFocus[ uSize - 1 ];
	}

	pFocusPrevious = FindSelectedFromGroup( pFocusPrevious );
	
	SetCurrentFocus( pFocusPrevious, true );

	if( pFocusPrevious )
	{
		return true;
	}
#endif	//	QHTM_ALLOW_FOCUS
	return false;
}


void CHTMLSection::ActivateLink()
{
	if( m_pFocus )
	{
		m_pFocus->Activate();
	}
}


bool CHTMLSection::CanTabForward() const
{
	if( m_pFocus )
	{
		const size_t uSize = m_arrFocus.GetSize();
		if( uSize > 1 && m_arrFocus[ uSize - 1 ] != m_pFocus )
		{
			return true;
		}
	}
	return false;
}


bool CHTMLSection::CanTabBackward() const
{
	if( m_pFocus )
	{
		if( m_arrFocus.GetSize() )
		{
			if( m_pFocus != m_arrFocus[ 0 ] )
			{
				return true;
			}
		}
	}
	return false;
}


void CHTMLSection::SetFocusControl( bool bFirst )
{
	const size_t uSize = m_arrFocus.GetSize();
	if( uSize )
	{
		if( bFirst )
		{
			SetCurrentFocus( m_arrFocus[ 0 ], false );
		}
		else
		{
			SetCurrentFocus( m_arrFocus[ uSize - 1 ], false );
		}
	}
}


void CHTMLSection::SetFocus( bool bFocused )
{
	m_bFocused = bFocused;

	if( m_bFocused )
	{
		if( !m_pFocus && m_arrFocus.GetSize() )
		{
			SetCurrentFocus( m_arrFocus[ 0 ], false );
		}
	}
	else if( m_pFocus )
	{
		m_pFocus->Update();
	}
}


bool CHTMLSection::HasControls() const
{
	return m_arrFocus.GetSize() ? true : false;
}


static void DigitToHexChar( UCHAR uVal, TCHAR &ch )
{
	ch = char(( uVal < UCHAR(10) ? uVal + _T('0') : uVal - 10 + _T('a') ));
}

static inline void CharToHex( TCHAR ch, LPTSTR pszHexBuf )
{
	DigitToHexChar( UCHAR( ch >> 4 &0x0f ), pszHexBuf[0] );
	DigitToHexChar( UCHAR( ch & 0xf), pszHexBuf[1] );
}

static void EncodeURL( const StringClass &strFrom, StringClass &strTo )
{
	strTo.Empty();
	if( strFrom.GetLength() == 0 )
		return;

	TCHAR   chCurrentChar;
	TCHAR   szHex[4] = _T("%");

	LPCTSTR pcszFrom = strFrom;
	while( *pcszFrom )
	{
		chCurrentChar = *pcszFrom;

		pcszFrom++;

		if ((chCurrentChar>= _T('0') && chCurrentChar<= _T('9')) ||
			 (chCurrentChar>= _T('a') && chCurrentChar<= _T('z')) ||
			 (chCurrentChar>= _T('A') && chCurrentChar<= _T('Z')))
		{
			strTo.Add( &chCurrentChar, 1 );
			continue;
		}

		if( chCurrentChar == _T(' ') )
		{
			strTo.Add( _T("+"), 1 );
			continue;

		}
		CharToHex( chCurrentChar, &szHex[1] );
		strTo.Add( szHex, 3 );
	}
} 


void CHTMLSection::SubmitForm( CHTMLForm *pForm, LPCTSTR pcszSubmitValue )
{
	pForm->UpdateFormFromControls();

	Container::CArray< CHTMLFormField > arrField;
	pForm->GetFormFields( arrField );

	CHTMLFormField &ffSubmit = arrField.Add();
	ffSubmit.m_strName = _T("submit");
	ffSubmit.m_strValue = pcszSubmitValue;

	if( !_tcsicmp( pForm->m_strMethod, _T("GET") ) )
	{
		StringClass strURL, strParameters;
		strURL = pForm->m_strAction;
		strURL += _T("?");

		if( arrField.GetSize() )
		{
			for( UINT u = 0; u < arrField.GetSize(); u++ )
			{
				StringClass strName, strValue;
				
				EncodeURL( arrField[ u ].m_strName, strName );
				EncodeURL( arrField[ u ].m_strValue, strValue );
				if( strParameters.GetLength() )
				{
					strParameters += _T("&");
				}
				strParameters += strName;
				strParameters += _T("=");
				strParameters += strValue;
			}

			strURL += strParameters;
		}

		CHTMLSectionLink link( strURL, _T(""), _T(""), 0, 0, NULL, false );
		OnExecuteHyperlink( &link );
	}
	else
	{
		//
		//	We need to do something else with it...
		if( m_pDefaults->m_funcQHTMFORMCallback )
		{
			QHTMFORMSubmit	form = {0};
			form.cbSize = sizeof( form );
			form.pcszMethod = pForm->m_strMethod;
			form.pcszAction = pForm->m_strAction;
			form.pcszName = pForm->m_strName;
			form.uFieldCount = static_cast< UINT >( arrField.GetSize() );

			Container::CArray< QHTMFORMSubmitField > arrFields;

			if( arrField.GetSize() )
			{
				for( UINT u = 0; u < arrField.GetSize(); u++ )
				{
					QHTMFORMSubmitField &f = arrFields.Add();
					f.pcszName = arrField[ u ].m_strName;
					f.pcszValue = arrField[ u ].m_strValue;
				}
				form.parrFields = arrFields.GetData();
			}
			m_pDefaults->m_funcQHTMFORMCallback( GetHwnd(), &form, m_pDefaults->m_lParam );
		}
		else
		{
			//	Do something!
		}
	}
}


const CHTMLSectionLink *CHTMLSection::GetLinkFromSection( const CSectionABC *pSect ) const
{
	const size_t uLinks = m_arrLinks.GetSize();
	for( size_t uLink = 0; uLink < uLinks; uLink++ )
	{
		const CHTMLSectionLink *pSectionLink = m_arrLinks[ uLink ];
		const size_t uSections = pSectionLink->m_arrSections.GetSize();
		for( size_t uSection = 0; uSection < uSections; uSection++ )
		{
			if( pSectionLink->m_arrSections[ uSection ] == pSect )
			{
				return pSectionLink;
			}
		}
	}
	return NULL;
}


void CHTMLSection::SelectNearestLink( const POINT &pt )
{
	CHTMLSectionLink *pNearestLink = NULL;
	UINT uNearestDistance = UINT_MAX;

	//
	//	Iterate the list of links and see if we get a match...
	const size_t uLinkCount = m_arrLinks.GetSize();
	for( size_t u = 0; u < uLinkCount; u++ )
	{
		CHTMLSectionLink *pLink = m_arrLinks[ u ];
		int nX = INT_MAX;
		int nY = INT_MAX;

		pLink->GetPointDistance( pt, nX, nY );
		UINT uDistance = ((UINT)nY<<16) + nX;
		if( uDistance < uNearestDistance )
		{
			uNearestDistance = uDistance;
			pNearestLink = pLink;
		}
	}

	if( pNearestLink )
	{
		SetCurrentFocus( pNearestLink, false );
	}
}


bool CHTMLSection::ResetForm( LPCTSTR pcszFormName )
{
	if( m_pDocument )
	{
		CHTMLForm *pFoundForm = NULL;

		for( UINT u = 0; u < m_pDocument->m_arrForms.GetSize(); u++ )
		{
			CHTMLForm *pForm = m_pDocument->m_arrForms[ u ];
			if( !_tcsicmp( pForm->m_strName, pcszFormName ) )
			{
				pFoundForm = pForm;
				break;
			}
		}

		if( pFoundForm )
		{
			pFoundForm->ResetContent();
			return true;
		}
	}
	return false;
}


bool CHTMLSection::SubmitForm( LPCTSTR pcszFormName, LPCTSTR pcszSubmitValue )
{
	if( m_pDocument )
	{
		CHTMLForm *pFoundForm = NULL;

		for( UINT u = 0; u < m_pDocument->m_arrForms.GetSize(); u++ )
		{
			CHTMLForm *pForm = m_pDocument->m_arrForms[ u ];
			if( pForm->m_strName.GetLength() && !_tcsicmp( pForm->m_strName, pcszFormName ) )
			{
				pFoundForm = pForm;
				break;
			}
		}

		if( pFoundForm )
		{
			SubmitForm( pFoundForm, pcszSubmitValue );
			return true;
		}
	}
	return false;
}


HRGN CHTMLSection::GetLinkRegion( LPCTSTR pcszLinkID ) const
{
	HRGN hrgn = NULL;
	if( pcszLinkID && *pcszLinkID )
	{

		const size_t uLinkCount = m_arrLinks.GetSize();
		for( size_t u = 0; u < uLinkCount; u++ )
		{

			const CHTMLSectionLink *pLink = m_arrLinks[ u ];
			if( pLink->m_strLinkID.GetLength() )
			{

				if( !_tcsicmp( pLink->m_strLinkID, pcszLinkID ) )
				{
					(void)pLink->GetRegion( hrgn );
					break;
				}

			}
		}

	}

	return hrgn;
}


void CHTMLSection::SetFocusAndActivate( LPCTSTR pcszNameSection )
{
	for( size_t u = 0; u < m_arrFocus.GetSize(); u++ )
	{
		CFocusControlABC * pItem = m_arrFocus[ u ];
		if( pItem->IsSameID( pcszNameSection ) )
		{
			SetCurrentFocus( pItem, true );
			pItem->Activate();
			break;
		}
	}
}