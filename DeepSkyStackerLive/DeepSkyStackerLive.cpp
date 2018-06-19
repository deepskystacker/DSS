// DeepSkyStackerLive.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "DeepSkyStackerLiveDlg.h"
#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
#include "registry.h"
#include "SetUILanguage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	SetRegistryKey(_T("DeepSkyStacker"));

	return FALSE;
}

/* ------------------------------------------------------------------- */

BOOL	IsExpired()
{
	BOOL				bResult = FALSE;
/*
#ifdef DSSBETA
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
#endif
*/
	return bResult;
};

/* ------------------------------------------------------------------- */

int WINAPI _tWinMain(HINSTANCE hInstance,  // handle to current instance
				   HINSTANCE hPrevInstance,  // handle to previous instance
				   LPTSTR lpCmdLine,      // pointer to command line
				   int nCmdShow          // show state of window
				   )
{
	ZFUNCTRACE_RUNTIME();
	int nRetCode = 0;

	OleInitialize(NULL);

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
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		theApp.InitInstance();

		if (!IsExpired())
		{
			CDeepSkyStackerLiveDlg	dlg;

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