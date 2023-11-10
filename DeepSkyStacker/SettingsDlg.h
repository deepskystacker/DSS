#pragma once
#include "dss_settings.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
private :
	CDSSSettings *			m_pSettings;
	CDSSSetting				m_CurrentSetting;
	bool					m_bLoadSettings;

// Construction
public:
	CSettingsDlg(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	CEdit			m_Name;
	CListBox		m_List;
	CButton			m_Load;
	CButton			m_Delete;
	CButton			m_Add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private :
	void	UpdateControls();

public :
	void	SetDSSSettings(CDSSSettings * pSettings, const CDSSSetting & cds)
	{
		m_pSettings			= pSettings;
		m_CurrentSetting	= cds;
	};

	bool	IsLoaded()
	{
		return m_bLoadSettings;
	};

	void	GetCurrentSettings(CDSSSetting & cds)
	{
		cds = m_CurrentSetting;
	};

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnLoad();
	afx_msg void OnSelchangeSettingslist();
	afx_msg void OnDblclkSettingslist();
	afx_msg void OnChangeSettingsname();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
