#pragma once
#if defined(_WINDOWS)
//
// Visual Leak Detector
//
#include <vld.h>
#endif

// Windows Files (eventually to go!)
#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN10	// Want to support Windows 10 and up

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

// Qt Files
#include <QtWidgets>
#include <QNetworkReply>

// Standard Libraries
#include <omp.h>
#include <concurrent_unordered_set.h>
#include <shared_mutex>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <deque>
#include <set>
#include <tuple>
#include <inttypes.h>
#include <filesystem>
namespace fs = std::filesystem;
namespace chr = std::chrono;

#include "dssbase.h"

using std::min;
using std::max;

