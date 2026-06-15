#pragma once

// Qt Files
#if defined (_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
#include <QtWidgets>
#include <QNetworkReply>
#if defined (_MSC_VER)
#pragma warning(pop)
#endif


// ZClass Files
#include <zexcept.h>
#include <ztrace.h>

// Dependency Libraries
#include <exiv2/exiv2.hpp>
#include <exiv2/exif.hpp>
#include <exiv2/easyaccess.hpp>

// Standard Libraries
#include <omp.h>
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

