// ProcessSettingsSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessSettingsSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CProcessSettingsSheet

IMPLEMENT_DYNAMIC(CProcessSettingsSheet, CPropertySheet)

CProcessSettingsSheet::CProcessSettingsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CProcessSettingsSheet::CProcessSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CProcessSettingsSheet::~CProcessSettingsSheet()
{
}


BEGIN_MESSAGE_MAP(CProcessSettingsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CProcessSettingsSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessSettingsSheet message handlers
