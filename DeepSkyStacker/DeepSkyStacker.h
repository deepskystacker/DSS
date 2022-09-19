
#pragma once
#include <list>
#include <QMainWindow>
#include "DeepStack.h"

class CDSSSetting
{
public:
	CString				m_strName;
	CBezierAdjust		m_BezierAdjust;
	CRGBHistogramAdjust	m_HistoAdjust;

private:
	void	CopyFrom(const CDSSSetting& cds)
	{
		m_strName = cds.m_strName;
		m_BezierAdjust = cds.m_BezierAdjust;
		m_HistoAdjust = cds.m_HistoAdjust;
	};

public:
	CDSSSetting() {};
	virtual ~CDSSSetting() {};

	CDSSSetting(const CDSSSetting& cds)
	{
		CopyFrom(cds);
	};

	CDSSSetting& operator = (const CDSSSetting& cds)
	{
		CopyFrom(cds);
		return (*this);
	};

	bool operator < (const CDSSSetting& cds) const
	{
		int			nCompare;
		nCompare = m_strName.CompareNoCase(cds.m_strName);

		if (nCompare < 0)
			return true;
		else
			return false;
	};

	bool	Load(FILE* hFile)
	{
		int		lNameSize;
		TCHAR	szName[2000] = { _T('\0') };

		fread(&lNameSize, sizeof(lNameSize), 1, hFile);
		fread(szName, sizeof(TCHAR), lNameSize, hFile);
		m_strName = szName;
		return m_BezierAdjust.Load(hFile) && m_HistoAdjust.Load(hFile);
	};

	bool	Save(FILE* hFile)
	{
		int lNameSize = m_strName.GetLength() + 1;
		fwrite(&lNameSize, sizeof(lNameSize), 1, hFile);
		fwrite((LPCTSTR)m_strName, sizeof(TCHAR), lNameSize, hFile);

		return m_BezierAdjust.Save(hFile) && m_HistoAdjust.Save(hFile);
	};
};

typedef std::list<CDSSSetting>			DSSSETTINGLIST;
typedef	DSSSETTINGLIST::iterator		DSSSETTINGITERATOR;

class CDSSSettings
{
private:
	std::list<CDSSSetting>	m_lSettings;
	bool					m_bLoaded;

public:
	CDSSSettings()
	{
		m_bLoaded = false;
	};
	virtual ~CDSSSettings() {};

	bool	IsLoaded()
	{
		return m_bLoaded;
	};
	bool	Load(LPCTSTR szFile = nullptr);
	bool	Save(LPCTSTR szFile = nullptr);

	int Count()
	{
		return static_cast<int>(m_lSettings.size());
	};

	bool	GetItem(int lIndice, CDSSSetting& cds)
	{
		bool			bResult = false;

		if (lIndice < m_lSettings.size())
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			cds = (*it);
			bResult = true;
		};

		return bResult;
	};

	bool	Add(const CDSSSetting& cds)
	{
		m_lSettings.push_back(cds);
		return true;
	};

	bool	Remove(int lIndice)
	{
		bool			bResult = false;

		if (lIndice < m_lSettings.size())
		{
			DSSSETTINGITERATOR	it;

			it = m_lSettings.begin();
			while (lIndice)
			{
				it++;
				lIndice--;
			};

			m_lSettings.erase(it);
			bResult = true;
		};

		return bResult;
	};
};


#include "ExplorerBar.h"
class QSplitter;
class QStackedWidget;
#include "StackingDlg.h"
class QWinHost;

#include "ProcessingDlg.h"

class DeepSkyStacker :
	public QMainWindow
{
	typedef QMainWindow
		Inherited;

	Q_OBJECT

private:
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
	CString					m_strStartFileList;
	CString					m_strBaseTitle;
	//ITaskbarList3* m_taskbarList;
	bool                    m_progress;

public:

	static inline DeepSkyStacker* theMainWindow{ nullptr };

	DeepSkyStacker();

	~DeepSkyStacker()
	{

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

	CDeepStack& GetDeepStack()
	{
		return m_DeepStack;
	};

	CDSSSettings& GetDSSSettings()
	{
		if (!m_Settings.IsLoaded())
			m_Settings.Load();

		return m_Settings;
	};

	DSS::StackingDlg& getStackingDlg()
	{
		return *stackingDlg;
	};

	CProcessingDlg& GetProcessingDlg()
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

	void restoreWindowPosition(bool bCenter = false);


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

DeepSkyStackerApp *		GetDSSApp();
