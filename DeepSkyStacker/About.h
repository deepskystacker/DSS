#pragma once

#include "afxwin.h"
#include "label.h"

// CAbout dialog

class CAbout : public CDialog
{
	DECLARE_DYNAMIC(CAbout)

public:
	CAbout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAbout();

// Dialog Data
	enum { IDD = IDD_ABOUT };

private :
	CButton			m_CheckVersion;
	CComboBox		m_Language;
	CQhtmWnd		m_Credits;
	CQhtmWnd		m_LanguageCredits;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
};
