#pragma once

#include "BitmapExt.h"

// CRegisterSettings_Advanced dialog

class CRegisterSettings_Advanced : public CPropertyPage
{
private :
	BOOL				m_bFirstActivation;

	DECLARE_DYNAMIC(CRegisterSettings_Advanced)

public:
	CRegisterSettings_Advanced();
	virtual ~CRegisterSettings_Advanced();

	virtual BOOL OnSetActive();
	void	UpdateSliderText();

// Dialog Data
	enum { IDD = IDD_REGISTERSETTINGS_ADVANCED };
	CStatic						m_PercentText;
	CSliderCtrl					m_PercentSlider;
	CButton						m_MedianFilter;
	CStatic						m_NrStars;
	CString						m_strMask;
	CString						m_strFirstLightFrame;
	CSmartPtr<CMemoryBitmap>	m_pFirstLightFrame;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnComputeStars();

	DECLARE_MESSAGE_MAP()
};
