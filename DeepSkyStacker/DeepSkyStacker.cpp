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
#include "pch.h"
#if defined(Q_OS_WIN) && !defined(NDEBUG) && __has_include(<vld.h>)
//
// Visual Leak Detector
//
#include <vld.h>
#endif

#if defined(Q_OS_WIN)
#include <htmlhelp.h>
#endif

#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/exceptions.hpp>

#include <locale>
#include <fstream>

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
#include <signal.h>
#endif

namespace bip = boost::interprocess;

#include "avx_simd_check.h"
#include "DeepSkyStacker.h"
#include "ui_StackingDlg.h"
#include "ztrace.h"
#include "StackingTasks.h"
#include "StackingDlg.h"
#include "ExplorerBar.h"
#include "lowerdockwidget.h"
#include "picturelist.h"
#include "processingcontrols.h"
#include "ProcessingDlg.h"
#include "ExceptionHandling.h"
#include "DeepStack.h"
#include "tracecontrol.h"
#include "Workspace.h"
#include "QEventLogger.h"
#include "QMessageLogger.h"
#include "Multitask.h"

bool	g_bShowRefStars = false;

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ std::source_location::current().file_name() };


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
	lowerDockWidget{ nullptr },
	explorerBar{ nullptr },
	stackedWidget{ nullptr },
	stackingDlg{ nullptr },
	activePanel{ ActivePanel::StackingPanel },
	args{ qApp->arguments() },
	// m_taskbarList{ nullptr },
	baseTitle{ QString("DeepSkyStacker %1").arg(VERSION_DEEPSKYSTACKER) },
	m_progress{ false },
	sponsorText{ new QLabel("") },
	statusBarText{ new QLabel("") },
	m_DeepStack{ std::make_unique<CDeepStack>() },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr },		// errorMessageDialogIcon pointer
	helpShortCut{ new QShortcut(QKeySequence::HelpContents, this) }
{
	ZFUNCTRACE_RUNTIME();
	DSSBase::setInstance(this);
	ZTRACE_RUNTIME(baseTitle);		// Report version in the trace file
	
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
	explorerBar->setFeatures(QDockWidget::DockWidgetFloatable);		// Can't be closed or moved
	addDockWidget(Qt::LeftDockWidgetArea, explorerBar);

	ZTRACE_RUNTIME("Creating lower dock widget");
	lowerDockWidget = new DSS::LowerDockWidget(this);
	lowerDockWidget->setFeatures(									// Can't be closed or moved
		QDockWidget::DockWidgetFeature::DockWidgetFloatable |
		QDockWidget::DockWidgetFeature::DockWidgetMovable);
	lowerDockWidget->setAllowedAreas(
		Qt::DockWidgetArea::BottomDockWidgetArea |
		Qt::DockWidgetArea::RightDockWidgetArea);
	addDockWidget(Qt::BottomDockWidgetArea, lowerDockWidget);

	ZTRACE_RUNTIME("Creating picture list");
	pictureList = new DSS::PictureList(this);
	lowerDockWidget->addWidget(pictureList);

	ZTRACE_RUNTIME("Creating processing controls");
	processingControls = new DSS::ProcessingControls(this);
	lowerDockWidget->addWidget(processingControls);

	ZTRACE_RUNTIME("Creating stackedWidget");
	stackedWidget = new QStackedWidget(this);
	stackedWidget->setObjectName("stackedWidget");
	setCentralWidget(stackedWidget);

	ZTRACE_RUNTIME("Creating Stacking Panel");
	stackingDlg = new DSS::StackingDlg(this, pictureList);
	stackingDlg->setObjectName("stackingDlg");

	ZTRACE_RUNTIME("Adding Stacking Panel to stackedWidget");
	stackedWidget->addWidget(stackingDlg);
	
	ZTRACE_RUNTIME("Creating Processing Panel");
	processingDlg = new DSS::ProcessingDlg(this, processingControls);
	processingDlg->setObjectName("processingDlg");

	ZTRACE_RUNTIME("Adding Processing Panel to stackedWidget");
	stackedWidget->addWidget(processingDlg);

	stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
	// Set initial size of the bottom dock widget (lowerDockWidget)
	//
	resizeDocks({ lowerDockWidget }, { 150 }, Qt::Vertical);

	ZTRACE_RUNTIME("Restoring Window State and Position");
	QSettings settings;
	settings.beginGroup("MainWindow");

	if (settings.contains("geometry"))
	{
		const QByteArray geometry{ settings.value("geometry").toByteArray() };

#if QT_VERSION < 0x060601
		// Shouldn't need this in 6.6.1 or above
		if (settings.contains("maximised"))
		{
			const bool maximised{ settings.value("maximised").toBool() };

			if (maximised)
			{
				setGeometry(screen()->availableGeometry());
				showMaximized();
			}
		}
		else
		{
			restoreGeometry(geometry);
		}
#else
		restoreGeometry(geometry);
#endif

	}

	if (settings.contains("windowState"))
	{
		auto windowState{ settings.value("windowState").toByteArray() };
		restoreState(windowState);
	}

	settings.endGroup();
}

void DeepSkyStacker::createStatusBar()
{
	setSponsorText();

	statusBarText->setAlignment(Qt::AlignHCenter);

	statusBar()->addPermanentWidget(sponsorText, 0);
	statusBar()->addWidget(statusBarText, 1);
	connect(stackingDlg, &DSS::StackingDlg::statusMessage, this, &DeepSkyStacker::updateStatus);
	connect(stackingDlg, &DSS::StackingDlg::setDockTitle, lowerDockWidget, &DSS::LowerDockWidget::setDockTitle);

}

void DeepSkyStacker::reportError(const QString& message, const QString& type, Severity severity,
	Method method, bool terminate)
{
	if (terminate) traceControl.setDeleteOnExit(false);
	if (Method::QMessageBox == method)
	{
		QMetaObject::invokeMethod(this, "qMessageBox", Qt::ConnectionType::AutoConnection,
			Q_ARG(const QString&, message),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity)),
			Q_ARG(bool, terminate));
	}
	else
	{
		QMetaObject::invokeMethod(this, "qErrorMessage", Qt::ConnectionType::AutoConnection,
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

	connect(this, &DeepSkyStacker::panelChanged, explorerBar, &ExplorerBar::panelChanged);
	connect(this, &DeepSkyStacker::panelChanged, lowerDockWidget, &LowerDockWidget::panelChanged);
}

void DeepSkyStacker::changeEvent(QEvent* e)
{
	if (e->type() == QEvent::LanguageChange) {
		setSponsorText();
	}
	else {
		QWidget::changeEvent(e);
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::setSponsorText()
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
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::closeEvent(QCloseEvent* e)
{
	ZFUNCTRACE_RUNTIME();
	if (false == processingDlg->saveOnClose())
	{
		e->ignore();
		return;
	}
	if (false == stackingDlg->saveOnClose())
	{
		e->ignore();
		return;
	}
	e->accept();

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
	//
	// On Linux and macOS, we need to close the help process if it is running
	// as it will not close down automatically.
	//
	if (helpProcess && QProcess::NotRunning != helpProcess->state())
	{
		ZTRACE_RUNTIME("Closing help process");
		helpProcess->close();
	}
#endif // defined(Q_OS_LINUX) || defined(Q_OS_MACOS)


#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
	//
	// Colossal Cave is now closing, tell the two dock widgets that they must now accept
	// close event requests otherwise DSS never closes down.
	//
	explorerBar->setDSSClosing();
	pictureList->setDSSClosing();
#endif

	ZTRACE_RUNTIME("Saving Window State and Position");

	//
	// Before saving the window state which saves the state the dock widgets, it is necessary to 
	// make the image list dock widget visible.   As we want the stacking dialog to be visible 
	// when opening again, make that visible too.
	//
	stackedWidget->setCurrentIndex(static_cast<int>(ActivePanel::StackingPanel));
	stackingDlg->showImageList();

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
	settings.setValue("Dialogs/PictureList/TableView/HorizontalHeader/windowState", tableView->horizontalHeader()->saveState());
	settings.setValue("Dialogs/PictureList/TableView/HorizontalHeader/numberOfColumns", StackingDlg::getNumberOfTableViewColumns(tableView));
	settings.sync();
}

void DeepSkyStacker::disableSubDialogs()
{
	stackingDlg->setEnabled(false);
	processingDlg->setEnabled(false);
	//m_dlgLibrary.EnableWindow(false);
	explorerBar->setEnabled(false);
}

void DeepSkyStacker::enableSubDialogs()
{
	stackingDlg->setEnabled(true);
	processingDlg->setEnabled(true);
	//m_dlgLibrary.EnableWindow(true);
	explorerBar->setEnabled(true);
}

DSS::StackingDlg& DeepSkyStacker::getStackingDlg()
{
	return *stackingDlg;
}

DSS::ProcessingDlg& DeepSkyStacker::getProcessingDlg()
{
	return *processingDlg;
}

CDeepStack& DeepSkyStacker::deepStack()
{
	return *m_DeepStack.get();
}

QString DeepSkyStacker::statusMessage()
{
	return statusBarText->text();
}

void DeepSkyStacker::setPanel(ActivePanel panel)
{
	activePanel = panel;
	updatePanel ();
	emit panelChanged(panel);
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

void DeepSkyStacker::updatePanel()
{
	//
	// Switch to the selected panel
	//
	stackedWidget->setCurrentIndex(static_cast<int>(activePanel));

	switch (activePanel)
	{
	case ActivePanel::StackingPanel:
		stackingDlg->showImageList();
		stackingDlg->update();
		break;
	case ActivePanel::ProcessingPanel:
		stackingDlg->showImageList(false);
		processingDlg->update();
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
	explorerBar->setHelpEnabled(false);
	QString appPath{ QCoreApplication::applicationDirPath() };
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
	QString helpFile{ appPath + "/Help/" + tr("DeepSkyStacker Help.chm","IDS_HELPFILE") };
#elif defined(Q_OS_MACOS)
	QString helpFile{ appPath + "/../Resources/" + tr("DeepSkyStacker Help.chm","IDS_HELPFILE") };
#endif

#if defined(Q_OS_LINUX)
	// On Linux, we use the kchmviewer application to display the help file
	QString program{ "kchmviewer" };
#elif defined(Q_OS_MACOS)
	// On macOS, we use the uchmviewer application to display the help file
	// which we've bundled with the application
	QString program{ appPath + "/uchmviewer" };
#endif

#if defined(Q_OS_WIN)
	::HtmlHelp(::GetDesktopWindow(), helpFile.toStdWString().c_str(), HH_DISPLAY_TOPIC, 0);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
	//
	// On Linux and macOS, we use a QProcess to start the help viewer
	//	
	if (!helpProcess)
	{
		helpProcess = new QProcess(this);
		connect(helpProcess, &QProcess::finished, this, [this]
			{
				helpProcess->deleteLater();
				helpProcess = nullptr;
			});
	}
	QStringList arguments{ "-token", "com.github.deepskystacker", helpFile };
	if (QProcess::NotRunning == helpProcess->state())
	{
		//
		// Start the help display program (kchmviewer or uchmviewer)
		//
		helpProcess->start(program, arguments);
		if(!helpProcess->waitForStarted())
		{
			qWarning() << "Failed to start help process:" << helpProcess->errorString();
			return;
		}
	}
#endif
	explorerBar->setHelpEnabled(true);
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate)
{
	QMessageBox msgBox{ icon, "DeepSkyStacker", message, QMessageBox::Ok , this };
	msgBox.exec();
	if (terminate)
	{
		// QCoreApplication::exit(1);
		QMetaObject::invokeMethod(QCoreApplication::instance(), "exit", Qt::QueuedConnection,
			Q_ARG(int, 1));
	}
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
	if (terminate)
	{
		// QCoreApplication::exit(1);
		QMetaObject::invokeMethod(QCoreApplication::instance(), "exit", Qt::QueuedConnection,
			Q_ARG(int, 1));
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStacker::updateStatus(const QString& text)
{
	statusBarText->setText(text);
}

using namespace std;

std::unique_ptr<std::uint8_t[]> backPocket;
constexpr size_t backPocketSize{ 1024 * 1024 };

char const* global_program_name;

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

	Q_INIT_RESOURCE(DeepSkyStackerKernel_translations);

	// Try to load each language file - allow failures though (due to issue with ro and reloading en translations)
	QSettings settings;
	const QString language = settings.value("Language").toString();
	LoadTranslationUnit(*qApp, theQtTranslator, "qt_", QLibraryInfo::path(QLibraryInfo::TranslationsPath), language);
	LoadTranslationUnit(*qApp, theAppTranslator, "DeepSkyStacker_", ":/i18n/", language);
	LoadTranslationUnit(*qApp, theKernelTranslator, "DeepSkyStackerKernel_", ":/i18n/", language);
	
	return true;
}

void atexitHandler()
{
	//
	// Delete the back pocket storage
	//
	backPocket.reset();
}

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();
	//
	// Set up the atexit handler to release our back-pocket storage
	//
	std::atexit(atexitHandler);

#if defined(Q_OS_WIN)
	// Set the C character locale for UTF-8 so Exiv2 can open files with UTF-8 names
	// I think this also applies to the use of regular fopen() calls.
	std::setlocale(LC_CTYPE, ".UTF-8");

	// Set console code page to UTF-8 so console knowns how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

	//
	// Log Qt messages to the trace file as well as to the debugger.
	//
	originalHandler = qInstallMessageHandler(qtMessageLogger);

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
	// Silence the windows heap checker as we use Visual Leak Detector
	//
#if defined(Q_OS_WIN)
	_CrtSetDbgFlag(0);
#endif

	QApplication app(argc, argv);

	if (hasExpired())
		return 1;

	//
	// Set up organisation etc. for QSettings usage
	//
	QCoreApplication::setOrganizationName("DeepSkyStacker");
	QCoreApplication::setOrganizationDomain("deepskystacker.free.fr");
	QCoreApplication::setApplicationName("DeepSkyStacker5");

	//
	// Set up retention of the trace based on user settings
	//
	QSettings settings;
	auto retainTrace{ settings.value("RetainTraceFile", false).toBool() };
	traceControl.setDeleteOnExit(!retainTrace);

	g_bShowRefStars = settings.value("ShowRefStars", false).toBool();

	//
	// Set the Qt Application Style
	//
	app.setStyle(QStyleFactory::create("Fusion"));

	ZTRACE_RUNTIME("Initialize Application - ok");

	ZTRACE_RUNTIME("Set UI Language");
	LoadTranslations();

	AvxSimdCheck::reportCpuType();

#if !defined(Q_OS_APPLE)
	//
	// Set things up to capture terminal errors
	//
	setDssExceptionHandling();
#endif

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

	//
	// Set the maximum number of threads we're allowed to use
	//
	const auto processorCountSetting = QSettings{}.value("MaxProcessors", uint{ 0 }).toUInt();
	Multitask::setMaxProcessors(processorCountSetting);

	ZTRACE_RUNTIME("Invoking QApplication::exec()");
	try
	{
		ZTRACE_RUNTIME("Creating Main Window");
		DeepSkyStacker mainWindow;

		//QEventLogger eventLogger("Qt-Event-log", &mainWindow, false);
		//app.installEventFilter(&eventLogger);

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
#if defined (Q_OS_WIN)
		auto newFile = std::ofstream(mutexFileName.toStdWString().c_str());
#else
		auto newFile = std::ofstream(mutexFileName.toUtf8().constData());
#endif

		//
		// Use a boost::interprocess::file_lock as unlike a named_mutex, the OS removes the lock in the case of abnormal termination
		//
#if defined(Q_OS_WIN)
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

	return result;
}