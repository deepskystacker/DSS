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
#include <afxcview.h>
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <commctrl.h>
#include <afxole.h>
#include <AFXPRIV.H>
#include <richole.h>  // for IRichEditOleCallback
#include <source_location>
#include <gdiplus.h>
using namespace Gdiplus;

// Qt Files
#include <QtWidgets>
#include <QNetworkReply>
#include <QStyleHints>

// Dependency Libraries
#include <exiv2/exiv2.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/easyaccess.hpp>

// Standard Libraries
#include <concurrent_unordered_set.h>
#include <shared_mutex>
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <deque>
#include <set>
#include <future>
#include <inttypes.h>
#include <filesystem>
#include <tuple>

#include "dssbase.h"

namespace fs = std::filesystem;
namespace chr = std::chrono;

using std::min;
using std::max;

#include <zexcept.h>

