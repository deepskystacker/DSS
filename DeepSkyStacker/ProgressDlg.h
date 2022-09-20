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
	CProgressDlg(CWnd* pParent = nullptr);   // standard constructor

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

	bool			m_bCancelled;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public :
	void PeekAndPump()
	{
		MSG msg;
		while (!m_bCancelled && ::PeekMessage(&msg, nullptr,0,0,PM_NOREMOVE))
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
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedTimeremaining();
};

/* ------------------------------------------------------------------- */

class CDSSProgressDlg : public CDSSProgress
{
private :
	CProgressDlg		m_dlg;
	int					m_lTotal1;
	int					m_lTotal2;
	unsigned long long	m_dwStartTime;
	unsigned long long	m_dwLastTime;
	int					m_lLastTotal1;
	int					m_lLastTotal2;
	bool				m_bFirstProgress;
	bool				m_bEnableCancel;

private:
	void CreateProgressDialog();

public:
    CDSSProgressDlg(CWnd* pParent = nullptr) :
		m_dlg(pParent),
		m_bEnableCancel{ false },
		m_lTotal1{ 0 },
		m_lTotal2{ 0 },
		m_dwStartTime{ 0 },
		m_dwLastTime{ 0 },
		m_lLastTotal1{ 0 },
		m_lLastTotal2{ 0 },
		m_bFirstProgress{ false }
	{}

	virtual ~CDSSProgressDlg()
	{
		Close();
	};

	virtual void SetNrUsedProcessors(int lNrProcessors = 1) override
	{
		if (m_dlg.m_hWnd)
		{
			if (lNrProcessors > 1)
			{
				CString strText;
				strText.Format(IDS_NRPROCESSORS, lNrProcessors);
				m_dlg.m_NrProcessors.SetWindowText(strText);
			}
			else
				m_dlg.m_NrProcessors.SetWindowText(_T(""));
		};
	}

	virtual void GetStartText(CString & strText) override
	{
		strText.Empty();
		if (m_dlg.m_hWnd)
			m_dlg.m_Text1.GetWindowText(strText);
	};

	virtual void GetStart2Text(CString & strText) override
	{
		strText.Empty();
		if (m_dlg.m_hWnd)
			m_dlg.m_Text2.GetWindowText(strText);
	};

	virtual	void Start(LPCTSTR szTitle, int lTotal1, bool bEnableCancel = true) override
	{
		CString strTitle = szTitle;

		CreateProgressDialog();

		m_lLastTotal1 = 0;
		m_lTotal1 = lTotal1;
		m_dwStartTime = GetTickCount64();
		m_dwLastTime  = m_dwStartTime;
		m_bFirstProgress = true;
		m_bEnableCancel = bEnableCancel;
		m_dlg.m_Cancel.EnableWindow(bEnableCancel);
		if (strTitle.GetLength())
			m_dlg.SetWindowText(strTitle);
		m_dlg.m_Progress1.SetRange32(0, lTotal1);
		m_dlg.m_Progress2.ShowWindow(SW_HIDE);
		m_dlg.m_Text2.ShowWindow(SW_HIDE);

		/* Make sure that the progress dialog receives keyboard input events. */
		m_dlg.SetFocus();

		m_dlg.PeekAndPump();
	}

	virtual void Progress1(LPCTSTR szText, int lAchieved1) override
	{
		CString strText = szText;
		unsigned long long dwCurrentTime = GetTickCount64();

		if (strText.GetLength())
			m_dlg.m_Text1.SetWindowText(szText);

		if (m_bFirstProgress || (static_cast<double>(lAchieved1 - m_lLastTotal1) > (m_lTotal1 / 100.0)) || ((dwCurrentTime - m_dwLastTime) > 1000))
		{
			m_bFirstProgress = false;
			m_lLastTotal1 = lAchieved1;
			m_dwLastTime  = dwCurrentTime;
			m_dlg.m_Progress1.SetPos(lAchieved1);

			//DeepSkyStacker::instance()->PostMessage(WM_PROGRESS_UPDATE, lAchieved1, m_lTotal1); TODO

			if (m_lTotal1 > 1 && lAchieved1 > 1)
			{
				std::uint32_t dwRemainingTime = static_cast<std::uint32_t>(static_cast<double>(dwCurrentTime - m_dwStartTime) / static_cast<double>(lAchieved1 - 1) * static_cast<double>(m_lTotal1 - lAchieved1 + 1));
				dwRemainingTime /= 1000;

				const std::uint32_t dwHour = dwRemainingTime / 3600;
				dwRemainingTime -= dwHour * 3600;
				const std::uint32_t dwMin = dwRemainingTime / 60;
				dwRemainingTime -= dwMin * 60;
				const std::uint32_t dwSec = dwRemainingTime;

				if (dwHour != 0)
					strText.Format(IDS_ESTIMATED3, dwHour, dwMin, dwSec);
				else if (dwMin != 0)
					strText.Format(IDS_ESTIMATED2, dwMin, dwSec);
				else if (dwSec != 0)
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
	}

	virtual void Start2(LPCTSTR szText, int lTotal2) override
	{
		CreateProgressDialog();

		CString strText = szText;
		m_lLastTotal2 = 0;
		if (strText.GetLength())
			m_dlg.m_Text2.SetWindowText(strText);

		m_dlg.m_Progress2.SetRange32(0, lTotal2);
		m_lTotal2 = lTotal2;
		if (lTotal2 == 0)
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
			Start(nullptr, lTotal2, m_bEnableCancel);
			if (strText.GetLength())
				m_dlg.m_Text1.SetWindowText(szText);
		};

		m_dlg.PeekAndPump();
	}

	virtual void Progress2(LPCTSTR szText, int lAchieved2) override
	{
		if (static_cast<double>(lAchieved2 - m_lLastTotal2) > (m_lTotal2 / 100.0))
		{
			m_lLastTotal2 = lAchieved2;

			CString strText = szText;
			if (strText.GetLength())
				m_dlg.m_Text2.SetWindowText(strText);

			m_dlg.m_Progress2.SetPos(lAchieved2);
			m_dlg.PeekAndPump();
		};

		if (m_bJointProgress)
			Progress1(szText, lAchieved2);
	}

	virtual void End2() override
	{
		m_dlg.m_Progress2.ShowWindow(SW_HIDE);
		m_dlg.m_Text2.ShowWindow(SW_HIDE);
	}

	virtual bool IsCanceled() override
	{
		return m_dlg.m_bCancelled;
	}

	virtual bool Close() override;

	virtual bool Warning(LPCTSTR szText) override
	{
		const int nResult = AfxMessageBox(szText, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
		return (nResult == IDYES);
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__D0849A0E_7DFE_4678_8C62_BA6DBD081F81__INCLUDED_)
