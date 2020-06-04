// GradientCtrlImpl.cpp: implementation of the CGradientCtrlImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GradientCtrlImpl.h"
#include "GradientCtrl.h"
#include "Gradient.h"

const TCHAR ampersandtag[] = _T("&&");
const TCHAR postag[] = _T("&SELPOS");
const TCHAR rtag[] = _T("&R");
const TCHAR gtag[] = _T("&G");
const TCHAR btag[] = _T("&B");
const TCHAR hexrtag[] = _T("&HEXR");
const TCHAR hexgtag[] = _T("&HEXG");
const TCHAR hexbtag[] = _T("&HEXB");
const TCHAR floatrtag[] = _T("&FLOATR");
const TCHAR floatgtag[] = _T("&FLOATG");
const TCHAR floatbtag[] = _T("&FLOATB");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradientCtrlImpl::CGradientCtrlImpl(CGradientCtrl *owner)
{
	m_Owner = owner;
	m_wndToolTip = nullptr;
	m_RectCount = 0;
	m_RightUpSide = true;
	m_LeftDownSide = false;
}

CGradientCtrlImpl::~CGradientCtrlImpl()
{

}

void CGradientCtrlImpl::Draw(CDC *dc)
{
	CRect clientrect;

	m_Owner->GetClientRect(&clientrect);

	//----- Draw the Palette -----//
	DrawGradient(dc);

	//----- Draw the marker arrows -----//
	DrawEndPegs(dc);
	DrawPegs(dc); //The order is important - Explanation: The function DrawSelPeg is called in the function DrawPegs so if we then draw the end peg we will be drawing over the inverse rectangle!
	DrawSelPeg(dc, m_Owner->m_Selected);

	//----- Draw a box around the palette -----//
	CBrush blackbrush(GetSysColor(COLOR_WINDOWTEXT));
	if(IsVertical())
	{

		dc->FrameRect(CRect(m_LeftDownSide ? 23 : 4, 4,
			GetDrawWidth() - (m_RightUpSide ? 23 : 4),
			clientrect.bottom-4), &blackbrush);
	}
	else
	{
		dc->FrameRect(CRect(4, clientrect.bottom - GetDrawWidth() + (m_RightUpSide ? 23 : 4),
			clientrect.right-4, clientrect.bottom-(m_LeftDownSide?23:4)),
			&blackbrush);
	}

	//clientrect.DeflateRect(1,1,1,1);
	//dc->DrawFocusRect(clientrect);
}

void CGradientCtrlImpl::DrawGradient(CDC *dc)
{
	CRect clientrect;
	bool vertical = IsVertical();
	int drawwidth = GetDrawWidth();
	int w = drawwidth - (m_RightUpSide ? 24 : 5) - (m_LeftDownSide ? 24 : 5);

	m_Owner->GetClientRect(&clientrect);
	if(clientrect.bottom < 11) return;

	//----- Draw the Gradient -----//
	CBitmap membmp;
	CDC memdc;
	COLORREF colour;
	POINT point;
	int l = (vertical ? clientrect.bottom : clientrect.right) - 10;

	RGBTRIPLE *pal = new RGBTRIPLE[l], *entry;

	if(vertical)
		membmp.CreateCompatibleBitmap(dc, 1, clientrect.bottom-10);
	else
		membmp.CreateCompatibleBitmap(dc, clientrect.right-10, 1);

	ASSERT(membmp.GetSafeHandle());

	memdc.CreateCompatibleDC(dc);
	ASSERT(memdc.GetSafeHdc());

	memdc.SelectObject(membmp);
	point = CPoint(0, 0);

	m_Owner->m_Gradient.MakeEntries(pal, l);

	if(vertical)
	{
		for(int i = 0; i < l; i++)
		{
			point.y = i;
			entry = &pal[i];
			colour = RGB(entry->rgbtRed, entry->rgbtGreen, entry->rgbtBlue);
			memdc.SetPixelV(point, colour);
		}
	}
	else
	{
		for(int i = 0; i < l; i++)
		{
			point.x = i;
			entry = &pal[i];
			colour = RGB(entry->rgbtRed, entry->rgbtGreen, entry->rgbtBlue);
			memdc.SetPixelV(point, colour);
		}
	}

	if(vertical)
		dc->StretchBlt(m_LeftDownSide ? 24 : 5, 5, w, l, &memdc, 0, 0, 1, l, SRCCOPY);
	else
		dc->StretchBlt(5, clientrect.bottom - drawwidth + (m_RightUpSide ? 24 : 5), l, w, &memdc, 0, 0, l, 1, SRCCOPY);

	//memdc.SelectObject(oldbmp);

	delete[] pal;

}

void CGradientCtrlImpl::DrawPegs(CDC *dc)
{
	CPeg peg;
	CRect clientrect;

	m_Owner->GetClientRect(&clientrect);

	// No stupid selection
	if(m_Owner->m_Selected > m_Owner->m_Gradient.GetPegCount())
		m_Owner->m_Selected = -1;

	int pegindent = 0;

	for(int i = 0; i < m_Owner->m_Gradient.GetPegCount(); i++)
	{
		peg = m_Owner->m_Gradient.GetPeg(i);

		if(m_RightUpSide)
		{
			//Indent if close
			pegindent = GetPegIndent(i)*11 + GetDrawWidth() - 23;

			//Obvious really
			if(IsVertical())
				DrawPeg(dc, CPoint(pegindent, PointFromPos(peg.position)), peg.colour, 0);
			else
				DrawPeg(dc, CPoint(PointFromPos(peg.position), clientrect.bottom - pegindent - 1), peg.colour, 1);
		}

		if(m_LeftDownSide)
		{
			//Indent if close
			pegindent = 23 - GetPegIndent(i)*11;

			//Obvious really
			if(IsVertical())
				DrawPeg(dc, CPoint(pegindent, PointFromPos(peg.position)), peg.colour, 2);
			else
				DrawPeg(dc, CPoint(PointFromPos(peg.position), clientrect.bottom - pegindent - 1), peg.colour, 3);

		}
	}
}

void CGradientCtrlImpl::DrawSelPeg(CDC *dc, CPoint point, int direction)
{
	POINT points[3];

	//Select The Colour
	CPen *oldpen = (CPen*)dc->SelectStockObject(BLACK_PEN);
	CBrush *oldbrush = (CBrush*)dc->SelectStockObject(NULL_BRUSH);
	int oldrop = dc->SetROP2(R2_NOT);

	//Prepare the coodrdinates
	switch(direction)
	{
	case 0:
		points[0].x = 8+point.x;
		points[0].y = point.y-2;
		points[1].x = 2+point.x;
		points[1].y = point.y+1;
		points[2].x = 8+point.x;
		points[2].y = point.y+4;
		break;
	case 1:
		points[0].x = point.x-2;
		points[0].y = point.y-8;
		points[1].x = point.x+1;
		points[1].y = point.y-2;
		points[2].x = point.x+4;
		points[2].y = point.y-8;
		break;
	case 2:
		points[0].x = point.x-9,	points[0].y = point.y-2;
		points[1].x = point.x-3,	points[1].y = point.y+1;
		points[2].x = point.x-9,	points[2].y = point.y+4;
		break;
	default:
		points[0].x = point.x-2;
		points[0].y = point.y+8;
		points[1].x = point.x+1;
		points[1].y = point.y+2;
		points[2].x = point.x+4;
		points[2].y = point.y+8;
		break;
	}
	dc->Polygon(points, 3);

	//Restore the old brush and pen
	dc->SetROP2(oldrop);
	dc->SelectObject(oldbrush);
	dc->SelectObject(oldpen);
}

void CGradientCtrlImpl::DrawSelPeg(CDC *dc, int peg)
{
	CBrush *oldbrush;
	CPen *oldpen;
	CRect clientrect;
	int drawwidth = GetDrawWidth()-23;
	bool vertical = IsVertical();

	m_Owner->GetClientRect(&clientrect);

	//"Select objects"//
	oldpen = (CPen*)dc->SelectStockObject(BLACK_PEN);
	oldbrush = (CBrush*)dc->SelectStockObject(NULL_BRUSH);
	int oldrop = dc->SetROP2(R2_NOT);

	if(peg == STARTPEG)
	{
		if(m_RightUpSide)
			if(vertical) dc->Rectangle(drawwidth+9, 5, drawwidth+14, 10);
			else dc->Rectangle(5, clientrect.bottom-drawwidth-9,
					10,	clientrect.bottom-drawwidth-14);

		if(m_LeftDownSide)
			if(vertical) dc->Rectangle(9, 5, 14, 10);
			else dc->Rectangle(5, clientrect.bottom-9, 10, clientrect.bottom-14);

		return;
	}

	if(peg == ENDPEG)
	{
		if(m_RightUpSide)
			if(vertical) dc->Rectangle(drawwidth+9, clientrect.bottom-10, drawwidth+14, clientrect.bottom-5);
			else dc->Rectangle(clientrect.right-10, clientrect.bottom-drawwidth-9,
					clientrect.right-5, clientrect.bottom-drawwidth-14);

		if(m_LeftDownSide)
			if(vertical) dc->Rectangle(9, clientrect.bottom-5, 14, clientrect.bottom-10);
			else dc->Rectangle(clientrect.right-5, clientrect.bottom-9,
				clientrect.right-10, clientrect.bottom-14);
		return;


	}

	dc->SelectObject(oldbrush);
	dc->SetROP2(oldrop);
	dc->SelectObject(oldpen);

	if(peg >= 0 && m_Owner->m_Gradient.GetPegCount())
	{
		CPeg mypeg = m_Owner->m_Gradient.GetPeg(peg);
		int pegindent = GetPegIndent(peg)*11;

		if(IsVertical())
		{
			if(m_RightUpSide)
				DrawSelPeg(dc, CPoint(pegindent + drawwidth, PointFromPos(mypeg.position)), 0);
			if(m_LeftDownSide)
				DrawSelPeg(dc, CPoint(23-pegindent, PointFromPos(mypeg.position)), 2);
		}
		else
		{
			if(m_RightUpSide)
				DrawSelPeg(dc, CPoint(PointFromPos(mypeg.position), clientrect.bottom-pegindent-drawwidth-1), 1);
			if(m_LeftDownSide)
				DrawSelPeg(dc, CPoint(PointFromPos(mypeg.position), clientrect.bottom-23+pegindent), 3);
		}
	}

	//"Restore old objects"//

}

void CGradientCtrlImpl::DrawEndPegs(CDC *dc)
{
	CRect clientrect;
	CPen blackpen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
	bool vertical = IsVertical();
	int drawwidth = GetDrawWidth();

	m_Owner->GetClientRect(&clientrect);
	CPen *oldpen = dc->SelectObject(&blackpen);

	//----- Draw the first marker -----//
	CBrush brush(m_Owner->m_Gradient.GetStartPegColour());
	CBrush *oldbrush = dc->SelectObject(&brush); //select the brush
	if(m_RightUpSide)
	{
		if(vertical)
			dc->Rectangle(drawwidth-15, 4, drawwidth-8, 11); //draw the rectangle
		else
			dc->Rectangle(4, clientrect.bottom-drawwidth+15,
				11, clientrect.bottom-drawwidth+8); //draw the rectangle
	}

	if(m_LeftDownSide)
	{
		if(vertical)
			dc->Rectangle(8, 4, 15, 11); //draw the rectangle
		else
			dc->Rectangle(4, clientrect.bottom-8,
				11, clientrect.bottom-15); //draw the rectangle
	}

	dc->SelectObject(oldbrush); // restore the old brush
	brush.DeleteObject();

	//----- Draw the second one -----//
	brush.CreateSolidBrush(m_Owner->m_Gradient.GetEndPegColour());
	oldbrush = dc->SelectObject(&brush); //select the brush

	if(m_LeftDownSide)
	{
		if(vertical)
			dc->Rectangle(8, clientrect.bottom-4, 15,
				clientrect.bottom-11);
		else
			dc->Rectangle(clientrect.right-4, clientrect.bottom-15,
				clientrect.right-11, clientrect.bottom-8);
	}

	if(m_RightUpSide)
	{
		if(vertical)
			dc->Rectangle(drawwidth-15, clientrect.bottom-4, drawwidth-8,
				clientrect.bottom-11);
		else
			dc->Rectangle(clientrect.right-4, clientrect.bottom-drawwidth+8,
				clientrect.right-11, clientrect.bottom-drawwidth+15);
	}

	dc->SelectObject(oldbrush); //restore the old brush

	dc->SelectObject(oldpen);
}

void CGradientCtrlImpl::DrawPeg(CDC *dc, CPoint point, COLORREF colour, int direction)
{
	CBrush brush, *oldbrush;
	POINT points[3];

	//Select The Colour
	brush.CreateSolidBrush(colour);
	CPen *oldpen = (CPen*)dc->SelectStockObject(NULL_PEN);
	oldbrush = dc->SelectObject(&brush);

	//Prepare the coodrdinates
	switch(direction)
	{
	case 0:
		points[0].x = point.x;
		points[0].y = point.y+1;
		points[1].x = point.x+9;
		points[1].y = point.y-3;
		points[2].x = point.x+9;
		points[2].y = point.y+5;
		break;
	case 1:
		points[0].x = point.x+1;
		points[0].y = point.y;
		points[1].x = point.x-3;
		points[1].y = point.y-9;
		points[2].x = point.x+5;
		points[2].y = point.y-9;
		break;
	case 2:
		points[0].x = point.x-1;
		points[0].y = point.y+1;
		points[1].x = point.x-10;
		points[1].y = point.y-3;
		points[2].x = point.x-10;
		points[2].y = point.y+5;
		break;
	default:
		points[0].x = point.x+1;
		points[0].y = point.y+1;
		points[1].x = point.x-3;
		points[1].y = point.y+10;
		points[2].x = point.x+5;
		points[2].y = point.y+10;
		break;
	}
	dc->Polygon(points, 3);

	//Restore the old brush and pen
	dc->SelectObject(oldbrush);
	dc->SelectObject(oldpen);

	//----- Draw lines manually in the right directions ------//
	CPen outlinepen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
	oldpen = dc->SelectObject(&outlinepen);

	dc->MoveTo(points[0]);
	dc->LineTo(points[1]);
	dc->LineTo(points[2]);
	dc->LineTo(points[0]);

	dc->SelectObject(oldpen);

	brush.DeleteObject();
}

CString CGradientCtrlImpl::ExtractLine(CString source, int line)
{
	int start = 0, end;
	CString textline;

	if(source == _T(""))
		return _T("");

	while(line > 0)
	{
		start = source.Find(_T('\n'), start);
		if(start == -1)
			return _T("");
		start++;
		line--;
	}

	end = source.Find(_T('\n'), start);
	if(end == -1)
		end = source.GetLength();

	textline = source;
	CString op = textline.Mid(start, end-start);

	return op;
}

void CGradientCtrlImpl::ParseToolTipLine(CString &tiptext, CPeg const& peg)
{
	CString str;

	str = _T("&");
	tiptext.Replace(ampersandtag, str);

	str.Format(_T("%.2f"), peg.position);
	tiptext.Replace(postag, str);

	str.Format(_T("%2.2X"), GetRValue(peg.colour));
	tiptext.Replace(hexrtag, str);

	str.Format(_T("%2.2X"), GetGValue(peg.colour));
	tiptext.Replace(hexgtag, str);

	str.Format(_T("%2.2X"), GetBValue(peg.colour));
	tiptext.Replace(hexbtag, str);

	str.Format(_T("%0.3f"), ((float)GetRValue(peg.colour))/255.0);
	tiptext.Replace(floatrtag, str);

	str.Format(_T("%0.3f"), ((float)GetGValue(peg.colour))/255.0);
	tiptext.Replace(floatgtag, str);

	str.Format(_T("%0.3f"), ((float)GetBValue(peg.colour))/255.0);
	tiptext.Replace(floatbtag, str);

	str.Format(_T("%u"), GetRValue(peg.colour));
	tiptext.Replace(rtag, str);

	str.Format(_T("%u"), GetGValue(peg.colour));
	tiptext.Replace(gtag, str);

	str.Format(_T("%u"), GetBValue(peg.colour));
	tiptext.Replace(btag, str);
}

void CGradientCtrlImpl::SynchronizeTooltips()
{
	CString tiptext;
	CRect pegrect, clientrect;
	int drawwidth = GetDrawWidth();

	CClientDC dc(m_Owner);

	m_Owner->GetClientRect(&clientrect);

	if(!m_ToolTipCtrl.GetSafeHwnd())
		m_ToolTipCtrl.Create(m_Owner);

	//----- Out with the old -----//
	for(int i = 0; i < m_RectCount+1; i++)
		m_ToolTipCtrl.DelTool(m_Owner, i);
	m_RectCount = m_Owner->m_Gradient.GetPegCount();

	if(m_Owner->m_ToolTipFormat == _T(""))
		return;

	//----- Add the main pegs -----//
	for(int i = 0; i < m_RectCount; i++)
	{
		if(m_LeftDownSide)
		{
			GetPegRect(i, &pegrect, false);
			tiptext = ExtractLine(m_Owner->m_ToolTipFormat, 1);
			ParseToolTipLine(tiptext, m_Owner->m_Gradient.GetPeg(i));
			m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, i+6);
		}

		if(m_RightUpSide)
		{
			GetPegRect(i, &pegrect, true);
			tiptext = ExtractLine(m_Owner->m_ToolTipFormat, 1);
			ParseToolTipLine(tiptext, m_Owner->m_Gradient.GetPeg(i));
			m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, i+6);
		}
	}

	//----- Add ones for the end pegs -----//
	tiptext = ExtractLine(m_Owner->m_ToolTipFormat, 2);
	ParseToolTipLine(tiptext, m_Owner->m_Gradient.GetPeg(STARTPEG));
	if(tiptext != _T(""))
	{
		if(IsVertical())
		{
			if(m_RightUpSide)
			{
				pegrect.SetRect(drawwidth-15, 4, drawwidth-8, 11);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 1);
			}

			if(m_LeftDownSide)
			{
				pegrect.SetRect(8, 4, 15, 11);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 2);
			}
		}
		else
		{
			if(m_RightUpSide)
			{
				pegrect.SetRect(4, clientrect.bottom-drawwidth+8, 11,
					clientrect.bottom-drawwidth+15);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 1);
			}

			if(m_LeftDownSide)
			{
				pegrect = CRect(4, clientrect.bottom-15, 11, clientrect.bottom-8);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 2);
			}
		}
	}

	tiptext = ExtractLine(m_Owner->m_ToolTipFormat, 3);
	ParseToolTipLine(tiptext, m_Owner->m_Gradient.GetPeg(ENDPEG));
	if(tiptext != _T(""))
	{
		if(IsVertical())
		{
			if(m_RightUpSide)
			{
				pegrect.SetRect(drawwidth-15, clientrect.bottom-11, drawwidth-8,
					clientrect.bottom-4);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 3);
			}

			if(m_LeftDownSide)
			{
				pegrect.SetRect(8, clientrect.bottom-11, 15,
					clientrect.bottom-4);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 4);
			}
		}
		else
		{
			if(m_RightUpSide)
			{
				pegrect.SetRect(clientrect.right-11, clientrect.bottom-drawwidth+8,
					clientrect.right-4, clientrect.bottom-drawwidth+15);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 3);
			}

			if(m_LeftDownSide)
			{
				pegrect.SetRect(clientrect.right-11, clientrect.bottom-15,
					clientrect.right-4, clientrect.bottom-8);
				m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 4);
			}
		}
	}

	//----- Add one for the gradient -----//
	if(IsVertical())
		pegrect.SetRect(m_LeftDownSide?23:4, 4,
			drawwidth-(m_RightUpSide?23:4),
			(clientrect.bottom>10)?clientrect.bottom-4:clientrect.bottom);
	else
		pegrect.SetRect(4, clientrect.bottom-drawwidth+(m_RightUpSide?23:4),
			(clientrect.right>10)?clientrect.right-4:clientrect.right,
			clientrect.bottom-(m_LeftDownSide?23:4));

	tiptext = ExtractLine(m_Owner->m_ToolTipFormat, 4);
	if(tiptext != _T(""))
	{
		m_ToolTipCtrl.AddTool(m_Owner, tiptext, pegrect, 5);
	}
}

void CGradientCtrlImpl::ShowTooltip(CPoint point, CString text)
{
	unsigned int uid = 0;       // for ti initialization

	if(text == _T(""))
		return;

	// CREATE A TOOLTIP WINDOW
	if(m_wndToolTip == nullptr)
	{
		m_wndToolTip = CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS, nullptr, TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, nullptr, nullptr);


		// INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE
		m_ToolInfo.cbSize = sizeof(TOOLINFO);
		m_ToolInfo.uFlags = TTF_TRACK;
		m_ToolInfo.hwnd = nullptr;
		m_ToolInfo.hinst = nullptr;
		m_ToolInfo.uId = uid;
		m_ToolInfo.lpszText = (LPTSTR)(LPCTSTR) text;
		// ToolTip control will cover the whole window
		m_ToolInfo.rect.left = 0;
		m_ToolInfo.rect.top = 0;
		m_ToolInfo.rect.right = 0;
		m_ToolInfo.rect.bottom = 0;

		// SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW
		::SendMessage(m_wndToolTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_ToolInfo);
	}

	::SendMessage(m_wndToolTip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(point.x, point.y));
	::SendMessage(m_wndToolTip, TTM_TRACKACTIVATE, true, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
}

/*void CGradientCtrlImpl::HideTooltip()
{
	if(m_wndToolTip != nullptr)
	::SendMessage (m_wndToolTip, TTM_TRACKACTIVATE, false, (LPARAM) (LPTOOLINFO) &m_ToolInfo);
}*/

/*void CGradientCtrlImpl::MoveTooltip(CPoint point)
{
	if(m_wndToolTip != nullptr)
	{
		::SendMessage(m_wndToolTip, TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(point.x, point.y));
	}
}*/

void CGradientCtrlImpl::SetTooltipText(CString text)
{
	if(m_wndToolTip != nullptr)
	{
		m_ToolInfo.lpszText = (LPTSTR)(LPCTSTR) text;
		::SendMessage(m_wndToolTip, TTM_SETTOOLINFO, 0, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
	}
}

void CGradientCtrlImpl::DestroyTooltip()
{
	::DestroyWindow(m_wndToolTip);
	m_wndToolTip = nullptr;
}

bool CGradientCtrlImpl::IsVertical()
{
	if(m_Owner->m_Orientation == CGradientCtrl::ForceVertical)
		return true;
	else if(m_Owner->m_Orientation == CGradientCtrl::ForceHorizontal)
		return false;
	else
	{
		CRect clientrect;
		m_Owner->GetClientRect(&clientrect);
		return (clientrect.right <= clientrect.bottom);
	}
}

int CGradientCtrlImpl::GetDrawWidth()
{
	CRect clientrect;
	int dw;

	m_Owner->GetClientRect(&clientrect);

	dw = (m_Owner->m_Width == GCW_AUTO) ? (IsVertical() ? clientrect.right :
		clientrect.bottom) : m_Owner->m_Width;
	return dw;
}

int CGradientCtrlImpl::PointFromPos(float pos)
{
	CRect clientrect;
	m_Owner->GetClientRect(clientrect);
	float length = IsVertical() ? ((float)clientrect.Height() - 10.0f)
		: ((float)clientrect.Width() - 10.0f);

	return (int)(pos*length) + 4;
}

float CGradientCtrlImpl::PosFromPoint(int point)
{
	CRect clientrect;
	m_Owner->GetClientRect(clientrect);
	int length = IsVertical() ? (clientrect.bottom -  9) : (clientrect.right -  9);
	int x = point-5;
	float val;

	val = (float)x/(float)length;

	if(val < 0) val = 0;
	else if(val > 1) val = 1;

	return val;
}

int CGradientCtrlImpl::GetPegIndent(int index)
{
	int lastpegpos = -1, pegindent = 0;

	for(int i = 0; i <= index; i++)
	{
		const CPeg &peg = m_Owner->m_Gradient.GetPeg(i);
		if(lastpegpos != -1 && lastpegpos >= PointFromPos(peg.position)-10)
			pegindent += 1;
		else pegindent = 0;

		lastpegpos = PointFromPos(peg.position);
	}

	return pegindent%2;
}

int CGradientCtrlImpl::PtInPeg(CPoint point)
{
	CPeg peg;
	CRect pegrect, clientrect;
	int drawwidth = GetDrawWidth();

	//----- Check if the point is on a marker peg -----//
	for(int i = 0; i < m_Owner->m_Gradient.GetPegCount(); i++)
	{
		peg = m_Owner->m_Gradient.GetPeg(i);

		if(m_LeftDownSide)
		{
			GetPegRect(i, &pegrect, false);
			if(pegrect.PtInRect(point))
				return i;
		}

		if(m_RightUpSide)
		{
			GetPegRect(i, &pegrect, true);
			if(pegrect.PtInRect(point))
				return i;
		}
	}

	//----- Check if the point is on an  end peg -----//
	m_Owner->GetClientRect(&clientrect);

	pegrect.left = drawwidth+8, pegrect.top = 4;
	pegrect.right = drawwidth+15, pegrect.bottom = 11;
	if(pegrect.PtInRect(point))
		return STARTPEG;

	pegrect.left = drawwidth+8, pegrect.top = clientrect.bottom-11;
	pegrect.right = drawwidth+15, pegrect.bottom = clientrect.bottom-4;
	if(pegrect.PtInRect(point))
		return ENDPEG;

	return -1;
}

void CGradientCtrlImpl::GetPegRect(int index, CRect *rect, bool right)
{
	CRect clientrect;
	int drawwidth = GetDrawWidth();
	bool vertical = IsVertical();
	m_Owner->GetClientRect(&clientrect);

	if(index == STARTPEG)
	{
		if(right)
		{
			if(vertical)
			{
				rect->left = drawwidth-15, rect->top = 4;
				rect->right = drawwidth-8, rect->bottom = 11;
			}
			else
			{
				rect->left = 4, rect->top = clientrect.bottom-drawwidth+8;
				rect->right = 11, rect->bottom = clientrect.bottom-drawwidth+15;
			}
		}
		else
		{
			if(vertical)
			{
				rect->left = 8, rect->top = 4;
				rect->right = 15, rect->bottom = 11;
			}
			else
			{
				rect->left = 4, rect->top = clientrect.bottom-15;
				rect->right = 11, rect->bottom = clientrect.bottom-8;
			}
		}

		return;
	}

	if(index == ENDPEG)
	{
		if(right)
		{
			if(vertical)
			{
				rect->left = drawwidth-15, rect->top = clientrect.bottom-11;
				rect->right = drawwidth-8, rect->bottom = clientrect.bottom-4;
			}
			else
			{
				rect->left = clientrect.right-11, rect->top = clientrect.bottom-drawwidth+8;
				rect->right = clientrect.right-4, rect->bottom = clientrect.bottom-drawwidth+15;
			}
		}
		else
		{
			if(vertical)
			{
				rect->left = 8, rect->top = clientrect.bottom-11;
				rect->right = 15, rect->bottom = clientrect.bottom-4;
			}
			else
			{
				rect->left = clientrect.right-11, rect->top = clientrect.bottom-15;
				rect->right = clientrect.right-4, rect->bottom = clientrect.bottom-8;
			}
		}

		return;
	}

	CPeg peg = m_Owner->m_Gradient.GetPeg(index);
	int p = PointFromPos(peg.position);
	int indent = GetPegIndent(index)*11;

	if(right)
	{
		if(vertical)
		{
			rect->top = p - 3;
			rect->bottom = p + 6;
			rect->left = drawwidth+indent-23;
			rect->right = drawwidth+indent-13;
		}
		else
		{
			rect->top = clientrect.bottom-drawwidth-indent+13;
			rect->bottom = clientrect.bottom-drawwidth-indent+23;
			rect->left = p - 3;
			rect->right = p + 6;
		}
	}
	else
	{
		if(vertical)
		{
			rect->top = p - 3;
			rect->bottom = p + 6;
			rect->left = 13-indent;
			rect->right = 23-indent;
		}
		else
		{
			rect->top = clientrect.bottom+indent-23;
			rect->bottom = clientrect.bottom+indent-13;
			rect->left = p - 3;
			rect->right = p + 6;
		}
	}
}
