#if !defined(AFX_RGBTAB_H__82B3FFF2_8DA4_4979_BCC7_09D28857619B__INCLUDED_)
#define AFX_RGBTAB_H__82B3FFF2_8DA4_4979_BCC7_09D28857619B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RGBTab.h : header file
//

#include <GradientCtrl.h>
#include <ChildProp.h>
#include <BtnST.h>
#include <cdxCRotBevelLine.h>
#include "Histogram.h"

/////////////////////////////////////////////////////////////////////////////
// CRGBTab dialog

class CRGBTab : public CChildPropertyPage
{
	DECLARE_DYNCREATE(CRGBTab)

private :
	BOOL				m_bFirstActivation;
	HISTOADJUSTTYPE		m_RedAdjustMethod;
	HISTOADJUSTTYPE		m_GreenAdjustMethod;
	HISTOADJUSTTYPE		m_BlueAdjustMethod;


	int	GetHATBitmap(HISTOADJUSTTYPE hat)
	{
		switch (hat)
		{
		case HAT_CUBEROOT :
			return IDB_CUBEROOT;
			break;
		case HAT_SQUAREROOT :
			return IDB_SQRT;
			break;
		case HAT_LOG :
			return IDB_LOG;
			break;
		case HAT_LOGLOG :
			return IDB_LOGLOG;
			break;
		case HAT_LOGSQUAREROOT :
			return IDB_LOGSQRT;
			break;
		case HAT_ASINH :
			return IDB_ASINH;
			break;
		default :
		case HAT_LINEAR	:
			return IDB_LINEAR;
			break;
		};
	};

// Construction
public:
	CRGBTab();
	~CRGBTab();

// Dialog Data
	//{{AFX_DATA(CRGBTab)
	enum { IDD = IDD_RGB };
	CButtonST			m_RedHAT;
	CButtonST			m_GreenHAT;
	CButtonST			m_BlueHAT;
	CButtonST			m_Settings;
	CButtonST			m_Redo;
	CButtonST			m_Undo;
	CButton				m_LinkSettings;
	//}}AFX_DATA
	CGradientCtrl		m_RedGradient;
	CGradientCtrl		m_GreenGradient;
	CGradientCtrl		m_BlueGradient;

	HISTOADJUSTTYPE	GetRedAdjustMethod()
	{
		return m_RedAdjustMethod;
	};

	HISTOADJUSTTYPE	GetGreenAdjustMethod()
	{
		return m_GreenAdjustMethod;
	};

	HISTOADJUSTTYPE	GetBlueAdjustMethod()
	{
		return m_BlueAdjustMethod;
	};

	void	SetRedAdjustMethod(HISTOADJUSTTYPE hat)
	{
		m_RedAdjustMethod = hat;
		m_RedHAT.SetBitmaps(GetHATBitmap(hat), RGB(255, 0, 255), (INT)BTNST_AUTO_GRAY);
		m_RedHAT.Invalidate(TRUE);
	};

	void	SetGreenAdjustMethod(HISTOADJUSTTYPE hat)
	{
		m_GreenAdjustMethod = hat;
		m_GreenHAT.SetBitmaps(GetHATBitmap(hat), RGB(255, 0, 255), (INT)BTNST_AUTO_GRAY);
		m_GreenHAT.Invalidate(TRUE);
	};

	void	SetBlueAdjustMethod(HISTOADJUSTTYPE hat)
	{
		m_BlueAdjustMethod = hat;
		m_BlueHAT.SetBitmaps(GetHATBitmap(hat), RGB(255, 0, 255), (INT)BTNST_AUTO_GRAY);
		m_BlueHAT.Invalidate(TRUE);
	};

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRGBTab)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRGBTab)
	afx_msg void OnReset();
	afx_msg void OnProcess();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnSettings();
	afx_msg void OnRedHat();
	afx_msg void OnBlueHat();
	afx_msg void OnGreenHat();
	//}}AFX_MSG
	afx_msg void OnNotifyRedChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyRedPegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyRedPegMoved(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenPegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyGreenPegMoved(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBlueChangeSelPeg(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBluePegMove(NMHDR * pNotifyStruct, LRESULT *result);
	afx_msg void OnNotifyBluePegMoved(NMHDR * pNotifyStruct, LRESULT *result);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RGBTAB_H__82B3FFF2_8DA4_4979_BCC7_09D28857619B__INCLUDED_)
