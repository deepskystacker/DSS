#include <stdafx.h>
#include "BackgroundLoading.h"

constexpr unsigned int WM_BL_LOAD	= WM_USER + 1;
constexpr unsigned int WM_BL_ABORT	= WM_USER + 2;
constexpr unsigned int WM_BL_STOP	= WM_USER + 3;

/* ------------------------------------------------------------------- */

CBackgroundLoading::CBackgroundLoading()
{
	m_hThread		= nullptr;
	m_dwThreadID	= 0;
	m_hEvent		= nullptr;
    m_hWnd          = NULL;
};

/* ------------------------------------------------------------------- */

CBackgroundLoading::~CBackgroundLoading()
{
	CloseThread();
	ClearList();
};

/* ------------------------------------------------------------------- */

void	CBackgroundLoading::ClearList()
{
	m_CriticalSection.Lock();
	m_vLoadedImages.clear();
	m_CriticalSection.Unlock();
};

/* ------------------------------------------------------------------- */

unsigned long WINAPI BackgroundLoadingThreadProc(LPVOID lpParameter)
{
	CBackgroundLoading *		pBackgroundLoading = reinterpret_cast<CBackgroundLoading *>(lpParameter);

	if (pBackgroundLoading)
		pBackgroundLoading->BackgroundLoad();

	return 0;
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::LoadCurrentImage()
{
	ZFUNCTRACE_RUNTIME();
	bool						bLoaded = false;


	m_CriticalSection.Lock();
	// Check that the image is not already available
	CString						strImage = m_strToLoad;
	for (int i = 0;i<m_vLoadedImages.size() && !bLoaded;i++)
	{
		if (!m_vLoadedImages[i].m_strName.CompareNoCase(m_strToLoad))
			bLoaded = true;
	};
	m_CriticalSection.Unlock();
	if (!bLoaded && strImage.GetLength())
	{
		CAllDepthBitmap				adb;

		if (LoadPicture(strImage, adb))
		{
			m_CriticalSection.Lock();
			CLoadedImage li;

			li.m_hBitmap = adb.m_pWndBitmap;
			li.m_pBitmap = adb.m_pBitmap;
			li.m_strName = strImage;
			li.m_lLastUse= 0;
			m_vLoadedImages.push_back(li);
			bLoaded = true;
			m_CriticalSection.Unlock();
		};
	};
	if (bLoaded)
	{
		// Post a message to the window to advise that the image is
		// loaded
		PostMessage(m_hWnd, WM_BACKGROUNDIMAGELOADED, 0, 0);
	};
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::BackgroundLoad()
{
	ZFUNCTRACE_RUNTIME();
	bool				bEnd = false;
	MSG					msg;

	PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
	SetEvent(m_hEvent);
	while (!bEnd && GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_BL_LOAD)
		{
			LoadCurrentImage();
			SetEvent(m_hEvent);
		}
		else if (msg.message == WM_BL_STOP)
			bEnd = true;
	};
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::LoadImageInBackground(LPCTSTR szImage)
{
	ZFUNCTRACE_RUNTIME();
	if (!m_hThread)
	{
		// Create the thread
		m_hEvent	= CreateEvent(nullptr, true, false, nullptr);
		if (m_hEvent)
		{
			m_hThread = CreateThread(nullptr, 0, BackgroundLoadingThreadProc, this, CREATE_SUSPENDED, &m_dwThreadID);
			if (m_hThread)
			{
				SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
				ResetEvent(m_hEvent);
				ResumeThread(m_hThread);
				WaitForMultipleObjects(1, &m_hEvent, true, INFINITE);
			};
		};
	};
	if (m_hThread && m_dwThreadID)
	{
		m_strToLoad = szImage;
		PostThreadMessage(m_dwThreadID, WM_BL_LOAD, 0, 0);
	};
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::CloseThread()
{
	ZFUNCTRACE_RUNTIME();
	if (m_hThread)
	{
		PostThreadMessage(m_dwThreadID, WM_BL_ABORT, 0, 0);
		WaitForMultipleObjects(1, &m_hEvent, true, INFINITE);
		PostThreadMessage(m_dwThreadID, WM_BL_STOP, 0, 0);

		WaitForMultipleObjects(1, &m_hThread, true, INFINITE);

		CloseHandle(m_hThread);
		CloseHandle(m_hEvent);
		m_hThread = nullptr;
		m_hEvent  = nullptr;
	};
};


bool CBackgroundLoading::LoadImage(LPCTSTR szImage, std::shared_ptr<CMemoryBitmap>& rpBitmap, std::shared_ptr<C32BitsBitmap>& rphBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	bool bFound = false;

	// Check if the image is in the list first
	m_CriticalSection.Lock();
	for (int i = 0; i < m_vLoadedImages.size(); i++)
	{
		if (!m_vLoadedImages[i].m_strName.CompareNoCase(szImage))
		{
			rpBitmap = m_vLoadedImages[i].m_pBitmap;
			rphBitmap = m_vLoadedImages[i].m_hBitmap;
			bFound = true;
		}
		else
		{
			m_vLoadedImages[i].m_lLastUse++;
		}
	}
	if (bFound)
	{
		if (m_vLoadedImages.size() > MAXIMAGESINCACHE)
		{
			// Remove the last images from the cache
			std::sort(m_vLoadedImages.begin(), m_vLoadedImages.end());
			m_vLoadedImages.resize(MAXIMAGESINCACHE);
		}

		bResult = true;
	}
	else
	{
		LoadImageInBackground(szImage);
		bResult = false;
	}
	m_CriticalSection.Unlock();

	return bResult;
}

/* ------------------------------------------------------------------- */
