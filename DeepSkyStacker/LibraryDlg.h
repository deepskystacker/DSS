#pragma once

#include <ControlPos.h>

// CLibraryDlg dialog

class CLibraryDlg : public CDialog
{
private :
	CControlPos				m_ControlPos;
	CQhtmWnd				m_HeaderHTML;
	CQhtmWnd				m_LibraryHTML;

	DECLARE_DYNAMIC(CLibraryDlg)

public:
	CLibraryDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLibraryDlg();

// Dialog Data
	enum { IDD = IDD_LIBRARY };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQHTMHyperlink(NMHDR*nmh, LRESULT*);

	DECLARE_MESSAGE_MAP()
};
