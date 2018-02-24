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
	BOOL				bResult = FALSE;
/*
#ifdef DSSBETA

	AddToLog("Check beta expiration\n");

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

	AddToLog("Check beta expiration - ok\n");

#endif
*/
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CheckVersion(CString & strVersion)
{
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

			CStdioFile *remotefile = mysession.OpenURL("http://deepskystacker.free.fr/download/CurrentVersion.txt",1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);

			int numbytes;

			while (numbytes = remotefile->Read(httpbuff, HTTPBUFLEN))
			{
				for (LONG i = 0;i<numbytes;i++)
					strVersion += httpbuff[i];
			};

			if (strVersion.Find("DeepSkyStackerVersion=")==0)
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
	CString					strFolder;
	WIN32_FIND_DATA			FindData;
	CString					strFileMask;
	HANDLE					hFindFiles;
	std::vector<CString>	vFiles;
	__int64					ulTotalSize = 0;
	
	AddToLog("Check remaining temp files\n");

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
	AddToLog("Check remaining temp files - ok\n");

	if (vFiles.size())
	{
		CString			strMsg;
		CString			strSize;
		int				nResult;

		AddToLog("Remove remaining temp files\n");

		SpaceToString(ulTotalSize, strSize);

		strMsg.Format(IDS_TEMPFILEREMOVAL, vFiles.size(), strSize);
		nResult = AfxMessageBox(strMsg, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION);

		if (nResult == IDYES)
		{
			for (LONG i = 0;i<vFiles.size();i++)
				DeleteFile(vFiles[i]);
		};

		AddToLog("Remove remaining temp files - ok\n");
	};


};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

/* ------------------------------------------------------------------- */

BOOL CDeepSkyStackerApp::InitInstance( )
{
	BOOL			bResult;

	AfxInitRichEdit2();

	bResult = CWinApp::InitInstance();
	SetRegistryKey("DeepSkyStacker");

	if (!IsWin98())
	{
		AddToLog("Reset dssfilelist extension association with DSS\n");

		CGCFileTypeAccess	FTA;
		TCHAR				szPath[_MAX_PATH];
		CString				strPath;
		CString				strTemp;

		::GetModuleFileName(NULL, szPath, sizeof(szPath));
		strPath = szPath;

		FTA.SetExtension("dssfilelist");

		strTemp = strPath;
		strTemp += " \"%1\"";
		FTA.SetShellOpenCommand(strTemp);
		FTA.SetDocumentShellOpenCommand(strTemp);

		FTA.SetDocumentClassName("DeepSkyStacker.FileList");
		
		CString				strFileListDescription;

		strFileListDescription.LoadString(IDS_FILELISTDESCRIPTION);

		FTA.SetDocumentDescription(strFileListDescription);

		// use first icon in program
		strTemp  = strPath;
		strTemp += ",1";
		FTA.SetDocumentDefaultIcon(strTemp);

		// set the necessary registry entries	
		FTA.RegSetAllInfo();
		AddToLog("Reset dssfilelist extension association with DSS - ok\n");
	};

	AddToLog("Initialized QHTM\n");
	QHTM_Initialize( AfxGetInstanceHandle() );
	AddToLog("Initialized QHTM - ok\n");
	AddToLog("Initialized QHTM Cooltips\n");
	QHTM_EnableCooltips();
	AddToLog("Initialized QHTM Cooltips - ok\n");

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

int WINAPI WinMain(HINSTANCE hInstance,  // handle to current instance
				   HINSTANCE hPrevInstance,  // handle to previous instance
				   LPSTR lpCmdLine,      // pointer to command line
				   int nCmdShow          // show state of window
				   )
{
	int				nRetCode = 0;
	HANDLE			hMutex;
	bool			bFirstInstance = true;

	StartLog();
	AddToLog("Checking Mutex\n");

	hMutex = CreateMutex(NULL, TRUE, "DeepSkyStacker.Mutex.UniqueID.12354687");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		bFirstInstance = false;
	AddToLog("Checking Mutex - ok\n");

	AddToLog("OLE Initialize\n");

	OleInitialize(NULL);
	AddToLog("OLE Initialize - ok\n");

	AddToLog("Set UI Language\n");

	SetUILanguage();

	AddToLog("Set UI Language - ok\n");

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

	AddToLog("Initialize GDI+\n");

	// Initialize GDI+.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, &gdiSO);
	gdiSO.NotificationHook(&gdiHookToken);

	AddToLog("Initialize GDI+ - ok\n");

	#endif

	AddToLog("Initialize Application\n");

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		theApp.InitInstance();
		
		AddToLog("Initialize Application - ok\n");

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

			AddToLog("Creating Main Window\n");

			CDeepStackerDlg		dlg;
			CString				strStartFileList = lpCmdLine;

			if (strStartFileList.GetLength())
			{
				strStartFileList.TrimLeft("\"");
				strStartFileList.TrimRight("\"");
			};

			theApp.m_pMainDlg = &dlg;
			AddToLog("Creating Main Window - ok\n");

			dlg.DragAcceptFiles(TRUE);

			AddToLog("Set Starting File List\n");
			dlg.SetStartingFileList(strStartFileList);
			AddToLog("Set Starting File List - ok\n");
			AddToLog("Going modal...\n");
			dlg.DoModal();
			AddToLog("Ending modal...\n");
		};
	}

	#ifndef NOGDIPLUS
	// Shutdown GDI+
	AddToLog("Shutting down GDI+\n");
	gdiSO.NotificationUnhook(gdiHookToken);
	GdiplusShutdown(gdiplusToken);
	AddToLog("Shutting down GDI+ - ok\n");
	#endif

	OleUninitialize();

	CloseHandle(hMutex);

	return nRetCode;
};

/* ------------------------------------------------------------------- */
