// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#if !defined(AFX_STDAFX_H_)
#define AFX_STDAFX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <algorithm>
using std::min;
using std::max;

//
// Want to support Windows XP and up
//
#define _WIN32_WINNT _WIN32_WINNT_WIN7

//
// Visual Leak Detector
//
#include <vld.h>

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
#include "DSSMemory.h"
#include "Ztrace.h"

//#define NOGDIPLUS
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include "resource.h"
#include <afxdlgs.h>

#include "BitmapExt.h"

#endif // !defined(AFX_STDAFX_H_)
