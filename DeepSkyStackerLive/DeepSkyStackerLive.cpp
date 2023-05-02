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

//
// Necessary Windows header
//
#if defined(_WINDOWS)
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

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ std::source_location::current().file_name() };

bool	g_bShowRefStars = false;

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

using namespace std;

/* ------------------------------------------------------------------- */

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
		QString message = QCoreApplication::translate("DeepSkyStackerLive",
			"This beta version of DeepSkyStacker has expired\nYou can probably get another one or download the final release from the web site.");
		QMessageBox::critical(nullptr, "DeepSkyStacker",
			message, QMessageBox::Ok);
		bResult = true;
	};

	ZTRACE_RUNTIME("Check beta expiration - ok\n");

#endif
	return bResult;
};

DeepSkyStackerLive::DeepSkyStackerLive() :
	Ui_DeepSkyStackerLive {},
	initialised{ false },
	winHost{ nullptr },
	args{ qApp->arguments() },
	baseTitle{ QString("DeepSkyStackerLive %1").arg(VERSION_DEEPSKYSTACKER) },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr }		// errorMessageDialogIcon pointer
{
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

void DeepSkyStackerLive::connectSignalsToSlots()
{
}


void DeepSkyStackerLive::onInitialise()
{
	ZFUNCTRACE_RUNTIME();
	//
	// Connect Qt Signals to appropriate slots
	//
	connectSignalsToSlots();

	setWindowIcon(QIcon(":/DSSIcon.png"));

	setWindowTitle(baseTitle);

	gamma1->setColorAt(sqrt(0.5), QColor(qRgb(128, 128, 128)));
	gamma1->setPegsOnLeftOrBottom(true).
		setOrientation(QLinearGradientCtrl::Orientation::ForceHorizontal);

	gamma2->setColorAt(sqrt(0.5), QColor(qRgb(128, 128, 128)));
	gamma2->setPegsOnLeftOrBottom(true).
		setOrientation(QLinearGradientCtrl::Orientation::ForceHorizontal);

	ZTRACE_RUNTIME("Restoring Window State and Position");
	QSettings settings;
	settings.beginGroup("MainWindow");

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


void DeepSkyStackerLive::qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate)
{
	QMessageBox msgBox{ icon, "DeepSkyStacker", message, QMessageBox::Ok , this };
	msgBox.exec();
	if (terminate) QCoreApplication::exit(1);
}

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

std::unique_ptr<std::uint8_t[]> backPocket;
constexpr size_t backPocketSize{ 1024 * 1024 };

static char const* global_program_name;

int main(int argc, char* argv[])
{
	ZFUNCTRACE_RUNTIME();
	int result{ 0 };

#if defined(_WINDOWS)
	// Set console code page to UTF-8 so console known how to interpret string data
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
#if defined(_WINDOWS)
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
		LiveSettings liveSettings;
		liveSettings.LoadFromRegistry();

		Exiv2::XmpParser::initialize();
		::atexit(Exiv2::XmpParser::terminate);

		mainWindow.show();
		//result = app.run(&theApp);
		result = app.exec();

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

	return result;
};

/* ------------------------------------------------------------------- */