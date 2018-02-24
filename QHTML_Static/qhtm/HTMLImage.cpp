/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLImage.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Image object
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "defaults.h"
#include "HTMLSectionCreator.h"
#include "HTMLImageSection.h"

CHTMLImage::CHTMLImage( int nWidth, int nHeight, int nBorder, LPCTSTR pcszFilename, CStyle::Align alg, CQHTMImageABC *pImage, const CStaticString &strALTText )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_nWidth( nWidth )
	, m_nHeight( nHeight )
	,	m_nBorder( nBorder )
	, m_strFilename( pcszFilename )
	, m_alg( alg )
	, m_pImage( pImage )
	, m_strALTText( strALTText.GetData(), strALTText.GetLength() )
{
}


CHTMLImage::~CHTMLImage()
{
	// Since images are cached in the document, they are not owned here.
	// This object just points to the image in the document.
	// delete m_pImage;
}

#ifdef _DEBUG
void CHTMLImage::Dump() const
{
	TRACENL( _T("Image\n") );
	TRACENL( _T("\tName(%s)\n"), (LPCTSTR)m_strFilename );
	TRACENL( _T("\t Width(%d)\n"), m_nWidth );
	TRACENL( _T("\t Height(%d)\n"), m_nHeight );
	TRACENL( _T("\tAlignment (%s)\n"), GetStringFromAlignment( m_alg ) );
}
#endif	//	_DEBUG


void CHTMLImage::AddDisplayElements( class CHTMLSectionCreator *psc )
{
#ifdef QHTM_BUILD_INTERNAL_IMAGING
	ASSERT( m_pImage );
	WinHelper::CSize size( m_pImage->GetSize() );
#else	//	QHTM_BUILD_INTERNAL_IMAGING
	WinHelper::CSize size( 100, 100 );
	if( m_pImage )
		size = m_pImage->GetSize();
#endif	//	QHTM_BUILD_INTERNAL_IMAGING

	if( psc->GetDC().IsPrinting() )
		size = psc->GetDC().Scale( size );

	int nWidth = m_nWidth;
	int nHeight = m_nHeight;
	const int nBorder = m_nBorder;

	if( nWidth == 0 )
	{
		nWidth = size.cx;
	}
	else if( nWidth < 0 )
	{
		nWidth = psc->GetCurrentWidth();
	}
	else
	{
		nWidth = psc->GetDC().ScaleX( nWidth );
	}

	if( nHeight == 0 )
		nHeight = size.cy;
	else
		nHeight = psc->GetDC().ScaleY( nHeight );

	nWidth += psc->GetDC().ScaleX(nBorder * 2);
	nHeight += psc->GetDC().ScaleY(nBorder * 2);

	int nTop = psc->GetCurrentYPos();
	int nBaseline = nHeight;

	if( nWidth > psc->GetRightMargin() - psc->GetCurrentXPos() )
	{
		psc->CarriageReturn( true );
		nTop = psc->GetCurrentYPos();
	}
	int nLeft = psc->GetCurrentXPos();

	switch( m_alg )
	{
	case CStyle::algBottom:
		break;


	case CStyle::algCentre:
	case CStyle::algMiddle:
		nBaseline = nHeight / 2;
		break;


	case CStyle::algTop:
		nBaseline = psc->GetDC().GetCurrentFontBaseline();
		break;


	case CStyle::algLeft:
		{
			nLeft = psc->GetLeftMargin();
			psc->AddNewLeftMargin( nLeft + nWidth + g_defaults.m_nImageMargin, psc->GetCurrentYPos() + nHeight );

			if( psc->GetCurrentXPos() == nLeft )
				psc->SetCurrentXPos( psc->GetLeftMargin() );
		}
		break;


	case CStyle::algRight:
		{
			nLeft = psc->GetRightMargin() - nWidth;
			psc->AddNewRightMargin( nLeft - g_defaults.m_nImageMargin, psc->GetCurrentYPos() + nHeight );
		}
		break;
	}


	CHTMLImageSection *pImageSection = (CHTMLImageSection *)psc->GetHTMLSection()->GetKeeperItemByID( m_uID );
	if( !pImageSection )
	{
		pImageSection = new CHTMLImageSection( psc->GetHTMLSection(), m_pImage, nBorder );
		pImageSection->SetID( m_uID );
		pImageSection->SetElementID( m_strElementID );
	}

	if( m_strALTText.GetLength() )
	{
		pImageSection->SetTipText( m_strALTText );
	}

	psc->AddSection( pImageSection );

	pImageSection->Set( nLeft, nTop, nLeft + nWidth, nTop + nHeight );

	if( m_alg != CStyle::algLeft && m_alg != CStyle::algRight )
	{
		psc->SetCurrentXPos( psc->GetCurrentXPos() + nWidth );
		psc->AddBaseline( nBaseline );
	}
}