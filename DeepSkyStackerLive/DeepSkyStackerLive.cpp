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
// DeepSkyStackerLive.cpp : Defines the class behaviors for the application.
//

#include <stdafx.h>
#include <source_location>

#include <QErrorMessage>
#include <QMessageBox>
#include <QImageReader>

//
// Necessary Windows header
//
#if defined(Q_OS_WIN)
#include <afx.h>
#endif

#include "avx_support.h"
#include "DeepSkyStackerLive.h"
#include "DSSVersion.h"
#include "ExceptionHandling.h"
#include "LiveSettings.h"
#include <zexcept.h>
#include <ztrace.h>
#include "./../DeepSkyStacker/SetUILanguage.h"	// Explicit include so not to pull over all headers in DSS if we added just a new include path.
#include "tracecontrol.h"

using namespace DSS;
using namespace std;

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ source_location::current().file_name() };

bool	g_bShowRefStars = false;

void reportCpuType();

namespace
{
	//
	// Comvert a QLabel with "plain text" to a hyperlink
	//
	static void makeLink(QLabel* label, QString color, QString text)
	{
		label->setText(QString("<a href='.' style='text-decoration: none; color: %1'>%2</a>").arg(color, text));
	}

	static void makeLink(QLabel* label, QString color)
	{
		makeLink(label, color, label->text());
	}

	bool hasExpired()
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
			QString message = QCoreApplication::translate("DeepSkyStackerLive",
				"This beta version of DeepSkyStacker has expired\nYou can probably get another one or download the final release from the web site.");
			QMessageBox::critical(nullptr, "DeepSkyStacker",
				message, QMessageBox::Ok);
			bResult = true;
		};

		ZTRACE_RUNTIME("Check beta expiration - ok\n");

#endif
		return bResult;
	}
}

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

	if (!app.installTranslator(&translator))
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
	LoadTranslationUnit(*qApp, theAppTranslator, "DSS.", ":/i18n/", language);
	LoadTranslationUnit(*qApp, theKernelTranslator, "KernelDSS.", ":/i18n/", language);

	return true;
}

/* ------------------------------------------------------------------- */

DeepSkyStackerLive::DeepSkyStackerLive() :
	Ui_DeepSkyStackerLive {},
	liveSettings {make_unique<LiveSettings>()},
	initialised{ false },
	monitoring {false}, 
	winHost{ nullptr },
	args{ qApp->arguments() },
	baseTitle{ QString("DeepSkyStackerLive %1").arg(VERSION_DEEPSKYSTACKER) },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr },		// errorMessageDialogIcon pointer
	linkColour{ palette().color(QPalette::ColorRole::Link).name() },
	stackedImageViewer {nullptr},
	lastImageViewer {nullptr}
{
	//
	// Must set dssInstance before invoking setupUi 
	//
	dssInstance = this;
	setupUi(this);
}

DeepSkyStackerLive::~DeepSkyStackerLive()
{
}

void DeepSkyStackerLive::closeEvent(QCloseEvent* e)
{
	ZFUNCTRACE_RUNTIME();

	e->accept();

	ZTRACE_RUNTIME("Saving Window State and Position");

	QSettings settings;
	settings.beginGroup("MainWindow");
	auto geometry{ saveGeometry() };
	settings.setValue("geometry", geometry);
#ifndef NDEBUG	
	ZTRACE_RUNTIME("Hex dump of geometry:");
	ZTrace::dumpHex(geometry.constData(), geometry.length());
#endif 

	settings.endGroup();
	//QTableView* tableView = this->findChild<QTableView*>("tableView");
	//settings.setValue("Dialogs/PictureList/TableView/HorizontalHeader/windowState",
	//	tableView->horizontalHeader()->saveState());
	settings.sync();
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::showEvent(QShowEvent* event)
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

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::connectSignalsToSlots()
{
	connect(folderName, &QLabel::linkActivated,
		this, &DSSLive::setMonitoredFolder);
	connect(actionMonitor, &QAction::triggered,
		this, &DSSLive::monitorTriggered);
	connect(actionStack, &QAction::triggered,
		this, &DSSLive::stackTriggered);
	connect(actionStop, &QAction::triggered,
		this, &DSSLive::stopTriggered);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::makeLinks()
{
	makeLink(folderName, linkColour);
}

/* ------------------------------------------------------------------- */


void DeepSkyStackerLive::onInitialise()
{
	ZFUNCTRACE_RUNTIME();

	//
	// Connect Qt Signals to appropriate slots
	//
	connectSignalsToSlots();

	setWindowIcon(QIcon(":/DSSIcon.png"));

	setWindowTitle(baseTitle);

	//
	// Has the folder to be monitored already been set?
	// If so change the text in the display panel
	//
	QSettings settings;
	settings.beginGroup("DeepSkyStackerLive");
	QString dir{ settings.value("MonitoredFolder", "").toString() };
	if (!dir.isEmpty())
		folderName->setText(dir);
	settings.endGroup();

	//
	// Before any messages are written to the log, reduce the line spacing a bit
	// and reduce the font size by one
	//
	QTextBlockFormat bf = messageLog->textCursor().blockFormat();
	bf.setLineHeight(85, QTextBlockFormat::ProportionalHeight);
	messageLog->textCursor().setBlockFormat(bf);
	QFont font{ messageLog->currentFont() };
	font.setPointSize(font.pointSize() - 1); font.setWeight(QFont::Medium);
	messageLog->setFont(font);

	writeToLog(baseTitle, true, true);
	writeToLog("\n");


	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"\nHow to use  DeepSkyStacker Live ?\n", "IDS_LOG_STARTING"),
		false, true, false);
	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 1\nCheck the Settings tabs for all the stacking and warning settings\n\n", "IDS_LOG_STARTING_1"),
		false, false, false);
	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 2\nClick on the Monitor button to start monitoring the folder\n"
		"When monitoring is active incoming images are only registered but not stacked.\n\n", "IDS_LOG_STARTING_2"),
		false, false, false);
	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"Step 3\nTo start stacking the images click on the Stack button\n"
		"At this point all the incoming (and all previously registered) images will be stacked.\n", "IDS_LOG_STARTING_3"),
		false, false, false);
	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"You can pause/restart the stacking process by clicking on the Stack button.\n", "IDS_LOG_STARTING_4"),
		false, false, false);
	writeToLog(QCoreApplication::translate("DeepSkyStackerLive",
		"To stop monitoring and stacking click on the Stop button.\n\n", "IDS_LOG_STARTING_5"),
		false, false, false);

	makeLinks();

	ZTRACE_RUNTIME("Restoring Window State and Position");

	settings.beginGroup("DeepSkyStackerLive/MainWindow");

	auto geometry{ settings.value("geometry", QByteArray()).toByteArray() };

#ifndef NDEBUG
	if (geometry.length())
	{
		ZTRACE_RUNTIME("Hex dump of geometry:");
		ZTrace::dumpHex(geometry.constData(), geometry.length());
	}
#endif

	restoreGeometry(geometry);
	settings.endGroup();
}

void DeepSkyStackerLive::reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate)
{
	if (terminate) traceControl.setDeleteOnExit(false);
	if (Method::QMessageBox == method)
	{
		QMetaObject::invokeMethod(this, "qMessageBox", Qt::QueuedConnection,
			Q_ARG(const QString&, message),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity)),
			Q_ARG(bool, terminate));
	}
	else
	{
		QMetaObject::invokeMethod(this, "qErrorMessage", Qt::QueuedConnection,
			Q_ARG(const QString&, message), Q_ARG(const QString&, type),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity)),
			Q_ARG(bool, terminate));
	}
}

void DeepSkyStackerLive::writeToLog(const QString& message, bool addTS, bool bold, bool italic, QColor colour)
{
	QMetaObject::invokeMethod(this, "writeLogMessage", Qt::QueuedConnection,
		Q_ARG(const QString&, message),
		Q_ARG(bool, addTS),
		Q_ARG(bool, bold),
		Q_ARG(bool, italic),
		Q_ARG(QColor, colour));
}

/* ------------------------------------------------------------------- */
/* Slots                                                               */
/* ------------------------------------------------------------------- */
void DeepSkyStackerLive::qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate)
{
	QMessageBox msgBox{ icon, "DeepSkyStacker", message, QMessageBox::Ok , this };
	msgBox.exec();
	if (terminate) QCoreApplication::exit(1);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::qErrorMessage(const QString& message, const QString& type, QMessageBox::Icon icon, bool terminate)
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

void DeepSkyStackerLive::writeLogMessage(const QString& message, [[maybe_unused]] bool addTimeStamp, bool bold, bool italic, QColor colour)
{
	messageLog->moveCursor(QTextCursor::End);
	messageLog->setTextColor(colour);

	if (addTimeStamp)
	{
		QDateTime now{ QDateTime::currentDateTime() };
		QString ts{ now.toString("yyyy-MM-dd hh:mm:ss") };
		ts += " - ";
		messageLog->setFontWeight(QFont::Medium);
		messageLog->setFontItalic(false);

		messageLog->insertPlainText(ts);
		messageLog->moveCursor(QTextCursor::End);
	}

	if (bold)
		messageLog->setFontWeight(QFont::Bold);
	else
		messageLog->setFontWeight(QFont::Medium);
	messageLog->setFontItalic(italic);

	messageLog->insertPlainText(message);
}


/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::setMonitoredFolder([[maybe_unused]] const QString& link)
{
	if (monitoring)
	{
		QMessageBox::information(this, "DeepSkyStackerLive",
			tr("You cannot change the monitored folder while monitoring.", "IDS_CANTCHANGEMONITOREDFOLDER"));
	}
	else
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");

		QString startDir{ settings.value("MonitoredFolder", "").toString() };
		if (startDir.isEmpty())
			startDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();

		QString dir = QFileDialog::getExistingDirectory(this,
			tr("Select the folder to be monitored", "IDS_SELECTMONITOREDFOLDER"),
			startDir,
			QFileDialog::ShowDirsOnly
			| QFileDialog::DontResolveSymlinks);
		if (!dir.isEmpty())
		{
			settings.setValue("MonitoredFolder", dir);

			folderName->setText(dir);
			makeLink(folderName, linkColour);
		}
		settings.endGroup();

	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::monitorTriggered([[maybe_unused]] bool checked)
{
	qDebug() << "Monitor button pressed";
}

void DeepSkyStackerLive::stackTriggered(bool checked)
{
	qDebug() << "Stack button pressed"; 
	actionMonitor->setChecked(checked);
}

void DeepSkyStackerLive::stopTriggered()
{
	qDebug() << "Stop button pressed";
	actionMonitor->setChecked(false);
	actionStack->setChecked(false);
}


std::unique_ptr<std::uint8_t[]> backPocket;
constexpr size_t backPocketSize{ 1024 * 1024 };

static char const* global_program_name;

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();
	int result{ 0 };

#if defined(Q_OS_WIN)
	// Set console code page to UTF-8 so console knowns how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

	//
	// Create a storage cushion (aka back pocket storage)
	// and ensure that it is actually touched.
	//
	backPocket = std::make_unique<std::uint8_t[]>(backPocketSize);
	for (auto* p = backPocket.get(); p < backPocket.get() + backPocketSize; p += 4096)
	{
		*p = static_cast<uint8_t>('\xff');
	}

	//
	// Silence the MFC memory leak dump as we use Visual Leak Detector.
	//
#if defined(Q_OS_WIN)
	_CrtSetDbgFlag(0);
#if !defined(NDEBUG)
	AfxEnableMemoryLeakDump(false);
#endif
#endif

	if (hasExpired())
		return 1;

	//
	// Set things up to capture terminal errors
	//
	setDssExceptionHandling();

	QApplication app(argc, argv);

	//
	// Set up organisation etc. for QSettings usage
	//
	QCoreApplication::setOrganizationName("DeepSkyStacker");
	QCoreApplication::setOrganizationDomain("deepskystacker.free.fr");
	QCoreApplication::setApplicationName("DeepSkyStacker5");

	//
	// Set the Qt Application Style
	//
	app.setStyle(QStyleFactory::create("Fusion"));

	LoadTranslations();

	reportCpuType();

	ZTRACE_RUNTIME("Creating Main Window");
	DeepSkyStackerLive mainWindow;
	DSSBase::setInstance(&mainWindow);

	//
	// Register QMessageBox::Icon enum as meta type
	//
	qRegisterMetaType<QMessageBox::Icon>();

	//
	// Increase maximum size of QImage from the default of 128MB to 1GB
	//
	constexpr int oneGB{ 1024 * 1024 * 1024 };
	QImageReader::setAllocationLimit(oneGB);

	ZTRACE_RUNTIME("Invoking QApplication::exec()");
	try
	{
		Exiv2::XmpParser::initialize();
		::atexit(Exiv2::XmpParser::terminate);

		mainWindow.show();
		result = app.exec();

	}
	catch (std::exception& e)
	{
		ZTRACE_RUNTIME("std::exception caught: %s", e.what());
		traceControl.setDeleteOnExit(false);
		QString errorMessage(e.what());

		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
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

		QMessageBox::critical(nullptr, "DeepSkyStacker", errorMessage);
	}

	return result;
};

/* ------------------------------------------------------------------- */