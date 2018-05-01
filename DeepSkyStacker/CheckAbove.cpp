// CheckAbove.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"
#include "CheckAbove.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckAbove dialog


CCheckAbove::CCheckAbove(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckAbove::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckAbove)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_fThreshold = 0;
	m_bPercent   = FALSE;
}


void CCheckAbove::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckAbove)
	DDX_Control(pDX, IDC_THRESHOLD, m_Threshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckAbove, CDialog)
	//{{AFX_MSG_MAP(CCheckAbove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckAbove message handlers

void CCheckAbove::OnOK() 
{
	CString			strValue;

	m_Threshold.GetWindowText(strValue);

	if (strValue.Find(_T("%")) > 0)
	{
		m_bPercent = TRUE;
		strValue = strValue.Left(strValue.GetLength()-1);
		m_fThreshold = _ttof(strValue);
	}
	else
		m_fThreshold = _ttof(strValue);
	
	CDialog::OnOK();
}
