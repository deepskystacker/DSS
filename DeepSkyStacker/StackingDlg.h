#if !defined(AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_)
#define AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StackingDlg.h : header file
//

#include <CtrlCache.h>
#include <WndImage.h>
#include <BtnST.h>
#include "PictureListCtrl.h"
#include "Label.h"
#include "StackingTasks.h"
#include "DeepStack.h"
#include <BitmapSlider.h>
#include <CustomTabCtrl.h>
#include "StackingEngine.h"
#include "BackgroundLoading.h"
#include "GradientCtrl.h"
#include "SplitterControl.h"

#include "ImageSinks.h"

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg dialog

class CStackingDlg : public CDialog,
					 public CButtonToolbarSink
{
private :
	CSplitterControl		m_Splitter;
	CString					m_strShowFile;
	CButtonToolbar			m_ButtonToolbar;
	CSelectRectSink			m_SelectRectSink;
	CEditStarsSink			m_EditStarSink;
	CMRUList				m_MRUList;
	CString					m_strStartingFileList;
	CBackgroundLoading		m_BackgroundLoading;
	CLoadedImage			m_LoadedImage;
	CGammaTransformation	m_GammaTransformation;
	CString					m_strCurrentFileList;

	// These values dictate how resizing should work.
	static const int sm_nMinListWidth = 500;
	static const int sm_nMinListHeight = 120;
	static const int sm_nMinImageHeight = 200;
	
	CCtrlCache m_cCtrlCache;

// Construction
public:
	CStackingDlg(CWnd* pParent = NULL);   // standard constructor

	BOOL	SaveOnClose();

	void	ClearStackList()
	{
		m_Pictures.Clear();
	};

	void	AddPictureToStackList(LPCTSTR szPicture)
	{
		m_Pictures.AddFile(szPicture, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID());
	};

	void	CheckBests(double fPercent);
	void	StackCheckedImage();
	void	BatchStack();

// Dialog Data
	//{{AFX_DATA(CStackingDlg)
	enum { IDD = IDD_STACKING };
	CPictureListCtrl	m_Pictures;
	CStatic				m_PictureStatic;
	CLabel				m_Infos;
	CLabel				m_ListInfo;
	//CBitmapSlider		m_Gamma;
	CGradientCtrl		m_Gamma;
	CCustomTabCtrl		m_GroupTab;
	CCustomTabCtrl		m_JobTab;
	CLabel				m_ShowHideJobs;
	CButtonST			m_4Corners;
	//}}AFX_DATA
	CWndImage			m_Picture;

private :
	//void		Autosave();
	void		UpdateListInfo();

public :
	BOOL		CheckDiskSpace(CAllStackingTasks & tasks);
	BOOL		CheckWorkspaceChanges();
	void		CheckAskRegister();
	void		RegisterCheckedImage();
	void		CheckAll();
	void		UncheckAll();
	void		ClearList();
	void		CheckAbove();
	void		OnAdddarks();
	void		OnAddDarkFlats();
	void		OnAddFlats();
	void		OnAddOffsets();
	void		OnAddpictures();
	void		ComputeOffsets();
	void		LoadList();
	void		SaveList();
	void		ShowStars(BOOL bShow);
	BOOL		ShowRecap(CAllStackingTasks & tasks);
	BOOL		CheckStacking(CAllStackingTasks & tasks);
	BOOL		CheckReadOnlyFolders(CAllStackingTasks & tasks);
	void		ReloadCurrentImage();
	void		DropFiles(HDROP hDropInfo);
	void		SetStartingFileList(LPCTSTR szFileList)
	{
		m_strStartingFileList = szFileList;
	};
	void		OpenFileList(LPCTSTR szFileList);

	void		FillTasks(CAllStackingTasks & tasks);

private :
	void		UncheckNonStackablePictures();
	void		UpdateCheckedAndOffsets(CStackingEngine & StackingEngine);
	void		DoStacking(CAllStackingTasks & tasks, double fPercent = 100.0);

	void		UpdateGroupTabs();
	BOOL		CheckEditChanges();
	void		UpdateLayout();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStackingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStackingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickPictures(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPictureChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	void OnSelChangeGroup(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	void OnSelChangeJob(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCheckItem(WPARAM, LPARAM);
	afx_msg LRESULT OnSelectItem(WPARAM, LPARAM);
	afx_msg	void OnCheckPicture();
	afx_msg void OnChangeGamma(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSplitter(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnBackgroundImageLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowHideJobs( NMHDR * pNotifyStruct, LRESULT * result );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public :
	CWndImageSink *	GetCurrentSink();
	virtual void ButtonToolbar_OnCheck(DWORD dwID, CButtonToolbar * pButtonToolbar);
	virtual void ButtonToolbar_OnClick(DWORD dwID, CButtonToolbar * pButtonToolbar);
	virtual void ButtonToolbar_OnRClick(DWORD dwID, CButtonToolbar * pButtonToolbar);
	afx_msg void OnBnClicked4corners();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/* ------------------------------------------------------------------- */

#endif // !defined(AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_)
