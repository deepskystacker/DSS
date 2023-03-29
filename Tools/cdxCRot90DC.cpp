// cdxCRot90DC.cpp: implementation of the cdxCRot90DC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cdxCRot90DC.h"
#include "Ztrace.h"

//////////////////////////////////////////////////////////////////////
// cdxCRot90DC
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(cdxCRot90DC,CDC);

//////////////////////////////////////////////////////////////////////

const cdxCRot90DC::Rotator	cdxCRot90DC::M_Rotator[cdxCRot90DC::a_num]	=
{
	{	0,1	},		// 0°
	{	-1,0	},		// 270°
	{	0,-1	},		// 180°
	{	1,0	},		// 90°
};

static const CSize	s_szNull(0,0);
static const CPoint	s_pntOne(1,1);

//////////////////////////////////////////////////////////////////////
// Creation / destruction
//////////////////////////////////////////////////////////////////////

/*
 * Create()
 * Creates a new device context compatible to the given dc "destDC".
 * It assumes you want to draw into rectDC of that DC with an angle of
 * iAngle.
 * If bCopy is true, this device context's context will be copied
 * initially.
 *
 *	destDC		-	output device context
 * rectDC		-	output device context area you want to draw to           ^
 * iAngle		-	angel of output (90° => an arrow -> you draw will appear | )
 * bCopy			-	if true, the original destDC's contence will
 *						be copied to this dc past creation
 *
 * Returns true on success.
 * Use the operator const CRect & to get the newly assigned rectangle
 * coordinates.
 *
 * NOTE that internally, only rectDC intersected with the clipping rectangle
 * of the original device context is stored.
 */

bool cdxCRot90DC::Create(CDC & destDC, const CRect & rectDC, int iAngle, bool bCopy)
{
	ZFUNCTRACE_RUNTIME();
	//
	// check whether there's something to do
	//

	if(destDC.IsPrinting())
	{
		m_pDC	=	nullptr;
		ASSERT(false);		// sorry, can't be used for printing.
								// If you want you can try to use it
								// - please report any results you made :)
		m_rectMine.SetRectEmpty();
		return false;
	}

	CRect	rectClip;
	destDC.GetClipBox(rectClip);

	m_rectDCClipped.IntersectRect(rectDC,rectClip);

	if((m_rectDCClipped.left >= m_rectDCClipped.right) || (m_rectDCClipped.top >= m_rectDCClipped.bottom))
	{
		m_pDC	=	nullptr;		// empty rectangle - nothing to do ...
		m_rectMine.SetRectEmpty();
		return false;
	}

	//
	// calc new stuff
	//

	m_pDC					=	&destDC;
	m_Angle				=	mkAngle(iAngle);;
	m_rectDC				=	rectDC;
	m_rectMine			=	rotate(m_rectDC);
	m_rectMineClipped	=	rotate(m_rectDCClipped);
	m_bFinished			=	false;

	CSize	szNeeded		=	m_rectMineClipped.Size();

	//
	// check whether we need a new device context
	//

	if(!m_hDC)
	{
		if(!CreateCompatibleDC(&destDC))
		{
			ASSERT(false);		// Oups ??
			m_pDC	=	nullptr;		// empty rectangle - nothing to do ...
			m_rectMine.SetRectEmpty();
			return false;
		}

		m_iOldBkColor		=	GetBkColor();
		m_iOldBkMode		=	GetBkMode();
		m_iOldTextColor	=	GetTextColor();
		m_pOldFont			=	GetCurrentFont();
		m_pOldBitmap		=	GetCurrentBitmap();
	}

	SetBkColor(destDC.GetBkColor());
	SetBkMode(destDC.GetBkMode());
	SetTextColor(destDC.GetTextColor());
	SelectObject(destDC.GetCurrentFont());

	//
	// check whether we need a new bitmap (reusing old if possible)
	//

	if(m_Bitmap.m_hObject != nullptr)
	{
		if((m_szBitmap.cx < szNeeded.cx) || (m_szBitmap.cy < szNeeded.cy))
		{
			SelectObject(m_pOldBitmap);
			m_Bitmap.DeleteObject();
			ASSERT( m_Bitmap.m_hObject == nullptr );
		}
	}

	if(m_Bitmap.m_hObject == nullptr)
	{
		m_szBitmap	=	szNeeded;
		VERIFY( m_Bitmap.CreateCompatibleBitmap(m_pDC,m_szBitmap.cx,m_szBitmap.cy) );

		BITMAP	bm;
		VERIFY( m_Bitmap.GetBitmap(&bm) );
		ASSERT( (bm.bmWidth == m_szBitmap.cx) && (bm.bmHeight == m_szBitmap.cy) );
	}

	//
	// set window origin
	//

	SetMapMode(MM_TEXT);
	SetWindowOrg(m_rectMineClipped.TopLeft());
/*	SetViewportOrg(0,0);
	SetViewportExt(m_szBitmap.cx,m_szBitmap.cy);
	SetWindowExt(m_szBitmap.cx,m_szBitmap.cy);
*/	SetBoundsRect(m_rectMineClipped,DCB_RESET|DCB_ACCUMULATE);
	SelectObject(&m_Bitmap);

	//
	// do user wants to copy contence of original image ?
	//

	if(bCopy)
	{
		CRect	rectSave	=	m_rectDCClipped;
		rectSave.InflateRect(1,1);
		CopyFrom(rectSave);
	}

	//
	// done :)
	//

	return true;
}

/*
 * Finish()
 * --------
 * Clips back the data in this context (this will be done automatically
 * in the destructor if you don't call Destroy(), Finish() or Invalidate()).
 *
 * returns true on success (if the context is created).
 */

bool cdxCRot90DC::Finish()
{
	ZFUNCTRACE_RUNTIME();
	if(!m_pDC)
		return false;			// no initial device context
	if(m_bFinished)
		return true;

	m_bFinished	=	true;
	CopyRotTo(m_rectMineClipped);
	return true;
}

/*
 * Destroy()
 * ---------
 * Destroys the data.
 * This function deletes all contence.
 */

void cdxCRot90DC::Destroy()
{
	m_pDC	=	nullptr;
	if(!m_hDC)
		return;

	SetBkColor(m_iOldBkColor);
	SetBkMode(m_iOldBkMode);
	SetTextColor(m_iOldTextColor);
	SelectObject(m_pOldFont);

	SelectObject(m_pOldBitmap);
	VERIFY( m_Bitmap.DeleteObject() );
	DeleteDC();
}

//////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////

/*
 * DrawControl()
 * -------------
 * Draws a control with a text and an icon.
 *
 * rect				-	area to draw into
 * strText			-	text (might be empty)
 * hIcon				-	optional icon (may be nullptr)
 * nDrawTextFlags	-	Flags for CDC::DrawText.
 *							These even define the orientation of the icon.
 * nMyFlags			-	DrawControlFlags:
 *								DRC_NO_ICON_ROTATION	don't rotate icon itself.
 *								DRC_DISABLED show disabled state
 *	nIconDist		-	distance between icon and text
 */

void cdxCRot90DC::DrawControl(CRect rect, CString strText, HICON hIcon, UINT nDrawTextFlags, UINT nMyFlags, UINT nIconDist)
{
	ZFUNCTRACE_RUNTIME();
	if(rect.IsRectEmpty())
		return;

	nDrawTextFlags	&=	~DT_CALCRECT;

	//
	// draw icon first
	//

	CSize	sz;

	if(hIcon && ((sz = GetIconSize(hIcon)) != s_szNull))
	{
		//
		// Position icon
		//

		if(nMyFlags & DRC_NO_ICON_ROTATION)
			sz	=	rotate(sz);

		CPoint	pntIcon;
		bool		bDoHpos	=	true;

		switch(nDrawTextFlags & DRAWTEXT_HPOS)
		{
			case	DT_LEFT	:

				// text is left-bound, we'll draw our icon right there
				pntIcon.x		=	rect.left;
				rect.left	+=	sz.cx + nIconDist;
				break;

			case	DT_RIGHT:

				// text is right-bound, we'll draw our icon right there
				rect.right	-=	sz.cx + nIconDist;
				pntIcon.x		=	rect.right + nIconDist;
				break;

			default			:			// DT_CENTER

				// text is centered, thus we put the icon on top of it
				pntIcon.x		=	rect.left + (rect.Width() - sz.cx) / 2;

				if((nDrawTextFlags & DRAWTEXT_HPOS) != DT_BOTTOM)
				{
					if(((nDrawTextFlags & DRAWTEXT_HPOS) == DT_CENTER) && strText.IsEmpty())
					{
						pntIcon.y	=	rect.top + (rect.Height() - sz.cy) / 2;
						rect.top		+=	sz.cy + nIconDist;
					}
					else
					{
						pntIcon.y	=	rect.top;
						rect.top		+=	sz.cy + nIconDist;
					}
				}
				else
				{
					pntIcon.y	=	rect.bottom - sz.cy;
					rect.bottom	-=	sz.cy + nIconDist;
				}

				bDoHpos	=	false;
				break;
		}

		if(bDoHpos)
			switch(nDrawTextFlags & DRAWTEXT_VPOS)
			{
				case	DT_TOP	:

					pntIcon.y	=	rect.top;
					break;

				case	DT_BOTTOM:

					pntIcon.y	=	rect.bottom - sz.cy;
					break;

				default		:		// DT_VCENTER

					pntIcon.y	=	rect.top + (rect.Height() - sz.cy) / 2;
					break;
			}

		// draw icon

		if(!(nMyFlags & DRC_NO_ICON_ROTATION))
			DrawState(	pntIcon,
							sz,
							hIcon,
							DST_ICON|((nMyFlags & DRC_DISABLED) ? DSS_DISABLED : DSS_NORMAL),
							(CBrush *)nullptr);
		else
		{
			cdxCRot90DC	iconDC(this,CRect(pntIcon.x,pntIcon.y,pntIcon.x + sz.cx,pntIcon.y + sz.cy),-mkAngle(m_Angle),true);
#pragma warning (suppress:4457)
			CRect	rect	=	iconDC;

			if(!rect.IsRectEmpty())
				iconDC.DrawState(	rect.TopLeft(),
										rect.Size(),
										hIcon,
										DST_ICON|((nMyFlags & DRC_DISABLED) ? DSS_DISABLED : DSS_NORMAL),
										(CBrush *)nullptr);
		}
	}

	//
	// now draw text into remaining rectangle
	//

	if(!rect.IsRectEmpty() && !strText.IsEmpty())
	{
		int	iCol	=	SetTextColor(::GetSysColor(COLOR_BTNTEXT)),
				iMode	=	SetBkMode(TRANSPARENT);


		CRect	rectPrevClip;
		bool	bClipped	=	false;

		if(((nDrawTextFlags & DRAWTEXT_VPOS) != DT_TOP) &&
			!(nDrawTextFlags & DT_SINGLELINE))
		{
			CRect	measure	=	rect;
			long	lHi	=	DrawText(strText,measure,(nDrawTextFlags | DT_CALCRECT) & ~DT_NOCLIP);

			GetClipBox(rectPrevClip);
			IntersectClipRect(rect);
			bClipped	=	true;

			if((nDrawTextFlags & DRAWTEXT_VPOS) == DT_VCENTER)
			{
				rect.top		=	rect.top + (rect.Height() - lHi) / 2;
				rect.bottom	=	rect.top + lHi;
			}
			else
			{
				rect.top		=	rect.bottom - lHi;
			}
		}

		if(nMyFlags & DRC_DISABLED)
		{
			SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
			DrawText(strText,rect + rotate(s_pntOne),nDrawTextFlags);
			SetTextColor(::GetSysColor(COLOR_3DSHADOW));
		}
		DrawText(strText,rect,nDrawTextFlags);

		if(bClipped)
			SetBoundsRect(rectPrevClip,DCB_RESET|DCB_ACCUMULATE);

		SetTextColor(iCol);
		SetBkMode(iMode);
	}
}

