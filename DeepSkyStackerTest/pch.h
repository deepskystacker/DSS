#pragma once
#define NOMINMAX

//
// Visual Leak Detector
// 
#if defined(_WINDOWS) && __has_include(<vld.h>)
//
// Visual Leak Detector
//
#include <vld.h>
#else 
#define VLDEnable()
#define VLDDisable()
#endif

// Qt
#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
#endif
#include <QCoreApplication>
#include <QString>
#include <QPoint>
#include <QElapsedTimer>
#include <QDateTime>
#include <QTimeZone>
#if defined (_MSC_VER)
#pragma warning(pop)
#pragma warning(disable: 4251)
#endif

#include "../DeepSkyStackerKernel/avx_includes.h"

// Std
#include <vector>
#include <algorithm>
#include <mutex>
#include <deque>
#include <set>
#include <numeric>
#include <limits>
#include <iostream>
#include <filesystem>
#include <ranges>

// Boost
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


namespace bip = boost::interprocess;
namespace fs = std::filesystem;

using std::min;
using std::max;

// Misc global headers.
#include <omp.h>

#include "../ZClass/zdefs.h"
#include "../ZClass/ztrace.h"
#include "../ZClass/zexcept.h"
