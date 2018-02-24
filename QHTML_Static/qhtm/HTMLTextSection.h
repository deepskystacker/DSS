/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTextSection.h
Owner:	russf@gipsysoft.com
Purpose:	Simple drawn text object.
----------------------------------------------------------------------*/
#ifndef HTMLTEXTSECTION_H
#define HTMLTEXTSECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef COLOR_H
	#include "Color.h"
#endif	//	COLOR_H

class CHTMLTextSection : public CHTMLSectionABC
{
public:
	CHTMLTextSection( CHTMLSection * pParent, LPCTSTR pcszText, size_t nLength, const GS::FontDef *pfdef, const CColor &crFore, const CColor &crBack );
	virtual ~CHTMLTextSection();

	virtual void OnDraw( GS::CDrawContext &dc );

private:
	StringClass m_str;

	const GS::FontDef *m_pfdef;
	const CColor m_crFore;
	const CColor m_crBack;

private:
	CHTMLTextSection();
	CHTMLTextSection( const CHTMLTextSection & );
	CHTMLTextSection& operator = ( const CHTMLTextSection & );
};

#endif //HTMLTEXTSECTION_H