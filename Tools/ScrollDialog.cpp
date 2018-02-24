// ScrollDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "ios.h"
#include "ScrollDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The following definitions specify how many points 
// are scrolled at each press of the scrollbar arrows
#define HORZ_PTS 8
#define VERT_PTS 4

IMPLEMENT_DYNAMIC(CScrollDialog, CDialog);

/////////////////////////////////////////////////////////////////////////////
// CGripper message map
BEGIN_MESSAGE_MAP(CGripper, CScrollBar)
	//{{AFX_MSG_MAP(CGripper)
//	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGripper message handlers
UINT CGripper::OnNcHitTest(CPoint point) 
{
	UINT ht = CScrollBar::OnNcHitTest(point);
	if(ht==HTCLIENT)
	{
		ht = HTBOTTOMRIGHT;
	}
	return ht;
}

/////////////////////////////////////////////////////////////////////////////
// CScrollDialog dialog

CScrollDialog::CScrollDialog(UINT nIDD, CWnd* pParent /*=NULL*/)
	: CDialog(nIDD, pParent)
{
	//{{AFX_DATA_INIT(CScrollDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_bInitialized = FALSE;
	m_bVert = TRUE;
	m_bHorz = TRUE;

}


void CScrollDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScrollDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScrollDialog, CDialog)
	//{{AFX_MSG_MAP(CScrollDialog)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollDialog message handlers

BOOL CScrollDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // Set Initial Scroll Positions
    m_nHscrollPos = 0;
    m_nVscrollPos = 0;

    // Get the initial dimensions of the dialog
    GetClientRect(&m_ClientRect);
    m_bInitialized = TRUE;

    // Create a resize gripper
    CRect initRect;
    GetClientRect(initRect);
    initRect.left = initRect.right - GetSystemMetrics(SM_CXHSCROLL);
    initRect.top  = initRect.bottom - GetSystemMetrics(SM_CYVSCROLL);

    DWORD dwStyle = WS_CHILD | 
                    SBS_SIZEBOX | 
                    SBS_SIZEBOXBOTTOMRIGHTALIGN | 
                    SBS_SIZEGRIP | 
                    WS_VISIBLE;

    m_Grip.Create(dwStyle,initRect, this, AFX_IDW_SIZE_BOX);

    // Modify our window style to ensure that we have a resizeable dialog
/*    ModifyStyle(WS_BORDER | DS_MODALFRAME,
                WS_THICKFRAME | WS_OVERLAPPEDWINDOW,
                SWP_FRAMECHANGED);*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CScrollDialog::ResetScrollbars()
{
    // Reset our window scrolling information
    ScrollWindow(m_nHscrollPos*HORZ_PTS, 0, NULL, NULL);
    ScrollWindow(0, m_nVscrollPos*VERT_PTS, NULL, NULL);
    m_nHscrollPos = 0;
    m_nVscrollPos = 0;
    SetScrollPos(SB_HORZ, m_nHscrollPos, TRUE);
    SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
}


void CScrollDialog::SetupScrollbars()
{
    CRect tempRect;
    GetClientRect(&tempRect);

    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    BOOL bMaximized = (wp.showCmd == SW_MAXIMIZE);

    // Calculate how many scrolling increments for the client area
    m_nHorzInc = (m_ClientRect.Width()  - tempRect.Width())/HORZ_PTS;
    m_nVertInc = (m_ClientRect.Height() - tempRect.Height())/VERT_PTS;

    // Do not show the sizing gripper
    if ((m_nHorzInc > 0) || (m_nVertInc > 0 ) || bMaximized)
        m_Grip.ShowWindow(SW_HIDE);
    else
        m_Grip.ShowWindow(SW_SHOW);

    // Set the vertical and horizontal scrolling info
	if (m_bHorz)
	{
		m_nHscrollMax = max(0, m_nHorzInc);
		m_nHscrollPos = min (m_nHscrollPos, m_nHscrollMax);
		SetScrollRange(SB_HORZ, 0, m_nHscrollMax, FALSE);
		SetScrollPos(SB_HORZ, m_nHscrollPos, TRUE);
	};
	if (m_bVert)
	{
		m_nVscrollMax = max(0, m_nVertInc);
		m_nVscrollPos = min(m_nVscrollPos, m_nVscrollMax);
		SetScrollRange(SB_VERT, 0, m_nVscrollMax, FALSE);
		SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
	};

    // Setup the gripper
    if (!bMaximized)
    {
        CWnd *resizeWnd = GetDlgItem(AFX_IDW_SIZE_BOX);
	    if (resizeWnd) 
        {
	        CRect rect, rect2;
	        resizeWnd->GetWindowRect(&rect);
	        GetClientRect(rect2);
	        rect2.left = rect2.right - rect.Width();
	        rect2.top  = rect2.bottom - rect.Height();
	        resizeWnd->MoveWindow(&rect2);
	    }
    }
}

void CScrollDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    // Handle vertical scrollbar messages
    // These can be tweaked to better fit the implementation
    int nInc;
    switch (nSBCode)
    {
        case SB_TOP:        nInc = -m_nVscrollPos;               break;
        case SB_BOTTOM:     nInc = m_nVscrollMax-m_nVscrollPos;  break;
        case SB_LINEUP:     nInc = -1;                           break;
        case SB_LINEDOWN:   nInc = 1;                            break;
        case SB_PAGEUP:     nInc = min(-1, -m_nVertInc);         break;
        case SB_PAGEDOWN:   nInc = max(1, m_nVertInc);           break;
        case SB_THUMBTRACK: nInc = nPos - m_nVscrollPos;         break;
        default:            nInc = 0;
    }

    nInc = max(-m_nVscrollPos, min(nInc, m_nVscrollMax - m_nVscrollPos));

    if (nInc)
    {
        m_nVscrollPos += nInc;
        int iMove = -VERT_PTS * nInc;
        ScrollWindow(0, iMove, NULL, NULL);
        SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
    }

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScrollDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    // Handle horizontal scrollbar messages
    // These can be tweaked to better fit the implementation
    int nInc;
    switch (nSBCode)
    {
        case SB_TOP:        nInc = -m_nHscrollPos;               break;
        case SB_BOTTOM:     nInc = m_nHscrollMax-m_nHscrollPos;  break;
        case SB_LINEUP:     nInc = -1;                           break;
        case SB_LINEDOWN:   nInc = 1;                            break;
        case SB_PAGEUP:     nInc = -HORZ_PTS;                    break;
        case SB_PAGEDOWN:   nInc = HORZ_PTS;                     break;
        case SB_THUMBTRACK: nInc = nPos - m_nHscrollPos;         break;
        default:            nInc = 0;

    }

    nInc = max(-m_nHscrollPos, min(nInc, m_nHscrollMax - m_nHscrollPos));

    if (nInc)
    {
        m_nHscrollPos += nInc;
        int iMove = -HORZ_PTS * nInc;
        ScrollWindow(iMove, 0, NULL, NULL);
        SetScrollPos(SB_HORZ, m_nHscrollPos, TRUE);
    }
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScrollDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
    if (m_bInitialized)
    {
        ResetScrollbars();
        SetupScrollbars();
    }
}
