// Bubble.cpp : implementation file
//

#include "stdafx.h"
#include "Bubble.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CBitmap	bmpShadow;
static CBrush	brShadow;

#define SHADOW_SIZE	5

/////////////////////////////////////////////////////////////////////////////
// CBubble

// static members
CString CBubble::m_strClassName;

CBubble::CBubble()
{
	m_iTextFormat = DT_CENTER;
	m_iRowHeight = 0;
	m_iImageWidth = 0;
	m_iImageHeight = 0;
	m_bShadow = FALSE;
}

CBubble::~CBubble()
{
}

BOOL CBubble::Create(CWnd* pWndParent, BOOL bShadow)
{
	m_bShadow = bShadow;

	// create our bubble window but leave it invisible

	// do we need to register the class?
	if (m_strClassName.IsEmpty ())
	{
		// first, create the background brush
		CBrush  brBrush;

		try
		{
			brBrush.CreateSolidBrush (::GetSysColor (COLOR_INFOBK));
		}
		catch (CResourceException* pEx)
		{
			// brush creation failed
			pEx->Delete ();
			return 0;
		}
		  
		// register the class name
		m_strClassName = ::AfxRegisterWndClass (
			CS_BYTEALIGNCLIENT | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 
			0, 
			(HBRUSH)brBrush.Detach ());
    
		// we're we successful?
    	if (m_strClassName.IsEmpty ())
		{
			return 0;
		}
	}  

	// create the bubble window and set the created flag
	CRect rect;
	rect.SetRectEmpty();

	HWND hwndParent = (pWndParent == NULL) ? NULL :
						pWndParent->GetSafeHwnd ();
	if (!CreateEx (0, m_strClassName, _T (""), WS_POPUP,
                           rect.left, rect.top, rect.right, rect.bottom, 
                           hwndParent, (HMENU)NULL))
	{
		return FALSE;
	}

	if (bmpShadow.GetSafeHandle () == NULL)
	{
		ASSERT (brShadow.GetSafeHandle () == NULL);

		int aPattern[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
		bmpShadow.CreateBitmap (8, 8, 1, 1, aPattern);
		brShadow.CreatePatternBrush (&bmpShadow);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CBubble, CWnd)
	//{{AFX_MSG_MAP(CBubble)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBubble message handlers

void CBubble::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// paint our text, centered in the window
	CRect rect;
	GetClientRect(rect);

	if (m_bShadow)
	{
		rect.right -= SHADOW_SIZE;
		rect.bottom -= SHADOW_SIZE;
	}

	dc.FillSolidRect (&rect, ::GetSysColor (COLOR_INFOBK));
	dc.Draw3dRect (rect, GetSysColor (COLOR_3DSHADOW), GetSysColor (COLOR_3DDKSHADOW));

	// select our font and setup for text painting
	CFont *pOldFont = (CFont*) dc.SelectStockObject (DEFAULT_GUI_FONT);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor (COLOR_INFOTEXT));

	// paint our text
	int y = rect.top + 3 * GetSystemMetrics(SM_CYBORDER);
	int x;

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	for (POSITION pos = m_strTextRows.GetHeadPosition (); pos != NULL;)
	{
		CString strText = m_strTextRows.GetNext (pos);

		x = m_iImageWidth + 5;

		if (strText [0] == '\r')	// Draw a line
		{
			dc.SelectStockObject (WHITE_PEN);
			dc.MoveTo (x, y);
			dc.LineTo (rect.right - x, y);

			dc.SelectStockObject (BLACK_PEN);
			dc.MoveTo (x, y + 1);
			dc.LineTo (rect.right - 5, y + 1);

			strText = strText.Right (strText.GetLength () - 1);
			y += 5;
		}

		if (strText [0] == '\a' &&	// Next 2 digits - image number
			m_Images.GetSafeHandle () != NULL)
		{
			int iImage = _ttol (strText.Mid (1, 2));
			m_Images.Draw (&dc, iImage, CPoint (3, y), ILD_NORMAL);

			strText = strText.Mid (3);
		}

		CRect rectText = rect;
		rectText.top = y;
		rectText.left = x;
		rectText.bottom = y + tm.tmHeight + tm.tmExternalLeading;

		int iFormat = m_iTextFormat | DT_SINGLELINE | DT_VCENTER;
		dc.DrawText (strText, rectText, iFormat);
		y += m_iRowHeight;
	}

  // Draw shadow:
	if (m_bShadow)
	{
		dc.SetBkColor(RGB(255,255,255));

		brShadow.UnrealizeObject ();
		CBrush *pbrOld = (CBrush *) dc.SelectObject (&brShadow);

		// bottom shadow

		dc.PatBlt (rect.left + SHADOW_SIZE,
			   rect.bottom,
			   rect.Width (),
			   SHADOW_SIZE,
			   0xA000C9);

		// right-side shadow

		dc.PatBlt (rect.right,
			   rect.top + SHADOW_SIZE,
			   SHADOW_SIZE,
			   rect.Height (),
			   0xA000C9);
		dc.SelectObject(pbrOld);
	}

	dc.SelectObject(pOldFont);
}
//*******************************************************************************************
BOOL CBubble::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
//*******************************************************************************************
afx_msg LONG CBubble::OnSetText(UINT, LONG lParam)
{
  // resize the bubble window to fit the new string
  int i;

	m_iTextFormat = DT_CENTER;

  //-----------------------------------------------------------
  // Parse a text to get bitmaps + lines. For example, if text
  // looks "<\a><image1-res-id>text1<\n><\a><image2-res-id>text2",
  // we should create a bubble window with tow rows and draw in the
  // beggining of each row a specific bitmap:
  // (REMARK: image should be placed in the start of row only!)
  //-----------------------------------------------------------
	CString str ((LPCTSTR) lParam);

	m_strTextRows.RemoveAll ();

	// compute new size based on string x extent
	CClientDC dc(this);
  
	CFont *pOldFont = (CFont*) dc.SelectStockObject (DEFAULT_GUI_FONT);

    // get the text metrics
    TEXTMETRIC tm;
    dc.GetTextMetrics(&tm);
      
	m_iRowHeight = max (m_iImageHeight, tm.tmHeight + tm.tmExternalLeading);
    int nBubbleHeight = 6*GetSystemMetrics(SM_CYBORDER);

	int nMaxWidth = 0;
	int nExtraHeight = 0;

	CString strTmp = str;
	while ((i = strTmp.Find ('\r')) != -1)
	{
		nExtraHeight += 5;
		strTmp = strTmp.Right (strTmp.GetLength () - i - 1);
	}

	do
	{
		CString strCurrRow;

		if ((i = str.Find ('\n')) == -1)
		{
			strCurrRow = str;	// Whole string
		}
		else
		{
			m_iTextFormat = DT_LEFT;
			strCurrRow = str.Left (i);
			str = str.Right (str.GetLength () - i - 1);
		}

		if (strCurrRow.IsEmpty ())
		{
			strCurrRow = " ";
		}

		m_strTextRows.AddTail (strCurrRow);

		int iCurrWidth = m_iImageWidth;
		if (strCurrRow [0] == '\a')
		{
			strCurrRow = strCurrRow.Mid (3);
		}

		iCurrWidth += dc.GetTextExtent (strCurrRow).cx;
		if (iCurrWidth > nMaxWidth)
		{
			nMaxWidth = iCurrWidth;
		}

		nBubbleHeight += m_iRowHeight;
	}
	while (i != -1);
  
	CRect rect;
	GetWindowRect(rect);  // get current size and position
  
	// compute width
	rect.right = rect.left + nMaxWidth + (6*GetSystemMetrics(SM_CXBORDER)) + 10;
  
	// set height
	rect.bottom = rect.top + nBubbleHeight + nExtraHeight;

	if (m_bShadow)
	{
		rect.right += SHADOW_SIZE;
		rect.bottom += SHADOW_SIZE;
	}
	  
	MoveWindow(&rect);
  
	// clean up
	dc.SelectObject(pOldFont);
  
	// do the default processing
	return CWnd::Default();
}
//*******************************************************************************************
void CBubble::Track(CPoint point, const CString& string)
{
	if (m_strLastText == string &&
		m_ptLastPoint == point)
	{
		return;
	}

	// set the text
	SetWindowText(string);

	// move the window
	SetWindowPos(&wndTop, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
  
	//-----------------------------------------------
	// Adjust the window position by the screen size:
	//-----------------------------------------------
	CRect rectWindow;
	GetWindowRect (rectWindow);

	if (rectWindow.right > ::GetSystemMetrics (SM_CXFULLSCREEN))
	{
		point.x = ::GetSystemMetrics (SM_CXFULLSCREEN) - rectWindow.Width ();
		SetWindowPos(&wndTop, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	}

	if (rectWindow.bottom > ::GetSystemMetrics (SM_CYFULLSCREEN) - 20)
	{
		point.y = ::GetSystemMetrics (SM_CYFULLSCREEN) - rectWindow.Height () - 20;
		SetWindowPos(&wndTop, point.x, point.y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	}

    // show the window
    ShowWindow(SW_SHOWNOACTIVATE);
	Invalidate ();
	UpdateWindow ();
    
	m_strLastText = string;
	m_ptLastPoint = point;
}
//*******************************************************************************************
void CBubble::Hide()
{
	// hide the bubble window
	ShowWindow(SW_HIDE);
	m_strLastText.Empty ();
}
//*******************************************************************************************
BOOL CBubble::SetImageList (UINT uiId, int iImageWidth, COLORREF cltTransparent)
{
	if (!m_Images.Create (uiId, iImageWidth, 0, cltTransparent))
	{
		return FALSE;
	}

	IMAGEINFO info;
	m_Images.GetImageInfo (0, &info);

	CRect rect = info.rcImage;
			
	m_iImageWidth = rect.Width () + 3;
	m_iImageHeight = rect.Height ();

	return TRUE;
}
