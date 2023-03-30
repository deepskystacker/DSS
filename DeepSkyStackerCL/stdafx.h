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

// Standard Libraries
#include <concurrent_unordered_set.h>
#include <shared_mutex>
#include <omp.h>
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
#include <future>
#include <inttypes.h>
#include <filesystem>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace bip = boost::interprocess;
namespace fs = std::filesystem;

using std::min;
using std::max;

// Windows Files (eventually to go!)
#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN7 // Want to support windows 7 and up

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
#include <wtypesbase.h>
using namespace Gdiplus;