#pragma once


// CStackingParameters dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include <RichToolTipCtrl.h>
#include "StackingTasks.h"

class CStackingParameters : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CStackingParameters)

public:
	CStackingParameters();
	virtual ~CStackingParameters();

// Dialog Data
	enum { IDD = IDD_STACKINGPARAMETERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		m_Tooltips.RelayEvent(pMsg);

		return 0;
	};


public:
	virtual BOOL OnSetActive();
	void	SetControls(MULTIBITMAPPROCESSMETHOD Method, double fKappa, LONG lIteration);
	void	GetControls(MULTIBITMAPPROCESSMETHOD & Method, double & fKappa, LONG & lIteration);

	void	SetBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE Mode);

	DECLARE_MESSAGE_MAP()

private :
	bool				m_bFirstActivation;
	CRichToolTipCtrl	m_Tooltips;

	void				UpdateControls();
	void				UpdateCalibrationMode();

	afx_msg void OnBnClickedAverage();
	afx_msg void OnBnClickedMedian();
	afx_msg void OnBnClickedMaximum();
	afx_msg void OnBnClickedSigmaclipping();
	afx_msg void OnBnClickedMedianSigmaclipping();
	afx_msg void OnEnChangeKappa();
	afx_msg void OnEnChangeIteration();
	afx_msg void OnBnClickedEntropyaverage();
	afx_msg void OnBnClickedAutoadaptiveaverage();
	afx_msg void OnBnClickedUseDarkFactor();
	afx_msg void OnBnClickedDarkOptimization();
	afx_msg void OnBnClickedDebloom();
	afx_msg void OnBnClickedDebloomSettings();
	afx_msg void OnEnChangeDarkFactor();
	afx_msg void OnBackgroundCalibration( NMHDR * pNotifyStruct, LRESULT * result );

public :
	CButton				m_Average;
	CButton				m_Median;
	CButton				m_Maximum;
	CButton				m_SigmaClipping;
	CButton				m_MedianSigmaClipping;
	CButton				m_EntropyAverage;
	CButton				m_WeightedAverage;
	CEdit				m_Kappa;
	CEdit				m_Iteration;
	CStatic				m_KappaStatic;
	CStatic				m_IterationStatic;
	CLabel				m_Title;
	CStatic				m_KappaFrame;
	CStatic				m_WeightedFrame;
	CLabel				m_BackgroundCalibration;
	CButton				m_DarkOptimization;
	CButton				m_HotPixels;
	CButton				m_BadColumns;
	CButton				m_UseDarkFactor;
	CEdit				m_DarkFactor;
	CButton				m_Debloom;
//	CButton				m_DebloomSettings;

	BACKGROUNDCALIBRATIONMODE	m_BackgroundCalibrationMode;
};
