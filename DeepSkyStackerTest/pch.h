#pragma once
#define NOMINMAX

// Qt
#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
#include <QCoreApplication>
#include <QString>
#include <QPoint>
#include <QElapsedTimer>
#include <QDateTime>
#if defined (_MSC_VER)
#pragma warning(pop)
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


// Windows (to go!)
#if defined(Q_OS_WIN)
#include <WTypesbase.h>
#include <atlstr.h>
#include <cassert>
#include <atltypes.h>
#endif
