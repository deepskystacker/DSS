// ExplorerBar.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "LogTab.h"

/* ------------------------------------------------------------------- */
// CLogTab dialog

IMPLEMENT_DYNAMIC(CLogTab, CDialog)

CLogTab::CLogTab(CWnd* pParent /*=nullptr*/, bool bDarkMode /*=false*/)
	: CDialog(CLogTab::IDD, pParent),
	m_bDarkMode(bDarkMode)
{
}

/* ------------------------------------------------------------------- */

CLogTab::~CLogTab()
{
}

/* ------------------------------------------------------------------- */

void CLogTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_Log);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CLogTab, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CLogTab::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ControlPos.MoveControls();
}

/* ------------------------------------------------------------------- */

BOOL CLogTab::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ControlPos.SetParent(this);
	m_Log.SetWordWrapMode(WBF_WORDWRAP);

	m_ControlPos.AddControl(IDC_LOG, CP_RESIZE_HORIZONTAL | CP_RESIZE_VERTICAL);

	if(m_bDarkMode)
		m_Log.SetBackgroundColor(false, COLORREF(RGB(80, 80, 80)));

	return TRUE;
}

/* ------------------------------------------------------------------- */

void CLogTab::AddToLog(QString szText, BOOL bAddDateTime, BOOL bBold, BOOL bItalic, COLORREF crColor)
{
	CHARFORMAT			cf;
	CString				strTime;

	if (bAddDateTime)
	{
		SYSTEMTIME		SystemTime;
		TCHAR			szTime[1000];
		TCHAR			szDate[1000];

		GetLocalTime(&SystemTime);
		GetTimeFormat(GetThreadLocale(), 0, &SystemTime, nullptr, szTime, sizeof(szTime));
		GetDateFormat(GetThreadLocale(), DATE_SHORTDATE, &SystemTime, nullptr, szDate, sizeof(szDate));

		strTime = szDate;
		strTime += "  ";
		strTime += szTime;
	};

	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_LINK;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR
	cf.crTextColor = crColor;


	if (strTime.GetLength())
	{
		strTime+= " - ";
		m_Log.SetSel(m_Log.GetTextLength(), -1);
		m_Log.SetSelectionCharFormat(cf);
		m_Log.ReplaceSel((LPCTSTR)strTime);
	};

	if (bBold)
		cf.dwEffects |= CFE_BOLD;
	if (bItalic)
		cf.dwEffects |= CFE_ITALIC;

	m_Log.SetSel(m_Log.GetTextLength(), -1);
	m_Log.SetSelectionCharFormat(cf);
	m_Log.ReplaceSel(szText.toStdWString().c_str());
};
