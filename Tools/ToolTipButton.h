#if !defined(AFX_TOOLTIPBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_)
#define AFX_TOOLTIPBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipBitmapButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTipButton window

//#define baseCToolTipButton CBitmapButton
#define baseCToolTipButton CButton
// if you do not want to profit of CBitmapButton, put simply CButton instead

// CToolTipRessourceButton by Jean-Louis GUENEGO
// Thanks to Niek Albers.
// A cool CButton with tooltips
class CToolTipButton : public baseCToolTipButton
{
// Construction
public:
	CToolTipButton();

// Attributes
public:

protected:
	CToolTipCtrl m_ToolTip;
	CString		 m_strTooltip;

// Operations
public:
	void SetToolTipText(LPCTSTR szText, BOOL bActivate = TRUE);
protected:
	void InitToolTip();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolTipButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPBITMAPBUTTON_H__DFD18BA7_9BC4_11D5_8F75_0048546F01E7__INCLUDED_)
