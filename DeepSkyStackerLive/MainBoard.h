#ifndef __MAINBOARD_H__
#define __MAINBOARD_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include "LiveEngine.h"
#include "LiveSettings.h"
#include <ControlPos.h>


// CMainBoard dialog

class CMainBoard : public CDialog
{
	DECLARE_DYNAMIC(CMainBoard)

public:
	CMainBoard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainBoard();

	//void	CallHelp();

// Dialog Data
	enum { IDD = IDD_MAINBOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	void	DrawSubFrameRect(CDC * pDC, const CRect & rc);
	void	DrawGradientRect(CDC * pDC, const CRect & rc, COLORREF crColor1, COLORREF crColor2, double fAlpha = 0.0);
	void	DrawGradientBackgroundRect(CDC * pDC, const CRect & rc);
	void	DrawGradientFrameRect(CDC * pDC, LPCTSTR szTitle, const CRect & rc, BOOL bActive, BOOL bShadow);
	void	DrawTab(CDC * pDC, LPCTSTR szText, const CRect & rc, BOOL bActive);
	void	DrawMonitorButton(CDC * pDC);
	void	DrawStackButton(CDC * pDC);
	void	DrawStopButton(CDC * pDC);
	void	DrawProgress(CDC * pDC);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnMonitoredFolder( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnMonitor();
	afx_msg void OnStack();
	afx_msg void OnStop();
	afx_msg LRESULT OnFolderChange(WPARAM, LPARAM);
	afx_msg LRESULT OnLiveEngine(WPARAM, LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

//	afx_msg void OnAbout( NMHDR * pNotifyStruct, LRESULT * result );
//	afx_msg void OnHelp( NMHDR * pNotifyStruct, LRESULT * result );
private :
	CControlPos				m_ControlPos;
	CStatic					m_MonitoringRect;
	CLabel					m_MonitoredFolder;
	CStatic					m_ProgressRect;

	CStatic					m_StackedImage;
	CStatic					m_LastImage;
	CStatic					m_Graphs;
	CStatic					m_ImageList;
	CStatic					m_Log;
	CStatic					m_Warnings;
	CStatic					m_Stats;

	ULONG					m_ulSHRegister;
	std::vector<CString>	m_vAllFiles;

	CLiveEngine				m_LiveEngine;

	BOOL					m_bProgressing;
	CString					m_strProgress;
	LONG					m_lProgressAchieved;
	LONG					m_lProgressTotal;
	CString					m_strStatsMask;

	CButton					m_Monitor;
	CButton					m_Stack;
	CButton					m_Stop;

	BOOL					m_bMonitoring;
	BOOL					m_bStacking;
	BOOL					m_bStopping;
	LONG					m_lNrPending;
	LONG					m_lNrRegistered;
	LONG					m_lNrStacked;
	double					m_fTotalExposureTime;
	LONG					m_lNrEmails;

	CLiveSettings			m_LiveSettings;

private :
	BOOL	IsMonitoredFolderOk();
	BOOL	ChangeMonitoredFolder();
	BOOL	CheckRestartMonitoring();
	void	GetNewFilesInMonitoredFolder(std::vector<CString> & vFiles);
	void	InvalidateProgress();
	void	InvalidateButtons();
	void	InvalidateStats();

public :
	void	UpdateLiveSettings()
	{
		m_LiveSettings.LoadFromRegistry();
		m_LiveEngine.UpdateSettings();
	};

	void	ResetEmailCount()
	{
		m_lNrEmails = 0;
	};

	void	PostSaveStackedImage()
	{
		m_LiveEngine.PostSaveStackedImage();
	};
};


#endif