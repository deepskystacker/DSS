#if !defined(AFX_REGISTERSETTINGS_H__ADADF4EE_1999_4E95_A37D_886007272EBE__INCLUDED_)
#define AFX_REGISTERSETTINGS_H__ADADF4EE_1999_4E95_A37D_886007272EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterSettings.h : header file
//

#include <Label.h>
#include "RegisterSettings_Actions.h"
#include "RegisterSettings_Advanced.h"
#include "StackingTasks.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterSettings dialog

class CRegisterSettings : public CDialog
{
// Construction
public:
	CRegisterSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterSettings)
	enum { IDD = IDD_REGISTERSETTINGS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected :
	CRegisterSettings_Actions		m_tabActions;
	CRegisterSettings_Advanced		m_tabAdvanced;
	CPropertySheet					m_Sheet;
	CStatic							m_Rect;
	CButton							m_OK;

	BOOL							m_bForceRegister;
	bool							m_bStack;
	double							m_fPercentStack;
	BOOL							m_bNoDark;
	BOOL							m_bNoFlat;
	BOOL							m_bNoOffset;
	DWORD							m_dwDetectionThreshold;
	DWORD							m_bMedianFilter;
	CString							m_strFirstLightFrame;
	CAllStackingTasks *				m_pStackingTasks;
	BOOL							m_bSettingsOnly;

	void	UpdateControls();

public :
	BOOL	GetForceRegister()
	{
		return m_bForceRegister;
	};

	void	SetForceRegister(BOOL bForce)
	{
		m_bForceRegister = bForce;
	};

	void		SetStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
	};

	BOOL	IsStackAfter(double & fPercent)
	{
		fPercent	= m_fPercentStack;

		return m_bStack;
	};

	LONG	GetDetectionThreshold()
	{
		return m_dwDetectionThreshold;
	};

	void	SetNoDark(BOOL bNoDark)
	{
		m_bNoDark = bNoDark;
	};

	void	SetNoFlat(BOOL bNoFlat)
	{
		m_bNoFlat = bNoFlat;
	};

	void	SetNoOffset(BOOL bNoOffset)
	{
		m_bNoOffset = bNoOffset;
	};

	void	SetFirstLightFrame(LPCTSTR szFile)
	{
		m_strFirstLightFrame = szFile;
	};

	void	SetSettingsOnly(BOOL bSettingsOnly)
	{
		m_bSettingsOnly = bSettingsOnly;
	};

protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterSettings)
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnStack();
	afx_msg void	OnHotPixels();
	afx_msg void	OnRawddpsettings();
	afx_msg void	OnStackingParameters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRecommandedsettings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTERSETTINGS_H__ADADF4EE_1999_4E95_A37D_886007272EBE__INCLUDED_)
