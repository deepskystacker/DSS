#if !defined(AFX_CHECKABOVE_H__E483FD70_CF10_47FD_A5B9_2A4C9905C441__INCLUDED_)
#define AFX_CHECKABOVE_H__E483FD70_CF10_47FD_A5B9_2A4C9905C441__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckAbove.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckAbove dialog

class CCheckAbove : public CDialog
{
private :
	double				m_fThreshold;
	BOOL				m_bPercent;

// Construction
public:
	CCheckAbove(CWnd* pParent = nullptr);   // standard constructor
	double		GetThreshold()
	{
		return m_fThreshold;
	};

	BOOL		IsPercent()
	{
		return m_bPercent;
	};

// Dialog Data
	//{{AFX_DATA(CCheckAbove)
	enum { IDD = IDD_CHECKABOVE };
	CEdit	m_Threshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckAbove)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCheckAbove)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKABOVE_H__E483FD70_CF10_47FD_A5B9_2A4C9905C441__INCLUDED_)
