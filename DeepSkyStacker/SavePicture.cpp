// SavePicture.cpp : implementation file
//

#include "stdafx.h"
#include "SavePicture.h"
#include "resourceCZ.h"

/* ------------------------------------------------------------------- */
// CSavePicture

IMPLEMENT_DYNAMIC(CSavePicture, CFileDialog)

CSavePicture::CSavePicture(bool bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		std::uint32_t dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0, false)
{
	SetTemplate(IDD_SAVEPICTURE, IDD_SAVEPICTURE);
	m_bApplied = false;
	m_bEnableUseRect = false;
	m_bUseRect = false;
	m_ofn.FlagsEx = OFN_EX_NOPLACESBAR;

	m_Compression = TC_DEFLATE;
}

/* ------------------------------------------------------------------- */

CSavePicture::~CSavePicture()
{
}

/* ------------------------------------------------------------------- */

void CSavePicture::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRawDDPSettings)
	DDX_Control(pDX, IDC_APPLIED, m_Applied);
	DDX_Control(pDX, IDC_EMBEDDED, m_Embedded);
	DDX_Control(pDX, IDC_USERECT, m_UseRect);
	DDX_Control(pDX, IDC_COMPRESSION_NONE, m_CompressionNone);
	DDX_Control(pDX, IDC_COMPRESSION_ZIP, m_CompressionZIP);
	DDX_Control(pDX, IDC_COMPRESSION_LZW, m_CompressionLZW);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CSavePicture, CFileDialog)
	ON_BN_CLICKED(IDC_APPLIED, OnApplied)
	ON_BN_CLICKED(IDC_EMBEDDED, OnEmbedded)
	ON_BN_CLICKED(IDC_USERECT, OnUseRect)
	ON_BN_CLICKED(IDC_COMPRESSION_NONE, OnCompressionNone)
	ON_BN_CLICKED(IDC_COMPRESSION_ZIP, OnCompressionZIP)
	ON_BN_CLICKED(IDC_COMPRESSION_LZW, OnCompressionLZW)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CSavePicture message handlers

BOOL CSavePicture::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	if (m_bApplied)
	{
		m_Applied.SetCheck(true);
		m_Embedded.SetCheck(false);
	}
	else
	{
		m_Applied.SetCheck(false);
		m_Embedded.SetCheck(true);
	};

	if (m_bEnableUseRect)
	{
		m_UseRect.EnableWindow(true);
		m_UseRect.SetCheck(m_bUseRect);
	}
	else
	{
		m_UseRect.EnableWindow(false);
	};

	m_Embedded.GetWindowText(m_strSaveEmbed);

	m_CompressionNone.SetCheck(m_Compression == TC_NONE);
	m_CompressionZIP.SetCheck(m_Compression == TC_DEFLATE);
	m_CompressionLZW.SetCheck(m_Compression == TC_LZW);

	CString			strText;

	strText.LoadString(IDS_TT_APPLIED);
	m_Applied.SetToolTipText(strText);

	strText.LoadString(IDS_TT_EMBEDDED);
	m_Embedded.SetToolTipText(strText);

	UpdateControls();
	EnableToolTips();
	EnableTrackingToolTips();

	return true;
}

/* ------------------------------------------------------------------- */

void CSavePicture::OnApplied()
{
	if (m_Applied.GetCheck())
	{
		m_Embedded.SetCheck(false);
		m_bApplied = true;
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnEmbedded()
{
	if (m_Embedded.GetCheck())
	{
		m_Applied.SetCheck(false);
		m_bApplied = false;
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnUseRect()
{
	m_bUseRect = m_UseRect.GetCheck();
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnCompressionNone()
{
	if (m_CompressionNone.GetCheck())
	{
		m_Compression = TC_NONE;
		m_CompressionZIP.SetCheck(false);
		m_CompressionLZW.SetCheck(false);
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnCompressionZIP()
{
	if (m_CompressionZIP.GetCheck())
	{
		m_Compression = TC_DEFLATE;
		m_CompressionNone.SetCheck(false);
		m_CompressionLZW.SetCheck(false);
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnCompressionLZW()
{
	if (m_CompressionLZW.GetCheck())
	{
		m_Compression = TC_LZW;
		m_CompressionNone.SetCheck(false);
		m_CompressionZIP.SetCheck(false);
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::UpdateControls()
{
	if (m_ofn.nFilterIndex>3)
	{
		m_Compression = TC_NONE;
		m_CompressionNone.SetCheck(true);
		m_CompressionLZW.SetCheck(false);
		m_CompressionZIP.SetCheck(false);
		m_CompressionLZW.EnableWindow(false);
		m_CompressionZIP.EnableWindow(false);
		CString			strText;

		strText.LoadString(IDS_SAVENOADJUSTMENT);
		m_Embedded.SetWindowText(strText);
	}
	else
	{
		m_CompressionLZW.EnableWindow(true);
		m_CompressionZIP.EnableWindow(true);
		m_Embedded.SetWindowText(m_strSaveEmbed);
	};
};

/* ------------------------------------------------------------------- */

void CSavePicture::OnTypeChange()
{
	CFileDialog::OnTypeChange();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

