/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Background.h
Owner:	russf@gipsysoft.com
Purpose:

		The drawn background of various objects

----------------------------------------------------------------------*/
#ifndef BACKGROUND_H
#define BACKGROUND_H

#ifndef COLOR_H
	#include "Color.h"
#endif	//	COLOR_H

#ifndef GUITOOLS_H
	#include <guitools/guitools.h>
#endif	//	GUITOOLS_H

class CQHTMImageABC;

extern void DrawTiledBitmap( int x, int y, int cx, int cy, HDC hdc, const CQHTMImageABC *pImage, UINT uFrame );

class CBackground
{
public:
	CBackground()
			: m_pimgBackground( NULL )
		{
		}

	bool IsSet() const
	{
		return m_pimgBackground || m_crBack.IsSet()
			|| ( m_crLeft.IsSet() && m_crRight.IsSet() )
			|| ( m_crTop.IsSet() && m_crBottom.IsSet() )
			|| m_crCenter.IsSet() && ( m_crTop.IsSet() || m_crBottom.IsSet() )
			|| m_crCenter.IsSet() && ( m_crLeft.IsSet() || m_crRight.IsSet() )
			;
	}

	CColor m_crBack;
	CColor m_crLeft;
	CColor m_crRight;
	CColor m_crTop;
	CColor m_crBottom;
	CColor m_crCenter;

	CQHTMImageABC *m_pimgBackground;

	bool Draw( GS::CDrawContext &dc, const WinHelper::CRect &rc ) const
	{
		bool bRetVal = false;
		if( m_pimgBackground )
		{
			//	TODO - add support for animated backgrounds
			dc.SetClipRect( rc );
			DrawTiledBitmap( rc.left, rc.top, rc.Width(), rc.Height(), dc.GetSafeHdc(), m_pimgBackground, 0 );
			dc.RemoveClip();
			bRetVal = true;
		}
		else if( m_crLeft.IsSet() && m_crRight.IsSet() )
		{
			GS::GradientFillLeftToRight( dc, rc, m_crLeft, m_crRight );
			bRetVal = true;
		}
		else if( m_crTop.IsSet() && m_crBottom.IsSet() )
		{
			GS::GradientFillTopToBottom( dc, rc, m_crTop, m_crBottom );
			bRetVal = true;
		}
		else if( m_crCenter.IsSet() && ( m_crTop.IsSet() || m_crBottom.IsSet() ) )
		{
			//	Vertical centered gradient
			const COLORREF cr = m_crTop.IsSet() ? m_crTop : m_crBottom;
			const int nCenter = rc.Height() / 2;
			WinHelper::CRect rc2( rc );
			rc2.bottom = rc2.top + nCenter;
			GS::GradientFillTopToBottom( dc, rc2, cr, m_crCenter );
			rc2.bottom = rc.bottom;
			rc2.top = rc.top + nCenter;
			GS::GradientFillTopToBottom( dc, rc2, m_crCenter, cr );
			bRetVal = true;
		}
		else if( m_crCenter.IsSet() && ( m_crLeft.IsSet() || m_crRight.IsSet() ) )
		{
			//	Horizontal centered gradient
			const COLORREF cr = m_crLeft.IsSet() ? m_crLeft : m_crRight;
			const int nCenter = rc.Width() / 2;
			WinHelper::CRect rc2( rc );
			rc2.right = rc2.left + nCenter;
			GS::GradientFillLeftToRight( dc, rc2, cr, m_crCenter );
			rc2.right = rc.right;
			rc2.left = rc.left + nCenter;
			GS::GradientFillLeftToRight( dc, rc2, m_crCenter, cr );
			bRetVal = true;
		}
		else if( m_crBack.IsSet() )
		{
			dc.FillRect( rc, m_crBack );
			bRetVal = true;
		}
		return bRetVal;
	}

};

#endif //BACKGROUND_H