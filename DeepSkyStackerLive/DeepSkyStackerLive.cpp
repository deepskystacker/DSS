// DeepSkyStackerLive.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include "Ztrace.h"
#include "./../DeepSkyStacker/SetUILanguage.h"	// Explicit include so not to pull over all headers in DSS if we added just a new include path.

bool	g_bShowRefStars = false;


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
	statusBarText{ new QLabel("") }

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

void DeepSkyStackerLive::displayMessage(const QString& message, QMessageBox::Icon icon)
{
	QMessageBox msgBox{ icon, "DeepSkyStacker", message, QMessageBox::Ok , this };
	msgBox.exec();
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

		LoadTranslations();

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