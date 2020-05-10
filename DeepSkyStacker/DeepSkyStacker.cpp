// DeepSkyStacker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DeepStack.h"
#include "DeepStackerDlg.h"
#include "Registry.h"
#include <gdiplus.h>
using namespace Gdiplus;
#include <afxinet.h>
#include "StackingTasks.h"
#include "StackRecap.h"
#include "cgfiltyp.h"
#include "SetUILanguage.h"
#include <zexcept.h>

#include "qmfcapp.h"

#include <QSettings>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString INPUTFILE_FILTERS;
CString OUTPUTFILE_FILTERS;
CString	OUTPUTLIST_FILTERS;
CString SETTINGFILE_FILTERS;
CString STARMASKFILE_FILTERS;
bool	g_bShowRefStars = false;

/* ------------------------------------------------------------------- */

bool	IsExpired()
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
#ifdef DSSBETA

	ZTRACE_RUNTIME("Check beta expiration\n");

	SYSTEMTIME			SystemTime;
	LONG				lMaxYear = DSSBETAEXPIREYEAR;
	LONG				lMaxMonth = DSSBETAEXPIREMONTH;

	GetSystemTime(&SystemTime);
	if ((SystemTime.wYear>lMaxYear) ||
		((SystemTime.wYear==lMaxYear) && (SystemTime.wMonth>lMaxMonth)))
	{
		AfxMessageBox(_T("This beta version has expired\nYou can probably get another one or download the final release from the web site."), MB_OK | MB_ICONSTOP);
		bResult = true;
	};

	ZTRACE_RUNTIME("Check beta expiration - ok\n");

#endif
	return bResult;
};

/* ------------------------------------------------------------------- */

bool CheckVersion(CString & strVersion)
{
	ZFUNCTRACE_RUNTIME();
	bool		bResult = false;

	#ifndef DSSBETA
	QSettings			reg;
	CStdioFile			*remotefile = nullptr;

	bool checkVersion = settings.value("InternetCheck"), false).toBool();
	if (checkVersion)
	
		#define HTTPBUFLEN    512 // Size of HTTP Buffer...
		char		httpbuff[HTTPBUFLEN];

		TRY
		{
			CInternetSession	mysession;

			mysession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
			mysession.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 1);

			remotefile = mysession.OpenURL(_T("http://deepskystacker.free.fr/download/CurrentVersion.txt"),1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);

			int numbytes;

			while (numbytes = remotefile->Read(httpbuff, HTTPBUFLEN))
			{
				for (LONG i = 0;i<numbytes;i++)
					strVersion += httpbuff[i];
			};

			if (strVersion.Find(_T("DeepSkyStackerVersion="))==0)
			{
				bResult = true;
				strVersion = strVersion.Right(strVersion.GetLength()-22);
			}
			else
				strVersion.Empty();
		}
		CATCH_ALL(error)
		{
			bResult = false;
		}
		END_CATCH_ALL;
	};
	delete remotefile;
	#endif

	return bResult;
}

/* ------------------------------------------------------------------- */

void	AskForVersionChecking()
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
		CString			strMsg;
		int				nResult;

		strMsg.LoadString(IDS_CHECKVERSION);

		nResult = AfxMessageBox(strMsg, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);
		if (nResult == IDYES)
			checkVersion = true;
		else
			checkVersion = false;
		settings.setValue("InternetCheck", checkVersion);
	};
};

/* ------------------------------------------------------------------- */

void	CheckRemainingTempFiles()
{
	ZFUNCTRACE_RUNTIME();
	CString					strFolder;
	WIN32_FIND_DATA			FindData;
	CString					strFileMask;
	HANDLE					hFindFiles;
	std::vector<CString>	vFiles;
	__int64					ulTotalSize = 0;

	ZTRACE_RUNTIME("Check remaining temp files\n");

	CAllStackingTasks::GetTemporaryFilesFolder(QString((QChar*)strFolder.GetBuffer()));
	strFileMask = strFolder;
	strFileMask += "DSS*.tmp";

	hFindFiles = FindFirstFile(strFileMask, &FindData);
	if (hFindFiles != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString			strFile;

			strFile = strFolder;
			strFile += FindData.cFileName;

			ulTotalSize += (__int64)(FindData.nFileSizeHigh * ((__int64)MAXDWORD+1)) + (__int64)FindData.nFileSizeLow;
			vFiles.push_back(strFile);
		}
		while (FindNextFile(hFindFiles, &FindData));

		FindClose(hFindFiles);
	};
	ZTRACE_RUNTIME("Check remaining temp files - ok\n");

	if (vFiles.size())
	{
		CString			strMsg;
		CString			strSize;
		int				nResult;

		ZTRACE_RUNTIME("Remove remaining temp files\n");

		SpaceToString(ulTotalSize, strSize);

		strMsg.Format(IDS_TEMPFILEREMOVAL, vFiles.size(), strSize);
		nResult = AfxMessageBox(strMsg, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);

		if (nResult == IDYES)
		{
			for (LONG i = 0;i<vFiles.size();i++)
				DeleteFile(vFiles[i]);
		};

		ZTRACE_RUNTIME("Remove remaining temp files - ok\n");
	};


};


/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// The one and only application object


// The CDeepSkyStacker class, a subclass of CWinApp, runs the event loop in the default implementation of Run().
// The MFC event loop is a standard Win32 event loop, but uses the CWinApp API PreTranslateMessage() to activate accelerators.
//
// In order to keep MFC accelerators working we must use the QApplication subclass QMfcApp that is provided by the Qt 
// Windows Migration framework and which merges the Qt and the MFC event loops.
// 
// The first step of the Qt migration is to reimplement the Run() function in the CDeepSkyStacker class.
// This is done by invoking the static run() function of the QMfcApp class to implicitly instantiate a QApplication object,
// and run the event loops for both Qt and MFC
//
/* ------------------------------------------------------------------- */
BOOL CDeepSkyStackerApp::Run()
{
	int result = QMfcApp::run(this);
	delete qApp;
	return result;
}

BOOL CDeepSkyStackerApp::InitInstance( )
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult;

	//
	// To be able to use Qt, we need to create a QApplication object.
    // The QApplication class controls the event delivery and display management for all Qt objects and widgets.
	//
	// Qt initialization
	QMfcApp::instance(this);

	//
	// Set up organisation etc. for QSettings usage
	//
	QCoreApplication::setOrganizationName("DeepSkyStacker");
	QCoreApplication::setOrganizationDomain("deepskystacker.free.fr");
	QCoreApplication::setApplicationName("DeepSkyStacker5");

	AfxInitRichEdit2();

	bResult = CWinApp::InitInstance();
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


	ZTRACE_RUNTIME("Reset dssfilelist extension association with DSS\n");

	CGCFileTypeAccess	FTA;
	TCHAR				szPath[1+_MAX_PATH];
	CString				strPath;
	CString				strTemp;

	::GetModuleFileName(nullptr, szPath, sizeof(szPath)/sizeof(TCHAR));
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
	strTemp  = strPath;
	strTemp += ",1";
	FTA.SetDocumentDefaultIcon(strTemp);

	// set the necessary registry entries
	FTA.RegSetAllInfo();
	ZTRACE_RUNTIME("Reset dssfilelist extension association with DSS - ok\n");


	ZTRACE_RUNTIME("Initialized QHTM\n");
	QHTM_Initialize( AfxGetInstanceHandle() );
	ZTRACE_RUNTIME("Initialized QHTM - ok\n");
	ZTRACE_RUNTIME("Initialized QHTM Cooltips\n");
	QHTM_EnableCooltips();
	ZTRACE_RUNTIME("Initialized QHTM Cooltips - ok\n");

	return bResult;
};

/* ------------------------------------------------------------------- */

CDeepSkyStackerApp theApp;

CDeepSkyStackerApp *		GetDSSApp()
{
	return &theApp;
};

using namespace std;

/* ------------------------------------------------------------------- */

int WINAPI _tWinMain(HINSTANCE hInstance,  // handle to current instance
				   HINSTANCE hPrevInstance,  // handle to previous instance
				   LPTSTR lpCmdLine,      // pointer to command line
				   int nCmdShow          // show state of window
				   )
{
	ZFUNCTRACE_RUNTIME();
	int				nRetCode = 0;
	HANDLE			hMutex;
	bool			bFirstInstance = true;

	ZTRACE_RUNTIME("Checking Mutex");

	hMutex = CreateMutex(nullptr, true, _T("DeepSkyStacker.Mutex.UniqueID.12354687"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		bFirstInstance = false;
	ZTRACE_RUNTIME("Checking Mutex - ok");

	ZTRACE_RUNTIME("AFXOleInit");

	if (!AfxOleInit())
	{
		cerr << _T("Fatal Error: AFXOleInit() failed") << endl;
		nRetCode = 1;
		return nRetCode;
	}

	OleInitialize(nullptr);
	ZTRACE_RUNTIME("OLE Initialize - ok");

	{
		bool		showRefStars = false;
		QSettings		settings;

		showRefStars = settings.value("ShowRefStars", false).toBool();

		g_bShowRefStars = showRefStars;
	}

	#ifndef NOGDIPLUS
	GdiplusStartupInput		gdiplusStartupInput;
	GdiplusStartupOutput	gdiSO;
	ULONG_PTR				gdiplusToken;
	ULONG_PTR				gdiHookToken;

	ZTRACE_RUNTIME("Initialize GDI+");

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = true;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	ZTRACE_RUNTIME("Initialize GDI+ - ok");

	#endif

	ZTRACE_RUNTIME("Initialize Application");

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(nullptr), nullptr, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		theApp.InitInstance();

		ZTRACE_RUNTIME("Initialize Application - ok");

		ZTRACE_RUNTIME("Set UI Language");

		SetUILanguage();

		ZTRACE_RUNTIME("Set UI Language - ok");


		INPUTFILE_FILTERS.LoadString(IDS_FILTER_INPUT);
		OUTPUTFILE_FILTERS.LoadString(IDS_FILTER_OUTPUT);
		OUTPUTLIST_FILTERS.LoadString(IDS_LISTFILTER_OUTPUT);
		SETTINGFILE_FILTERS.LoadString(IDS_FILTER_SETTINGFILE);
		STARMASKFILE_FILTERS.LoadString(IDS_FILTER_MASK);

		if (!IsExpired())
		{
			#ifndef DSSBETA
			AskForVersionChecking();
			#endif
			if (bFirstInstance)
				CheckRemainingTempFiles();

			ZTRACE_RUNTIME("Creating Main Window");

			CDeepStackerDlg		dlg;
			CString				strStartFileList = lpCmdLine;

			if (strStartFileList.GetLength())
			{
				strStartFileList.TrimLeft(_T("\""));
				strStartFileList.TrimRight(_T("\""));
			};

			theApp.m_pMainDlg = &dlg;
			ZTRACE_RUNTIME("Creating Main Window - ok");

			ZTRACE_RUNTIME("Set Starting File List");
			dlg.SetStartingFileList(strStartFileList);
			ZTRACE_RUNTIME("Set Starting File List - ok");
			ZTRACE_RUNTIME("Going modal...");
			try
			{
				dlg.DoModal();
			}
			catch (std::exception & e)
			{
				CString errorMessage(static_cast<LPCTSTR>(CA2CT(e.what())));
#if defined(_CONSOLE)
				std::cerr << errorMessage;
#else
				AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
			}
			catch (CException & e)
			{
				e.ReportError();
				e.Delete();
			}
			catch (ZException & ze)
			{
				CString errorMessage;
				CString name(CA2CT(ze.name()));
				CString fileName(CA2CT(ze.locationAtIndex(0)->fileName()));
				CString functionName(CA2CT(ze.locationAtIndex(0)->functionName()));
				CString text(CA2CT(ze.text(0)));

				errorMessage.Format(
					_T("Exception %s thrown from %s Function: %s() Line: %lu\n\n%s"),
					name,
					fileName,
					functionName,
					ze.locationAtIndex(0)->lineNumber(),
					text);
#if defined(_CONSOLE)
					std::cerr << errorMessage;
#else
					AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
			}
			catch (...)
			{
				CString errorMessage(_T("Unknown exception caught"));
#if defined(_CONSOLE)
				std::cerr << errorMessage;
#else
				AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif

			}


			ZTRACE_RUNTIME("Ending modal...");
		};
	}

	#ifndef NOGDIPLUS
	// Shutdown GDI+
	ZTRACE_RUNTIME("Shutting down GDI+");
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
	ZTRACE_RUNTIME("Shutting down GDI+ - ok");
	#endif

	ZTRACE_RUNTIME("Shutting down QHTM");
	QHTM_Uninitialize();

	OleUninitialize();

	CloseHandle(hMutex);

	return nRetCode;
};

/* ------------------------------------------------------------------- */
