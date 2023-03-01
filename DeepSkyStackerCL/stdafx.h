// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <type_traits>
using std::min;
using std::max;

// This is for CString's in BitmapExt.h :(
#include <atlstr.h>

#include <resource.h>
#include <ztrace.h>
#include "BitmapExt.h"

// Need to get rid of these soon!
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")