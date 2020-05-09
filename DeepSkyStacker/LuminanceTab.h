#if !defined(AFX_LUMINANCETAB_H__C143C1EE_81D8_44F1_B5FD_B66EA4028433__INCLUDED_)
#define AFX_LUMINANCETAB_H__C143C1EE_81D8_44F1_B5FD_B66EA4028433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LuminanceTab.h : header file
//

#include <ChildProp.h>
#include <BtnST.h>
#include <Label.h>
#include "afxwin.h"

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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUMINANCETAB_H__C143C1EE_81D8_44F1_B5FD_B66EA4028433__INCLUDED_)
