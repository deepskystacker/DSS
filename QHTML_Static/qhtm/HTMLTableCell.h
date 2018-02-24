/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLTableCell.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLTABLECELL_H
#define HTMLTABLECELL_H

#ifndef BACKGROUND_H
	#include "Background.h"
#endif	//	BACKGROUND_H

class CHTMLTableCell : public CHTMLDocument			//	cell
//
//	Table cell.
//	Is a document.
{
public:
	CHTMLTableCell( CDefaults *pDefaults, int nWidth, int nHeight, bool bNoWrap, CBackground &grad, COLORREF crDark, COLORREF crLight, CStyle::Align valg, int nColSpan );

	int m_nWidth;
	int m_nHeight;
	bool m_bNoWrap;
	COLORREF m_crBorderLight;
	COLORREF m_crBorderDark;

	CStyle::Align	m_valg;

	int m_nColSpan;
	
#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG
	
private:
	CHTMLTableCell();
	CHTMLTableCell( const CHTMLTableCell &);
	CHTMLTableCell& operator =( const CHTMLTableCell &);
};

#endif //HTMLTABLECELL_H