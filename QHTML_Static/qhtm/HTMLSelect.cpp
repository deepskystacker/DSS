/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLSelect.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "htmlparse.h"
#include "HTMLOption.h"
#include "HTMLSectionCreator.h"
#include "HTMLComboSection.h"
#include "HTMLListboxSection.h"
#include "defaults.h"


CHTMLSelect::CHTMLSelect( const HTMLFontDef * pFont )
	: m_uSize( 4 )
	, m_bMultiple( false )
	, m_pFont( pFont )
	, m_pCurrentOption( NULL )
{
}


CHTMLSelect::~CHTMLSelect()
{
	for( UINT nIndex = 0; nIndex < m_arrItems.GetSize(); nIndex++ )
	{
		delete m_arrItems[ nIndex ];
	}
	m_arrItems.RemoveAll();
}


void CHTMLSelect::AddItem( CHTMLOption *pItem )
{
	m_arrItems.Add( pItem );
	m_pCurrentOption = pItem;
}


#ifdef _DEBUG
void CHTMLSelect::Dump() const
{
	const size_t size = m_arrItems.GetSize();

	TRACENL( _T("Select----------------\n") );
	TRACENL( _T(" Name %s\n"), (LPCTSTR)m_strName );
	TRACENL( _T(" Size %d\n"), m_uSize );
	TRACENL( _T(" Multiple %s\n"), m_bMultiple ? _T("true") : _T("false") );
	TRACENL( _T(" bDisabled %s\n"), m_bDisabled ? _T("true") : _T("false") );

	for( size_t nIndex = 0; nIndex < size; nIndex++ )
	{
		TRACENL( _T(" Item %d\n"), nIndex );
		m_arrItems[ nIndex ]->Dump();
	}
}
#endif	//	_DEBUG


void CHTMLSelect::AddDisplayElements( class CHTMLSectionCreator *psc )
{
#ifdef QHTM_ALLOW_FORMS
	GS::FontDef fdef( m_pFont->m_strFont, m_pFont->m_nSize, m_pFont->m_nWeight, m_pFont->m_bItalic, m_pFont->m_bUnderline, m_pFont->m_bStrike, psc->GetCurrentCharSet(), m_pFont->m_bFixedPitchFont );
	fdef.m_nSizePixels = GetFontSizeAsPixels( psc->GetDC().GetSafeHdc(), m_pFont->m_nSize, psc->GetZoomLevel() );

	if( m_bMultiple )
	{
		
		CHTMLListboxSection *pSect = (CHTMLListboxSection *)psc->GetHTMLSection()->GetKeeperItemByID( m_uObjectID );
		if( !pSect )
		{
			pSect = new CHTMLListboxSection( psc->GetHTMLSection(), fdef, this, psc );
			pSect->SetID( m_uObjectID );
		}

		//
		//	Figure out our sizes...
		WinHelper::CSize size;
		int nBaseline = 0;
		pSect->FigureOutSize( psc, size, nBaseline );

		const int nWidth = size.cx;
		const int nHeight = size.cy;

		//
		//	See if we'll fit
		int nTop = psc->GetCurrentYPos();
		if( nWidth > psc->GetRightMargin() - psc->GetCurrentXPos() )
		{
			psc->CarriageReturn( true );
			nTop = psc->GetCurrentYPos();
		}

		//
		//	Add our section into the mix
		psc->AddSection( pSect );

		int nLeft = psc->GetCurrentXPos();
		pSect->Set( nLeft, nTop, nLeft + nWidth, nTop + nHeight );

		pSect->OnLayout( *pSect );

		psc->SetCurrentXPos( nLeft + nWidth );

		psc->GetHTMLSection()->AddFocusObject( pSect );

		psc->AddBaseline( nBaseline );
		if( m_pForm && !psc->IsMeasuring() )
		{
			m_pForm->AddDisplayElement( pSect );
		}

	}
	else
	{
		CHTMLComboSection *pSect = (CHTMLComboSection *)psc->GetHTMLSection()->GetKeeperItemByID( m_uObjectID );
		if( !pSect )
		{
			pSect = new CHTMLComboSection( psc->GetHTMLSection(), fdef, this, psc );
			pSect->SetID( m_uObjectID );
		}

		//
		//	Figure out our sizes...
		WinHelper::CSize size;
		pSect->FigureOutSize( psc, size );

		const int nWidth = size.cx;
		const int nHeight = size.cy;

		//
		//	See if we'll fit
		int nTop = psc->GetCurrentYPos();
		if( nWidth > psc->GetRightMargin() - psc->GetCurrentXPos() )
		{
			psc->CarriageReturn( true );
			nTop = psc->GetCurrentYPos();
		}

		//
		//	Add our section into the mix
		psc->AddSection( pSect );

		int nLeft = psc->GetCurrentXPos();
		pSect->Set( nLeft, nTop, nLeft + nWidth, nTop + nHeight );

		pSect->OnLayout( *pSect );

		psc->SetCurrentXPos( nLeft + nWidth );

		psc->GetHTMLSection()->AddFocusObject( pSect );

		psc->AddBaseline( nHeight );

		if( m_pForm && !psc->IsMeasuring() )
		{
			m_pForm->AddDisplayElement( pSect );
		}
	}
#endif	//	QHTM_ALLOW_FORMS
}


bool CHTMLSelect::GetFormFields( Container::CArray< CHTMLFormField > &arrField )
{
	bool bAdded = false;
	for( UINT u = 0; u < m_arrItems.GetSize(); u++ )
	{
		CHTMLOption *pOption = m_arrItems[ u ];
		if( pOption->m_bSelected )
		{
			CHTMLFormField &f = arrField.Add();
			f.m_strName = m_strName;
			f.m_strValue = pOption->m_strValue;
			bAdded = true;
		}
	}

	return bAdded;
}
