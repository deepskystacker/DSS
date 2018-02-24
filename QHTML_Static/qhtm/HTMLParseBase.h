/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLParseBase.h
Owner:	russf@gipsysoft.com
Purpose:	HTML parser base class
					At it's most basic it will fire OnGotText, OnGotTag and OnGotEndTag
					events, you must derived from this class to get these events. All
					plain text goes to OnGotText and all tags go to OnGotTag and
					OnGotEndTag. Withinthe tag is also any attributes the tag may have.
----------------------------------------------------------------------*/
#ifndef HTMLPARSEBASE_H
#define HTMLPARSEBASE_H

#ifndef STATICSTRING_H
	#include "StaticString.h"
#endif	//	STATICSTRING_H

#ifndef ArrayClass
	#include <reuse/array.h>
	#define ArrayClass			Container::CArray
#endif	//	ArrayClass

#ifndef STACK_H
	#include <reuse/Stack.h>
#endif	//	STACK_H

class CHTMLParseBase
{
public:

	CHTMLParseBase( LPCTSTR pcszStream, size_t uLength );
	virtual ~CHTMLParseBase();

	enum Token
	{
		tokIgnore
		,tokBody
		,tokFont
		,tokBold
		,tokUnderline
		,tokItalic
		,tokStrikeout
		,tokImage
		,tokTableDef
		,tokTableRow
		,tokTableHeading
		,tokTableHeadingRow
		,tokTable
		,tokHorizontalRule
		,tokParagraph
		,tokBreak
		,tokAnchor
		,tokH1
		,tokH2
		,tokH3
		,tokH4
		,tokH5
		,tokH6
		,tokListItem
		,tokOrderedList
		,tokUnorderedList
		,tokBlockQuote
		,tokAddress
		,tokDocumentTitle
		,tokCenter
		,tokDiv
		,tokPre
		,tokCode
		,tokKBD
		,tokSAMP
		,tokMeta
		,tokHead
		,tokHTML
		,tokSub
		,tokSup
		,tokStyleSheet
		,tokDocType
		,tokScript
		,tokForm
		,tokLabel
		,tokInput
		,tokSelect
		,tokTextarea
		,tokOptGroup
		,tokOption
		,tokFieldSet
		,tokLegend
		,tokTeletype
		,tokDefinitionList
		,tokDefinitionTerm
		,tokDefinitionDescription
		,tokControl
		,tokVAR
		,tokDFN
		,tokSpan
		,tokACRONYM
		,tokBIG
		,tokSMALL
	};

	enum Param
	{
		pUnknown
		,pBColor
		,pAlign
		,pVAlign
		,pNoShade
		,pSize
		,pWidth
		,pHeight
		,pName
		,pHref
		,pTitle
		,pSrc
		,pColor
		,pFace
		,pAlt
		,pBorder
		,pNoWrap
		,pCellSpacing
		,pCellPadding
		,pBorderColor
		,pBorderColorLight
		,pBorderColorDark
		,pLink
		,pALink
		,pValue
		,pType
		,pMarginTop
		,pMarginBottom
		,pMarginLeft
		,pMarginRight
		,pMarginWidth
		,pMarginHeight
		,pContent
		,pHTTPEquiv
		,pBackground
		,pTransparentColor
		,pAction
		,pMethod
		,pChecked
		,pDisabled
		,pReadonly
		,pMaxLength
		,pTabIndex
		,pID
		,pAccessKey
		,pRows
		,pCols
		,pFor
		,pCompact
		,pMultiple
		,pLabel
		,pSelected
		,pControlClass
		,pControlStyle
		,pControlStyleEx
		,pClear
		,pBColorLeft
		,pBColorRight
		,pBColorTop
		,pBColorBottom
		,pBColorCenter
		,pColSpan
		,pRowSpan
		,pStyle
	};

	static int Initialise();

protected:
	class CParameterPair
	{
	public:
		Param m_param;
		CStaticString m_strValue;
	};
	typedef ArrayClass< CParameterPair > CParameters;

	//	Call this to actually do the parsing
	bool ParseBase();

	//	Called when a tag has been parsed, complete with it's parameter list
	virtual void OnGotTag( const Token token, const CParameters &arrParameter );
	//	Called when an end tag has been parsed
	virtual void OnGotEndTag( const Token token );

	//	Send when a comment is found
	virtual void OnGotComment( LPCTSTR pszText, size_t uLength );

	//	called when some text has been parsed
	virtual void OnGotText( TCHAR ch );

	//	Called when the end of document is reached
	virtual void OnEndDoc();

	//	Are we currently in preformatted mode?
	bool IsPreformatted() const { return m_bPreFormatted; }

	//	Called when the end of document is reached
	virtual bool OnGetIncludeFile( LPCTSTR pcszFilename, LPTSTR &pcszContent, UINT &uLength);
private:
	inline void EatStreamWhitespace();

	bool OnGotTagText( const CStaticString &strTag );

	bool GetTag( CStaticString &strTag );
	bool GetSpecialChar( TCHAR &ch );
	bool AddTokenChar( const TCHAR ch );
	bool GetChar( TCHAR &ch );
	bool GetTagChar( TCHAR &ch );
	bool PeekNextChar( TCHAR &ch ) const;

	bool GetNextBuffer() const;

	mutable bool m_bCurrentBufferDelete;
	mutable LPCTSTR  m_pcszStream;
	mutable LPCTSTR  m_pcszStreamBegin;
	mutable LPCTSTR  m_pcszStreamMax;

	class CBufferStack
	{
	public:
		LPTSTR m_pszBufferStart;
		LPTSTR m_pszAllocatedBegin;
		size_t m_uLength;
		bool m_bDelete;
	};

	mutable Container::CStack< CBufferStack > m_bufferStack;

	bool m_bPreviousCharWasSpace;
	bool m_bPreviousWasSpecial;
	bool m_bPreFormatted;
	bool m_bTextArea;

private:
	CHTMLParseBase();
	CHTMLParseBase( const CHTMLParseBase & );
	CHTMLParseBase& operator =( const CHTMLParseBase & );
};

COLORREF GetColourFromString( const CStaticString &strColour, COLORREF crDefault);

#endif //HTMLPARSEBASE_H
