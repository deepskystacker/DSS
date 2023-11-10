// cdxCRotBevelLine.cpp : implementation file
//

#include "stdafx.h"
#include "cdxCRotBevelLine.h"
#include "cdxCRot90DC.h"

/////////////////////////////////////////////////////////////////////////////
// cdxCRotBevelLine
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(cdxCRotBevelLine,CStatic);

/////////////////////////////////////////////////////////////////////////////

static const CString	s_strTwoSpaces	=	_T("  ");
static const CPoint	s_pnt11			=	CPoint(1,1);

/////////////////////////////////////////////////////////////////////////////
// construction
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(cdxCRotBevelLine, CStatic)
	//{{AFX_MSG_MAP(cdxCRotBevelLine)
	ON_WM_PAINT()
	ON_WM_DROPFILES()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cdxCRotBevelLine misc stuff
/////////////////////////////////////////////////////////////////////////////

static inline CRect makeTextRect(const CRect & rectClient, const CSize & sz, DWORD dwType, int iAngle)
{
	CRect	rectText;

	switch(iAngle)
	{
		default:	ASSERT(false);
		case	0:

			rectText.top		=	rectClient.top - sz.cy/2 - 2;
			rectText.bottom	=	rectText.top + sz.cy;

			switch(dwType)
			{
				default			:	ASSERT(false);
				case	SS_LEFT	:	rectText.left	=	rectClient.left;
										rectText.right	=	rectClient.left + sz.cx;
										break;
				case	SS_CENTER:	rectText.left	=	rectClient.left + (rectClient.Width() - sz.cx) / 2;
										rectText.right	=	rectText.left + sz.cx;
										break;
				case	SS_RIGHT	:	rectText.left	=	rectClient.right - sz.cx;
										rectText.right	=	rectClient.right;
										break;
			}
			break;

		case	90:

			rectText.left		=	rectClient.left - sz.cy/2 - 1;
			rectText.right		=	rectText.left + sz.cy;

			switch(dwType)
			{
				default			:	ASSERT(false);
				case	SS_RIGHT	:	rectText.top		=	rectClient.top;
										rectText.bottom	=	rectClient.top + sz.cx;
										break;
				case	SS_CENTER:	rectText.top		=	rectClient.top + (rectClient.Height() - sz.cx) / 2;
										rectText.bottom	=	rectText.top + sz.cx;
										break;
				case	SS_LEFT	:	rectText.top		=	rectClient.bottom - sz.cx;
										rectText.bottom	=	rectClient.bottom;
										break;
			}
			break;

		case	270:

			rectText.left		=	rectClient.left - sz.cy/2 + 1;
			rectText.right		=	rectText.left + sz.cy;

			switch(dwType)
			{
				default			:	ASSERT(false);
				case	SS_LEFT	:	rectText.top		=	rectClient.top;
										rectText.bottom	=	rectClient.top + sz.cx;
										break;
				case	SS_CENTER:	rectText.top		=	rectClient.top + (rectClient.Height() - sz.cx) / 2;
										rectText.bottom	=	rectText.top + sz.cx;
										break;
				case	SS_RIGHT	:	rectText.top		=	rectClient.bottom - sz.cx;
										rectText.bottom	=	rectClient.bottom;
										break;
			}
			break;
	}

	return rectText;
}

static inline void drawText(CDC & dc, CString s, const CRect & r, const CPoint & pnt11, bool bDisabled)
{
	COLORREF	txtCol	=	dc.GetTextColor();
	int		md			=	dc.SetBkMode(TRANSPARENT);

	if(bDisabled)
	{
		dc.SetTextColor(::GetSysColor(COLOR_3DHIGHLIGHT));
		dc.DrawText(s,r + pnt11,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		dc.SetTextColor(::GetSysColor(COLOR_3DSHADOW));
	}
	else
		dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

	dc.DrawText(s,(RECT *)(const RECT *)r,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	dc.SetTextColor(txtCol);
	dc.SetBkMode(md);
}

static inline CSize textExtent(CDC & dc, CString s)
{
	CRect	r(0,10,0,10);
	CSize	sz;
	sz.cy	=	dc.DrawText(s,r,DT_LEFT|DT_SINGLELINE|DT_NOCLIP|DT_CALCRECT);
	sz.cx	=	r.Width();
	return sz;
}

void cdxCRotBevelLine::OnPaint()
{
	// make dc

	CPaintDC dc(this); // device context for painting

	CRect	rectClient;
	GetClientRect(rectClient);

	COLORREF	oldBkCol		=	dc.GetBkColor();
	CFont		*pOldFont	=	dc.SelectObject(GetFont());
	COLORREF	hiCol			=	::GetSysColor(!m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW),
				loCol			=	::GetSysColor(m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW);

	dc.FillSolidRect(rectClient,::GetSysColor(COLOR_BTNFACE));

	// draw bevel

	CRect		rectBevel;
	bool		bHorz;

	if(rectClient.Width() > rectClient.Height())
	{
		rectBevel.left		=	rectClient.left;
		rectBevel.right	=	rectClient.right;
		rectBevel.top		=	rectClient.top + rectClient.Height() / 2;
		rectBevel.bottom	=	rectBevel.top + 2;
		bHorz					=	true;
	}
	else
	{
		rectBevel.left		=	rectClient.left + rectClient.Width() / 2;
		rectBevel.right	=	rectBevel.left + 2;
		rectBevel.top		=	rectClient.top;
		rectBevel.bottom	=	rectClient.bottom;
		bHorz					=	false;
	}

	dc.Draw3dRect(rectBevel,hiCol,loCol);

	// add some text

	CString	s;
	GetWindowText(s);

	if(!s.IsEmpty())
	{
		// interpret my ~ stuff (even for horizontals where we cut it off only)

		int	iAngle;

		if(s[0] == _T('~'))
		{
			s			=	s.Mid(1);
			iAngle	=	bHorz ? 0 : 270;
		}
		else
			iAngle	=	bHorz ? 0 : 90;

		// get proper style

		DWORD	dwType	=	GetStyle() & SS_TYPEMASK;

		switch(dwType)
		{
			default:				dwType	=	SS_LEFT;
			case	SS_LEFT	:	break;
			case	SS_RIGHT	:	break;
			case	SS_CENTER:	break;
		}

		if(dwType != SS_LEFT)
			s	=	s_strTwoSpaces + s;
		if(dwType != SS_RIGHT)
			s	+=	s_strTwoSpaces;

		// get proper rect

		CSize	sz			=	textExtent(dc,s);
		CRect	rectText	=	makeTextRect(rectBevel,sz,dwType,iAngle);

		++rectText.bottom;
		if(dwType == SS_RIGHT)		// ensure disabled text is drawn properly
			--rectText.left;

		dc.IntersectClipRect(rectText);
		dc.FillSolidRect(rectText,::GetSysColor(COLOR_BTNFACE));

		if(bHorz)
		{
			drawText(dc,s,rectText,s_pnt11,(GetStyle() & WS_DISABLED) != 0);
		}
		else
		{
			cdxCRot90DC	rotDC(dc,rectText,iAngle,true);

			if(rotDC.IsCreated())
				drawText(rotDC,s,rotDC,rotDC.rotate(s_pnt11),(GetStyle() & WS_DISABLED) != 0);
		}
	}

	dc.SelectObject(pOldFont);
	dc.SetBkColor(oldBkCol);
}

/////////////////////////////////////////////////////////////////////////////

void cdxCRotBevelLine::OnEnable(BOOL bEnable)
{
	SetRedraw(FALSE);
	CStatic::OnEnable(bEnable);
	SetRedraw(TRUE);
	Invalidate();
}

BOOL cdxCRotBevelLine::OnEraseBkgnd([[maybe_unused]] CDC* pDC)
{
	return TRUE;
}

void cdxCRotBevelLine::OnNcPaint()
{
	// do nothing here
}
