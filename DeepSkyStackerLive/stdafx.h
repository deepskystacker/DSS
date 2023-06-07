#pragma once
#if defined(_WINDOWS)
//
// Visual Leak Detector
//
#include <vld.h>
#endif

// Windows Files (eventually to go!)
#if defined (_WINDOWS)
#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN10	// Want to support Windows 10 and up
#include <afx.h>
#endif

// Qt Files
#include <QtWidgets>
#include <QNetworkReply>

// ZClass Files
#include <zexcept.h>
#include <Ztrace.h>

// Dependency Libraries
#include <exiv2/exiv2.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/easyaccess.hpp>

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

#include "dssliveenums.h"

