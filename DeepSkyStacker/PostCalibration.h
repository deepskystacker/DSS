#pragma once


// CPostCalibration dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include "StackingTasks.h"


class CPostCalibration : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CPostCalibration)

public:
	CPostCalibration();
	virtual ~CPostCalibration();

// Dialog Data
	enum { IDD = IDD_POSTCALIBRATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTest( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnCosmeticMethod( NMHDR * pNotifyStruct, LRESULT * result );

public:
	virtual BOOL OnSetActive();

	void	SetPostCalibration(const CPostCalibrationSettings & Settings)
	{
		m_Settings	= Settings;
	};

	void	GetPostCalibration(CPostCalibrationSettings & Settings)
	{
		Settings	= m_Settings;
	};

	void	SetStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
	};


	DECLARE_MESSAGE_MAP()

private :
	bool				m_bFirstActivation;

	void				UpdateSettingsTexts();
	void				UpdateControlsFromSettings();
	void				UpdateSettingsFromControls();
	void				UpdateControls();

	afx_msg void OnBnClickedDetectCleanHotCold();

public :
	CLabel				m_Title;
	CButton				m_DetectCleanHot;
	CStatic				m_HotFilterText;
	CSliderCtrl			m_HotFilter;
	CStatic				m_HotDetectionText;
	CSliderCtrl			m_HotDetection;

	CButton				m_DetectCleanCold;
	CStatic				m_ColdFilterText;
	CSliderCtrl			m_ColdFilter;
	CStatic				m_ColdDetectionText;
	CSliderCtrl			m_ColdDetection;

	CButton				m_SaveDelta;

	CString				m_strPixelMask;
	CString				m_strPercentMask;

	CLabel				m_Strong1;
	CLabel				m_Strong2;
	CLabel				m_Weak1;
	CLabel				m_Weak2;

	CLabel				m_ReplaceText;
	CLabel				m_ReplaceMethod;
	CLabel				m_Test;

	CAllStackingTasks *			m_pStackingTasks;
	CPostCalibrationSettings	m_Settings;
};
