// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C280BBDC_7360_4663_B015_7C2BE32EF97A__INCLUDED_)
#define AFX_STDAFX_H__C280BBDC_7360_4663_B015_7C2BE32EF97A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(USE_HOARD)
#pragma comment(lib, "lib\\winhoard.lib")
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//
// Want to support windows 7 and up
//
#define _WIN32_WINNT _WIN32_WINNT_WIN7

// Visual Leak Detector
//
//#include <vld.h>

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>

#include <stdlib.h>
#ifndef NDEBUG
#include <crtdbg.h>
#endif

#include <windows.h>
#include <commctrl.h>

#include <algorithm>
using std::min;
using std::max;

#include <iostream>
#include <vector>

#include "DSSCommon.h"
#include "DSSMemory.h"
#include "Ztrace.h"

extern CString INPUTFILE_FILTERS;
extern CString OUTPUTFILE_FILTERS;
extern CString OUTPUTLIST_FILTERS;
extern CString SETTINGFILE_FILTERS;
extern CString STARMASKFILE_FILTERS;

extern BOOL		g_bShowRefStars;

BOOL CheckVersion(CString & strVersion);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "resource.h"
#include "commonresource.h"
#include <afxdlgs.h>
#include "..\QHTML_Static\QHTM\QHTM.h"

#include "BitmapExt.h"
#include "DeepStackerDlg.h"
#include "Utils.h"

#endif // !defined(AFX_STDAFX_H__C280BBDC_7360_4663_B015_7C2BE32EF97A__INCLUDED_)
