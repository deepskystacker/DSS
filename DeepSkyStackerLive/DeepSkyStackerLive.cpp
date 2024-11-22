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

#include "stdafx.h"

#if defined(Q_OS_WIN)
#include <htmlhelp.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <source_location>

#include <QApplication>
#include <QErrorMessage>
#include <QMessageBox>
#include <QImageReader>
#include <QtLogging>

//
// Necessary Windows header
//
#if defined(Q_OS_WIN) && !defined(NDEBUG)
//
// Visual Leak Detector
#include <vld.h>
#endif

#include "avx_simd_check.h"
#include "DeepSkyStackerLive.h"
#include "DSSVersion.h"
#include "ExceptionHandling.h"
#include "FrameInfoSupport.h"
#include "LiveSettings.h"
#include <zexcept.h>
#include <ztrace.h>
#include "tracecontrol.h"
#include "Workspace.h"
#include "foldermonitor.h"
#include "fileregistrar.h"
#include "filestacker.h"
#include "progresslive.h"
#include "RegisterEngine.h"
#include "RestartMonitoring.h"
#include <SmtpMime/SmtpMime>
#include "QMessageLogger.h"

using namespace DSS;
using namespace std;

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ source_location::current().file_name() };

bool	g_bShowRefStars = false;

namespace
{
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

	//qDebug() << "Loading translator file [" << translatorFileName << "] from path: " << path;
	if (!translator.load(translatorFileName, path))
	{
		//qDebug() << " *** Failed to load file [" << translatorFileName << "] into translator";
		return false;
	}

	if (!app.installTranslator(&translator))
	{
		//qDebug() << " *** Failed to install translator for file [" << translatorFileName << "]";
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
	LoadTranslationUnit(*qApp, theAppTranslator, "DeepSkyStackerLive_", ":/i18n/", language);
	LoadTranslationUnit(*qApp, theKernelTranslator, "DeepSkyStackerKernel_", ":/i18n/", language);

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
	winHost{ nullptr },
	args{ qApp->arguments() },
	baseTitle{ QString("DeepSkyStackerLive %1").arg(VERSION_DEEPSKYSTACKER) },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr },		// errorMessageDialogIcon pointer
	helpShortCut{ new QShortcut(QKeySequence::HelpContents, this) },
	linkColour{ palette().color(QPalette::ColorRole::Link).name() },
	folderMonitor { nullptr },
	fileRegistrar { nullptr },
	fileStacker{ nullptr },
	progressLabel { new QLabel(this) },
	pProgress {new DSS::ProgressLive(this)},
	stackedImageCnt{ 0 },
	totalExposure{ 0.0 },
	emailsSent{ 0 }
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

	//
	// Delete the file stacker and file registrar (which will also stop them).
	// Note: It's important that the fileStacker is deleted first!
	//
	delete fileStacker; fileStacker = nullptr;
	delete fileRegistrar; fileRegistrar = nullptr;

	ZTRACE_RUNTIME("Saving Window Geometry");

	QSettings settings;
	settings.beginGroup("DeepSkyStackerLive/MainWindow");
	auto geometry{ saveGeometry() };
	settings.setValue("geometry", geometry);
	settings.setValue("maximised", isMaximized());
	settings.endGroup();

	settings.beginGroup("DeepSkyStackerLive/ImageList");
	settings.setValue("HorizontalHeader/windowState", imageList->horizontalHeader()->saveState());
	settings.setValue("HorizontalHeader/numberOfColumns", static_cast<int>(ImageListColumns::ColumnCount));
	settings.endGroup();

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
	connect(this, &DSSLive::showResetEmailCount,
		settingsTab, &SettingsTab::showResetEmailCount);

	connect(this, &DSSLive::clearCharts,
		chartTab, &ChartTab::clear);

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

void DeepSkyStackerLive::createFileRegistrar()
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
	connect(fileRegistrar, &FileRegistrar::addToStackingQueue,
		this, &DSSLive::addToStackingQueue);
	connect(fileRegistrar, &FileRegistrar::fileNotStackable,
		this, &DSSLive::fileNotStackable);
	connect(fileRegistrar, &FileRegistrar::setImageInfo,			// This goes to the chart handling code
		chartTab, &ChartTab::setImageInfo);
	connect(fileRegistrar, &FileRegistrar::handleWarning,
		this, &DSSLive::handleWarning);
	connect(this, &DSSLive::dropPendingImages,
		fileRegistrar, &FileRegistrar::dropPendingImages);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::createFileStacker()
{
	fileStacker = new FileStacker(this, pProgress);
	connect(fileStacker, &FileStacker::writeToLog,
		this, &DSSLive::writeToLog);
	connect(fileStacker, &FileStacker::setImageOffsets,
		this, &DSSLive::setImageOffsets);
	connect(fileStacker, &FileStacker::fileStacked,
		this, &DSSLive::fileStacked);
	connect(fileStacker, &FileStacker::fileNotStackable,
		this, &DSSLive::fileNotStackable);
	connect(fileStacker, &FileStacker::setImageFootprint,
		this, &DSSLive::setImageFootprint);
	connect(fileStacker, &FileStacker::setImageInfo,			// This goes to the chart handling code
		chartTab, &ChartTab::setImageInfo);
	connect(fileStacker, &FileStacker::handleWarning,
		this, &DSSLive::handleWarning);
	connect(fileStacker, &FileStacker::showStackedImage,
		this, &DSSLive::showStackedImage);
	connect(stackedImage, &ImageViewer::saveStackedImage,
		fileStacker, &FileStacker::saveStackedImage);
	connect(fileStacker, &FileStacker::stackedImageSaved,
		stackedImage, &ImageViewer::stackedImageSaved);
	connect(this, &DSSLive::dropPendingImages,
		fileStacker, &FileStacker::dropPendingImages);
	connect(this, &DSSLive::clearStackedImage,
		fileStacker, &FileStacker::clearStackedImage);
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
	settings.endGroup();

	Workspace workspace;
	// Read the DSSLive setting file from the folder %AppData%/DeepSkyStacker/DeepSkyStacker5
	QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	fs::path fileName(directory.toStdU16String());
	create_directories(fileName);		// In case they don't exist

	fileName /= "DSSLive.settings";		// Append the filename with a path separator
	ZTRACE_RUNTIME("Loading DSSLive settings from: %s", fileName.generic_u8string().c_str());
	workspace.ReadFromFile(fileName);

	if (liveSettings->IsProcess_RAW()) validExtensions += rawExtensions;
	if (liveSettings->IsProcess_FITS()) validExtensions += fitsExtensions;
	if (liveSettings->IsProcess_TIFF()) validExtensions += tiffExtensions;
	if (liveSettings->IsProcess_Others()) validExtensions += otherExtensions;

	//
	// Force the progressBar to keep its size when hidden and hide it
	// 
	QSizePolicy sp{ progressBar->sizePolicy() };
	sp.setRetainSizeWhenHidden(true);
	progressBar->setSizePolicy(sp);
	progressBar->setVisible(false);

	//
	// Set initial text for image viewer tabs information text
	//
	stackedImage->information->setText(tr("No stacked image", "IDS_NOSTACKEDIMAGE"));
	lastImage->information->setText(tr("No image loaded", "IDS_NOIMAGELOADED"));

	//
	// Enable footprint display for the stacked image viewer
	//
	stackedImage->enableFootprint();

	//
	// Similarly for the copy to clipboard messages in the image viewers
	//
	lastImage->copyToClipboard->setSizePolicy(sp);
	lastImage->copyToClipboard->setVisible(false);
	stackedImage->copyToClipboard->setSizePolicy(sp);
	stackedImage->copyToClipboard->setVisible(false);

	// Create a QLabel on top of the progress bar to display text.
	//
	progressLabel->setAlignment(Qt::AlignCenter);
	progressLabel->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));
	progressLabel->setWordWrap(true);
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
	// Reduce the row height somewhat, as the default is a bit "fat"
	//
	QHeaderView* verticalHeader = imageList->verticalHeader();
	double height = verticalHeader->defaultSectionSize();
	height *= 0.734;		// reduce height (if the default is 30 this reduces it to 22)
	// Need to set minimum size as well (otherwise default size may be smaller).
	verticalHeader->setMinimumSectionSize(height);
	verticalHeader->setDefaultSectionSize(height);

	// 
	// Set image list headers and their alignments
	//
	imageList->setHorizontalHeaderLabels(QStringList{}
		<< tr("Status")
		<< tr("File", "IDS_COLUMN_FILE")
		<< tr("Exposure", "IDS_COLUMN_EXPOSURE")
		<< tr("Aperture", "IDS_COLUMN_APERTURE")
		<< tr("Score", "IDS_COLUMN_SCORE")
		<< tr("Quality", "IDS_COLUMN_MEANQUALITY")
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
	for (int column = 0; column != static_cast<int>(ImageListColumns::ColumnCount); ++column)
	{
		auto item = imageList->horizontalHeaderItem(column);
		Qt::Alignment alignment{ Qt::AlignLeft };
		switch (static_cast<ImageListColumns>(column))
		{
		case ImageListColumns::Exposure:
		case ImageListColumns::Aperture:
		case ImageListColumns::Score:
		case ImageListColumns::Quality:
		case ImageListColumns::Stars:
		case ImageListColumns::FWHM:
		case ImageListColumns::dX:
		case ImageListColumns::dY:
		case ImageListColumns::Angle:
		case ImageListColumns::DateTime:
		case ImageListColumns::Size:
		case ImageListColumns::ISOGain:
		case ImageListColumns::SkyBackground:
			alignment = Qt::AlignRight;
			break;
		case ImageListColumns::CFA:
			alignment = Qt::AlignHCenter;
			break;
		}
		item->setTextAlignment(alignment);
	}
	//
	// Restore windowState of table widget's horizontal header
	//
	settings.beginGroup("DeepSkyStackerLive/ImageList");
	const int numberOfSavedColumns = settings.value("HorizontalHeader/numberOfColumns", 0).toInt();
	if (numberOfSavedColumns == static_cast<int>(ImageListColumns::ColumnCount))
		imageList->horizontalHeader()->restoreState(settings.value("HorizontalHeader/windowState").toByteArray());
	settings.endGroup();
	
	//
	// Set image list non-editable
	//
	imageList->setEditTriggers(QAbstractItemView::NoEditTriggers);


	//
	// Set the active tab to be the message log.
	//
	tabWidget->setCurrentIndex(4);

	//
	// Finally create our worker threads
	//
	createFileRegistrar();
	createFileStacker();
}

void DeepSkyStackerLive::reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate)
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

void DeepSkyStackerLive::writeToLog(const QString& message, bool addTS, bool bold, bool italic, QColor colour)
{
	QMetaObject::invokeMethod(this, "writeLogMessage", Qt::AutoConnection,
		Q_ARG(const QString&, message),
		Q_ARG(bool, addTS),
		Q_ARG(bool, bold),
		Q_ARG(bool, italic),
		Q_ARG(QColor, colour));
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::startMonitoring()
{
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

	fileRegistrar->enableRegistration();
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::stopMonitoring()
{
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
	fileRegistrar->enableRegistration(false);
}
void DeepSkyStackerLive::stopStacking()
{
	QString message{ tr("Stop Stacking files", "IDS_LOG_STOPSTACKING") };
	ZTRACE_RUNTIME(message.toUtf8().constData());
	message += "\n";
	writeToLog(message,
		true, true, false, Qt::red);
	fileStacker->enableStacking(false);
}


/* ------------------------------------------------------------------- */
/* Slots                                                               */
/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::help()
{
	ZFUNCTRACE_RUNTIME();
#if defined(Q_OS_WIN)

	QString helpFile{ QCoreApplication::applicationDirPath() + "/" + tr("DeepSkyStacker Help.chm","IDS_HELPFILE") };

	::HtmlHelp(::GetDesktopWindow(), helpFile.toStdWString().c_str(), HH_DISPLAY_TOPIC, 0);
#else
	QMessageBox::information(this, "DeepSkyStacker", "Sorry, there's no help available for Linux yet");
#endif
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::progress(const QString& str, int achieved, int total)
{
	progressBar->setMaximum(total);
	progressBar->setValue(achieved);
	progressBar->setVisible(true);
	progressLabel->setText(str);
	update();
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::endProgress()
{
	progressLabel->setText("");
	progressBar->setValue(0);
	progressBar->setVisible(false);
	update();

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

bool DeepSkyStackerLive::setMonitoredFolder([[maybe_unused]] const QString& link)
{
	bool result{ false };
	if (folderMonitor)
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
			result = true;
		}
		settings.endGroup();
	}
	return result;
}

/* ------------------------------------------------------------------- */

bool DeepSkyStackerLive::checkRestartMonitor()
{
	bool result{ true };

	if ((fileRegistrar && fileStacker && stackedImageCnt) || fileRegistrar->pendingImageCount() || fileStacker->registeredImageCount())
	{
		result = false;
		RestartMonitoring dlg;
		if ( QDialog::Accepted == dlg.exec())
		{ 

			result = true;
			if (dlg.clearStackedImage())
			{
				emit clearStackedImage();
				removeFromListIfStatusIs(tr("Stacked", "IDS_STATUS_STACKED"));
			}
			if (dlg.dropPendingImages())
			{
				emit clearCharts();
				emit dropPendingImages();
				removeFromListIfStatusIs(tr("Pending"));
				removeFromListIfStatusIs(tr("Registered", "IDS_STATUS_REGISTERED"));

			}
			updateStatusMessage();
		}
	}
	return result;
}


/* ------------------------------------------------------------------- */

bool DeepSkyStackerLive::canWriteToMonitoredFolder()
{
	bool result{ false };
	QDir dir{ monitoredFolder};

	//
	// Check that a file can be written
	//
	if (dir.exists())
	{
		QFile file(dir.filePath("DSSLive.test.txt"));
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			auto bytes = file.write("DeepSkyStacker: This is a test file to check that it is possible to write in this folder");
			if (bytes > 0)
				result = true;
			file.remove();
		}
	}

	return result;
}


/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::monitorPressed([[maybe_unused]] bool checked)
{
	ZFUNCTRACE_RUNTIME();
	if (checked )
	{
		if (checkRestartMonitor())
		{
			startMonitoring();
		}
	}
	else
	{
		stopMonitoring();
	}

}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::moveToNonStackable(fs::path& file)
{
	std::error_code ec;
	fs::path name{ file.filename() };
	fs::path output{ file };
	output.remove_filename();
	output /= "NonStackable";
	fs::create_directory(output, ec);
	if (ec)
	{
		QString message{ QString::fromStdString(ec.message()) };
		reportError(message, "", Severity::Warning, Method::QMessageBox, false);
	}
	output /= name;
	fs::rename(file, output, ec);
	if (ec)
	{
		QString message{ QString::fromStdString(ec.message()) };
		reportError(message, "", Severity::Warning, Method::QMessageBox, false);
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::stackPressed(bool checked)
{
	if (checked)
	{
		bool startStacker{ false };
		if (!folderMonitor)
		{
			if (canWriteToMonitoredFolder() || setMonitoredFolder(""))
			{
				if (checkRestartMonitor())
				{
					actionMonitor->setChecked(true);
					startMonitoring();
					startStacker = true;
				}
			}
		}
		else startStacker = true;

		if (startStacker)
		{
			QString message{ tr("Start Stacking files", "IDS_LOG_STARTSTACKING") };
			ZTRACE_RUNTIME(message.toUtf8().constData());
			message += "\n";
			writeToLog(message,
				true, true, false, Qt::yellow);

			fileStacker->enableStacking();
		}
	}
	else
	{
		stopStacking();
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::stopPressed()
{
	if (actionMonitor->isChecked())
	{
		actionMonitor->setChecked(false);
		stopMonitoring();

	}
	if (actionStack->isChecked())
	{
		actionStack->setChecked(false);
		stopStacking();
	}
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

	if (0 == fileRegistrar->pendingImageCount() && 0 == stackedImageCnt && !filteredFiles.empty())
	{
		if (QMessageBox::Yes == QMessageBox::question(this, "DeepSkyStackerLive",
			tr("You have %n image(s) in the monitored folder.\nDo you want to process them?", "IDS_USEEXISTINGIMAGES",
				static_cast<int>(filteredFiles.size())),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes))
		{
			useExistingFiles = true;
		}
	}
	
	if (useExistingFiles && !filteredFiles.empty())
	{
		writeToLog(tr("%n new file(s) found\n", "IDS_LOG_NEWFILESFOUND", static_cast<int>(filteredFiles.size())),
			true, false, false, Qt::green);
		for (const auto& file : filteredFiles)
		{
			fileRegistrar->addFile(file);
		}
		updateStatusMessage();
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::onNewFile(const fs::path& file)
{
	ZTRACE_RUNTIME("New file created in watched folder: %s", file.generic_u8string().c_str());
	QString extension{ QString::fromStdU16String(file.extension().u16string()).toLower() };
	if (validExtensions.contains(extension))
	{
		ZTRACE_RUNTIME(" File passed filtering");

		fileRegistrar->addFile(file);
		updateStatusMessage();
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::addImageToList(fs::path path)
{
	ZFUNCTRACE_RUNTIME();
	QString name{ QString::fromStdU16String(path.filename().generic_u16string().c_str()) };
	int row = imageList->rowCount();
	imageList->insertRow(row);

	//
	// Insert TableWidgetItems for all columns
	//
	for (int column = 0; column < static_cast<int>(ImageListColumns::ColumnCount); ++column)
	{
		QTableWidgetItem* item = new QTableWidgetItem;
		Qt::Alignment alignment{ Qt::AlignLeft };
		switch (static_cast<ImageListColumns>(column))
		{
		case ImageListColumns::Exposure:
		case ImageListColumns::Aperture:
		case ImageListColumns::Score:
		case ImageListColumns::Quality:
		case ImageListColumns::Stars:
		case ImageListColumns::FWHM:
		case ImageListColumns::dX:
		case ImageListColumns::dY:
		case ImageListColumns::Angle:
		case ImageListColumns::DateTime:
		case ImageListColumns::Size:
		case ImageListColumns::ISOGain:
		case ImageListColumns::SkyBackground:
			alignment = Qt::AlignRight;
			break;
		case ImageListColumns::CFA:
			alignment = Qt::AlignHCenter;
			break;
		}
		item->setTextAlignment(alignment);
		imageList->setItem(row, column, item);
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

void DeepSkyStackerLive::removeFromListIfStatusIs(const QString& status)
{
	for (int row = imageList->rowCount() -1; row >= 0; --row)
	{
		if (status == imageList->item(row, static_cast<int>(ImageListColumns::Status))->text())
		{
			imageList->removeRow(row);
		}
	}
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileLoaded(std::shared_ptr<LoadedImage> image, const fs::path fileName)
{
	QString name{ QString::fromStdU16String(fileName.filename().generic_u16string()) };
	lastImage->setLoadedImage(image);
	lastImage->copyToClipboard->setVisible(true);
	lastImage->information->setText(name);

	changeImageStatus(name, ImageStatus::loaded);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::showStackedImage(std::shared_ptr<LoadedImage> li, int count, double exposure)
{
	stackedImageCnt = count;
	totalExposure = exposure;

	//
	// If there wasn't a valid stacked image, clear the stacked image display, otherwise
	// display the image
	//
	if (!li->m_pBitmap)
	{
		stackedImage->information->setText(tr("No stacked image", "IDS_NOSTACKEDIMAGE"));
		stackedImage->picture->clear();
		stackedImage->copyToClipboard->setVisible(false);
	}
	else
	{
		stackedImage->setLoadedImage(li);
		stackedImage->copyToClipboard->setVisible(true);
		stackedImage->information->setText(
			QString("<a href='.' style='text-decoration: none; color: %1'>%2</a>")
			.arg(palette().color(QPalette::ColorRole::Link).name())
			.arg(tr("Click here to save the stacked image to file", "IDS_SAVESTACKEDIMAGE")));
	}
	updateStatusMessage();
}

/* ------------------------------------------------------------------- */

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
		newStatus = tr("Stack delayed", "IDS_STATUS_STACKDELAYED");
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

void DeepSkyStackerLive::setImageFootprint(QPointF p1, QPointF p2, QPointF p3, QPointF p4)
{
	stackedImage->setFootprint(p1, p2, p3, p4);
}


void DeepSkyStackerLive::setImageOffsets(QString name, double dX, double dY, double angle)
{
	QLocale locale;
	for (int row = 0; row < imageList->rowCount(); ++row)
	{
		if (name == imageList->item(row, static_cast<int>(ImageListColumns::File))->text())
		{
			imageList->item(row, static_cast<int>(ImageListColumns::dX))->setText(locale.toString(dX, 'f', 2));
			imageList->item(row, static_cast<int>(ImageListColumns::dY))->setText(locale.toString(dY, 'f', 2));
			imageList->item(row, static_cast<int>(ImageListColumns::Angle))->setText(locale.toString(angle, 'f', 2));

			break;
		}
	}
	chartTab->addOffsetAngle(name, dX, dY, angle);
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileRegistered(std::shared_ptr<CLightFrameInfo> lfi)
{
	QString name{ QString::fromStdU16String(lfi->filePath.filename().generic_u16string().c_str()) };
	ZTRACE_RUNTIME("File %s registered", name.toUtf8().constData());
	changeImageStatus(name, ImageStatus::registered);
	CBitmapInfo				bmpInfo;
	QString temp;

	GetPictureInfo(lfi->filePath, bmpInfo);

	lfi->m_strDateTime = bmpInfo.m_strDateTime;
	lfi->m_DateTime = bmpInfo.m_DateTime;

	temp = bmpInfo.m_strFileType;
	if (bmpInfo.m_strModel.length())
		temp += " " + bmpInfo.m_strModel;
	lfi->m_strInfos = temp;
	temp = "";

	QLocale locale;
	for (int row = 0; row < imageList->rowCount(); ++row)
	{
		if (name == imageList->item(row, static_cast<int>(ImageListColumns::File))->text())
		{
			imageList->item(row, static_cast<int>(ImageListColumns::Exposure))->setText(exposureToString(lfi->m_fExposure));
			imageList->item(row, static_cast<int>(ImageListColumns::Aperture))->setText(locale.toString(lfi->m_fAperture, 'f', 1));
			imageList->item(row, static_cast<int>(ImageListColumns::Score))->setText(locale.toString(lfi->m_fOverallQuality, 'f', 2));
			imageList->item(row, static_cast<int>(ImageListColumns::Quality))->setText(locale.toString(lfi->quality, 'f', 2));
			imageList->item(row, static_cast<int>(ImageListColumns::Stars))->setText(locale.toString(lfi->m_vStars.size()));
			imageList->item(row, static_cast<int>(ImageListColumns::FWHM))->setText(locale.toString(lfi->m_fFWHM, 'f', 2));
			//
			imageList->item(row, static_cast<int>(ImageListColumns::dX))->setText("NC");
			imageList->item(row, static_cast<int>(ImageListColumns::dY))->setText("NC");
			imageList->item(row, static_cast<int>(ImageListColumns::Angle))->setText("NC");
			//
			imageList->item(row, static_cast<int>(ImageListColumns::DateTime))->setText(bmpInfo.m_strDateTime);
			temp = QString("%1 x %2").arg(bmpInfo.m_lWidth).arg(bmpInfo.m_lHeight);
			imageList->item(row, static_cast<int>(ImageListColumns::Size))->setText(temp);
			if (bmpInfo.m_CFAType == CFATYPE_NONE)
				temp = tr("No", "IDS_NO");
			else
				temp = tr("Yes", "IDS_YES");
			imageList->item(row, static_cast<int>(ImageListColumns::CFA))->setText(temp);
			if (lfi->m_lNrChannels == 3)
				temp = tr("RGB %1 bit/ch", "IDS_FORMAT_RGB").arg(lfi->m_lBitsPerChannel);
			else
				temp = tr("Gray %1 bit", "IDS_FORMAT_GRAY").arg(lfi->m_lBitsPerChannel);
			imageList->item(row, static_cast<int>(ImageListColumns::Depth))->setText(temp);
			imageList->item(row, static_cast<int>(ImageListColumns::Info))->setText(lfi->m_strInfos);
			if (lfi->m_lISOSpeed)
			{
				temp = isoToString(lfi->m_lISOSpeed);
			}
			else if (lfi->m_lGain >= 0)
			{
				temp = gainToString(lfi->m_lGain);
			}
			imageList->item(row, static_cast<int>(ImageListColumns::ISOGain))->setText(temp);
			temp = tr("%1%").arg(locale.toString(lfi->m_SkyBackground.m_fLight * 100.0, 'f', 2));
			imageList->item(row, static_cast<int>(ImageListColumns::SkyBackground))->setText(temp);
			break;
		}

	}
	chartTab->addScoreFWHMStars(name, lfi->m_fOverallQuality, lfi->m_fFWHM, lfi->m_vStars.size(), lfi->m_SkyBackground.m_fLight * 100.0);
}

void DeepSkyStackerLive::addToStackingQueue(std::shared_ptr<CLightFrameInfo> lfi)
{
	//
	// Add the file to the stacking work queue 
	//
	if (nullptr != fileStacker)
		fileStacker->addFile(lfi);
	updateStatusMessage();

}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileStacked(std::shared_ptr<CLightFrameInfo> lfi)
{
	QString name{ QString::fromStdU16String(lfi->filePath.filename().generic_u16string().c_str()) };
	ZTRACE_RUNTIME("File %s stacked", name.toUtf8().constData());

	changeImageStatus(name, ImageStatus::stacked);

	updateStatusMessage();
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::fileNotStackable(fs::path file)
{
	QString name{ QString::fromStdU16String(file.filename().generic_u16string().c_str()) };
	ZTRACE_RUNTIME("File %s not Stackable", name.toUtf8().constData());
	changeImageStatus(name, ImageStatus::nonStackable);
	if (liveSettings->IsStack_Move())
	{
		moveToNonStackable(file);
	}
	updateStatusMessage();
}

/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::resetEmailCount()
{
	emailsSent = 0;
}

/* ------------------------------------------------------------------- */

void DSSLive::handleWarning(QString warning)
{
	if (!warning.isEmpty())
	{
		if (liveSettings->IsWarning_Flash())
			QApplication::alert(this);
		if (liveSettings->IsWarning_Sound())
			QApplication::beep();
		if (liveSettings->IsWarning_File())
		{
			QString folder{ liveSettings->GetWarning_FileFolder() };
			if (!folder.isEmpty())
			{
				fs::path path(folder.toStdU16String());
				if (is_directory(path))
				{
					path /= "DSSLiveWarning.txt";
					QFile file(path);
					if (file.open(QFile::WriteOnly | QFile::Append | QIODeviceBase::Text))
					{
						QTextStream stream{ &file };
						stream << warning << Qt::endl;
					}
				}
			}
		}
		if (liveSettings->IsWarning_Email())
		{
			bool sendEmail {false};

			if (liveSettings->IsWarning_SendMultipleEmails())
				sendEmail = true;
			else if (!emailsSent)
				sendEmail = true;

			if (sendEmail)
			{
				QString	addressee{};
				QString	subject{};
				QString	server{};
				int		port;
				uint	encryption;
				QString	account{};
				QString	password{};

				liveSettings->getEmailSettings(addressee, subject, server, port, encryption, account, password);

				//
				// De-obfuscate the password
				//
				for (auto& character : password)
				{
					character = QChar(static_cast<uint16_t>(character.unicode()) ^ 0x82U);
				}

				SmtpClient::ConnectionType connectionType { static_cast<SmtpClient::ConnectionType>(encryption) };

				MimeMessage message;

				EmailAddress sender(account, "DeepSkyStackerLive");
				message.setSender(sender);

				EmailAddress to(addressee, "");
				message.addRecipient(to);

				message.setSubject(subject);

				// Now add some text to the email.
				// First we create a MimeText object.

				MimeText text;

				text.setText(warning);

				// Now add it to the mail

				message.addPart(&text);

				// Now we can send the mail
				SmtpClient smtp(server, port, connectionType);

				smtp.connectToHost();
				if (!smtp.waitForReadyConnected())
				{
					QString errorMessage{ tr("Failed to connect to email server %1 (%2)!").arg(server).arg(port) };
					errorMessage += "\n";
					emit writeToLog(errorMessage, true, true, false, Qt::red);
					return;
				}

				smtp.login(account, password);
				if (!smtp.waitForAuthenticated())
				{
					QString errorMessage{ tr("Failed to login to email server as %1!").arg(account) };
					errorMessage += "\n";
					emit writeToLog(errorMessage, true, true, false, Qt::red);
					return;
				}

				smtp.sendMail(message);
				if (!smtp.waitForMailSent())
				{
					QString errorMessage{ "Failed to send mail!" };
					errorMessage += "\n";
					emit writeToLog(errorMessage, true, true, false, Qt::red);

					return;
				}

				smtp.quit();
				emailsSent++;
				emit showResetEmailCount();

			}
		}
	}
}


/* ------------------------------------------------------------------- */

void DeepSkyStackerLive::updateStatusMessage()
{
	QString exposure{ exposureToString(totalExposure) };
	if (fileRegistrar && fileStacker)
	{
		QString message{ tr("Pending: %1 - Registered: %2 - Stacked: %3 - Total exposure time: %4")
			.arg(fileRegistrar->pendingImageCount()).arg(fileStacker->registeredImageCount()).arg(stackedImageCnt).arg(exposure) };
		statusMessage->setText(message);
		update();
	}
}

std::unique_ptr<std::uint8_t[]> backPocket;
constexpr size_t backPocketSize{ 1024 * 1024 };

static char const* global_program_name;

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();
	int result{ 0 };
	//
	// Save the program name in case we need it later
	// 
	global_program_name = argv[0];


#if defined(Q_OS_WIN)
	// Set console code page to UTF-8 so console knowns how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

	//
	// Log Qt messages to the trace file as well as to the debugger.
	//
	originalHandler = qInstallMessageHandler(qtMessageLogger);

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
	// Set up retention of the trace based on user settings
	//
	QSettings settings;
	settings.beginGroup("DeepSkyStackerLive");
	auto retainTrace{ settings.value("RetainTraceFile", false).toBool() };
	traceControl.setDeleteOnExit(!retainTrace);
	settings.endGroup();

	//
	// Set the Qt Application Style
	//
	app.setStyle(QStyleFactory::create("Fusion"));

	LoadTranslations();

	AvxSimdCheck::reportCpuType();

	ZTRACE_RUNTIME("Creating Main Window");
	DeepSkyStackerLive mainWindow;
	DSSBase::setInstance(&mainWindow);

	//
	// Register QMessageBox::Icon enum as meta type
	//
	qRegisterMetaType<STACKIMAGEINFO>();
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
