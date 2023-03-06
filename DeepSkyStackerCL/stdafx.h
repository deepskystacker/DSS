// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QString>
#include <QImage>
#include <QLocale>
#include <QSettings>
#include <QGlobalStatic>
#include <QMutex>
#include <QPoint>
#include <QPointF>
#include <QAbstractItemModel>
#include <QIcon>
#include <QElapsedTimer>

#include <tchar.h>
#include <vector>
#include <tuple>
#include <deque>
#include <set>
#include <list>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <float.h>
#include <cmath>
using std::min;
using std::max;

// This is for CString's in BitmapExt.h :(
//#include <atlstr.h>

// #include <resource.h>
// #include <ztrace.h>
// #include "BitmapExt.h"

// Windows Files (eventaully to go!)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxdlgs.h>
#include <afxcview.h>
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxpriv.h>
#include <afxsock.h>
#include <commctrl.h>
#include <gdiplus.h>
using namespace Gdiplus;