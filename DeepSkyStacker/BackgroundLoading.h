#ifndef __BACKGROUNDLOADING_H__
#define __BACKGROUNDLOADING_H__

/* ------------------------------------------------------------------- */

#include "DSSTools.h"
#include "DSSProgress.h"
#include "BitmapExt.h"

const	DWORD					MAXIMAGESINCACHE		 = 2;
const	DWORD					WM_BACKGROUNDIMAGELOADED = (WM_USER+100);

class CLoadedImage
{
public :
	CString						m_strName;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<C32BitsBitmap>	m_hBitmap;
	LONG						m_lLastUse;

private :
	void	CopyFrom(const CLoadedImage & li)
	{
		m_strName		= li.m_strName;
		m_pBitmap		= li.m_pBitmap;
		m_hBitmap		= li.m_hBitmap;
		m_lLastUse		= li.m_lLastUse;
	};

public :
	CLoadedImage()
	{
		m_hBitmap	= NULL;
		m_lLastUse	= 0;
	};

	CLoadedImage(const CLoadedImage & li)
	{
		CopyFrom(li);
	};

	~CLoadedImage()
	{
	};

	CLoadedImage & operator = (const CLoadedImage & li)
	{
		CopyFrom(li);
		return (*this);
	};

	bool operator < (const CLoadedImage & li) const
	{
		return m_lLastUse < li.m_lLastUse;
	};

	void	Clear()
	{
		m_hBitmap.Release();
		m_pBitmap.Release();
	};

};

/* ------------------------------------------------------------------- */

typedef std::vector<CLoadedImage>			LOADEDIMAGEVECTOR;
typedef	LOADEDIMAGEVECTOR::iterator			LOADEDIMAGEITERATOR;

/* ------------------------------------------------------------------- */

class CBackgroundLoading
{
private :
	CComAutoCriticalSection	m_CriticalSection;
	LOADEDIMAGEVECTOR		m_vLoadedImages;
	CString					m_strToLoad;
	HANDLE					m_hThread;
	DWORD					m_dwThreadID;
	HANDLE					m_hEvent;
	HWND					m_hWnd;

private :
	void	CloseThread();
	void	LoadImageInBackground(LPCTSTR szImage);
	void	LoadCurrentImage();

public :
	CBackgroundLoading();
	virtual ~CBackgroundLoading();

	void	SetWindow(HWND hWnd)
	{
		m_hWnd = hWnd;
	};

	void	ClearList();
	void	BackgroundLoad();
	BOOL	LoadImage(LPCTSTR szImage, CMemoryBitmap ** ppBitmap, C32BitsBitmap ** pphBitmap);
};

/* ------------------------------------------------------------------- */

#endif