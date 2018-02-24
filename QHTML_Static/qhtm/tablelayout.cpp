/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	tablelayout.cpp
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	Lays out the cells of a table
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <math.h>
#include "DebugHlp/DebugHlp.h"
#include "Defaults.h"
#include "HTMLSectionCreator.h"
#include "HTMLSection.h"
#include "tablelayout.h"

//#define HIGH_DETAIL

#ifdef HIGH_DETAIL
	#define DETAIL_TRACE TRACE
#else
	#if _MSC_VER >= 1300 
		#define DETAIL_TRACE __noop
	#else
		#define DETAIL_TRACE 1 ? (void)0 : (void)0
	#endif	//	_MSC_VER >= 1300 
#endif

CHTMLTableLayout::CHTMLTableLayout( CHTMLTable* ptab, CHTMLSectionCreator *psc )
	: m_pTab(ptab)
	, m_nMaxWidth( psc->GetDefaultRightMargin() - psc->GetDefaultLeftMargin() )
	, m_dc( psc->GetDC() )
	, m_nCols( ptab->GetRowsCols().cy )
	, m_nRows( ptab->GetRowsCols().cx )
	, m_nZoomLevel( psc->GetZoomLevel() )
	, m_pscParent( psc )
{

	// Ensure that CHTMLTable is properly initialized...
	if (!m_pTab->m_bCellsMeasured)
	{
		//
		//	The code herin relies upon these being set, so if this is the second time we have been through this
		//	(due to zoom level changing) then we need to reset them.
		m_pTab->m_arrDesiredWidth.SetSize( 0 );
		m_pTab->m_arrMinimumWidth.SetSize( 0 );
		m_pTab->m_arrMaximumWidth.SetSize( 0 );
		m_pTab->m_arrLayoutWidth.SetSize( 0 );
		m_pTab->m_arrNoWrap.SetSize( 0 );

		m_pTab->m_arrDesiredWidth.SetSize( m_nCols );
		m_pTab->m_arrMinimumWidth.SetSize( m_nCols );
		m_pTab->m_arrMaximumWidth.SetSize( m_nCols );
		m_pTab->m_arrLayoutWidth.SetSize( m_nCols );
		m_pTab->m_arrNoWrap.SetSize( m_nCols );
	}

	
	Layout();
}


WinHelper::CSize CHTMLTableLayout::MeasureDocument(CHTMLDocument* pdoc, int nWidth)
{
	// We'll use an HTMLSection, since it will destroy the temporary sections
	// for us.
	CHTMLSection TempSection( &g_defaults );

	CHTMLSectionCreator htCreate( &TempSection, m_dc, 0, 0, nWidth, COLORREF(0), true, m_nZoomLevel, NULL, m_pscParent );
	htCreate.AddDocument( pdoc );

	return  htCreate.GetSize();
}


//	Determine the extrema for the given cell, and return the values.
void CHTMLTableLayout::MeasureCell(CHTMLTableCell* pCell, size_t nCol)
{
	const int nDesiredWidth = pCell->m_nWidth;
	bool bNoWrap = pCell->m_bNoWrap;

	if (bNoWrap)
		m_pTab->m_arrNoWrap[nCol] = true;
	// Start with minimum width:
 	if (!m_pTab->m_bCellsMeasured)
	{
		int testWidth;
		// NOWRAP takes precedence, so look there first
		if ( bNoWrap )
			testWidth = knFindMaximumWidth;
		else
			testWidth = 1;		// small enough to get smallest object

		WinHelper::CSize size( MeasureDocument( pCell, testWidth ));
		size.cx /= pCell->m_nColSpan;


		m_pTab->m_arrMinimumWidth[nCol] = max(m_pTab->m_arrMinimumWidth[nCol], size.cx);
	}
	// We need maximum width only if the table width was not specified!
	// If a percentage is specified for a nested tables width, then the
	// maximum width for that document is difficult to determine.
	// In fact, the maximu should be limited based on the maxWidth, rather
	// than on the artificial value of 32000.
	if( m_pTab->m_nWidth == 0 )
	{
		if (m_pTab->m_arrNoWrap[nCol])
		{
			m_pTab->m_arrMaximumWidth[ nCol ] = m_pTab->m_arrMinimumWidth[ nCol ];
		}
		else
		{
 			// We use nMaxWidth here, which changes on a resize, but the
 			// cells are not recalculated if they are relative!
			int nTestWidth = 0;
			if( nDesiredWidth > 0 )
			{
				nTestWidth = nDesiredWidth;
			}
			else if( nDesiredWidth == -100 )
			{
				nTestWidth = m_nMaxWidth;
			}
			else if( nDesiredWidth == 0 )
			{
				nTestWidth = knFindMaximumWidth;
			}
			else
			{
				nTestWidth = m_nMaxWidth * abs(nDesiredWidth) / 100;
			}

						
			const WinHelper::CSize size( MeasureDocument( pCell, nTestWidth ) );
			m_pTab->m_arrMaximumWidth[ nCol ] = max(m_pTab->m_arrMaximumWidth[ nCol ], size.cx);
		}
	}
	else
		m_pTab->m_arrMaximumWidth[ nCol ] = m_pTab->m_arrMinimumWidth[ nCol ];
	DETAIL_TRACE( _T("\t\tCol %d has width of %d and colspan of %d, minimumWidth %d \n"), nCol, m_pTab->m_arrMaximumWidth[ nCol ], pCell->m_nColSpan, m_pTab->m_arrMinimumWidth[ nCol ] );
}


void CHTMLTableLayout::MeasureCells()
{
 	// Zero the max widths, because they are recalculated
 	m_pTab->m_arrMaximumWidth.SetSize( m_nCols );		
	// Iterate all of the cells
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		DETAIL_TRACE( _T("\tMeasuring row %d\n"), nRow );
		CHTMLTable::CHTMLTableRow *pRow = m_pTab->m_arrRows[ nRow ];
		const size_t nCols = pRow->m_arrCells.GetSize();
		for( size_t nCol = 0; nCol < nCols; nCol++ )
		{
			CHTMLTableCell *pCell = pRow->m_arrCells[ nCol ];

			MeasureCell( pCell, nCol );

			//
			//	If the cell has colspan then we need to skip that many columns when assigning our measurements
			if( pCell->m_nColSpan )
			{
				nCol += pCell->m_nColSpan - 1;
			}
		}
	}

	m_pTab->m_bCellsMeasured = true;

#ifdef DETAIL_TRACE
		DETAIL_TRACE( _T("Table (%d, %d)\n"), m_nRows, m_nCols);
		DETAIL_TRACE( _T("  Measurements:\n") );
		for (int zz =0; zz < m_nCols; ++zz)
		{
			DETAIL_TRACE( _T("    Column: %d    desired: %d   Min: %d   Max: %d\n"), zz, m_pTab->m_arrDesiredWidth[ zz ], m_pTab->m_arrMinimumWidth[zz], m_pTab->m_arrMaximumWidth[zz]);
		}
#endif

	// Be sure to include border and padding space.
	int nDeadSpace = (2 * m_pTab->m_nBorder) + ((m_nCols + 1) * m_pTab->m_nCellSpacing) + (m_nCols * 2 * m_pTab->m_nCellPadding);
	nDeadSpace = m_dc.ScaleX(nDeadSpace);
	if (m_pTab->m_nBorder)
		nDeadSpace += m_dc.ScaleX(2 * m_nCols);
	// Calculate the extrema of table width
	m_nMaxTableWidth = m_nMinTableWidth = nDeadSpace;
	for (int i = 0; i < m_nCols; ++i)
	{
		m_nMinTableWidth += m_pTab->m_arrMinimumWidth[i];
		m_nMaxTableWidth += max(0, m_pTab->m_arrMaximumWidth[i]);	// ignore negatives!
	}
}


bool CHTMLTableLayout::LayoutMaximum()
{
	if (m_pTab->m_nWidth == 0 && m_nMaxTableWidth <= m_nMaxWidth)
	{
		m_nTableWidth = m_nMaxTableWidth;
		for (int i = 0; i < m_nCols; ++i)
			m_pTab->m_arrLayoutWidth[i] = m_pTab->m_arrMaximumWidth[i];
		return true;
	}
	else
		return false;
}


void CHTMLTableLayout::GetDesiredWidths()
{
	// Now that we know the size of the table, we can calculate desired widths
	for (int nRow = 0; nRow < m_nRows; nRow++)
	{
		CHTMLTable::CHTMLTableRow *pRow = m_pTab->m_arrRows[ nRow ];
		for( UINT nCol = 0; nCol < pRow->m_arrCells.GetSize(); nCol++ )
		{
			const int nDesiredWidth = pRow->m_arrCells[ nCol ]->m_nWidth;
			int nWidth = 0;
			if (nDesiredWidth != 0)
			{
				if( nDesiredWidth < 0 )
				{
					if( nDesiredWidth == -100 )
					{
						nWidth = m_nTableWidth;
					}
					else
					{
						nWidth = min( int( (float( m_nTableWidth ) / 100 ) * abs( nDesiredWidth ) ), m_nTableWidth );
					}
				}
				else
				{
					nWidth = min( m_dc.ScaleX(nDesiredWidth), m_nTableWidth );
				}
				
				nWidth = max(m_pTab->m_arrMinimumWidth[nCol], nWidth);

				DETAIL_TRACE( _T("Cell( %d, %d ).DesiredWidth = %d\n"), nCol, nRow, nWidth );
				m_pTab->m_arrDesiredWidth[nCol] = max(m_pTab->m_arrDesiredWidth[nCol], nWidth);
				DETAIL_TRACE( _T("Col( %d ).DesiredWidth = %d\n"), nCol, m_pTab->m_arrDesiredWidth[nCol] );
			}
		}
	}
}


int CHTMLTableLayout::CalculateTableSize()
{
	if( m_pTab->m_nWidth < 0 )
	{
		if( m_pTab->m_nWidth == -100 )
		{
			m_nTableWidth = m_nMaxWidth;
		}
		else
		{
			m_nTableWidth = min( int( (float( m_nMaxWidth ) / 100 ) * abs( m_pTab->m_nWidth ) ), m_nMaxWidth );
		}
	}
	else if( m_pTab->m_nWidth > 0 )
	{
		m_nTableWidth = m_dc.ScaleX(m_pTab->m_nWidth);
		// m_nTableWidth = min( m_pTab->m_nWidth, m_nMaxWidth );
	}
	else
	{
		m_nTableWidth = m_nMaxWidth;
	}
	// Adjust the table width to at least as wide as the minimum required
	m_nTableWidth = max(m_nTableWidth, m_nMinTableWidth);

	DETAIL_TRACE( _T("CalculateTableSize=%d\n"), m_nTableWidth );
	return m_nTableWidth;
}


int CHTMLTableLayout::GetSpaceNeeded()
{
	// Calculate the need for space...
	int nSpaceNeeded = 0;
	for (int i = 0; i <m_nCols; ++i)
		if (m_pTab->m_arrDesiredWidth[i] != 0)
			nSpaceNeeded += m_pTab->m_arrDesiredWidth[i] - m_pTab->m_arrMinimumWidth[i];
	return nSpaceNeeded;
}


//  Allow for tables wider than the available space if the table width or
//  Sum of cell widths indicate that it should be. Otherwise, squeeze it in!
//  For this to work properly, we need to be able to determine the width of cell
//  Contents for cells that have no predetermined width....
//  
//  Experimenation has revealed the following:
//		NoWrap cells will not wrap under any cicumstamnces!
//		A Cell will always be at least as wide as it's smallest element
//			i.e. image or word, or paragraph if NOWRAP
//		A tables width will not exceed the width of the page unless
//			the table width is specified, or the sum of the minimum
//			widths is wider than the page.
//		Cells with a specified width CAN shrink to maintain the
//			size of NOWRAP cells. This shrinking will occur until
//			it is illegal to do so any further.

void CHTMLTableLayout::LayoutAllSpace(int nSpaceAvailable, int nSpaceNeeded)
{
	//	Readjust the cells that require space to their
	//	desired widths.
	m_pTab->m_arrLayoutWidth = m_pTab->m_arrMinimumWidth;

	for (int i = 0; i < m_nCols; ++i)
	{
		if (m_pTab->m_arrDesiredWidth[i] != 0)
			m_pTab->m_arrLayoutWidth[i] = m_pTab->m_arrDesiredWidth[i];
	}
	nSpaceAvailable -= nSpaceNeeded;
	
	//	If there is some space left, see if any of the cells
	//	that did not require it, can use it
	if (nSpaceAvailable > 0)
	{
		int nNoSpecCount = 0;
		for (int i = 0; i < m_nCols; ++i)
		{
			if (m_pTab->m_arrDesiredWidth[i] == 0)
			{
				nNoSpecCount++;
			}
		}

		int nSpacePerCell = 0;
		if (nNoSpecCount)
		{
			// assign space to those cells
			nSpacePerCell = max(nSpaceAvailable / nNoSpecCount, 1);
			for (int i = 0; i < m_nCols; ++i)
			{
				if (m_pTab->m_arrDesiredWidth[i] == 0)
				{
					m_pTab->m_arrLayoutWidth[i] += nSpacePerCell;
				}
			}
		}
		else
		{
			//	This might seem wrong, but remember thatthe case where all cells
			//	fit on the page has already been handled.
			//	assign space to all cells!
			nSpacePerCell = max(nSpaceAvailable / m_nCols, 1);
			for (int i = 0; i < m_nCols; ++i)
			{
				m_pTab->m_arrLayoutWidth[i] += nSpacePerCell;
			}
			nNoSpecCount = m_nCols;
		}

		if( nNoSpecCount * nSpacePerCell < nSpaceAvailable )
		{
			int nLeftOver = nSpaceAvailable % nNoSpecCount;
			for (int i = 0; i < m_nCols && nLeftOver; ++i, nLeftOver--)
			{
				m_pTab->m_arrLayoutWidth[i] += 1;
			}				
		}
	}
}

void CHTMLTableLayout::LayoutSpaceProportional(int nSpaceAvailable, int nSpaceNeeded)
{
	// Assign remaining space proportional to need
	m_pTab->m_arrLayoutWidth = m_pTab->m_arrMinimumWidth;

	int nTotalSpaceAdded = 0;
	for (int i = 0; i < m_nCols; ++i)
	{
		if (m_pTab->m_arrDesiredWidth[i] != 0)
		{
			int nCellSpaceNeeded = m_pTab->m_arrDesiredWidth[i] - m_pTab->m_arrMinimumWidth[i];
			int nSpaceAdded = (nSpaceAvailable * nCellSpaceNeeded) / nSpaceNeeded;
			m_pTab->m_arrLayoutWidth[i] += nSpaceAdded;
			nTotalSpaceAdded += nSpaceAdded;
		}
	}
}


void CHTMLTableLayout::Layout()
{
	DETAIL_TRACE( _T("Detail trace start\n") );
	// If we are detemining the maximum size, and the table's width is relative,
	// there is no resonable value to return. So set the size
	// to -1. The cells will still be measured, but may be subject to the same
	// constraint.
	
	MeasureCells();

	if (m_nMaxWidth == knFindMaximumWidth &&  m_pTab->m_nWidth < 0 )
	{
		m_nTableWidth = -1;
		return;
	}
	
	// Simple case - If table width not specified, and the nMaxTableWidth is
	// small enough to fit, use those widths.
	if (!LayoutMaximum())
	{
		CalculateTableSize();
		GetDesiredWidths();

		// Calculate the need for space...
		int nSpaceNeeded = GetSpaceNeeded();
		int nSpaceAvailable = m_nTableWidth - m_nMinTableWidth;

		if (nSpaceAvailable >= nSpaceNeeded)
		{
			LayoutAllSpace( nSpaceAvailable, nSpaceNeeded );
		}
		else
		{
			LayoutSpaceProportional( nSpaceAvailable, nSpaceNeeded );
		}
	}
	DETAIL_TRACE( _T("Detail trace end\n\n\n") );
}
