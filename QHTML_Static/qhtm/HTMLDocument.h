/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLDocument.h
Owner:	russf@gipsysoft.com
Purpose:	The main document object.
----------------------------------------------------------------------*/
#ifndef HTMLDOCUMENT_H
#define HTMLDOCUMENT_H

#ifndef BACKGROUND_H
	#include "Background.h"
#endif	//	BACKGROUND_H

class CHTMLDocumentObject;
class CHTMLParagraph;
class CQHTMImageABC;
class CDefaults;


class CStyle
{
public:
	//	REVIEW - os specific stuff
	#ifdef WIN32
		#define MAX_FACE_NAME	LF_FACESIZE
	#endif	//	_WINDOWS_

	CStyle()
		: m_bFixedPitchFont( false )
		, nSize( 0 )
		, bBold( false )
		, bItalic( false )
		, bStrikeThrough( false )
		, bUnderline( false )
		, m_nSub( 0 )
		, m_nSup( 0 )
		, m_nCentered( 0 )
		, nAlignment( algEmpty )
		{
			szFaceName[ 0 ] = 0;
		}

	CStyle( const CStyle &rhs )
		{
			m_crFore = rhs.m_crFore;
			m_crBack = rhs.m_crBack;
			_tcscpy( szFaceName, rhs.szFaceName );
			m_bFixedPitchFont = rhs.m_bFixedPitchFont;
			nSize = rhs.nSize;
			bBold = rhs.bBold;
			bItalic = rhs.bItalic;
			bStrikeThrough = rhs.bStrikeThrough;
			bUnderline = rhs.bUnderline;
			m_nSub = rhs.m_nSub;
			m_nSup = rhs.m_nSup;
			m_nCentered = rhs.m_nCentered;
			nAlignment = rhs.nAlignment;
			m_strTitle = rhs.m_strTitle;
		}
	CColor m_crFore;
	CColor m_crBack;

	TCHAR szFaceName[ MAX_FACE_NAME ];
	bool m_bFixedPitchFont;
	int nSize;			//	Logical size not pixels
	bool bBold;
	bool bItalic;
	bool bStrikeThrough;
	bool bUnderline;
	int m_nSub;
	int m_nSup;
	int m_nCentered;

	enum Align { algEmpty, algLeft, algCentre, algRight, algTop, algMiddle, algBottom, algJustify };

	Align nAlignment;

	StringClass m_strTitle;
	StringClass m_strActivationTarget;
};


class CHTMLDocument		//	doc
//
//	Main HTML document
{
public:
	explicit CHTMLDocument( CDefaults *pDefaults );
	virtual ~CHTMLDocument();

	//	Hack alert - to workaround problem with data stored in tables which should not be (ideally).
	void ResetMeasuringKludge();

	void AddItem( CHTMLDocumentObject *pdocobj );
	void AddParagraph( CHTMLParagraph *pPara );
	CHTMLParagraph *CurrentParagraph() const;
	
	const HTMLFontDef * GetFontDefIndex( const HTMLFontDef &def );

	//
	//	This is the buffer used by all text for this document
	//	Items don't get deleted from it, instead new items are added to the
	//	end and old items remain unused.
	ArrayOfChar m_arrTextBuffer;

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

	CBackground m_back;

	COLORREF m_crLink;
	COLORREF m_crLinkHover;
	int m_nLeftMargin;
	int m_nTopMargin;
	int m_nRightMargin;
	int m_nBottomMargin;
	
	ArrayClass<CHTMLDocumentObject*> m_arrItems;

	// A Map which stores the images loaded for this document
	MapClass<StringClass, CQHTMImageABC*> m_mapImages;

	StringClass m_strTitle;
	CQHTMImageABC *m_pimgBackground;

	ArrayClass<HTMLFontDef*> m_arrFontDefs;
	MapClass<HTMLFontDef, size_t> m_mapFonts;

	ArrayClass< class CHTMLForm * > m_arrForms;

	BYTE m_cCharSet;
	CDefaults *m_pDefaults;
	UINT GetNextItemID() { return m_uCurrentItemID++; }

private:
	CHTMLParagraph *m_pCurrentPara;
	UINT m_uCurrentItemID;

private:
	CHTMLDocument();
	CHTMLDocument( const CHTMLDocument &);
	CHTMLDocument& operator =( const CHTMLDocument &);
};

//
//	Document object includes....

#ifndef HTMLDOCUMENTOBJECT_H
	#include "HTMLDocumentObject.h"
#endif	//	HTMLDOCUMENTOBJECT_H

#ifndef HTMLPARAGRAPH_H
	#include "HTMLParagraph.h"
#endif	//	HTMLPARAGRAPH_H

#ifndef HTMLPARAGRAPHOBJECT_H
	#include "HTMLParagraphObject.h"
#endif	//	HTMLPARAGRAPHOBJECT_H

#ifndef HTMLBLOCKQUOTE_H
	#include "HTMLBlockQuote.h"
#endif	//	HTMLBLOCKQUOTE_H

#ifndef HTMLLIST_H
	#include "HTMLList.h"
#endif	//	HTMLLIST_H

#ifndef HTMLLISTITEM_H
	#include "HTMLListItem.h"
#endif	//	HTMLLISTITEM_H

#ifndef HTMLTABLE_H
	#include "HTMLTable.h"
#endif	//HTMLTABLE_H

#ifndef HTMLTABLECELL_H
	#include "HTMLTableCell.h"
#endif	//	HTMLTABLECELL_H

#ifndef HTMLTEXTBLOCK_H
	#include "HTMLTextBlock.h"
#endif	//	HTMLTEXTBLOCK_H

#ifndef HTMLANCHOR_H
	#include "HTMLAnchor.h"
#endif	//	HTMLANCHOR_H

#ifndef HTMLHORIZONTALRULE_H
	#include "HTMLHorizontalRule.h"
#endif	//	HTMLHORIZONTALRULE_H

#ifndef HTMLIMAGE_H
	#include "HTMLImage.h"
#endif	//	HTMLIMAGE_H

#ifndef HTMLBREAK_H
	#include "HTMLBreak.h"
#endif	//	HTMLBREAK_H

#ifndef HTMLDEFINITIONLIST_H
	#include "HTMLDefinitionList.h"
#endif	//	HTMLDEFINITIONLIST_H

#ifndef HTMLDEFINITIONLISTITEM_H
	#include "HTMLDefinitionListItem.h"
#endif	//	HTMLDEFINITIONLISTITEM_H

#ifndef HTMLFORM_H
	#include "HTMLForm.h"
#endif	//	 HTMLFORM_H

#ifndef HTMLFORMINPUT_H
	#include "HTMLFormInput.h"
#endif	//	 HTMLFORMINPUT_H

#ifndef HTMLTEXTAREA_H
	#include "HTMLTextArea.h"
#endif	//	 HTMLTEXTAREA_H

#ifndef HTMLSELECT_H
	#include "HTMLSelect.h"
#endif	//	 HTMLSELECT_H

#ifndef HTMLOPTION_H
	#include "HTMLOption.h"
#endif	//	 HTMLOPTION_H

#ifndef HTMLCONTROL_H
	#include "HTMLControl.h"
#endif	//	 HTMLCONTROL_H


#endif //HTMLDOCUMENT_H