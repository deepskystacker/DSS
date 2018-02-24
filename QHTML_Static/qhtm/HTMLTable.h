/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLTable.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLTABLE_H
#define HTMLTABLE_H

class CHTMLTableCell;

#ifndef BACKGROUND_H
	#include "Background.h"
#endif	//	BACKGROUND_H

class CHTMLTableLayout;
class CHTMLSectionCreator;

class CHTMLTable : public CHTMLParagraphObject			//	tab
//
//	Table.
//	Can have many rows, each row can have many columns
{
public:
	CHTMLTable(	int nWidth, int nHeight, int nBorder, CStyle::Align alg, CStyle::Align valg, int nCellSpacing, int nCellPadding, CBackground &background, COLORREF crDark, COLORREF crLight);

	~CHTMLTable();

	//	Add a cell to the current row.
	void AddCell( CHTMLTableCell *pCell );

	//	Create a new row.
	void NewRow( CStyle::Align valg );

	//	Get the dimensions of the table in rows and columns
	WinHelper::CSize GetRowsCols() const;
	
	//	Hack alert - to workaround problem with data stored in tables which should not be (ideally).
	virtual void ResetMeasuringKludge();

	virtual void AddDisplayElements( CHTMLSectionCreator *psc );

	class CHTMLTableRow				//	row
	{
	public:
		explicit CHTMLTableRow( CStyle::Align valg );
		~CHTMLTableRow();
#ifdef _DEBUG
		void Dump() const;
#endif	//	_DEBUG
		ArrayClass< CHTMLTableCell* > m_arrCells;
		CStyle::Align	m_valg;
		CBackground m_back;
		int m_nHeight;

	private:
		CHTMLTableRow();
		CHTMLTableRow( const CHTMLTableRow &);
		CHTMLTableRow& operator = ( const CHTMLTableRow &);
	};
	ArrayClass< CHTMLTableRow * > m_arrRows;

	int GetColumnWidth( int nColumns, int nCol, CHTMLTableLayout &layout, CHTMLSectionCreator *psc );

	CHTMLTableRow	*GetCurrentRow() { return m_pCurrentRow; }

	int m_nWidth, m_nHeight;
	int m_nBorder;
	CStyle::Align	m_alg;
	CStyle::Align	m_valg;
	COLORREF m_crBorderLight;
	COLORREF m_crBorderDark;
	CBackground m_back;

	//COLORREF m_crBgColor;
	int	m_nCellSpacing;
	int m_nCellPadding;

	//  Data used to layout the table and it's contents. It is referenced
	//  primarily by CHTMLSectionCreator and CHTMLTableLayout.
	bool m_bCellsMeasured;	// Have the cells been measured?
	ArrayOfInt m_arrDesiredWidth;	// Desired width for column
	ArrayOfInt m_arrMinimumWidth;	// Minimum width for column
	ArrayOfInt m_arrMaximumWidth;	// Maximum width for column
	ArrayOfBool m_arrNoWrap;			// NoWrap status for column
	ArrayOfInt m_arrLayoutWidth;	// Current layout width for column

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLTableRow	*m_pCurrentRow;

private:
	CHTMLTable();
	CHTMLTable( const CHTMLTable &);
	CHTMLTable& operator =( const CHTMLTable &);
};


#endif //HTMLTABLE_H