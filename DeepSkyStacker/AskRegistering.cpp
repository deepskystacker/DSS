// RegisterSettings.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "AskRegistering.h"
#include "Registry.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CAskRegistering dialog


CAskRegistering::CAskRegistering(CWnd* pParent /*=nullptr*/)
	: CDialog(CAskRegistering::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAskRegistering)
	//}}AFX_DATA_INIT

    m_Result = ASKREGISTERINGANSWER(0);
}

/* ------------------------------------------------------------------- */

void CAskRegistering::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAskRegistering)
	DDX_Control(pDX, IDC_REGISTERONE, m_RegisterOne);
	DDX_Control(pDX, IDC_REGISTERALL, m_RegisterAll);
	DDX_Control(pDX, IDC_CONTINUE, m_RegisterNone);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CAskRegistering, CDialog)
	//{{AFX_MSG_MAP(CAskRegistering)
	ON_BN_CLICKED(IDC_REGISTERONE, OnRegisterOne)
	ON_BN_CLICKED(IDC_REGISTERALL, OnRegisterAll)
	ON_BN_CLICKED(IDC_CONTINUE, OnRegisterNone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CAskRegistering message handlers

BOOL CAskRegistering::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_RegisterOne.SetCheck(true);
	m_Result = ARA_ONE;

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

void CAskRegistering::OnOK()
{
	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */

void CAskRegistering::OnRegisterOne()
{
	if (m_RegisterOne.GetCheck())
	{
		m_RegisterAll.SetCheck(false);
		m_RegisterNone.SetCheck(false);
		m_Result = ARA_ONE;
	};
}

/* ------------------------------------------------------------------- */

void CAskRegistering::OnRegisterAll()
{
	if (m_RegisterAll.GetCheck())
	{
		m_RegisterOne.SetCheck(false);
		m_RegisterNone.SetCheck(false);
		m_Result = ARA_ALL;
	};
}

/* ------------------------------------------------------------------- */

void CAskRegistering::OnRegisterNone()
{
	if (m_RegisterNone.GetCheck())
	{
		m_RegisterAll.SetCheck(false);
		m_RegisterOne.SetCheck(false);
		m_Result = ARA_CONTINUE;
	};
}

/* ------------------------------------------------------------------- */

