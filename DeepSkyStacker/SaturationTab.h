#if !defined(AFX_SATURATIONTAB_H__947342A1_F290_4BAF_AA71_4D5432E47E65__INCLUDED_)
#define AFX_SATURATIONTAB_H__947342A1_F290_4BAF_AA71_4D5432E47E65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaturationTab.h : header file
//

#include <ChildProp.h>
#include <BtnST.h>
#include "afxwin.h"
#include <Label.h>

/////////////////////////////////////////////////////////////////////////////
// CSaturationTab dialog

class CSaturationTab : public CChildPropertyPage
{
	DECLARE_DYNCREATE(CSaturationTab)
private :
	BOOL				m_bFirstActivation ;

// Construction
public:
	CSaturationTab();
	~CSaturationTab();

// Dialog Data
	//{{AFX_DATA(CSaturationTab)
	enum { IDD = IDD_SATURATION };
	CButtonST	m_Settings;
	CButtonST	m_Undo;
	CButtonST	m_Redo;
	CSliderCtrl	m_Saturation;
	CLabel		m_ST_Saturation;	
	CLabel		m_SaturationTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSaturationTab)
	public:
	virtual BOOL OnSetActive();
	void	UpdateTexts();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSaturationTab)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnProcess();
	afx_msg void OnRedo();
	afx_msg void OnReset();
	afx_msg void OnSettings();
	afx_msg void OnUndo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SATURATIONTAB_H__947342A1_F290_4BAF_AA71_4D5432E47E65__INCLUDED_)
