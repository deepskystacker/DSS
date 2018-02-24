/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLParagraph.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLPARAGRAPH_H
#define HTMLPARAGRAPH_H

#ifndef HTMLDOCUMENTOBJECT_H
	#include "HTMLDocumentObject.h"
#endif	//	HTMLDOCUMENTOBJECT_H

class CHTMLParagraphObject;

class CHTMLParagraph : public CHTMLDocumentObject				//	para
//
//	Paragraph.
//	Contains a list of images, text blocks, links and link targets.
{
public:
	CHTMLParagraph( int nSpaceAbove, int nSpaceBelow, CStyle::Align alg );
	~CHTMLParagraph();

	void AddItem( CHTMLParagraphObject *pItem );
	bool IsEmpty() const;
	void Reset( int nSpaceAbove, int nSpaceBelow, CStyle::Align alg );

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

	void ResetMeasuringKludge();

	int m_nSpaceAbove, m_nSpaceBelow;
	CStyle::Align m_alg;

	ArrayClass<CHTMLParagraphObject*> m_arrItems;

private:
	CHTMLParagraph();
	CHTMLParagraph( const CHTMLParagraph &);
	CHTMLParagraph& operator =( const CHTMLParagraph &);
};


#endif //HTMLPARAGRAPH_H