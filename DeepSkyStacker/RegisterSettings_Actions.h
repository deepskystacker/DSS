#pragma once

#include <Label.h>

// CRegisterSettings_Actions dialog

class CRegisterSettings_Actions : public CPropertyPage
{
private :
	BOOL				m_bFirstActivation;

	DECLARE_DYNAMIC(CRegisterSettings_Actions)

public:
	CRegisterSettings_Actions();
	virtual ~CRegisterSettings_Actions();

	virtual BOOL OnSetActive();

// Dialog Data
	enum { IDD = IDD_REGISTERSETTINGS_BASIC };
	CButton		m_Stack;
	CEdit		m_Percent;
	CButton		m_ForceRegister;
	CButton		m_HotPixels;
	CLabel		m_StackWarning;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void	OnStack();
	afx_msg void	OnHotPixels();
	afx_msg void	OnRawddpsettings();

	DECLARE_MESSAGE_MAP()
};
