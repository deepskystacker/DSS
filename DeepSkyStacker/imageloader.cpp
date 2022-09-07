#include <vld.h>

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//
// Want to support windows 7 and up
//
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <atlbase.h>

//#include <stdlib.h>

#include <windows.h>
#include <commctrl.h>

#include <algorithm>
using std::min;
using std::max;

#include "Ztrace.h"

#include "DSSCommon.h"
#include "BitmapExt.h"
//#include "DSSMemory.h"
#include <QThreadPool>

#include "imageloader.h"

void ThreadLoader::run()
{
	ZFUNCTRACE_RUNTIME();

	QString strImage(imageLoader->fileToLoad);

	if (strImage.size())
	{
		CAllDepthBitmap				adb;

		if (LoadPicture((LPCTSTR)strImage.utf16(), adb))
		{
			const std::lock_guard <std::mutex> lock { imageLoader->mutex };

	   		LoadedImage			li;

			li.m_Image = adb.m_Image;
			li.m_pBitmap = adb.m_pBitmap;
			li.fileName = imageLoader->fileToLoad;
			li.lastUse = 0;
			imageLoader->imageVector.push_back(li);
		};
		emit(imageLoader->imageLoaded());
	};
}

void ImageLoader::clearCache()
{
	std::lock_guard lock(mutex);
	imageVector.clear();
}

bool ImageLoader::load(QString fileName, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage)
{
	bool found(false), result(false);
	ZFUNCTRACE_RUNTIME();
	std::lock_guard lock(mutex);

	//
	// Search the images we have stored in our cache, if found return the bitmap pointers.
	//
	for (LoadedImage& image : imageVector)
	{
		if (image.fileName == fileName)
		{
			pBitmap = image.m_pBitmap;		// Return the pointer stored in the shared ptr
			pImage = image.m_Image;		// Return the pointer stored in the shared ptr
			found = true;
		}
		else
		{
			image.lastUse++;
		};
	}

	// Did we find the image in our cache?
	if (found)
	{
		ZTRACE_RUNTIME("Image file %s found in image cache", fileName.toLocal8Bit().constData());
		result = true;
		if (imageVector.size() > MAXIMAGESINCACHE)
		{
			// Remove the last images from the cache
			std::sort(imageVector.begin(), imageVector.end());
			imageVector.resize(MAXIMAGESINCACHE);
		}
	}
	else
	{
		ZTRACE_RUNTIME("Loading image file %s in thread", fileName.toLocal8Bit().constData());
		fileToLoad = fileName;
		ThreadLoader* threadLoader(new ThreadLoader(this));
		QThreadPool::globalInstance()->start(threadLoader);
	}
	
	return result;
}

