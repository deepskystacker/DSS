// EditInterval.cpp : implementation file
//

#include "stdafx.h"
#include "EditInterval.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditInterval dialog


CEditInterval::CEditInterval(CWnd* pParent /*=NULL*/)
	: CDialog(CEditInterval::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditInterval)
	m_Value = 0.0f;
	//}}AFX_DATA_INIT
}


void CEditInterval::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditInterval)
	DDX_Text(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditInterval, CDialog)
	//{{AFX_MSG_MAP(CEditInterval)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditInterval message handlers

void CEditInterval::OnOK() 
{
	float prev;
	CWnd* wnd;
	CEdit* edit;

	prev = m_Value;
	UpdateData(TRUE);

	if(m_Value > m_Max || m_Value < m_Min)
	{
		m_Value = prev;
		UpdateData(FALSE);
		wnd = GetDlgItem(IDC_VALUE);
		wnd->SetFocus();
		edit = (CEdit*)wnd;
		edit->SetSel(0, -1);
		return;
	}
	
	CDialog::OnOK();
}
