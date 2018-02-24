#pragma once


// COutputTab dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include "StackingTasks.h"
#include "afxwin.h"


class COutputTab : public CChildPropertyPage
{
	DECLARE_DYNAMIC(COutputTab)

public:
	COutputTab();
	virtual ~COutputTab();

	void	SetOutputSettings(const COutputSettings & os)
	{
		m_OutputSettings = os;
	};

	void	GetOutputSettings(COutputSettings & os)
	{
		os = m_OutputSettings;
	};

// Dialog Data
	enum { IDD = IDD_OUTPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()

private :
	BOOL				m_bFirstActivation;

	void				UpdateControls();

public :
	CLabel						m_Title;
	COutputSettings				m_OutputSettings;
	CString						m_strNoFolder;

	afx_msg void OnBnClickedAutosave();
	afx_msg void OnBnClickedFilelist();
	afx_msg void OnBnClickedUsereferenceframefolder();
	afx_msg void OnBnClickedUsefilelistfolder();
	afx_msg void OnBnClickedUseanotherfolder();
	afx_msg void OnBnClickedCreatehtml();
	afx_msg void OnBnClickedCreateoutput();
	afx_msg void OnBnClickedAppendnumber();
	afx_msg void OnOutputFolder( NMHDR * pNotifyStruct, LRESULT * result );

	CButton m_OutputFile;
	CButton m_OutputHTML;
	CButton m_Autosave;
	CButton m_FileListName;
	CButton m_RefFrameFolder;
	CButton m_FileListFolder;
	CButton m_OtherFolder;
	CButton m_AppendNumber;
	CLabel  m_OutputFolder;
};
