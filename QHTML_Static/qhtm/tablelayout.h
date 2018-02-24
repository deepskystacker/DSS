/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	tablelayout.cpp
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	Lays out the cells of a table
----------------------------------------------------------------------*/
#ifndef HTMLTABLELAYOUT_H
#define HTMLTABLELAYOUT_H

#ifndef HTMLPARSE_H
	#include "htmlparse.h"
#endif	//	HTMLPARSE_H

// Object used to lay out a table
class CHTMLTableLayout
{
public:
	CHTMLTableLayout( CHTMLTable* ptab, CHTMLSectionCreator *psc );

	int GetTableWidth() const { return m_nTableWidth; }
	int GetColumnWidth(int nCol ) 
	{
		ASSERT(nCol >= 0 && nCol < m_nCols); 
		return m_pTab->m_arrLayoutWidth[nCol]; 
	}

	int GetColumnCount() const { return m_nCols; }

private:
	//	Method is used to measure the contents of a document
	WinHelper::CSize MeasureDocument(CHTMLDocument* pdoc, int nWidth);

	//	Determint extrema for a cell
	void MeasureCell(CHTMLTableCell* pCell, size_t nCol);
	void MeasureCells();
	bool LayoutMaximum();
	void GetDesiredWidths();
	int CalculateTableSize();
	int GetSpaceNeeded();
	void LayoutAllSpace(int nSpaceAvailable, int nSpaceNeeded);
	void LayoutSpaceProportional(int nSpaceAvailable, int nSpaceNeeded);
	void Layout();

	
	CHTMLTable*	m_pTab;
	int	m_nMaxWidth;
	GS::CDrawContext& m_dc;
	int m_nCols, m_nRows;
	int m_nMinTableWidth, m_nMaxTableWidth;
	int m_nTableWidth;
	int m_nZoomLevel;
	CHTMLSectionCreator *m_pscParent;

private:
	CHTMLTableLayout();
	CHTMLTableLayout( const CHTMLTableLayout& );
	CHTMLTableLayout& operator =( const CHTMLTableLayout& );

};
	
#endif
