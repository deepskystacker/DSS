/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLHorizontalRuleSection.h
Owner:	russf@gipsysoft.com
Purpose:	Horizontal rule HTML section.
----------------------------------------------------------------------*/
#ifndef HTMLHORIZONTALRULESECTION_H
#define HTMLHORIZONTALRULESECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H


class CHTMLHorizontalRuleSection : public CHTMLSectionABC
{
public:
	CHTMLHorizontalRuleSection( CHTMLSection *pSectParent, bool bNoShade, COLORREF crColor );
	virtual ~CHTMLHorizontalRuleSection();
	virtual void OnDraw( GS::CDrawContext &dc );

private:
	bool m_bNoShade;
	COLORREF m_crColor;
private:
	CHTMLHorizontalRuleSection();
	CHTMLHorizontalRuleSection( const CHTMLHorizontalRuleSection & );
	CHTMLHorizontalRuleSection& operator = ( const CHTMLHorizontalRuleSection & );
};

#endif //HTMLHORIZONTALRULESECTION_H