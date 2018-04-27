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

#pragma comment(lib, "gdiplus.lib")

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
BOOL	g_bShowRefStars = FALSE;

/* ------------------------------------------------------------------- */

BOOL	IsExpired()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
/*
#ifdef DSSBETA

	ZTRACE_RUNTIME("Check beta expiration\n");

	SYSTEMTIME			SystemTime;
	LONG				lMaxYear = DSSBETAEXPIREYEAR;
	LONG				lMaxMonth = DSSBETAEXPIREMONTH;

	GetSystemTime(&SystemTime);
	if ((SystemTime.wYear>lMaxYear) || 
		((SystemTime.wYear==lMaxYear) && (SystemTime.wMonth>lMaxMonth)))
	{
		AfxMessageBox("This beta version has expired\nYou can probably get another one or download the final release from the web site.", MB_OK | MB_ICONSTOP);
		bResult = TRUE;
	};

	ZTRACE_RUNTIME("Check beta expiration - ok\n");

#endif
*/
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CheckVersion(CString & strVersion)
{
	ZFUNCTRACE_RUNTIME();
	BOOL		bResult = FALSE;
	
	#ifndef DSSBETA
	CRegistry			reg;
	DWORD				bCheckVersion = 0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);
	if (bCheckVersion == 2)
	{
		#define HTTPBUFLEN    512 // Size of HTTP Buffer...
		char		httpbuff[HTTPBUFLEN];

		TRY
		{
			CInternetSession	mysession;

			mysession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
			mysession.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 1);

			CStdioFile *remotefile = mysession.OpenURL(_T("http://deepskystacker.free.fr/download/CurrentVersion.txt"),1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);

			int numbytes;

			while (numbytes = remotefile->Read(httpbuff, HTTPBUFLEN))
			{
				for (LONG i = 0;i<numbytes;i++)
					strVersion += httpbuff[i];
			};

			if (strVersion.Find(_T("DeepSkyStackerVersion="))==0)
			{
				bResult = TRUE;
				strVersion = strVersion.Right(strVersion.GetLength()-22);
			}
			else
				strVersion.Empty();
		}
		CATCH_ALL(error)
		{
			bResult = FALSE;
		}
		END_CATCH_ALL;
	};
	#endif

	return bResult;
}

/* ------------------------------------------------------------------- */

void	AskForVersionChecking()
{
	ZFUNCTRACE_RUNTIME();
	CRegistry			reg;
	DWORD				bCheckVersion = 0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);

	if (!bCheckVersion)
	{
		CString			strMsg;
		int				nResult;

		strMsg.LoadString(IDS_CHECKVERSION);

		nResult = AfxMessageBox(strMsg, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);
		if (nResult == IDYES)
			bCheckVersion = 2;
		else
			bCheckVersion = 1;
		reg.SaveKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);
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

	CAllStackingTasks::GetTemporaryFilesFolder(strFolder);
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

/* ------------------------------------------------------------------- */

BOOL CDeepSkyStackerApp::InitInstance( )
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult;

	AfxInitRichEdit2();

	bResult = CWinApp::InitInstance();
	SetRegistryKey(_T("DeepSkyStacker"));

	
	ZTRACE_RUNTIME("Reset dssfilelist extension association with DSS\n");

	CGCFileTypeAccess	FTA;
	TCHAR				szPath[1+_MAX_PATH];
	CString				strPath;
	CString				strTemp;
	
	::GetModuleFileName(NULL, szPath, sizeof(szPath)/sizeof(TCHAR));
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

	hMutex = CreateMutex(NULL, TRUE, _T("DeepSkyStacker.Mutex.UniqueID.12354687"));
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

	OleInitialize(NULL);
	ZTRACE_RUNTIME("OLE Initialize - ok");

	ZTRACE_RUNTIME("Set UI Language");

	SetUILanguage();

	ZTRACE_RUNTIME("Set UI Language - ok");

	{
		DWORD			dwShowRefStars = 0;
		CRegistry		reg;

		reg.LoadKey(REGENTRY_BASEKEY, _T("ShowRefStars"), dwShowRefStars);

		g_bShowRefStars = dwShowRefStars;
	}

	#ifndef NOGDIPLUS
	GdiplusStartupInput		gdiplusStartupInput;
	GdiplusStartupOutput	gdiSO;
	ULONG_PTR				gdiplusToken;
	ULONG_PTR				gdiHookToken;

	ZTRACE_RUNTIME("Initialize GDI+");

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	ZTRACE_RUNTIME("Initialize GDI+ - ok");

	#endif

	ZTRACE_RUNTIME("Initialize Application");

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		theApp.InitInstance();
		
		ZTRACE_RUNTIME("Initialize Application - ok");

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
			dlg.DoModal();
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
