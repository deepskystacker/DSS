#pragma once
// Qt Files
#include <QtWidgets>
#include <QNetworkReply>
#include <QStyleHints>

// Dependency Libraries
#include <exiv2/exiv2.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/easyaccess.hpp>

// Standard Libraries
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
#include <ranges>

#include "dssbase.h"

namespace fs = std::filesystem;
namespace chr = std::chrono;

using std::min;
using std::max;

#include <zexcept.h>
#include <ztrace.h>

