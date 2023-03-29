#pragma once

#define VC_EXTRALEAN					// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN7	// Want to support Windows XP and up
//
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

using std::min;
using std::max;

// Windows Files (eventaully to go!)
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxdlgs.h>
#include <afxcview.h>
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxpriv.h>
#include <afxsock.h>
#include <commctrl.h>
#include <gdiplus.h>
using namespace Gdiplus;
