// GradientCtrl.cpp : implementation file
//
#include "stdafx.h"
#include "GradientCtrl.h"
#include "GradientCtrlImpl.h"

#define GRADIENTCTRL_CLASSNAME _T("MFCGradientCtrl")


/////////////////////////////////////////////////////////////////////////////
// CGradientCtrl

CGradientCtrl::CGradientCtrl()
{
	RegisterWindowClass();
	m_Width = GCW_AUTO;
	m_Selected = -1;
	m_ToolTipFormat = _T("&SELPOS\nPosition: &SELPOS Colour: R &R G &G B &B\nColour: R &R G &G B &B\nColour: R &R G &G B &B\nDouble Click to Add a New Peg");
	m_bShowToolTip = true;
	m_Orientation = Auto;
	m_Impl = new CGradientCtrlImpl(this);
	m_crBackground = GetSysColor(COLOR_WINDOW);
}

CGradientCtrl::~CGradientCtrl()
{
	delete m_Impl;
}

BEGIN_MESSAGE_MAP(CGradientCtrl, CWnd)
	//{{AFX_MSG_MAP(CGradientCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGradientCtrl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();
	HBRUSH background;
	background = ::CreateSolidBrush(0x00FFFFFF);

    if (!(::GetClassInfo(hInst, GRADIENTCTRL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = nullptr;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = background;
        wndcls.lpszMenuName     = nullptr;
        wndcls.lpszClassName    = GRADIENTCTRL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGradientCtrl message handlers

BOOL CGradientCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::CreateEx(WS_EX_CLIENTEDGE, GRADIENTCTRL_CLASSNAME, _T(""),
		0x50010000, rect, pParentWnd, nID);
}

void CGradientCtrl::OnPaint()
{
	CPaintDC dc(this);

	m_Impl->Draw(&dc);

	//----- Refresh -----//
	if(m_Selected > m_Gradient.GetPegCount())
		m_Selected = NONE;

	if(m_bShowToolTip)
		m_Impl->SynchronizeTooltips();
}

BOOL CGradientCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRgn pegrgn, clientrgn, gradientrgn, erasergn;
	CRect clientrect;
	BOOL leftdown = m_Impl->m_LeftDownSide;
	BOOL rightup = m_Impl->m_RightUpSide;

	//----- The area of the window -----//
	GetClientRect(&clientrect);
	clientrgn.CreateRectRgn(0, 0, clientrect.right, clientrect.bottom);

	//----- The area of the gradient -----//
	if(m_Impl->IsVertical())
		gradientrgn.CreateRectRgn(leftdown ? 23 : 4, 4, m_Impl->GetDrawWidth() - (rightup ? 23 : 3), clientrect.bottom-4);
	else
		gradientrgn.CreateRectRgn(4, clientrect.bottom-m_Impl->GetDrawWidth() + (rightup ? 23 : 4),
			clientrect.right-4, clientrect.bottom- (leftdown ? 23 : 4));

	//----- The area of the pegs -----//
	GetPegRgn(&pegrgn);

	//----- Create a region outside all of these -----//
	erasergn.CreateRectRgn(0, 0, 0, 0); // Create a dummy rgn
	erasergn.CombineRgn(&clientrgn, &pegrgn, RGN_DIFF);
	erasergn.CombineRgn(&erasergn, &gradientrgn, RGN_DIFF);

	//----- Fill the result in -----//
	CBrush background(m_crBackground);
	pDC->FillRgn(&erasergn, &background);

	return 1;
}

void CGradientCtrl::PreSubclassWindow()
{
	CWnd::PreSubclassWindow();

}

void CGradientCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	CRect clientrect, pegrect;
	bool nowselected = false;

	m_LastPos = -1;
	GetClientRect(&clientrect);

	//----- Just in case the user starts dragging -----//
	SetFocus();
	SetCapture();
	m_MouseDown = point;

	//----- Check if the user is selecting a marker peg -----//
	for(int i = 0; i < m_Gradient.GetPegCount(); i++)
	{
		if(m_Impl->m_LeftDownSide)
		{
			m_Impl->GetPegRect(i, &pegrect, false);
			if(pegrect.PtInRect(point))
			{
				SetSelIndex(i);
				nowselected = true;
				break;
			}
		}

		if(m_Impl->m_RightUpSide)
		{
			m_Impl->GetPegRect(i, &pegrect, true);
			if(pegrect.PtInRect(point))
			{
				SetSelIndex(i);
				nowselected = true;
				break;
			}
		}
	}

	//----- Check if the user is trying to select the first or last one -----//
	GetClientRect(&clientrect);

	if(m_Impl->m_RightUpSide)
	{
		m_Impl->GetPegRect(STARTPEG, &pegrect, true);
		if(pegrect.PtInRect(point))
		{
			SetSelIndex(STARTPEG);
			nowselected = true;
		}

		m_Impl->GetPegRect(ENDPEG, &pegrect, true);
		if(pegrect.PtInRect(point))
		{
			SetSelIndex(ENDPEG);
			nowselected = true;
		}
	}

	if(m_Impl->m_LeftDownSide)
	{
		m_Impl->GetPegRect(STARTPEG, &pegrect, false);
		if(pegrect.PtInRect(point))
		{
			SetSelIndex(STARTPEG);
			nowselected = true;
		}

		m_Impl->GetPegRect(ENDPEG, &pegrect, false);
		if(pegrect.PtInRect(point))
		{
			SetSelIndex(ENDPEG);
			nowselected = true;
		}
	}

	if(!nowselected)
	{
		m_Impl->DrawSelPeg(&dc, m_Selected);
		m_Selected = NONE;
		m_LastPos = -1;
	}

	CWnd *pParent = GetParent();

	if (pParent)
	{
		PegNMHDR nmhdr;

		nmhdr.nmhdr.code = GC_SELCHANGE;
		nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
		nmhdr.nmhdr.idFrom = GetDlgCtrlID();
		if(m_Selected != -1) nmhdr.peg = m_Gradient.GetPeg(m_Selected);
		else nmhdr.peg = m_Impl->m_Null;
		nmhdr.index = m_Selected;
		pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));
	}

	SetFocus();

	CWnd::OnLButtonDown(nFlags, point);
}

void CGradientCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(nFlags & MK_LBUTTON && m_Selected > -1)
	{
		CRgn oldrgn, newrgn, erasergn;
		CBrush brush;
		POINT tippoint;
		CString tiptext;
		bool vertical = m_Impl->IsVertical();
		int selpegpos;

		//----- Prepare -----//
		CClientDC dc(this);
		float pos;

		//----- Checks to see if the mouse is far enough away to "drop out" -----//
		if(vertical)
		{
			if(max(point.x - m_MouseDown.x, m_MouseDown.x - point.x) <= 200)
				selpegpos = point.y;
			else
				selpegpos = m_MouseDown.y;
		}
		else
		{
			if(max(point.y - m_MouseDown.y, m_MouseDown.y - point.y) <= 200)
				selpegpos = point.x;
			else
				selpegpos = m_MouseDown.x;
		}


		//----- A brush for thebackground -----//
		brush.CreateSolidBrush(m_crBackground);

		//----- Get the orginal erase area -----//
		GetPegRgn(&oldrgn);

		//----- Continue -----//
		CPeg peg = m_Gradient.GetPeg(m_Selected);
		pos = m_Impl->PosFromPoint(selpegpos);
		//"The id of the selection may change"//
		m_Selected = m_Gradient.SetPeg(m_Selected, peg.colour, pos);

		//----- Draw the peg -----//
		m_Impl->DrawPegs(&dc);
		m_Impl->DrawEndPegs(&dc);
		m_Impl->DrawSelPeg(&dc, m_Selected);
		m_Impl->DrawGradient(&dc);
		GetPegRgn(&newrgn);

		erasergn.CreateRectRgn(0,0,0,0); //Dummy rgn
		erasergn.CombineRgn(&oldrgn, &newrgn, RGN_DIFF);

		dc.FillRgn(&erasergn, &brush);

		m_LastPos = selpegpos;

		//----- Free up stuff -----//
		oldrgn.DeleteObject();
		newrgn.DeleteObject();
		erasergn.DeleteObject();

		//----- Show tooltip -----//
		if(m_bShowToolTip)
		{
			tiptext = m_Impl->ExtractLine(m_ToolTipFormat, 0);
			m_Impl->ParseToolTipLine(tiptext, peg);

			if(m_Impl->m_wndToolTip == nullptr)
			{
				if(m_Impl->IsVertical())
				{
					tippoint.y = point.y - 8;
					tippoint.x = m_Impl->GetDrawWidth() + m_Impl->GetPegIndent(GetSelIndex())*11 - 7;
				}
				else
				{
					CRect clientrect;
					GetClientRect(&clientrect);
					tippoint.y = clientrect.bottom - m_Impl->GetDrawWidth() - m_Impl->GetPegIndent(GetSelIndex())*11 - 7;
					tippoint.x = point.x - 8;
				}

				ClientToScreen(&tippoint);

				m_Impl->ShowTooltip(tippoint, tiptext);
			}
			else
				m_Impl->SetTooltipText(tiptext);
		}

		//----- Send parent messages -----//
		CWnd *pParent = GetParent();
		if (pParent)
		{
			PegNMHDR nmhdr;
			nmhdr.nmhdr.code = GC_PEGMOVE;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			if(m_Selected != -1) nmhdr.peg = peg;
			else nmhdr.peg = m_Impl->m_Null;
			nmhdr.index = m_Selected;
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));

			nmhdr.nmhdr.code = GC_CHANGE;
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&(nmhdr.nmhdr)));
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CGradientCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_Selected >= -4 && m_Selected != -1)
	{
		bool vertical = m_Impl->IsVertical();
		CRect clientrect;
		GetClientRect(&clientrect);
		//int selpegpos = vertical ? point.y-5 : point.x-5;

		//----- Update the pegs -----//
		CClientDC dc(this);
		if(vertical)
			dc.FillSolidRect(CRect(m_Impl->GetDrawWidth(), 0, clientrect.right,
				clientrect.bottom), 0x00FFFFFF); //Erase the old ones
		else
			dc.FillSolidRect(CRect(0, 0, clientrect.right,
				clientrect.bottom - m_Impl->GetDrawWidth()), 0x00FFFFFF); //Erase the old ones

		m_Impl->DrawPegs(&dc);
		m_Impl->DrawEndPegs(&dc); //Draw the new ones
		m_Impl->DrawSelPeg(&dc, m_Selected);

		m_Impl->DestroyTooltip();

		CWnd *pParent = GetParent();
		if (pParent)
		{
			PegNMHDR nmhdr;
			nmhdr.nmhdr.code = GC_PEGMOVED;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			if(m_Selected != -1) nmhdr.peg = m_Gradient.GetPeg(m_Selected);
			else nmhdr.peg = m_Impl->m_Null;
			nmhdr.index = m_Selected;
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));

			nmhdr.nmhdr.code = GC_CHANGE;
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&(nmhdr.nmhdr)));
		}
	}

	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CGradientCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_TAB:
		if(m_Selected > -4 && m_Selected < m_Gradient.GetPegCount())
		{
			CClientDC dc(this);
			m_Impl->DrawSelPeg(&dc, m_Selected);
			switch(m_Selected)
			{
			case NONE:
				m_Selected = STARTPEG;
				break;
			case STARTPEG:
				if(m_Gradient.GetPegCount() > 0)
					m_Selected = 0;
				else m_Selected = ENDPEG;
				break;
			case ENDPEG:
				m_Selected = STARTPEG;
				break;
			default:
				if(m_Selected+1 >= m_Gradient.GetPegCount())
					m_Selected = ENDPEG;
				else m_Selected++;
			}

			m_Impl->DrawSelPeg(&dc, m_Selected);
		}
		break;

	case VK_UP:
	case VK_LEFT:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position -= 0.005f;
			//Make sure that the position does not stray bellow zero
			selpeg.position = (selpeg.position <= 1.0f) ?  selpeg.position : 1.0f;
			MoveSelected(selpeg.position, true);

			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}
		break;

	case VK_DOWN:
	case VK_RIGHT:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position += 0.005f;
			//Make sure that the position does not stray above 1
			selpeg.position = (selpeg.position <= 1.0f) ?  selpeg.position : 1.0f;
			MoveSelected(selpeg.position, true);

			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}

		break;

	case VK_HOME:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position = 0.0f;
			MoveSelected(selpeg.position, true);
			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}

		break;

	case VK_END:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position = 1.0f;
			MoveSelected(selpeg.position, true);
			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}

		break;

	case VK_DELETE:
	case VK_BACK:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
			DeleteSelected(TRUE);

		break;
	case VK_PRIOR: // Shift the peg up a big jump
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position -= 0.01f;
			//Make sure that the position does not stray bellow zero
			selpeg.position = (selpeg.position >= 0.0f) ?  selpeg.position : 0.0f;
			MoveSelected(selpeg.position, true);

			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}

		break;

	case VK_NEXT:
		if(m_Selected >= 0 && m_Selected < m_Gradient.GetPegCount())
		{
			CPeg selpeg = GetSelPeg();
			selpeg.position += 0.01f;
			//Make sure that the position does not stray above 1
			selpeg.position = (selpeg.position <= 1.0f) ?  selpeg.position : 1.0f;
			MoveSelected(selpeg.position, true);

			//Send parent messages
			SendBasicNotification(GC_PEGMOVED, selpeg, m_Selected);
		}
		break;

	case VK_RETURN:
	case VK_SPACE:
		if(m_Selected > -4 && m_Selected < m_Gradient.GetPegCount()
			&& m_Selected != -1)
		{
			PegNMHDR nmhdr;

			nmhdr.nmhdr.code = GC_EDITPEG;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			nmhdr.peg = GetSelPeg();
			nmhdr.index = m_Selected;
			GetParent()->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));
		}

		break;

	case VK_INSERT:
		if(m_Selected > -4 && m_Selected < m_Gradient.GetPegCount()
			&& m_Selected != -1)
		{
			PegCreateNMHDR nmhdr;
			CPeg peg = GetSelPeg();

			nmhdr.nmhdr.code = GC_CREATEPEG;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			nmhdr.position = peg.position;
			nmhdr.colour = peg.colour;
			GetParent()->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));
		}
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGradientCtrl::DeleteSelected(BOOL bUpdate)
{
	CPeg peg;
	int oldsel;
	float oldpos;

	if(m_Selected == NONE)
		return;
	if(m_Selected <= -4)
		return;
	if(m_Selected >= m_Gradient.GetPegCount())
		return;

	oldsel = m_Selected;
	peg = m_Gradient.GetPeg(m_Selected);
	oldpos = peg.position;

	m_Gradient.RemovePeg(m_Selected);

	//Select the previous peg
	if(m_Selected == STARTPEG) m_Selected = STARTPEG;
	else m_Selected--;
	if(m_Selected < 0) m_Selected = STARTPEG;

	if(bUpdate)
		Invalidate();

	CWnd *pParent = GetParent();

	if (pParent)
	{
		PegNMHDR nmhdr, nmhdr2, nmhdr3;

		nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
		nmhdr.nmhdr.idFrom = GetDlgCtrlID();
		nmhdr.nmhdr.code = GC_PEGREMOVED;
		if(m_Selected != -1) nmhdr.peg = peg;
		else nmhdr.peg = m_Impl->m_Null;
		nmhdr.index = oldsel;
		pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));

		nmhdr2 = nmhdr;
		nmhdr2.nmhdr.code = GC_SELCHANGE;
		if(m_Selected != -1) nmhdr2.peg = m_Gradient.GetPeg(m_Selected);
		else nmhdr2.peg = m_Impl->m_Null;
		nmhdr2.index = m_Selected;
		pParent->SendMessage(WM_NOTIFY, nmhdr2.nmhdr.idFrom, (DWORD_PTR)(&nmhdr2));

		nmhdr3 = nmhdr2;
		nmhdr3.nmhdr.code = GC_CHANGE;
		nmhdr3.peg = m_Impl->m_Null;
		nmhdr3.index = -1;
		pParent->SendMessage(WM_NOTIFY, nmhdr3.nmhdr.idFrom, (DWORD_PTR)(&nmhdr3));
	}
}

int CGradientCtrl::SetSelIndex(int iSel)
{
	int oldsel = m_Selected;
	ASSERT(iSel > -4); //Nothing smaller than -4 ok?
	ASSERT(iSel != BACKGROUND); //You can't select the background
	ASSERT(iSel < m_Gradient.GetPegCount()); //Make sure things are in range

	m_Selected = iSel;

	return oldsel;
}

int CGradientCtrl::MoveSelected(float newpos, BOOL bUpdate)
{
	CPeg peg;

	if(m_Selected < 0)
		return -1;

	peg = GetSelPeg();
	m_Selected = m_Gradient.SetPeg(m_Selected, peg.colour, newpos);

	if(bUpdate) Invalidate();

	return m_Selected;
}

COLORREF CGradientCtrl::SetColourSelected(COLORREF crNewColour, BOOL bUpdate)
{
	CPeg peg;

	if(m_Selected < 0)
		return 0x00000000;

	peg = GetSelPeg();
	m_Gradient.SetPeg(m_Selected, crNewColour, peg.position);

	if(bUpdate) Invalidate();

	return peg.colour;
}

void CGradientCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd *pParent = GetParent();
	float pos;
	CRect clientrect, pegrect;
	CPeg peg;
	CClientDC dc(this);
	BOOL edit = FALSE;
	int drawwidth = m_Impl->GetDrawWidth();

	if(m_Impl->IsVertical())
		pos = m_Impl->PosFromPoint(point.y);
	else
		pos = m_Impl->PosFromPoint(point.x);

	//----- Check if the user is selecting a marker peg -----//
	for(int i = 0; i < m_Gradient.GetPegCount(); i++)
	{
		peg = m_Gradient.GetPeg(i);

		if(m_Impl->m_LeftDownSide)
		{
			m_Impl->GetPegRect(i, &pegrect, false);
			if(pegrect.PtInRect(point))
			{
				SetSelIndex(i);
				edit = true;
				break;
			}
		}

		if(m_Impl->m_RightUpSide)
		{
			m_Impl->GetPegRect(i, &pegrect, true);
			if(pegrect.PtInRect(point))
			{
				SetSelIndex(i);
				edit = true;
				break;
			}
		}
	}

	//----- Check if the user is trying to select the first or last one -----//
	GetClientRect(&clientrect);

	pegrect.left = drawwidth+8, pegrect.top = 4;
	pegrect.right = drawwidth+15, pegrect.bottom = 11;
	if(pegrect.PtInRect(point))
	{
		m_Impl->DrawSelPeg(&dc, m_Selected); //Erase the last m_Selected peg
		m_Selected = STARTPEG;
		m_Impl->DrawSelPeg(&dc, m_Selected); //Draw the new one
		edit = true;
	}

	pegrect.left = drawwidth+8, pegrect.top = clientrect.bottom-11;
	pegrect.right = drawwidth+15, pegrect.bottom = clientrect.bottom-4;
	if(pegrect.PtInRect(point))
	{
		m_Impl->DrawSelPeg(&dc, m_Selected); //Erase the last m_Selected peg
		m_Selected = ENDPEG;
		m_Impl->DrawSelPeg(&dc, m_Selected);
		edit = true;
	}

	if (pParent)
	{
		if(edit)
		{
			PegNMHDR nmhdr;

			nmhdr.nmhdr.code = GC_EDITPEG;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			if(m_Selected != -1) nmhdr.peg = peg;
			else nmhdr.peg = m_Impl->m_Null;
			nmhdr.index = m_Selected;
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));
		}
		else
		{
			PegCreateNMHDR nmhdr;

			nmhdr.nmhdr.code = GC_CREATEPEG;
			nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
			nmhdr.nmhdr.idFrom = GetDlgCtrlID();
			nmhdr.position = pos;
			nmhdr.colour = m_Gradient.ColourFromPosition(pos);
			pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CGradientCtrl::GetPegRgn(CRgn *rgn)
{
	CRect clientrect;
	POINT pegpoint;
	int j;
	int drawwidth = m_Impl->GetDrawWidth();
	bool vertical = m_Impl->IsVertical();
	int colcount = (int)m_Impl->m_LeftDownSide+(int)m_Impl->m_RightUpSide;

	if(colcount == 0)
	{
		rgn->CreateRectRgn(0,0,0,0); // A dummy
		return;
	}

	//----- Carefully do some preparation -----//
	int pegcount = m_Gradient.GetPegCount();
	POINT *points = new POINT[pegcount*3*colcount + 8*colcount];
	ASSERT(points);
	int *polycounts = new int[pegcount*colcount + 2*colcount];
	ASSERT(polycounts);

	GetClientRect(&clientrect);

	//----- End pegs -----//
	j = 0;
	if(vertical)
	{
		if(m_Impl->m_RightUpSide)
		{
			polycounts[j/4]=4;
			points[j].x = drawwidth-15,	points[j].y = 4;
			j++;
			points[j].x = drawwidth-15,	points[j].y = 11;
			j++;
			points[j].x = drawwidth-8,	points[j].y = 11;
			j++;
			points[j].x = drawwidth-8,	points[j].y = 4;
			j++;

			polycounts[j/4]=4;
			points[j].x = drawwidth-15,	points[j].y = clientrect.bottom-4;
			j++;
			points[j].x = drawwidth-15,	points[j].y = clientrect.bottom-11;
			j++;
			points[j].x = drawwidth-8,	points[j].y = clientrect.bottom-11;
			j++;
			points[j].x = drawwidth-8,	points[j].y = clientrect.bottom-4;
			j++;
		}

		if(m_Impl->m_LeftDownSide)
		{
			polycounts[j/4]=4;
			points[j].x = 8,	points[j].y = 4;
			j++;
			points[j].x = 8,	points[j].y = 11;
			j++;
			points[j].x = 15,	points[j].y = 11;
			j++;
			points[j].x = 15,	points[j].y = 4;
			j++;

			polycounts[j/4]=4;
			points[j].x = 8,	points[j].y = clientrect.bottom-4;
			j++;
			points[j].x = 8,	points[j].y = clientrect.bottom-11;
			j++;
			points[j].x = 15,	points[j].y = clientrect.bottom-11;
			j++;
			points[j].x = 15,	points[j].y = clientrect.bottom-4;
			j++;
		}
	}
	else
	{
		if(m_Impl->m_RightUpSide)
		{
			polycounts[j/4]=4;
			points[j].x = 4,	points[j].y = clientrect.bottom-drawwidth+8;
			j++;
			points[j].x = 11,	points[j].y = clientrect.bottom-drawwidth+8;
			j++;
			points[j].x = 11,	points[j].y = clientrect.bottom-drawwidth+15;
			j++;
			points[j].x = 4,	points[j].y = clientrect.bottom-drawwidth+15;
			j++;

			polycounts[j/4]=4;
			points[j].x = clientrect.right-4,	points[j].y = clientrect.bottom-drawwidth+8;
			j++;
			points[j].x = clientrect.right-11,	points[j].y = clientrect.bottom-drawwidth+8;
			j++;
			points[j].x = clientrect.right-11,	points[j].y = clientrect.bottom-drawwidth+15;
			j++;
			points[j].x = clientrect.right-4,	points[j].y = clientrect.bottom-drawwidth+15;
			j++;
		}

		if(m_Impl->m_LeftDownSide)
		{
			polycounts[j/4]=4;
			points[j].x = 4,	points[j].y = clientrect.bottom-8;
			j++;
			points[j].x = 11,	points[j].y = clientrect.bottom-8;
			j++;
			points[j].x = 11,	points[j].y = clientrect.bottom-15;
			j++;
			points[j].x = 4,	points[j].y = clientrect.bottom-15;
			j++;

			polycounts[j/4]=4;
			points[j].x = clientrect.right-4,	points[j].y = clientrect.bottom-8;
			j++;
			points[j].x = clientrect.right-11,	points[j].y = clientrect.bottom-8;
			j++;
			points[j].x = clientrect.right-11,	points[j].y = clientrect.bottom-15;
			j++;
			points[j].x = clientrect.right-4,	points[j].y = clientrect.bottom-15;
			j++;
		}
	}

	j=0;

	//----- Main pegs -----//
	for(int i = 0; i < pegcount; i++)
	{
		if(vertical)
		{
			pegpoint.y = m_Impl->PointFromPos(m_Gradient.GetPeg(i).position);

			if(m_Impl->m_LeftDownSide)
			{
				pegpoint.x = 23 - m_Impl->GetPegIndent(i)*11;

				points[j*3+8*colcount].x = pegpoint.x;
				points[j*3+8*colcount].y = pegpoint.y+1;
				points[j*3+8*colcount+1].x = pegpoint.x-10;
				points[j*3+8*colcount+1].y = pegpoint.y-4;
				points[j*3+8*colcount+2].x = pegpoint.x-10;
				points[j*3+8*colcount+2].y = pegpoint.y+6;
				polycounts[j + 2*colcount] = 3;
				j++;
			}

			if(m_Impl->m_RightUpSide)
			{
				pegpoint.x = m_Impl->GetPegIndent(i)*11 + drawwidth - 23;

				points[j*3+8*colcount].x = pegpoint.x;
				points[j*3+8*colcount].y = pegpoint.y+1;
				points[j*3+8*colcount+1].x = pegpoint.x+10;
				points[j*3+8*colcount+1].y = pegpoint.y-4;
				points[j*3+8*colcount+2].x = pegpoint.x+10;
				points[j*3+8*colcount+2].y = pegpoint.y+6;
				polycounts[j + 2*colcount] = 3;
				j++;
			}
		}
		else
		{
			pegpoint.x = m_Impl->PointFromPos(m_Gradient.GetPeg(i).position);

			if(m_Impl->m_LeftDownSide)
			{
				pegpoint.y = clientrect.bottom - 23 + m_Impl->GetPegIndent(i)*11;

				points[j*3+8*colcount].x = pegpoint.x+1;
				points[j*3+8*colcount].y = pegpoint.y-1;
				points[j*3+8*colcount+1].x = pegpoint.x-4;
				points[j*3+8*colcount+1].y = pegpoint.y+10;
				points[j*3+8*colcount+2].x = pegpoint.x+7;
				points[j*3+8*colcount+2].y = pegpoint.y+10;
				polycounts[j + 2*colcount] = 3;
				j++;
			}

			if(m_Impl->m_RightUpSide)
			{
				pegpoint.y = clientrect.bottom - m_Impl->GetPegIndent(i)*11 - drawwidth + 22;

				points[j*3+8*colcount].x = pegpoint.x+1;
				points[j*3+8*colcount].y = pegpoint.y+1;
				points[j*3+8*colcount+1].x = pegpoint.x-4;
				points[j*3+8*colcount+1].y = pegpoint.y-9;
				points[j*3+8*colcount+2].x = pegpoint.x+6;
				points[j*3+8*colcount+2].y = pegpoint.y-9;
				polycounts[j + 2*colcount] = 3;
				j++;
			}
		}
	}

	rgn->CreatePolyPolygonRgn(points, polycounts,
		(pegcount+2)*colcount, ALTERNATE);

	delete[] points;
	delete[] polycounts;
}

BOOL CGradientCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_Impl->m_ToolTipCtrl.m_hWnd)
		m_Impl->m_ToolTipCtrl.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CGradientCtrl::ShowTooltips(BOOL bShow)
{
	m_bShowToolTip = bShow;
	if(m_Impl->m_ToolTipCtrl.GetSafeHwnd() != nullptr)
		m_Impl->m_ToolTipCtrl.Activate(bShow);
	if(m_bShowToolTip)
		m_Impl->SynchronizeTooltips();
}

void CGradientCtrl::SetPegSide(BOOL setrightup, BOOL enable)
{
	if(setrightup) m_Impl->m_RightUpSide = enable;
	else m_Impl->m_LeftDownSide = enable;
}

BOOL CGradientCtrl::GetPegSide(BOOL rightup) const
{
	return rightup?m_Impl->m_RightUpSide:m_Impl->m_LeftDownSide;
}

void CGradientCtrl::SetTooltipFormat(CString format) {m_ToolTipFormat = format;}
CString CGradientCtrl::GetTooltipFormat() const {return m_ToolTipFormat;}

void CGradientCtrl::SendBasicNotification(UINT code, CPeg const& peg, int index)
{
	//----- Send parent messages -----//
	CWnd *pParent = GetParent();
	if (pParent)
	{
		PegNMHDR nmhdr;
		nmhdr.nmhdr.code = code;
		nmhdr.nmhdr.hwndFrom = GetSafeHwnd();
		nmhdr.nmhdr.idFrom = GetDlgCtrlID();
		nmhdr.peg = peg;
		nmhdr.index = index;
		pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&nmhdr));

		nmhdr.nmhdr.code = GC_CHANGE;
		pParent->SendMessage(WM_NOTIFY, nmhdr.nmhdr.idFrom, (DWORD_PTR)(&(nmhdr.nmhdr)));
	}
}

const CPeg CGradientCtrl::GetSelPeg() const
{
	if(m_Selected == -1)
		return m_Impl->m_Null;
	else if(m_Selected >= -3 && m_Selected < m_Gradient.GetPegCount())
		return m_Gradient.GetPeg(m_Selected);
	else ASSERT(nullptr); //Some kind of stupid selection error?
	return m_Impl->m_Null;
}

UINT CGradientCtrl::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CGradientCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
}
