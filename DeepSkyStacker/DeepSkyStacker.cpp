/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// DeepSkyStacker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <chrono>
#include <boost/interprocess/sync/named_mutex.hpp>
using namespace boost::interprocess;
#include <gdiplus.h>
using namespace Gdiplus;
#include <QApplication>
#include <QLibraryInfo>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>
#include <QtGui>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

//#include "QMfcApp"

#include "qwinhost.h"

#include "DeepSkyStacker.h"
#include "DeepStack.h"


#include <afxinet.h>
#include "StackingTasks.h"
#include "ui_StackingDlg.h"
#include "StackRecap.h"
#include "cgfiltyp.h"
#include "SetUILanguage.h"
#include <ZExcept.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

CString OUTPUTFILE_FILTERS;
CString	OUTPUTLIST_FILTERS;
CString SETTINGFILE_FILTERS;
CString STARMASKFILE_FILTERS;
bool	g_bShowRefStars = false;

#include <string.h>
#include <stdio.h>

bool	hasExpired()
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	using namespace std::chrono;
#ifdef DSSBETA

	ZTRACE_RUNTIME("Check beta expiration\n");

	auto now{ system_clock::now() };
	auto tt{ system_clock::to_time_t(now) };
	auto local_tm{ *localtime(&tt) };

	auto year{ local_tm.tm_year + 1900 };
	auto month{ local_tm.tm_mon + 1 };

	constexpr auto		lMaxYear = DSSBETAEXPIREYEAR;
	constexpr auto		lMaxMonth = DSSBETAEXPIREMONTH;
	if ((year > lMaxYear) || ((year == lMaxYear) && (month > lMaxMonth)))
	{
		QString message = QCoreApplication::translate("DeepSkyStacker",
			"This beta version of DeepSkyStacker has expired\nYou can probably get another one or download the final release from the web site.");
		QMessageBox::critical(nullptr, "DeepSkyStacker",
			message, QMessageBox::Ok);
		bResult = true;
	};

	ZTRACE_RUNTIME("Check beta expiration - ok\n");

#endif
	return bResult;
};

/* ------------------------------------------------------------------- */

void	askIfVersionCheckWanted()
{
	ZFUNCTRACE_RUNTIME();
	QSettings			settings;

	bool checkVersion = false;

	//
	// If we don't know whether to do a version check or not
	// we ask
	//
	if (settings.value("InternetCheck").isNull())
	{
		QString	strMsg{ QCoreApplication::translate("DeepSkyStacker",
			"Do you want DeepSkyStacker to check if a newer version is available at startup?\n"
			"(You can enable or disable this option later from the About box)",
			"IDS_CHECKVERSION") };

		auto result = QMessageBox::question(nullptr, "DeepSkyStacker", strMsg);

		if (QMessageBox::Yes == result)
			checkVersion = true;
		else
			checkVersion = false;
		settings.setValue("InternetCheck", checkVersion);
	};
};

/* ------------------------------------------------------------------- */

void	deleteRemainingTempFiles()
{
	ZFUNCTRACE_RUNTIME();
	std::vector<QString>	vFiles;
	qint64					totalSize = 0;

	ZTRACE_RUNTIME("Finding remaining temp files\n");

	QString folder(CAllStackingTasks::GetTemporaryFilesFolder());

	QStringList nameFilters("DSS*.tmp");

	QDir dir(folder);
	dir.setNameFilters(nameFilters);

	for (QFileInfo item : dir.entryInfoList())
	{
		if (item.isFile())
		{
			vFiles.emplace_back(item.absoluteFilePath());
			totalSize += item.size();
		}
	}
	ZTRACE_RUNTIME("Find remaining temp files - ok\n");

	if (!vFiles.empty())
	{
		QString			strMsg;
		QString			strSize;

		ZTRACE_RUNTIME("Remove remaining temp files\n");

		SpaceToQString(totalSize, strSize);

		strMsg = QString("One or more temporary files created by DeepSkyStacker are still in the working directory."
			"\n\nDo you want to remove them?\n\n(%1 file(s) using %2)")
			.arg(vFiles.size()).arg(strSize);

		QMessageBox msgBox(QMessageBox::Question, QString(""), strMsg, (QMessageBox::Yes | QMessageBox::No));

		msgBox.setDefaultButton(QMessageBox::Yes);

		if (QMessageBox::Yes == msgBox.exec())
		{
			QFile file;
			for (size_t i = 0; i < vFiles.size(); i++)
			{
				file.setFileName(vFiles[i]);
				file.remove();
			}
		};

		ZTRACE_RUNTIME("Remove remaining temp files - ok\n");
	};


};

void DeepSkyStacker::dragEnterEvent(QDragEnterEvent* e)
{
	if (e->mimeData()->hasFormat("text/uri-list"))
		e->acceptProposedAction();
}

void DeepSkyStacker::dropEvent(QDropEvent* e)
{
	raise();
	show();
	activateWindow();

	stackingDlg->dropFiles(e);
}

void DeepSkyStacker::showEvent(QShowEvent* event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitialise();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

void DeepSkyStacker::onInitialise()
{
	ZFUNCTRACE_RUNTIME();

	widget = new QWidget(this);
	widget->setObjectName("centralWidget");
	setCentralWidget(widget);

	QHBoxLayout* horizontalLayout{ new QHBoxLayout(widget) };
	widget->setLayout(horizontalLayout);
	ZTRACE_RUNTIME("Creating Horizontal Splitter");
	splitter = new QSplitter(Qt::Horizontal, widget);
	splitter->setObjectName("splitter");

	ZTRACE_RUNTIME("Creating Explorer Bar (Left Panel)");
	explorerBar = new ExplorerBar(widget);
	explorerBar->setObjectName("explorerBar");
	splitter->addWidget(explorerBar);

	ZTRACE_RUNTIME("Creating stackedWidget");
	stackedWidget = new QStackedWidget(splitter);
	stackedWidget->setObjectName("stackedWidget");
	splitter->addWidget(stackedWidget);

	ZTRACE_RUNTIME("Creating Stacking Panel");
	stackingDlg = new DSS::StackingDlg(widget);
	stackingDlg->setObjectName("stackingDlg");

	ZTRACE_RUNTIME("Adding Stacking Panel to stackedWidget");
	stackedWidget->addWidget(stackingDlg);

	winHost = new QWinHost(stackedWidget);
	winHost->setObjectName("winHost");
	stackedWidget->addWidget(winHost);

	ZTRACE_RUNTIME("Creating Processing Panel");
	BOOL result = processingDlg.Create(IDD_PROCESSING);
	//processingDlg.OnInitDialog();

	HWND hwnd{ processingDlg.GetSafeHwnd() };
	Q_ASSERT(NULL != hwnd);
	winHost->setWindow(hwnd);

	splitter->setStretchFactor(1, 1);		// Want Stacking part to take any spare space.

	horizontalLayout->addWidget(splitter);
	widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	winHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	setWindowIcon(QIcon(":/DSSIcon.png"));

	setWindowTitle(baseTitle);

	ZTRACE_RUNTIME("Restoring Window State and Position");
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());

	//
	// Check to see if we were passed a filelist file to open
	//
	if (2 <= args.size())
	{
		QString name{ args[1] };
		fs::path file{ name.toStdWString() };
		if (fs::file_type::regular == status(file).type())
		{
			stackingDlg->setFileList(file); // TODO
		}
		else
			QMessageBox::warning(this,
				"DeepSkyStacker",
				tr("%1 does not exist or is not a file").arg(name));
	}

}

void DeepSkyStacker::setTitleFilename(const fs::path file)
{
	fs::path filename{ file.filename() };
	if (!filename.empty())
	{
		setWindowTitle(QString("%1 - %2").arg(baseTitle).arg(filename.generic_string().c_str()));
	}
	else
		setWindowTitle(baseTitle);
}


void DeepSkyStacker::closeEvent(QCloseEvent* e)
{
	ZFUNCTRACE_RUNTIME();
	processingDlg.DestroyWindow();
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	QTableView* tableView = this->findChild<QTableView*>("tableView");
	settings.setValue("Dialogs/StackingDlg/TableView/HorizontalHeader/windowState",
		tableView->horizontalHeader()->saveState());
};


GdiplusStartupOutput gdiSO;
ULONG_PTR gdiplusToken{ 0ULL };
ULONG_PTR gdiHookToken{ 0ULL };

DeepSkyStacker::DeepSkyStacker() :
	initialised{ false },
	QMainWindow(),
	widget{ nullptr },
	splitter{ nullptr },
	explorerBar{ nullptr },
	stackedWidget{ nullptr },
	stackingDlg{ nullptr },
	winHost{ nullptr },
	currTab{ 0 },
	args{ qApp->arguments() },
	// m_taskbarList{ nullptr },
	baseTitle{ QString("DeepSkyStacker %1").arg(VERSION_DEEPSKYSTACKER) },
	m_progress{ false }

{
	ZFUNCTRACE_RUNTIME();
	setAcceptDrops(true);
}

void DeepSkyStacker::updateTab()
{
	switch (currTab)
	{
	case IDD_REGISTERING:
	case IDD_STACKING:
		stackedWidget->setCurrentIndex(0);
		stackingDlg->update();
		break;
	case IDD_PROCESSING:
		stackedWidget->setCurrentIndex(1);
		processingDlg.ShowWindow(SW_SHOW);
		break;
	};
	explorerBar->update();
};



BOOL DeepSkyStackerApp::InitInstance()
{
	ZFUNCTRACE_RUNTIME();
	BOOL result = CWinApp::InitInstance();

	EnableHtmlHelp();

	SetRegistryKey(_T("DeepSkyStacker"));

	//
	// Set our Profile Name to DeepSkyStacker5 so native Windows registry stuff
	// will be written under "DeepSkyStacker\\DeepSkyStacker5"
	// 
	// First free the string allocated by MFC at CWinApp startup.
	// The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	// Change the name of the registry profile to use.
	// The CWinApp destructor will free the memory.
	m_pszProfileName = _tcsdup(_T("DeepSkyStacker5"));

	//ZTRACE_RUNTIME("AfxInitialize()");
	//if (!AfxInitialize())
	//{
	//	AfxMessageBox(L"AfxInitialize failed.", MB_OK | MB_ICONSTOP);
	//}


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(L"OLE initialization failed.\nMake sure that the OLE libraries are the correct version.",
			MB_OK | MB_ICONSTOP);
		return false;
	}

	GdiplusStartupInput		gdiplusStartupInput;


	ZTRACE_RUNTIME("Initialize GDI+");

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = true;
	Gdiplus::Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);
	if (Gdiplus::Ok != status)
	{
		AfxMessageBox(L"GDI+ initialization failed.");
		return false;
	}
	ZTRACE_RUNTIME("Initialize GDI+ - ok");

	AfxInitRichEdit2();


	OUTPUTFILE_FILTERS.LoadString(IDS_FILTER_OUTPUT);
	OUTPUTLIST_FILTERS.LoadString(IDS_LISTFILTER_OUTPUT);
	SETTINGFILE_FILTERS.LoadString(IDS_FILTER_SETTINGFILE);
	STARMASKFILE_FILTERS.LoadString(IDS_FILTER_MASK);

	ZTRACE_RUNTIME("Reset dssfilelist extension association with DSS\n");

	CGCFileTypeAccess	FTA;
	TCHAR				szPath[1 + _MAX_PATH];
	CString				strPath;
	CString				strTemp;

	::GetModuleFileName(nullptr, szPath, sizeof(szPath) / sizeof(TCHAR));
	strPath = szPath;

	FTA.SetExtension(_T("dssfilelist"));

	strTemp = strPath;
	strTemp += _T(" \"%1\"");
	FTA.SetShellOpenCommand(strTemp);
	FTA.SetDocumentShellOpenCommand(strTemp);
	FTA.SetDocumentClassName(_T("DeepSkyStacker.FileList"));

	CString				strFileListDescription;

	strFileListDescription.LoadString(IDS_FILELISTDESCRIPTION);

	FTA.SetDocumentDescription(strFileListDescription);

	// use first icon in program
	strTemp = strPath;
	strTemp += ",1";
	FTA.SetDocumentDefaultIcon(strTemp);

	// set the necessary registry entries
	FTA.RegSetAllInfo();
	ZTRACE_RUNTIME("Reset dssfilelist extension association with DSS - ok\n");

	return TRUE;
};

int DeepSkyStackerApp::ExitInstance()
{
	ZFUNCTRACE_RUNTIME();
	//TODO: handle additional resources you may have added

	AfxOleTerm(FALSE);

#ifndef NOGDIPLUS
	// Shutdown GDI+
	ZTRACE_RUNTIME("Shutting down GDI+");
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
#endif

	return CWinApp::ExitInstance();
}

// The DeepSkyStacker class, a subclass of CWinApp, runs the event loop in the default implementation of Run().
// The MFC event loop is a standard Win32 event loop, but uses the CWinApp API PreTranslateMessage() to activate accelerators.
//
/* ------------------------------------------------------------------- */
int DeepSkyStackerApp::Run()
{
	ZFUNCTRACE_RUNTIME();
	ZASSERT(false);
	return 0;
}

named_mutex dssMutex{ open_or_create, "DeepSkyStacker.Mutex.UniqueID.12354687" };

DeepSkyStackerApp theApp;

DeepSkyStackerApp *		GetDSSApp()
{
	return &theApp;
};

using namespace std;

QTranslator theQtTranslator;
QTranslator theAppTranslator;

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();
	int result{ 0 };
	bool firstInstance = true;

	//
	// Silence the MFC memory leak dump as we use Visual Leak Detector.
	//
#if defined(_WINDOWS)
	_CrtSetDbgFlag(0);
#if !defined(NDEBUG)
	AfxEnableMemoryLeakDump(false);
#endif
#endif

	if (hasExpired())
		return FALSE;

	ZTRACE_RUNTIME("Checking Mutex");
	if (!dssMutex.try_lock()) firstInstance = false;
	ZTRACE_RUNTIME("Checking Mutex - ok");

	ZTRACE_RUNTIME("Initialize MFC");
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(nullptr), nullptr, ::GetCommandLine(), 0))
	{
		ZTRACE_RUNTIME("Fatal Error: MFC initialization failed");
		QString errorMessage{ "Fatal Error: MFC initialization failed" };
		cerr << errorMessage.toStdWString().c_str() << endl;
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
		return 1;
	}
	// initialize all the windows stuff we need for now
	theApp.InitInstance();

	// High DPI support
#if QT_VERSION < 0x060000
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

	//QMfcApp app(&theApp, argc, argv);
	QApplication app(argc, argv);

	//
	// Set up organisation etc. for QSettings usage
	//
	QCoreApplication::setOrganizationName("DeepSkyStacker");
	QCoreApplication::setOrganizationDomain("deepskystacker.free.fr");
	QCoreApplication::setApplicationName("DeepSkyStacker5");

	QSettings		settings;

	g_bShowRefStars = settings.value("ShowRefStars", false).toBool();

	//
	// Set the Qt Application Style
	//
	app.setStyle(QStyleFactory::create("Fusion"));

	ZTRACE_RUNTIME("Initialize Application - ok");

	ZTRACE_RUNTIME("Set UI Language");

	//
	// Retrieve the Qt language name (e.g.) en_GB
	//
	QString language = settings.value("Language").toString();

	//
	// Language was not defined in our preferences, so select the system default
	//
	if (language == "")
	{
		language = QLocale::system().name();
	}

	bool installTranslatorResult{ false };

	QString translatorFileName = QLatin1String("qt_");
	translatorFileName += language;
	qDebug() << "qt translator filename: " << translatorFileName;
	
#if QT_VERSION >= 0x060000
	qDebug() << "translationPath " << QLibraryInfo::path(QLibraryInfo::TranslationsPath);
	if (theQtTranslator.load(translatorFileName, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
#else
	qDebug() << "translationPath " << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
	if (theQtTranslator.load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
#endif
	{
		bool r{ app.installTranslator(&theQtTranslator) };
		Q_ASSERT(r);
	}

	translatorFileName = QLatin1String("DSS.");
	translatorFileName += language;
	qDebug() << "app translator filename: " << translatorFileName;
	//
	// Install the language if it actually exists.
	//
	if (theAppTranslator.load(translatorFileName, ":/i18n/"))
	{
		bool r{ app.installTranslator(&theAppTranslator) };
		Q_ASSERT(r);
	}

	//
	// Do the old Windows language stuff
	//
	SetUILanguage();

	ZTRACE_RUNTIME("Set UI Language - ok");

	askIfVersionCheckWanted();
	if (firstInstance)
		deleteRemainingTempFiles();

	ZTRACE_RUNTIME("Creating Main Window");
	try
	{
		DeepSkyStacker mainWindow;
		DeepSkyStacker::setInstance(&mainWindow);

		mainWindow.show();
		//result = app.run(&theApp);
		result = app.exec();
	}
	catch (std::exception& e)
	{
		QString errorMessage(e.what());
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif
	}
	catch (CException& e)
	{
		e.ReportError();
		e.Delete();
	}
	catch (ZException& ze)
	{
		QString name(ze.name());
		QString fileName(ze.locationAtIndex(0)->fileName());
		QString functionName(ze.locationAtIndex(0)->functionName());
		QString text(ze.text(0));

		QString errorMessage{ "Exception %1 thrown from %2 Function: %3() Line: %4\n\n%5" };
		errorMessage = errorMessage
			.arg(name)
			.arg(fileName)
			.arg(functionName)
			.arg(ze.locationAtIndex(0)->lineNumber())
			.arg(text);

#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif
	}
	catch (...)
	{
		QString errorMessage("Unknown exception caught");
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif

	}
	theApp.ExitInstance();
	dssMutex.unlock();
	return result;
}
/* ------------------------------------------------------------------- */
void	SaveWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath)
{
	ZFUNCTRACE_RUNTIME();
	std::uint32_t dwMaximized = 0;
	std::uint32_t dwTop = 0;
	std::uint32_t dwLeft = 0;
	std::uint32_t dwWidth = 0;
	std::uint32_t dwHeight = 0;

	QSettings	settings;

	WINDOWPLACEMENT		wp;

	memset(&wp, 0, sizeof(wp));
	wp.length = sizeof(wp);

	pWnd->GetWindowPlacement(&wp);
	dwMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
	dwLeft = wp.rcNormalPosition.left;
	dwTop = wp.rcNormalPosition.top;

	dwWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	dwHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;

	ZTRACE_RUNTIME("Saving window position to: %s", szRegistryPath);
	QString regBase(szRegistryPath);
	QString key = regBase + "/Maximized";
	settings.setValue(key, (uint)dwMaximized);

	key = regBase + "/Top";
	settings.setValue(key, (uint)dwTop);

	key = regBase + "/Left";
	settings.setValue(key, (uint)dwLeft);

	key = regBase + "/Width";
	settings.setValue(key, (uint)dwWidth);

	key = regBase + "/Height";
	settings.setValue(key, (uint)dwHeight);

};

/* ------------------------------------------------------------------- */

void	RestoreWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath, bool bCenter)
{
	ZFUNCTRACE_RUNTIME();
	std::uint32_t dwMaximized = 0;
	std::uint32_t dwTop = 0;
	std::uint32_t dwLeft = 0;
	std::uint32_t dwWidth = 0;
	std::uint32_t dwHeight = 0;

	QSettings   settings;

	ZTRACE_RUNTIME("Restoring window position from: %s", szRegistryPath);

	QString regBase(szRegistryPath);
	QString key = regBase + "/Maximized";
	dwMaximized = settings.value(key).toUInt();

	key = regBase + "/Top";
	dwTop = settings.value(key).toUInt();

	key = regBase + "/Left";
	dwLeft = settings.value(key).toUInt();

	key = regBase + "/Width";
	dwWidth = settings.value(key).toUInt();

	key = regBase += "/Height";
	dwHeight = settings.value(key).toUInt();

	if (dwTop && dwLeft && dwWidth && dwHeight)
	{
		WINDOWPLACEMENT		wp;

		memset(&wp, 0, sizeof(wp));
		wp.length = sizeof(wp);
		wp.flags = 0;
		wp.showCmd = dwMaximized ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;
		wp.rcNormalPosition.left = dwLeft;
		wp.rcNormalPosition.top = dwTop;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + dwWidth;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + dwHeight;

		pWnd->SetWindowPlacement(&wp);
		if (bCenter)
			pWnd->CenterWindow();
	};
};
