/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLList.cpp
Owner:	rich@woodbridgeinternalmed.com
Purpose:	A List, and List Entries
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "htmlparse.h"
#include "HTMLSectionCreator.h"
#include "HTMLTextSection.h"
#include "defaults.h"

static UINT Decimal( LPTSTR pszBuffer, UINT uIndex )
{
	static LPCTSTR knBulletFormat = _T("%u.");
	return wsprintf( pszBuffer, knBulletFormat, uIndex );
}


static TCHAR g_arrUpperAlphaChars[ 27 ]  = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
static TCHAR g_arrLowerAlphaChars[ 27 ]  = _T("abcdefghijklmnopqrstuvwxyz");

static int ConvertToAlpha( UINT uIndex, LPTSTR pszBuffer, LPCTSTR pcszChars, UINT uRadix )
{
  TCHAR szBuffer[ 32 ];
	static const int nBufferSize = countof( szBuffer );
	szBuffer[ 31 ] ='\000';

  UINT u = countof( szBuffer ) - 1;
  if( uIndex <= 0 )
	{
    uIndex = 1;
  }

  do {
    uIndex--;
    UINT cur = uIndex % uRadix;
    szBuffer[ --u ] = pcszChars[cur];
    uIndex /= uRadix;
  } while ( uIndex > 0);

	_tcscpy( pszBuffer, szBuffer + u );
	return nBufferSize - u - 1;
}


static UINT AlphaUpper( LPTSTR pszBuffer, UINT uIndex )
{
	return ConvertToAlpha( uIndex, pszBuffer, g_arrUpperAlphaChars, countof( g_arrUpperAlphaChars ) - 1 );
}


static UINT AlphaLower( LPTSTR pszBuffer, UINT uIndex )
{
	return ConvertToAlpha( uIndex, pszBuffer, g_arrLowerAlphaChars, countof( g_arrUpperAlphaChars ) - 1);
}

static UINT g_arrRomanValue[]= {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
static LPTSTR g_arrRomanUpper[]= { _T("M"), _T("CM"), _T("D"), _T("CD"), _T("C"), _T("XC"), _T("L"), _T("XL"), _T("X"), _T("IX"), _T("V"), _T("IV"), _T("I") };
static LPTSTR g_arrRomanLower[]= { _T("m"), _T("cm"), _T("d"), _T("cd"), _T("c"), _T("xc"), _T("l"), _T("xl"), _T("x"), _T("ix"), _T("v"), _T("iv"), _T("i") };

static UINT ConvertToRoman( LPTSTR pszBuffer, UINT uIndex, LPTSTR *arr )
{
	pszBuffer[0] = '\000';
	int pos=0;
	for( int i = 0; i < countof( g_arrRomanValue ) ; i++ )
	{
		while( g_arrRomanValue[i] <= uIndex )
		{
			_tcscat( pszBuffer + pos, arr[ i ] );
			pos++;

			uIndex -= g_arrRomanValue[i];
		}
	}
	pszBuffer[ pos ]= '\0';
	return pos;
}


static UINT RomanUpper( LPTSTR pszBuffer, UINT uIndex )
{
	return ConvertToRoman( pszBuffer, uIndex, g_arrRomanUpper );
}

static UINT RomanLower( LPTSTR pszBuffer, UINT uIndex )
{
	return ConvertToRoman( pszBuffer, uIndex, g_arrRomanLower );
}


static UINT Bullet( LPTSTR pszBuffer, UINT )
{
	static TCHAR knBulletText[] = _T("\xB7");
	_tcscpy( pszBuffer, knBulletText );
	return countof( knBulletText ) - 1;
}

static UINT Circle( LPTSTR pszBuffer, UINT )
{
	static TCHAR knBulletText[] = _T("\xB0");
	_tcscpy( pszBuffer, knBulletText );
	return countof( knBulletText ) - 1;
}


static UINT Square( LPTSTR pszBuffer, UINT )
{
	static TCHAR knBulletText[] = _T("\xE0");
	_tcscpy( pszBuffer, knBulletText );
	return countof( knBulletText ) - 1;
}


CHTMLList::CHTMLList(	bool bOrdered )
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_bOrdered( bOrdered )
	, m_bCompact( false )
{
	if( m_bOrdered )
	{
		m_funcGetListItemText = Decimal;
	}
	else
	{
		m_funcGetListItemText = Bullet;
	}
}


CHTMLList::~CHTMLList()
{
	for( UINT nIndex = 0; nIndex < m_arrItems.GetSize(); nIndex++ )
	{
		delete m_arrItems[ nIndex ];
	}
	m_arrItems.RemoveAll();
}


void CHTMLList::AddItem( CHTMLListItem *pItem )
{
	m_arrItems.Add( pItem );
}


UINT CHTMLList::GetItemText( LPTSTR pszBuffer, UINT uIndex ) const
{
	return m_funcGetListItemText( pszBuffer, uIndex );
}


void CHTMLList::SetType( const CStaticString &strType )
{
	if( strType.GetLength() == 1 )
	{
		switch( *strType.GetData() )
		{
		case 'a':
			m_funcGetListItemText = AlphaLower;
			break;

		case 'A':
			m_funcGetListItemText = AlphaUpper;
			break;

		case 'i':
			m_funcGetListItemText = RomanLower;
			break;

		case 'I':
			m_funcGetListItemText = RomanUpper;
			break;
		}
	}
	else if( m_bOrdered == false )
	{
		if( !_tcsnicmp( strType, _T("circle"), strType.GetLength() ) )
		{
			m_funcGetListItemText = Circle;
		}
		else if( !_tcsnicmp( strType, _T("disc"), strType.GetLength() ) )
		{
			m_funcGetListItemText = Bullet;
		}
		else if( !_tcsnicmp( strType, _T("square"), strType.GetLength() ) )
		{
			m_funcGetListItemText = Square;
		}
	}
}

#ifdef _DEBUG
void CHTMLList::Dump() const
{
	const size_t size = m_arrItems.GetSize();

	TRACENL( _T("List----------------\n") );
	TRACENL( _T(" Size (%d)\n"), size );
	TRACENL( _T(" Ordered (%s)\n"), (m_bOrdered ? _T("true") : _T("false") ));
	TRACENL( _T(" Compact (%s)\n"), (m_bCompact ? _T("true") : _T("false") ));

	for( UINT nIndex = 0; nIndex < size; nIndex++ )
	{
		TRACENL( _T(" Item %d\n"), nIndex );
		m_arrItems[ nIndex ]->Dump();
	}
}
#endif	//	_DEBUG


void CHTMLList::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	/*
		A List is similar to a table, in that each list item is a
		document in itself. Lists are laid out by creating the
		bullet, which is right justified along the left margin, 
		and has width knIndentSize. The contents depend on whether
		the list is ordered, and the index of the item.
		The list item contents use the right edge if the bullet space
		as the left margin. That's it! Note that current font properties
		apply to the bullet.
		Only color applies to bullets in unordered lists.
	*/
	int nFontSize = 2;
	if( m_arrItems.GetSize() )
	{
		CHTMLListItem* pItem = m_arrItems[0];
		if( !pItem->m_bBullet )
		{
			nFontSize = pItem->m_nSize;
		}
	}
	
	int nBulletSpace = WinHelper::MulDiv( m_bCompact ? g_defaults.m_nIndentSpaceSize / 2 : g_defaults.m_nIndentSpaceSize, GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX), 1000); 
	int nIndex = 1;	// Index, used for ordered lists

	// Iterate the list items, creating the bullet and the subdocument.
	const size_t nItems = m_arrItems.GetSize();

	psc->SetNextYPos( psc->GetNextYPos() + psc->GetDC().GetCurrentFontHeight() );

	TCHAR szBuffer[12];
	UINT uLength = 1;

	//
	//	Measure the indent size...
	size_t i;
	for ( i = 0; i < nItems; ++i)
	{
		CHTMLListItem* pItem = m_arrItems[i];

		psc->NewParagraph( 0,0,CStyle::algLeft );

		// Do we need to create a bullet?
		if( pItem->m_bBullet )
		{
			// Create the bullet...
			LPCTSTR pcszFont;
			//int nFontSize;
			bool bBold, bItalic, bUnderline, bStrikeOut;
			LPCTSTR pcszText = 0;

			nIndex = pItem->GetValue( nIndex );
			uLength = GetItemText( szBuffer, nIndex );

			pcszText = szBuffer;
			if( m_bOrdered )
			{
				// Set text parameters
				pcszFont = pItem->m_strFont;
				nFontSize = pItem->m_nSize;
				bBold = pItem->m_bBold;
				bItalic = pItem->m_bItalic;
				bUnderline = pItem->m_bUnderline;
				bStrikeOut = pItem->m_bStrikeout;
			}
			else
			{
				// Set text parameters
				pcszFont = _T("Symbol");
				bBold = false;
				bItalic = false;
				bUnderline = false;
				bStrikeOut = false;
			}

			HTMLFontDef htmlfdef( pcszFont, nFontSize, bBold, bItalic, bUnderline, bStrikeOut, 0, 0, false );

			const GS::FontDef *pfdef = psc->GetDrawingFont( htmlfdef );

			//
			// Position the object...
			psc->GetDC().SelectFont( *pfdef );

			const int nTextWidth = psc->GetDC().GetTextExtent(pcszText, uLength );
			if( nTextWidth > nBulletSpace )
			{
				nBulletSpace = nTextWidth;
			}
		}
	}

	const int nBulletWidth = WinHelper::MulDiv( m_bCompact ? g_defaults.m_nIndentSize / 4 : g_defaults.m_nIndentSize / 2, GetDeviceCaps( psc->GetDC().GetSafeHdc(), LOGPIXELSX), 1000); 

	for( i = 0; i < nItems; ++i )
	{
		int nBulletHeight = 0;
		CHTMLListItem* pItem = m_arrItems[i];

		psc->NewParagraph( 0,0,CStyle::algLeft );

		// Do we need to create a bullet?
		if( pItem->m_bBullet )
		{
			// Create the bullet...
			LPCTSTR pcszFont;
			//int nFontSize;
			bool bBold, bItalic, bUnderline, bStrikeOut;
			LPCTSTR pcszText = 0;

			nIndex = pItem->GetValue( nIndex );
			uLength = GetItemText( szBuffer, nIndex );

			pcszText = szBuffer;

			if( m_bOrdered )
			{
				// Set text parameters
				pcszFont = pItem->m_strFont;
				nFontSize = pItem->m_nSize;
				bBold = pItem->m_bBold;
				bItalic = pItem->m_bItalic;
				bUnderline = pItem->m_bUnderline;
				bStrikeOut = pItem->m_bStrikeout;
			}
			else
			{
				// Set text parameters
				pcszFont = _T("Symbol");
				//nFontSize = nBulletFontSize;
				bBold = true;
				bItalic = false;
				bUnderline = false;
				bStrikeOut = false;
			}

			HTMLFontDef htmlfdef( pcszFont, nFontSize, bBold, bItalic, bUnderline, bStrikeOut, 0, 0, false );

			const GS::FontDef *pfdef = psc->GetDrawingFont( htmlfdef );
			CColor crBack;
			CHTMLTextSection* pText = new CHTMLTextSection( psc->GetHTMLSection(), pcszText, uLength, pfdef, pItem->m_crFore, crBack );

			const size_t nLabelID = psc->GetCurrentShapeID();

			psc->AddSection( pText );

			//
			// Position the object...
			psc->GetDC().SelectFont( *pfdef );

			const int nTextWidth = psc->GetDC().GetTextExtent(pcszText, uLength );
			nBulletHeight = psc->GetCurrentYPos() + psc->GetDC().GetCurrentFontHeight();
			const int right = psc->GetCurrentXPos() + nBulletSpace;
			const int left = right - nTextWidth;
			pText->Set( left, psc->GetCurrentYPos(), right, nBulletHeight );
			// Increment the index
			nIndex++;

			// Now, create the sub-document
			CHTMLSectionCreator htCreate( psc->GetHTMLSection(), psc->GetDC(), psc->GetCurrentYPos(), right + nBulletWidth, psc->GetRightMargin(), psc->GetBackgroundColor(), false, psc->GetZoomLevel(), psc->GetCurrentLink(), psc );
			htCreate.AddDocument( pItem );
			const WinHelper::CSize size( htCreate.GetSize() );

			// Adjust y-pos
			psc->SetCurrentYPos( max(size.cy, nBulletHeight) + 1 );

			if( nLabelID + 1 > psc->GetCurrentShapeID() )
			{
				CSectionABC *pSect = psc->GetHTMLSection()->GetSectionAt( nLabelID + 1 );
				if( pSect )
				{
					if( pSect->Height() > pText->Height() )
					{
						const int nDiff = (pSect->Height() - pText->Height() ) / 2;
						pText->Offset( 0, nDiff );
					}
					else
					{
						const int nDiff = (pText->Height() - pSect->Height() ) / 2;
						pText->Offset( 0, -nDiff );
					}
				}
			}
		}
		else
		{
			// Now, create the sub-document
			CHTMLSectionCreator htCreate( psc->GetHTMLSection(), psc->GetDC(), psc->GetCurrentYPos(), psc->GetCurrentXPos() + nBulletWidth + nBulletSpace, psc->GetRightMargin(), psc->GetBackgroundColor(), false, psc->GetZoomLevel(), psc->GetCurrentLink(), psc );
			htCreate.AddDocument( pItem );
			const WinHelper::CSize size( htCreate.GetSize() );

			// Adjust y-pos
			psc->SetCurrentYPos( max(size.cy, nBulletHeight) + 1 );
		}
	}
}