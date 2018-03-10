/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	htmlparse.cpp
Owner:	russf@gipsysoft.com
Purpose:	Main HTML parser
					The CHTMLParse::Parse() function generates a document, the document
					contains all of the elements of the HTML page but broken into
					their parts.
					The document will then be used to create the display page.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <reuse/WinHelper.h>
#include "AquireImage.h"
#include "defaults.h"
#include "smallstringhash.h"
#include "HTMLParse.h"
#include "QHTMImage.h"
#include "FontInfo.h"
#include <datasource/ResourceDataSource.h>
#include "ResourceLoader.h"
#include "ParseStyles.h"


extern LPTSTR stristr( LPTSTR pszSource, LPCTSTR pcszSearch );
extern BYTE DecodeCharset( const CStaticString &strCharSet );

static CStyle::Align knDefaultImageAlignment = CStyle::algBottom;
static CStyle::Align knDefaultHRAlignment = CStyle::algLeft;
static CStyle::Align knDefaultParagraphAlignment = CStyle::algLeft;
// richg - 19990224 - Default table alignment changed form algLeft to algTop
static CStyle::Align knDefaultTableAlignment = CStyle::algMiddle;
// richg - 19990224 - Table Cells have their own default
static CStyle::Align knDefaultTableCellAlignment = CStyle::algLeft;

TCHAR g_cCarriageReturn = _T('\r');
static const CStaticString g_strTabSpaces( _T("        ") );

CHTMLParse::CHTMLParse( LPCTSTR pcszStream, size_t uLength, HINSTANCE hInstLoadedFrom, LPCTSTR pcszFilePath, CDefaults *pDefaults )
	: CHTMLParseBase( pcszStream, uLength )
	, m_pCurrentStyle( NULL )
	, m_pDocument( NULL )
	, m_pMasterDocument( NULL )
	, m_pLastAnchor( NULL )
	, m_pTextArea( NULL )
	, m_pCurrentSelect( NULL )
	, m_hInstLoadedFrom( hInstLoadedFrom )
	, m_pcszFilePath( pcszFilePath )
	, m_pDefaults( pDefaults )
	, m_bInStyleSheet( false )
{
	ASSERT( m_pDefaults );
}


CHTMLParse::~CHTMLParse()
{
	CleanupParse();
}


CHTMLDocument * CHTMLParse::Parse( CHTMLDocument *pdocOwner )
//
//	returns either a fully created document ready to create the display from or
//	 NULL in the event of failure.
{
	CleanupParse();

	//
	//	Create the first CStyle
	m_pCurrentStyle = new CStyle;
	m_pCurrentStyle->nAlignment = CStyle::algLeft;
	m_pCurrentStyle->m_crFore = m_pDefaults->m_crDefaultForeColour;
	_tcscpy( m_pCurrentStyle->szFaceName, m_pDefaults->m_strFontName );
	m_pCurrentStyle->nSize = m_pDefaults->m_nFontSize;
	m_stkStyle.Push( m_pCurrentStyle );

	//
	//	Create the main document and a paragraph to add to it.
	if( pdocOwner )
	{
		m_pMasterDocument = m_pDocument = pdocOwner;
	}
	else
	{
		m_pMasterDocument = m_pDocument = new CHTMLDocument( m_pDefaults );
		CreateNewParagraph( m_pDefaults->m_nParagraphLinesAbove, m_pDefaults->m_nParagraphLinesBelow, knDefaultParagraphAlignment );
	}

	if( !ParseBase() )
	{
		if( m_pMasterDocument && !pdocOwner )
		{
			delete m_pMasterDocument;
			m_pDocument = NULL;
			m_pMasterDocument = NULL;
		}
	}

	if( pdocOwner && !m_pMasterDocument )
	{
		return NULL;
	}

	//
	//	Uncomment this to see the document structure in the output window!
	//	This is very handy for debugging additions to the parser.
	//m_pMasterDocument->Dump();

	return m_pMasterDocument;
}


void CHTMLParse::CleanupParse()
//
//	Cleanup anything left over from our previous parsing
{
	while( m_stkStyle.GetSize() )
		delete m_stkStyle.Pop();

	while( m_stkDocument.GetSize() )
		(void)m_stkDocument.Pop();


	//	richg - 19990227 - Clean up the table stack as well
	while( m_stkTable.GetSize() )
		(void)m_stkTable.Pop();

	while( m_stkInTableCell.GetSize() )
		(void)m_stkInTableCell.Pop();

	// richg - 19990621 - Clean up list stack
	while( m_stkList.GetSize() )
		(void)m_stkList.Pop();

	while( m_stkDefinitionList.GetSize() )
		(void)m_stkDefinitionList.Pop();
	
	m_pLastAnchor = NULL;
}


void CHTMLParse::OnGotText( TCHAR ch )
//
//	Callback when some text has been interrupted with a tag or end tag.
{
	if( ch == _T('\t') )
	{
		m_strToken.Add( g_strTabSpaces, g_strTabSpaces.GetLength() );
	}
	else
	{
		m_strToken.Add( ch );
	}
}


void CHTMLParse::OnEndDoc()
{
	CreateNewTextObject();
}


void CHTMLParse::OnGotTag( const Token token, const CParameters &pList )
{
	switch( token )
	{
	case tokFont:
		OnGotFont( pList );
		break;

	case tokBold:
		CreateNewTextObject();
		CreateNewStyle();
		m_pCurrentStyle->bBold = true;
		break;

	case tokUnderline:
		CreateNewTextObject();
		CreateNewStyle();
		m_pCurrentStyle->bUnderline = true;
		break;

	case tokVAR:
	case tokDFN:
	case tokItalic:
		CreateNewTextObject();
		CreateNewStyle();
		m_pCurrentStyle->bItalic = true;
		break;

	case tokStrikeout:
		CreateNewTextObject();
		CreateNewStyle();
		m_pCurrentStyle->bStrikeThrough = true;
		break;

	case tokImage:
		OnGotImage( pList );
		break;

	case tokSub:
		CreateNewTextObject();
		CreateNewStyle();
		if( m_pCurrentStyle->nSize > 1 )
		{
			m_pCurrentStyle->nSize--;
		}
		else if( m_pCurrentStyle->nSize <= 3 )
		{
			m_pCurrentStyle->nSize -= m_pCurrentStyle->nSize / 3 ;
		}


		m_pCurrentStyle->m_nSub++;
		break;

	case tokSup:
		CreateNewTextObject();
		CreateNewStyle();

		if( m_pCurrentStyle->nSize > 1 )
		{
			m_pCurrentStyle->nSize--;
		}
		else if( m_pCurrentStyle->nSize <= 3 )
		{
			m_pCurrentStyle->nSize -= m_pCurrentStyle->nSize / 3;
		}
		m_pCurrentStyle->m_nSup++;
		break;

	case tokAnchor:
		OnGotAnchor( pList );
		break;

	case tokPre:
		CreateNewTextObject();
		CreateNewStyle();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		_tcscpy( m_pCurrentStyle->szFaceName, m_pDefaults->m_strDefaultPreFontName );
		break;

	case tokBody:
		OnGotBody( pList );
		break;

	case tokTableDef:
		OnGotTableCell( pList );
		break;

	case tokTableHeading:
		OnGotTableCell( pList );
		CreateNewStyle();
		m_pCurrentStyle->nAlignment = CStyle::algCentre;
		m_pCurrentStyle->m_nCentered++;
		break;

	case tokTableHeadingRow:
		OnGotTableRow( pList );
		break;

	case tokTableRow:
		OnGotTableRow( pList );
		break;

	case tokTable:
		CreateNewStyle();
		OnGotTable( pList );
		break;

	case tokHorizontalRule:
		OnGotHR( pList );
		break;

	case tokCenter:
		CreateNewStyle();
		m_pCurrentStyle->nAlignment = CStyle::algCentre;
		m_pCurrentStyle->m_nCentered++;
		OnGotParagraph( pList );
		break;

	case tokDiv:
		OnGotParagraph( pList );
		break;

	case tokParagraph:
		OnGotParagraph( pList );
		break;

	case tokBreak:
		OnGotBreak( pList );
		break;

	case tokH1:
	case tokH2:
	case tokH3:
	case tokH4:
	case tokH5:
	case tokH6:
		OnGotHeading( token, pList );
		break;

	case tokOrderedList:
		OnGotOrderedList( pList );
		break;

	case tokUnorderedList:
		OnGotUnorderedList( pList );
		break;

	case tokListItem:
		OnGotListItem( pList );
		break;

	case tokDefinitionList:
		OnGotDefinitionList();
		break;
		
	case tokDefinitionTerm:
		OnGotDefinitionItem( true );
		break;

	case tokDefinitionDescription:
		OnGotDefinitionItem( false );
		break;

	case tokAddress:
		OnGotAddress( pList );
		break;

	case tokBlockQuote:
		OnGotBlockQuote( pList );
		break;

	case tokKBD:
	case tokSAMP:
	case tokTeletype:
	case tokCode:
		CreateNewTextObject();
		CreateNewStyle();
		m_pCurrentStyle->m_bFixedPitchFont = true;
		_tcscpy( m_pCurrentStyle->szFaceName, m_pDefaults->m_strDefaultPreFontName );
		break;

	case tokBIG:
		CreateNewTextObject();
		CreateNewStyle();
		if( m_pCurrentStyle->nSize > 0 )
		{
			m_pCurrentStyle->nSize = min( 7, m_pCurrentStyle->nSize + 1 );
		}
		else
		{
			m_pCurrentStyle->nSize-=2;
		}
		break;

	case tokSMALL:
		CreateNewTextObject();
		CreateNewStyle();
		if( m_pCurrentStyle->nSize > 0 )
		{
			m_pCurrentStyle->nSize = max( 1, m_pCurrentStyle->nSize - 1 );
		}
		else
		{
			m_pCurrentStyle->nSize+=2;
		}
		break;

	case tokMeta:
		OnGotMeta( pList );
		break;

	case tokStyleSheet:
		m_bInStyleSheet = true;
		break;
	
	case tokForm:
		OnGotForm( pList );
		break;

	case tokInput:
		OnGotFormInput( pList );
		break;

	case tokTextarea:
		OnGotTextArea( pList );
		break;
	
	case tokIgnore:
		break;

	case tokSelect:
		OnGotSelect( pList );
		break;

	case tokOption:
		OnGotOption( pList );
		break;

	case tokSpan:
		OnGotSpan( pList );
		break;

	case tokACRONYM:
		OnGotAcronym( pList );
		break;

	case tokLabel:
		OnGotLabel( pList );
		break;

#ifdef QHTM_WINDOWS_CONTROL
	case tokControl:
		OnGotControl( pList );
		break;
#endif	//	QHTM_WINDOWS_CONTROL
	}

}


void CHTMLParse::OnGotEndTag( const Token token )
{
	switch( token )
	{
	case tokTeletype:
	case tokCode:
		CreateNewTextObject();
		PopPreviousStyle();
		break;

	case tokPre:
		CreateNewTextObject();
		PopPreviousStyle();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;

	case tokCenter:
		CreateNewTextObject();
		PopPreviousStyle();
		if( m_pCurrentStyle->m_nCentered )
		{
			m_pCurrentStyle->m_nCentered--;
		}
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;
		
	case tokParagraph:
		CreateNewTextObject();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;

	case tokImage:				//	Ignore end images
		//	There is no end image tag!
		ASSERT( FALSE );
		break;

	case tokDiv:
	case tokIgnore:
		CreateNewTextObject();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;

	case tokFont:
	case tokBold:
	case tokUnderline:
	case tokItalic:
	case tokStrikeout:
	case tokSub:
	case tokSup:
		CreateNewTextObject();
		PopPreviousStyle();
		break;

	case tokH1:
	case tokH2:
	case tokH3:
	case tokH4:
	case tokH5:
	case tokH6:
		CreateNewTextObject();
		PopPreviousStyle();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;

	case tokAnchor:
		OnGotEndAnchor();
		break;

	case tokTableDef:
		OnGotEndTableCell();
		break;

	case tokTableHeading:
		OnGotEndTableCell();
		break;

	case tokTableHeadingRow:
		OnGotEndTableRow();
		break;

	case tokTableRow:
		OnGotEndTableRow();
		break;

	//
	//	IE seems to ne okay about using an end BR (</br>) so we should too.
	case tokBreak:
		{
			const CParameters pList;
			OnGotBreak( pList);
		}
		break;

	case tokTable:
		if( m_stkTable.GetSize() )
		{
			(void)m_stkTable.Pop();
		}
		else
		{
			//QHTM_TRACE( _T("Got an table but no tables left in the stack\n") );
		}

		// Pop the InACell flag
		if (m_stkInTableCell.GetSize())
		{
			// If the cell is still open... close it!
			OnGotEndTableRow();		// Does the same thing.
			(void)m_stkInTableCell.Pop();
		}
		else
		{
			//QHTM_TRACE( _T("Got an end table but in-cell stack empty.") );
		}
		PopPreviousStyle();
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		break;

	case tokHorizontalRule:
		break;

	case tokOrderedList:
	case tokUnorderedList:
		OnGotEndList();
		break;

	case tokDefinitionList:
		OnGotEndDefinitionList();
		break;

	case tokListItem:
		OnGotEndListItem();
		break;

	case tokAddress:
		OnGotEndAddress();
		break;

	case tokBlockQuote:
		OnGotEndBlockQuote();
		break;

	case tokDocumentTitle:
		m_strToken.Add( 0 );
		m_pMasterDocument->m_strTitle = m_strToken.GetData();
		(void)m_strToken.SetSize( 0 );
		break;

	case tokStyleSheet:
		m_bInStyleSheet = false;
		m_strToken.RemoveAll();
		break;

	case tokForm:
		OnGotEndForm();
		break;

	case tokTextarea:
		OnGotEndTextArea();
		break;

	case tokSelect:
		OnGotEndSelect();
		break;

	case tokOption:
		OnGotEndOption();
		break;

	case tokSpan:
		CreateNewTextObject();
		PopPreviousStyle();
		break;

	case tokSMALL:
	case tokBIG:
		CreateNewTextObject();
		PopPreviousStyle();
		break;

	case tokACRONYM:
		CreateNewTextObject();
		PopPreviousStyle();
		break;

	case tokLabel:
		CreateNewTextObject();
		PopPreviousStyle();
		break;
	
	}
}


void CHTMLParse::OnGotBody( const CParameters &pList )
{
	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pBColor:
			m_pDocument->m_back.m_crBack = GetColourFromString( strParam, m_pDocument->m_back.m_crBack );
			break;

		case pBColorLeft:
			m_pDocument->m_back.m_crLeft = GetColourFromString( strParam, m_pDocument->m_back.m_crBack );
			break;

		case pBColorRight:
			m_pDocument->m_back.m_crRight = GetColourFromString( strParam, m_pDocument->m_back.m_crBack );
			break;

		case pBColorTop:
			m_pDocument->m_back.m_crTop = GetColourFromString( strParam, m_pDocument->m_back.m_crBack );
			break;

		case pBColorBottom:
			m_pDocument->m_back.m_crBottom = GetColourFromString( strParam, m_pDocument->m_back.m_crBack );
			break;

		case pBColorCenter:
			m_pDocument->m_back.m_crCenter = GetColourFromString( strParam, m_pDocument->m_back.m_crCenter );
			break;

		case pLink:
			m_pDocument->m_crLink = GetColourFromString( strParam, RGB( 141, 7, 102 ) );
			break;

		case pALink:
			m_pDocument->m_crLinkHover = GetColourFromString( strParam, RGB( 29, 49, 149 ) );
			break;

		case pMarginTop:
			m_pDocument->m_nTopMargin = GetNumberParameter( strParam, m_pDocument->m_nTopMargin );
			break;

		case pMarginBottom:
			m_pDocument->m_nBottomMargin = GetNumberParameter( strParam, m_pDocument->m_nBottomMargin );
			break;

		case pMarginLeft:
			m_pDocument->m_nLeftMargin = GetNumberParameter( strParam, m_pDocument->m_nLeftMargin );
			break;

		case pMarginRight:
			m_pDocument->m_nRightMargin = GetNumberParameter( strParam, m_pDocument->m_nRightMargin );
			break;

		case pMarginWidth:
			m_pDocument->m_nLeftMargin = m_pDocument->m_nRightMargin = GetNumberParameter( strParam, 0 ) / 2;
			break;

		case pMarginHeight:
			m_pDocument->m_nTopMargin = m_pDocument->m_nBottomMargin = GetNumberParameter( strParam, 0 ) / 2;
			break;

		case pBackground:
			{
				StringClass fname( strParam.GetData(), strParam.GetLength() );
				m_pDocument->m_pimgBackground = OnLoadImage( fname, false, 0 );
			}
			break;
		}
	}
}


void CHTMLParse::OnGotImage( const CParameters &pList )
{
	CreateNewTextObject();
	int nHeight = 0;
	int nWidth = 0;
	int nBorder = 0;
	COLORREF crTransparent = 0;
	bool bTransparent = false;;

	CStaticString strFilename, strAltText;
	StringClass strID;
	CStyle::Align alg = knDefaultImageAlignment;

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pTransparentColor:
			crTransparent = GetColourFromString( strParam, RGB( 255, 255, 255 ) );
			bTransparent = true;
			break;

		case pWidth:
			nWidth = GetNumberParameterPercent( strParam, nWidth );
			break;

		case pHeight:
			nHeight = GetNumberParameterPercent( strParam, nHeight );
			break;

		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;

		case pSrc:
			strFilename = strParam;
			break;

		case pBorder:
			nBorder = GetNumberParameter( strParam, nBorder );
			break;
		
		case pAlt:
			strAltText = strParam;
			break;

		case pID:
			strID.Set( strParam, strParam.GetLength() );
			break;
		}
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();

	StringClass fname( strFilename.GetData(), strFilename.GetLength() );
	StringClass fnameFind( strFilename.GetData(), strFilename.GetLength() );
	if( bTransparent )
	{
		TCHAR szBuffer[ 20 ];
		wsprintf( szBuffer, _T("t-%d"), crTransparent );
		fnameFind += szBuffer;
	}

	// Before loading the image, see if it already exists in the cache.

	CQHTMImageABC** ppLoadedImage = NULL;
	if( fnameFind.GetLength() )
	{
		ppLoadedImage = m_pMasterDocument->m_mapImages.Lookup( fnameFind );
	}

	CQHTMImageABC* pImage = NULL;
	if (!ppLoadedImage)
	{
		//TRACENL( _T("Image %s not in cache. Attempting Load\n"), (LPCTSTR)fname);
		
		if( fname.GetLength() )
		{
			pImage = OnLoadImage( fname, bTransparent, crTransparent );
			//TRACENL( _T("Loaded %s\n"), (LPCTSTR)fname);
		}

		if( !pImage )
		{
			//TRACENL( _T("Image %s could not be loaded.\n"), (LPCTSTR)fname);

#ifdef QHTM_BUILD_INTERNAL_IMAGING
			// Consider making this image a global instance.
			pImage = new CQHTMImage;

			CResourceDataSource ds;
			if( ds.Open( g_hResourceInstance, MAKEINTRESOURCE( g_uNoImageBitmapID ), RT_RCDATA ) )
			{
				VERIFY( static_cast<CQHTMImage *>( pImage )->Load( ds ) );
			}
#endif	//	QHTM_BUILD_INTERNAL_IMAGING
		}

		if( fnameFind.GetLength() )
		{
			m_pMasterDocument->m_mapImages.SetAt( fnameFind, pImage );
		}
	}
	else
	{
		//TRACENL( _T("Image %s loaded from cache.\n"), (LPCTSTR)fname);
		pImage = *ppLoadedImage;
	}

	CHTMLImage *pHTMLImage = new CHTMLImage( nWidth, nHeight, nBorder, fname, alg, pImage, strAltText );
	pHTMLImage->m_uID = m_pMasterDocument->GetNextItemID();
	pHTMLImage->SetElementID( strID );
	UpdateItemLinkStatus( pHTMLImage );

	pPara->AddItem( pHTMLImage );
}


CQHTMImageABC *CHTMLParse::OnLoadImage( LPCTSTR pcszFilename, bool bIsTransparent, COLORREF crForceTransparent )
//
//	Called when we need an image to be loaded.
{
#ifdef QHTM_ALLOW_IMAGECALLBACK
	if( m_pDefaults->m_funcQHTMImageCallback )
	{
		CQHTMImageABC *pImage = m_pDefaults->m_funcQHTMImageCallback( pcszFilename, m_pDefaults->m_lParam );
		if( pImage )
		{
			return pImage;
		}
	}
	else if( m_pDefaults->m_funcBitmapCallback )
	{
		HBITMAP hbmp = m_pDefaults->m_funcBitmapCallback( pcszFilename, m_pDefaults->m_lParam );
		if( hbmp )
		{
			CQHTMImage *pImage = new CQHTMImage;
			(void)pImage->CreateFromBitmap( hbmp );
			if( bIsTransparent ) pImage->ForceTransparent( crForceTransparent );
			return pImage;
		}
	}
	else if( m_pDefaults->m_funcQHTMResourceCallback )
	{
		HGLOBAL h = m_pDefaults->m_funcQHTMResourceCallback( pcszFilename, m_pDefaults->m_lParam );
		if( h )
		{
			DWORD dw = static_cast<DWORD>( GlobalSize( h ) );
			BYTE *pByte = reinterpret_cast<BYTE *>( GlobalLock( h ) );
			if( pByte )
			{
				CBufferDataSource ds;
				if( ds.Open( pByte, dw ) )
				{
					CQHTMImage *pImage = new CQHTMImage;
					if( pImage->Load( ds ) )
					{
						GlobalUnlock( h );
						GlobalFree( h );

						if( bIsTransparent ) pImage->ForceTransparent( crForceTransparent );

						return pImage;
					}
				}
				GlobalUnlock( h );
				GlobalFree( h );
			}
		}
	}

#endif	//	QHTM_ALLOW_IMAGECALLBACK

#ifdef  QHTM_BUILD_INTERNAL_IMAGING
	return AquireImage( m_hInstLoadedFrom, m_pcszFilePath, pcszFilename, bIsTransparent, crForceTransparent );
#else
	UNREF( pcszFilename );
	UNREF( bIsTransparent );
	UNREF( crForceTransparent );
	return NULL;
#endif	//	QHTM_BUILD_INTERNAL_IMAGING
}


void CHTMLParse::UpdateItemLinkStatus( CHTMLParagraphObject *pItem )
{
	// Assign the current link pointer to the object
	pItem->m_pAnchor = m_pLastAnchor;	

}


void CHTMLParse::CreateNewTextObject()
//
//	Create a new text block and add it to the current docuemnt-paragraph
{
	if( m_strToken.GetSize() )
	{
		CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
		HTMLFontDef def( m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_nSup, m_pCurrentStyle->m_nSub, m_pCurrentStyle->m_bFixedPitchFont );

		CStaticString str( m_strToken.GetData(), m_strToken.GetSize() );
		CHTMLTextBlock *pText = new CHTMLTextBlock( str, m_pDocument->GetFontDefIndex( def ), m_pCurrentStyle->m_crFore, m_pCurrentStyle->m_crBack, IsPreformatted() );
		if( m_pLastAnchor )
			UpdateItemLinkStatus( pText );

		if( m_pCurrentStyle->m_strTitle.GetLength() )
		{
			pText->m_strTip = m_pCurrentStyle->m_strTitle;
		}

		if( m_pCurrentStyle->m_strActivationTarget.GetLength() )
		{
			pText->m_strActivationTarget = m_pCurrentStyle->m_strActivationTarget;
		}

		pPara->AddItem( pText );

		(void)m_strToken.SetSize( 0 );
	}
}


void CHTMLParse::CreateNewStyle()
//
//	Create a new CStyle set and add it to the stack.
{
	m_pCurrentStyle = new CStyle( *m_pCurrentStyle );
	m_stkStyle.Push( m_pCurrentStyle );
}


void CHTMLParse::PopPreviousStyle()
{
	if( m_stkStyle.GetSize() > 1 )
	{
		delete m_stkStyle.Pop();
		m_pCurrentStyle = m_stkStyle.Top();
	}
}


void CHTMLParse::CreateNewParagraph( int nLinesAbove, int nLinesBelow, CStyle::Align alg )
{
	if( m_pDocument->CurrentParagraph() && m_pDocument->CurrentParagraph()->IsEmpty() )
	{
		m_pDocument->CurrentParagraph()->Reset( nLinesAbove, nLinesBelow, alg );
	}
	else
	{
		m_pDocument->AddParagraph( new CHTMLParagraph( nLinesAbove, nLinesBelow, alg ) );
	}
}


void CHTMLParse::OnGotHR( const CParameters &pList )
{
	CreateNewTextObject();
	if( !m_pDocument->CurrentParagraph() || !m_pDocument->CurrentParagraph()->IsEmpty() )
	{
		CreateNewParagraph( m_pDefaults->m_nParagraphLinesAbove, m_pDefaults->m_nParagraphLinesBelow, m_pCurrentStyle->nAlignment );
	}

	CStyle::Align alg = knDefaultHRAlignment;
	int nSize = 2;
	int nWidth = -100;
	bool bNoShade = false;
	COLORREF crColor = 0;

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pWidth:
			nWidth = GetNumberParameterPercent( strParam, nWidth );
			if( nWidth == 0 )
				nWidth = -100;
			break;

		case pSize:
			//	REVIEW - russf - could allow dan to use point/font sizes here too.
			nSize = GetNumberParameter(strParam, nSize);
			if( nSize <= 0 )
				nSize = 3;
			break;

		case pNoShade:
			bNoShade = true;
			break;

		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;

		case pColor:
			crColor = GetColourFromString( strParam, crColor );
			break;
		}
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	CHTMLHorizontalRule *pHR = new CHTMLHorizontalRule( alg, nSize, nWidth, bNoShade, crColor );
	UpdateItemLinkStatus( pHR );
	pPara->AddItem( pHR );
	CreateNewParagraph( m_pDefaults->m_nParagraphLinesAbove, m_pDefaults->m_nParagraphLinesBelow, m_pCurrentStyle->nAlignment );
}


void CHTMLParse::OnGotParagraph( const CParameters &pList )
{
	CreateNewTextObject();
	CStyle::Align alg = m_pCurrentStyle->nAlignment;

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;
		}
	}

	m_pCurrentStyle->nAlignment;
	CreateNewParagraph( m_pDefaults->m_nParagraphLinesAbove, m_pDefaults->m_nParagraphLinesBelow, alg );
}


void CHTMLParse::OnGotFont( const CParameters &pList )
{
	CreateNewTextObject();
	CreateNewStyle();

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pColor:
			m_pCurrentStyle->m_crFore = GetColourFromString( strParam, m_pCurrentStyle->m_crFore );
			break;

		case pBColor:
			m_pCurrentStyle->m_crBack = GetColourFromString( strParam, m_pCurrentStyle->m_crFore );
			break;

		case pFace:
			if( strParam.GetLength() )
			{
				GetFontName( m_pCurrentStyle->szFaceName, countof( m_pCurrentStyle->szFaceName ), strParam );
				m_pCurrentStyle->m_bFixedPitchFont = IsFontFixed( m_pCurrentStyle->szFaceName, m_pDocument->m_cCharSet );
			}
			break;

		case pSize:
			{
				m_pCurrentStyle->nSize = GetFontSize( strParam, m_pCurrentStyle->nSize );
				if( m_pCurrentStyle->nSize > 7 )
				{
					m_pCurrentStyle->nSize = 7;
				}
			}
			break;
		}
	}
}


void CHTMLParse::OnGotHeading( const Token token, const CParameters &pList )
{
	int nLinesAbove = 1, nLinesBelow = 1;
	CStyle::Align alg	= m_pCurrentStyle->m_nCentered ? m_pCurrentStyle->nAlignment : CStyle::algLeft;

	CreateNewTextObject();
	CreateNewStyle();

	switch( token )
	{
	case tokH1:
		m_pCurrentStyle->nSize = 6;
		m_pCurrentStyle->bBold = true;
		break;

	case tokH2:
		m_pCurrentStyle->nSize = 5;
		m_pCurrentStyle->bBold = true;
		break;

	case tokH3:
		m_pCurrentStyle->nSize = 4;
		m_pCurrentStyle->bBold = true;
		break;

	case tokH4:
		m_pCurrentStyle->nSize = 3;
		m_pCurrentStyle->bBold = true;
		break;

	case tokH5:
		m_pCurrentStyle->nSize = 3;
		m_pCurrentStyle->bBold = true;
		break;

	case tokH6:
		nLinesAbove = 2;
		m_pCurrentStyle->nSize = 1;
		m_pCurrentStyle->bBold = true;
		break;
	}

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;
		}
	}

	CreateNewParagraph( nLinesAbove, nLinesBelow, alg );

}


void CHTMLParse::OnGotAnchor( const CParameters &pList )
{
	//	We will no longer store link attributes in the CStyle.
	//	They will instead be stored in an inline ParagraphObject
	//	The sectionCreator will be responsible for keeping the list
	//	of links and map of named sections
	
	//	Since the coloring will change, we will still create a new
	//	text object. This also ensures that we have a paragraph into which
	//	we store the HTMLAnchor object.
	CreateNewTextObject();

	//	Anchor information is no longer stored in the CStyle stack,
	//	So we don't need new CStyle.
	//	CreateNewStyle();


	//	Create a new Anchor object, then store some data in it.
	//	This one has a default ctor, which provides empty members.
	//	We need to specify colors, though.

	CHTMLAnchor* pAnchor = new CHTMLAnchor();

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pName:
			pAnchor->m_strLinkName.Set( strParam.GetData(), strParam.GetLength() );
			break;

		case pHref:
			pAnchor->m_strLinkTarget.Set( strParam.GetData(), strParam.GetLength() );
			break;

		case pID:
			pAnchor->m_strID.Set( strParam.GetData(), strParam.GetLength() );
			pAnchor->SetElementID( pAnchor->m_strID );
			break;

		case pTitle:
			pAnchor->m_bTitleSet = true;
			pAnchor->m_strLinkTitle.Set( strParam.GetData(), strParam.GetLength() );
			break;
		}
	}

	//	Get the colors from the master document, which is the only place they should be set
	pAnchor->m_crLink = m_pMasterDocument->m_crLink;
	pAnchor->m_crHover = m_pMasterDocument->m_crLinkHover;

	CreateNewStyle();

	if( pAnchor->m_strLinkTarget.GetLength() )
	{
		m_pCurrentStyle->m_crFore = m_pMasterDocument->m_crLink;
	}

	
	//	Signifies to the parser that we are in a anchor tag, and this is it!
	//	This remains complicated by named tags. i.e. what should happen
	//	in this case:
	//	<a href="someTarget">Some text<a name="here">More Text</a> Other Text</a>
	//	is the Other Text in a link?
	//	In IE4, the second tag cancels out the first. We will use that behavior.
	if( pAnchor->m_strLinkTarget.GetLength() )
	{
		m_pLastAnchor = pAnchor;
		m_pCurrentStyle->bUnderline = true;
	}
	else
	{
		m_pLastAnchor = NULL;
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	pPara->AddItem( pAnchor );
}


void CHTMLParse::OnGotEndAnchor()
{
	//	Signify to the parser that we are no longer in an anchor tag.
	//	If we are truly anding a link, make a new text object.
	//	Otherwise, it's ignored.
	if( m_pLastAnchor )
		CreateNewTextObject();
	m_pLastAnchor = NULL;
	PopPreviousStyle();
}


void CHTMLParse::OnGotTable( const CParameters &pList )
{
	CreateNewTextObject();
	int nWidth = 0;	// Default width - unspec, -100;	//	For 100 percent
	int nBorder = 0;
	int nHeight = 0;
	int nCellPadding = m_pDefaults->m_nCellPadding;
	int nCellSpacing = m_pDefaults->m_nCellSpacing;
	COLORREF crBorder = RGB(0, 0, 0);
	CBackground background;
	COLORREF crBorderLight = m_pDefaults->m_crBorderLight;
	COLORREF crBorderDark = m_pDefaults->m_crBorderDark;
	bool bBorderDarkSet = false;
	bool bBorderLightSet = false;

	CStyle::Align alg = knDefaultTableAlignment;
	CStyle::Align valg = knDefaultTableAlignment;

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pHeight:
			nHeight = GetNumberParameterPercent( strParam, nHeight );
			break;

		case pWidth:
			nWidth = GetNumberParameterPercent( strParam, nWidth );
			break;

		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;

		case pVAlign:
			valg = GetAlignmentFromString( strParam, valg );
			break;

		case pBorder:
			nBorder = GetNumberParameter( strParam, nBorder );
			break;

		case pBColor:
			background.m_crBack = GetColourFromString( strParam, RGB( 255, 255, 255 ) );
			break;

		case pBColorLeft:
			background.m_crLeft = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorRight:
			background.m_crRight = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorTop:
			background.m_crTop = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorBottom:
			background.m_crBottom = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorCenter:
			background.m_crCenter = GetColourFromString( strParam, background.m_crCenter );
			break;

		case pCellSpacing:
			nCellSpacing = GetNumberParameter( strParam, nCellSpacing );
			break;

		case pCellPadding:
			nCellPadding = GetNumberParameter( strParam, nCellPadding );
			break;

		case pBackground:
			{
				StringClass fname( strParam.GetData(), strParam.GetLength() );
				background.m_pimgBackground = OnLoadImage( fname, false, 0 );
			}
			break;

		case pBorderColor:
			crBorder = GetColourFromString( strParam, crBorder );
			if ( !bBorderDarkSet )
				crBorderDark = crBorder;
			if ( !bBorderLightSet )
				crBorderLight = crBorder;
			break;

		case pBorderColorLight:
			crBorderLight = GetColourFromString( strParam, crBorderLight );
			bBorderLightSet = true;
			break;

		case pBorderColorDark:
			crBorderDark = GetColourFromString( strParam, crBorderDark );
			bBorderDarkSet = true;
			break;

		}
	}

	switch( alg )
	{
	case CStyle::algLeft: case CStyle::algRight:
		break;

	default:
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
	}

	CHTMLTable *ptab = new CHTMLTable( nWidth, nHeight, nBorder, alg, valg, nCellSpacing, nCellPadding, background, crBorderDark, crBorderLight);
	m_stkTable.Push( ptab  );
	m_pDocument->CurrentParagraph()->AddItem( m_stkTable.Top() );
	// Push onto the inTableCell stack
	m_stkInTableCell.Push(false);	// Not in a cell yet!
}


void CHTMLParse::OnGotTableRow( const CParameters &pList )
{
	// Close an open cell first.
	if (m_stkInTableCell.GetSize() && m_stkInTableCell.Top())
		OnGotEndTableCell();

	CStyle::Align valg = knDefaultTableAlignment;
	if( m_stkTable.GetSize() )
	{
		valg = m_stkTable.Top()->m_valg;
		
		m_stkTable.Top()->NewRow( valg );

		CHTMLTable::CHTMLTableRow	*pRow = m_stkTable.Top()->GetCurrentRow();

		const size_t uParamSize = pList.GetSize();
		for( size_t n = 0; n < uParamSize; n++ )
		{
			const CStaticString &strParam = pList[n].m_strValue;
			switch( pList[n].m_param )
			{
			case pVAlign:
				pRow->m_valg = GetAlignmentFromString( strParam, valg );
				break;

			case pHeight:
				pRow->m_nHeight = GetNumberParameter( strParam, 0 );
				break;

			case pBackground:
				{
					StringClass fname( strParam.GetData(), strParam.GetLength() );
					pRow->m_back.m_pimgBackground = OnLoadImage( fname, false, 0 );
				}
				break;

			case pBColor:
				pRow->m_back.m_crBack = GetColourFromString( strParam, 0 );
				break;

			case pBColorLeft:
				pRow->m_back.m_crLeft = GetColourFromString( strParam, 0 );
				break;

			case pBColorRight:
				pRow->m_back.m_crRight = GetColourFromString( strParam, 0 );
				break;

			case pBColorTop:
				pRow->m_back.m_crTop = GetColourFromString( strParam, 0 );
				break;

			case pBColorBottom:
				pRow->m_back.m_crBottom = GetColourFromString( strParam, 0 );
				break;

			case pBColorCenter:
				pRow->m_back.m_crCenter = GetColourFromString( strParam, 0 );
				break;
			}
		}
	}
}


void CHTMLParse::OnGotTableCell( const CParameters &pList )
{
	//
	//	No table? no table cell then.
	if( !m_stkTable.GetSize() )
		return;

	// Ensure that we are processing documents properly.
	// close the previous cell if it was left open...
	if (m_stkInTableCell.GetSize())
	{
		if (m_stkInTableCell.Top())
			OnGotEndTableCell();
		m_stkInTableCell.Top() = true;
	}

	if( !m_stkTable.Top()->GetCurrentRow() )
	{
		return;
	}
	int nWidth = 0;
	int nHeight = 0;
	bool bNoWrap = false;
	COLORREF crBorder = RGB(0,0,0);
	CBackground background = m_stkTable.Top()->GetCurrentRow()->m_back;
	COLORREF crBorderLight = m_pDefaults->m_crBorderLight;
	COLORREF crBorderDark = m_pDefaults->m_crBorderDark;
	bool bBorderDarkSet = false;
	bool bBorderLightSet = false;
	int nColSpan = 1;

	CStyle::Align alg = knDefaultTableCellAlignment;
	CHTMLTable *pTable = m_stkTable.Top();

	if( pTable && !pTable->GetCurrentRow() )
	{
		pTable->NewRow( knDefaultTableAlignment );
	}
	CStyle::Align valg = pTable ? pTable->GetCurrentRow()->m_valg : knDefaultTableAlignment;

	// Get defaults from the current table
	// Could really ignore everything if we are not in a table!
	if( m_stkTable.GetSize() )
	{
		CHTMLTable* pTab = m_stkTable.Top();
		crBorderLight = pTab->m_crBorderLight;
		crBorderDark = pTab->m_crBorderDark;
	}

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pWidth:
			nWidth = GetNumberParameterPercent( strParam, nWidth );
			break;

		case pHeight:
			nHeight = GetNumberParameter( strParam, nHeight );
			break;

		case pColSpan:
			nColSpan = GetNumberParameter( strParam, nColSpan );
			break;


		case pAlign:
			alg = GetAlignmentFromString( strParam, alg );
			break;

		case pVAlign:
			valg = GetAlignmentFromString( strParam, alg );
			break;

		//  richg - 19990224 - Add support for NOWRAP
		case pNoWrap:
			bNoWrap = true;
			break;

		case pBackground:
			{
				StringClass fname( strParam.GetData(), strParam.GetLength() );
				background.m_pimgBackground = OnLoadImage( fname, false, 0 );
			}
			break;

		case pBColor:
			background.m_crBack = GetColourFromString( strParam, RGB( 255, 255, 255 ) );
			break;

		case pBColorLeft:
			background.m_crLeft = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorRight:
			background.m_crRight = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorTop:
			background.m_crTop = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorBottom:
			background.m_crBottom = GetColourFromString( strParam, background.m_crBack );
			break;

		case pBColorCenter:
			background.m_crCenter = GetColourFromString( strParam, background.m_crCenter );
			break;

		case pBorderColor:
			crBorder = GetColourFromString( strParam, crBorder );
			if ( !bBorderDarkSet )
				crBorderDark = crBorder;
			if ( !bBorderLightSet )
				crBorderLight = crBorder;
			break;

		case pBorderColorLight:
			crBorderLight = GetColourFromString( strParam, crBorderLight );
			bBorderLightSet = true;
			break;

		case pBorderColorDark:
			crBorderDark = GetColourFromString( strParam, crBorderDark );
			bBorderDarkSet = true;
			break;

		}
	}

	//
	//	If there is a table to add our cell to...
	if( m_stkTable.GetSize() )
	{
		CHTMLTableCell *pCell = new CHTMLTableCell( m_pDefaults, nWidth, nHeight, bNoWrap, background, crBorderDark, crBorderLight, valg, nColSpan );
		pCell->m_cCharSet = m_pDocument->m_cCharSet;
		// Copy the current documents link colors into the new document
		pCell->m_crLink = m_pDocument->m_crLink;
		pCell->m_crLinkHover = m_pDocument->m_crLinkHover;
		// Push it onto the document stack
		m_stkDocument.Push( m_pDocument );

		// TRACENL( _T("Document Stack Pushed. %d\n"), m_stkDocument.GetSize());

		m_pDocument = pCell;
		m_pCurrentStyle->nAlignment = alg;
		CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );
		m_stkTable.Top()->AddCell( pCell );
	}
}


void CHTMLParse::OnGotEndTableCell()
{
	CreateNewTextObject();
	if( m_stkDocument.GetSize() )
	{
		m_pDocument = m_stkDocument.Pop();
		// TRACENL( _T("Document Stack Popped. %d\n"), m_stkDocument.GetSize());
	}
	else
	{	
		//QHTM_TRACE( _T("Got an end table cell but no document stack left\n") );
	}

	if (m_stkInTableCell.GetSize())
		m_stkInTableCell.Top() = false;
}


void CHTMLParse::OnGotEndTableRow()
{
	if( m_stkInTableCell.GetSize() && m_stkInTableCell.Top() )
		OnGotEndTableCell();
}


void CHTMLParse::CreateList( bool bOrdered, const CParameters & paramList )
{
	CreateNewTextObject();
	CreateNewParagraph( 0, 0, CStyle::algLeft );

	CHTMLList *pList = new CHTMLList( bOrdered );

	const size_t uParamSize = paramList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = paramList[n].m_strValue;
		switch( paramList[ n ].m_param )
		{
		case pCompact:
			pList->m_bCompact = true;
			break;

		case pType:
			pList->SetType( strParam );
			break;
		}
	}
	m_stkList.Push( pList  );
	m_pDocument->CurrentParagraph()->AddItem( m_stkList.Top() );

	// Create the default first item
	CHTMLListItem *pItem = new CHTMLListItem( m_pDefaults, false, m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_crFore, m_pDocument->m_cCharSet );	// Not bulleted
	// Copy the current documents link colors into the new document
	pItem->m_crLink = m_pDocument->m_crLink;
	pItem->m_crLinkHover = m_pDocument->m_crLinkHover;
	m_stkDocument.Push( m_pDocument );
	m_pDocument = pItem;
	CreateNewParagraph( 0, 0, CStyle::algLeft );
	m_stkList.Top()->AddItem( pItem ); 
}


void CHTMLParse::OnGotUnorderedList( const CParameters & pList )
{
	CreateList( false, pList );
}


void CHTMLParse::OnGotOrderedList( const CParameters & pList )
{
	CreateList( true, pList );
}


void CHTMLParse::OnGotListItem( const CParameters & paramList )
{
	// </li> is completely ignored by IE4, and by NetScape
	// Additionally, items occuring after the <ol> or <ul> and
	// before the first <li> are positioned as if they were preceeded
	// by <li>, but the bullet is not shown. Empty <LI> tags DO show their
	// bullets.
	// In order to handle these cases, do the following:
	// For a new list, create the initial list item, but flag it as
	// unbulleted. When a <LI> is reached, determine if there is
	// exactly one item in the list. If so, and it is empty, Mark the
	// existing item as bulleted, and use it... otherwise, proceed by adding
	// another list item.

	// See that we are in a list, if not then create a new list
	if( !m_stkList.GetSize() )
	{
		CParameters paramEmpty;
		CreateList( false, paramEmpty );
	}

	{
		CreateNewTextObject();

		UINT uValue = 0;
		bool bGotValue = false;
		const size_t uParamSize = paramList.GetSize();
		for( size_t n = 0; n < uParamSize; n++ )
		{
			const CStaticString &strParam = paramList[n].m_strValue;
			switch( paramList[ n ].m_param )
			{
			case pValue:
				uValue = GetNumberParameter( strParam, static_cast< int >( m_stkList.GetSize() + 1 ) );
				bGotValue = true;
				break;
			}
		}

		// See if we can use the existing item
		if ( m_stkList.Top()->m_arrItems.GetSize() == 1
				&& !m_stkList.Top()->m_arrItems[0]->m_bBullet
				&&  m_stkList.Top()->m_arrItems[0]->IsEmpty() )
		{
			// There is en emptry entry that is not bulleted
			m_stkList.Top()->m_arrItems[0]->m_bBullet = true;

			if( bGotValue )
			{
				m_stkList.Top()->m_arrItems[0]->SetValue( uValue );
			}

			return;
		}

		// We are in a list and creating a new item, close the previous item
		CreateNewTextObject();
		if( m_stkDocument.GetSize() )
		{
			m_pDocument = m_stkDocument.Pop();
		}

		// Need to create a new item...
		CHTMLListItem *pItem = new CHTMLListItem( m_pDefaults, true, m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_crFore, m_pDocument->m_cCharSet );	// bulleted
		if( bGotValue )
		{
			pItem->SetValue( uValue );
		}

		// Copy the current documents link colors into the new document
		pItem->m_crLink = m_pDocument->m_crLink;
		pItem->m_crLinkHover = m_pDocument->m_crLinkHover;
		m_stkDocument.Push( m_pDocument );
		m_pDocument = pItem;
		CreateNewParagraph( 0, 0, CStyle::algLeft );
		m_stkList.Top()->AddItem( pItem ); 
	}
}


void CHTMLParse::OnGotEndListItem()
{
	// Do nothing.
}

void CHTMLParse::OnGotEndList()
{
	// See that we are in a list, otherwise, ignore it...
	if ( m_stkList.GetSize() )
	{
		// Close the last item...
		CreateNewTextObject();
		if( m_stkDocument.GetSize() )
		{
			m_pDocument = m_stkDocument.Pop();
		}
		CreateNewParagraph( 0, 0, CStyle::algLeft );
		// Close this list.
		(void)m_stkList.Pop();
	}
}


void CHTMLParse::OnGotBlockQuote( const CParameters & /* pList */ )
{
	/*
		A Blockquote is treated as a sub document with different
		margins. This is fairly straightforward.
	*/
	CreateNewTextObject();
	CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );

	CHTMLBlockQuote *pBQ = new CHTMLBlockQuote( m_pDefaults );
	m_pDocument->CurrentParagraph()->AddItem( pBQ );
	// Make the sub document the main document
	m_stkDocument.Push( m_pDocument );
	m_pDocument = pBQ->m_pDoc;
	CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment);
}


void CHTMLParse::OnGotEndBlockQuote()
{
	/*
		Cause a document stack pop if possible...
	*/
	CreateNewTextObject();
	if( m_stkDocument.GetSize() )
	{
		m_pDocument = m_stkDocument.Pop();
	}
	else
	{
		//QHTM_TRACE( _T("At </BLOCKQUOTE> but document stack is empty.\n") );
	}
	CreateNewParagraph(0, 0, m_pCurrentStyle->nAlignment);
}



void CHTMLParse::OnGotAddress( const CParameters & /* pList */ )
//
//	Fundtionally the same as BLOCKQUOTE but with italics
{
	CreateNewTextObject();
	CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment );

	CreateNewStyle();
	m_pCurrentStyle->bItalic = true;

	CHTMLBlockQuote *pBQ = new CHTMLBlockQuote( m_pDefaults );
	m_pDocument->CurrentParagraph()->AddItem( pBQ );
	// Make the sub document the main document
	m_stkDocument.Push( m_pDocument );
	m_pDocument = pBQ->m_pDoc;
	CreateNewParagraph( 0, 0, m_pCurrentStyle->nAlignment);
}


void CHTMLParse::OnGotEndAddress()
{
	CreateNewTextObject();
	if( m_stkDocument.GetSize() )
	{
		m_pDocument = m_stkDocument.Pop();
	}
	else
	{
		//QHTM_TRACE( _T("At </BLOCKQUOTE> but document stack is empty.\n") );
	}
	PopPreviousStyle();
	CreateNewParagraph(0, 0, m_pCurrentStyle->nAlignment);
}


void CHTMLParse::GetFontName( LPTSTR pszBuffer, int nBufferSize, const CStaticString &strFontNameSpec )
//
//	Get the font name from a font name spec.
//	This consists of comma delimited font names "tahoma,arial,helvetica"
//	We need to find
{
	static const TCHAR cComma = _T(',');
	if( strFontNameSpec.GetData() )
	{
		LPCTSTR pcszFontNameSpec = strFontNameSpec.GetData();
		if( strFontNameSpec.Find( cComma ) )
		{
			//
			//	Iterate over the passed font names, copying each font name as we go.
			LPCTSTR pcszFontNameSpecEnd = strFontNameSpec.GetEndPointer();
			LPCTSTR pszFontNameStart, pszFontNameEnd;
			pszFontNameStart = pszFontNameEnd = pcszFontNameSpec;

			while( pszFontNameStart < pcszFontNameSpecEnd )
			{
				//
				//	Find the end of the first font.
				while( pszFontNameEnd < pcszFontNameSpecEnd && *pszFontNameEnd != cComma )
					pszFontNameEnd++;

				if( pszFontNameEnd - pszFontNameStart && pszFontNameEnd - pszFontNameStart < nBufferSize)
				{
					StringClass strFontName( pszFontNameStart, static_cast<UINT>( pszFontNameEnd - pszFontNameStart ) );

					//
					//	We have our font name, now determine whether the font exists or not.
					if( DoesFontExist( strFontName, m_pDocument->m_cCharSet ) )
					{
						//	It does, then we get out of here. If the font doesn't exist then we simply
						//	continue looping around.
						_tcscpy( pszBuffer, strFontName );
						return;
					}
				}


				//
				//	Skip past the white space and commas
				while( pszFontNameEnd < pcszFontNameSpecEnd && ( isspace( *pszFontNameEnd ) || *pszFontNameEnd == cComma ) )
					pszFontNameEnd++;

				pszFontNameStart = pszFontNameEnd;
			}
		}
		else
		{
			StringClass strFontName( strFontNameSpec.GetData(), strFontNameSpec.GetLength() );
			if( DoesFontExist( strFontName, m_pDocument->m_cCharSet ) )
			{
				//	It does, then we get out of here. If the font doesn't exist then we simply
				//	continue looping around.
				_tcscpy( pszBuffer, strFontName );
			}
			else
			{
				_tcscpy( pszBuffer, m_pDefaults->m_strFontName );
			}
		}

	}
	else
	{
		_tcscpy( pszBuffer, m_pDefaults->m_strFontName );
	}

	//
	//	If we drop out here it means that none of the user requested fonts exist on the system.
}


void CHTMLParse::OnGotMeta( const CParameters &pList )
{
	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pContent:
			{
				LPCTSTR pcszEnd = strParam.GetData() + strParam.GetLength();
				static const TCHAR szCharSet[] = _T("charset");
				LPCTSTR p = stristr( (LPTSTR)strParam.GetData(), szCharSet );
				if( p )
				{
					//
					//	We have a charset, so next we need to decode it
					p += countof( szCharSet ) - 1;
					while( p < pcszEnd && ( isspace( *p ) || *p == _T('=') ) )
						p++;
					if( p < pcszEnd )
					{
						const CStaticString str( p, static_cast< UINT > ( pcszEnd - p ) );
						m_pDocument->m_cCharSet = DecodeCharset( str );
					}
				}
			}
			break;

		case pHTTPEquiv:
			//TRACE( "pHTTPEquiv %s\n", strParam );
			break;
		}
	}
}


void CHTMLParse::OnGotBreak( const CParameters &pList )
{
	CreateNewTextObject();

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	CHTMLBreak *pBreak = new CHTMLBreak;
	if( m_pLastAnchor )
		UpdateItemLinkStatus( pBreak );

	pBreak->m_clear = CHTMLBreak::knNone;

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pClear:
			if( _tcsnicmp( strParam.GetData(), _T("all"), 3 ) == 0 )
			{
				pBreak->m_clear = CHTMLBreak::knAll;
			}
			else if( _tcsnicmp( strParam.GetData(), _T("left"), 4 ) == 0 )
			{
				pBreak->m_clear = CHTMLBreak::knLeft;
			}
			else if( _tcsnicmp( strParam.GetData(), _T("right"), 5 ) == 0 )
			{
				pBreak->m_clear = CHTMLBreak::knRight;
			}
			break;
		}
	}

	pPara->AddItem( pBreak );
}


void CHTMLParse::OnGotDefinitionList()
{
	CreateNewTextObject();
	CreateNewParagraph( 0, 0, CStyle::algLeft );

	CHTMLDefinitionList *pList = new CHTMLDefinitionList;

	m_stkDefinitionList.Push( pList  );
	m_pDocument->CurrentParagraph()->AddItem( m_stkDefinitionList.Top() );

	// Create the default first item
	CHTMLDefinitionListItem *pItem = new CHTMLDefinitionListItem( m_pDefaults, true );

	// Copy the current documents link colors into the new document
	pItem->m_crLink = m_pDocument->m_crLink;
	pItem->m_crLinkHover = m_pDocument->m_crLinkHover;

	m_stkDocument.Push( m_pDocument );
	m_pDocument = pItem;

	CreateNewParagraph( 0, 0, CStyle::algLeft );
	m_stkDefinitionList.Top()->AddItem( pItem ); 
}


void CHTMLParse::OnGotDefinitionItem( bool bTerm )
{
	if( !m_stkDefinitionList.GetSize() )
		OnGotDefinitionList();

	CreateNewTextObject();

	// See if we can use the existing item
	if ( m_stkDefinitionList.Top()->m_arrItems.GetSize() == 1
			&&  m_stkDefinitionList.Top()->m_arrItems[0]->IsEmpty() )
	{
		m_stkDefinitionList.Top()->m_arrItems[0]->m_bTerm = bTerm;
		return;
	}


	// We are in a list and creating a new item, close the previous item
	CreateNewTextObject();
	if( m_stkDocument.GetSize() )
	{
		m_pDocument = m_stkDocument.Pop();
	}

	// Need to create a new item...
	CHTMLDefinitionListItem *pItem = new CHTMLDefinitionListItem( m_pDefaults, bTerm );

	// Copy the current documents link colors into the new document
	pItem->m_crLink = m_pDocument->m_crLink;
	pItem->m_crLinkHover = m_pDocument->m_crLinkHover;
	m_stkDocument.Push( m_pDocument );
	m_pDocument = pItem;

	CreateNewParagraph( 0, 0, CStyle::algLeft );
	m_stkDefinitionList.Top()->AddItem( pItem );
}


void CHTMLParse::OnGotEndDefinitionList()
{
	if( m_stkDefinitionList.GetSize() )
	{
		// Close the last item...
		CreateNewTextObject();

		if( m_stkDocument.GetSize() )
		{
			m_pDocument = m_stkDocument.Pop();
		}

		CreateNewParagraph( 0, 0, CStyle::algLeft );

		// Close this list.
		(void)m_stkDefinitionList.Pop();
	}
}



void CHTMLParse::OnGotForm( const CParameters &pList )
{
	CreateNewTextObject();
	CreateNewParagraph( 0, 0, CStyle::algLeft );

	CHTMLForm *pForm = new CHTMLForm;
	m_stkForm.Push( pForm );
	m_pMasterDocument->m_arrForms.Add( pForm );

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pAction:
			pForm->m_strAction.Set( strParam, strParam.GetLength() );
			break;

		case pMethod:
			pForm->m_strMethod.Set( strParam, strParam.GetLength() );
			break;

		case pName:
			pForm->m_strName.Set( strParam, strParam.GetLength() );
			break;
		}
	}
}


void CHTMLParse::OnGotEndForm()
{
	if( m_stkForm.GetSize() )
	{
		(void)m_stkForm.Pop();
	}
#ifdef _DEBUG
	else
	{
		TRACE( _T("End form found but no form specified\n") );
	}
#endif	//	 _DEBUG

	CreateNewTextObject();
	CreateNewParagraph( 0, 0, CStyle::algLeft );
}


void CHTMLParse::OnGotFormInput( const CParameters &pList )
{
	CreateNewTextObject();
	m_pCurrentStyle->m_bFixedPitchFont = true;

	HTMLFontDef def( m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_nSup, m_pCurrentStyle->m_nSub, m_pCurrentStyle->m_bFixedPitchFont );

	CHTMLFormInput *pInput = new CHTMLFormInput( m_pDocument->GetFontDefIndex( def ) );

	pInput->m_uObjectID = m_pMasterDocument->GetNextItemID();

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pType:
			pInput->SetFormType( strParam );

			if( pInput->GetFormType() == CHTMLFormInput::knReset && pInput->m_strValue.GetLength() == 0 )
			{
				pInput->m_strValue = _T("Reset");
			}
			
			if( pInput->GetFormType() == CHTMLFormInput::knSubmit && pInput->m_strValue.GetLength() == 0 )
			{
				pInput->m_strValue = _T("Submit");
			}
			break;

		case pName:
			pInput->m_strName.Set( strParam, strParam.GetLength() );
			break;

		case pValue:
			pInput->m_strValue.Set( strParam, strParam.GetLength() );
			break;

		case pSize:
			pInput->m_nCols = GetNumberParameterPercent( strParam, pInput->m_nCols );
			break;

		case pMaxLength:
			pInput->m_uMaxLength = GetNumberParameter( strParam, pInput->m_uMaxLength );
			break;

		case pChecked:
			pInput->m_bChecked = true;
			break;

		case pSrc:
			pInput->m_strImageSrc.Set( strParam, strParam.GetLength() );
			break;

		case pReadonly:
			pInput->m_bReadonly = true;
			break;

		case pDisabled:
			pInput->m_bDisabled = true;
			break;

		case pID:
			pInput->m_strID.Set( strParam, strParam.GetLength() );
			pInput->SetElementID( pInput->m_strID );
			break;
		}
	}

	if( m_stkForm.GetSize() && m_stkForm.Top() )
	{
		m_stkForm.Top()->AddItem( pInput );
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	pPara->AddItem( pInput );
}


void CHTMLParse::OnGotTextArea( const CParameters &pList )
{
	CreateNewTextObject();
	m_pCurrentStyle->m_bFixedPitchFont = true;

	HTMLFontDef def( m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_nSup, m_pCurrentStyle->m_nSub, m_pCurrentStyle->m_bFixedPitchFont );
	CHTMLTextArea *pTextArea = new CHTMLTextArea( m_pDocument->GetFontDefIndex( def ) );

	pTextArea->m_uObjectID = m_pMasterDocument->GetNextItemID();
	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pRows:
			pTextArea->m_uRows = GetNumberParameter( strParam, pTextArea->m_uRows );
			break;

		case pCols:
			pTextArea->m_nCols = GetNumberParameterPercent( strParam, pTextArea->m_nCols );
			break;

		case pReadonly:
			pTextArea->m_bReadonly = true;
			break;

		case pDisabled:
			pTextArea->m_bDisabled = true;
			break;

		case pID:
			pTextArea->m_strID.Set( strParam, strParam.GetLength() );
			pTextArea->SetElementID( pTextArea->m_strID );
			break;

		case pName:
			pTextArea->m_strName.Set( strParam, strParam.GetLength() );
			break;
		}
	}

	m_pTextArea = pTextArea;
	if( m_stkForm.GetSize() && m_stkForm.Top() )
	{
		m_stkForm.Top()->AddItem( pTextArea );
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	pPara->AddItem( pTextArea );
}


void CHTMLParse::OnGotEndTextArea()
{
	if( m_pTextArea )
	{
		m_strToken.Add( 0 );

		m_pTextArea->m_strValue = m_strToken.GetData();
		m_pTextArea = NULL;
		m_strToken.SetSize( 0 );
	}
}


void CHTMLParse::OnGotSelect( const CParameters &pList )
{
	CreateNewTextObject();
	m_pCurrentStyle->m_bFixedPitchFont = true;

	HTMLFontDef def( m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_nSup, m_pCurrentStyle->m_nSub, m_pCurrentStyle->m_bFixedPitchFont );

	m_pCurrentSelect = new CHTMLSelect( m_pDocument->GetFontDefIndex( def ) );

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;
		switch( pList[n].m_param )
		{
		case pName:
			m_pCurrentSelect->m_strName.Set( strParam, strParam.GetLength() );
			break;

		case pSize:
			m_pCurrentSelect->m_uSize = GetNumberParameter( strParam, m_pCurrentSelect->m_uSize );
			break;

		case pDisabled:
			m_pCurrentSelect->m_bDisabled = true;
			break;

		case pMultiple:
			m_pCurrentSelect->m_bMultiple = true;
			break;

		case pID:
			m_pCurrentSelect->m_strID.Set( strParam, strParam.GetLength() );
			m_pCurrentSelect->SetElementID( m_pCurrentSelect->m_strID );
			break;
		}
	}
	m_pCurrentSelect->m_uObjectID = m_pMasterDocument->GetNextItemID();


	if( m_stkForm.GetSize() && m_stkForm.Top() )
	{
		m_stkForm.Top()->AddItem( m_pCurrentSelect );
	}
	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	pPara->AddItem( m_pCurrentSelect );
}


void CHTMLParse::OnGotEndSelect()
{
	if( m_pCurrentSelect )
	{
		m_pCurrentSelect = NULL;
	}
}


void CHTMLParse::OnGotOption( const CParameters &pList )
{
	//
	//	If there's no select then we create one
	if( !m_pCurrentSelect )
	{
		// 
		// The commented declaration below was hiding the function parameter pList resulting in
		// waring C4457.  In this case I believe it was a mistake, so I've commented the line
		// David C. Partridge 6 Mar 2018
		//
		// CParameters pList; Was raising warning C4457 declaration hide function parameter
		OnGotSelect( pList );
	}

	if( m_pCurrentSelect )
	{
		m_pCurrentSelect->ClearCurrentOption();
		CHTMLOption *pOption = new CHTMLOption;

		const size_t uParamSize = pList.GetSize();
		for( size_t n = 0; n < uParamSize; n++ )
		{
			const CStaticString &strParam = pList[n].m_strValue;
			switch( pList[n].m_param )
			{
			case pValue:
				strParam.TrimBoth();
				pOption->m_strValue.Set( strParam, strParam.GetLength() );
				break;

			case pLabel:
				strParam.TrimBoth();
				pOption->m_strLabel.Set( strParam, strParam.GetLength() );
				break;

			case pSelected:
				pOption->m_bSelected = true;
				break;

			case pID:
				pOption->m_strID.Set( strParam, strParam.GetLength() );
				//pOption->SetElementID( pOption->m_strID );
				break;
			}
		}

		m_pCurrentSelect->AddItem( pOption );
	}
}


void CHTMLParse::OnGotEndOption()
{
	if( m_pCurrentSelect )
	{
		CHTMLOption *pOption = m_pCurrentSelect->GetCurrentOption();
		pOption->m_strText.Set( m_strToken.GetData(), m_strToken.GetSize() );
		pOption->m_strText.TrimBoth();
		if( !pOption->m_strValue.GetLength() )
			pOption->m_strValue = pOption->m_strText;

		(void)m_strToken.SetSize( 0 );
		m_pCurrentSelect->ClearCurrentOption();
	}
}


#ifdef QHTM_WINDOWS_CONTROL
void CHTMLParse::OnGotControl( const CParameters &pList )
{
	CreateNewTextObject();
	m_pCurrentStyle->m_bFixedPitchFont = true;

	HTMLFontDef def( m_pCurrentStyle->szFaceName, m_pCurrentStyle->nSize, m_pCurrentStyle->bBold, m_pCurrentStyle->bItalic, m_pCurrentStyle->bUnderline, m_pCurrentStyle->bStrikeThrough, m_pCurrentStyle->m_nSup, m_pCurrentStyle->m_nSub, m_pCurrentStyle->m_bFixedPitchFont );
	CHTMLControl *pControl = new CHTMLControl( m_pDocument->GetFontDefIndex( def ) );

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pWidth:
			pControl->SetWidth( strParam );
			break;


		case pHeight:
			pControl->SetHeight( strParam );
			break;


		case pControlClass:
			pControl->SetControlClass( strParam );
			break;


		case pControlStyle:
			pControl->SetStyle( strParam );
			break;


		case pControlStyleEx:
			pControl->SetStyleEx( strParam );
			break;

		case pID:
			pControl->SetID( strParam );
			pControl->SetElementID( StringClass( strParam, strParam.GetLength() ) );
			break;
		}
	}

	CHTMLParagraph *pPara = m_pDocument->CurrentParagraph();
	pPara->AddItem( pControl );
}
#endif	//	QHTM_WINDOWS_CONTROL


bool CHTMLParse::OnGetIncludeFile( LPCTSTR pcszFilename, LPTSTR &pcszContent, UINT &uLength )
{
//	, m_hInstLoadedFrom( hInstLoadedFrom )
//	, m_pcszFilePath( pcszFilePath )

	//
	//	Try from resources
	if( !_tcsnicmp( _T("RES:"), pcszFilename, 4 ) )
	{
		CResourceLoader rsrc( m_hInstLoadedFrom );

		if( rsrc.Load( pcszFilename + 4, RT_RCDATA ) || rsrc.Load( pcszFilename + 4, RT_HTML ) )
		{
			pcszContent = new TCHAR[ rsrc.GetSize() + 1 ];
#ifdef _UNICODE
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)rsrc.GetData(), rsrc.GetSize(), pcszContent, rsrc.GetSize() * sizeof( TCHAR ) );
#else	//	_UNICODE
			_tcsncpy( pcszContent, (LPCSTR)rsrc.GetData(), rsrc.GetSize() );
#endif	//	_UNICODE
			uLength = rsrc.GetSize();
			return true;
		}		
	}
	else
	{
		extern bool LoadTextFile( LPCTSTR pcszFilename, LPTSTR &pszBuffer, UINT &uLength );
		if( LoadTextFile( pcszFilename, pcszContent, uLength ) )
		{
			return true;
		}
		else
		{
			StringClass strImageName( m_pcszFilePath );
			strImageName += pcszFilename ;
			if( LoadTextFile( pcszFilename, pcszContent, uLength ) )
			{
				return true;
			}			
		}
		
	}

	return false;
}


void CHTMLParse::OnGetInlineStyle( const CStaticString &strStyle )
{
	CStyles styles;
	//TRACE( _T("Style: %s\n"), (LPCTSTR)CSimpleString( strStyle, strStyle.GetLength() ) );

	ParseStyles( strStyle, styles );

	const size_t uStyleCount = styles.GetSize();
	for( size_t n = 0; n < uStyleCount; n++ )
	{
		const CStaticString &strParam = styles[n].m_strValue;

		switch( styles[n].m_style )
		{
		case sFontWeight:
			break;

		case sFontsize:
			m_pCurrentStyle->nSize = GetFontSize( strParam, m_pCurrentStyle->nSize );
			if( m_pCurrentStyle->nSize > 7 )
			{
				m_pCurrentStyle->nSize = 7;
			}
			break;

		case sFontFamily:
			GetFontName( m_pCurrentStyle->szFaceName, countof( m_pCurrentStyle->szFaceName ), strParam );
			m_pCurrentStyle->m_bFixedPitchFont = IsFontFixed( m_pCurrentStyle->szFaceName, m_pDocument->m_cCharSet );
			break;

		case sColor:
			m_pCurrentStyle->m_crFore = GetColourFromString( strParam, m_pCurrentStyle->m_crFore );
			break;

		case sBackgroundColor:
			m_pCurrentStyle->m_crBack = GetColourFromString( strParam, m_pCurrentStyle->m_crFore );
			break;
		}
	}
}


void CHTMLParse::OnGotSpan( const CParameters &pList )
{
	CreateNewTextObject();
	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pStyle:
			CreateNewStyle();
			OnGetInlineStyle( strParam );
			break;
		}
	}	
}


void CHTMLParse::OnGotAcronym( const CParameters &pList )
{
	CreateNewTextObject();

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pTitle:
			CreateNewStyle();
			m_pCurrentStyle->m_strTitle.Set( strParam, strParam.GetLength() );
			break;
		}
	}	
}


void CHTMLParse::OnGotLabel( const CParameters &pList )
{
	CreateNewTextObject();
	CreateNewStyle();

	const size_t uParamSize = pList.GetSize();
	for( size_t n = 0; n < uParamSize; n++ )
	{
		const CStaticString &strParam = pList[n].m_strValue;

		switch( pList[n].m_param )
		{
		case pFor:
			m_pCurrentStyle->m_strActivationTarget.Set( strParam, strParam.GetLength() );
			break;
		}
	}	
}