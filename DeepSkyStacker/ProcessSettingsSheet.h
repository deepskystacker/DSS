#if !defined(AFX_PROCESSSETTINGSSHEET_H__E02AC601_555D_4F13_9260_01D84356A200__INCLUDED_)
#define AFX_PROCESSSETTINGSSHEET_H__E02AC601_555D_4F13_9260_01D84356A200__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessSettingsSheet.h : header file
//

#include "RGBTab.h"
#include "LuminanceTab.h"
#include "SaturationTab.h"

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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSSETTINGSSHEET_H__E02AC601_555D_4F13_9260_01D84356A200__INCLUDED_)
