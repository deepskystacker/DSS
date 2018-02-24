/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	ScrollContainer.cpp
Owner:	russf@gipsysoft.com
Purpose:	General purpose scroll container
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <math.h>
#include "ScrollContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScrollContainer::CScrollContainer()
{
	Reset( 0, 0 );
}


CScrollContainer::~CScrollContainer()
{

}


void CScrollContainer::Reset( int nMaxWidth, int nMaxHeight )
{
	m_nLastScrollPos = m_nScrollPos = 0;
	m_nMaxHeight = nMaxHeight;
	m_nLastScrollPosH = m_nScrollPosH = 0;
	m_nMaxWidth = nMaxWidth;
}


void CScrollContainer::OnLayout( const WinHelper::CRect &rc )
{
	CSectionABC::OnLayout( rc );

	m_nLastScrollPos = max( (m_nMaxHeight - Height()), 0 );
	if( m_nScrollPos > m_nLastScrollPos )
		m_nScrollPos = m_nLastScrollPos;

	m_nLastScrollPosH = max((m_nMaxWidth - Width()), 0);
	if (m_nScrollPosH > m_nLastScrollPosH )
		m_nScrollPosH = m_nLastScrollPosH;
}


void CScrollContainer::OnDraw( GS::CDrawContext &dc )
{
	//
	//	Get a clip rectangle that *only* encompasses this section.
	//	This helps to restrict drawing to just the parts that are visible
	const WinHelper::CRect &rcDCClip = dc.GetClipRect();
	WinHelper::CRect rcClip( max( rcDCClip.left, left )
							, max( rcDCClip.top, top )
							, min( rcDCClip.right, right)
							, min( rcDCClip.bottom, bottom ) );

	const size_t uSize = m_arrSections.GetSize();
	for( size_t n = 0; n < uSize; n++ )
	{
		CSectionABC *psect = m_arrSections[ n ];
		if( rcClip.Intersect( *psect ) )
		{
			psect->OnDraw( dc );
		}
	}
}


void CScrollContainer::InternalScroll( int cx, int cy )
{
	const int nOldPos = m_nScrollPos;
	const int nOldPosH = m_nScrollPosH;

	m_nScrollPos += cy;
	m_nScrollPosH += cx;

	m_nScrollPos = max( 0, min( m_nScrollPos, m_nLastScrollPos ) );
	m_nScrollPosH = max( 0, min( m_nScrollPosH, m_nLastScrollPosH ) );

	int nDeltaY = nOldPos - m_nScrollPos;
	int nDeltaX = nOldPosH - m_nScrollPosH;

	DoInternalScroll( nDeltaX, nDeltaY );
}


void CScrollContainer::InternalScrollAbsolute( int cx, int cy )
{
	//	Ignores extrema!
	const int nOldPos = m_nScrollPos;
	const int nOldPosH = m_nScrollPosH;

	m_nScrollPos += cy;
	m_nScrollPosH += cx;

	const int nDeltaY = nOldPos - m_nScrollPos;
	const int nDeltaX = nOldPosH - m_nScrollPosH;

	DoInternalScroll( nDeltaX, nDeltaY );
}


void CScrollContainer::DoInternalScroll( int nDeltaX, int nDeltaY )
{
	const size_t uSize = m_arrSections.GetSize();
	if( uSize && ( nDeltaX || nDeltaY ) )
	{

		//
		//	Minor optimisation - if we have no need to scroll horizontally then why adjust
		//	all of the X coords. A: There's no reason to do so.
		if( !nDeltaX )
		{
			for( size_t n = 0; n < uSize; n++ )
			{
				CSectionABC *pSect = m_arrSections[ n ];
				pSect->MoveY( nDeltaY );
			}
		}
		else if( nDeltaY || nDeltaX )
		{
			for( size_t n = 0; n < uSize; n++ )
			{
				m_arrSections[ n ]->MoveXY( nDeltaX, nDeltaY );
			}
		}
	}
}



void CScrollContainer::ScrollV( int nAmount )
{
	InternalScroll( 0, nAmount );
	ForceRedraw();
}


void CScrollContainer::ScrollH( int nAmount )
{
	InternalScroll( nAmount, 0 );
	ForceRedraw();
}


int CScrollContainer::GetScrollPos() const
{
	return m_nScrollPos;
}


int CScrollContainer::GetScrollPosH() const
{
	return m_nScrollPosH;
}


void CScrollContainer::SetPos( int nPos )
{
	InternalScroll( 0, nPos - m_nScrollPos );
}


void CScrollContainer::SetPosH( int nPos )
{
	InternalScroll( nPos - m_nScrollPosH, 0 );
}

void CScrollContainer::SetPosAbsolute( int nHPos, int nVPos )
{
	InternalScrollAbsolute( nHPos - m_nScrollPosH, nVPos - m_nScrollPos );
}

bool CScrollContainer::CanScrollUp() const
{
	return m_nScrollPos == 0 ? false : true;
}


bool CScrollContainer::CanScrollDown() const
{
	return m_nScrollPos == m_nLastScrollPos || Height() > m_nMaxHeight ? false : true;
}

bool CScrollContainer::CanScrollLeft() const
{
	return m_nScrollPosH == 0 ? false : true;
}


bool CScrollContainer::CanScrollRight() const
{
	return m_nScrollPosH == m_nLastScrollPosH || Width() > m_nMaxWidth ? false : true;
}
