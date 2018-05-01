/**********************************************************************
**
**	CustomTabCtrl.cpp : implementation file of CCustomTabCtrl class
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#include "stdafx.h"
#include <AFXPRIV.H>
#include "CustomTabCtrl.h"

// CCustomTabCtrlItem

CCustomTabCtrlItem::CCustomTabCtrlItem(CString sText,LPARAM lParam) : 
			m_sText(sText), 
			m_lParam(lParam),	
			m_bShape(TAB_SHAPE1),
			m_fSelected(FALSE),
			m_fHighlighted(FALSE),
			m_fHighlightChanged(FALSE)
{
}

void CCustomTabCtrlItem::operator=(const CCustomTabCtrlItem &other)
{
	m_sText = other.m_sText;
	m_lParam = other.m_lParam;
}

void CCustomTabCtrlItem::ComputeRgn(BOOL fOnTop)
{
	m_rgn.DeleteObject();

	CPoint pts[6];
	GetRegionPoints(m_rect,pts, fOnTop);
	m_rgn.CreatePolygonRgn(pts, 6, WINDING);
}

void CCustomTabCtrlItem::GetRegionPoints(const CRect& rc, CPoint* pts, BOOL fOnTop) const
{
	switch(m_bShape)
	{
	case TAB_SHAPE2:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left, rc.bottom+1);
				pts[1] = CPoint(rc.left, rc.top);
				pts[2] = CPoint(rc.left + rc.Height()/2, rc.bottom+1);
				pts[3] = CPoint(rc.left + rc.Height()/2, rc.bottom+1);
				pts[4] = CPoint(rc.left + rc.Height()/2, rc.bottom+1);
				pts[5] = CPoint(rc.left + rc.Height()/2, rc.bottom+1);
			}
			else
			{
				pts[0] = rc.TopLeft();
				pts[1] = CPoint(rc.left, rc.bottom);
				pts[2] = CPoint(rc.left + rc.Height()/2, rc.top);
				pts[3] = CPoint(rc.left + rc.Height()/2, rc.top);
				pts[4] = CPoint(rc.left + rc.Height()/2, rc.top);
				pts[5] = CPoint(rc.left + rc.Height()/2, rc.top);
			}
		}
		break;
	case TAB_SHAPE3:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left, rc.bottom+1);
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left, rc.top);
				pts[3] = CPoint(rc.left, rc.top);
				pts[4] = CPoint(rc.left, rc.top);
				pts[5] = CPoint(rc.left, rc.top);
			}
			else
			{
				pts[0] = rc.TopLeft();
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left, rc.bottom);
				pts[3] = CPoint(rc.left, rc.bottom);
				pts[4] = CPoint(rc.left, rc.bottom);
				pts[5] = CPoint(rc.left, rc.bottom);
			}
		}
		break;
	case TAB_SHAPE4:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left, rc.bottom+1);
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left + rc.Height()/2, rc.top);
				pts[3] = CPoint(rc.right - rc.Height()/2, rc.top);
				pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
				pts[5] = CPoint(rc.right, rc.bottom+1);
			}
			else
			{
				pts[0] = rc.TopLeft();
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left + rc.Height()/2, rc.bottom);
				pts[3] = CPoint(rc.right - rc.Height()/2, rc.bottom);
				pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
				pts[5] = CPoint(rc.right, rc.top);
			}
		}
		break;
	case TAB_SHAPE5:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left, rc.bottom+1);
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left + rc.Height()/2 , rc.top);
				pts[3] = CPoint(rc.right - rc.Height()/2, rc.top);
				pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
				pts[5] = CPoint(rc.right - rc.Height()/2, rc.bottom+1);
			}
			else
			{
				pts[0] = rc.TopLeft();
				pts[1] = CPoint(rc.left + rc.Height()/4, rc.Height()/2);
				pts[2] = CPoint(rc.left + rc.Height()/2 , rc.bottom);
				pts[3] = CPoint(rc.right - rc.Height()/2, rc.bottom);
				pts[4] = CPoint(rc.right - rc.Height()/4, rc.Height()/2);
				pts[5] = CPoint(rc.right - rc.Height()/2, rc.top);
			}
		}
		break;
	default:
		{
			pts[0] = CPoint(0,0);
			pts[1] = CPoint(0,0);
			pts[2] = CPoint(0,0);
			pts[3] = CPoint(0,0);
			pts[4] = CPoint(0,0);
			pts[5] = CPoint(0,0);
		}
		break;
	}
}

void CCustomTabCtrlItem::GetDrawPoints(const CRect& rc, CPoint* pts, BOOL fOnTop) const
{
	switch(m_bShape)
	{
	case TAB_SHAPE2:
	case TAB_SHAPE3:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left, rc.top);
				pts[1] = CPoint(rc.left + rc.Height()/2, rc.bottom);
			}
			else
			{
				pts[0] = CPoint(rc.left, rc.bottom);
				pts[1] = CPoint(rc.left + rc.Height()/2, rc.top);
			}
		}
		break;
	case TAB_SHAPE4:
	case TAB_SHAPE5:
		{
			if(fOnTop)
			{
				pts[0] = CPoint(rc.left,rc.bottom);
				pts[1] = CPoint(rc.left + rc.Height()/2, rc.top);
				pts[2] = CPoint(rc.right - rc.Height()/2, rc.top);
				pts[3] = CPoint(rc.right, rc.bottom);
			}
			else
			{
				pts[0] = rc.TopLeft();
				pts[1] = CPoint(rc.left + rc.Height()/2, rc.bottom);
				pts[2] = CPoint(rc.right - rc.Height()/2, rc.bottom);
				pts[3] = CPoint(rc.right, rc.top);
			}
		}
		break;
	}

}

void CCustomTabCtrlItem::Draw(CDC& dc, CFont& font, BOOL fOnTop, BOOL fRTL)
{
	COLORREF bgColor = GetSysColor((m_fSelected||m_fHighlighted) ? COLOR_WINDOW     : COLOR_3DFACE);
	COLORREF fgColor = GetSysColor((m_fSelected ||m_fHighlighted) ? COLOR_WINDOWTEXT : COLOR_BTNTEXT);

	CBrush brush(bgColor);

	CPen blackPen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	CPen shadowPen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

	CPoint pts[4];
	CRect rc = m_rect;
	GetDrawPoints(rc, pts, fOnTop);
	
	// Paint item background
	dc.FillRgn(&m_rgn, &brush);	

	CPen* pOldPen = dc.SelectObject(&blackPen);

	// draw item
	switch(m_bShape)
	{
	case TAB_SHAPE2:
	case TAB_SHAPE3:
		{
			dc.MoveTo(pts[0]);
			dc.LineTo(pts[1]);
		
			if(!m_fSelected)
			{
				dc.SelectObject(&shadowPen);
				dc.MoveTo(pts[0].x-1,pts[0].y);
				dc.LineTo(pts[1].x-1,pts[1].y);
			}
		}
		break;
	case TAB_SHAPE4:
	case TAB_SHAPE5:
		{
			dc.MoveTo(pts[0]);
			dc.LineTo(pts[1]);
			
			dc.SelectObject(&shadowPen);
			dc.LineTo(pts[2]);
			
			if(!m_fSelected)
			{
				dc.MoveTo(pts[2].x-1,pts[2].y);
				dc.LineTo(pts[3].x-1,pts[3].y);
			}

			dc.SelectObject(&blackPen);
			dc.MoveTo(pts[2]);
			dc.LineTo(pts[3]);

			// draw item text

			COLORREF bgOldColor = dc.SetBkColor(bgColor);
			COLORREF fgOldColor = dc.SetTextColor(fgColor);
			rc.DeflateRect(rc.Height()/2, 2,rc.Height()/2,2);
			CFont* pOldFont = dc.SelectObject(&font);
			if(fRTL)
				dc.DrawText(m_sText, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_RTLREADING);
			else 
				dc.DrawText(m_sText, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			dc.SelectObject(pOldFont);
			dc.SetTextColor(fgOldColor);
			dc.SetBkColor(bgOldColor);
		}
		break;
	}
	dc.SelectObject(pOldPen);
}

// CCustomTabCtrl

LOGFONT CCustomTabCtrl::lf_default = {12, 0, 0, 0, FW_NORMAL, 0, 0, 0,
			DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Microsoft Sans Serif")};

BYTE CCustomTabCtrl::m_bBitsGlyphs[] = {
										0xBD,0xFB,0xDF,0xBD,0x3C,0x00,
										0xB9,0xF3,0xCF,0x9D,0x99,0x00,
										0xB1,0xE3,0xC7,0x8D,0xC3,0x00,
										0xA3,0xC7,0xE3,0xC5,0xE7,0x00,
										0xB1,0xE3,0xC7,0x8D,0xC3,0x00,
										0xB9,0xF3,0xCF,0x9D,0x99,0x00,
										0xBD,0xFB,0xDF,0xBD,0x3C,0x00
										};


CCustomTabCtrl::CCustomTabCtrl() :
			m_nButtonIDDown(CTCID_NOBUTTON),
			m_nPrevState(BNST_INVISIBLE),
			m_nNextState(BNST_INVISIBLE),
			m_nFirstState(BNST_INVISIBLE),
			m_nLastState(BNST_INVISIBLE),
			m_nCloseState(BNST_INVISIBLE),
			m_nItemSelected(-1),
			m_nItemNdxOffset(0),
			m_dwLastRepeatTime(0),
			m_hBmpBkLeftSpin(NULL),
			m_hBmpBkRightSpin(NULL),
			m_hCursorMove(NULL),
			m_hCursorCopy(NULL),
			m_nItemDragDest(0)
{
	RegisterWindowClass();
	SetControlFont(GetDefaultFont());
	m_bmpGlyphsMono.CreateBitmap(48,7,1,1,m_bBitsGlyphs);
}

// Register the window class if it has not already been registered.

BOOL CCustomTabCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CustomTabCtrl_CLASSNAME, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = CustomTabCtrl_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

CCustomTabCtrl::~CCustomTabCtrl()
{
	for(int i=0; i< m_aItems.GetSize(); i++)
		delete m_aItems[i];
	m_aItems.RemoveAll();

	::DeleteObject(m_hBmpBkLeftSpin);
	m_hBmpBkLeftSpin = NULL;
	::DeleteObject(m_hBmpBkRightSpin);
	m_hBmpBkRightSpin = NULL;
	::DestroyCursor(m_hCursorMove);
	m_hCursorMove = NULL;
	::DestroyCursor(m_hCursorCopy);
	m_hCursorCopy = NULL;
}

BEGIN_MESSAGE_MAP(CCustomTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CCustomTabCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(THM_WM_THEMECHANGED,OnThemeChanged)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_EN_UPDATE(CTCID_EDITCTRL, OnUpdateEdit)
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()

// CCustomTabCtrl message handlers

BOOL CCustomTabCtrl::Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID)
{
	return CWnd::Create(CustomTabCtrl_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CCustomTabCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CCustomTabCtrl::OnPaint()
{
	CPaintDC dc(this);

	if(!m_hBmpBkLeftSpin)
	{
		m_rgbGlyph[0] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[1] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[2] = GetSysColor(COLOR_BTNTEXT);
		m_rgbGlyph[3] = GetSysColor(COLOR_BTNTEXT);
	}

	CRect rCl;
	GetClientRect(&rCl);
	if(IsVertical())
		rCl.SetRect(0,0,rCl.Height(),rCl.Width());

	CPen blackPen(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));

	CDC dcMem;
	CBitmap bmpMem;
	CBitmap* pOldBmp=NULL;

	if(dcMem.CreateCompatibleDC(&dc))
	{
		if(bmpMem.CreateCompatibleBitmap(&dc,rCl.Width(),rCl.Height()))
			pOldBmp = dcMem.SelectObject(&bmpMem);
		else
			return;
    }
	else
		return;

	int nBtns = 0;
	if(m_nCloseState)
		nBtns++;
	if(m_nPrevState)
		nBtns += 2;
	if(m_nFirstState)
		nBtns += 2;
	

	// clear background
	dcMem.FillSolidRect(&rCl,GetSysColor(COLOR_BTNFACE));

	BOOL fRTL = (BOOL)(GetExStyle()&WS_EX_LAYOUTRTL);
	BOOL fAfter = (BOOL)GetStyle()&CTCS_BUTTONSAFTER;
	BOOL fTop = (BOOL)GetStyle()&CTCS_TOP;
	int nA = rCl.Height()-3;
	CRect rAll;
	if(fTop)
		rAll.SetRect(0,0,nBtns*nA+3,rCl.Height()-1);
	else
		rAll.SetRect(0,1,nBtns*nA+3,rCl.Height());
	
	if(nBtns==0)
		rAll.SetRectEmpty();
	int nCloseOffset = 0;
	
	if(fAfter)
	{
		nCloseOffset = rCl.Width()-rAll.Width();
		rAll.OffsetRect(nCloseOffset,0);
	}

		// draw tab items visible and not selected	
	for(int i=0; i<m_aItems.GetSize(); i++)
	{
		if(m_aItems[i]->m_bShape && !m_aItems[i]->m_fSelected)
		{
			if(m_aItems[i]->m_fHighlighted)
				m_aItems[i]->Draw(dcMem,m_FontSelected,GetStyle()&CTCS_TOP,fRTL);
			else
				m_aItems[i]->Draw(dcMem,m_Font,GetStyle()&CTCS_TOP,fRTL);
		}
	}

	// draw selected tab item	
	if(m_nItemSelected!=-1 && m_aItems[m_nItemSelected]->m_bShape)
		m_aItems[m_nItemSelected]->Draw(dcMem,m_FontSelected,GetStyle()&CTCS_TOP,fRTL);

	if(m_nCloseState || m_nPrevState)
	{
		CPen* pOldPen = dcMem.SelectObject(&blackPen);
		dcMem.Rectangle(rAll);
		dcMem.SelectObject(pOldPen);
	}


	// draw buttons
	if(m_nCloseState)
	{
		BOOL fMirrored = TRUE;
		if(fRTL&&fAfter || !fRTL&&!fAfter)
			fMirrored = FALSE;
		CRect rClose;
		if(fAfter)
		{
			if(fTop)
				rClose.SetRect(rCl.Width()-nA-1,1,rCl.Width()-1,rCl.Height()-2);
			else
				rClose.SetRect(rCl.Width()-nA-1,2,rCl.Width()-1,rCl.Height()-1);
		}
		else
		{
			nCloseOffset = nA;
			if(fTop)
				rClose.SetRect(1,1,nA+1,rCl.Height()-2);
			else
				rClose.SetRect(1,2,nA+1,rCl.Height()-1);

		}
		CPoint ptClose(rClose.left+(rClose.Width()-8)/2+rClose.Width()%2,rClose.top+(rClose.Height()-7)/2);
		if(fMirrored && m_hBmpBkRightSpin)
			DrawBk(dcMem,rClose,m_hBmpBkRightSpin,m_fIsRightImageHorLayout,m_mrgnRight,m_nCloseState-1);
		else if(m_hBmpBkLeftSpin)
			DrawBk(dcMem,rClose,m_hBmpBkLeftSpin,m_fIsLeftImageHorLayout,m_mrgnLeft,m_nCloseState-1);
		else
		{
			if(m_nCloseState==BNST_PRESSED)
				dcMem.DrawFrameControl(rClose,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
			else
				dcMem.DrawFrameControl(rClose,DFC_BUTTON,DFCS_BUTTONPUSH);
		}
		if(fRTL)
			DrawGlyph(dcMem,ptClose,1,m_nCloseState-1);
		else
			DrawGlyph(dcMem,ptClose,4,m_nCloseState-1);
	}

	if(m_nPrevState)
	{
		CRect rFirst,rPrev,rNext,rLast;

		if(fTop)
		{
			if(nBtns<4)
			{
				rPrev.SetRect(nCloseOffset+1,1,nCloseOffset+nA+1,rCl.Height()-2);
				rNext.SetRect(nCloseOffset+nA+2,1,nCloseOffset+2*nA+2,rCl.Height()-2);
			}
			else
			{
				rFirst.SetRect(nCloseOffset+1,1,nCloseOffset+nA+1,rCl.Height()-2);
				rPrev.SetRect(nCloseOffset+nA+1,1,nCloseOffset+2*nA+1,rCl.Height()-2);
				rNext.SetRect(nCloseOffset+2*nA+2,1,nCloseOffset+3*nA+2,rCl.Height()-2);
				rLast.SetRect(nCloseOffset+3*nA+2,1,nCloseOffset+4*nA+2,rCl.Height()-2);
			}
		}
		else
		{
			if(nBtns<4)
			{
				rPrev.SetRect(nCloseOffset+1,2,nCloseOffset+nA+1,rCl.Height()-1);
				rNext.SetRect(nCloseOffset+nA+2,2,nCloseOffset+2*nA+2,rCl.Height()-1);
			}
			else
			{
				rFirst.SetRect(nCloseOffset+1,2,nCloseOffset+nA+1,rCl.Height()-1);
				rPrev.SetRect(nCloseOffset+nA+1,2,nCloseOffset+2*nA+1,rCl.Height()-1);
				rNext.SetRect(nCloseOffset+2*nA+2,2,nCloseOffset+3*nA+2,rCl.Height()-1);
				rLast.SetRect(nCloseOffset+3*nA+2,2,nCloseOffset+4*nA+2,rCl.Height()-1);
			}
		}

		if(nBtns>=4)
		{
			CPoint ptFirst(rFirst.left+(rFirst.Width()-8)/2,rFirst.top+(rFirst.Height()-7)/2);
			if(fRTL && m_hBmpBkRightSpin)
				DrawBk(dcMem,rFirst,m_hBmpBkRightSpin,m_fIsRightImageHorLayout,m_mrgnRight,m_nFirstState-1);
			else if(m_hBmpBkLeftSpin)
				DrawBk(dcMem,rFirst,m_hBmpBkLeftSpin,m_fIsLeftImageHorLayout,m_mrgnLeft,m_nFirstState-1);
			else
			{
				if(m_nFirstState==BNST_PRESSED)
					dcMem.DrawFrameControl(rFirst,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
				else
					dcMem.DrawFrameControl(rFirst,DFC_BUTTON,DFCS_BUTTONPUSH);
			}
			if(fRTL)
				DrawGlyph(dcMem,ptFirst,2,m_nFirstState-1);
			else
				DrawGlyph(dcMem,ptFirst,0,m_nFirstState-1);
				
			CPoint ptLast(rLast.left+(rLast.Width()-8)/2,rLast.top+(rLast.Height()-7)/2);
			if(fRTL && m_hBmpBkLeftSpin)
				DrawBk(dcMem,rLast,m_hBmpBkLeftSpin,m_fIsLeftImageHorLayout,m_mrgnLeft,m_nLastState-1);
			else if(m_hBmpBkRightSpin)
				DrawBk(dcMem,rLast,m_hBmpBkRightSpin,m_fIsRightImageHorLayout,m_mrgnRight,m_nLastState-1);
			else
			{
				if(m_nLastState==BNST_PRESSED)
					dcMem.DrawFrameControl(rLast,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
				else
					dcMem.DrawFrameControl(rLast,DFC_BUTTON,DFCS_BUTTONPUSH);
			}
			if(fRTL)
				DrawGlyph(dcMem,ptLast,5,m_nLastState-1);
			else
				DrawGlyph(dcMem,ptLast,3,m_nLastState-1);
		}

		CPoint ptPrev(rPrev.left+(rPrev.Width()-8)/2,rPrev.top+(rPrev.Height()-7)/2);
		if(fRTL && m_hBmpBkRightSpin)
			DrawBk(dcMem,rPrev,m_hBmpBkRightSpin,m_fIsRightImageHorLayout,m_mrgnRight,m_nPrevState-1);
		else if(m_hBmpBkLeftSpin)
			DrawBk(dcMem,rPrev,m_hBmpBkLeftSpin,m_fIsLeftImageHorLayout,m_mrgnLeft,m_nPrevState-1);
		else
		{
			if(m_nPrevState==BNST_PRESSED)
				dcMem.DrawFrameControl(rPrev,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
			else
				dcMem.DrawFrameControl(rPrev,DFC_BUTTON,DFCS_BUTTONPUSH);
		}
		if(fRTL)
			DrawGlyph(dcMem,ptPrev,3,m_nPrevState-1);
		else
			DrawGlyph(dcMem,ptPrev,1,m_nPrevState-1);

		CPoint ptNext(rNext.left+(rNext.Width()-8)/2,rNext.top+(rNext.Height()-7)/2);
		if(fRTL && m_hBmpBkLeftSpin)
			DrawBk(dcMem,rNext,m_hBmpBkLeftSpin,m_fIsLeftImageHorLayout,m_mrgnLeft,m_nNextState-1);
		else if(m_hBmpBkRightSpin)
			DrawBk(dcMem,rNext,m_hBmpBkRightSpin,m_fIsRightImageHorLayout,m_mrgnRight,m_nNextState-1);
		else
		{
			rNext.left -= 1;
			if(m_nNextState==BNST_PRESSED)
				dcMem.DrawFrameControl(rNext,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
			else
				dcMem.DrawFrameControl(rNext,DFC_BUTTON,DFCS_BUTTONPUSH);
		}
		if(fRTL)
			DrawGlyph(dcMem,ptNext,4,m_nNextState-1);
		else
			DrawGlyph(dcMem,ptNext,2,m_nNextState-1);
	}

	// draw black lines
	int nOffsetX = rAll.Width();

	CPoint pts[4];
	if(fAfter)
	{
		if(GetStyle()&CTCS_TOP)
		{
			if(m_nItemSelected==-1)
			{
				pts[0] = CPoint(0,rCl.bottom-2); 
				pts[1] = CPoint(0,rCl.bottom-2); 
				pts[2] = CPoint(0,rCl.bottom-2); 
				pts[3] = CPoint(rCl.right-nOffsetX,rCl.bottom-2);
			}
			else
			{
				if(m_aItems[m_nItemSelected]->m_bShape)
				{
					pts[0] = CPoint(0,rCl.bottom-2); 
					pts[1] = CPoint(m_aItems[m_nItemSelected]->m_rect.left,rCl.bottom-2); 
					pts[2] = CPoint(m_aItems[m_nItemSelected]->m_rect.right,rCl.bottom-2); 
					pts[3] = CPoint(rCl.right-nOffsetX,rCl.bottom-2);
				}
				else
				{
					pts[0] = CPoint(0,rCl.bottom-2); 
					pts[1] = CPoint(0,rCl.bottom-2); 
					pts[2] = CPoint(0,rCl.bottom-2); 
					pts[3] = CPoint(rCl.right-nOffsetX,rCl.bottom-2);
				}
			}
		}
		else
		{
			if(m_nItemSelected==-1)
			{
				pts[0] = CPoint(0,1); 
				pts[1] = CPoint(0,1); 
				pts[2] = CPoint(0,1); 
				pts[3] = CPoint(rCl.right-nOffsetX,1);
			}
			else
			{
				if(m_aItems[m_nItemSelected]->m_bShape)
				{
					pts[0] = CPoint(0,1); 
					pts[1] = CPoint(m_aItems[m_nItemSelected]->m_rect.left,1); 
					pts[2] = CPoint(m_aItems[m_nItemSelected]->m_rect.right,1); 
					pts[3] = CPoint(rCl.right-nOffsetX,1);
				}
				else
				{
					pts[0] = CPoint(0,1); 
					pts[1] = CPoint(0,1); 
					pts[2] = CPoint(0,1); 
					pts[3] = CPoint(rCl.right-nOffsetX,1);
				}
			}
		}
	}
	else
	{
		if(GetStyle()&CTCS_TOP)
		{
			if(m_nItemSelected==-1)
			{
				pts[0] = CPoint(nOffsetX,rCl.bottom-2); 
				pts[1] = CPoint(nOffsetX,rCl.bottom-2); 
				pts[2] = CPoint(nOffsetX,rCl.bottom-2); 
				pts[3] = CPoint(rCl.right,rCl.bottom-2);
			}
			else
			{
				if(m_aItems[m_nItemSelected]->m_bShape)
				{
					pts[0] = CPoint(nOffsetX,rCl.bottom-2); 
					pts[1] = CPoint(m_aItems[m_nItemSelected]->m_rect.left,rCl.bottom-2); 
					pts[2] = CPoint(m_aItems[m_nItemSelected]->m_rect.right,rCl.bottom-2); 
					pts[3] = CPoint(rCl.right,rCl.bottom-2);
				}
				else
				{
					pts[0] = CPoint(nOffsetX,rCl.bottom-2); 
					pts[1] = CPoint(nOffsetX,rCl.bottom-2); 
					pts[2] = CPoint(nOffsetX,rCl.bottom-2); 
					pts[3] = CPoint(rCl.right,rCl.bottom-2);
				}
			}
		}
		else
		{
			if(m_nItemSelected==-1)
			{
				pts[0] = CPoint(nOffsetX,1); 
				pts[1] = CPoint(nOffsetX,1); 
				pts[2] = CPoint(nOffsetX,1); 
				pts[3] = CPoint(rCl.right,1);
			}
			else
			{
				if(m_aItems[m_nItemSelected]->m_bShape)
				{
					pts[0] = CPoint(nOffsetX,1); 
					pts[1] = CPoint(m_aItems[m_nItemSelected]->m_rect.left,1); 
					pts[2] = CPoint(m_aItems[m_nItemSelected]->m_rect.right,1); 
					pts[3] = CPoint(rCl.right,1);
				}
				else
				{
					pts[0] = CPoint(nOffsetX,1); 
					pts[1] = CPoint(nOffsetX,1); 
					pts[2] = CPoint(nOffsetX,1); 
					pts[3] = CPoint(rCl.right,1);
				}
			}
		}
	}
		
	CPen* pOldPen = dcMem.SelectObject(&blackPen);
	dcMem.MoveTo(pts[0]);
	dcMem.LineTo(pts[1]);
	dcMem.MoveTo(pts[2]);
	dcMem.LineTo(pts[3]);
	dcMem.SelectObject(pOldPen);
	
	if(m_nButtonIDDown>=0 && (GetCursor()==m_hCursorMove || GetCursor()==m_hCursorCopy))
	{
		// Draw drag destination marker
		CPen* pOldPen = dcMem.SelectObject(&blackPen);
		int x;
		if(m_nItemDragDest==m_aItems.GetSize())
			x = m_aItems[m_nItemDragDest-1]->m_rectText.right + rCl.Height()/4-3;
		else
			x = m_aItems[m_nItemDragDest]->m_rectText.left - rCl.Height()/4-3;
		if(x>=rCl.right-7)
			x = rCl.right-7;
		dcMem.MoveTo(x,1);
		dcMem.LineTo(x+7,1);
		dcMem.MoveTo(x+1,2);
		dcMem.LineTo(x+6,2);
		dcMem.MoveTo(x+2,3);
		dcMem.LineTo(x+5,3);
		dcMem.MoveTo(x+3,4);
		dcMem.LineTo(x+4,4);
		dcMem.SelectObject(pOldPen);
	}
	if(IsVertical())
	{
		POINT pts[3];
		if(fRTL)
		{
			pts[0].x = -1;
			pts[0].y = rCl.Width();
			pts[1].x = -1;
			pts[1].y = 0;
			pts[2].x = rCl.Height()-1;
			pts[2].y = rCl.Width();
			::PlgBlt(dc.m_hDC,pts,dcMem.m_hDC,-1,0,rCl.Width(),rCl.Height(),NULL,0,0);
		}
		else
		{
			pts[0].x = 0;
			pts[0].y = rCl.Width();
			pts[1].x = 0;
			pts[1].y = 0;
			pts[2].x = rCl.Height();
			pts[2].y = rCl.Width();
			::PlgBlt(dc.m_hDC,pts,dcMem.m_hDC,0,0,rCl.Width(),rCl.Height(),NULL,0,0);
		}
	}
	else
		dc.BitBlt(rCl.left,rCl.top,rCl.Width(),rCl.Height(),&dcMem,rCl.left,rCl.top,SRCCOPY);
	dcMem.SelectObject(pOldBmp);
}

void CCustomTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(cx && cy)
		RecalcLayout(RECALC_RESIZED, m_nItemSelected);
	Invalidate();
}

void CCustomTabCtrl::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);
	Invalidate();
};


LRESULT CCustomTabCtrl::OnSizeParent(WPARAM, LPARAM lParam)
{
	AFX_SIZEPARENTPARAMS* pParams = reinterpret_cast<AFX_SIZEPARENTPARAMS*>(lParam);

	CRect r;
	GetWindowRect(r);


	if(IsVertical())
	{
		if(GetStyle()&CTCS_TOP) // left
		{
			pParams->rect.left += r.Width();
			MoveWindow(pParams->rect.left-r.Width(), pParams->rect.top, r.Width(), pParams->rect.bottom-pParams->rect.top, TRUE);
		}
		else // right
		{
			pParams->rect.right -= r.Width();
			MoveWindow(pParams->rect.right, pParams->rect.top, r.Width(), pParams->rect.bottom-pParams->rect.top, TRUE);
		}
	}
	else
	{
		if(GetStyle()&CTCS_TOP)
		{
			pParams->rect.top += r.Height();
			MoveWindow(pParams->rect.left, pParams->rect.top-r.Height(), pParams->rect.right-pParams->rect.left, r.Height(), TRUE);
		}
		else
		{
			pParams->rect.bottom -= r.Height();
			MoveWindow(pParams->rect.left, pParams->rect.bottom, pParams->rect.right-pParams->rect.left, r.Height(), TRUE);
		}
	}

	return 0;

}

void CCustomTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nHitTest = HitTest(point);
	NotifyParent(CTCN_CLICK,nHitTest,point);
	ProcessLButtonDown(nHitTest,nFlags,point);
	CWnd::OnLButtonDown(nFlags,point);
}

void CCustomTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nHTRet = ProcessLButtonDown(HitTest(point),nFlags,point);
	if(nHTRet>=0)
	{
		m_nButtonIDDown = CTCID_NOBUTTON;
		if(nHTRet==HitTest(point))
			EditLabel(nHTRet,TRUE);
	}
	NotifyParent(CTCN_DBLCLK,HitTest(point),point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

int CCustomTabCtrl::ProcessLButtonDown(int nHitTest, UINT nFlags, CPoint point)
{
	SetCapture();
	switch(nHitTest)
	{
	case CTCHT_NOWHERE:
		m_nButtonIDDown = CTCID_NOBUTTON;
		break;
	case CTCHT_ONFIRSTBUTTON:
		{
			m_nButtonIDDown = CTCID_FIRSTBUTTON;
			m_nFirstState = BNST_PRESSED;
			RecalcLayout(RECALC_FIRST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONPREVBUTTON:
		{
			m_nButtonIDDown = CTCID_PREVBUTTON;
			m_nPrevState = BNST_PRESSED;
			RecalcLayout(RECALC_PREV_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONNEXTBUTTON:
		{
			m_nButtonIDDown = CTCID_NEXTBUTTON;
			m_nNextState = BNST_PRESSED;
			RecalcLayout(RECALC_NEXT_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONLASTBUTTON:
		{
			m_nButtonIDDown = CTCID_LASTBUTTON;
			m_nLastState = BNST_PRESSED;
			RecalcLayout(RECALC_LAST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			SetTimer(1,100,NULL);
		}
		break;
	case CTCHT_ONCLOSEBUTTON:
		{
			m_nButtonIDDown = CTCID_CLOSEBUTTON;
			m_nCloseState = BNST_PRESSED;
			RecalcLayout(RECALC_CLOSE_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
		}
		break;
	default:
		{
			DWORD dwStyle = GetStyle();
			if(((dwStyle&CTCS_DRAGMOVE) && !(nFlags&MK_CONTROL) && m_hCursorMove) || 
				((dwStyle&CTCS_DRAGCOPY) && (nFlags&MK_CONTROL) && m_hCursorCopy))
			{
				m_nButtonIDDown = nHitTest;
				m_nItemDragDest = CTCID_NOBUTTON;
				SetTimer(2,300,NULL);
			}
			else
				m_nButtonIDDown = CTCID_NOBUTTON;

			if((GetStyle()&CTCS_MULTIHIGHLIGHT) && (nFlags&MK_CONTROL))
				HighlightItem(nHitTest,TRUE,nFlags&MK_CONTROL);
			else
			{
				BOOL bNotify = nHitTest!=m_nItemSelected;
				SetCurSel(nHitTest,TRUE,nFlags&MK_CONTROL);
				if(bNotify)
					NotifyParent(CTCN_SELCHANGE,m_nItemSelected,point);
			}
			for(int i=0; i<m_aItems.GetSize();i++)
			{
				if(m_aItems[i]->m_fHighlightChanged)
					NotifyParent(CTCN_HIGHLIGHTCHANGE,i,point);
			}
		}
		break;
	}
	return nHitTest;
}

BOOL CCustomTabCtrl::NotifyParent(UINT code, int nItem, CPoint pt)
{
	CTC_NMHDR nmh;
	memset(&nmh,0,sizeof(CTC_NMHDR));
	nmh.hdr.hwndFrom = GetSafeHwnd();
	nmh.hdr.idFrom = GetDlgCtrlID();
	nmh.hdr.code = code;
	nmh.nItem = nItem;
	nmh.ptHitTest = pt;
	if(nItem>=0)
	{
		_tcscpy(nmh.pszText,m_aItems[nItem]->m_sText);
		nmh.lParam = m_aItems[nItem]->m_lParam;
		nmh.rItem = m_aItems[nItem]->m_rectText;
		nmh.fSelected = m_aItems[nItem]->m_fSelected;
		nmh.fHighlighted = m_aItems[nItem]->m_fHighlighted;
	}
	return (BOOL)GetParent()->SendMessage(WM_NOTIFY,GetDlgCtrlID(),(LPARAM)&nmh);
}

void CCustomTabCtrl::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	if(m_nPrevState || m_nNextState || m_nFirstState || m_nLastState || m_nCloseState)
	{
		if(m_nCloseState)
			m_nCloseState = BNST_NORMAL;
		if(m_nPrevState)
		{
			m_nPrevState = BNST_NORMAL;
			m_nNextState = BNST_NORMAL;
		}
		if(m_nFirstState)
		{
			m_nFirstState = BNST_NORMAL;
			m_nLastState = BNST_NORMAL;
		}
		Invalidate(FALSE);
		KillTimer(1);
	}
	if(m_nButtonIDDown>=0)
	{
		if((GetCursor()==m_hCursorCopy) && (GetKeyState(VK_CONTROL)&0x8000))
			CopyItem(m_nButtonIDDown,m_nItemDragDest, TRUE);
		else if((GetCursor()==m_hCursorMove) && !(GetKeyState(VK_CONTROL)&0x8000))
			MoveItem(m_nButtonIDDown,m_nItemDragDest, TRUE);
	}
	m_nButtonIDDown = CTCID_NOBUTTON;
	m_nItemDragDest = CTCID_NOBUTTON;
	ReleaseCapture();
}

void CCustomTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACKMOUSEEVENT trackmouseevent;
	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0;
	_TrackMouseEvent(&trackmouseevent);

	int nHitTest = HitTest(point);
	
	if(m_nFirstState)
	{
		if(nHitTest==CTCHT_ONFIRSTBUTTON)
		{
			if(m_nButtonIDDown==CTCID_FIRSTBUTTON)
				m_nFirstState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nFirstState = BNST_HOT;
			else
				m_nFirstState = BNST_NORMAL;
		}
		else
			m_nFirstState = BNST_NORMAL;
		Invalidate(FALSE);
	}
	if(m_nPrevState)
	{
		if(nHitTest==CTCHT_ONPREVBUTTON)
		{
			if(m_nButtonIDDown==CTCID_PREVBUTTON)
				m_nPrevState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nPrevState = BNST_HOT;
			else
				m_nPrevState = BNST_NORMAL;
		}
		else
			m_nPrevState = BNST_NORMAL;
		Invalidate(FALSE);
	}
	if(m_nNextState)
	{
		if(nHitTest==CTCHT_ONNEXTBUTTON)
		{
			if(m_nButtonIDDown==CTCID_NEXTBUTTON)
				m_nNextState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nNextState = BNST_HOT;
			else
				m_nNextState = BNST_NORMAL;
		}
		else
			m_nNextState = BNST_NORMAL;
		Invalidate(FALSE);
	}
	if(m_nLastState)
	{
		if(nHitTest==CTCHT_ONLASTBUTTON)
		{
			if(m_nButtonIDDown==CTCID_LASTBUTTON)
				m_nLastState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nLastState = BNST_HOT;
			else
				m_nLastState = BNST_NORMAL;
		}
		else
			m_nLastState = BNST_NORMAL;
		Invalidate(FALSE);
	}
	if(m_nCloseState)
	{
		if(nHitTest==CTCHT_ONCLOSEBUTTON)
		{
			if(m_nButtonIDDown==CTCID_CLOSEBUTTON)
				m_nCloseState = BNST_PRESSED;
			else if(m_nButtonIDDown==CTCID_NOBUTTON && !(nFlags&MK_LBUTTON))
				m_nCloseState = BNST_HOT;
			else
				m_nCloseState = BNST_NORMAL;
		}
		else
			m_nCloseState = BNST_NORMAL;
		Invalidate(FALSE);
	}
	if(m_nButtonIDDown>=0 && m_nItemDragDest>=0)
	{
		CRect rCl;
		GetClientRect(&rCl);
		int x = point.x;
		if(IsVertical())
		{
			x = rCl.Height()-point.y;
			rCl.SetRect(0,0,rCl.Height(),rCl.Width());
		}
		if(m_nItemDragDest>=m_aItems.GetSize())
			m_nItemDragDest = m_aItems.GetSize()-1;
		
		int x1 = m_aItems[m_nItemDragDest]->m_rectText.left - rCl.Height()/4;
		int x2 = m_aItems[m_nItemDragDest]->m_rectText.right + rCl.Height()/4;
		if(x>=rCl.right)
		{
			m_nItemDragDest++;
			if(m_nItemDragDest>=m_aItems.GetSize())
				RecalcLayout(RECALC_NEXT_PRESSED,m_aItems.GetSize()-1);
			else
				RecalcLayout(RECALC_NEXT_PRESSED,m_nItemDragDest);
			Invalidate(FALSE);
		}
		else if(x>=x2)
		{
			m_nItemDragDest++;
			if(m_nItemDragDest>=m_aItems.GetSize())
				RecalcLayout(RECALC_ITEM_SELECTED,m_aItems.GetSize()-1);
			else
				RecalcLayout(RECALC_ITEM_SELECTED,m_nItemDragDest);
			Invalidate(FALSE);
		}
		else if(x<x1)
		{
			if(m_nItemDragDest>0)
				m_nItemDragDest--;
			
			RecalcLayout(RECALC_ITEM_SELECTED,m_nItemDragDest);
			Invalidate(FALSE);
		}
	}
}

LRESULT CCustomTabCtrl::OnMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	if(m_nFirstState || m_nPrevState || m_nCloseState)
	{
		if(m_nCloseState)
			m_nCloseState = BNST_NORMAL;
		if(m_nPrevState)
		{
			m_nPrevState = BNST_NORMAL;
			m_nNextState = BNST_NORMAL;
		}
		if(m_nFirstState)
		{
			m_nFirstState = BNST_NORMAL;
			m_nLastState = BNST_NORMAL;
		}
		Invalidate(FALSE);
		KillTimer(1);
	}
	return 0;
}

void CCustomTabCtrl::OnUpdateEdit() 
{
	if(m_ctrlEdit.m_hWnd)
	{
		m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
		RecalcLayout(RECALC_EDIT_RESIZED,m_nItemSelected);
		Invalidate(FALSE);
	}
}

LRESULT CCustomTabCtrl::OnThemeChanged(WPARAM /*wParam*/, LPARAM /*lParam*/) 
{
	::DeleteObject(m_hBmpBkLeftSpin);
	m_hBmpBkLeftSpin = NULL;
	::DeleteObject(m_hBmpBkRightSpin);
	m_hBmpBkRightSpin = NULL;

	HBITMAP hBmpGlyph = NULL;
	CDC dcGlyph;
	dcGlyph.CreateCompatibleDC(NULL);
	CBitmap* pOldBmpGlyph = NULL;

	try
	{
		CThemeUtil tm;
		if(!tm.OpenThemeData(m_hWnd, L"SPIN"))
			AfxThrowUserException();

		{
			// left spin background
			int nBkType;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_BGTYPE,&nBkType))
				AfxThrowUserException();
			if(nBkType!=BT_IMAGEFILE)
				AfxThrowUserException();

			int nImageCount;
			if(!tm.GetThemeInt(SPNP_DOWNHORZ,0,TMT_IMAGECOUNT,&nImageCount))
				AfxThrowUserException();
			if(nImageCount!=4)
				AfxThrowUserException();

			WCHAR szSpinBkLeftBitmapFilename[MAX_PATH];
			if(!tm.GetThemeFilename(SPNP_DOWNHORZ,0,TMT_IMAGEFILE,szSpinBkLeftBitmapFilename,MAX_PATH))
				AfxThrowUserException();
			m_hBmpBkLeftSpin = tm.LoadBitmap(szSpinBkLeftBitmapFilename);
			if(!m_hBmpBkLeftSpin)
				AfxThrowUserException();

			int nLeftImageLayout;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_IMAGELAYOUT,&nLeftImageLayout))
				AfxThrowUserException();
			if(nLeftImageLayout==IL_VERTICAL)
				m_fIsLeftImageHorLayout = FALSE;
			else
				m_fIsLeftImageHorLayout = TRUE;
			
			if(!tm.GetThemeMargins(SPNP_DOWNHORZ,0,TMT_SIZINGMARGINS,&m_mrgnLeft))
				AfxThrowUserException();
		}
		{
			// right spin background
			int nBkType;
			if(!tm.GetThemeEnumValue(SPNP_UPHORZ,0,TMT_BGTYPE,&nBkType))
				AfxThrowUserException();
			if(nBkType!=BT_IMAGEFILE)
				AfxThrowUserException();

			int nImageCount;
			if(!tm.GetThemeInt(SPNP_UPHORZ,0,TMT_IMAGECOUNT,&nImageCount))
				AfxThrowUserException();
			if(nImageCount!=4)
				AfxThrowUserException();

			WCHAR szSpinBkRightBitmapFilename[MAX_PATH];
			if(!tm.GetThemeFilename(SPNP_UPHORZ,0,TMT_IMAGEFILE,szSpinBkRightBitmapFilename,MAX_PATH))
				AfxThrowUserException();
			
			m_hBmpBkRightSpin = tm.LoadBitmap(szSpinBkRightBitmapFilename);
			if(!m_hBmpBkRightSpin)
				AfxThrowUserException();
	
			int nRightImageLayout;
			if(!tm.GetThemeEnumValue(SPNP_UPHORZ,0,TMT_IMAGELAYOUT,&nRightImageLayout))
				AfxThrowUserException();
			if(nRightImageLayout==IL_VERTICAL)
				m_fIsRightImageHorLayout = FALSE;
			else
				m_fIsRightImageHorLayout = TRUE;

			if(!tm.GetThemeMargins(SPNP_UPHORZ,0,TMT_SIZINGMARGINS,&m_mrgnRight))
				AfxThrowUserException();
		}
		{
			// glyph color
			int nGlyphType;
			if(!tm.GetThemeEnumValue(SPNP_DOWNHORZ,0,TMT_GLYPHTYPE,&nGlyphType))
				AfxThrowUserException();
			
			if(nGlyphType==GT_IMAGEGLYPH)
			{
				COLORREF rgbTransGlyph = RGB(255,0,255);
				if(!tm.GetThemeColor(SPNP_DOWNHORZ,0,TMT_GLYPHTRANSPARENTCOLOR,&rgbTransGlyph))
					AfxThrowUserException();
				WCHAR szSpinGlyphIconFilename[MAX_PATH];
				if(!tm.GetThemeFilename(SPNP_DOWNHORZ,0,TMT_GLYPHIMAGEFILE,szSpinGlyphIconFilename,MAX_PATH))
					AfxThrowUserException();
				hBmpGlyph = tm.LoadBitmap(szSpinGlyphIconFilename);
				if(!hBmpGlyph)
					AfxThrowUserException();

				CBitmap* pBmp = CBitmap::FromHandle(hBmpGlyph);
				if(pBmp==NULL)
					AfxThrowUserException();
				pOldBmpGlyph = dcGlyph.SelectObject(pBmp);
				BITMAP bm;
				pBmp->GetBitmap(&bm);
				m_rgbGlyph[0] = rgbTransGlyph;
				m_rgbGlyph[1] = rgbTransGlyph;
				m_rgbGlyph[2] = rgbTransGlyph;
				m_rgbGlyph[3] = rgbTransGlyph;
				if(m_fIsLeftImageHorLayout)
				{
					for(int i=0;i<bm.bmWidth;i++)
					{
						if(i<bm.bmWidth/4 && m_rgbGlyph[0]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[0]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth/4-1 && m_rgbGlyph[0]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmWidth/4 && i<bm.bmWidth/2 && m_rgbGlyph[1]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[1]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth/2-1 && m_rgbGlyph[1]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmWidth/2 && i<3*bm.bmWidth/4 && m_rgbGlyph[2]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[2]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==3*bm.bmWidth/4-1 && m_rgbGlyph[2]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=3*bm.bmWidth/4 && i<bm.bmWidth && m_rgbGlyph[3]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmHeight;j++)
							{
								if((m_rgbGlyph[3]=dcGlyph.GetPixel(i,j))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmWidth-1 && m_rgbGlyph[3]==rgbTransGlyph)
								AfxThrowUserException();
						}
					}
				}
				else
				{
					for(int i=0;i<bm.bmHeight;i++)
					{
						if(i<bm.bmHeight/4 && m_rgbGlyph[0]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[0] = dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight/4-1 && m_rgbGlyph[0]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmHeight/4 && i<bm.bmHeight/2 && m_rgbGlyph[1]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[1]=dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight/2-1 && m_rgbGlyph[1]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=bm.bmHeight/2 && i<3*bm.bmHeight/4 && m_rgbGlyph[2]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[2] = dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==3*bm.bmHeight/4-1 && m_rgbGlyph[2]==rgbTransGlyph)
								AfxThrowUserException();
						}
						else if(i>=3*bm.bmHeight/4 && i<bm.bmHeight && m_rgbGlyph[3]==rgbTransGlyph)
						{
							for(int j=0;j<bm.bmWidth;j++)
							{
								if((m_rgbGlyph[3]=dcGlyph.GetPixel(j,i))!=rgbTransGlyph)
									break;
							}
							if(i==bm.bmHeight-1 && m_rgbGlyph[3]==rgbTransGlyph)
								AfxThrowUserException();
						}
					}
				}
				dcGlyph.SelectObject(pOldBmpGlyph);
				pOldBmpGlyph = NULL;
				::DeleteObject(hBmpGlyph);
				hBmpGlyph = NULL;
			}
			else if(nGlyphType==GT_FONTGLYPH)
			{
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_NORMAL,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[0]))
					AfxThrowUserException();
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_HOT,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[1]))
					AfxThrowUserException();
				if(!tm.GetThemeColor(SPNP_UPHORZ,UPHZS_PRESSED,TMT_GLYPHTEXTCOLOR,&m_rgbGlyph[2]))
					AfxThrowUserException();	
			}
			else
				AfxThrowUserException();
		}
		tm.CloseThemeData();
	}
	catch(CUserException* e)
	{
		e->Delete();
		::DeleteObject(m_hBmpBkLeftSpin);
		m_hBmpBkLeftSpin = NULL;
		::DeleteObject(m_hBmpBkRightSpin);
		m_hBmpBkRightSpin = NULL;
		if(pOldBmpGlyph)
			dcGlyph.SelectObject(pOldBmpGlyph);
		::DeleteObject(hBmpGlyph);
		hBmpGlyph = NULL;
	}
	return 0;
}

void CCustomTabCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	CWnd::OnTimer(nIDEvent);
	if(nIDEvent==1)
	{
		if(m_nFirstState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nFirstState = BNST_PRESSED;
			RecalcLayout(RECALC_FIRST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;

		}
		if(m_nPrevState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nPrevState = BNST_PRESSED;
			RecalcLayout(RECALC_PREV_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;

		}
		if(m_nNextState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nNextState = BNST_PRESSED;
			RecalcLayout(RECALC_NEXT_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;
		}
		if(m_nLastState==BNST_PRESSED && ::GetTickCount()-m_dwLastRepeatTime>=REPEAT_TIMEOUT)
		{
			m_nLastState = BNST_PRESSED;
			RecalcLayout(RECALC_LAST_PRESSED,m_nItemSelected);
			Invalidate(FALSE);
			m_dwLastRepeatTime = ::GetTickCount();
			return;
		}
	}
	else if(nIDEvent==2)
	{
		KillTimer(2);
		if(m_nButtonIDDown>=0)
		{
			if(m_nItemDragDest==CTCID_NOBUTTON)
				m_nItemDragDest = m_nButtonIDDown;
			SetTimer(2,10,NULL);
			DWORD dwStyle = GetStyle();
			if((dwStyle&CTCS_DRAGCOPY) && (GetKeyState(VK_CONTROL)&0x8000))
				SetCursor(m_hCursorCopy);
			else if((dwStyle&CTCS_DRAGMOVE) && !(GetKeyState(VK_CONTROL)&0x8000))
				SetCursor(m_hCursorMove);
			else
			{
				m_nButtonIDDown = CTCID_NOBUTTON;
				ReleaseCapture();
			}
			Invalidate(FALSE);
		}
	}
}

void CCustomTabCtrl::SetControlFont(const LOGFONT& lf, BOOL fRedraw)
{
	if(m_Font.m_hObject)
	{
		DeleteObject(m_Font);
		m_Font.m_hObject = NULL;
	}

	if(m_FontSelected.m_hObject)
	{
		DeleteObject(m_FontSelected);
		m_FontSelected.m_hObject = NULL;
	}

	if(!m_Font.CreateFontIndirect(&lf))
		m_Font.CreateFontIndirect(&lf_default);


	LOGFONT lfSel;
	m_Font.GetLogFont(&lfSel);
	lfSel.lfWeight = FW_BOLD;
	m_FontSelected.CreateFontIndirect(&lfSel);

	if(fRedraw)
	{
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
		Invalidate();
	}
}

int CCustomTabCtrl::InsertItem(int nItem, CString sText, LPARAM lParam)
{
	if(nItem<0 || nItem>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(sText.GetLength()>MAX_LABEL_TEXT-1)
		return CTCERR_TEXTTOOLONG;

	CCustomTabCtrlItem* pItem = new CCustomTabCtrlItem(sText,lParam);
	if(pItem==NULL)
		return CTCERR_OUTOFMEMORY;

	try
	{
		m_aItems.InsertAt(nItem,pItem);
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		delete pItem;
		return CTCERR_OUTOFMEMORY;
	}

	if(m_nItemSelected>=nItem)
		m_nItemSelected++;

	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=m_aItems.GetSize()-1; i>nItem; i--)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i);
			m_ctrlToolTip.DelTool(this,i);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i+1);
		}
		m_ctrlToolTip.DelTool(this,nItem+1);
	}
	
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);

	return nItem;
}

int CCustomTabCtrl::MoveItem(int nItemSrc, int nItemDst)
{
	return MoveItem(nItemSrc, nItemDst, FALSE);
}

int CCustomTabCtrl::MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
	if(nItemSrc<0||nItemSrc>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(nItemDst<0||nItemDst>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(nItemSrc==nItemDst || nItemSrc==nItemDst-1)
	{
		Invalidate(FALSE);
		return nItemSrc;
	};

	CCustomTabCtrlItem *pItem = m_aItems[nItemSrc];
	
	// remove item from old place
	CString sOldTooltip;
	if(m_ctrlToolTip.m_hWnd)
	{
		m_ctrlToolTip.GetText(sOldTooltip,this,nItemSrc+1);
		for(int i=nItemSrc+1; i< m_aItems.GetSize(); i++)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i+1);
			m_ctrlToolTip.DelTool(this,i);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i);
		}
	}

	m_aItems.RemoveAt(nItemSrc);

	// insert item in new place
	if(nItemDst>nItemSrc)
		nItemDst--;

	try
	{
		m_aItems.InsertAt(nItemDst,pItem);
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		delete pItem;
		if(fMouseSel)
			NotifyParent(CTCN_ITEMMOVE,nItemSrc,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}

	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=m_aItems.GetSize()-1; i>nItemDst; i--)
		{
			CString s;
			m_ctrlToolTip.GetText(s,this,i);
			m_ctrlToolTip.DelTool(this,i+1);
			m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i+1);
		}
		m_ctrlToolTip.DelTool(this,nItemDst+1);
		m_ctrlToolTip.AddTool(this,sOldTooltip,CRect(0,0,0,0),nItemDst+1);
	}
	
	m_nItemSelected = nItemDst;

	RecalcLayout(RECALC_ITEM_SELECTED,m_nItemSelected);
	Invalidate(FALSE);
	if(fMouseSel)
		NotifyParent(CTCN_ITEMMOVE,m_nItemSelected,CPoint(0,0));
	return nItemDst;
}

int CCustomTabCtrl::CopyItem(int nItemSrc, int nItemDst)
{
	return CopyItem(nItemSrc, nItemDst, FALSE);
}

int CCustomTabCtrl::CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel)
{
	if(nItemSrc<0||nItemSrc>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(nItemDst<0||nItemDst>m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	CString sDst;
	try
	{
		BOOL bAppendFlag=TRUE;
		int n = m_aItems[nItemSrc]->m_sText.GetLength();
		if(n>=4)
		{
			if(m_aItems[nItemSrc]->m_sText[n-1]==_T(')') && 
				m_aItems[nItemSrc]->m_sText[n-2]>_T('1') &&
				m_aItems[nItemSrc]->m_sText[n-2]<=_T('9') &&
				m_aItems[nItemSrc]->m_sText[n-3]==_T('('))
			{
				n = m_aItems[nItemSrc]->m_sText.GetLength()-3;
				bAppendFlag = FALSE;
			}
			else if(m_aItems[nItemSrc]->m_sText[n-1]==_T(')') && 
					m_aItems[nItemSrc]->m_sText[n-2]>=_T('0') &&
					m_aItems[nItemSrc]->m_sText[n-2]<=_T('9') &&
					m_aItems[nItemSrc]->m_sText[n-3]>=_T('1') &&
					m_aItems[nItemSrc]->m_sText[n-3]<=_T('9') &&
					m_aItems[nItemSrc]->m_sText[n-4]==_T('('))
			{
				n = m_aItems[nItemSrc]->m_sText.GetLength()-4;
				bAppendFlag = FALSE;
			}
		}
		int ndx = 1;
		while(1)
		{
			ndx++;
			if(bAppendFlag)
				sDst.Format(_T("%s (%d)"),(LPCTSTR)m_aItems[nItemSrc]->m_sText,ndx);
			else
				sDst.Format(_T("%s(%d)"),(LPCTSTR)m_aItems[nItemSrc]->m_sText.Left(n),ndx);
			int i;
			for(i=0;i<m_aItems.GetSize();i++)
			{
				if(m_aItems[i]->m_sText==sDst)
					break;
			}
			if(i==m_aItems.GetSize())
				break;
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		if(fMouseSel)
			NotifyParent(CTCN_OUTOFMEMORY,nItemSrc,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}

	int nRetItem = InsertItem(nItemDst,sDst,m_aItems[nItemSrc]->m_lParam);
	if(nRetItem>=0)
	{
		SetCurSel(nRetItem);
		if(fMouseSel)
			NotifyParent(CTCN_ITEMCOPY,nRetItem,CPoint(0,0));
	}
	else if(fMouseSel && nRetItem==CTCERR_OUTOFMEMORY)
		NotifyParent(CTCN_OUTOFMEMORY,nRetItem,CPoint(0,0));

	return nRetItem;
}

int CCustomTabCtrl::DeleteItem(int nItem)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	try
	{
		if(m_ctrlToolTip.m_hWnd)
		{
			for(int i=nItem; i<m_aItems.GetSize(); i++)
			{
				m_ctrlToolTip.DelTool(this,i+1);
				if(i!=m_aItems.GetSize()-1)
				{
					CString s;
					m_ctrlToolTip.GetText(s,this,i+2);
					m_ctrlToolTip.AddTool(this,s,CRect(0,0,0,0),i+1);
				}
			}
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		return CTCERR_OUTOFMEMORY;
	}

	if(m_aItems.GetSize()==1)
		m_nItemSelected = -1;
	else if(m_nItemSelected==nItem)
	{
		if(m_nItemSelected==m_aItems.GetSize()-1) // last item
		{
			m_nItemSelected--;
			m_aItems[m_nItemSelected]->m_fSelected = TRUE;	
		}
		else
			m_aItems[m_nItemSelected+1]->m_fSelected = TRUE;	
	}
	else if(m_nItemSelected>nItem)
		m_nItemSelected--;

	delete m_aItems[nItem];
	m_aItems.RemoveAt(nItem);

	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

void CCustomTabCtrl::DeleteAllItems()
{
	if(m_ctrlToolTip.m_hWnd)
	{
		for(int i=0; i< m_aItems.GetSize(); i++)
		{
			delete m_aItems[i];
			m_ctrlToolTip.DelTool(this,i+1);
		}
	}
	else
	{
		for(int i=0; i< m_aItems.GetSize(); i++)
			delete m_aItems[i];
	}

	m_aItems.RemoveAll();

	m_nItemSelected = -1;
		
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
}

int CCustomTabCtrl::SetCurSel(int nItem)
{
	return SetCurSel(nItem,FALSE,FALSE);
}

int CCustomTabCtrl::HighlightItem(int nItem, BOOL fHighlight)
{
	if(!(GetStyle()&CTCS_MULTIHIGHLIGHT))
		return CTCERR_NOMULTIHIGHLIGHTSTYLE;
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(m_nItemSelected==-1 && !fHighlight)
		return CTCERR_NOERROR;
	if(m_nItemSelected==-1)
	{
		SetCurSel(nItem);
		return CTCERR_NOERROR;
	}
	if(fHighlight==m_aItems[nItem]->m_fHighlighted || nItem==m_nItemSelected)
		return CTCERR_NOERROR;
	
	m_aItems[nItem]->m_fHighlighted = fHighlight;
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::GetItemText(int nItem, CString& sText)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	sText = m_aItems[nItem]->m_sText;
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::SetItemText(int nItem, CString sText)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	m_aItems[nItem]->m_sText = sText;
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::SetItemTooltipText(int nItem, CString sText)
{
	if(!(GetStyle()&CTCS_TOOLTIPS))
		return CTCERR_NOTOOLTIPSSTYLE;
	if(nItem>=CTCID_CLOSEBUTTON && nItem<m_aItems.GetSize())
	{
		if(m_ctrlToolTip.m_hWnd==NULL)
		{
			if(!m_ctrlToolTip.Create(this))
				return CTCERR_CREATETOOLTIPFAILED;
			m_ctrlToolTip.Activate(TRUE);
		}
		if(nItem>=0)
			nItem++;
		m_ctrlToolTip.DelTool(this,nItem);
		m_ctrlToolTip.AddTool(this,sText,CRect(0,0,0,0),nItem);
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
		Invalidate(FALSE);
		return CTCERR_NOERROR;
	}
	return CTCERR_INDEXOUTOFRANGE;
}

int CCustomTabCtrl::GetItemData(int nItem, DWORD& dwData)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	dwData = m_aItems[nItem]->m_lParam;
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::GetItemRect(int nItem, CRect& rect) const
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	rect = m_aItems[nItem]->m_rectText;
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::SetItemData(int nItem, DWORD dwData)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	m_aItems[nItem]->m_lParam = dwData;
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::IsItemHighlighted(int nItem)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	return (m_aItems[nItem]->m_fHighlighted)?1:0;
}

int	CCustomTabCtrl::HitTest(CPoint pt)
{
	CRect rCl;
	GetClientRect(&rCl);
	if(IsVertical())
	{
		rCl.SetRect(0,0,rCl.Height(),rCl.Width());
		pt = CPoint(rCl.Width()-pt.y,pt.x);
	}

	int nBtns = 0;
	if(m_nCloseState)
		nBtns++;
	if(m_nPrevState)
		nBtns += 2;
	if(m_nFirstState)
		nBtns += 2;
	int nA = rCl.Height()-3;

	int nCloseOffset = 0;
	if(m_nCloseState)
	{
		nCloseOffset = nA;
		CRect rClose(0,0,nA+1,rCl.Height());
		if(GetStyle()&CTCS_BUTTONSAFTER)
			rClose.SetRect(rCl.Width()-nA-1,0,rCl.Width(),rCl.Height());
		if(rClose.PtInRect(pt))
			return CTCHT_ONCLOSEBUTTON;
	}
	CRect rFirst,rPrev,rNext,rLast;
	if(GetStyle()&CTCS_BUTTONSAFTER)
	{
		if(nBtns==2||nBtns==3)
		{
			rNext.SetRect(rCl.Width()-nCloseOffset-nA-1,0,rCl.Width()-nCloseOffset,rCl.Height());
			rPrev.SetRect(rCl.Width()-nCloseOffset-2*nA-3,0,rCl.Width()-nCloseOffset-nA-1,rCl.Height());
		}
		else if(nBtns==4||nBtns==5)
		{
			rLast.SetRect(rCl.Width()-nCloseOffset-nA-1,0,rCl.Width()-nCloseOffset,rCl.Height());
			rNext.SetRect(rCl.Width()-nCloseOffset-2*nA-1,0,rCl.Width()-nCloseOffset-nA-1,rCl.Height());
			rPrev.SetRect(rCl.Width()-nCloseOffset-3*nA-2,0,rCl.Width()-nCloseOffset-2*nA-1,rCl.Height());
			rFirst.SetRect(rCl.Width()-nCloseOffset-4*nA-3,0,rCl.Width()-nCloseOffset-3*nA-2,rCl.Height());
		}
	}
	else
	{
		if(nBtns==2||nBtns==3)
		{
			rPrev.SetRect(nCloseOffset,0,nCloseOffset+nA+1,rCl.Height());
			rNext.SetRect(nCloseOffset+nA+1,0,nCloseOffset+2*nA+3,rCl.Height());
		}
		else if(nBtns==4||nBtns==5)
		{
			rFirst.SetRect(nCloseOffset,0,nCloseOffset+nA+1,rCl.Height());
			rPrev.SetRect(nCloseOffset+nA+1,0,nCloseOffset+2*nA+1,rCl.Height());
			rNext.SetRect(nCloseOffset+2*nA+1,0,nCloseOffset+3*nA+2,rCl.Height());
			rLast.SetRect(nCloseOffset+3*nA+2,0,nCloseOffset+4*nA+3,rCl.Height());
		}
	}

	if(nBtns>=4 && m_nFirstState && rFirst.PtInRect(pt))
		return CTCHT_ONFIRSTBUTTON;

	if(m_nPrevState && rPrev.PtInRect(pt))
		return CTCHT_ONPREVBUTTON;
	
	if(m_nNextState && rNext.PtInRect(pt))
		return CTCHT_ONNEXTBUTTON;
	
	if(nBtns>=4 && m_nLastState && rLast.PtInRect(pt))
		return CTCHT_ONLASTBUTTON;

	for(int i=0; i<m_aItems.GetSize(); i++)
	{
		if(m_aItems[i]->HitTest(pt))
			return i;
	}
	return CTCHT_NOWHERE;
}

int CCustomTabCtrl::HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
	if(!(GetStyle()&CTCS_MULTIHIGHLIGHT))
		return CTCERR_NOMULTIHIGHLIGHTSTYLE;

	for(int i=0; i<m_aItems.GetSize();i++)
		m_aItems[i]->m_fHighlightChanged = FALSE;

	if(fCtrlPressed)
	{
		if(nItem!=m_nItemSelected)
		{
			m_aItems[nItem]->m_fHighlighted = !m_aItems[nItem]->m_fHighlighted;
			if(fMouseSel)
				m_aItems[nItem]->m_fHighlightChanged = TRUE;
		}
	}
	else if(!m_aItems[nItem]->m_fHighlighted)
	{
		m_aItems[nItem]->m_fHighlighted = TRUE;
		m_aItems[nItem]->m_fHighlightChanged = TRUE;
		for(int i=0;i<m_aItems.GetSize();i++)
		{
			if(i!=m_nItemSelected)
			{
				if(m_aItems[i]->m_fHighlighted)
				{
					m_aItems[i]->m_fHighlighted = FALSE;
					if(fMouseSel)
						m_aItems[i]->m_fHighlightChanged = TRUE;
				}
			}
		}
	}
	if(fMouseSel)
		RecalcLayout(RECALC_ITEM_SELECTED,nItem);
	Invalidate(FALSE);
	return CTCERR_NOERROR;
}

int CCustomTabCtrl::SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;

	if(m_nItemSelected!=-1)
		m_aItems[m_nItemSelected]->m_fSelected = FALSE;

	m_nItemSelected = nItem;
	
	if(m_nItemSelected!=-1)
		m_aItems[m_nItemSelected]->m_fSelected = TRUE;

	if(fMouseSel)
		RecalcLayout(RECALC_ITEM_SELECTED,m_nItemSelected);
	else
	{
		m_nItemNdxOffset = nItem;
		RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	}
	Invalidate(FALSE);
	HighlightItem(nItem, fMouseSel, fCtrlPressed);
	return CTCERR_NOERROR;
}

void CCustomTabCtrl::RecalcLayout(int nRecalcType, int nItem)
{
	CRect rCl;
	GetClientRect(&rCl);
	if(IsVertical())
		rCl.SetRect(0,0,rCl.Height(),rCl.Width());

	int nCloseOffset = 0;
	int nA = rCl.Height()-3;
	int nBnWidth = 0;
	int nBtns = 0;
	if(GetStyle()&CTCS_CLOSEBUTTON)
	{
		nBtns++;
		nCloseOffset = nA;
		nBnWidth = nA+3;
		if(m_nCloseState==BNST_INVISIBLE)
			m_nCloseState = BNST_NORMAL;
	}
	else
		m_nCloseState = BNST_INVISIBLE;

	int nWidth = RecalcRectangles();

	if((GetStyle()&CTCS_AUTOHIDEBUTTONS) && (m_aItems.GetSize()<2 || nWidth <= rCl.Width()-nBnWidth))
	{
		m_nFirstState = BNST_INVISIBLE;
		m_nPrevState = BNST_INVISIBLE;
		m_nNextState = BNST_INVISIBLE;
		m_nLastState = BNST_INVISIBLE;
		m_nItemNdxOffset = 0;
		RecalcOffset(nBnWidth);
		if(nRecalcType==RECALC_EDIT_RESIZED)
			RecalcEditResized(0,nItem);
		
		if(m_ctrlToolTip.m_hWnd)
		{
			m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,0,0,0));
			m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(0,0,0,0));
			m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(0,0,0,0));
			m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,0,0,0));
			if(GetStyle()&CTCS_CLOSEBUTTON)
			{
				if(IsVertical())
				{
					if(GetStyle()&CTCS_BUTTONSAFTER)
						m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,0,rCl.Height(),nA+1));
					else
						m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,rCl.Width()-nA-1,rCl.Height(),rCl.Width()));
				}
				else
				{
					if(GetStyle()&CTCS_BUTTONSAFTER)
						m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(rCl.Width()-nA-1,0,rCl.Width(),rCl.Height()));
					else
						m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,0,nA+1,rCl.Height()));
				}
			}

		}
		return;
	}
	
	if(m_nPrevState==BNST_INVISIBLE)
	{
		m_nPrevState = BNST_NORMAL;
		m_nNextState = BNST_NORMAL;
	}
	if(GetStyle()&CTCS_FOURBUTTONS)
	{
		nBtns += 4;
		if(m_nFirstState==BNST_INVISIBLE)
		{
			m_nFirstState = BNST_NORMAL;
			m_nLastState = BNST_NORMAL;
		}
	}
	else
	{
		nBtns += 2;
		m_nFirstState = BNST_INVISIBLE;
		m_nLastState = BNST_INVISIBLE;
	}
	
	if(m_ctrlToolTip.m_hWnd)
	{
		if(GetStyle()&CTCS_BUTTONSAFTER)
		{
			if(IsVertical())
			{
				if(nBtns<4)
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,0,0,0));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(0,nCloseOffset+nA+1,rCl.Height(),nCloseOffset+2*nA+3));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(0,nCloseOffset,rCl.Height(),nCloseOffset+nA+1));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,0,0,0));
				}
				else
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,nCloseOffset+3*nA+2,rCl.Height(),nCloseOffset+4*nA+3));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(0,nCloseOffset+2*nA+1,rCl.Height(),nCloseOffset+3*nA+2));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(0,nCloseOffset+nA+1,rCl.Height(),nCloseOffset+2*nA+1));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,nCloseOffset,rCl.Height(),nCloseOffset+nA+1));
				}
				if(GetStyle()&CTCS_CLOSEBUTTON)
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,0,rCl.Height(),nA+1));
			}
			else
			{
				if(nBtns<4)
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,0,0,0));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(rCl.Width()-nCloseOffset-2*nA-3,0,rCl.Width()-nCloseOffset-nA-1,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(rCl.Width()-nCloseOffset-nA-1,0,rCl.Width()-nCloseOffset,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,0,0,0));
				}
				else
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(rCl.Width()-nCloseOffset-4*nA-3,0,rCl.Width()-nCloseOffset-3*nA-2,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(rCl.Width()-nCloseOffset-3*nA-2,0,rCl.Width()-nCloseOffset-2*nA-1,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(rCl.Width()-nCloseOffset-2*nA-1,0,rCl.Width()-nCloseOffset-nA-1,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(rCl.Width()-nCloseOffset-nA-1,0,rCl.Width()-nCloseOffset,rCl.Height()));
				}
				if(GetStyle()&CTCS_CLOSEBUTTON)
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(rCl.Width()-nA-1,0,rCl.Width(),rCl.Height()));
			}
		}
		else
		{
			if(IsVertical())
			{
				if(nBtns<4)
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,0,0,0));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(0,rCl.Width()-nCloseOffset-nA-1,rCl.Height(),rCl.Width()-nCloseOffset));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(0,rCl.Width()-nCloseOffset-2*nA-3,rCl.Height(),rCl.Width()-nCloseOffset-nA-1));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,0,0,0));
				}
				else
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,rCl.Width()-nCloseOffset-nA-1,rCl.Height(),rCl.Width()-nCloseOffset));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(0,rCl.Width()-nCloseOffset-2*nA-3,rCl.Height(),rCl.Width()-nCloseOffset-nA-1));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(0,rCl.Width()-nCloseOffset-3*nA-2,rCl.Height(),rCl.Width()-nCloseOffset-2*nA-1));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,rCl.Width()-nCloseOffset-4*nA-3,rCl.Height(),rCl.Width()-nCloseOffset-3*nA-2));
				}
				if(GetStyle()&CTCS_CLOSEBUTTON)
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,rCl.Width()-nA-1,rCl.Height(),rCl.Width()));
			}
			else
			{
				if(nBtns<4)
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(0,0,0,0));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(nCloseOffset,0,nCloseOffset+nA+1,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(nCloseOffset+nA+1,0,nCloseOffset+2*nA+3,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(0,0,0,0));
				}
				else
				{
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_FIRSTBUTTON,CRect(nCloseOffset,0,nCloseOffset+nA+1,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_PREVBUTTON,CRect(nCloseOffset+nA+1,0,nCloseOffset+2*nA+3,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_NEXTBUTTON,CRect(nCloseOffset+2*nA+1,0,nCloseOffset+3*nA+2,rCl.Height()));
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_LASTBUTTON,CRect(nCloseOffset+3*nA+2,0,nCloseOffset+4*nA+3,rCl.Height()));
				}
				if(GetStyle()&CTCS_CLOSEBUTTON)
					m_ctrlToolTip.SetToolRect(this,(UINT)CTCID_CLOSEBUTTON,CRect(0,0,nA+1,rCl.Height()));
			}
		}
	}
	
	if(m_aItems.GetSize()==0)
		return;
	
	nBnWidth = nBtns*nA+3;	
	if(GetStyle()&CTCS_BUTTONSAFTER)
		rCl.right -= nBnWidth;
	switch(nRecalcType)
	{
	case RECALC_CLOSE_PRESSED:
		RecalcRectangles();
		RecalcOffset(nBnWidth);
		break;
	case RECALC_FIRST_PRESSED:
		{
			m_nItemNdxOffset=0;
			RecalcRectangles();
			RecalcOffset(nBnWidth);
		}
		break;
	case RECALC_PREV_PRESSED:
		{	
			RecalcOffset(nBnWidth);
			if(m_nItemNdxOffset>0)
			{
				m_nItemNdxOffset--;
				RecalcRectangles();
				RecalcOffset(nBnWidth);
			}
		}
		break;
	case RECALC_NEXT_PRESSED:
		{	
			RecalcOffset(nBnWidth);
			if(m_aItems[m_aItems.GetSize()-1]->m_rect.right>rCl.Width() && m_nItemNdxOffset!=m_aItems.GetSize()-1)
			{
				m_nItemNdxOffset++;
				RecalcRectangles();
				RecalcOffset(nBnWidth);
			}
		}
		break;
	case RECALC_ITEM_SELECTED:
		{
			RecalcOffset(nBnWidth);
			if(m_aItems[nItem]->m_bShape==TAB_SHAPE2 || m_aItems[nItem]->m_bShape==TAB_SHAPE3)
			{
				m_nItemNdxOffset--;
				RecalcRectangles();
				RecalcOffset(nBnWidth);
			}
			else
			{
				while(m_nItemNdxOffset<nItem && 
						m_aItems[nItem]->m_bShape==TAB_SHAPE4 && 
						m_aItems[nItem]->m_rect.right>rCl.Width() && 
						m_aItems[nItem]->m_rect.left>((GetStyle()&CTCS_BUTTONSAFTER)?0:nBnWidth))
				{
					m_nItemNdxOffset++;
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
			}
		}
		break;
	case RECALC_EDIT_RESIZED:
		{
			RecalcOffset(nBnWidth);
			RecalcEditResized(nBnWidth,nItem);
		}
		break;
	case RECALC_LAST_PRESSED:
		{
			m_nItemNdxOffset=m_aItems.GetSize()-1;
		}
	default:	// window resized
		{
			BOOL bNdxOffsetChanged = FALSE;
			RecalcOffset(nBnWidth);
			while(m_nItemNdxOffset>=0 && m_aItems[m_aItems.GetSize()-1]->m_rect.right<rCl.Width())
			{
				m_nItemNdxOffset--;
				if(m_nItemNdxOffset>=0)
				{
					RecalcRectangles();
					RecalcOffset(nBnWidth);
				}
				bNdxOffsetChanged = TRUE;
			}
			if(bNdxOffsetChanged)
			{
				m_nItemNdxOffset++;
				RecalcRectangles();
				RecalcOffset(nBnWidth);
			}
		}
		break;
	}
}

void CCustomTabCtrl::RecalcEditResized(int nOffset, int nItem)
{
	CRect rCl;
	GetClientRect(rCl);
	if(GetStyle()&CTCS_BUTTONSAFTER)
		rCl.right -= nOffset;
	do
	{
		CRect r;
		CDC* pDC = GetDC();
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);
		int h = pDC->DrawText(m_aItems[nItem]->m_sText+"X", r, DT_CALCRECT);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		r = m_aItems[nItem]->m_rectText;
		if(r.Height()>h)
		{
			r.top += (r.Height()-h)/2;
			r.bottom -= (r.Height()-h)/2;
		}
		r.left += 2;
		if(r.right>rCl.right && m_nItemSelected>m_nItemNdxOffset)
		{
			m_nItemNdxOffset++;
			RecalcRectangles();
			RecalcOffset(nOffset);
		}
		else
		{
			if(r.right>rCl.right)
				r.right = rCl.right;
			m_ctrlEdit.MoveWindow(r);
			int n = m_aItems[nItem]->m_sText.GetLength();
			int nStart, nEnd;
			m_ctrlEdit.GetSel(nStart,nEnd);
			if(nStart==nEnd && nStart==n)
			{
				m_ctrlEdit.SetSel(0,0);
				m_ctrlEdit.SetSel(n,n);
			}
			return;
		}
	} 
	while(1);
}

void CCustomTabCtrl::RecalcOffset(int nOffset)
{
	CRect rCl;
	GetClientRect(&rCl);
	if(IsVertical())
		rCl.SetRect(0,0,rCl.Height(),rCl.Width());
	
	int rightAdjusment = 0;
	if(GetStyle()&CTCS_BUTTONSAFTER)
	{
		rightAdjusment = nOffset;
		nOffset = 0;
	}

	for(int i = 0; i<m_aItems.GetSize(); i++)
	{
		if(i<m_nItemNdxOffset-1)
		{
			m_aItems[i]->m_bShape = TAB_SHAPE1;
			nOffset -= m_aItems[i]->m_rect.Width()-rCl.Height()/2;
			m_aItems[i]->m_rectText.SetRectEmpty();
		}
		else if(i==m_nItemNdxOffset-1)
		{
			int nBtns = 2;
			if(GetStyle()&CTCS_FOURBUTTONS)
				nBtns = 4;
			if(GetStyle()&CTCS_CLOSEBUTTON)
				nBtns++;
			int nBnWidth = nBtns*(rCl.Height()-3)+3;
			if(i==m_nItemSelected)
				m_aItems[i]->m_bShape = TAB_SHAPE2;
			else
				m_aItems[i]->m_bShape = TAB_SHAPE3;
			nOffset -= m_aItems[i]->m_rect.Width()-rCl.Height()/2;
			m_aItems[i]->m_rect.SetRect(0,1,rCl.Height()/2,rCl.Height()-1);
			if(!(GetStyle()&CTCS_BUTTONSAFTER))
				m_aItems[i]->m_rect.OffsetRect(nBnWidth,0);
			m_aItems[i]->m_rectText.SetRectEmpty();
		}
		else
		{
			if(i==m_nItemSelected)
				m_aItems[i]->m_bShape = TAB_SHAPE4;
			else if(i==m_aItems.GetSize()-1)	// last item
				m_aItems[i]->m_bShape = TAB_SHAPE4;
			else
				m_aItems[i]->m_bShape = TAB_SHAPE5;
			m_aItems[i]->m_rect.OffsetRect(nOffset,0);
			m_aItems[i]->m_rectText.OffsetRect(nOffset,0);
		}
		m_aItems[i]->ComputeRgn(GetStyle()&CTCS_TOP);
		if(m_ctrlToolTip.m_hWnd)
		{
			CRect rT = m_aItems[i]->m_rectText;
			if(rT.left>=rCl.Width()-rightAdjusment)
				rT.SetRect(0,0,0,0);
			else if(rT.right>rCl.Width()-rightAdjusment)
				rT.right = rCl.right-rightAdjusment;
			if(IsVertical())
				rT.SetRect(0,rCl.Width()-rT.right,rCl.Height(),rCl.Width()-rT.left);
			m_ctrlToolTip.SetToolRect(this,i+1,rT);
		}
	}
}

int CCustomTabCtrl::RecalcRectangles()
{
	CRect rCl;
	GetClientRect(&rCl);
	if(IsVertical())
		rCl.SetRect(0,0,rCl.Height(),rCl.Width());
	BOOL fTop = GetStyle()&CTCS_TOP;
	int nWidth = 0;
	
	{
		// calculate width

		int nOffset = 0;
		CRect rcText;
		CDC* pDC = GetDC();
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);
		if(GetStyle()&CTCS_FIXEDWIDTH)
		{
			int nMaxWidth=0;
			int i;
			for(i=0; i<m_aItems.GetSize(); i++)
			{
				int w=0;
				int h = pDC->DrawText(m_aItems[i]->m_sText, rcText, DT_CALCRECT);
				if(h>0)
					w = rcText.Width();
				if(w>nMaxWidth)
					nMaxWidth = w;
			}
			for(i=0; i<m_aItems.GetSize(); i++)
			{
				if(fTop)
				{
					m_aItems[i]->m_rect = CRect(0,0,nMaxWidth+rCl.Height()+4,rCl.Height()-2);
					m_aItems[i]->m_rectText = CRect(rCl.Height()/2,0,nMaxWidth+rCl.Height()/2+4,rCl.Height()-2);
				}
				else
				{
					m_aItems[i]->m_rect = CRect(0,1,nMaxWidth+rCl.Height()+4,rCl.Height()-1);
					m_aItems[i]->m_rectText = CRect(rCl.Height()/2,1,nMaxWidth+rCl.Height()/2+4,rCl.Height()-1);
				}
				m_aItems[i]->m_rect += CPoint(nOffset,0);
				m_aItems[i]->m_rectText += CPoint(nOffset,0);

				nOffset += m_aItems[i]->m_rect.Width()-rCl.Height()/2;
				nWidth = m_aItems[i]->m_rect.right;
			}
		}
		else
		{
			for(int i= 0; i<m_aItems.GetSize(); i++)
			{
				int w=0;
				int h = pDC->DrawText(m_aItems[i]->m_sText, rcText, DT_CALCRECT);
				if(h>0)
					w = rcText.Width();
				if(fTop)
				{
					m_aItems[i]->m_rect = CRect(0,0,w+rCl.Height()+4,rCl.Height()-2);
					m_aItems[i]->m_rectText = CRect(rCl.Height()/2,0,w+rCl.Height()/2+4,rCl.Height()-2);
				}
				else
				{
					m_aItems[i]->m_rect = CRect(0,1,w+rCl.Height()+4,rCl.Height()-1);
					m_aItems[i]->m_rectText = CRect(rCl.Height()/2,1,w+rCl.Height()/2+4,rCl.Height()-1);
				}
				
				m_aItems[i]->m_rect += CPoint(nOffset,0);
				m_aItems[i]->m_rectText += CPoint(nOffset,0);

				nOffset += m_aItems[i]->m_rect.Width()-rCl.Height()/2;
				nWidth = m_aItems[i]->m_rect.right;

			}
		}
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
	}

	return nWidth;
}

BOOL CCustomTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(GetStyle()&CTCS_TOOLTIPS && m_ctrlToolTip.m_hWnd && 
		(pMsg->message==WM_LBUTTONDOWN || pMsg->message==WM_LBUTTONUP || pMsg->message==WM_MOUSEMOVE))
			m_ctrlToolTip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CCustomTabCtrl::DrawBk(CDC& dc, CRect& r, HBITMAP hBmp, BOOL fIsImageHorLayout, MY_MARGINS& mrgn, int nImageNdx)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap* pBmp = CBitmap::FromHandle(hBmp);
	BITMAP bm;
	pBmp->GetBitmap(&bm);
	CBitmap* pOldBmp = dcMem.SelectObject(pBmp);
	if(fIsImageHorLayout)
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				mrgn.cxLeftWidth,
				mrgn.cyTopHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				0,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-mrgn.cxRightWidth,
				r.top,
				mrgn.cxRightWidth,
				mrgn.cyTopHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-mrgn.cxRightWidth,
				0,
				SRCCOPY);

		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-mrgn.cyBottomHeight,
				mrgn.cxLeftWidth,
				mrgn.cyBottomHeight,
				&dcMem,
				nImageNdx*bm.bmWidth/4,
				bm.bmHeight-mrgn.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-mrgn.cxRightWidth,
				r.bottom-mrgn.cyBottomHeight,
				mrgn.cxRightWidth,
				mrgn.cyBottomHeight,
				&dcMem,
				(nImageNdx+1)*bm.bmWidth/4-mrgn.cxRightWidth,
				bm.bmHeight-mrgn.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+mrgn.cxLeftWidth,
			r.top,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyTopHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+mrgn.cxLeftWidth,
			0,
			bm.bmWidth/4-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+mrgn.cxLeftWidth,
			r.bottom-mrgn.cyBottomHeight,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4+mrgn.cxLeftWidth,
			bm.bmHeight-mrgn.cyBottomHeight,
			bm.bmWidth/4-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+mrgn.cyTopHeight,
			mrgn.cxLeftWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4,
			mrgn.cyTopHeight,
			mrgn.cxLeftWidth,
			bm.bmHeight-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-mrgn.cxRightWidth,
			r.top+mrgn.cyTopHeight,
			mrgn.cxRightWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			(nImageNdx+1)*bm.bmWidth/4-mrgn.cxRightWidth,
			mrgn.cyTopHeight,
			mrgn.cxRightWidth,
			bm.bmHeight-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+mrgn.cxLeftWidth,
			r.top+mrgn.cyTopHeight,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			nImageNdx*bm.bmWidth/4 + mrgn.cxLeftWidth,
			mrgn.cyTopHeight,
			bm.bmWidth/4-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			bm.bmHeight-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);
	}
	else
	{
		// left-top
		dc.BitBlt(r.left,
				r.top,
				mrgn.cxLeftWidth,
				mrgn.cyTopHeight,
				&dcMem,
				0,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// right-top
		dc.BitBlt(r.right-mrgn.cxRightWidth,
				r.top,
				mrgn.cxRightWidth,
				mrgn.cyTopHeight,
				&dcMem,
				bm.bmWidth-mrgn.cxRightWidth,
				nImageNdx*bm.bmHeight/4,
				SRCCOPY);
		
		// left-bottom
		dc.BitBlt(r.left,
				r.bottom-mrgn.cyBottomHeight,
				mrgn.cxLeftWidth,
				mrgn.cyBottomHeight,
				&dcMem,
				0,
				(nImageNdx+1)*bm.bmHeight/4-mrgn.cyBottomHeight,
				SRCCOPY);

		// right-bottom
		dc.BitBlt(r.right-mrgn.cxRightWidth,
				r.bottom-mrgn.cyBottomHeight,
				mrgn.cxRightWidth,
				mrgn.cyBottomHeight,
				&dcMem,
				bm.bmWidth-mrgn.cxRightWidth,
				(nImageNdx+1)*bm.bmHeight/4-mrgn.cyBottomHeight,
				SRCCOPY);

		// middle-top
		dc.StretchBlt(r.left+mrgn.cxLeftWidth,
			r.top,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyTopHeight,
			&dcMem,
			mrgn.cxLeftWidth,
			nImageNdx*bm.bmHeight/4,
			bm.bmWidth-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyTopHeight,
			SRCCOPY);

		// middle-bottom
		dc.StretchBlt(r.left+mrgn.cxLeftWidth,
			r.bottom-mrgn.cyBottomHeight,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyBottomHeight,
			&dcMem,
			mrgn.cxLeftWidth,
			(nImageNdx+1)*bm.bmHeight/4-mrgn.cyBottomHeight,
			bm.bmWidth-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			mrgn.cyBottomHeight,
			SRCCOPY);

		// middle-left
		dc.StretchBlt(r.left,
			r.top+mrgn.cyTopHeight,
			mrgn.cxLeftWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			0,
			nImageNdx*bm.bmHeight/4+mrgn.cyTopHeight,
			mrgn.cxLeftWidth,
			bm.bmHeight/4-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);

		// middle-right
		dc.StretchBlt(r.right-mrgn.cxRightWidth,
			r.top+mrgn.cyTopHeight,
			mrgn.cxRightWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			bm.bmWidth-mrgn.cxRightWidth,
			nImageNdx*bm.bmHeight/4+mrgn.cyTopHeight,
			mrgn.cxRightWidth,
			bm.bmHeight/4-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);

		// middle
		dc.StretchBlt(
			r.left+mrgn.cxLeftWidth,
			r.top+mrgn.cyTopHeight,
			r.Width()-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			r.Height()-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			&dcMem,
			mrgn.cxLeftWidth,
			nImageNdx*bm.bmHeight/4+mrgn.cyTopHeight,
			bm.bmWidth-mrgn.cxLeftWidth-mrgn.cxRightWidth,
			bm.bmHeight/4-mrgn.cyTopHeight-mrgn.cyBottomHeight,
			SRCCOPY);
	}
	dcMem.SelectObject(pOldBmp);
}

void CCustomTabCtrl::DrawGlyph(CDC& dc, CPoint& pt, int nImageNdx, int nColorNdx)
{
	CDC dcMem, dcMemMono;
	dcMem.CreateCompatibleDC(&dc);
	dcMemMono.CreateCompatibleDC(&dc);

	CBitmap* pOldBmpGlyphMono = dcMemMono.SelectObject(&m_bmpGlyphsMono);

	CBitmap bmpGlyphColor;
	bmpGlyphColor.CreateCompatibleBitmap(&dc,8,7);
	
	CBitmap* pOldBmpGlyphColor = dcMem.SelectObject(&bmpGlyphColor);

	COLORREF rgbOldTextGlyph =  dcMem.SetTextColor(m_rgbGlyph[nColorNdx]);
	dcMem.BitBlt(0, 0, 8, 7, &dcMemMono, nImageNdx*8, 0, SRCCOPY);
	dcMem.SetTextColor(rgbOldTextGlyph);

	COLORREF rgbOldBk = dc.SetBkColor(RGB(255,255,255));
	COLORREF rgbOldText = dc.SetTextColor(RGB(0,0,0));
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMem, 0, 0, SRCINVERT);
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMemMono, nImageNdx*8, 0, SRCAND);
	dc.BitBlt(pt.x, pt.y, 8, 7, &dcMem, 0, 0, SRCINVERT);

	dcMem.SelectObject(pOldBmpGlyphColor);
	dcMemMono.SelectObject(pOldBmpGlyphMono);
	dc.SetBkColor(rgbOldBk);
	dc.SetTextColor(rgbOldText);
}

BOOL CCustomTabCtrl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	if(dwRemove&CTCS_TOOLTIPS)
		m_ctrlToolTip.DestroyWindow();
	if(dwRemove&CTCS_MULTIHIGHLIGHT)
	{
		for(int i=0;i<m_aItems.GetSize();i++)
			m_aItems[i]->m_fHighlighted = FALSE;
	}
	if(dwAdd&CTCS_MULTIHIGHLIGHT)
	{
		for(int i=0;i<m_aItems.GetSize();i++)
		{
			if(i==m_nItemSelected)
				m_aItems[i]->m_fHighlighted = TRUE;
		}
	}
	CWnd::ModifyStyle(dwRemove,dwAdd,nFlags);
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return TRUE;
}

BOOL CCustomTabCtrl::ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	CWnd::ModifyStyleEx(dwRemove,dwAdd,nFlags);
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	Invalidate(FALSE);
	return TRUE;
}

void CCustomTabCtrl::PreSubclassWindow() 
{
	OnThemeChanged(0,0);
	CWnd::ModifyStyle(0,WS_CLIPCHILDREN);
	RecalcLayout(RECALC_RESIZED,m_nItemSelected);
	CWnd::PreSubclassWindow();
}

void CCustomTabCtrl::SetDragCursors(HCURSOR hCursorMove, HCURSOR hCursorCopy)
{
	::DestroyCursor(m_hCursorMove);
	m_hCursorMove = NULL;
	::DestroyCursor(m_hCursorCopy);
	m_hCursorCopy = NULL;
	m_hCursorMove = CopyCursor(hCursorMove);
	m_hCursorCopy = CopyCursor(hCursorCopy);
}

void CCustomTabCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	NotifyParent(CTCN_RCLICK,HitTest(point),point);
	CWnd::OnRButtonDown(nFlags, point);
}

void CCustomTabCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	NotifyParent(CTCN_RDBLCLK,HitTest(point),point);
	CWnd::OnRButtonDblClk(nFlags, point);
}

int CCustomTabCtrl::EditLabel(int nItem)
{
	return EditLabel(nItem, FALSE);
}

int CCustomTabCtrl::EditLabel(int nItem, BOOL fMouseSel)
{
	if(nItem<0 || nItem>=m_aItems.GetSize())
		return CTCERR_INDEXOUTOFRANGE;
	if(!(GetStyle()&CTCS_EDITLABELS))
		return CTCERR_NOEDITLABELSTYLE;
	if(nItem!=m_nItemSelected)
		return CTCERR_ITEMNOTSELECTED;
	if(m_ctrlEdit.m_hWnd)
		return CTCERR_ALREADYINEDITMODE;
	if(IsVertical())
		return CTCERR_EDITNOTSUPPORTED;
	try
	{
		CRect r;
		CDC* pDC = GetDC();
		CFont* pOldFont = pDC->SelectObject(&m_FontSelected);
		int h = pDC->DrawText(m_aItems[nItem]->m_sText, r, DT_CALCRECT);
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		r = m_aItems[nItem]->m_rectText;
		if(r.Height()>h)
		{
			r.top += (r.Height()-h)/2;
			r.bottom -= (r.Height()-h)/2;
		}
		r.left += 2;
		if(m_ctrlEdit.Create(WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,r,this,CTCID_EDITCTRL))
		{
			CString sOld = m_aItems[nItem]->m_sText;
			m_ctrlEdit.SetFont(&m_FontSelected,FALSE);
			m_ctrlEdit.SetLimitText(MAX_LABEL_TEXT);
			m_ctrlEdit.SetWindowText(m_aItems[nItem]->m_sText);
			m_ctrlEdit.SetFocus();
			m_ctrlEdit.SetSel(0,-1);
			if(fMouseSel)
				ReleaseCapture();
			for (;;) 
			{
				MSG msg;
				::GetMessage(&msg, NULL, 0, 0);

				switch (msg.message) 
				{
				case WM_SYSKEYDOWN:
					{
						if(msg.wParam == VK_F4 && msg.lParam&29)
							break;
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					break;
				case WM_KEYDOWN:
					{
						if (msg.wParam == VK_ESCAPE)
						{
							m_aItems[nItem]->m_sText = sOld;
							m_ctrlEdit.DestroyWindow();
							RecalcLayout(RECALC_RESIZED,m_nItemSelected);
							Invalidate(FALSE);
							return CTCERR_NOERROR;
						}
						if(msg.wParam == VK_RETURN)
						{
							if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
								break;
							m_ctrlEdit.GetWindowText(m_aItems[nItem]->m_sText);
							m_ctrlEdit.DestroyWindow();
							RecalcLayout(RECALC_RESIZED,nItem);
							Invalidate(FALSE);
							return CTCERR_NOERROR;
						}
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					break;
				case WM_LBUTTONDOWN:
					{
						if(msg.hwnd==m_hWnd)
						{
							POINTS pt = MAKEPOINTS(msg.lParam);
							if(HitTest(CPoint(pt.x,pt.y))!=m_nItemSelected)
							{
								if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
									break;
								m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
								m_ctrlEdit.DestroyWindow();
								TranslateMessage(&msg);
								DispatchMessage(&msg);
								return CTCERR_NOERROR;
							}
						}
						else if(msg.hwnd==m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
						else
						{
							if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
								break;
							m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
							m_ctrlEdit.DestroyWindow();
							return CTCERR_NOERROR;
						}
					}
					break;
				case WM_LBUTTONUP:
					{
						if(msg.hwnd==m_ctrlEdit.m_hWnd)
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
					break;
				case WM_NCLBUTTONDOWN:
					{
						if(NotifyParent(CTCN_LABELUPDATE,nItem,CPoint(0,0)))
							break;
						m_ctrlEdit.GetWindowText(m_aItems[m_nItemSelected]->m_sText);
						m_ctrlEdit.DestroyWindow();
						TranslateMessage(&msg);
						DispatchMessage(&msg);
						return CTCERR_NOERROR;
					}
					break;
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
					break;
				default:
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					break;
				}
			}
		}
	}
	catch(CMemoryException* e)
	{
		e->Delete();
		if(fMouseSel)
			NotifyParent(CTCN_OUTOFMEMORY,nItem,CPoint(0,0));
		return CTCERR_OUTOFMEMORY;
	}
	return CTCERR_NOERROR;
}
