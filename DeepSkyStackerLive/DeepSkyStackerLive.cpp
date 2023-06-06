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
#include <htmlhelp.h>

#include <source_location>

#include <QErrorMessage>
#include <QMessageBox>
#include <QImageReader>
#include <QtLogging>

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
#include "Workspace.h"
#include "foldermonitor.h"
#include "fileregistrar.h"
#include "progresslive.h"
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>

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

#ifndef NDEBUG
	QtMessageHandler originalHandler;
	void qtMessageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
	{
		QByteArray localMsg = msg.toLocal8Bit();
		const char* file = context.file ? context.file : "";
		const char* function = context.function ? context.function : "";
		switch (type) {
		case QtDebugMsg:
			ZTRACE_RUNTIME("Qt Debug: %s (%s:%u, %s)", localMsg.constData(), file, context.line, function);
			break;
		case QtInfoMsg:
			ZTRACE_RUNTIME("Qt Info: %s (%s:%u, %s)", localMsg.constData(), file, context.line, function);
			break;
		case QtWarningMsg:
			ZTRACE_RUNTIME("Qt Warning: %s (%s:%u, %s)", localMsg.constData(), file, context.line, function);
			break;
		case QtCriticalMsg:
			ZTRACE_RUNTIME("Qt Critical: %s (%s:%u, %s)", localMsg.constData(), file, context.line, function);
			break;
		case QtFatalMsg:
			ZTRACE_RUNTIME("Qt Fatal: %s (%s:%u, %s)", localMsg.constData(), file, context.line, function);
			break;
		}
		originalHandler(type, context, msg);
	}
#endif

	//
	// Convert a QLabel with "plain text" to a hyperlink
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

const QStringList rawExtensions { ".cr2", ".cr3", ".crw", ".nef", ".mrw", ".orf", ".raf", ".pef", ".x3f", ".dcr",
								  ".kdc", ".srf", ".arw", ".raw", ".dng", ".ia", ".rw2" };
const QStringList fitsExtensions { ".fits", ".fit", ".fts" };
const QStringList tiffExtensions { ".tif", ".tiff" };
const QStringList otherExtensions{ ".jpg", ".jpeg", ".png" };

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
	helpShortCut{ new QShortcut(QKeySequence::HelpContents, this) },
	linkColour{ palette().color(QPalette::ColorRole::Link).name() },
	stackedImageViewer {nullptr},
	lastImageViewer {nullptr},
	folderMonitor { nullptr },
	fileRegistrar { nullptr },
	progressLabel { new QLabel(this) },
	pProgress {new DSS::ProgressLive(this)},
	pendingImageCount{ 0 },
	stackedImageCount{ 0 }
{
	//
	// Must set dssInstance before invoking setupUi 
	//
	dssInstance = this;
	setupUi(this);

	//
	// Set to F1 (Windows) or Command + ? (MacOs) or ?? to invoke help
	//
	helpShortCut->setContext(Qt::ApplicationShortcut);
}

DeepSkyStackerLive::~DeepSkyStackerLive()
{
}

void DeepSkyStackerLive::closeEvent(QCloseEvent* e)
{
	ZFUNCTRACE_RUNTIME();

	e->accept();

	//
	// Stop the folder monitor if it is running
	//
	ZTRACE_RUNTIME("Stopping folder monitor");
	emit stopMonitor();		

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
	connect(helpShortCut, &QShortcut::activated,
		this, &DeepSkyStacker::help);
	connect(folderName, &QLabel::linkActivated,
		this, &DSSLive::setMonitoredFolder);
	connect(actionMonitor, &QAction::triggered,
		this, &DSSLive::monitorPressed);
	connect(actionStack, &QAction::triggered,
		this, &DSSLive::stackPressed);
	connect(actionStop, &QAction::triggered,
		this, &DSSLive::stopPressed);
	connect(settingsTab, &SettingsTab::settingsChanged,
		this, &DSSLive::settingsChanged);

	// Progress signals
	connect(pProgress, &ProgressLive::progress,
		this, &DSSLive::progress);
	connect(pProgress, &ProgressLive::endProgress,
		this, &DSSLive::endProgress);
}

void DeepSkyStackerLive::connectMonitorSignals()
{
	connect(folderMonitor, &FolderMonitor::existingFiles,
		this, &DSSLive::onExistingFiles);
	connect(folderMonitor, &FolderMonitor::fileCreated,
		this, &DSSLive::onNewFile);
	connect(this, &DSSLive::stopMonitor,
		folderMonitor, &FolderMonitor::stop);
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

	traceControl.setDeleteOnExit(false);
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
	{
		folderName->setText(dir);
		monitoredFolder = dir;
	}
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


	writeToLog(tr(
		"\nHow to use  DeepSkyStacker Live ?\n", "IDS_LOG_STARTING"),
		false, true, false);
	writeToLog(tr(
		"Step 1\nCheck the Settings tabs for all the stacking and warning settings\n\n", "IDS_LOG_STARTING_1"),
		false, false, false);
	writeToLog(tr(
		"Step 2\nClick on the Monitor button to start monitoring the folder\n"
		"When monitoring is active incoming images are only registered but not stacked.\n\n", "IDS_LOG_STARTING_2"),
		false, false, false);
	writeToLog(tr(
		"Step 3\nTo start stacking the images click on the Stack button\n"
		"At this point all the incoming (and all previously registered) images will be stacked.\n", "IDS_LOG_STARTING_3"),
		false, false, false);
	writeToLog(tr(
		"You can pause/restart the stacking process by clicking on the Stack button.\n", "IDS_LOG_STARTING_4"),
		false, false, false);
	writeToLog(tr(
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

	Workspace workspace;
	// Read the DSSLive setting file from the folder %AppData%/DeepSkyStacker/DeepSkyStacker5
	QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	fs::path fileName(directory.toStdU16String());
	create_directories(fileName);		// In case they don't exist

	fileName /= "DSSLive.settings";		// Append the filename with a path separator
	ZTRACE_RUNTIME("Loading DSSLive settings from: %s", fileName.generic_string().c_str());
	workspace.ReadFromFile(fileName);

	if (liveSettings->IsProcess_RAW()) validExtensions += rawExtensions;
	if (liveSettings->IsProcess_FITS()) validExtensions += fitsExtensions;
	if (liveSettings->IsProcess_TIFF()) validExtensions += tiffExtensions;
	if (liveSettings->IsProcess_Others()) validExtensions += otherExtensions;

	//
	// Create a QLabel on top of the progress bar to display text.
	//
	progressLabel->setAlignment(Qt::AlignCenter); progressLabel->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));
	auto layout = new QVBoxLayout(progressBar);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(progressLabel);

	//
	// Make sure the table header row is always visible by setting the column count now
	// 
	imageList->setColumnCount(static_cast<int>(ImageListColumns::ColumnCount));

	//
	// Reduce image list font size and increase weight
	//
	font = imageList->font();
	font.setPointSize(font.pointSize() - 1); font.setWeight(QFont::Medium);
	imageList->setFont(font);
	font = imageList->horizontalHeader()->font();
	//font.setPointSize(font.pointSize() - 1);
	font.setWeight(QFont::Medium);
	imageList->horizontalHeader()->setFont(font);

	// 
	// Set image list headers
	//
	imageList->setHorizontalHeaderLabels(QStringList{}
		<< tr("Status")
		<< tr("File", "IDS_COLUMN_FILE")
		<< tr("Exposure", "IDS_COLUMN_EXPOSURE")
		<< tr("Aperture", "IDS_COLUMN_APERTURE")
		<< tr("Score", "IDS_COLUMN_SCORE")
		<< tr("#Stars", "IDS_COLUMN_STARS")
		<< tr("FWHM")
		<< tr("dX", "IDS_COLUMN_DX")
		<< tr("dY", "IDS_COLUMN_DY")
		<< tr("Angle", "IDS_COLUMN_ANGLE")
		<< tr("Date/Time", "IDS_COLUMN_DATETIME")
		<< tr("Size", "IDS_COLUMN_SIZES")
		<< tr("CFA", "IDS_COLUMN_CFA")
		<< tr("Depth", "IDS_COLUMN_DEPTH")
		<< tr("Info", "IDS_COLUMN_INFOS")
		<< tr("ISO/Gain", "IDS_COLUMN_ISO_GAIN")
		<< tr("Sky Background", "IDS_COLUMN_SKYBACKGROUND"));
	//
	// Set image list non-editable
	//
	imageList->setEditTriggers(QAbstractItemView::NoEditTriggers);

	tabWidget->setCurrentIndex(0);
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

void DeepSkyStackerLive::help()
{
	ZFUNCTRACE_RUNTIME();
	QString helpFile{ QCoreApplication::applicationDirPath() + "/" + tr("DeepSkyStacker Help.chm","IDS_HELPFILE") };

	::HtmlHelp(::GetDesktopWindow(), helpFile.toStdWString().c_str(), HH_DISPLAY_TOPIC, 0);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::progress(const QString& str, int achieved, int total)
{
	progressLabel->setText(str);
	progressBar->setMaximum(total);
	progressBar->setValue(achieved);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::endProgress()
{
	progressBar->setValue(progressBar->maximum());
}

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
			monitoredFolder = dir;
		}
		settings.endGroup();

	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::monitorPressed([[maybe_unused]] bool checked)
{
	ZFUNCTRACE_RUNTIME();
	qDebug() << "Monitor button pressed";
	QSettings settings;
	settings.beginGroup("DeepSkyStackerLive");

	//
	// Has the folder to monitor aleady been set?  If not set it.
	// 
	if (monitoredFolder.isEmpty())
	{
		setMonitoredFolder("");
	}
	fs::path dir{ monitoredFolder.toStdU16String() };

	if (!is_directory(dir))
	{
		QMessageBox::information(this, "DeepSkyStackerLive",
			tr("%1 is not a directory. Please select a valid directory.").arg(monitoredFolder));
		settings.setValue("MonitoredFolder", "");
		setMonitoredFolder("");
	}

	folderMonitor = new FolderMonitor();
	connectMonitorSignals();
	QThreadPool::globalInstance()->start(folderMonitor);

	ZTRACE_RUNTIME("Start monitoring folder %s", monitoredFolder.toUtf8().constData());
	writeToLog(tr("Start monitoring folder %1\n", "IDS_LOG_STARTMONITORING").arg(monitoredFolder),
		true, true, false, Qt::green);

}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::stackPressed(bool checked)
{
	qDebug() << "Stack button pressed"; 
	actionMonitor->setChecked(checked);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::stopPressed()
{
	qDebug() << "Stop button pressed";
	actionMonitor->setChecked(false);
	actionStack->setChecked(false);

	// 
	// Stop the folder monitor thread.
	//
	QString message{ tr("Stop monitoring folder %1", "IDS_LOG_STOPMONITORING").arg(monitoredFolder) };
	ZTRACE_RUNTIME(message.toUtf8().constData());
	message += "\n";
	writeToLog(message,
		true, true, false, Qt::red);
	emit stopMonitor();
	folderMonitor = nullptr;

	//
	// Delete the file registrar (which will also stop it).
	//
	delete fileRegistrar; fileRegistrar = nullptr;
}

void DSSLive::settingsChanged()
{
	validExtensions.clear();
	if (liveSettings->IsProcess_RAW()) validExtensions += rawExtensions;
	if (liveSettings->IsProcess_FITS()) validExtensions += fitsExtensions;
	if (liveSettings->IsProcess_TIFF()) validExtensions += tiffExtensions;
	if (liveSettings->IsProcess_Others()) validExtensions += otherExtensions;
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::onExistingFiles(const std::vector<fs::path>& files)
{
	ZFUNCTRACE_RUNTIME();
	bool useExistingFiles{ false };
	
	std::vector<fs::path> filteredFiles;
	ZTRACE_RUNTIME(" %d existing files files found", files.size());

	for (const auto& file : files)
	{
		QString extension{ QString::fromStdU16String(file.extension().u16string()).toLower() };
		if (validExtensions.contains(extension))
		{
			filteredFiles.emplace_back(file);
		}
	}

	ZTRACE_RUNTIME(" of which %d passed filtering", filteredFiles.size());

	if (0 == pendingImageCount && 0 == stackedImageCount && !filteredFiles.empty())
	{
		if (QMessageBox::Yes == QMessageBox::question(this, "DeepSkyStackerLive",
			tr("You have %n images(s) in the monitored folder.\nDo you want to process them?", "IDS_USEEXISTINGIMAGES",
				static_cast<int>(filteredFiles.size())),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes))
		{
			useExistingFiles = true;
		}
	}
	
	if (useExistingFiles && !filteredFiles.empty() && nullptr == fileRegistrar)
	{
		fileRegistrar = new FileRegistrar(this, pProgress);
		connect(fileRegistrar, &FileRegistrar::writeToLog,
			this, &DSSLive::writeToLog);
		connect(fileRegistrar, &FileRegistrar::addImageToList,
			this, &DSSLive::addImageToList);
		connect(fileRegistrar, &FileRegistrar::fileLoaded,
			this, &DSSLive::fileLoaded);
		connect(fileRegistrar, &FileRegistrar::fileRegistered,
			this, &DSSLive::fileRegistered);

		for (const auto& file : filteredFiles)
		{
			fileRegistrar->addFile(file);
		}
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::onNewFile(const fs::path& file)
{
	ZTRACE_RUNTIME("New file created in watched folder: %s", file.generic_string().c_str());
	QString extension{ QString::fromStdU16String(file.extension().u16string()).toLower() };
	if (validExtensions.contains(extension))
	{
		ZTRACE_RUNTIME(" File passed filtering");
		fileRegistrar->addFile(file);
	}
}

void DeepSkyStackerLive::addImageToList(fs::path path)
{
	ZFUNCTRACE_RUNTIME();
	QString name{ QString::fromStdU16String(path.filename().generic_u16string().c_str()) };
	int row = imageList->rowCount();
	imageList->insertRow(row);

	//
	// Insert TableWidgetItems for all columns
	//
	for (int i = 0; i < static_cast<int>(ImageListColumns::ColumnCount); ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem;
		imageList->setItem(row, i, item);
	}

	QFileInfo info{ name };
	QDateTime birthTime{ info.birthTime() };
	if (!birthTime.isValid())
	{
		birthTime = info.lastModified();
	}

	imageList->item(row, static_cast<int>(ImageListColumns::Status))->setText(tr("Pending"));
	imageList->item(row, static_cast<int>(ImageListColumns::File))->setText(name);
	imageList->item(row, static_cast<int>(ImageListColumns::DateTime))->setText(birthTime.toString("yyyy/MM/dd hh:mm:ss"));
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileLoaded(std::shared_ptr<CMemoryBitmap> bitmap, std::shared_ptr<QImage> image, fs::path file)
{
	QString name{ QString::fromStdU16String(file.filename().generic_u16string().c_str()) };
	lastImage->picture->setPixmap(QPixmap::fromImage(*image));

	changeImageStatus(name, ImageStatus::loaded);
}

void DeepSkyStackerLive::changeImageStatus(const QString& name, ImageStatus status)
{
	QString newStatus;
	switch (status)
	{
	case ImageStatus::pending:
		newStatus = tr("Pending");
		break;
	case ImageStatus::loaded:
		newStatus = tr("Loaded", "IDS_STATUS_LOADED");
		break;
	case ImageStatus::registered:
		newStatus = tr("Registered", "IDS_STATUS_REGISTERED");
		break;
	case ImageStatus::stackDelayed:
		newStatus = tr("Stack delayed", "IDS_STATUS_STACKDELAYE");
		break;
	case ImageStatus::nonStackable:
		newStatus = tr("Not stackable", "IDS_STATUS_NOTSTACKABLE");
		break;
	case ImageStatus::stacked:
		newStatus = tr("Stacked", "IDS_STATUS_STACKED");
		break;
	}

	for (int row = 0; row < imageList->rowCount(); ++row)
	{
		if (name == imageList->item(row, static_cast<int>(ImageListColumns::File))->text())
		{
			imageList->item(row, static_cast<int>(ImageListColumns::Status))->setText(newStatus);
			break;
		}
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileRegistered(fs::path file)
{
	qDebug() << "File " << QString::fromStdU16String(file.filename().generic_u16string().c_str()) << " registered";
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

#ifndef NDEBUG
	//
	// If this is a debug build, log Qt messages to the trace file as well as to the debugger.
	//
	originalHandler = qInstallMessageHandler(qtMessageLogger);
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
