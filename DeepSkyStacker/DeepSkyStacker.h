
#pragma once
#include <list>
#include <QMainWindow>
#include "commonresource.h"
#include "DeepStack.h"


#include "ExplorerBar.h"
class QSplitter;
class QStackedWidget;
#include "StackingDlg.h"
class QWinHost;

#include "ProcessingDlg.h"
#include "dss_settings.h"

class DeepSkyStacker :
	public QMainWindow
{
	typedef QMainWindow
		Inherited;

	Q_OBJECT

private:
	bool initialised;
	QWidget* widget;
	QSplitter* splitter;
	ExplorerBar* explorerBar;
	QStackedWidget* stackedWidget;
	DSS::StackingDlg* stackingDlg;
	QWinHost* winHost;

	CProcessingDlg	processingDlg;

	CDeepStack				m_DeepStack;
	CDSSSettings			m_Settings;
	std::uint32_t			currTab;
	QStringList				args;
	QString					baseTitle;
	//ITaskbarList3* m_taskbarList;
	bool                    m_progress;

	void showEvent(QShowEvent* event) override;

	void onInitialise();

public:


	inline static DeepSkyStacker* instance()
	{
		return theMainWindow;
	}

	inline static void setInstance(DeepSkyStacker* instance)
	{
		ZASSERT(nullptr == theMainWindow);
		theMainWindow = instance;
	}

	DeepSkyStacker();

	~DeepSkyStacker()
	{
	};

	CDeepStack& deepStack()
	{
		return m_DeepStack;
	};

	void	setTab(std::uint32_t dwTabID)
	{
		if (dwTabID == IDD_REGISTERING)
			dwTabID = IDD_STACKING;
		//#ifdef DSSBETA
		//	if (dwTabID == IDD_STACKING && 	(GetAsyncKeyState(VK_CONTROL) & 0x8000))
		//		dwTabID = IDD_LIBRARY;
		//#endif
		currTab = dwTabID;
		updateTab();
	};

	void setTitleFilename(fs::path file);

	inline void setTitleFilename(char* name)
	{
		setTitleFilename(fs::path(name));
	}

	std::uint32_t tab()
	{
		return currTab;
	};

	inline void disableSubDialogs()
	{
		stackingDlg->setEnabled(false);
		processingDlg.EnableWindow(false);
		//m_dlgLibrary.EnableWindow(false);
		explorerBar->setEnabled(false);
	};

	inline void enableSubDialogs()
	{
		stackingDlg->setEnabled(true);
		processingDlg.EnableWindow(true);
		//m_dlgLibrary.EnableWindow(true);
		explorerBar->setEnabled(true);
	};

	CDSSSettings& settings()
	{
		if (!m_Settings.IsLoaded())
			m_Settings.Load();

		return m_Settings;
	};

	DSS::StackingDlg& getStackingDlg()
	{
		return *stackingDlg;
	};

	CProcessingDlg& getProcessingDlg()
	{
		return processingDlg;
	};


	ExplorerBar& GetExplorerBar()
	{
		return *explorerBar;
	};

protected:
	void closeEvent(QCloseEvent* e) override;

private:
	void updateTab();
	static inline DeepSkyStacker* theMainWindow{ nullptr };

};


class DeepSkyStackerApp : public CWinApp
{
public :
	CWnd *				m_pMainDlg;

public :
	DeepSkyStackerApp() :
		m_pMainDlg{ nullptr }
	{
        
	};

	virtual ~DeepSkyStackerApp()
	{

	};

	virtual BOOL InitInstance( ) override;
	virtual int ExitInstance() override;
	virtual int Run() override;

};

//
// Temporarily left here while still have to position MFC windows
//
void	SaveWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath);
void	RestoreWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath, bool bCenter = false);

