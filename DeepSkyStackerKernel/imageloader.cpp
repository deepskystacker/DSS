#include "stdafx.h"
#include "imageloader.h"
#include "Ztrace.h"
#include "BitmapExt.h"
#include "ZExcBase.h"


void ThreadLoader::run()
{
	ZFUNCTRACE_RUNTIME();

	if (!imageLoader->fileToLoad.empty())
	{
		CAllDepthBitmap				adb;

		if (LoadPicture(imageLoader->fileToLoad, adb))
		{
			const std::lock_guard <std::mutex> lock { imageLoader->mutex };

	   		LoadedImage			li;

			li.m_Image = adb.m_Image;
			li.m_pBitmap = adb.m_pBitmap;
			li.fileName = imageLoader->fileToLoad;
			li.lastUse = 0;
			imageLoader->imageVector.push_back(li);
			emit(imageLoader->imageLoaded());
		}
		else
		{
			emit(imageLoader->imageLoadFailed());
		}

	}
}

void ImageLoader::clearCache()
{
	std::lock_guard lock(mutex);
	imageVector.clear();
}

bool ImageLoader::load(QString fileName, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage)
{
	fs::path p{ fileName.toStdU16String() };

	return load(p, pBitmap, pImage);
}

bool ImageLoader::load(const fs::path& p, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage)
{
	ZFUNCTRACE_RUNTIME();

	auto type{ status(p).type() };

	if (fs::file_type::regular != type)
	{
		ZTHROW (ZAccessError("File not found"));
	}
	bool found(false), result(false);
	std::lock_guard lock(mutex);

	//
	// Search the images we have stored in our cache, if found return the bitmap pointers.
	//
	for (LoadedImage& image : imageVector)
	{
		if (image.fileName == p)
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
		ZTRACE_RUNTIME("Image file %s found in image cache", p.generic_u8string().c_str());
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
		ZTRACE_RUNTIME("Loading image file %s in thread", p.generic_u8string().c_str());
		fileToLoad = p;
		ThreadLoader* threadLoader(new ThreadLoader(this));
		QThreadPool::globalInstance()->start(threadLoader);
	}
	
	return result;
}

