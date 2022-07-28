#pragma once
#include <memory>
#include <mutex>
#include <qobject.h>
#include <QRunnable> 
#include <qstring.h>

class CMemoryBitMap;
class C32BitsBitmap;

class LoadedImage
{
public:
	QString						fileName;
	std::shared_ptr<CMemoryBitmap>	m_pBitmap;
	std::shared_ptr<C32BitsBitmap>	m_hBitmap;
	int						lastUse;

private:
	void	CopyFrom(const LoadedImage& li)
	{
		fileName = li.fileName;
		m_pBitmap = li.m_pBitmap;
		m_hBitmap = li.m_hBitmap;
		lastUse = li.lastUse;
	};

public:
	LoadedImage() :
		lastUse(0)
	{
	};

	LoadedImage(const LoadedImage& li)
	{
		CopyFrom(li);
	};

	~LoadedImage()
	{
	};

	LoadedImage& operator = (const LoadedImage& li)
	{
		CopyFrom(li);
		return (*this);
	};

	bool operator < (const LoadedImage& li) const
	{
		return lastUse < li.lastUse;
	};

	void	reset()
	{
		m_hBitmap.reset();
		m_pBitmap.reset();
	};

};

class ImageLoader :
    public QObject
{
	friend class ThreadLoader;
	Q_OBJECT

    static const inline int16_t MAXIMAGESINCACHE{ 20 };
	std::mutex	mutex;
	QString	fileToLoad;
	std::vector<LoadedImage>	imageVector;

public:
	ImageLoader()
	{}
	virtual ~ImageLoader()
	{}

	void	clearCache();
	bool	load(QString fileName, std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<C32BitsBitmap>& phBitmap);

signals:
	void imageLoaded();
};

class ThreadLoader : public QObject, public QRunnable
{
	Q_OBJECT

	void run() override;

public:
	ThreadLoader(ImageLoader * loader ) :
		imageLoader(loader)
	{}

	virtual ~ThreadLoader()
	{}

	ImageLoader* imageLoader;
};