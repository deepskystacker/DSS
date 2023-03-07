// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma once

#if defined(USE_HOARD)
#pragma comment(lib, "lib\\winhoard.lib")
#endif

#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN7 // Want to support windows 7 and up

// Visual Leak Detector
//
#include <vld.h>

// Qt Files
#include <QApplication>
#include <QDialog>
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
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QStatusBar>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMimeType>
#include <QStackedWidget>
#include <QWidget>
#include <QTranslator>
#include <QStyleFactory>
#include <QLibraryInfo>
#include <QDockWidget>
#include <QMenu>
#include <QStandardPaths>
#include <QValidator>
#include <QStyledItemDelegate>
#include <QThreadPool>
#include <QTextLayout>
#include <QPainter>
#include <QTimeEdit>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QToolTip>
#include <QClipboard>
#include <QToolBar>
#include <QGraphicsOpacityEffect>
#include <QActionGroup>
#include <QNetworkReply>
#include <QErrorMessage>
#include <QDialog>
//#include <QFlags>

// Standard Libraries
#pragma warning( push )
#pragma warning( disable: 4996 ) // Remove this as soon as having switched to c++20.
#include <concurrent_unordered_set.h>
#include <shared_mutex>
#pragma warning( pop )

#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <deque>
#include <set>
#include <inttypes.h>
#include <filesystem>
namespace fs = std::filesystem;

using std::min;
using std::max;

// Misc global headers.
#include <omp.h>

// Windows Files (eventaully to go!)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <commctrl.h>
#include <gdiplus.h>
using namespace Gdiplus;
