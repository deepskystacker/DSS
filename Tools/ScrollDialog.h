#if !defined(AFX_SCROLLDIALOG_H__D46063D6_1480_11D3_8EEA_00104B30908A__INCLUDED_)
#define AFX_SCROLLDIALOG_H__D46063D6_1480_11D3_8EEA_00104B30908A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGripper
// This class is for the resizing gripper control
class CGripper : public CScrollBar
{
// Construction
public:
	CGripper(){};
// Implementation
public:
	virtual ~CGripper(){};
// Generated message map functions
protected:
	//{{AFX_MSG(CGripper)
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CScrollDialog dialog

class CScrollDialog : public CDialog
{
	DECLARE_DYNAMIC(CScrollDialog);

// Construction
public:
	CScrollDialog(UINT nIDD, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScrollDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected :
	BOOL				m_bVert,
						m_bHorz;
protected:
    void SetupScrollbars();
    void ResetScrollbars();
    int  m_nHorzInc, m_nVertInc,
         m_nVscrollMax, m_nHscrollMax,
         m_nVscrollPos, m_nHscrollPos;

    CRect m_ClientRect;
    BOOL m_bInitialized;
    CGripper m_Grip;

	// Generated message map functions
	//{{AFX_MSG(CScrollDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLDIALOG_H__D46063D6_1480_11D3_8EEA_00104B30908A__INCLUDED_)
