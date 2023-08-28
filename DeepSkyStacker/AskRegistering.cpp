// RegisterSettings.cpp : implementation file
//


/*
don't forget to make connections when you port it to Qt GUI (make it not use SIGNAL and SLOT macros)
connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
connect(ui->registerOne, SIGNAL(clicked()), this, SLOT(onRegisterOne()));
connect(ui->registerAll, SIGNAL(clicked()), this, SLOT(onRegisterAll()));
connect(ui->registerNone, SIGNAL(clicked()), this, SLOT(onRegisterNone()));
*/

#include "stdafx.h"
#include "deepskystacker.h"
#include "AskRegistering.h"


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

