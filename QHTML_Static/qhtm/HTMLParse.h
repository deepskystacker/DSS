/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	htmlparse.h
Owner:	russf@gipsysoft.com
Purpose:	HTML parser, it will interpret the HTML and create HTML display engine
					The engine will then be used to layout and display to a window
					display context.
----------------------------------------------------------------------*/
#ifndef HTMLPARSE_H
#define HTMLPARSE_H

#ifndef QHTM_TYPES_H
	#include "QHTM_Types.h"
#endif	//	QHTM_TYPES_H

#ifndef HTMLPARSEBASE_H
	#include "HTMLParseBase.h"
#endif	//	HTMLPARSEBASE_H

#ifndef SIMPLESTRING_H
	#include "SimpleString.h"
#endif	//	SIMPLESTRING_H

#ifndef HTMLFONTDEF_H
	#include "HTMLFontDef.h"
#endif	//	HTMLFONTDEF_H

#ifndef QHTM_H
	#include "QHTM.h"
#endif	//	QHTM_H

#ifndef HTMLDOCUMENT_H
	#include "HTMLDocument.h"
#endif	//	HTMLDOCUMENT_H

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

class CHTMLParagraphObject;
class CHTMLTable;
class CHTMLAnchor;
class CHTMLList;
class CHTMLDefinitionList;
class CHTMLForm;
class CDefaults;
class CQHTMImageABC;

class CHTMLParse : public CHTMLParseBase
//
//	HTML Parser
{
public:
	explicit CHTMLParse( LPCTSTR pcszStream, size_t uLength, HINSTANCE hInstLoadedFrom, LPCTSTR pcszFilePath, CDefaults *pDefaults );
	virtual ~CHTMLParse();

	CHTMLDocument * Parse( CHTMLDocument *pdocOwner = NULL );

protected:
	virtual void OnGotTag( const Token token, const CParameters &arrParameter );
	virtual void OnGotEndTag( const Token token );
	virtual void OnGotText( TCHAR ch );
	virtual void OnEndDoc();

	class CQHTMImageABC *OnLoadImage( LPCTSTR pcszFilename, bool bIsTransparent, COLORREF crForceTransparent );

private:
	void OnGotBreak( const CParameters &pList );
	void OnGotBody( const CParameters &pList );
	void OnGotImage( const CParameters &pList );
	void OnGotHR( const CParameters &pList );
	void OnGotParagraph( const CParameters &pList );
	void OnGotFont( const CParameters &pList );
	void OnGotAnchor( const CParameters &pList );

	void OnGotTable( const CParameters &pList );
	void OnGotTableRow( const CParameters &pList );
	void OnGotTableCell( const CParameters &pList );

	void OnGotEndTableCell();
	void OnGotEndTableRow();
	void OnGotEndAnchor();

	void OnGotUnorderedList( const CParameters &pList );
	void OnGotOrderedList( const CParameters &pList );
	void OnGotListItem( const CParameters &pList );
	void CreateList( bool bOrdered, const CParameters & pList );

	void OnGotEndListItem();
	void OnGotEndList();	// Both ordered & unordered

	void OnGotBlockQuote( const CParameters &pList );
	void OnGotEndBlockQuote();

	void OnGotAddress( const CParameters &pList );
	void OnGotEndAddress();
	
	void CleanupParse();

	void OnGotHeading( const Token token, const CParameters &pList );
	void OnGotMeta( const CParameters &pList );

	void OnGotDefinitionList();
	void OnGotDefinitionItem( bool bTerm );
	void OnGotEndDefinitionList();
	
	void OnGotForm( const CParameters &pList );
	void OnGotEndForm();
	void OnGotFormInput( const CParameters &pList );
	void OnGotTextArea( const CParameters &pList );
	void OnGotEndTextArea();
	void OnGotSelect( const CParameters &pList );
	void OnGotEndSelect();
	void OnGotOption( const CParameters &pList );
	void OnGotEndOption();
#ifdef QHTM_WINDOWS_CONTROL
	void OnGotControl( const CParameters &pList );
#endif	//	QHTM_WINDOWS_CONTROL

	void CreateNewParagraph( int nLinesAbove, int nLinesBelow, CStyle::Align alg );
	void CreateNewTextObject();
	void CreateNewStyle();
	void PopPreviousStyle();
	void UpdateItemLinkStatus( CHTMLParagraphObject *pItem );
	void GetFontName( LPTSTR pszBuffer, int nBufferSize, const CStaticString &srFontNameSpec );
	bool OnGetIncludeFile( LPCTSTR pcszFilename, LPTSTR &pcszContent, UINT &uLength );

	void OnGotSpan( const CParameters &pList );
	void OnGotAcronym( const CParameters &pList );
	void OnGotLabel( const CParameters &pList );
	void OnGetInlineStyle( const CStaticString &strStyle );

	ArrayOfChar m_strToken;

	CStyle *m_pCurrentStyle;
	StackClass<CStyle*> m_stkStyle;

	CHTMLDocument *m_pDocument;
	StackClass<CHTMLDocument *> m_stkDocument;
	CHTMLDocument *m_pMasterDocument;		// Top Most Doc

	CHTMLAnchor	*m_pLastAnchor;
	
	StackClass<CHTMLTable *> m_stkTable;
	StackClass<bool> m_stkInTableCell;

	StackClass<CHTMLList *> m_stkList;
	StackClass<CHTMLDefinitionList *> m_stkDefinitionList;
	StackClass<CHTMLForm *> m_stkForm;
	class CHTMLTextArea *m_pTextArea;
	class CHTMLSelect *m_pCurrentSelect;
	
	HINSTANCE m_hInstLoadedFrom;
	LPCTSTR m_pcszFilePath;

	CDefaults *m_pDefaults;
	
	bool m_bInStyleSheet;

private:
	CHTMLParse();
	CHTMLParse( const CHTMLParse & );
	CHTMLParse & operator =( const CHTMLParse & );
};

#ifndef HTMLDOCUMENT_H
	#include "HTMLDocument.h"
#endif	//	HTMLDOCUMENT_H

////////////////////////////////////////////////////////////////////////
// Utility Methods
////////////////////////////////////////////////////////////////////////
//
//	Free fucntion to get the descriptive text from an alignment value
LPCTSTR GetStringFromAlignment( CStyle::Align alg );

//
//	Free fucntion to get the alignment value from descriptive text
CStyle::Align GetAlignmentFromString( const CStaticString &str, CStyle::Align algDefault );

//
//	Get a parameter as a number, can be positive to reflect pixels
//	or negative to represent a percentage.
int GetNumberParameterPercent( const CStaticString &strParam, int nDefault );
int GetNumberParameter( const CStaticString &strParam, int nDefault );
int GetFontSize( const CStaticString &strParam, int nDefault );

//
//	Get the font size as pixels. Basically converts the HTML logical font sizes to pixels.
//	If the nSize passed is negative then it will assume the number is a point size request
int GetFontSizeAsPixels( HDC hdc, int nSize, UINT nZoomLevel );

#endif //HTMLPARSE_H
