// DeepSkyStackerLiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RestartMonitoring.h"

// CRestartMonitoring dialog


/* ------------------------------------------------------------------- */

CRestartMonitoring::CRestartMonitoring(CWnd* pParent /*=nullptr*/)
	: CDialog(CRestartMonitoring::IDD, pParent)
{
    m_bClearStacked = false;
    m_bDropPending = false;
}

/* ------------------------------------------------------------------- */

void CRestartMonitoring::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CREATENEWSTACKEDIMAGE, m_CreateNewEmpty);
	DDX_Control(pDX, IDC_STARTFROMCURRENTSTACKEDIMAGE, m_StartFromCurrent);
	DDX_Control(pDX, IDC_DROPPENDINGIMAGES, m_DropPending);
	DDX_Control(pDX, IDC_USEPENDINGIMAGES, m_UsePending);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CRestartMonitoring, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CREATENEWSTACKEDIMAGE, &CRestartMonitoring::OnBnClickedCreatenewstackedimage)
	ON_BN_CLICKED(IDC_STARTFROMCURRENTSTACKEDIMAGE, &CRestartMonitoring::OnBnClickedStartfromcurrentstackedimage)
	ON_BN_CLICKED(IDC_DROPPENDINGIMAGES, &CRestartMonitoring::OnBnClickedDroppendingimages)
	ON_BN_CLICKED(IDC_USEPENDINGIMAGES, &CRestartMonitoring::OnBnClickedUsependingimages)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
// CRestartMonitoring message handlers

BOOL CRestartMonitoring::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_DropPending.SetCheck(TRUE);
	m_CreateNewEmpty.SetCheck(TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/* ------------------------------------------------------------------- */

void CRestartMonitoring::OnOK()
{
	m_bDropPending = m_DropPending.GetCheck();
	m_bClearStacked = m_CreateNewEmpty.GetCheck();

	CDialog::OnOK();
};

/* ------------------------------------------------------------------- */

void CRestartMonitoring::OnBnClickedCreatenewstackedimage()
{
	if (m_CreateNewEmpty.GetCheck())
	{
		m_StartFromCurrent.SetCheck(FALSE);
		m_DropPending.SetCheck(TRUE);
		m_UsePending.SetCheck(FALSE);
	};
}

/* ------------------------------------------------------------------- */

void CRestartMonitoring::OnBnClickedStartfromcurrentstackedimage()
{
	if (m_StartFromCurrent.GetCheck())
	{
		m_CreateNewEmpty.SetCheck(FALSE);
	};
}

/* ------------------------------------------------------------------- */

void CRestartMonitoring::OnBnClickedDroppendingimages()
{
	if (m_DropPending.GetCheck())
		m_UsePending.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CRestartMonitoring::OnBnClickedUsependingimages()
{
	if (m_UsePending.GetCheck())
		m_DropPending.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */
