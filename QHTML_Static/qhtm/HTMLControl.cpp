/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLControl.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Text block.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "HTMLControlSection.h"
#ifdef QHTM_WINDOWS_CONTROL

extern UINT ParseWindowStyle( const CTextABC &str );

static int GetNumberParameter( const CTextABC &strParam, CHTMLControl::SizeType &nType )
{
	nType = CHTMLControl::knPixels;

  int nTotal = 0;
	if( strParam.GetLength() )
	{
		LPCTSTR p = strParam.GetData();
		LPCTSTR pEnd = p + strParam.GetLength();

		while( p < pEnd && isspace( *p ))
		{
			p++;
		}
  
		while( p < pEnd && isdigit( *p) )
		{
			nTotal = 10 * nTotal + (*p - _T('0') );     /* accumulate digit */
			p++;
		}

		if( p < pEnd )
		{
			if( *p == _T('%') )
			{
				nType = CHTMLControl::knPercent;
			}
			else if( _tcsnicmp( p, _T("dlu"), 3 ) == 0 )
			{
				nType = CHTMLControl::knDLUs;
			}
			else if( _tcsnicmp( p, _T("pt"), 2 ) == 0 )
			{
				nType = CHTMLControl::knPoints;
			}
		}
	}
	else
	{
		nTotal = 0;
	}
	
	return nTotal;
}


CHTMLControl::CHTMLControl( const HTMLFontDef * pFont )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_pFont( pFont )
	, m_uWidth( 0 )
	, m_uHeight( 0 )
	, m_uStyle( 0 )
	, m_uStyleEx( 0 )
	, m_uID( 0 )
{
}

#ifdef _DEBUG
void CHTMLControl::Dump() const
{
	TRACENL(_T("HTMLControl\n") );
}
#endif	//	_DEBUG


void CHTMLControl::SetStyle( const CTextABC &str )
{
	m_uStyle = ParseWindowStyle( str );
}


void CHTMLControl::SetStyleEx( const CTextABC &str )
{
	m_uStyleEx = ParseWindowStyle( str );
}


void CHTMLControl::SetID( const CTextABC &str )
{
	LPTSTR endptr;
	m_uID = _tcstol( str, &endptr, 10 );
}


void CHTMLControl::SetWidth( const CTextABC &str )
{
	m_uWidth = GetNumberParameter( str, m_WidthType );
}


void CHTMLControl::SetHeight( const CTextABC &str )
{
	m_uHeight = GetNumberParameter( str, m_HeightType );
}


void CHTMLControl::SetControlClass( const CTextABC &str )
{
	m_strControlClass.Set( str, str.GetLength() );
}


void CHTMLControl::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	CHTMLControlSection *pSect = (CHTMLControlSection*)psc->GetHTMLSection()->GetKeeperItemByID( m_uID );
	if( !pSect )
	{
		GS::FontDef fdef( m_pFont->m_strFont, m_pFont->m_nSize, m_pFont->m_nWeight, m_pFont->m_bItalic, m_pFont->m_bUnderline, m_pFont->m_bStrike, psc->GetCurrentCharSet(), m_pFont->m_bFixedPitchFont );
		fdef.m_nSizePixels = GetFontSizeAsPixels( psc->GetDC().GetSafeHdc(), m_pFont->m_nSize, psc->GetZoomLevel() );

		pSect = new CHTMLControlSection( psc->GetHTMLSection(), fdef, m_strControlClass, m_uStyle, m_uStyleEx, m_uWidth, m_uHeight );
		pSect->SetID( m_uID );
	}
	pSect->SetFont( psc );

	UINT uWidth, uHeight;
	uWidth = m_uWidth;
	uHeight = m_uHeight;
	UINT uMaxWidth = psc->GetCurrentWidth();

	switch( m_WidthType )
	{
	case knPoints:
		{
			const int nX = GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSY );
			uWidth = WinHelper::MulDiv( m_uWidth, nX, 72 );
		}
		break;

	case knDLUs:
		uWidth = m_uWidth * LOWORD( GetDialogBaseUnits() );
		break;

	case knPercent:
		uWidth = min( UINT( (float( uMaxWidth ) / 100 ) * abs( uWidth ) ), uMaxWidth );
		break;
	}

	switch( m_HeightType )
	{
	case knPoints:
		{
			const int nX = GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX );
			uHeight = WinHelper::MulDiv( m_uHeight, nX, 72 );
		}
		break;


	case knDLUs:
		{
			UINT u = m_uHeight * (HIWORD( GetDialogBaseUnits() )- 2) / 8;
			uHeight = u;
		}
		break;

	case knPercent:
		uHeight = m_uHeight;		//	Can't have a percentage height!
		break;
	}

	if( !uHeight )
	{
		uHeight = abs( GetFontSizeAsPixels( psc->GetDC().GetSafeHdc(), m_pFont->m_nSize, psc->GetZoomLevel() ) );
	}

	//
	//	See if we'll fit
	int nTop = psc->GetCurrentYPos();
	if( uWidth > (UINT)(psc->GetRightMargin() - psc->GetCurrentXPos()) )
	{
		psc->CarriageReturn( true );
		nTop = psc->GetCurrentYPos();
	}

	//
	//	Add our section into the mix
	psc->AddSection( pSect );

	int nLeft = psc->GetCurrentXPos();
	pSect->Set( nLeft, nTop, nLeft + uWidth, nTop + uHeight );

	pSect->OnLayout( *pSect );

	psc->SetCurrentXPos( nLeft + uWidth );

	psc->GetHTMLSection()->AddFocusObject( pSect );

	psc->AddBaseline( -1 );
}

#endif	//	QHTM_WINDOWS_CONTROL