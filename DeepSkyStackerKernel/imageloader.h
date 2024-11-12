#pragma once

class CMemoryBitmap;
class DeepSkyStackerLive;


namespace DSS
{
	class FileRegistrar;
	class FileStacker;
	class ImageViewer;
	class StackingDlg;
}

class LoadedImage
{
private:
	friend class ImageLoader;
	friend class DeepSkyStackerLive;
	friend class DSS::FileRegistrar;
	friend class DSS::FileStacker;
	friend class DSS::ImageViewer;
	friend class DSS::StackingDlg;

	std::shared_ptr<CMemoryBitmap>	m_pBitmap;
	std::shared_ptr<QImage>	m_Image;

public:
	LoadedImage() = default;
	explicit LoadedImage(auto&& pb, auto&& pi) :
		m_pBitmap{ std::forward<decltype(pb)>(pb)},
		m_Image{ std::forward<decltype(pi)>(pi)}
	{}
	LoadedImage(const LoadedImage&) noexcept = default;
	LoadedImage(LoadedImage&&) noexcept = default;
	~LoadedImage() = default;
	LoadedImage& operator=(const LoadedImage&) noexcept = default;
	LoadedImage& operator=(LoadedImage&&) noexcept = default;

	void reset()
	{
		m_Image.reset();
		m_pBitmap.reset();
	}
};

class ImageLoader : public QObject
{
	using CacheKeyType = std::filesystem::path;
	using CacheValueType = std::tuple<LoadedImage, int, bool>; // <image, lastUse, currentlyLoading>
	using CacheType = std::unordered_map<CacheKeyType, CacheValueType>;

	friend class ThreadLoader;
	Q_OBJECT

	static inline constexpr int16_t MAXIMAGESINCACHE = 20;
	static inline constinit std::atomic_int age{ 0 };
	static inline std::shared_mutex rwMutex{};
	static inline CacheType imageCache{};

private:
	void addOrUpdateCache(const CacheKeyType& key, LoadedImage&& loadedImage, const bool alreadyLoaded);
	void limitCacheSize();

public:
	ImageLoader() = default;
	virtual ~ImageLoader() = default;

	void	clearCache();
//	bool	load(const QString fileName, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage);
	bool	load(const fs::path file, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<QImage>& pImage);

signals:
	void imageLoaded(std::filesystem::path p);
	void imageLoadFailed();
};

class ThreadLoader : public QObject, public QRunnable
{
	Q_OBJECT

	void run() override;

public:
	ThreadLoader(const std::filesystem::path& p, ImageLoader* loader) :
		filepath{ p },
		imageLoader{ loader }
	{}

	virtual ~ThreadLoader() = default;

	std::filesystem::path filepath;
	ImageLoader* imageLoader;
};
