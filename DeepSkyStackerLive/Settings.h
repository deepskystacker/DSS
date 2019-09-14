#ifndef __SETTINGSTAB_H__
#define __SETTINGSTAB_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include <ControlPos.h>
#include <WndImage.h>
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "ListViewCtrlEx.h"
#include "LiveSettings.h"

// CSettingsTab dialog

class CSettingsTab : public CDialog
{
	DECLARE_DYNAMIC(CSettingsTab)
private :
	CControlPos			m_ControlPos;
	CLiveSettings		m_LiveSettings;
	CString				m_strStackedOutputFolder;
	CString				m_strWarnFileFolder;
	CString				m_strEmail;
	BOOL				m_bDirty;

public:
	CSettingsTab(CWnd* pParent = NULL, bool bDarkMode = false);   // standard constructor
	virtual ~CSettingsTab();


// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnApplyChanges();
	afx_msg void OnCancelChanges();

	afx_msg void OnChangeSetting();

	afx_msg void OnResetEmailCount();

	afx_msg void OnWarningFileFolder( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackedOutputFolder( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnEmailAddress( NMHDR * pNotifyStruct, LRESULT * result );
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private :
	void	UpdateFromRegistry();
	void	SaveToRegistry();
	void	UpdateControls();

private :
	CStatic				m_Stacking;
	CButton				m_DontStack;
	CEdit				m_MinImages;
	CButton				m_Warn_Score;
	CButton				m_Warn_Stars;
	CButton				m_Warn_FWHM;
	CButton				m_Warn_Offset;
	CButton				m_Warn_Angle;
	CButton				m_Warn_SkyBackground;
	CButton				m_DontStack_Score;
	CButton				m_DontStack_Stars;
	CButton				m_DontStack_FWHM;
	CButton				m_DontStack_Offset;
	CButton				m_DontStack_Angle;
	CButton				m_DontStack_SkyBackground;
	CButton				m_MoveNonStackable;

	CEdit				m_Score;
	CEdit				m_Stars;
	CEdit				m_FWHM;
	CEdit				m_Offset;
	CEdit				m_Angle;
	CEdit				m_SkyBackground;

	CStatic				m_Warnings;
	CButton				m_Warn_Sound;
	CButton				m_Warn_Flash;
	CButton				m_Warn_Email;
	CLabel				m_Warn_EmailAddress;
	CButton				m_Warn_File;
	CLabel				m_Warn_FileFolder;
	CButton				m_Warn_ResetEmailCount;

	CStatic				m_Options;
	CButton				m_SaveStackedImage;
	CEdit				m_ImageCount;
	CLabel				m_StackedOutputFolder;

	CButton				m_ApplyChanges;
	CButton				m_CancelChanges;

	CStatic				m_Filters;
	CButton				m_Process_RAW;
	CButton				m_Process_FITS;
	CButton				m_Process_TIFF;
	CButton				m_Process_Others;

	BOOL m_bDarkMode;

public :
	BOOL	Close();
	void	ShowResetEmailCountButton()
	{
		m_Warn_ResetEmailCount.ShowWindow(SW_SHOW);
	};
};


#endif