// Label.cpp : implementation file
//

#include "stdafx.h"
#include "Label.h"

BEGIN_MESSAGE_MAP(CLabel, CStatic)
	//{{AFX_MSG_MAP(CLabel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabel Version 1.2
//
// From now on I'll try to keep a log of fixes and enhancements...
//
// The new feature were added due to the response of people.
// All I ask is to all you programmers out there, is if you add, fix or
// enhance this code, sent me a copy and I'll send the copy on to www.codeproject.com
//
// Happy Software Engineer :)
//
// New features include:
//
// A. Support for 3D Fonts
// B. Support for background transparency
// C. More comments provided
// D. If alignment is 'centered' and the window text is seperated by '\r\n'
//	  the will be centered accordingly - requested by someone @ nasa ;)
// E. Support for font rotation.
// F. Respond to System Color Change
// G. OnPaint improved performance - using Double Buffering Technique
//
// Thanks to:
// Mark McDowell	- For suggestion on 'Increasing the flexibility of "hypertext" setting...'
// Erich Ruth		- For suggestion on 'Font Rotation'
//

/////////////////////////////////////////////////////////////////////////////
// CLabel Version 1.3
//
// A. Added SS_LEFTNOWORDWRAP to include wordwrap
// B. Fix repainting problem
// C. Fix SetBkColor
// D. Added SS_CENTER

// Thanks to:
// Marius						- Added styling problem.
// Azing Vondeling & Broker		- Spotting painting Problem.
// Mel Stober					- Back Color & SS_CENTER
//
/////////////////////////////////////////////////////////////////////////////
// CLabel Version 1.4
//
// A. Fix to transparency mode
// B. Added new SetText3DHiliteColor to change the 3D Font face color - default is white.
//
// Thanks to:
// michael.groeger				- Spotting Transparency with other controls bug.
//
//
/////////////////////////////////////////////////////////////////////////////
// CLabel Version 1.5
//
// A. Sanity handle check
// B. Support Interface Charset
// C. Check compilition with _UNICODE
// D. Fix hyperlink feature
// E. Support default Dialog Font
// F. Inclusion of SS_OWNERDRAW via control creation and subclassing
// G. Modification to Text aligmnent code
// H. New background gradient fill function
//
// Thanks to:
// Steve Kowald				- Using null handles
// Alan Chan				- Supporting International Windows
// Dieter Fauth				- Request for default Dialog font
// Herb Illfelder			- Text Alignment code
//
/////////////////////////////////////////////////////////////////////////////
// CLabel Version 1.6
// Jeroen Roosendaal		- SetFont suggestion
// Laurent					- Spotting SelectObject bugs
// Bernie					- Fix PreCreateWindow bug
// Jignesh I. Patel			- Added expanded tabs feature
// Jim Farmelant 			- Fix SetText crash

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::CLabel
//
// Description:		Default contructor
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel::CLabel()
{
	m_crText = GetSysColor(COLOR_WINDOWTEXT);

// 1.1
	m_hBackBrush = nullptr;

	m_crHiColor =		0;
	m_crLoColor	=		0;

	m_bTimer =			FALSE;
	m_bState =			FALSE;
	m_bTransparent =	FALSE;
	m_Link =			LinkNone;
	m_hCursor =			nullptr;
	m_Type =			None;
	m_bFont3d =			FALSE;
	m_bNotifyParent =	FALSE;
	m_bToolTips =		FALSE;
	m_bRotation =		FALSE;
	m_fillmode =		Normal;
	m_cr3DHiliteColor =	RGB(255,255,255);
	m_bHover =			FALSE;
    m_3dType =          Type3D(0);

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::~CLabel
//
// Description:
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				15092000	1.5		Handle Check
//////////////////////////////////////////////////////////////////////////
CLabel::~CLabel()
{
	// Clean up
	m_font.DeleteObject();
	::DeleteObject(m_hwndBrush);

	// Stop Checking complaining
	if (m_hBackBrush)
		::DeleteObject(m_hBackBrush);

}

void CLabel::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);

	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc,TRUE);
	GetParent()->UpdateWindow();

	RedrawWindow();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::ReconstructFont
//
// Description:		Helper function to build font after it was changed
//
// INPUTS:
//
// RETURNS:
//
// NOTES:			PROTECTED
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
void CLabel::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);

	ASSERT(bCreated);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::OnPaint
//
// Description:		Handles all the drawing code for the label
//
// INPUTS:
//
// RETURNS:
//
// NOTES:			Called by Windows... not by USER
//					Probably needs tiding up a some point.
//					Different states will require this code to be reworked.
//
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
// NT ALMOND				15092000	1.5		Handle Check
// NT ALMOND				15092000	1.5		Alignment mods
// NT ALMOND				15092000	1.5		Gradient Fill Mode
// NT ALMOND				02072002	1.6		Fill SelectObject bugs
// NT ALMOND				02072002	1.6		Added to expand tabs
//////////////////////////////////////////////////////////////////////////

void CLabel::Draw(CDC * pDC)
{
	DWORD dwFlags = 0;

	CRect rc;
	GetClientRect(rc);
	CString strText;
	GetWindowText(strText);
	CBitmap bmp;

	///////////////////////////////////////////////////////
	//
	// Set up for double buffering...
	//
	CDC* pDCMem;
	CBitmap*	pOldBitmap = nullptr;

	if (!m_bTransparent)
	{
		pDCMem = new CDC;
		pDCMem->CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
		pOldBitmap = pDCMem->SelectObject(&bmp);
	}
	else
	{
		pDCMem = pDC;
	}

	UINT nMode = pDCMem->SetBkMode(TRANSPARENT);

	COLORREF crText = pDCMem->SetTextColor(m_crText);
	CFont *pOldFont = pDCMem->SelectObject(&m_font);

	// Fill in backgound if not transparent
	if (!m_bTransparent)
	{
		if (m_fillmode == Normal)
		{
			CBrush br;

			if (m_hBackBrush != nullptr)
				br.Attach(m_hBackBrush);
			else
				br.Attach(m_hwndBrush);

			pDCMem->FillRect(rc,&br);

			br.Detach();
		}
		else // Gradient Fill
		{
			DrawGradientFill(pDCMem, &rc, m_crLoColor, m_crHiColor, 100);
		}

	}

	// If the text is flashing turn the text color on
	// then to the color of the window background.

	LOGBRUSH lb;
	ZeroMemory(&lb,sizeof(lb));

	// Stop Checking complaining
	if (m_hBackBrush)
		::GetObject(m_hBackBrush,sizeof(lb),&lb);

	// Something to do with flashing
	if (!m_bState && m_Type == Text)
		pDCMem->SetTextColor(lb.lbColor);

	DWORD style = GetStyle();

	switch (style & SS_TYPEMASK)
	{
		case SS_RIGHT:
			dwFlags = DT_RIGHT | DT_WORDBREAK;
			break;

		case SS_CENTER:
			dwFlags = SS_CENTER | DT_WORDBREAK;
			break;

		case SS_LEFTNOWORDWRAP:
			dwFlags = DT_LEFT;
			break;

		default: // treat other types as left
			case SS_LEFT:
				dwFlags = DT_LEFT | DT_WORDBREAK;
				break;
	}

	// Added to expand tabs...
	if(strText.Find(_T('\t')) != -1)
		dwFlags |= DT_EXPANDTABS;

	// If the text centered make an assumtion that
	// the will want to center verticly as well
	if (style & SS_CENTERIMAGE)
	{
		dwFlags = DT_CENTER;

		// Apply
		if (strText.Find(_T("\r\n")) == -1)
		{
			dwFlags |= DT_VCENTER;

			// And because DT_VCENTER only works with single lines
			dwFlags |= DT_SINGLELINE;
		}

	}

	//
	// 3333   DDDDD
	//     3  D    D
	//   33   D     D    E F X
	//     3  D    D
	// 3333   DDDDD
	//
	//
	if (m_bRotation)
	{
		int nAlign = pDCMem->SetTextAlign (TA_BASELINE);

		CPoint pt;
		GetViewportOrgEx (pDCMem->m_hDC,&pt) ;
		SetViewportOrgEx (pDCMem->m_hDC,rc.Width() / 2, rc.Height() / 2, nullptr) ;
		pDCMem->TextOut (0, 0, strText) ;
		SetViewportOrgEx (pDCMem->m_hDC,pt.x / 2, pt.y / 2, nullptr) ;
		pDCMem->SetTextAlign (nAlign);
	}
	else
	{
		pDCMem->DrawText(strText,rc,dwFlags);
		if (m_bFont3d)
		{
			pDCMem->SetTextColor(m_cr3DHiliteColor);

			if (m_3dType == Raised)
				rc.OffsetRect(-1,-1);
			else
				rc.OffsetRect(1,1);

			pDCMem->DrawText(strText,rc,dwFlags);
		}
	}

	// Restore DC's State
	pDCMem->SetBkMode(nMode);
	pDCMem->SelectObject(pOldFont);
	pDCMem->SetTextColor(crText);

	if (!m_bTransparent)
	{
		pDC->BitBlt(0,0,rc.Width(),rc.Height(),pDCMem,0,0,SRCCOPY);
		// continue DC restore
		pDCMem->SelectObject ( pOldBitmap ) ;
		delete pDCMem;
	}
};

void CLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
}

void CLabel::OnMouseMove([[maybe_unused]] UINT nFlags, [[maybe_unused]] CPoint pt)
{
	if (m_bHover)
	{
		CRect		rc;

		GetClientRect(&rc);
		if (!PtInRect(&rc, pt))
		{
			ReleaseCapture();
			SetFontUnderline(FALSE);
			m_bHover = FALSE;
		};
	}
	else
	{
		if (m_Link != LinkNone)
		{
			SetCapture();
			m_bHover = TRUE;
			// Change the appearance of the text
			SetFontUnderline(TRUE);
		};
	};
};

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::OnTimer
//
// Description:		Used in conjunction with 'FLASH' functions
//
// INPUTS:          Windows API
//
// RETURNS:         Windows API
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
void CLabel::OnTimer(UINT_PTR nIDEvent)
{

	m_bState = !m_bState;

	UpdateSurface();

	CStatic::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::OnSetCursor
//
// Description:		Used in conjunction with 'LINK' function
//
// INPUTS:          Windows API
//
// RETURNS:         Windows API
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
BOOL CLabel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

	if (m_hCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::OnLButtonDown
//
// Description:		Called when a link is click on
//
// INPUTS:          Windows API
//
// RETURNS:         Windows API
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				02072002    1.6     Added Mail support
//////////////////////////////////////////////////////////////////////////
void CLabel::OnLButtonDown(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	SetFontUnderline(FALSE);
	m_bHover = FALSE;

	if (!m_bNotifyParent) // Fix
	{
		CString strLink;

		GetWindowText(strLink);
		if (m_Link == HyperLink)
		{
			ShellExecute(nullptr,_T("open"),m_sLink.IsEmpty() ? strLink : m_sLink,nullptr,nullptr,SW_SHOWNORMAL);
		}
		if (m_Link == MailLink)
		{
			if (m_sLink.IsEmpty())
				strLink = "mailto:" + strLink;
			else
				strLink = m_sLink;
			ShellExecute( nullptr, nullptr,  strLink,  nullptr, nullptr, SW_SHOWNORMAL );
		}
	}
	else
	{
		// To use notification in parent window
		// Respond to a OnNotify in parent and disassemble the message
		//
		NMHDR nm;

		nm.hwndFrom = GetSafeHwnd();
		nm.idFrom  = GetDlgCtrlID();
		nm.code = NM_LINKCLICK;
		GetParent()->SendMessage(WM_NOTIFY,nm.idFrom,(LPARAM) &nm);
	}

	CStatic::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// THE FUNCTIONS START HERE :----
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetText
//
// Description:		Short cut to set window text - caption - label
//
// INPUTS:          Text to use
//
// RETURNS:         Reference to this
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26081998    1.0     Origin
// NT ALMOND				02072002    1.6     Crash Fix
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetText(const CString& strText)
{
	if(IsWindow(this->GetSafeHwnd()))
	{
		SetWindowText(strText);
		UpdateSurface();
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetTextColor
//
// Description:		Sets the text color
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetTextColor(COLORREF crText)
{

	m_crText = crText;

	UpdateSurface();
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFontBold
//
// Description:		Sets the font ot bold
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFontBold(BOOL bBold)
{

	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	UpdateSurface();
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFontUnderline
//
// Description:		Sets font underline attribue
//
// INPUTS:          True of false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFontUnderline(BOOL bSet)
{
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFontItalic
//
// Description:		Sets font italic attribute
//
// INPUTS:          True of false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFontItalic(BOOL bSet)
{

	m_lf.lfItalic = bSet;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetSunken
//
// Description:		Sets sunken effect on border
//
// INPUTS:          True of false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetSunken(BOOL bSet)
{

	if (!bSet)
		ModifyStyleEx(WS_EX_STATICEDGE,0,SWP_DRAWFRAME);
	else
		ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_DRAWFRAME);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetBorder
//
// Description:		Toggles the border on/off
//
// INPUTS:          True of false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetBorder(BOOL bSet)
{

	if (!bSet)
		ModifyStyle(WS_BORDER,0,SWP_DRAWFRAME);
	else
		ModifyStyle(0,WS_BORDER,SWP_DRAWFRAME);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFontSize
//
// Description:		Sets the font size
//
// INPUTS:          True of false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFontSize(int nSize)
{

	CFont cf;
	LOGFONT lf;

	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);

	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;

//	nSize*=-1;
//	m_lf.lfHeight = nSize;
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetBkColor
//
// Description:		Sets background color
//
// INPUTS:          Colorref of background color
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh , BackFillMode mode)
{

	m_crLoColor = crBkgnd;
	m_crHiColor = crBkgndHigh;

	m_fillmode = mode;

	if (m_hBackBrush)
		::DeleteObject(m_hBackBrush);

	if (m_fillmode == Normal)
		m_hBackBrush = ::CreateSolidBrush(crBkgnd);

	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFontName
//
// Description:		Sets the fonts face name
//
// INPUTS:          String containing font name
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				15092000	1.5		Support internation windows
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFontName(const CString& strFont, BYTE byCharSet /* Default = ANSI_CHARSET */)
{

	m_lf.lfCharSet = byCharSet;

	_tcscpy(m_lf.lfFaceName,strFont);
	ReconstructFont();
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::FlashText
//
// Description:		As the function states
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::FlashText(BOOL bActivate)
{

	if (m_bTimer)
		KillTimer(1);

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;

		SetTimer(1,500,nullptr);

		m_Type = Text;
	}
	else
		m_Type = None; // Fix

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::FlashBackground
//
// Description:		As the function states
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::FlashBackground(BOOL bActivate)
{

	if (m_bTimer)
		KillTimer(1);

	if (bActivate)
	{
		m_bState = FALSE;

		m_bTimer = TRUE;
		SetTimer(1,500,nullptr);

		m_Type = Background;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetLink
//
// Description:		Indicates the string is a link
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
// NT ALMOND				26/08/99	1.2		Added flexbility of
//												Sending Click meessage to parent
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetLink(BOOL bLink,BOOL bNotifyParent)
{

	if (bLink)
		m_Link = HyperLink;
	else
		m_Link = LinkNone;

	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetLinkCursor
//
// Description:		Sets the internet browers link
//
// INPUTS:          Handle of cursor
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				26/08/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////

CLabel& CLabel::SetLinkCursor(HCURSOR hCursor)
{

	m_hCursor = hCursor;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetTransparent
//
// Description:		Sets the Label window to be transpaent
//
// INPUTS:          True or false
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////

CLabel& CLabel::SetTransparent(BOOL bSet)
{

	m_bTransparent = bSet;
	ModifyStyleEx(0,WS_EX_TRANSPARENT); // Fix for transparency
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFont3D
//
// Description:		Sets the 3D attribute of the font.
//
// INPUTS:          True or false, Raised or Sunken
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFont3D(BOOL bSet,Type3D type)
{

	m_bFont3d = bSet;
	m_3dType = type;
	UpdateSurface();

	return *this;
}

void CLabel::OnSysColorChange()
{

	if (m_hwndBrush)
		::DeleteObject(m_hwndBrush);

	m_hwndBrush = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	UpdateSurface();

}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetRotationAngle
//
// Description:		Sets the rotation angle for the current font.
//
// INPUTS:          Angle in Degress
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				22/10/98    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetRotationAngle(UINT nAngle,BOOL bRotation)
{
	// Arrrrh...
	// Your looking in here why the font is rotating, aren't you?
	// Well try setting the font name to 'Arial' or 'Times New Roman'
	// Make the Angle 180 and set bRotation to true.
	//
	// Font rotation _ONLY_ works with TrueType fonts...
	//
	//
	m_lf.lfEscapement = m_lf.lfOrientation = (nAngle * 10);
	m_bRotation = bRotation;

	ReconstructFont();

	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetText3DHiliteColor
//
// Description:		Sets the 3D font hilite color
//
// INPUTS:          Color
//
// RETURNS:         Reference to 'this' object
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				17/07/00    1.0     Origin
//
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetText3DHiliteColor(COLORREF cr3DHiliteColor)
{
	m_cr3DHiliteColor = cr3DHiliteColor;
	UpdateSurface();

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::PreSubclassWindow
//
// Description:		Assigns default dialog font
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
// NT ALMOND				02072002    1.6     Fix crash when GetFont returns nullptr
//////////////////////////////////////////////////////////////////////////
void CLabel::PreSubclassWindow()
{

	CStatic::PreSubclassWindow();

	CFont* cf = GetFont();
	if(cf !=nullptr)
	{
		cf->GetObject(sizeof(m_lf),&m_lf);
	}
	else
	{
		GetObject(GetStockObject(SYSTEM_FONT),sizeof(m_lf),&m_lf);
	}

	ReconstructFont();

}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::PreCreateWindow
//
// Description:
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
//////////////////////////////////////////////////////////////////////////
BOOL CLabel::PreCreateWindow(CREATESTRUCT& cs)
{
	return CStatic::PreCreateWindow(cs);
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::DrawGradientFill
//
// Description:		Internal help function to gradient fill background
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				15092000    1.5     Origin
//////////////////////////////////////////////////////////////////////////
void CLabel::DrawGradientFill(CDC* pDC, CRect* pRect, COLORREF crStart, COLORREF crEnd, int nSegments)
{
	// Get the starting RGB values and calculate the incremental
	// changes to be applied.

	COLORREF cr;
	int nR = GetRValue(crStart);
	int nG = GetGValue(crStart);
	int nB = GetBValue(crStart);

	int neB = GetBValue(crEnd);
	int neG = GetGValue(crEnd);
	int neR = GetRValue(crEnd);

	if(nSegments > pRect->Width())
		nSegments = pRect->Width();

	int nDiffR = (neR - nR);
	int nDiffG = (neG - nG);
	int nDiffB = (neB - nB);

	int ndR = 256 * (nDiffR) / (max(nSegments,1));
	int ndG = 256 * (nDiffG) / (max(nSegments,1));
	int ndB = 256 * (nDiffB) / (max(nSegments,1));

	nR *= 256;
	nG *= 256;
	nB *= 256;

	neR *= 256;
	neG *= 256;
	neB *= 256;

	int nCX = pRect->Width() / max(nSegments,1), nLeft = pRect->left, nRight;
	pDC->SelectStockObject(NULL_PEN);

	for (int i = 0; i < nSegments; i++, nR += ndR, nG += ndG, nB += ndB)
	{
		// Use special code for the last segment to avoid any problems
		// with integer division.

		if (i == (nSegments - 1))
			nRight = pRect->right;
		else
			nRight = nLeft + nCX;

		cr = RGB(nR / 256, nG / 256, nB / 256);

		{
			CBrush br(cr);
			CBrush* pbrOld = pDC->SelectObject(&br);
			pDC->Rectangle(nLeft, pRect->top, nRight + 1, pRect->bottom + 1);
			pDC->SelectObject(pbrOld);
		}

		// Reset the left side of the drawing rectangle.

		nLeft = nRight;
	}
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetFont
//
// Description:		Sets font with LOGFONT structure
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetFont(LOGFONT lf)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));
	ReconstructFont();
	UpdateSurface();
	return *this;

}

BOOL CLabel::OnEraseBkgnd([[maybe_unused]]CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}

void CLabel::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	InvalidateRect(nullptr);
};

void CLabel::OnMove(int x, int y)
{
	CStatic::OnMove(x, y);
	InvalidateRect(nullptr);
};

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetMailLink
//
// Description:		Sets the label so it becomes Mail enabled
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetMailLink(BOOL bEnable, BOOL bNotifyParent)
{
	if (bEnable)
		m_Link = MailLink;
	else
		m_Link = LinkNone;

	m_bNotifyParent = bNotifyParent;

	if (m_Link != LinkNone)
		ModifyStyle(0,SS_NOTIFY);
	else
		ModifyStyle(SS_NOTIFY,0);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:		CLabel::SetHyperLink
//
// Description:		Sets the label so it becomes hyperlink enabled
//
// INPUTS:
//
// RETURNS:
//
// NOTES:
//
// MODIFICATIONS:
//
// Name                     Date        Version Comments
// NT ALMOND				02072002    1.6     Origin
//////////////////////////////////////////////////////////////////////////
CLabel& CLabel::SetHyperLink(const CString& sLink)
{
	m_sLink = sLink;

	return *this;
}
