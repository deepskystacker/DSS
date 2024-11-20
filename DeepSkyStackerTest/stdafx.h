#pragma once
#define NOMINMAX

// Qt
#include <QCoreApplication>
#include <QString>
#include <QPoint>
#include <QElapsedTimer>
#include <QDateTime>

// Std
#include <vector>
#include <algorithm>
#include <mutex>
#include <deque>
#include <set>
#include <numeric>
#include "../DeepSkyStackerKernel/avx_includes.h"
#include <limits>
#include <iostream>
#include <filesystem>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <ranges>

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
