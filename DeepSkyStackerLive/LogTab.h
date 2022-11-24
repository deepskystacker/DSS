#ifndef __LOGTAB_H__
#define __LOGTAB_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include <ControlPos.h>


// CLogTab dialog

class CLogTab : public CDialog
{
	DECLARE_DYNAMIC(CLogTab)
private :
	CControlPos			m_ControlPos;
	CRichEditCtrl		m_Log;
	bool				m_bDarkMode;

public:
	CLogTab(CWnd* pParent = nullptr, bool bDarkMode = false);   // standard constructor
	virtual ~CLogTab();

	//void	CallHelp();

// Dialog Data
	enum { IDD = IDD_LOG };

public :
	void	AddToLog(QString szText, BOOL bAddDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0));

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

//	afx_msg void OnAbout( NMHDR * pNotifyStruct, LRESULT * result );
//	afx_msg void OnHelp( NMHDR * pNotifyStruct, LRESULT * result );

public :

};

#endif