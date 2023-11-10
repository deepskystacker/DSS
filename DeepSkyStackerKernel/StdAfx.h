// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

// Visual Leak Detector
//
#include <vld.h>

// Qt Files
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QString>
#include <QImage>
#include <QLocale>
#include <QSettings>
#include <QGlobalStatic>
#include <QMutex>
#include <QPoint>
#include <QPointF>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QIcon>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMimeType>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QValidator>
#include <QThreadPool>
#include <QTextLayout>
#include <QPainter>
#include <QSortFilterProxyModel>
#include <QClipboard>
#include <QActionGroup>
#include <QMimeDatabase>
#include <QStandardPaths>

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
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace bip = boost::interprocess;
namespace fs = std::filesystem;

using std::min;
using std::max;

// As this interface is used everywhere for error reporting.
// If it got too big, or changed a lot, then we could move out to specific cpp files.
#include "dssbase.h"


// Windows Files (eventually to go!)
#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN10	// Want to support Windows 10 and up

#include <afx.h>
#include <zexcept.h>