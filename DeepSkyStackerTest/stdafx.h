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
#include <immintrin.h>
#include <limits>
#include <iostream>
#include <filesystem>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace bip = boost::interprocess;
namespace fs = std::filesystem;

using std::min;
using std::max;

// Misc global headers.
#include <omp.h>

#include "../ZCLass/zdefs.h"
#include "../ZCLass/Ztrace.h"
#include "../ZCLass/zexcept.h"


// Windows (to go!)
#include <WTypesbase.h>
#include <atlstr.h>
#include <cassert>
#include <atltypes.h>
