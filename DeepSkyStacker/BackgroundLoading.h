#ifndef __BACKGROUNDLOADING_H__
#define __BACKGROUNDLOADING_H__

/* ------------------------------------------------------------------- */

#include "DSSTools.h"
#include "DSSProgress.h"
#include "BitmapExt.h"

constexpr size_t MAXIMAGESINCACHE = 2;
constexpr unsigned int WM_BACKGROUNDIMAGELOADED = WM_USER + 100;

class CLoadedImage
{
public:
	CString m_strName;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;
	std::shared_ptr<C32BitsBitmap> m_hBitmap;
	int m_lLastUse;

private:
	void CopyFrom(const CLoadedImage& li)
	{
		m_strName		= li.m_strName;
		m_pBitmap		= li.m_pBitmap;
		m_hBitmap		= li.m_hBitmap;
		m_lLastUse		= li.m_lLastUse;
	}

public:
	CLoadedImage() : m_lLastUse{ 0 } {}

	CLoadedImage(const CLoadedImage&) = default;

	~CLoadedImage()
	{}

	CLoadedImage& operator=(const CLoadedImage& li) = default;

	bool operator< (const CLoadedImage& li) const
	{
		return m_lLastUse < li.m_lLastUse;
	}

	void Clear()
	{
		m_hBitmap.reset();
		m_pBitmap.reset();
	}
};

/* ------------------------------------------------------------------- */

typedef std::vector<CLoadedImage> LOADEDIMAGEVECTOR;
typedef	LOADEDIMAGEVECTOR::iterator LOADEDIMAGEITERATOR;

/* ------------------------------------------------------------------- */

class CBackgroundLoading
{
private:
	CComAutoCriticalSection	m_CriticalSection;
	LOADEDIMAGEVECTOR		m_vLoadedImages;
	CString					m_strToLoad;
	HANDLE					m_hThread;
	unsigned long			m_dwThreadID;
	HANDLE					m_hEvent;
	HWND					m_hWnd;

private:
	void	CloseThread();
	void	LoadImageInBackground(LPCTSTR szImage);
	void	LoadCurrentImage();

public:
	CBackgroundLoading();
	virtual ~CBackgroundLoading();

	void	SetWindow(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	void	ClearList();
	void	BackgroundLoad();
	bool	LoadImage(LPCTSTR szImage, std::shared_ptr<CMemoryBitmap>& rpBitmap, std::shared_ptr<C32BitsBitmap>& rphBitmap);
};

/* ------------------------------------------------------------------- */

#endif