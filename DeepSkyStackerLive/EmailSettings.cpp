// DeepSkyStackerLiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EmailSettings.h"
#include <..\SMTP\PJNSMTP.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEmailSettings dialog


/* ------------------------------------------------------------------- */

CEmailSettings::CEmailSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CEmailSettings::IDD, pParent)
{
	m_pLiveSettings = NULL;
}

/* ------------------------------------------------------------------- */

void CEmailSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SENDTO, m_SendTo);
	DDX_Control(pDX, IDC_OBJECT, m_Object);
	DDX_Control(pDX, IDC_SMTPSERVER, m_SMTP);
	DDX_Control(pDX, IDC_ACCOUNT, m_Account);
	DDX_Control(pDX, IDC_SENDONCE, m_SendOnce);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CEmailSettings, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_TEST, &CEmailSettings::OnBnClickedTest)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CEmailSettings message handlers

BOOL CEmailSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pLiveSettings)
	{
		CString				strEmail;
		CString				strSMTP;
		CString				strObject;
		CString				strAccount;
		BOOL				bMultiple;

		m_pLiveSettings->GetEmailSettings(strEmail, strAccount, strSMTP, strObject);

		m_SendTo.SetWindowText(strEmail);
		m_Object.SetWindowText(strObject);
		m_SMTP.SetWindowText(strSMTP);
		m_Account.SetWindowText(strAccount);

		bMultiple = m_pLiveSettings->IsWarning_SendMultipleEmails();

		m_SendOnce.SetCheck(!bMultiple);
	};

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ------------------------------------------------------------------- */

void CEmailSettings::OnOK()
{
	if (m_pLiveSettings)
	{
		CString				strEmail;
		CString				strSMTP;
		CString				strObject;
		CString				strAccount;
		BOOL				bMultiple;

		m_SendTo.GetWindowText(strEmail);
		m_Object.GetWindowText(strObject);
		m_SMTP.GetWindowText(strSMTP);
		m_Account.GetWindowText(strAccount);
		m_pLiveSettings->SetEmailSettings(strEmail, strAccount, strSMTP, strObject);

		bMultiple = !m_SendOnce.GetCheck();
		m_pLiveSettings->SetWarning_SendMultipleEmails(bMultiple);
	};

	CDialog::OnOK();
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CEmailSettings::OnBnClickedTest()
{
	BOOL				bOk = TRUE;
	CString				strEmail;
	CString				strSMTP;
	CString				strObject;
	CString				strAccount;
	CString				strText;

	m_SendTo.GetWindowText(strEmail);
	m_Object.GetWindowText(strObject);
	m_SMTP.GetWindowText(strSMTP);
	m_Account.GetWindowText(strAccount);

	try
	{
		CPJNSMTPConnection smtp;
		smtp.Connect(strSMTP);

		CPJNSMTPMessage m;
		m.m_To.Add(CPJNSMTPAddress(strEmail));
		m.m_From = CPJNSMTPAddress(strAccount);
		m.m_sSubject = strObject;
		m.AddTextBody(strObject);
		smtp.SendMessage(m);
	}
	catch (...)
	{
		bOk = FALSE;
	};

	if (bOk)
		strText.LoadString(IDS_EMAILSENTOK);
	else
		strText.LoadString(IDS_ERRORSENDINGEMAIL);

	AfxMessageBox(strText, MB_OK | MB_ICONINFORMATION);
}

/* ------------------------------------------------------------------- */
