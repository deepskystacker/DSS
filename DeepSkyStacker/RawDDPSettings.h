#include "afxwin.h"
#if !defined(AFX_RAWDDPSETTINGS_H__ABCA7A4A_5016_4D90_BE60_7053D5D10AF5__INCLUDED_)
#define AFX_RAWDDPSETTINGS_H__ABCA7A4A_5016_4D90_BE60_7053D5D10AF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RawDDPSettings.h : header file
//

#include "RawFilesTab.h"
#include "FitsFilesTab.h"

/////////////////////////////////////////////////////////////////////////////
// CRawDDPSettings dialog

class CRawDDPSettings : public CDialog
{
// Construction
public:
	CRawDDPSettings(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRawDDPSettings)
	enum { IDD = IDD_RAWSETTINGS };
	CButton	m_OK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRawDDPSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private :
	CStatic					m_SheetStatic;
	CPropertySheet			m_Sheet;
	CRawFilesTab			m_tabRAWFiles;
	CFitsFilesTab			m_tabFITSFiles;

	void	UpdateControls();

protected:

	// Generated message map functions
	//{{AFX_MSG(CRawDDPSettings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RAWDDPSETTINGS_H__ABCA7A4A_5016_4D90_BE60_7053D5D10AF5__INCLUDED_)
