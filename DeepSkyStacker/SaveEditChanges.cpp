// RegisterSettings.cpp : implementation file
//

#include "stdafx.h"
#include "SaveEditChanges.h"

#include <QSettings>

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSaveEditChanges dialog


CSaveEditChanges::CSaveEditChanges(CWnd* pParent /*=nullptr*/)
	: CDialog(CSaveEditChanges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveEditChanges)
	//}}AFX_DATA_INIT

}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveEditChanges)
	DDX_Control(pDX, IDC_ASKAGAIN, m_AskAlways);
	DDX_Control(pDX, IDC_SAVEWITHOUTASKING, m_SaveDontAsk);
	DDX_Control(pDX, IDC_DONTSAVEWITHOUTASKING, m_DontSaveDontAsk);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CSaveEditChanges, CDialog)
	//{{AFX_MSG_MAP(CSaveEditChanges)
	ON_BN_CLICKED(IDC_ASKAGAIN, OnAskAlways)
	ON_BN_CLICKED(IDC_SAVEWITHOUTASKING, OnSaveDontAsk)
	ON_BN_CLICKED(IDC_DONTSAVEWITHOUTASKING, OnDontSaveDontAsk)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CSaveEditChanges message handlers

BOOL CSaveEditChanges::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch (GetSaveEditMode())
	{
	case EditSaveMode::SECM_SAVEDONTASK :
		m_SaveDontAsk.SetCheck(true);
		break;
	case EditSaveMode::SECM_DONTSAVEDONTASK :
		m_DontSaveDontAsk.SetCheck(true);
		break;
	case EditSaveMode::SECM_ASKALWAYS :
		m_AskAlways.SetCheck(true);
		break;
	};

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::SaveSettings()
{
	EditSaveMode	Mode = EditSaveMode::SECM_ASKALWAYS;

	if (m_SaveDontAsk.GetCheck())
		Mode = EditSaveMode::SECM_SAVEDONTASK;
	else if (m_DontSaveDontAsk.GetCheck())
		Mode = EditSaveMode::SECM_DONTSAVEDONTASK;

	SetSaveEditMode(Mode);
};

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnYes()
{
	SaveSettings();
	EndDialog(IDYES);
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnNo()
{
	SaveSettings();
	EndDialog(IDNO);
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnCancel()
{
	CDialog::OnCancel();
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnAskAlways()
{
	if (m_AskAlways.GetCheck())
	{
		m_SaveDontAsk.SetCheck(false);
		m_DontSaveDontAsk.SetCheck(false);
	};
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnSaveDontAsk()
{
	if (m_SaveDontAsk.GetCheck())
	{
		m_AskAlways.SetCheck(false);
		m_DontSaveDontAsk.SetCheck(false);
	};
}

/* ------------------------------------------------------------------- */

void CSaveEditChanges::OnDontSaveDontAsk()
{
	if (m_DontSaveDontAsk.GetCheck())
	{
		m_AskAlways.SetCheck(false);
		m_SaveDontAsk.SetCheck(false);
	};
}

/* ------------------------------------------------------------------- */

void	SetSaveEditMode(EditSaveMode Mode)
{
	QSettings settings;
	uint	value = static_cast<uint>(Mode);

	settings.setValue("EditStars/AutoSave", value);
};

/* ------------------------------------------------------------------- */

EditSaveMode GetSaveEditMode()
{
	QSettings settings;

	uint value = settings.value("EditStars/AutoSave", (uint)0).toUInt();

	return static_cast<EditSaveMode>(value);
};

/* ------------------------------------------------------------------- */

int AskSaveEditChangesMode()
{
	QSettings settings;

	int value = settings.value("EditStars/AutoSave", 0).toInt();

	if (value == static_cast<int>(EditSaveMode::SECM_SAVEDONTASK))
		return IDYES;
	else if (value == static_cast<int>(EditSaveMode::SECM_DONTSAVEDONTASK))
		return IDNO;
	else
	{
		CSaveEditChanges	dlg;

		return dlg.DoModal();
	};
};

/* ------------------------------------------------------------------- */

