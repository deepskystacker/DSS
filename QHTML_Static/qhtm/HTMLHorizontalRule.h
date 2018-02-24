/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLHorizontalRule.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLHORIZONTALRULE_H
#define HTMLHORIZONTALRULE_H

class CHTMLHorizontalRule : public CHTMLParagraphObject					//	hr
//
//	Horizontal rule.
{
public:
	CHTMLHorizontalRule( CStyle::Align alg, int nSize, int nWidth, bool bNoShade, COLORREF crColor );

	CStyle::Align m_alg;
	int m_nSize;
	int m_nWidth;
	bool m_bNoShade;
	COLORREF m_crColor;

	void AddDisplayElements( class CHTMLSectionCreator *psc );

private:
#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLHorizontalRule();
	CHTMLHorizontalRule( const CHTMLHorizontalRule &);
	CHTMLHorizontalRule& operator =( const CHTMLHorizontalRule &);
};


#endif //HTMLHORIZONTALRULE_H