/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DrawContext.inl
Owner:	russf@gipsysoft.com
Purpose:	Inline draw context functions
----------------------------------------------------------------------*/
#ifndef DRAWCONTEXT_INL
#define DRAWCONTEXT_INL

inline HDC CDrawContext::GetSafeHdc()
	{ return m_hdc; }


inline void CDrawContext::FillRect( const WinHelper::CRect &rc, COLORREF cr )
	{ VAPI( ::FillRect( m_hdc, &rc, GetBrush( cr ) ) ); }


inline void CDrawContext::PolyLine( const POINT *pt, int nCount, COLORREF cr )
	{
		SelectPen( cr );
		VAPI( ::Polyline( m_hdc, pt, nCount ) );
	}


inline void CDrawContext::SelectPen( COLORREF cr )
	{
		if( !m_bPenSelected || cr != m_crCurrentPen )
		{
			VAPI( ::SelectObject( m_hdc, GetPen( cr ) ) );
			m_crCurrentPen = cr;
			m_bPenSelected = true;
		}
	}

inline void CDrawContext::SelectBrush( COLORREF cr )
	{
		if( !m_bBrushSelected || cr != m_crCurrentBrush )
		{
			VAPI( ::SelectObject( m_hdc, GetBrush( cr ) ) );
			m_crCurrentBrush = cr;
			m_bBrushSelected = true;
		}
	}

inline void CDrawContext::PolyFillOutlined( const POINT *pt, int nCount, COLORREF cr, COLORREF crOutline )
	{
		ASSERT( nCount > 0 );
		ASSERT_VALID_READPTR( pt, sizeof( POINT) * nCount );

		SelectBrush( cr );
		SelectPen( crOutline );
		VAPI( ::Polygon( m_hdc, pt, nCount ) );

	}


inline void CDrawContext::Rectangle( const WinHelper::CRect &rc, COLORREF cr )
	{
		SelectPen( cr );
		//	Save and restore the brush because otherwise the caching system (GetBrush) still thinks
		//	a coloured brush is selected!
		HGDIOBJ hOld = ::SelectObject( m_hdc, GetStockObject( HOLLOW_BRUSH ) );
		VAPI( ::Rectangle( m_hdc, rc.left, rc.top, rc.right, rc.bottom ) );
		::SelectObject( m_hdc, hOld );
	}


inline void CDrawContext::DrawText( int x, int y, LPCTSTR pcszText, int nLength, COLORREF crFore )
	{
		if( crFore != m_crCurrentText )
		{
			::SetTextColor( m_hdc, 0x02000000 | crFore );
			m_crCurrentText = crFore;
		}
		VAPI( ::TextOut( m_hdc, x, y, pcszText, nLength ) );
	}


inline const WinHelper::CRect &CDrawContext::GetClipRect() const
	{	return m_rcClip; }


inline void CDrawContext::SetClipRect( const WinHelper::CRect &rc )
	{
		HRGN hrgn = CreateRectRgnIndirect( &rc );
		::SelectClipRgn( m_hdc, hrgn );
		POINT pt;
		VAPI( ::GetWindowOrgEx( m_hdc, &pt ) );
		(void)::OffsetClipRgn( m_hdc, -pt.x, -pt.y );
		VAPI( ::DeleteObject( hrgn ) );
	}


inline void CDrawContext::RemoveClipRect()
	{	SelectClipRgn( m_hdc, NULL );	}


inline void CDrawContext::SelectFont( LPCTSTR pcszFontNames, int nSizePixels, int nWeight, bool bItalic, bool bUnderline, bool bStrike, BYTE cCharSet )
	{
		//	we set the current font to be an invalid font because we don't know what this font may be,
		FontDef fdef( pcszFontNames, nSizePixels, nWeight, bUnderline, bItalic, bStrike, cCharSet, false );
		SelectFont( fdef );
	}


inline void CDrawContext::SelectFont( const FontDef &fdef )
	{
		FontInfo *pfinfo = GetFont( fdef );
		ASSERT( pfinfo );	//	Can't be NULL!
		if( pfinfo != m_pFontInfo )
		{
			m_pFontInfo = pfinfo;
			::SelectObject( m_hdc, m_pFontInfo->hFont );
		}
	}


inline int CDrawContext::ScaleX(int x) const
	{ return ::MulDiv(x, m_cxDeviceScaleNumer, m_cxDeviceScaleDenom); }


inline int CDrawContext::ScaleY(int y) const
	{ return ::MulDiv(y, m_cyDeviceScaleNumer, m_cyDeviceScaleDenom); }


inline WinHelper::CSize CDrawContext::Scale(const WinHelper::CSize& size) const
	{
		return WinHelper::CSize(ScaleX(size.cx), ScaleY(size.cy));
	}


inline void CDrawContext::SetScaling( int cxDeviceScaleNumer, int cxDeviceScaleDenom, int cyDeviceScaleNumer, int cyDeviceScaleDenom )
	{
		m_cxDeviceScaleNumer = cxDeviceScaleNumer;
		m_cxDeviceScaleDenom = cxDeviceScaleDenom;
		m_cyDeviceScaleNumer = cyDeviceScaleNumer;
		m_cyDeviceScaleDenom = cyDeviceScaleDenom;
	}


#endif //DRAWCONTEXT_INL
