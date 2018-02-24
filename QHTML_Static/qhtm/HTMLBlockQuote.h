/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLBlockQuote.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLBLOCKQUOTE_H
#define HTMLBLOCKQUOTE_H

#ifndef HTMLPARAGRAPHOBJECT_H
	#include "HTMLParagraphObject.h"
#endif	//	HTMLPARAGRAPHOBJECT_H

class CHTMLDocument;

class CHTMLBlockQuote : public CHTMLParagraphObject
// A BlockQuote. Contains a single sub document
{
public:
	explicit CHTMLBlockQuote( CDefaults *pDefaults );
	~CHTMLBlockQuote();

	CHTMLDocument*	m_pDoc;

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLBlockQuote();
	CHTMLBlockQuote( const CHTMLBlockQuote &);
	CHTMLBlockQuote& operator =( const CHTMLBlockQuote &);
};

#endif //HTMLBLOCKQUOTE_H