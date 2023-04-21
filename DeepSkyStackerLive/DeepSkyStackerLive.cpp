// DeepSkyStackerLive.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "commonresource.h"
#include "Ztrace.h"
#include "SetUILanguage.h"
#include "tracecontrol.h"

//
// Set up tracing and manage trace file deletion
//
DSS::TraceControl traceControl{ std::source_location::current().file_name() };


bool	g_bShowRefStars = false;

// CDeepSkyStackerLiveApp

BEGIN_MESSAGE_MAP(CDeepSkyStackerLiveApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDeepSkyStackerLiveApp construction

CDeepSkyStackerLiveApp::CDeepSkyStackerLiveApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDeepSkyStackerLiveApp object

CDeepSkyStackerLiveApp theApp;

/* ------------------------------------------------------------------- */

CDeepSkyStackerLiveApp *		GetDSSLiveApp()
{
	return &theApp;
};

using namespace std;

/* ------------------------------------------------------------------- */
// CDeepSkyStackerLiveApp initialization

BOOL CDeepSkyStackerLiveApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	AfxInitRichEdit2();
	AfxSocketInit();

	CWinApp::InitInstance();

	// Standard initialization
	SetRegistryKey(_T("DeepSkyStacker5"));

	return FALSE;
}

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
	QMainWindow(),
	initialised{ false },
	winHost{ nullptr },
	args{ qApp->arguments() },
	baseTitle{ QString("DeepSkyStackerLive %1").arg(VERSION_DEEPSKYSTACKER) },
	statusBarText{ new QLabel("") },
	errorMessageDialog{ new QErrorMessage(this) },
	eMDI{ nullptr }		// errorMessageDialogIcon pointer
{
}

DeepSkyStackerLive::~DeepSkyStackerLive()
{
}

void DeepSkyStackerLive::createStatusBar()
{
	statusBarText->setAlignment(Qt::AlignHCenter);
	statusBar()->addWidget(statusBarText, 1);
	//connect(stackingDlg, SIGNAL(statusMessage(const QString&)), this, SLOT(updateStatus(const QString&)));
}

void DeepSkyStackerLive::updateStatus(const QString& text)
{
	statusBarText->setText(text);
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
QTranslator theQtTranslator;
QTranslator theAppTranslator;

int WINAPI _tWinMain(
	[[maybe_unused]] HINSTANCE hInstance,  // handle to current instance
	[[maybe_unused]] HINSTANCE hPrevInstance,  // handle to previous instance
	[[maybe_unused]] LPTSTR lpCmdLine,      // pointer to command line
	[[maybe_unused]] int nCmdShow          // show state of window
				   )
{
	ZFUNCTRACE_RUNTIME();
#if defined(_WINDOWS)
	// Set console code page to UTF-8 so console known how to interpret string data
	SetConsoleOutputCP(CP_UTF8);
#endif

	//
	// Silence the MFC memory leak dump as we use Visual Leak Detector.
	//
#if defined(_WINDOWS)
	_CrtSetDbgFlag(0);
#if !defined(NDEBUG)
	AfxEnableMemoryLeakDump(false);
#endif
#endif

	int nRetCode = 0;

	OleInitialize(nullptr);

	SetUILanguage();

	#ifndef NOGDIPLUS
	GdiplusStartupInput		gdiplusStartupInput;
	GdiplusStartupOutput	gdiSO;
	ULONG_PTR				gdiplusToken;
	ULONG_PTR				gdiHookToken;

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);
	#endif

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(nullptr), nullptr, ::GetCommandLine(), 0))
	{
		wcerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		theApp.InitInstance();
		QString cmdLine{ QString::fromWCharArray(theApp.m_lpCmdLine, wcslen(theApp.m_lpCmdLine)) };
		QStringList argList{ cmdLine.split(' ') };
		int argc = argList.count();
		char** argv = new char* [1 + argc];
		int i;
		for (i = 0; i < argc; ++i)
		{
			QString arg = argList[i];
			argv[i] = new char[arg.length() + 1];
			qstrcpy(argv[i], arg.toLocal8Bit().data());
		}
		argv[i] = 0;

		QApplication app(argc, argv);

		for (i = 0; i < argc; ++i)
		{
			char* arg = argv[i];
			delete[] arg;
		}
		delete[] argv;

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

		QSettings settings;
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

		QString translatorFileName = QLatin1String("qt_");
		translatorFileName += language;
		qDebug() << "qt translator filename: " << translatorFileName;

		qDebug() << "translationPath " << QLibraryInfo::path(QLibraryInfo::TranslationsPath);
		if (theQtTranslator.load(translatorFileName, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
		{
			app.installTranslator(&theQtTranslator);
		}

		translatorFileName = QLatin1String("DSSLive.");
		translatorFileName += language;
		qDebug() << "app translator filename: " << translatorFileName;
		//
		// Install the language if it actually exists.
		//
		if (theAppTranslator.load(translatorFileName, ":/i18n/"))
		{
			app.installTranslator(&theAppTranslator);
		}

		if (!hasExpired())
		{
			CLiveSettings liveSettings;
			liveSettings.LoadFromRegistry();

			CDeepSkyStackerLiveDlg	dlg(liveSettings.UseDarkTheme());

			theApp.m_pMainWnd = &dlg;
			//dlg.DragAcceptFiles(TRUE);
			dlg.DoModal();
		};
	}

	#ifndef NOGDIPLUS
	// Shutdown GDI+
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
	#endif

	OleUninitialize();

	return nRetCode;
};

/* ------------------------------------------------------------------- */