/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTextBlock.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Text block.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "defaults.h"
#include "HTMLSectionCreator.h"
#include "HTMLButtonSection.h"
#include "HTMLEditSection.h"

CHTMLFormInput::CHTMLFormInput( const HTMLFontDef * pFont )
	: m_nType( knText )
	, m_nCols( 30 )
	, m_uRows( 30 )
	, m_uMaxLength( 0 )
	, m_bReadonly( false )
	, m_pFont( pFont )
{
}

#ifdef _DEBUG
void CHTMLFormInput::Dump() const
{
	TRACENL(_T("Form Input\n") );
	TRACENL(_T("  Type %d\n"), m_nType );
	TRACENL(_T("  Value %s\n"), (LPCTSTR)m_strValue );
	TRACENL(_T("  Size %d\n"), m_nCols );
	TRACENL(_T("  MaxLength %d\n"), m_uMaxLength );
	TRACENL(_T("  Checked %s\n"), m_bChecked ? _T("true") : _T("false") );
	TRACENL(_T("  Disabled %s\n"), m_bDisabled ? _T("true") : _T("false") );
	TRACENL(_T("  Readonly %s\n"), m_bReadonly ? _T("true") : _T("false") );
	TRACENL(_T("  Src %s\n"), (LPCTSTR)m_strImageSrc );
	
}
#endif	//	_DEBUG


struct StructFormInputType
{
	StructFormInputType( LPCTSTR pcszName, CHTMLFormInput::FormInputType nType )
		: m_strName( pcszName )
		, m_nType( nType )
		{}
	CStaticString m_strName;
	CHTMLFormInput::FormInputType m_nType;
};


static const StructFormInputType g_nInputType[] =
{
		StructFormInputType( _T("text"), CHTMLFormInput::knText )
	, StructFormInputType( _T("Password"), CHTMLFormInput::knPassword )
	, StructFormInputType( _T("Checkbox"), CHTMLFormInput::knCheckbox )
	, StructFormInputType( _T("Radio"), CHTMLFormInput::knRadio )
	, StructFormInputType( _T("Submit"), CHTMLFormInput::knSubmit )
	, StructFormInputType( _T("Reset"), CHTMLFormInput::knReset )
	, StructFormInputType( _T("File"), CHTMLFormInput::knFile )
	, StructFormInputType( _T("Hidden"), CHTMLFormInput::knHidden )
	, StructFormInputType( _T("Image"), CHTMLFormInput::knImage )
	, StructFormInputType( _T("Button"), CHTMLFormInput::knButton )
};

static MapClass< CStaticString, CHTMLFormInput::FormInputType > g_mapFormInputType;

static int InitialiseMap()
{
	if( g_mapFormInputType.GetSize() == 0 )
	{
		for( UINT n = 0; n < countof( g_nInputType ); n++ )
		{
			g_mapFormInputType.SetAt( g_nInputType[ n ].m_strName, g_nInputType[ n ].m_nType );
		}
	}
	return 1;
}


//	REVIEW - russf - Is this a performance hit on app start?
//	To ensure the colour map is initialised correctly before anything else.
//	This can make the program startup slower but I will cross that bridge later.
static int nDummy = InitialiseMap();


void CHTMLFormInput::SetFormType( const CStaticString &strType )
{
	FormInputType *pnType = g_mapFormInputType.Lookup( strType );
	if( pnType )
	{
		m_nType = *pnType;
	}
#ifdef _DEBUG
	else
	{
		StringClass str( strType, strType.GetLength() );
		TRACE( _T("Unknown form input type %s\n"), (LPCTSTR)str );
	}
#endif //	_DEBUG
}



void CHTMLFormInput::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	switch( m_nType )
	{
	case knText:
		OnAddEdit( psc );
		break;

	case knPassword:
		OnAddEdit( psc );
		break;

	case knCheckbox:
		OnAddButton( psc );
		break;

	case knRadio:
		OnAddButton( psc );
		break;

	case knTextArea:
		OnAddEdit( psc );
		break;

	case knSubmit:
		OnAddButton( psc );
		break;

	case knReset:
		OnAddButton( psc );
		break;

	case knFile:
		//	Not supported
		//TRACE( _T("CHTMLFormInput::knFile not supported\n") );
		break;

	case knHidden:
		//	Do nothing!
		break;

	case knImage:
		//	Should we add a standard image?
		//TRACE( _T("CHTMLFormInput::knImage not supported\n") );
		break;

	case knButton:
		OnAddButton( psc );
		break;
	}

}


void CHTMLFormInput::OnAddButton( class CHTMLSectionCreator *psc )
{
#ifdef QHTM_ALLOW_FORMS
	GS::FontDef fdef( m_pFont->m_strFont, m_pFont->m_nSize, m_pFont->m_nWeight, m_pFont->m_bItalic, m_pFont->m_bUnderline, m_pFont->m_bStrike, psc->GetCurrentCharSet(), m_pFont->m_bFixedPitchFont );
	fdef.m_nSizePixels = GetFontSizeAsPixels( psc->GetDC().GetSafeHdc(), m_pFont->m_nSize, psc->GetZoomLevel() );

	//const int nHorizontalGap = WinHelper::MulDiv( g_defaults.m_nHorizontalFormControlGap, GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX), 1000); 
	
	CHTMLButtonSection *pSect = (CHTMLButtonSection *)psc->GetHTMLSection()->GetKeeperItemByID( m_uObjectID );
	if( !pSect )
	{
		pSect = new CHTMLButtonSection( psc->GetHTMLSection(), fdef, this, psc );
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
#endif	//	QHTM_ALLOW_FORMS
}



void CHTMLFormInput::OnAddEdit( class CHTMLSectionCreator *psc )
{
#ifdef QHTM_ALLOW_FORMS
	GS::FontDef fdef( m_pFont->m_strFont, m_pFont->m_nSize, m_pFont->m_nWeight, m_pFont->m_bItalic, m_pFont->m_bUnderline, m_pFont->m_bStrike, psc->GetCurrentCharSet(), m_pFont->m_bFixedPitchFont );
	fdef.m_nSizePixels = GetFontSizeAsPixels( psc->GetDC().GetSafeHdc(), m_pFont->m_nSize, psc->GetZoomLevel() );

	CHTMLEditSection *pSect = (CHTMLEditSection*)psc->GetHTMLSection()->GetKeeperItemByID( m_uObjectID );
	if( !pSect )
	{
		pSect = new CHTMLEditSection( psc->GetHTMLSection(), fdef, this, psc );
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
#endif	//	QHTM_ALLOW_FORMS
}


bool CHTMLFormInput::GetFormFields( Container::CArray< CHTMLFormField > &arrField )
{
	if( !m_bDisabled )
	{
		switch( m_nType )
		{
		case knTextArea:
		case knText:
		case knPassword:
			{
				CHTMLFormField &f = arrField.Add();
				f.m_strValue = m_strValue;
				f.m_strName = m_strName;
			}
			return true;

		case knCheckbox:
		case knRadio:
		case knButton:
			if( m_bChecked )
			{
				CHTMLFormField &f = arrField.Add();
				f.m_strValue = m_strValue;
				f.m_strName = m_strName;
				return true;
			}
			break;

		case knSubmit:
			break;

		case knReset:
			break;

		case knFile:
			break;

		case knHidden:
			{
				CHTMLFormField &f = arrField.Add();
				f.m_strValue = m_strValue;
				f.m_strName = m_strName;
			}
			return true;

		case knImage:
			break;

		}
	}
	return false;
}