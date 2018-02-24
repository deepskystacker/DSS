/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTextBlock.cpp
Owner:	russf@gipsysoft.com
Purpose:	HTML Text block.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "HTMLTextSection.h"

#define QHTM_NBSP	0xa0

#ifndef _tcsinc
	#define _tcsinc( p )	(p+1)
#endif	//	_tcsinc

CHTMLTextBlock::CHTMLTextBlock( const CTextABC &strText, const HTMLFontDef * pFont, CColor crFore, CColor crBack, bool bPreformatted )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_strText( strText, strText.GetLength() )
	, m_pFont( pFont )
	, m_crFore( crFore )
	, m_bPreformatted( bPreformatted )
	, m_crBack( crBack )
{
}

#ifdef _DEBUG
void CHTMLTextBlock::Dump() const
{
	TRACENL(_T("TextBlock\n") );
	TRACENL(_T("\tText(%s)\n"), (LPCTSTR)m_strText );
	TRACENL(_T("\t crFore(%d)\n"), m_crFore );
	TRACENL(_T("\t bPreformatted(%d)\n"), m_bPreformatted );	
}
#endif	//	_DEBUG

bool CHTMLTextBlock::IsEmpty() const
{
	return m_strText.GetLength() == 1 && *(m_strText.GetData()) == _T(' ');
}


void CHTMLTextBlock::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	
	const GS::FontDef *fdef = psc->GetDrawingFont( *m_pFont );

	psc->GetDC().SelectFont( *fdef );

	const int nCurrentFontHeight = psc->GetDC().GetCurrentFontBaseline();

	if( m_bPreformatted )
	{
		LPCTSTR pcszTextLocal = m_strText;
		LPCTSTR pcszPrevious = pcszTextLocal;

		while( 1 )
		{
			if( *pcszTextLocal == '\r' || *pcszTextLocal == '\n' || *pcszTextLocal == '\000' )
			{
				const size_t uLength = pcszTextLocal - pcszPrevious;
				const WinHelper::CSize size( psc->GetDC().GetTextExtent( pcszPrevious, uLength ), nCurrentFontHeight );

				CHTMLTextSection *pText = new CHTMLTextSection( psc->GetHTMLSection(), pcszPrevious, uLength, fdef, m_crFore, m_crBack );
				psc->AddSection( pText );
				
				pText->Set( psc->GetCurrentXPos(), psc->GetCurrentYPos(), psc->GetCurrentXPos() + size.cx, psc->GetCurrentYPos() + size.cy );
				psc->SetCurrentXPos( psc->GetCurrentXPos() + size.cx );
				psc->AddBaseline( psc->GetDC().GetCurrentFontBaseline() );

				//	

				if( *pcszTextLocal == '\000' )
				{
					break;
				}

				if( *pcszTextLocal == '\r' && *( pcszTextLocal + 1 ) == '\n' )
				{
					*pcszTextLocal++;
					psc->CarriageReturn( true );
				}
				pcszPrevious = pcszTextLocal + 1;
			}
			*pcszTextLocal++;
		}
	}
	else
	{
		
		//
		//	Set up our baseline, in case it's different...
		int nBaseLine = nCurrentFontHeight;
		if( m_pFont->m_nSup )
		{
			nBaseLine = nCurrentFontHeight + m_pFont->m_nSup * nCurrentFontHeight / 3;
		}
		else if( m_pFont->m_nSub )
		{
			nBaseLine = nCurrentFontHeight - m_pFont->m_nSub * nCurrentFontHeight / 3;
		}

		LPCTSTR pcszTextLocal = m_strText;
		LPCTSTR pcszPrevious = pcszTextLocal;


		//
		//	Basically we loop over the string picking out the words and adding the individual words as text sections.
		//	We include the spaces in the text so that it's a simple loop and so links get underline across the whole text including spaces.
		//
		//	We have one bugbear. nbsp (non-breaking spaces) are encoded as character #160 and we need to remove these and replace them with spaces.

		bool bHasNBSP = false;
		while( 1 )
		{
			if( _istspace( static_cast< unsigned char >( *pcszTextLocal ) ) || *pcszTextLocal == '\000' )
			{
				size_t nStringLength = pcszTextLocal - pcszPrevious;
				if( nStringLength )
				{
					int nWidth = psc->GetDC().GetTextExtent( pcszPrevious, nStringLength );
					if( nWidth > psc->GetRightMargin() - psc->GetCurrentXPos() )
					{
						psc->CarriageReturn( true );

						//
						//	If we did a carriage return then we need to NOT have the leading space that got us here in the first place
						while( _istspace( static_cast< unsigned char >( *pcszPrevious ) ) )
						{
							pcszPrevious = _tcsinc( pcszPrevious );
						}
						nStringLength = max( pcszTextLocal - pcszPrevious, 0 );
						ASSERT( nStringLength >= 0 );
						nWidth = psc->GetDC().GetTextExtent( pcszPrevious, nStringLength );

					}

					CHTMLTextSection *pText = NULL;

					//
					//	If our string DID have a nonbreaking space then we have to remove it and create our text object as appropriate
					if( bHasNBSP )
					{
						StringClass str( pcszPrevious, nStringLength );
						str.Replace( QHTM_NBSP, _T(' ') );
						pText = new CHTMLTextSection( psc->GetHTMLSection(), str, str.GetLength(), fdef, m_crFore, m_crBack );
					}
					else
					{
						pText = new CHTMLTextSection( psc->GetHTMLSection(), pcszPrevious, nStringLength, fdef, m_crFore, m_crBack );
					}

					if( m_strTip.GetLength() )
					{
						pText->SetTipText( m_strTip );
					}

					if( m_strActivationTarget.GetLength() )
					{
						pText->SetActivationTarget( m_strActivationTarget );
					}
					
					psc->AddSection( pText );
					pText->Set( psc->GetCurrentXPos(), psc->GetCurrentYPos(), psc->GetCurrentXPos() + nWidth, psc->GetCurrentYPos() + psc->GetDC().GetCurrentFontHeight() );
					psc->SetCurrentXPos( pText->right );

					psc->AddBaseline( nBaseLine );
				}

				if( *pcszTextLocal == '\000' )
				{
					break;
				}
				pcszPrevious = pcszTextLocal;

				pcszTextLocal = _tcsinc( pcszTextLocal );
			}
			else
			{
				if( (unsigned char)( *pcszTextLocal ) == QHTM_NBSP )
				{
					bHasNBSP = true;
				}
				pcszTextLocal = _tcsinc( pcszTextLocal );
			}
		}
	}
}
