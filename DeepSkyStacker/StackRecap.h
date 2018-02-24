#pragma once
#include "afxcmn.h"
#include "StackingTasks.h"
#include "EasySize.h"

// CStackRecap dialog

class CStackRecap : public CDialog
{
	DECLARE_DYNAMIC(CStackRecap)
	DECLARE_EASYSIZE

public:
	CStackRecap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStackRecap();

	void		SetStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
	};

// Dialog Data
	enum { IDD = IDD_STACKRECAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSizing(UINT nSide, LPRECT lpRect);

	DECLARE_MESSAGE_MAP()
private :
	void	ClearText();

	void	InsertHeaderHTML(CString & strHTML);
	void	InsertHTML(CString & strHTML, LPCTSTR szText, COLORREF crColor = RGB(0, 0, 0), BOOL bBold = FALSE, BOOL bItalic = FALSE, LONG lLinkID = 0);
	void	FillWithAllTasksHTML();
	void	CallStackingParameters(LONG lID = 0);

private:
	CQhtmWnd						m_RecapHTML;
	CScrollBar						m_Gripper;

	CAllStackingTasks *				m_pStackingTasks;
	afx_msg void OnBnClickedStackingparameters();
	afx_msg void OnQHTMHyperlink(NMHDR*nmh, LRESULT*);

public:
	afx_msg void OnBnClickedRecommandedsettings();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

/* ------------------------------------------------------------------- */
