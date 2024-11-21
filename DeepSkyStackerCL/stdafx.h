#pragma once

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
#include <ranges>

#include "dssbase.h"

namespace bip = boost::interprocess;
namespace fs = std::filesystem;
namespace chr = std::chrono;

using std::min;
using std::max;

#include <zexcept.h>
