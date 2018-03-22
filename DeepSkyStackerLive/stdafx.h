// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#if !defined(AFX_STDAFX_H_)
#define AFX_STDAFX_H_

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//
// Want to support windows 7 and up
//
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>
#include <afxpriv.h>
#include <afxsock.h>
#include <WinDNS.h>
#include <WinCrypt.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <atlbase.h>

#include <windows.h>

#include <iostream>
#include <vector>

#include "DSSCommon.h"

//#define NOGDIPLUS
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "resource.h"
#include <afxdlgs.h>

#endif // !defined(AFX_STDAFX_H_)
