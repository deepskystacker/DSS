#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>
#include <afxcmn.h>
#include <afxcview.h>
#include <afxwin.h>

#include <QMenu>
#include <QAction>
#include <QMouseEvent>

#include <Ztrace.h>
#include <zexcept.h>

extern bool		g_bShowRefStars;

#include "..\QHTML_Static\QHTM\QHTM.h"

#include "DSSCommon.h"
#include "commonresource.h"
#include "About.h"
#include "DSSVersion.h"
#include "DeepSkyStacker.h"
#include "DeepStackerDlg.h"

//#include "FrameList.h"
#include "RawDDPSettings.h"
#include "RecommendedSettings.h"
#include "RegisterSettings.h"
#include "StackSettings.h"
#include "Workspace.h"

#include "ExplorerBar.h"
#include "ui/ui_ExplorerBar.h"

static void makeLink(QLabel *label, QString color, QString text)
{
	label->setText(QString("<a href='.' style='text-decoration: none; color: %1'>%2</a>").arg(color, text));
}

static void makeLink(QLabel *label, QString color)
{
	makeLink(label, color, label->text());
}

ExplorerBar::ExplorerBar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ExplorerBar)
{
	ZTRACE_RUNTIME("Creating Left Panel");
	ui->setupUi(this);

	makeLinks();

	raise();
	show();
	activateWindow();
	ZTRACE_RUNTIME("Creating Left Panel - ok");
}

ExplorerBar::~ExplorerBar()
{
	delete ui;
}

void ExplorerBar::makeLinks()
{
	QString defColour = palette().color(QPalette::ColorRole::WindowText).name();
	QString redColour = QColor(Qt::red).name();

	makeLink(ui->openLights, redColour);
	makeLink(ui->openDarks, defColour);
	makeLink(ui->openFlats, defColour);
	makeLink(ui->openDarkFlats, defColour);
	makeLink(ui->openBias, defColour);

	makeLink(ui->openFilelist, defColour);
	makeLink(ui->saveFilelist, defColour);

	makeLink(ui->clearList, defColour);

	makeLink(ui->checkAll, defColour);
	makeLink(ui->checkAbove, defColour);
	makeLink(ui->unCheckAll, defColour);

	makeLink(ui->registerChecked, redColour);
	makeLink(ui->computeOffsets, defColour);
	makeLink(ui->stackChecked, redColour);
	makeLink(ui->batchStacking, defColour);

	makeLink(ui->openPicture, defColour);
	makeLink(ui->copyPicture, defColour);
	makeLink(ui->doStarMask, defColour);
	makeLink(ui->savePicture, defColour);

	makeLink(ui->settings, defColour);
	makeLink(ui->ddpSettings, defColour);
	makeLink(ui->loadSettings, defColour);
	makeLink(ui->saveSettings, defColour);
	makeLink(ui->recommendedSettings, redColour);
	makeLink(ui->about, defColour);
	makeLink(ui->help, defColour);
}

//void ExplorerBar::linkActivated()
//{
//}

void ExplorerBar::onOpenLights()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().OnAddpictures();
	};
}
void ExplorerBar::onOpenDarks()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().OnAdddarks();
	};
}
void ExplorerBar::onOpenFlats()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().OnAddFlats();
	};
}
void ExplorerBar::onOpenDarkFlats()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().OnAddDarkFlats();
	};
}
void ExplorerBar::onOpenBias()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().OnAddOffsets();
	};
}

/************************************************************************************/

void ExplorerBar::onOpenFilelist()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().LoadList();
	};
}
void ExplorerBar::onSaveFilelist()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().SaveList();
	};
}
void ExplorerBar::onClearList()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().ClearList();
	};
}

/************************************************************************************/

void ExplorerBar::onCheckAll()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().CheckAll();
	};
}
void ExplorerBar::onCheckAbove()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().CheckAbove();
	};
}
void ExplorerBar::onUncheckAll()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().UncheckAll();
	};
}

/************************************************************************************/

void ExplorerBar::onRegisterChecked()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().RegisterCheckedImage();
	};
}
void ExplorerBar::onComputeOffsets()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().ComputeOffsets();
	};
}
void ExplorerBar::onStackChecked()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().StackCheckedImage();
	};
}
void ExplorerBar::onBatchStacking()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetStackingDlg().BatchStack();
	};
}

/************************************************************************************/

void ExplorerBar::onOpenPicture()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().OnLoaddsi();
	};
}
void ExplorerBar::onCopyPicture()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().CopyPictureToClipboard();
	};
}
void ExplorerBar::onDoStarMask()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().CreateStarMask();
	};
}
void ExplorerBar::onSavePicture()
{
	CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);

	if (pDlg)
	{
		pDlg->GetProcessingDlg().SavePictureToFile();
	};
}

/************************************************************************************/

void ExplorerBar::onSettings()
{
	QMenu menu(this);


	QAction *aRegisterSettings;
	QAction *aStackingSettings;

	aRegisterSettings = menu.addAction(QCoreApplication::translate("ExplorerBar", "Register Settings..."));
	aStackingSettings = menu.addAction(QCoreApplication::translate("ExplorerBar", "Stacking Settings..."));

	QPoint point(ui->settings->mapToGlobal(QPoint(0, 2 + ui->settings->height())));
	QAction *a = menu.exec(point);

	if (a == aRegisterSettings)
	{
		RegisterSettings dlg(this);
		dlg.setSettingsOnly(true);
		dlg.exec();
	}
	else if (a == aStackingSettings)
	{
		StackSettings dlg(this);
		dlg.setEnableAll(true);
		dlg.exec();
	}
}

void ExplorerBar::onDDPSettings()
{
	RawDDPSettings dlg(this);

	dlg.exec();
}

void ExplorerBar::onLoadSettings()
{
#if (0)
	bool			bOpenAnother = true;

	{
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;
		UINT				lStartID;
		CWorkspace			workspace;

		bOpenAnother = false;

		menu.LoadMenu(IDR_LOADSETTINGS);
		popup = menu.GetSubMenu(0);

		CRect				rc;

		m_Options_LoadSettings.GetWindowRect(&rc);
		pt.x = rc.left;
		pt.y = rc.bottom;

		lStartID = ID_LOADSETTINGS_FIRSTMRU + 1;
		for (LONG i = 0; i < m_MRUSettings.m_vLists.size(); i++)
		{
			TCHAR				szDrive[1 + _MAX_DRIVE];
			TCHAR				szDir[1 + _MAX_DIR];
			TCHAR				szName[1 + _MAX_FNAME];
			CString				strItem;

			_tsplitpath((LPCTSTR)m_MRUSettings.m_vLists[i], szDrive, szDir, szName, nullptr);
			strItem = szName;

			popup->InsertMenu(ID_LOADSETTINGS_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
			lStartID++;
		};

		popup->DeleteMenu(ID_LOADSETTINGS_FIRSTMRU, MF_BYCOMMAND);
		if (!m_MRUSettings.m_vLists.size())
		{
			// Remove the separator in first position
			popup->DeleteMenu(0, MF_BYPOSITION);
		};

		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, nullptr);;

		if (nResult == ID_LOADSETTINGS_LOAD)
			bOpenAnother = true;
		else if (nResult == ID_LOADSETTINGS_LOADDEEPSKYSTACKERLIVESETTINGS)
		{
			// Load the DSSLive setting file
			TCHAR				szPath[1 + _MAX_PATH];
			CString				strPath;

			SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);

			strPath = szPath;
			strPath += "\\DeepSkyStacker\\DSSLive.settings";
			workspace.ReadFromFile(strPath);
			workspace.saveSettings();
		}
		else if (nResult == ID_LOADSETTINGS_RESTOREDEFAULTSETTINGS)
		{
			workspace.ResetToDefault();
			workspace.saveSettings();
		}
		else if (nResult > ID_LOADSETTINGS_FIRSTMRU)
		{
			CString			strFile;

			strFile = m_MRUSettings.m_vLists[nResult - ID_LOADSETTINGS_FIRSTMRU - 1];

			workspace.ReadFromFile(strFile);
			workspace.saveSettings();
			m_MRUSettings.Add(strFile);
			m_MRUSettings.saveSettings();
		};
	};

	if (bOpenAnother)
		LoadSettingFile();
#endif
}
void ExplorerBar::onSaveSettings()
{
#if (0)
	bool			bSaveAnother = true;

	{
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;
		UINT				lStartID;
		CWorkspace			workspace;

		bSaveAnother = false;

		menu.LoadMenu(IDR_SAVESETTINGS);
		popup = menu.GetSubMenu(0);

		CRect				rc;

		m_Options_SaveSettings.GetWindowRect(&rc);
		pt.x = rc.left;
		pt.y = rc.bottom;

		lStartID = ID_SAVESETTINGS_FIRSTMRU + 1;
		for (LONG i = 0; i < m_MRUSettings.m_vLists.size(); i++)
		{
			TCHAR				szDrive[1 + _MAX_DRIVE];
			TCHAR				szDir[1 + _MAX_DIR];
			TCHAR				szName[1 + _MAX_FNAME];
			CString				strItem;

			_tsplitpath((LPCTSTR)m_MRUSettings.m_vLists[i], szDrive, szDir, szName, nullptr);
			strItem = szName;

			popup->InsertMenu(ID_SAVESETTINGS_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
			lStartID++;
		};

		popup->DeleteMenu(ID_SAVESETTINGS_FIRSTMRU, MF_BYCOMMAND);
		if (!m_MRUSettings.m_vLists.size())
		{
			// Remove the separator in first position
			popup->DeleteMenu(0, MF_BYPOSITION);
		};

		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, nullptr);;

		if (nResult == ID_SAVESETTINGS_SAVEAS)
			bSaveAnother = true;
		else if (nResult == ID_SAVESETTINGS_SAVEASDEEPSKYSTACKERLIVESETTINGS)
		{
			// Sace the DSSLive setting file
			TCHAR				szPath[1 + _MAX_PATH];
			CString				strPath;

			SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);

			strPath = szPath;
			strPath += "\\DeepSkyStacker\\DSSLive.settings";
			workspace.SaveToFile(strPath);
		}
		else if (nResult > ID_SAVESETTINGS_FIRSTMRU)
		{
			CString			strFile;

			strFile = m_MRUSettings.m_vLists[nResult - ID_SAVESETTINGS_FIRSTMRU - 1];

			workspace.SaveToFile(strFile);
			m_MRUSettings.Add(strFile);
		};
	};

	if (bSaveAnother)
		SaveSettingFile();
#endif
}

void ExplorerBar::onRecommendedSettings()
{
	CRecommendedSettings		dlg;

	dlg.DoModal();
}

/************************************************************************************/

void ExplorerBar::onAbout()
{
	About dlg(this);

	dlg.exec();
}

void ExplorerBar::onHelp()
{
	QString helpFile = QCoreApplication::applicationDirPath() + 
		"/" + tr("DeepSkyStacker Help.chm", "IDS_HELPFILE");

	//
	// Invoking HtmlHelp works fine on Windows but ...
	//
	::HtmlHelp(::GetDesktopWindow(), CString((wchar_t*)helpFile.utf16()), HH_DISPLAY_TOPIC, 0);
}

void ExplorerBar::LoadSettingFile()
{
#if (0)
	CString						strBaseDirectory;
	CString						strTitle;
	DWORD						dwFilterIndex = 0;
	CString						strBaseExtension;

	TCHAR						szPath[1 + _MAX_PATH];

	SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);

	strBaseDirectory = szPath;
	strBaseDirectory += "\\DeepSkyStacker";

	// Check that the path exists, else create it
	CreateDirectory(strBaseDirectory, nullptr);

	strBaseExtension = _T(".dsssettings");
	strTitle.LoadString(IDS_TITLE_LOADSETTINGS);

	CFileDialog			dlgOpen(true,
		strBaseExtension,
		nullptr,
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
		SETTINGFILE_FILTERS,
		this);

	dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	TCHAR				szBigBuffer[20000] = _T("");

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

	if (dlgOpen.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			CWorkspace	workspace;

			strFile = dlgOpen.GetNextPathName(pos);

			workspace.ReadFromFile(strFile);
			workspace.saveSettings();

			m_MRUSettings.Add(strFile);
			m_MRUSettings.saveSettings();
		};
	};
#endif
};

void ExplorerBar::SaveSettingFile()
{
#if (0)
	CString						strBaseDirectory;
	CString						strBaseExtension;
	CString						strTitle;
	DWORD						dwFilterIndex = 0;

	TCHAR						szPath[1 + _MAX_PATH];

	SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);

	strBaseDirectory = szPath;
	strBaseDirectory += "\\DeepSkyStacker";

	// Check that the path exists, else create it
	CreateDirectory(strBaseDirectory, nullptr);

	strBaseExtension = _T(".dsssettings");
	strTitle.LoadString(IDS_TITLE_SAVESETTINGS);

	CFileDialog					dlgSave(false,
		strBaseExtension,
		nullptr,
		OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_DONTADDTORECENT,
		SETTINGFILE_FILTERS,
		this);

	dlgSave.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgSave.m_ofn.nFilterIndex = dwFilterIndex;
	dlgSave.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	if (dlgSave.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgSave.GetStartPosition();
		if (pos)
		{
			CString		strFile;
			CWorkspace	workspace;

			strFile = dlgSave.GetNextPathName(pos);

			workspace.SaveToFile(strFile);
			m_MRUSettings.Add(strFile);
		};
	};
#endif
};

void ExplorerBar::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);

		//
		// The Labels are now plain text labels, so make them into links
		// just as done by the ctor.
		//
		makeLinks();
	}

	Inherited::changeEvent(event);
}

void ExplorerBar::mousePressEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons())
	{
		CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);
		DWORD				dwTabID = 0;

		if (pDlg)
		{
			dwTabID = pDlg->GetCurrentTab();
			if ((ui->registerAndStack->underMouse()) && (dwTabID != IDD_REGISTERING) && (dwTabID != IDD_STACKING))
			{
				// Change tab to stacking
				pDlg->ChangeTab(IDD_STACKING);
			}
			else if (ui->processing->underMouse() && (dwTabID != IDD_PROCESSING))
			{
				// Change tab to processing
				pDlg->ChangeTab(IDD_PROCESSING);
			};
		}
	}
	Inherited::mousePressEvent(event);
}