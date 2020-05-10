// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "OutputTab.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "ProgressDlg.h"
#include "MasterFrames.h"
#include "FolderDlg.h"

// COutputTab dialog

IMPLEMENT_DYNAMIC(COutputTab, CChildPropertyPage)

/* ------------------------------------------------------------------- */

COutputTab::COutputTab()
	: CChildPropertyPage(COutputTab::IDD)
{
	m_bFirstActivation = true;
}

/* ------------------------------------------------------------------- */

COutputTab::~COutputTab()
{
}

/* ------------------------------------------------------------------- */

void COutputTab::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE, m_Title);

	DDX_Control(pDX, IDC_CREATEOUTPUT, m_OutputFile);
	DDX_Control(pDX, IDC_CREATEHTML, m_OutputHTML);
	DDX_Control(pDX, IDC_AUTOSAVE, m_Autosave);
	DDX_Control(pDX, IDC_FILELIST, m_FileListName);
	DDX_Control(pDX, IDC_USEREFERENCEFRAMEFOLDER, m_RefFrameFolder);
	DDX_Control(pDX, IDC_USEFILELISTFOLDER, m_FileListFolder);
	DDX_Control(pDX, IDC_USEANOTHERFOLDER, m_OtherFolder);
	DDX_Control(pDX, IDC_APPENDNUMBER, m_AppendNumber);
	DDX_Control(pDX, IDC_OUTPUTFOLDER, m_OutputFolder);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(COutputTab, CChildPropertyPage)
	ON_BN_CLICKED(IDC_AUTOSAVE, &COutputTab::OnBnClickedAutosave)
	ON_BN_CLICKED(IDC_FILELIST, &COutputTab::OnBnClickedFilelist)
	ON_BN_CLICKED(IDC_USEREFERENCEFRAMEFOLDER, &COutputTab::OnBnClickedUsereferenceframefolder)
	ON_BN_CLICKED(IDC_USEFILELISTFOLDER, &COutputTab::OnBnClickedUsefilelistfolder)
	ON_BN_CLICKED(IDC_USEANOTHERFOLDER, &COutputTab::OnBnClickedUseanotherfolder)
	ON_BN_CLICKED(IDC_CREATEHTML, &COutputTab::OnBnClickedCreatehtml)
	ON_BN_CLICKED(IDC_CREATEOUTPUT, &COutputTab::OnBnClickedCreateoutput)
	ON_BN_CLICKED(IDC_APPENDNUMBER, &COutputTab::OnBnClickedAppendnumber)
	ON_NOTIFY(NM_LINKCLICK, IDC_OUTPUTFOLDER, OnOutputFolder)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void COutputTab::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());
	bool				bEnable = m_OutputFile.GetCheck();

	m_OutputHTML.EnableWindow(bEnable);
	m_Autosave.EnableWindow(bEnable);
	m_FileListName.EnableWindow(bEnable);
	m_AppendNumber.EnableWindow(bEnable);
	m_RefFrameFolder.EnableWindow(bEnable);
	m_FileListFolder.EnableWindow(bEnable);
	m_OtherFolder.EnableWindow(bEnable);
	m_OutputFolder.EnableWindow(bEnable);

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL COutputTab::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Title.SetTextColor(RGB(0, 0, 0));
		m_Title.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);

		m_OutputFolder.SetTransparent(true);
		m_OutputFolder.SetLink(true, true);
		m_OutputFolder.SetTextColor(RGB(0, 0, 128));

		m_OutputFolder.GetWindowText(m_strNoFolder);

		m_OutputFile.SetCheck(m_OutputSettings.m_bOutput);
		m_OutputHTML.SetCheck(m_OutputSettings.m_bOutputHTML);
		m_Autosave.SetCheck(m_OutputSettings.m_bAutosave);
		m_FileListName.SetCheck(m_OutputSettings.m_bFileList);
		m_AppendNumber.SetCheck(m_OutputSettings.m_bAppend);
		m_RefFrameFolder.SetCheck(m_OutputSettings.m_bRefFrameFolder);
		m_FileListFolder.SetCheck(m_OutputSettings.m_bFileListFolder);
		m_OtherFolder.SetCheck(m_OutputSettings.m_bOtherFolder);

		if ("" != m_OutputSettings.m_strFolder)
			m_OutputFolder.SetWindowText((LPCTSTR)m_OutputSettings.m_strFolder.utf16());

		UpdateControls();
		m_bFirstActivation = false;
	};

	return true;
};

/* ------------------------------------------------------------------- */
// COutputTab message handlers

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedAutosave()
{
	if (m_Autosave.GetCheck())
	{
		m_FileListName.SetCheck(false);
		m_OutputSettings.m_bAutosave = true;
		m_OutputSettings.m_bFileList = false;
	};
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedFilelist()
{
	if (m_FileListName.GetCheck())
	{
		m_Autosave.SetCheck(false);
		m_OutputSettings.m_bAutosave = false;
		m_OutputSettings.m_bFileList = true;
	};
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedUsereferenceframefolder()
{
	if (m_RefFrameFolder.GetCheck())
	{
		m_FileListFolder.SetCheck(false);
		m_OtherFolder.SetCheck(false);
		m_OutputSettings.m_bRefFrameFolder = true;
		m_OutputSettings.m_bFileListFolder = false;
		m_OutputSettings.m_bOtherFolder    = false;
	};
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedUsefilelistfolder()
{
	if (m_FileListFolder.GetCheck())
	{
		m_RefFrameFolder.SetCheck(false);
		m_OtherFolder.SetCheck(false);
		m_OutputSettings.m_bRefFrameFolder = false;
		m_OutputSettings.m_bFileListFolder = true;
		m_OutputSettings.m_bOtherFolder    = false;
	};
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedUseanotherfolder()
{
	if (m_OtherFolder.GetCheck())
	{
		m_FileListFolder.SetCheck(false);
		m_RefFrameFolder.SetCheck(false);
		m_OutputSettings.m_bRefFrameFolder = false;
		m_OutputSettings.m_bFileListFolder = false;
		m_OutputSettings.m_bOtherFolder    = true;
	};
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedCreatehtml()
{
	m_OutputSettings.m_bOutputHTML = m_OutputHTML.GetCheck() ? true : false;
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedCreateoutput()
{
	m_OutputSettings.m_bOutput = m_OutputFile.GetCheck() ? true : false;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void COutputTab::OnBnClickedAppendnumber()
{
	m_OutputSettings.m_bAppend = m_AppendNumber.GetCheck() ? true : false;
}

/* ------------------------------------------------------------------- */

void COutputTab::OnOutputFolder( NMHDR * pNotifyStruct, LRESULT * result )
{
	CString					strFolder;
	CString					strTitle;

	m_OutputFolder.GetWindowText(strFolder);

	CFolderDlg				dlg(false, strFolder, this);

	strTitle.LoadString(IDS_SELECTOUTPUTFOLDER);
	dlg.SetTitle(strTitle);

	if (dlg.DoModal() == IDOK)
	{
		strFolder = dlg.GetFolderName();
		m_OutputFolder.SetWindowText(strFolder);
		m_OutputSettings.m_strFolder = QString::fromWCharArray(strFolder.GetBuffer());
		m_OtherFolder.SetCheck(true);
		OnBnClickedUseanotherfolder();
	};
};

/* ------------------------------------------------------------------- */
