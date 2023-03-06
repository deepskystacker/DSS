#pragma once
#include "childprop.h"
#include "BtnST.h"
#include "Label.h"
#include "resourceCZ.h"

/////////////////////////////////////////////////////////////////////////////
// CLuminanceTab dialog

class CLuminanceTab : public CChildPropertyPage //CChildPropertyPage
{
	DECLARE_DYNCREATE(CLuminanceTab)
private :
	bool				m_bFirstActivation;

// Construction
public:
	CLuminanceTab();
	~CLuminanceTab();

// Dialog Data
	//{{AFX_DATA(CLuminanceTab)
	enum { IDD = IDD_LUMINANCE };
	CButtonST	m_Settings;
	CButtonST	m_Undo;
	CButtonST	m_Redo;
	CSliderCtrl	m_MidTone;
	CSliderCtrl	m_MidAngle;
	CSliderCtrl	m_HighPower;
	CSliderCtrl	m_HighAngle;
	CSliderCtrl	m_DarkPower;
	CSliderCtrl	m_DarkAngle;
	CLabel		m_ST_Darkness;
	CLabel		m_ST_Midtone;
	CLabel		m_ST_Highlight;
	CLabel		m_DarknessTitle;
	CLabel		m_MidtoneTitle;
	CLabel		m_HighlightTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLuminanceTab)
	public:
	virtual BOOL OnSetActive();
	void	UpdateTexts();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLuminanceTab)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnProcess();
	afx_msg void OnRedo();
	afx_msg void OnUndo();
	afx_msg void OnReset();
	afx_msg void OnSettings();
	afx_msg void OnLocalcontrast();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
