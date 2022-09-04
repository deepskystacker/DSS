

#pragma once

// DeepStackerDlg.h : header file
//

#include "DeepStack.h"
#include "DeepSkyStacker.h"
#include <list>

class CDSSSetting
{
public :
	CString				m_strName;
	CBezierAdjust		m_BezierAdjust;
	CRGBHistogramAdjust	m_HistoAdjust;

private :
	void	CopyFrom(const CDSSSetting & cds)
	{
		m_strName		= cds.m_strName;
		m_BezierAdjust	= cds.m_BezierAdjust;
		m_HistoAdjust	= cds.m_HistoAdjust;
	};

public :
	CDSSSetting() {};
	virtual ~CDSSSetting() {};

	CDSSSetting(const CDSSSetting & cds)
	{
		CopyFrom(cds);
	};

	CDSSSetting & operator = (const CDSSSetting & cds)
	{
		CopyFrom(cds);
		return (*this);
	};

	bool operator < (const CDSSSetting & cds) const
	{
		int			nCompare;
		nCompare = m_strName.CompareNoCase(cds.m_strName);

		if (nCompare < 0)
			return true;
		else
			return false;
	};

	bool	Load(FILE * hFile)
	{
		int		lNameSize;
		TCHAR	szName[2000] = { _T('\0') };

		fread(&lNameSize, sizeof(lNameSize), 1, hFile);
		fread(szName, sizeof(TCHAR), lNameSize, hFile);
		m_strName = szName;
		return m_BezierAdjust.Load(hFile) && m_HistoAdjust.Load(hFile);
	};

	bool	Save(FILE * hFile)
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
private :
	std::list<CDSSSetting>	m_lSettings;
	bool					m_bLoaded;

public :
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

	bool	GetItem(int lIndice, CDSSSetting & cds)
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

	bool	Add(const CDSSSetting & cds)
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
class QSplitter;
class QStackedWidget;
#include "ExplorerBar.h"
#include "StackingDlg.h"

#include "ProcessingDlg.h"
//#include "LibraryDlg.h"

#include "afxwin.h"

#include "qwinwidget.h"

/////////////////////////////////////////////////////////////////////////////
// CDeepStackerDlg dialog

enum DeepStackerDlgMessages
{
    WM_PROGRESS_INIT = WM_USER + 10000,
    WM_PROGRESS_UPDATE,
    WM_PROGRESS_STOP,
};

class CDeepStackerDlg : public CDialog
{
private :
	QWinWidget*		widget;
	QSplitter*		splitter;
	ExplorerBar*	explorerBar;
	QStackedWidget* stackedWidget;
	DSS::StackingDlg*	stackingDlg;

	CProcessingDlg	m_dlgProcessing;
	//CLibraryDlg				m_dlgLibrary;

	CDeepStack				m_DeepStack;
	CDSSSettings			m_Settings;
	std::uint32_t			CurrentTab;
	CString					m_strStartFileList;
	CString					m_strBaseTitle;
    ITaskbarList3*          m_taskbarList;
    bool                    m_progress;

// Construction
public:
	CDeepStackerDlg(CWnd* pParent = nullptr);   // standard constructor

	~CDeepStackerDlg()
	{
		if (explorerBar)
			delete explorerBar;
		if (stackingDlg)
			delete stackingDlg;
		if (widget)
			delete widget;
	};

	void	ChangeTab(std::uint32_t dwTabID);
	std::uint32_t GetCurrentTab()
	{
		return CurrentTab;
	};

	void	SetStartingFileList(LPCTSTR szStartFileList)
	{
		m_strStartFileList = szStartFileList;
	};

	inline void disableSubDialogs()
	{
		stackingDlg->setEnabled(false);
		m_dlgProcessing.EnableWindow(false);
		//m_dlgLibrary.EnableWindow(false);
		explorerBar->setEnabled(false);
	};

	inline void enableSubDialogs()
	{
		stackingDlg->setEnabled(true);
		m_dlgProcessing.EnableWindow(true);
		//m_dlgLibrary.EnableWindow(true);
		explorerBar->setEnabled(true);
	};

// Dialog Data
	//{{AFX_DATA(CDeepStackerDlg)
	enum { IDD = IDD_DEEPSTACKER };
	CStatic		m_BarStatic;
	//}}AFX_DATA

	CDeepStack & GetDeepStack()
	{
		return m_DeepStack;
	};

	CDSSSettings & GetDSSSettings()
	{
		if (!m_Settings.IsLoaded())
			m_Settings.Load();

		return m_Settings;
	};

	DSS::StackingDlg & GetStackingDlg()
	{
		return *stackingDlg;
	};

	CProcessingDlg & GetProcessingDlg()
	{
		return m_dlgProcessing;
	};


	ExplorerBar & GetExplorerBar()
	{
		return *explorerBar;
	};

	void	SetCurrentFileInTitle(LPCTSTR szFileName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepStackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeepStackerDlg)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg	BOOL	OnEraseBkgnd(CDC * pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnHTMLHelp(WPARAM, LPARAM);
	afx_msg	LRESULT OnOpenStartFileList(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

private :
	void	UpdateTab();
	// void	UpdateSizes();
	afx_msg void OnOK();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg	void OnHelp();

	afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg LRESULT OnTaskbarButtonCreated(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProgressInit(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProgressUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProgressStop(WPARAM wParam, LPARAM lParam);
};

/* ------------------------------------------------------------------- */

inline CDeepStackerDlg * GetDeepStackerDlg(CWnd * pDialog)
{
	if (pDialog)
	{
		CWnd *				pParent = pDialog->GetParent();
		CDeepStackerDlg *	pDlg = dynamic_cast<CDeepStackerDlg *>(pParent);

		if (!pDlg)
			pDlg = dynamic_cast<CDeepStackerDlg *>(GetDSSApp()->m_pMainDlg);

		return pDlg;
	}
	else
	{
		CWnd *				pWnd = GetDSSApp()->m_pMainDlg;
		CDeepStackerDlg *	pDlg = dynamic_cast<CDeepStackerDlg *>(pWnd);

		return pDlg;
	};
};

/* ------------------------------------------------------------------- */

inline CDeepStack & GetDeepStack(CWnd * pDialog)
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(pDialog);

	return pDlg->GetDeepStack();
};

/* ------------------------------------------------------------------- */

inline CDSSSettings & GetDSSSettings(CWnd * pDialog)
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(pDialog);

	return pDlg->GetDSSSettings();
};

/* ------------------------------------------------------------------- */

inline DSS::StackingDlg & GetStackingDlg(CWnd * pDialog)
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(pDialog);

	return pDlg->GetStackingDlg();
};

/* ------------------------------------------------------------------- */

inline CProcessingDlg & GetProcessingDlg(CWnd * pDialog)
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(pDialog);

	return pDlg->GetProcessingDlg();
};


/* ------------------------------------------------------------------- */

inline void	SetCurrentFileInTitle(LPCTSTR szFileName)
{
	CDeepStackerDlg *		pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
		pDlg->SetCurrentFileInTitle(szFileName);
};

/* ------------------------------------------------------------------- */

void	SaveWindowPosition(CWnd * pWnd, LPCSTR szRegistryPath);
void	RestoreWindowPosition(CWnd * pWnd, LPCSTR szRegistryPath, bool bCenter = false);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.