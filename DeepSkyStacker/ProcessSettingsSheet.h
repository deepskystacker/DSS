#pragma once

/////////////////////////////////////////////////////////////////////////////
// CProcessSettingsSheet

class CProcessSettingsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CProcessSettingsSheet)

// Construction
public:
	CProcessSettingsSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	CProcessSettingsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessSettingsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProcessSettingsSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CProcessSettingsSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

