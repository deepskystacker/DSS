#pragma once
#if defined(_WINDOWS)
//
// Visual Leak Detector
//
#include <vld.h>
#endif

// Windows Files (eventually to go!)
#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN10 // Want to support windows 10 and up

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

#include <QtCore>

#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QAbstractItemModel>
#include <QIcon>
#include <QImageReader>

// Dependency Libraries
#include <exiv2/exiv2.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/easyaccess.hpp>

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

#include "dssbase.h"

namespace bip = boost::interprocess;
namespace fs = std::filesystem;
namespace chr = std::chrono;

using std::min;
using std::max;

#include <zexcept.h>
