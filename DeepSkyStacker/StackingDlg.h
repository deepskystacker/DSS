#pragma once
// StackingDlg.h : header file
//

#include "mrupath.h"
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
#include "imageloader.h"

#include "GradientCtrl.h"
#include "SplitterControl.h"

#include "ImageSinks.h"

class QNetworkAccessManager;
class QNetworkReply;

#include <QDialog>
#include <QFileDialog>
#include <QStyledItemDelegate>

namespace DSS
{
	class Group;
	class EditStars;
	class SelectRect;
	class ToolBar;
}

namespace Ui
{
	class StackingDlg;
}

namespace std::filesystem
{
	class path;
}

class QTextOption;

namespace fs = std::filesystem;

namespace DSS
{

	class IconSizeDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		using QStyledItemDelegate::QStyledItemDelegate;


	protected:
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		//inline QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
		//{
		//	QSize result = QStyledItemDelegate::sizeHint(option, index);
		//	result.setHeight(result.height() * 1.2);
		//	return result;
		//}

		QString calculateElidedText(const QString& text, const QTextOption& textOption,
			const QFont& font, const QRect& textRect, const Qt::Alignment valign,
			Qt::TextElideMode textElideMode, int flags,
			bool lastVisibleLineShouldBeElided, QPointF* paintStartPosition) const;

	};

	class StackingDlg : public QWidget
	{
		typedef QWidget
			Inherited;

		Q_OBJECT

	public slots:
		void setSelectionRect(QRectF rect);
		void tableViewItemClickedEvent(const QModelIndex&);
		void imageLoad();
		
		void toolBar_rectButtonPressed(bool checked);
		void toolBar_starsButtonPressed(bool checked);
		void toolBar_cometButtonPressed(bool checked);
		void toolBar_saveButtonPressed(bool checked);

	public:
		explicit StackingDlg(QWidget* parent = nullptr);
		~StackingDlg();

		void		onAddPictures();
		void		onAddDarks();
		void		onAddDarkFlats();
		void		onAddFlats();
		void		onAddOffsets();

		//
		// dssfilelist operations
		//
		void		clearList();
		void		loadList();
		void		saveList();

		//
		// Check marks
		//
		void		checkAbove();
		void		checkAll();
		void		unCheckAll();

		void setFileList(const fs::path& file)
		{
			fileList = file;
		};

		void computeOffsets();

		void registerCheckedImages();

		void stackCheckedImages();

		void batchStack();

		inline void fillTasks(CAllStackingTasks& tasks)
		{
			return frameList.fillTasks(tasks);
		}

		inline size_t checkedImageCount(PICTURETYPE t)
		{
			return frameList.checkedImageCount(t);
		}

		inline QString getFirstCheckedLightFrame()
		{
			return frameList.getFirstCheckedLightFrame();
		}

		inline size_t unregisteredCheckedLightFrameCount(int id = -1) const
		{
			return frameList.unregisteredCheckedLightFrameCount(id);
		}

		void reloadCurrentImage();

		void pictureChanged();


	private:
		Ui::StackingDlg* ui;
		std::unique_ptr<Workspace> workspace;
		bool initialised;
		QString			m_strShowFile;
		CGammaTransformation	m_GammaTransformation;
		fs::path		fileList;
		FrameList		frameList;
		CMRUList		m_MRUList;
		std::unique_ptr<IconSizeDelegate> iconSizeDelegate;



		bool fileAlreadyLoaded(const fs::path& file);

		std::unique_ptr<EditStars> editStarsPtr;
		std::unique_ptr<SelectRect> selectRectPtr;
		std::unique_ptr<ToolBar> pToolBar;

		QRectF	selectRect;

		//QFileDialog			fileDialog;

		MRUPath			mruPath;
		QNetworkAccessManager* networkManager;			// deleted using QObject::deleteLater();

		ImageLoader		imageLoader;
		LoadedImage		m_LoadedImage;


		void showEvent(QShowEvent* event) override;

		void onInitDialog();

		void versionInfoReceived(QNetworkReply* reply);
		void retrieveLatestVersionInfo();

		bool checkEditChanges();

		bool checkReadOnlyFolders(CAllStackingTasks& tasks);

		bool CheckStacking(CAllStackingTasks& tasks);

		bool showRecap(CAllStackingTasks& tasks);

		bool saveOnClose();

		void DoStacking(CAllStackingTasks& tasks, const double fPercent = 100.0);

		void UpdateCheckedAndOffsets(CStackingEngine& StackingEngine);
		
		bool checkWorkspaceChanges();
		
		void openFileList(const fs::path& file);

		void updateListInfo();

		void loadList(CMRUList& MRUList, QString& strFileList);

		void saveList(CMRUList& MRUList, QString& strFileList);

	};
}
#if (0)
/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg dialog

class CStackingDlg : public CDialog,
					 public CButtonToolbarSink
{
private :
	CSplitterControl		m_Splitter;
	CButtonToolbar			m_ButtonToolbar;
	CSelectRectSink			m_SelectRectSink;
	CEditStarsSink			m_EditStarSink;
	CString					m_strStartingFileList;
	CGammaTransformation	m_GammaTransformation;
	CString					m_strCurrentFileList;
	//QFileDialog			    fileDialog;
	QNetworkAccessManager* networkManager;			// deleted using QObject::deleteLater();


	// These values dictate how resizing should work.
	static const int sm_nMinListWidth = 500;
	static const int sm_nMinListHeight = 120;
	static const int sm_nMinImageHeight = 200;

	CCtrlCache m_cCtrlCache;

// Construction
public:
	CStackingDlg(CWnd* pParent = nullptr);   // standard constructor

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

	void		CheckAskRegister();
	void		RegisterCheckedImage();
	void		CheckAll();
	void		UncheckAll();
	void		ClearList();
	void		CheckAbove();

	void		LoadList();
	void		SaveList();
	void		ShowStars(BOOL bShow);
	void		ReloadCurrentImage();
	void		DropFiles(HDROP hDropInfo);

	void		OpenFileList(LPCTSTR szFileList);

private :
	void		UncheckNonStackablePictures();
	void		UpdateCheckedAndOffsets(CStackingEngine & StackingEngine);

	void		UpdateGroupTabs();
	void		UpdateLayout();
	void versionInfoReceived(QNetworkReply* reply);
	void retrieveLatestVersionInfo();


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
#endif
