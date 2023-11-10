// childprop.cpp : implementation file
//
#include <stdafx.h>
#include "childprop.h"

/////////////////////////////////////////////////////////////////////////////
// CChildPropertyPage
//

IMPLEMENT_DYNCREATE(CChildPropertyPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CChildPropertyPage, CPropertyPage)
	ON_MESSAGE(PSM_QUERYSIBLINGS, OnQuerySiblings)
END_MESSAGE_MAP()

CChildPropertyPage::CChildPropertyPage(UINT nIDPage, UINT nIDCaption, UINT nSelectedPage, UINT nIDGroup) :
	m_nSelectedPage(nSelectedPage),
	m_nIDGroup(nIDGroup),
	CPropertyPage(nIDPage, nIDCaption)
{

}

CChildPropertyPage::~CChildPropertyPage()
{

}

void CChildPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	// validate the the data on the child pages
	if (pDX->m_bSaveAndValidate && GetPageCount() > 0)
	{
		if (!m_Sheet.GetActivePage()->UpdateData(TRUE))
		{
			pDX->Fail();
		}
	}
}

BOOL CChildPropertyPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

	if (GetPageCount() > 0)
	{
		for (int i=0; i<m_arrPages.GetSize(); i++)
		{
			m_Sheet.AddPage(m_arrPages[i]);
		}

		// only have tabs in one row
		m_Sheet.EnableStackedTabs(FALSE);
		// create the child sheet
		m_Sheet.Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP, m_nSelectedPage);
		// Fix for: "Child CPropertySheet Hangs If Focus Is Switched" (Q149501)
		m_Sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);

		if (CWnd* pWnd = (m_nIDGroup) ? GetDlgItem(m_nIDGroup) : 0)
		{
			CRect rcPage, rcSheet;
			GetWindowRect(rcPage);
			pWnd->GetWindowRect(rcSheet);
			// offset the sheet from the parent page
			rcSheet.OffsetRect(-rcPage.left, -rcPage.top);
			// make room for the group header
			if (pWnd->GetStyle() & BS_GROUPBOX)	rcSheet.top += 12;
			// make room around the sheet
			rcSheet.DeflateRect(12,12);
			// move it
			m_Sheet.MoveWindow(rcSheet);
			m_Sheet.GetTabControl()->MoveWindow(0, 0, rcSheet.Width(), rcSheet.Height());
			// repaint it
			m_Sheet.SetActivePage(m_nSelectedPage);
		}
		else
		{
			CRect rcSheet;
			GetClientRect(&rcSheet);
			m_Sheet.MoveWindow(rcSheet);
		}
	}
	return TRUE;
}

void CChildPropertyPage::OnOK()
{
	if (GetPageCount() > 0) m_Sheet.PressButton(PSBTN_OK);
    CPropertyPage::OnOK();
}

void CChildPropertyPage::OnCancel()
{
	if (GetPageCount() > 0) m_Sheet.PressButton(PSBTN_CANCEL);
    CPropertyPage::OnCancel();
}

LRESULT CChildPropertyPage::OnQuerySiblings(WPARAM wParam, LPARAM lParam)
{
	CPropertyPage* pPage = GetActivePage();
	return (pPage) ? pPage->QuerySiblings(wParam, lParam) : 0;
}
