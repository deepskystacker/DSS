#if !defined(AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_)
#define AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StackingDlg.h : header file
//

class QNetworkAccessManager;
class QNetworkReply;
class QTreeWidgetItem;

#include <QWidget>
#include <QString>
#include <QToolBar>

#include <CtrlCache.h>
#include <WndImage.h>
#include <BtnST.h>
#include "PictureListCtrl.h"
#include "Label.h"
#include "StackingTasks.h"
#include "DeepStack.h"
//#include <BitmapSlider.h>
#include <CustomTabCtrl.h>
#include "StackingEngine.h"
#include "BackgroundLoading.h"
#include "QLinearGradientCtrl.h"
#include "SplitterControl.h"

#include "ImageSinks.h"

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// StackingDlg dialog

namespace Ui {
	class StackingDlg;
}

class StackingDlg : public QWidget,
					 public CButtonToolbarSink
{
	Q_OBJECT

typedef QWidget
	Inherited;

private :
	Ui::StackingDlg* ui;
	CSplitterControl		m_Splitter;
	QString					m_strShowFile;
	QToolBar				m_ButtonToolbar;
	// CSelectRectSink			m_SelectRectSink;
	// CEditStarsSink			m_EditStarSink;
	CMRUList				m_MRUList;
	QString					m_strStartingFileList;
	CBackgroundLoading		m_BackgroundLoading;
	CLoadedImage			m_LoadedImage;
	CGammaTransformation	m_GammaTransformation;
	QString					m_strCurrentFileList;
	QNetworkAccessManager *   networkManager;			// deleted using QObject::deleteLater();

	// These values dictate how resizing should work.
	static const int sm_nMinListWidth = 500;
	static const int sm_nMinListHeight = 120;
	static const int sm_nMinImageHeight = 200;

	//CCtrlCache m_cCtrlCache;

// Construction
public:
	explicit StackingDlg(QWidget * parent = nullptr);   // standard constructor

	bool	SaveOnClose();

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
	enum { IDD = IDD_STACKING };
	CPictureListCtrl	m_Pictures;
	CStatic				m_PictureStatic;
	CLabel				m_Infos;
	CLabel				m_ListInfo;
	//CBitmapSlider		m_Gamma;
	QLinearGradientCtrl		m_Gamma;
	CCustomTabCtrl		m_GroupTab;
	// CCustomTabCtrl		m_JobTab;
	CLabel				m_ShowHideJobs;
	// CButtonST			m_4Corners;
	//}}AFX_DATA
	CWndImage			m_Picture;

private :
	//void		Autosave();
	void		UpdateListInfo();

public :
	bool		CheckDiskSpace(CAllStackingTasks & tasks);
	bool		CheckWorkspaceChanges();
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
	void		ShowStars(bool bShow);
	bool		ShowRecap(CAllStackingTasks & tasks);
	bool		CheckStacking(CAllStackingTasks & tasks);
	bool		CheckReadOnlyFolders(CAllStackingTasks & tasks);
	void		ReloadCurrentImage();
	void		DropFiles(HDROP hDropInfo);
	void		SetStartingFileList(LPCTSTR szFileList)
	{
		m_strStartingFileList = QString::fromWCharArray((wchar_t *)szFileList);
	};
	void		OpenFileList(LPCTSTR szFileList);

	void		FillTasks(CAllStackingTasks & tasks);

private:
	void		UncheckNonStackablePictures();
	void		UpdateCheckedAndOffsets(CStackingEngine & StackingEngine);
	void		DoStacking(CAllStackingTasks & tasks, double fPercent = 100.0);

	void		UpdateGroupTabs();
	bool		CheckEditChanges();
	void		UpdateLayout();
	void		versionInfoReceived(QNetworkReply * reply);
	void		retrieveLatestVersionInfo();

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

private:
	std::unique_ptr<CWorkspace> workspace;
	bool	initialised;

	void showEvent(QShowEvent* event) override;
	void onInitDialog();


public :
	CWndImageSink *	GetCurrentSink();
	virtual void ButtonToolbar_OnCheck(DWORD dwID, CButtonToolbar * pButtonToolbar);
	virtual void ButtonToolbar_OnClick(DWORD dwID, CButtonToolbar * pButtonToolbar);
	virtual void ButtonToolbar_OnRClick(DWORD dwID, CButtonToolbar * pButtonToolbar);
	afx_msg void OnBnClicked4corners();

private slots:
	void on_pictures_itemClicked(QTreeWidgetItem* item, int column);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/* ------------------------------------------------------------------- */

#endif // !defined(AFX_STACKINGDLG_H__04779310_B6E7_4523_BB0C_90ACAC6C1522__INCLUDED_)
