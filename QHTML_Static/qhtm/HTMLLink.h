/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLLink.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLLINK_H
#define HTMLLINK_H

class CHTMLLink : public CHTMLTextBlock					//	lnk
//
//	Hyperlink
//	A basic text block with a link target.
{
public:
	CHTMLLink();

private:
	CHTMLParagraph *m_pPara;

private:
	CHTMLLink( const CHTMLLink &);
	CHTMLLink& operator =( const CHTMLLink &);
};

#endif //HTMLLINK_H