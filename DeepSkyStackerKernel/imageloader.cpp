#include "stdafx.h"
#include "imageloader.h"
#include "ztrace.h"
#include "BitmapExt.h"
#include "zexcbase.h"


void ThreadLoader::run()
{
	ZFUNCTRACE_RUNTIME();

	if (!this->filepath.empty())
	{
		ZTRACE_RUNTIME("ThreadLoader: Trying to load picture %s", this->filepath.generic_u8string().c_str());
		imageLoader->addOrUpdateCache(this->filepath, LoadedImage{}, false);

		if (CAllDepthBitmap adb; LoadPicture(this->filepath, adb))
		{
			imageLoader->addOrUpdateCache(this->filepath, LoadedImage{ std::move(adb.m_pBitmap), std::move(adb.m_Image) }, true);
			emit(imageLoader->imageLoaded(this->filepath)); // Inform the GUI that we successfully loaded the image and stored it in the cache.
		}
		else
		{
			emit(imageLoader->imageLoadFailed());
		}

	}
}

void ImageLoader::clearCache()
{
	std::unique_lock writeLock{ rwMutex };
	this->imageCache.clear();
}

//bool ImageLoader::load(QString fileName, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage)
//{
//	fs::path p{ fileName.toStdU16String() };
//
//	return load(p, pBitmap, pImage);
//}

bool ImageLoader::load(const fs::path p, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage)
{
	ZFUNCTRACE_RUNTIME();

	auto type{ status(p).type() };

	if (fs::file_type::regular != type)
	{
		ZTHROW (ZAccessError("File not found"));
	}

	std::shared_lock rLock{ rwMutex };
	if (CacheType::iterator it = imageCache.find(p); it != imageCache.end())
	{
		++age;
		if (std::get<2>(it->second) == true) // Image - ready loading - found in cache.
		{
			ZTRACE_RUNTIME("Image file %s found in image cache", p.generic_u8string().c_str());
			std::get<1>(it->second) = age.load(); // Update age to "youngest" value.
			pBitmap = std::get<0>(it->second).m_pBitmap;
			pImage = std::get<0>(it->second).m_Image;
			return true;
		}
		// else image is still loading from disk.
	}
	else // Image not yet loaded.
	{
		QThreadPool::globalInstance()->start(new ThreadLoader(p, this)); // Request loading from disk in separate thread.
	}

	return false; // Image not in cache or still loading from disk.
}

void ImageLoader::addOrUpdateCache(const CacheKeyType& key, LoadedImage&& loadedImage, const bool alreadyLoaded)
{
	std::unique_lock writeLock{ rwMutex };
	this->imageCache.insert_or_assign(key, std::make_tuple(std::move(loadedImage), age++, alreadyLoaded));
	limitCacheSize();
}

// Note: This function does not lock the mutex, this must have been done before the call.
void ImageLoader::limitCacheSize()
{
	if (imageCache.size() <= MAXIMAGESINCACHE)
		return;
	CacheType::const_iterator it2oldest = imageCache.cbegin();
	for (CacheType::const_iterator it = imageCache.cbegin(); it != imageCache.cend(); ++it)
	{
		if (std::get<1>(it->second) < std::get<1>(it2oldest->second)) // Search for smallest (=oldest) age field in the cache.
			it2oldest = it;
	}
	imageCache.erase(it2oldest);
}
