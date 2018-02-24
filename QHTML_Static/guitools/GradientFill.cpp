/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GradientFill.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "GuiTools.h"

void GS::GradientFillTopToBottom( GS::CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom )
{
  const int r1 = GetRValue( crTop ), g1 = GetGValue( crTop ), b1 = GetBValue( crTop );
  const int r2 = GetRValue( crBottom ), g2 = GetGValue( crBottom ), b2 = GetBValue( crBottom );
  
	WinHelper::CRect rcTemp;
  rcTemp.left = rc.left;
  rcTemp.right = rc.right; 
	rcTemp.top = rc.top - 1;
	rcTemp.bottom = rc.top;

	const int nEnd = rc.Height();
  for( int i=0; i < nEnd; i++ )
  { 
    int r,g,b; 

    r = r1 + (i * (r2-r1) / nEnd); 
    g = g1 + (i * (g2-g1) / nEnd);
    b = b1 + (i * (b2-b1) / nEnd);
    
    
    rcTemp.top++;
    rcTemp.bottom++; 
    
    dc.FillRect( rcTemp, RGB( r, g, b) );
  }
}


void GS::GradientFillLeftToRight( GS::CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom )
{
  const int r1 = GetRValue( crTop ), g1 = GetGValue( crTop ), b1 = GetBValue( crTop );
  const int r2 = GetRValue( crBottom ), g2 = GetGValue( crBottom ), b2 = GetBValue( crBottom );
  
	WinHelper::CRect rcTemp;
  rcTemp.left = rc.left - 1;
  rcTemp.right = rc.left + 1; 
	rcTemp.top = rc.top;
	rcTemp.bottom = rc.bottom;

	const int nEnd = rc.Width() - 1;
  for( int i=0; i < nEnd; i++ )
  { 
    int r,g,b; 

    r = r1 + (i * (r2-r1) / nEnd); 
    g = g1 + (i * (g2-g1) / nEnd);
    b = b1 + (i * (b2-b1) / nEnd);
    
    
    rcTemp.left++;
    rcTemp.right++;
    
    dc.FillRect( rcTemp, RGB( r, g, b) );
  }
}


void GS::GradientFillTopToBottom( GS::CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom, UINT uRoundCorners, UINT uRoundness )
{
	HRGN hrgn = CreateRoundCornerRegion( rc, uRoundCorners, uRoundness );
	dc.SetClipRegion( hrgn );
	GradientFillTopToBottom( dc, rc, crTop, crBottom );
	VERIFY( DeleteObject( hrgn ) );
	dc.RemoveClip();
}


void GS::GradientFillLeftToRight( GS::CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom, UINT uRoundCorners, UINT uRoundness )
{
	HRGN hrgn = CreateRoundCornerRegion( rc, uRoundCorners, uRoundness );
	dc.SetClipRegion( hrgn );
	GradientFillLeftToRight( dc, rc, crTop, crBottom );
	VERIFY( DeleteObject( hrgn ) );
	dc.RemoveClip();
}
