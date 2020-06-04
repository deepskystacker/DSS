// ToolTipBitmapButton.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTipButton.h"

/////////////////////////////////////////////////////////////////////////////
// CToolTipButton

CToolTipButton::CToolTipButton()
{
}

CToolTipButton::~CToolTipButton()
{
}


BEGIN_MESSAGE_MAP(CToolTipButton, baseCToolTipButton)
	//{{AFX_MSG_MAP(CToolTipButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipButton message handlers

// Set the tooltip with a CString
void CToolTipButton::SetToolTipText(LPCTSTR szText, BOOL bActivate)
{
	m_strTooltip = szText;

	// We cannot accept nullptr pointer
	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn;
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)m_strTooltip, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)m_strTooltip, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CToolTipButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == nullptr)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create active
		m_ToolTip.Activate(TRUE);
	}
} // End of InitToolTip


BOOL CToolTipButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	return baseCToolTipButton::PreTranslateMessage(pMsg);
}


