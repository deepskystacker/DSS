#if !defined(AFX_EDITINTERVAL_H__274F44B4_B484_11D3_B7E7_00C04F0DA600__INCLUDED_)
#define AFX_EDITINTERVAL_H__274F44B4_B484_11D3_B7E7_00C04F0DA600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditInterval.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditInterval dialog

class CEditInterval : public CDialog
{
// Construction
public:
	float m_Min, m_Max;
	CEditInterval(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditInterval)
	enum { IDD = IDD_EDITINTERVAL };
	float	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditInterval)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditInterval)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITINTERVAL_H__274F44B4_B484_11D3_B7E7_00C04F0DA600__INCLUDED_)
