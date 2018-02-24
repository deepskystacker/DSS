/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLTableSection.h
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	A Section for displaying tables.
----------------------------------------------------------------------*/
#ifndef HTMLTABLESECTION_H
#define HTMLTABLESECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef BACKGROUND_H
	#include "Background.h"
#endif	//	BACKGROUND_H

class CHTMLTableSection : public CHTMLSectionABC
{
public:
	CHTMLTableSection( CHTMLSection * pParent, int border, COLORREF crBorderDark, COLORREF crBorderLight, CBackground &grad );
	virtual ~CHTMLTableSection();

	virtual void OnDraw( GS::CDrawContext &dc );
	
	//	Fix for table background making links in the table not get any mouse input
	virtual bool IsPointInSection( const WinHelper::CPoint & ) const { return false; }

private:
	CBackground m_back;
	COLORREF	m_crBorderDark;
	COLORREF	m_crBorderLight;
	int			m_nBorder;

private:
	CHTMLTableSection();
	CHTMLTableSection( const CHTMLTableSection & );
	CHTMLTableSection& operator =( const CHTMLTableSection & );
};


#endif //HTMLTABLESECTION_H
