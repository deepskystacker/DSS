#if !defined(AFX_PROGRESSDLG_H__D0849A0E_7DFE_4678_8C62_BA6DBD081F81__INCLUDED_)
#define AFX_PROGRESSDLG_H__D0849A0E_7DFE_4678_8C62_BA6DBD081F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

#include "DeepStack.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CDialog
{
// Construction
public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CStatic			m_TimeRemaining;
	CButton			m_Cancel;
	CProgressCtrl	m_Progress2;
	CProgressCtrl	m_Progress1;
	CStatic			m_Text2;
	CStatic			m_Text1;
	CStatic			m_NrProcessors;
	//}}AFX_DATA

	BOOL			m_bCancelled;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public :
	void PeekAndPump()
	{
		if (::GetFocus() != m_hWnd)
			SetFocus();

		MSG msg;
		while (!m_bCancelled && ::PeekMessage(&msg, NULL,0,0,PM_NOREMOVE)) 
		{
/*
			if (bCancelOnESCkey && (msg.message == WM_CHAR) && (msg.wParam == VK_ESCAPE))
				OnCancel();

			// Cancel button disabled if modal, so we fake it.
			if (m_bModal && (msg.message == WM_LBUTTONUP))
			{
				CRect rect;
				m_CancelButton.GetWindowRect(rect);
				if (rect.PtInRect(msg.pt))
					OnCancel();
			}*/
  
			if (!AfxGetApp()->PumpMessage()) 
			{
				::PostQuitMessage(0);
				return;
			} 
		}
	}

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual void OnCancel();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/* ------------------------------------------------------------------- */

class CDSSProgressDlg : public CDSSProgress
{
private :
	CProgressDlg		m_dlg;
	LONG				m_lTotal1,
						m_lTotal2;
	DWORD				m_dwStartTime,
						m_dwLastTime;
	LONG				m_lLastTotal1,
						m_lLastTotal2;
	BOOL				m_bFirstProgress;

private :
	void				CreateProgressDialog()
	{
		if (!m_dlg.m_hWnd)
		{
			CWnd *pMainWnd = AfxGetMainWnd();
			m_dlg.Create(IDD_PROGRESS);

			// Disable main window
			if (pMainWnd)
			{
				m_dlg.CenterWindow(pMainWnd);
				pMainWnd->EnableWindow(FALSE);
			};

			// Re-enable this window
			m_dlg.EnableWindow(TRUE);
			m_dlg.ShowWindow(SW_SHOW);		
		};
	};

public :
	CDSSProgressDlg() {};
	virtual ~CDSSProgressDlg() 
	{
		Close();
	};

	virtual void	SetNrUsedProcessors(LONG lNrProcessors=1) 
	{
		if (m_dlg.m_hWnd)
		{
			if (lNrProcessors>1)
			{
				CString			strText;

				strText.Format(IDS_NRPROCESSORS, lNrProcessors);
				m_dlg.m_NrProcessors.SetWindowText(strText);
			}
			else
				m_dlg.m_NrProcessors.SetWindowText(_T(""));
		};
	};

	virtual void	GetStartText(CString & strText)
	{
		strText.Empty();
		if (m_dlg.m_hWnd)
			m_dlg.m_Text1.GetWindowText(strText);
	};

	virtual void	GetStart2Text(CString & strText)
	{
		strText.Empty();
		if (m_dlg.m_hWnd)
			m_dlg.m_Text2.GetWindowText(strText);
	};

	virtual	void	Start(LPCTSTR szTitle, LONG lTotal1, BOOL bEnableCancel = TRUE)
	{
		CString			strTitle = szTitle;

		CreateProgressDialog();

		m_lLastTotal1 = 0;
		m_lTotal1 = lTotal1;
		m_dwStartTime = GetTickCount();
		m_dwLastTime  = m_dwStartTime;
		m_bFirstProgress = TRUE;
		m_dlg.m_Cancel.EnableWindow(bEnableCancel);
		if (strTitle.GetLength())
			m_dlg.SetWindowText(strTitle);
		m_dlg.m_Progress1.SetRange32(0, lTotal1);
		m_dlg.m_Progress2.ShowWindow(SW_HIDE);
		m_dlg.m_Text2.ShowWindow(SW_HIDE);

		m_dlg.PeekAndPump();
	};
	virtual void	Progress1(LPCTSTR szText, LONG lAchieved1)
	{
		CString			strText = szText;
		DWORD			dwCurrentTime = GetTickCount();

		if (strText.GetLength())
			m_dlg.m_Text1.SetWindowText(szText);

		if (m_bFirstProgress || ((double)(lAchieved1-m_lLastTotal1) > (m_lTotal1 / 100.0)) || ((dwCurrentTime - m_dwLastTime) > 1000))
		{
			m_bFirstProgress = FALSE;
			m_lLastTotal1 = lAchieved1;
			m_dwLastTime  = dwCurrentTime;
			m_dlg.m_Progress1.SetPos(lAchieved1);

			if (m_lTotal1 > 1 && lAchieved1 > 1)
			{
				DWORD			dwRemainingTime;
				DWORD			dwHour,
								dwMin,
								dwSec;

				dwRemainingTime = (DWORD)((double)(dwCurrentTime-m_dwStartTime) / (double)(lAchieved1-1) * (double)(m_lTotal1-lAchieved1+1));
				dwRemainingTime /= 1000;

				dwHour = dwRemainingTime / 3600;
				dwRemainingTime -= dwHour * 3600;
				dwMin = dwRemainingTime / 60;
				dwRemainingTime -= dwMin * 60;
				dwSec = dwRemainingTime;

				if (dwHour)
					strText.Format(IDS_ESTIMATED3, dwHour, dwMin, dwSec);
				else if (dwMin)
					strText.Format(IDS_ESTIMATED2, dwMin, dwSec);
				else if (dwSec)
					strText.Format(IDS_ESTIMATED1, dwSec);
				else
					strText.Format(IDS_ESTIMATED0);
				m_dlg.m_TimeRemaining.SetWindowText(strText);
			}
			else
			{
				strText.LoadString(IDS_ESTIMATEDUNKNOWN);
				m_dlg.m_TimeRemaining.SetWindowText(strText);
			};

			m_dlg.PeekAndPump();
		};
	};
	
	virtual void	Start2(LPCTSTR szText, LONG lTotal2)
	{
		CString			strText = szText;

		CreateProgressDialog();

		m_lLastTotal2 = 0;
		if (strText.GetLength())
			m_dlg.m_Text2.SetWindowText(strText);

		m_dlg.m_Progress2.SetRange32(0, lTotal2);
		m_lTotal2 = lTotal2;
		if (!lTotal2)
		{
			m_dlg.m_Progress2.ShowWindow(SW_HIDE);
			m_dlg.m_Text2.ShowWindow(SW_HIDE);
		}
		else
		{
			m_dlg.m_Progress2.ShowWindow(SW_SHOW);
			m_dlg.m_Text2.ShowWindow(SW_SHOW);
			m_dlg.m_Progress2.SetPos(0);
		};

		if (m_bJointProgress)
		{
			Start(NULL, lTotal2, FALSE);
			if (strText.GetLength())
				m_dlg.m_Text1.SetWindowText(szText);
		};

		m_dlg.PeekAndPump();
	};

	virtual void	Progress2(LPCTSTR szText, LONG lAchieved2)
	{
		if ((double)(lAchieved2 - m_lLastTotal2) > (m_lTotal2 / 100.0))
		{
			CString			strText = szText;

			m_lLastTotal2 = lAchieved2;

			if (strText.GetLength())
				m_dlg.m_Text2.SetWindowText(strText);

			m_dlg.m_Progress2.SetPos(lAchieved2);
			m_dlg.PeekAndPump();
		};

		if (m_bJointProgress)
			Progress1(szText, lAchieved2);
	};

	virtual void	End2()
	{
		m_dlg.m_Progress2.ShowWindow(SW_HIDE);
		m_dlg.m_Text2.ShowWindow(SW_HIDE);
	};

	virtual BOOL	IsCanceled()
	{
		return m_dlg.m_bCancelled;
	};
	virtual BOOL	Close()
	{
		m_dlg.PeekAndPump();
		m_dlg.EndDialog(TRUE);

		CWnd *pMainWnd = AfxGetMainWnd();
		if (pMainWnd)
			pMainWnd->EnableWindow(TRUE);

		return TRUE;
	};

	virtual BOOL	Warning(LPCTSTR szText) 
	{ 
		int				nResult;

		nResult = AfxMessageBox(szText, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
		return (nResult == IDYES); 
	}

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__D0849A0E_7DFE_4678_8C62_BA6DBD081F81__INCLUDED_)
