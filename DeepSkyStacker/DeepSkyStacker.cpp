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
#include <stdafx.h>
#include <htmlhelp.h>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/exceptions.hpp>

#include <fstream>

namespace bip = boost::interprocess;

#include "DeepSkyStacker.h"
#include "ui_StackingDlg.h"
#include "Ztrace.h"
#include "StackingTasks.h"
#include "StackingDlg.h"
#include "ExplorerBar.h"
#include "picturelist.h"
#include "resource.h"
#include "commonresource.h"
#include "ProcessingDlg.h"
#include "ExceptionHandling.h"
#include "SetUILanguage.h"
#include "qwinhost.h"
#include "DeepStack.h"
#include "tracecontrol.h"
#include "Workspace.h"
#include "QEventLogger.h"


CString OUTPUTFILE_FILTERS;
CString STARMASKFILE_FILTERS;
bool	g_bShowRefStars = false;

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ std::source_location::current().file_name() };

namespace
{
	QtMessageHandler originalHandler;
	void qtMessageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		QByteArray localMsg = msg.toLocal8Bit();
		const char* file = context.file ? context.file : "";
		char* name{ static_cast<char*>(_alloca(1 + strlen(file))) };
		strcpy(name, file);
		if (0 != strlen(name))
		{
			fs::path path{ name };
			strcpy(name, path.filename().string().c_str());
		}

		switch (type) {
		case QtDebugMsg:
			ZTRACE_RUNTIME("Qt Debug: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtInfoMsg:
			ZTRACE_RUNTIME("Qt Info: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtWarningMsg:
			ZTRACE_RUNTIME("Qt Warn: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtCriticalMsg:
			ZTRACE_RUNTIME("Qt Critical: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		case QtFatalMsg:
			ZTRACE_RUNTIME("Qt Fatal: (%s:%u) %s", name, context.line, localMsg.constData());
			break;
		}
		originalHandler(type, context, msg);
	}
}

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

	if (!vFiles.empty())
	{
		QString			strMsg;
		QString			strSize;

		ZTRACE_RUNTIME("Remove remaining %d temp files", vFiles.size());

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

	};


};

DeepSkyStacker::DeepSkyStacker() :
	QMainWindow(),
	initialised{ false },
	pictureList{ nullptr },
	explorerBar{ nullptr },
	stackedWidget{ nullptr },
	stackingDlg{ nullptr },
	winHost{ nullptr },
	currTab{ IDD_REGISTERING },
	args{ qApp->arguments() },
	// m_taskbarList{ nullptr },
	baseTitle{ QString("DeepSkyStacker %1").arg(VERSION_DEEPSKYSTACKER) },
	m_progress{ false },
	sponsorText{ new QLabel("") },
	statusBarText{ new QLabel("") },
	processingDlg{ std::make_unique<CProcessingDlg>() },
	m_DeepStack{ std::make_unique<CDeepStack>() },
	m_ImageProcessingSettings{ std::make_unique<CDSSSettings>() },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr },		// errorMessageDialogIcon pointer
	helpShortCut{ new QShortcut(QKeySequence::HelpContents, this) }
{
	ZFUNCTRACE_RUNTIME();
	DSSBase::setInstance(this);
	
	//
	// Set to F1 (Windows) or Command + ? (MacOs) or ?? to invoke help
	//
	helpShortCut->setContext(Qt::ApplicationShortcut);
	setAcceptDrops(true);
	errorMessageDialog->setWindowTitle("DeepSkyStacker");

	//
	// Force setting of blackPointToZero as initially false
	//
	Workspace{}.setValue("RawDDP/BlackPointTo0", false);

	//
	// Set the Docking Area Corner Configuration so that the
	// Explorer Bar takes the full left side docking area
	//
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

	ZTRACE_RUNTIME("Creating Explorer Bar (Left Panel)");
	explorerBar = new ExplorerBar(this);
	addDockWidget(Qt::LeftDockWidgetArea, explorerBar);

	ZTRACE_RUNTIME("Creating pictureList");
	pictureList = new DSS::PictureList(this);
	addDockWidget(Qt::BottomDockWidgetArea, pictureList);

	ZTRACE_RUNTIME("Creating stackedWidget");
	stackedWidget = new QStackedWidget(this);
	stackedWidget->setObjectName("stackedWidget");
	setCentralWidget(stackedWidget);

	ZTRACE_RUNTIME("Creating Stacking Panel");
	stackingDlg = new DSS::StackingDlg(this, pictureList);
	stackingDlg->setObjectName("stackingDlg");

	ZTRACE_RUNTIME("Adding Stacking Panel to stackedWidget");
	stackedWidget->addWidget(stackingDlg);

	winHost = new QWinHost(stackedWidget);
	winHost->setObjectName("winHost");
	stackedWidget->addWidget(winHost);
	
	stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	winHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//
	// Connect Qt Signals to appropriate slots
	//
	connectSignalsToSlots();

	setWindowIcon(QIcon(":/DSSIcon.png"));

	setWindowTitle(baseTitle);

	//
	// Set up the status bar
	//
	createStatusBar();

	//
	// Check to see if we were passed a filelist file to open
	//
	if (2 <= args.size())
	{
		QString name{ args[1] };
		fs::path file{ name.toStdU16String() };
		if (fs::file_type::regular == status(file).type())
		{
			stackingDlg->setFileList(file);
		}
		else
			QMessageBox::warning(this,
				"DeepSkyStacker",
				tr("%1 does not exist or is not a file").arg(name));
	}

	//
	// Set initial size of the bottom dock widget (pictureList)
	//
	resizeDocks({ pictureList }, { 150 }, Qt::Vertical);

	ZTRACE_RUNTIME("Restoring Window State and Position");
	QSettings settings;
	settings.beginGroup("MainWindow");

	if (settings.contains("geometry") && settings.contains("maximised"))
	{
		const QByteArray geometry{ settings.value("geometry").toByteArray() };
		const bool maximised{ settings.value("maximised").toBool() };

		if (maximised)
		{
			setGeometry(screen()->availableGeometry());
			showMaximized();
		}
		else
		{
			restoreGeometry(geometry);
		}
	}

	if (settings.contains("windowState"))
	{
		auto windowState{ settings.value("windowState").toByteArray() };
		restoreState(windowState);
	}


	settings.endGroup();
}

DeepSkyStacker::~DeepSkyStacker()
{
	hostWnd.Detach();
}

void DeepSkyStacker::showEvent(QShowEvent* event)
{
	// Invoke base class showEvent()
	Inherited::showEvent(event);
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitialise();
		}
	}
}

void DeepSkyStacker::onInitialise()
{
	ZFUNCTRACE_RUNTIME();
	//
	// Attach the hwnd of the stacked widget to a CWnd that we use as the parent for the 
	// processing dialog.   This code can't be in the ctor because stackedWidget doesn't
	// have an HWND at that point, whereas it does when Qt inokes the showEvent handler.
	//
	HWND hwnd{ reinterpret_cast<HWND>(stackedWidget->effectiveWinId()) };
	hostWnd.Attach(hwnd);

	ZTRACE_RUNTIME("Creating Processing Panel");
	auto result = processingDlg->Create(IDD_PROCESSING, &hostWnd);
	if (FALSE == result)
	{
		int lastErr = GetLastError();
		ZTRACE_RUNTIME("lastErr = %d", lastErr);
	}
	processingDlg->setParent(winHost);			// Provide a Qt object to be parent for any Qt Widgets this creates

	hwnd = processingDlg->GetSafeHwnd();
	ZASSERT(NULL != hwnd);
	winHost->setWindow(hwnd);

	//
	// If the Stacking Dialog was not visible when DeepSkyStacker last closed, it
	// may not be visible now.  We want it to be visible.
	//
	QTimer::singleShot(20,
		[this]()
		{
			this->stackingDlg->setVisible(true);
			this->setTab(IDD_REGISTERING);
			this->update();
		});

}

void DeepSkyStacker::createStatusBar()
{
	QColor	linkColour{ (Qt::ColorScheme::Dark == QGuiApplication::styleHints()->colorScheme()) ? Qt::cyan : Qt::darkBlue };

	QString text{ QString("<img border=\"0\" src=\":/Heart.png\" width=\"16\" height=\"16\" >&nbsp;"
		"<a style=\"font-size:16px; color:%1;\" href=\"https://github.com/sponsors/deepskystacker\""
		"<span>%2</span></a>")
		.arg(linkColour.name())
		.arg(tr("Sponsor DeepSkyStacker"))
	};
		
	sponsorText->setAlignment(Qt::AlignRight | Qt::AlignTop);
	sponsorText->setTextFormat(Qt::RichText);
	sponsorText->setOpenExternalLinks(true);
	sponsorText->setText(text);

	statusBarText->setAlignment(Qt::AlignHCenter);

	statusBar()->addPermanentWidget(sponsorText, 0);
	statusBar()->addWidget(statusBarText, 1);
	connect(stackingDlg, &DSS::StackingDlg::statusMessage, this, &DeepSkyStacker::updateStatus);
}

void DeepSkyStacker::reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate)
{
	if (terminate) traceControl.setDeleteOnExit(false);
	if (Method::QMessageBox == method)
	{
		QMetaObject::invokeMethod(this, "qMessageBox", Qt::AutoConnection,
			Q_ARG(const QString&, message),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity)),
			Q_ARG(bool, terminate));
	}
	else
	{
		QMetaObject::invokeMethod(this, "qErrorMessage", Qt::AutoConnection,
			Q_ARG(const QString&, message), Q_ARG(const QString&, type),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity)),
			Q_ARG(bool, terminate));
	}
}

/* ------------------------------------------------------------------- */

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

/* ------------------------------------------------------------------- */

void DeepSkyStacker::connectSignalsToSlots()
{
	connect(helpShortCut, &QShortcut::activated, this, &DeepSkyStacker::help);
	connect(explorerBar, &ExplorerBar::addImages, stackingDlg, &DSS::StackingDlg::onAddImages);

	connect(explorerBar, &ExplorerBar::loadList, stackingDlg, static_cast<void(DSS::StackingDlg::*)(const QPoint&)>(&DSS::StackingDlg::loadList));
	connect(explorerBar, &ExplorerBar::saveList, stackingDlg, static_cast<void(DSS::StackingDlg::*)()>(&DSS::StackingDlg::saveList));
	connect(explorerBar, &ExplorerBar::clearList, stackingDlg, &DSS::StackingDlg::clearList);

	connect(explorerBar, &ExplorerBar::checkAbove, stackingDlg, &DSS::StackingDlg::checkAbove);
	connect(explorerBar, &ExplorerBar::checkAll, stackingDlg, &DSS::StackingDlg::checkAll);
	connect(explorerBar, &ExplorerBar::unCheckAll, stackingDlg, &DSS::StackingDlg::unCheckAll);

	connect(explorerBar, &ExplorerBar::registerCheckedImages, stackingDlg, &DSS::StackingDlg::registerCheckedImages);
	connect(explorerBar, &ExplorerBar::computeOffsets, stackingDlg, &DSS::StackingDlg::computeOffsets);
	connect(explorerBar, &ExplorerBar::stackCheckedImages, stackingDlg, &DSS::StackingDlg::stackCheckedImages);
	connect(explorerBar, &ExplorerBar::batchStack, stackingDlg, &DSS::StackingDlg::batchStack);

	connect(this, &DeepSkyStacker::tabChanged, explorerBar, &ExplorerBar::tabChanged);
}

void DeepSkyStacker::closeEvent(QCloseEvent* e)
{
	ZFUNCTRACE_RUNTIME();
	if (false == processingDlg->SaveOnClose())
	{
		e->ignore();
		return;
	}
	processingDlg->DestroyWindow();
	if (false == stackingDlg->saveOnClose())
	{
		e->ignore();
		return;
	}
	e->accept();

	//
	// DSS is now closing, tell the two dock widgets that they must now accept
	// close event requests otherwise DSS never closes down.
	//
	explorerBar->setDSSClosing();
	pictureList->setDSSClosing();

	ZTRACE_RUNTIME("Saving Window State and Position");

	QSettings settings;
	settings.beginGroup("MainWindow");
	auto geometry{ saveGeometry() };
	settings.setValue("geometry", geometry);
#ifndef NDEBUG	
	ZTRACE_RUNTIME("Hex dump of geometry:");
	ZTrace::dumpHex(geometry.constData(), geometry.length());
#endif 

	auto windowState{ saveState()};
	settings.setValue("windowState", windowState);
#ifndef NDEBUG	
	ZTRACE_RUNTIME("Hex dump of windowState:");
	ZTrace::dumpHex(windowState.constData(), windowState.length());
#endif
	settings.setValue("maximised", isMaximized());

	settings.endGroup();
	QTableView* tableView = this->findChild<QTableView*>("tableView");
	settings.setValue("Dialogs/PictureList/TableView/HorizontalHeader/windowState",
		tableView->horizontalHeader()->saveState());
	settings.sync();
};


GdiplusStartupOutput gdiSO;
ULONG_PTR gdiplusToken{ 0ULL };
ULONG_PTR gdiHookToken{ 0ULL };

void DeepSkyStacker::disableSubDialogs()
{
	stackingDlg->setEnabled(false);
	processingDlg->EnableWindow(false);
	//m_dlgLibrary.EnableWindow(false);
	explorerBar->setEnabled(false);
}

void DeepSkyStacker::enableSubDialogs()
{
	stackingDlg->setEnabled(true);
	processingDlg->EnableWindow(true);
	//m_dlgLibrary.EnableWindow(true);
	explorerBar->setEnabled(true);
}

CDSSSettings& DeepSkyStacker::imageProcessingSettings()
{
	if (!m_ImageProcessingSettings->IsLoaded())
		m_ImageProcessingSettings->Load();

	return *m_ImageProcessingSettings.get();
}

DSS::StackingDlg& DeepSkyStacker::getStackingDlg()
{
	return *stackingDlg;
}

CProcessingDlg& DeepSkyStacker::getProcessingDlg()
{
	return *processingDlg.get();
}

CDeepStack& DeepSkyStacker::deepStack()
{
	return *m_DeepStack.get();
}

QString DeepSkyStacker::statusMessage()
{
	return statusBarText->text();
}

void DeepSkyStacker::setTab(std::uint32_t dwTabID)
{
	if (dwTabID == IDD_STACKING)
		dwTabID = IDD_REGISTERING;
	currTab = dwTabID;
	updateTab();
	emit tabChanged();
}

ExplorerBar& DeepSkyStacker::GetExplorerBar()
{
	return *explorerBar;
}

void DeepSkyStacker::setWindowFilePath(const QString& name)
{
	if (currentPathName == name) return;
	currentPathName = name;
	if (!name.isEmpty())
		setWindowTitle(QString("%1 - %2").arg(baseTitle).arg(name));
	else
		setWindowTitle(baseTitle);
}

void DeepSkyStacker::updateTab()
{
	switch (currTab)
	{
	case IDD_REGISTERING:
	case IDD_STACKING:
		stackedWidget->setCurrentIndex(0);
		stackingDlg->showImageList();
		stackingDlg->update();
		break;
	case IDD_PROCESSING:
		stackedWidget->setCurrentIndex(1);
		stackingDlg->showImageList(false);
		processingDlg->ShowWindow(SW_SHOW);
		break;
	};
	explorerBar->update();
};

/* ------------------------------------------------------------------- */
/* Slots                                                               */
/* ------------------------------------------------------------------- */

void DeepSkyStacker::help()
{
	ZFUNCTRACE_RUNTIME();
	QString helpFile{ QCoreApplication::applicationDirPath() + "/" + tr("DeepSkyStacker Help.chm","IDS_HELPFILE") };

	::HtmlHelp(::GetDesktopWindow(), helpFile.toStdWString().c_str(), HH_DISPLAY_TOPIC, 0);
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate)
{
	QMessageBox msgBox{ icon, "DeepSkyStacker", message, QMessageBox::Ok , this };
	msgBox.exec();
	if (terminate) QCoreApplication::exit(1);
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::qErrorMessage(const QString& message, const QString& type, QMessageBox::Icon icon, bool terminate)
{
	//
	// Hack to access the Icon displayed by QErrorMessage
	//
	if (nullptr == eMDI)
	{
		eMDI = errorMessageDialog->findChild<QLabel*>();
	}

	if (eMDI != nullptr)
	{
		switch (icon)
		{
		case (QMessageBox::Information):
			eMDI->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxInformation));
			break;
		case (QMessageBox::Critical):
			eMDI->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxCritical));
			break;
		case (QMessageBox::Warning):
		default:
			eMDI->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxWarning));
			break;
		}
	}
	errorMessageDialog->showMessage(message, type);
	if (terminate) QCoreApplication::exit(1);
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::updateStatus(const QString& text)
{
	statusBarText->setText(text);
}

BOOL DeepSkyStackerApp::InitInstance()
{
	ZFUNCTRACE_RUNTIME();
	CWinApp::InitInstance();

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
	STARMASKFILE_FILTERS.LoadString(IDS_FILTER_MASK);

	return TRUE;
};

int DeepSkyStackerApp::ExitInstance()
{
	ZFUNCTRACE_RUNTIME();

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

DeepSkyStackerApp theApp;

DeepSkyStackerApp *		GetDSSApp()
{
	return &theApp;
};

using namespace std;

std::unique_ptr<std::uint8_t[]> backPocket;
constexpr size_t backPocketSize{ 1024 * 1024 };

static char const* global_program_name;

//namespace
//{
//	void writeOutput(const char* text)
//	{
//		fputs(text, stderr);
//		ZTRACE_RUNTIME(text);
//	};
//
//#if defined(_WINDOWS)
//#define EXCEPTION_CASE(code) \
// case code: \
//  exceptionString = #code "\n"; \
//  break
//
//	DSSStackWalker sw;
//
//	LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS* ExceptionInfo)
//	{
//		const char* exceptionString = NULL;
//
//		switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
//		{
//		EXCEPTION_CASE(EXCEPTION_ACCESS_VIOLATION);
//		EXCEPTION_CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
//		EXCEPTION_CASE(EXCEPTION_BREAKPOINT);
//		EXCEPTION_CASE(EXCEPTION_DATATYPE_MISALIGNMENT);
//		EXCEPTION_CASE(EXCEPTION_FLT_DENORMAL_OPERAND);
//		EXCEPTION_CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
//		EXCEPTION_CASE(EXCEPTION_FLT_INEXACT_RESULT);
//		EXCEPTION_CASE(EXCEPTION_FLT_INVALID_OPERATION);
//		EXCEPTION_CASE(EXCEPTION_FLT_OVERFLOW);
//		EXCEPTION_CASE(EXCEPTION_FLT_STACK_CHECK);
//		EXCEPTION_CASE(EXCEPTION_FLT_UNDERFLOW);
//		EXCEPTION_CASE(EXCEPTION_ILLEGAL_INSTRUCTION);
//		EXCEPTION_CASE(EXCEPTION_IN_PAGE_ERROR);
//		EXCEPTION_CASE(EXCEPTION_INT_DIVIDE_BY_ZERO);
//		EXCEPTION_CASE(EXCEPTION_INT_OVERFLOW);
//		EXCEPTION_CASE(EXCEPTION_INVALID_DISPOSITION);
//		EXCEPTION_CASE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
//		EXCEPTION_CASE(EXCEPTION_PRIV_INSTRUCTION);
//		EXCEPTION_CASE(EXCEPTION_SINGLE_STEP);
//		EXCEPTION_CASE(EXCEPTION_STACK_OVERFLOW);
//		case 0xE06D7363:
//			exceptionString = "Unhandled C++ Exception ...\n";
//			break;
//		default:
//			exceptionString = "Error: Unrecognized Exception\n";
//			break;
//		}
//		writeOutput(exceptionString);
//		fflush(stderr);
//		/* If this is a stack overflow then we can't walk the stack, so just show
//		  where the error happened */
//		if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode)
//		{
//			sw.ShowCallstack();
//		}
//		else
//		{
//			char buffer[128]{};
//			snprintf(buffer, sizeof(buffer), "Stack Overflow Exception address: %p\n", (void*)ExceptionInfo->ContextRecord->Rip);
//			writeOutput(buffer);
//		}
//		DeepSkyStacker::instance()->close();
//		return EXCEPTION_EXECUTE_HANDLER;
//	}
//
//	LONG WINAPI RedirectedSetUnhandledExceptionFilter(EXCEPTION_POINTERS* /*ExceptionInfo*/)
//	{
//		// When the CRT calls SetUnhandledExceptionFilter with NULL parameter, our handler will not get removed.
//		return 0;
//	}
//#else
//	/* Resolve symbol name and source location given the path to the executable
//	   and an address */
//	int addr2line(char const* const program_name, void const* const addr)
//	{
//		char addr2line_cmd[512] { 0 };
//
//		/* have addr2line map the address to the relevant line in the code */
//	#ifdef __APPLE__
//	  /* apple does things differently... */
//		sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
//	#else
//		sprintf(addr2line_cmd, "addr2line -f -p -e %.256s %p", program_name, addr);
//	#endif
//
//		/* This will print a nicely formatted string specifying the
//		   function and source line of the address */
//		FILE* in;
//		char buff[512];
//		// is this the check for command execution exited with not 0?
//		if (!(in = popen(addr2line_cmd, "r"))) {
//			// I want to return the exit code and error message too if any
//			return 1;
//		}
//		// this part echoes the output of the command that's executed
//		while (fgets(buff, sizeof(buff), in) != NULL)
//		{
//			writeOutput(buff);
//		}
//		return WEXITSTATUS(pclose(in));
//	}
//
//	constexpr size_t MAX_STACK_FRAMES{ 64 };
//	static void* stack_traces[MAX_STACK_FRAMES];
//	void posix_print_stack_trace()
//	{
//		int i, trace_size = 0;
//		char** messages = (char**)NULL;
//		char buffer[1024]{};	// buffer for error message
//
//
//		trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
//		messages = backtrace_symbols(stack_traces, trace_size);
//
//		/* skip the first couple stack frames (as they are this function and
//		   our handler) and also skip the last frame as it's (always?) junk. */
//		   // for (i = 3; i < (trace_size - 1); ++i)
//		   // we'll use this for now so you can see what's going on
//		for (i = 0; i < trace_size; ++i)
//		{
//			if (addr2line(global_program_name, stack_traces[i]) != 0)
//			{
//				snprintf(buffer, sizeof(buffer)/sizeof(char),
//					"  error determining line # for: %s\n", messages[i]);
//				writeOutput(buffer);
//			}
//
//		}
//		if (messages) { free(messages); }
//	}
//
//	void signalHandler(int signal)
//	{
//		if (backPocket)
//		{
//			free(backPocket);
//			backPocket = nullptr;
//		}
//
//		char name[8]{};
//		switch (signal)
//		{
//		case SIGINT:
//			strcpy(name, "SIGINT");
//			break;
//		case SIGILL:
//			strcpy(name, "SIGILL");
//			break;
//		case SIGFPE:
//			strcpy(name, "SIGFPE");
//			break;
//		case SIGSEGV:
//			strcpy(name, "SIGSEGV");
//			break;
//		case SIGTERM:
//			strcpy(name, "SIGTERM");
//			break;
//		default:
//			snprintf(name, sizeof(name)/sizeof(char), "%d", signal);
//		}
//
//		ZTRACE_RUNTIME("In signalHandler(%s)", name);
//
//		posix_print_stack_trace();
//		DeepSkyStacker::instance()->close();
//	}
//#endif
//}

void reportCpuType();

bool LoadTranslationUnit(QApplication& app, QTranslator& translator, const char* prefix, const QString& path, const QString& language)
{
	QString translatorFileName(prefix);
	translatorFileName += (language == "") ? QLocale::system().name() : language;

	qDebug() << "Loading translator file [" << translatorFileName << "] from path: " << path;
	if (!translator.load(translatorFileName, path))
	{
		qDebug() << " *** Failed to load file [" << translatorFileName << "] into translator";
		return false;
	}
	
	if(!app.installTranslator(&translator))
	{
		qDebug() << " *** Failed to install translator for file [" << translatorFileName << "]";
		return false;
	}
	return true;
}

bool LoadTranslations()
{
	if (!qApp)
		return false;

	static QTranslator theQtTranslator;
	static QTranslator theAppTranslator;
	static QTranslator theKernelTranslator;

	Q_INIT_RESOURCE(translations_kernel);

	// Try to load each language file - allow failures though (due to issue with ro and reloading en translations)
	QSettings settings;
	const QString language = settings.value("Language").toString();
	LoadTranslationUnit(*qApp, theQtTranslator, "qt_", QLibraryInfo::path(QLibraryInfo::TranslationsPath), language);
	LoadTranslationUnit(*qApp, theAppTranslator, "DSS_", ":/i18n/", language);
	LoadTranslationUnit(*qApp, theKernelTranslator, "DSSKernel_", ":/i18n/", language);
	
	return true;
}

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();

#if defined(_WINDOWS)
	// Set console code page to UTF-8 so console known how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

#ifndef NDEBUG
	//
	// If this is a debug build, log Qt messages to the trace file as well as to the debugger.
	//
	originalHandler = qInstallMessageHandler(qtMessageLogger);
#endif

	//
	// Save the program name in case we need it later
	// 
	global_program_name = argv[0];

	//
	// Create a storage cushion (aka back pocket storage)
	// and ensure that it is actually touched.
	//
	backPocket = std::make_unique<std::uint8_t[]>(backPocketSize);
	for (auto* p = backPocket.get(); p < backPocket.get() + backPocketSize; p += 4096)
	{
		*p = static_cast<uint8_t>('\xff');
	}

	int result{ 0 };

	//
	// Silence the MFC memory leak dump as we use Visual Leak Detector.
	//
#if defined(_WINDOWS)
	_CrtSetDbgFlag(0);
#if !defined(NDEBUG)
	AfxEnableMemoryLeakDump(false);
#endif
#endif

//#if defined(_WINDOWS)

//#else
	//
	// Set up to handle signals
	//
//	std::signal(SIGINT, signalHandler);
//	std::signal(SIGILL, signalHandler);
//	std::signal(SIGFPE, signalHandler);
//	std::signal(SIGSEGV, signalHandler);
//	std::signal(SIGTERM, signalHandler);
//#endif

	//QMfcApp app(&theApp, argc, argv);
	QApplication app(argc, argv);

	if (hasExpired())
		return FALSE;

	ZTRACE_RUNTIME("Initialize MFC");
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(nullptr), nullptr, ::GetCommandLine(), 0))
	{
		ZTRACE_RUNTIME("Fatal Error: MFC initialization failed");
		QString errorMessage{ "Fatal Error: MFC initialization failed" };
		cerr << errorMessage.toUtf8().constData() << endl;
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
		return 1;
	}
	// initialize all the windows stuff we need for now
	theApp.InitInstance();

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
	LoadTranslations();

	//
	// Do the old Windows language stuff
	//
	SetUILanguage();

	reportCpuType();

	//
	// Set things up to capture terminal errors
	//
	setDssExceptionHandling();

	askIfVersionCheckWanted();

	//
	// Register PICTURETYPE and QMessageBox::Icon enums as meta types
	//
	qRegisterMetaType<PICTURETYPE>();
	qRegisterMetaType<QMessageBox::Icon>();

	//
	// Increase maximum size of QImage from the default of 128MB to 1GB
	//
	constexpr int oneGB{ 1024 * 1024 * 1024 };
	QImageReader::setAllocationLimit(oneGB);

	ZTRACE_RUNTIME("Invoking QApplication::exec()");
	try
	{
		ZTRACE_RUNTIME("Creating Main Window");
		DeepSkyStacker mainWindow;

		QEventLogger eventLogger("C:/temp/DSSEvents", &mainWindow, false);
		app.installEventFilter(&eventLogger);

		ZTRACE_RUNTIME("Checking Mutex");
		//
		// Get the name of the writable local application data directory
		// and create the directories if necessary
		//
		QString mutexFileName{ QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) };
		fs::path file{ mutexFileName.toStdU16String() };
		create_directories(file);

		//
		// Append the file name to the directory name
		//
		mutexFileName += "/DeepSkyStacker.Interprocess.Mutex";

		//
		// Create the file if it doesn't exist.  It is intentionally never deleted.
		//
#ifdef _WINDOWS
		auto newFile = std::ofstream(mutexFileName.toStdWString().c_str());
#else
		auto newFile = std::ofstream(mutexFileName.toUtf8().constData());
#endif

		//
		// Use a boost::interprocess::file_lock as unlike a named_mutex, the OS removes the lock in the case of abnormal termination
		//
#ifdef _WINDOWS
		bip::file_lock dssMutex{ mutexFileName.toStdWString().c_str() };
#else
		bip::file_lock dssMutex{ mutexFileName.toUtf8().constData() };
#endif
		bip::scoped_lock<bip::file_lock> lock(dssMutex, bip::try_to_lock);
		const bool firstInstance{ lock.owns() };
		ZTRACE_RUNTIME("  firstInstance: %s", firstInstance ? "true" : "false");

		if (firstInstance)
			deleteRemainingTempFiles();

		Exiv2::XmpParser::initialize();
		::atexit(Exiv2::XmpParser::terminate);

		mainWindow.show();
		//result = app.run(&theApp);
		result = app.exec();
	}
	catch (bip::interprocess_exception& e)
	{
		ZTRACE_RUNTIME("boost::interprocess_exception caught: %s", e.what());
		traceControl.setDeleteOnExit(false);
		QString errorMessage(e.what());
#if defined(_CONSOLE)
		std::cerr << errorMessage.toUtf8().constData();
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif

	}
	catch (std::exception& e)
	{
		ZTRACE_RUNTIME("std::exception caught: %s", e.what());
		traceControl.setDeleteOnExit(false);
		QString errorMessage(e.what());
#if defined(_CONSOLE)
		std::cerr << errorMessage.toUtf8().constData();
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif
	}
	catch (CException& e)
	{
		traceControl.setDeleteOnExit(false);
		constexpr unsigned int msglen{ 255 };
		TCHAR message[msglen]{ 0x00 };
		e.GetErrorMessage(&message[0], msglen);
		ZTRACE_RUNTIME("CException caught: %s", (LPCSTR)CT2CA(message));

		e.ReportError();
		e.Delete();
	}
	catch (ZException& ze)
	{
		traceControl.setDeleteOnExit(false);

		ZTRACE_RUNTIME("ZException %s thrown from: %s Function: %s() Line: %d\n\n%s",
			ze.name(),
			ze.locationAtIndex(0)->fileName(),
			ze.locationAtIndex(0)->functionName(),
			ze.locationAtIndex(0)->lineNumber(),
			ze.text(0));

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
		std::cerr << errorMessage.toUtf8().constData();
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif
	}
#if (0)
	catch (...)
	{
		ZTRACE_RUNTIME("Unknown exception caught");

		QString errorMessage("Unknown exception caught");
#if defined(_CONSOLE)
		std::cerr << errorMessage.toUtf8().constData();
#else
		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
#endif

	}
#endif
	theApp.ExitInstance();
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