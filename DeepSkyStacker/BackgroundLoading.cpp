#include <stdafx.h>
#include "BackgroundLoading.h"

const DWORD				WM_BL_LOAD		= WM_USER+1;
const DWORD				WM_BL_ABORT		= WM_USER+2;
const DWORD				WM_BL_STOP		= WM_USER+3;

/* ------------------------------------------------------------------- */

CBackgroundLoading::CBackgroundLoading()
{
	m_hThread		= NULL;
	m_dwThreadID	= 0;
	m_hEvent		= NULL;

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

DWORD	WINAPI	BackgroundLoadingThreadProc(LPVOID lpParameter)
{
	DWORD						dwResult = 0;
	CBackgroundLoading *		pBackgroundLoading = reinterpret_cast<CBackgroundLoading *>(lpParameter);

	if (pBackgroundLoading)
		pBackgroundLoading->BackgroundLoad();

	return dwResult;
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::LoadCurrentImage()
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bLoaded = FALSE;


	m_CriticalSection.Lock();
	// Check that the image is not already available
	CString						strImage = m_strToLoad;
	for (LONG i = 0;i<m_vLoadedImages.size() && !bLoaded;i++)
	{
		if (!m_vLoadedImages[i].m_strName.CompareNoCase(m_strToLoad))
			bLoaded = TRUE;
	};
	m_CriticalSection.Unlock();
	if (!bLoaded && strImage.GetLength())
	{
		CAllDepthBitmap				adb;

		if (LoadPicture(strImage, adb))
		{
			m_CriticalSection.Lock();
			CLoadedImage			li;

			li.m_hBitmap = adb.m_pWndBitmap;
			li.m_pBitmap = adb.m_pBitmap;
			li.m_strName = strImage;
			li.m_lLastUse= 0;
			m_vLoadedImages.push_back(li);
			bLoaded = TRUE;
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
	BOOL				bEnd = FALSE;
	MSG					msg;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(m_hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_BL_LOAD)
		{
			LoadCurrentImage();
			SetEvent(m_hEvent);
		}
		else if (msg.message == WM_BL_STOP)
			bEnd = TRUE;
	};
};

/* ------------------------------------------------------------------- */

void CBackgroundLoading::LoadImageInBackground(LPCTSTR szImage)
{
	ZFUNCTRACE_RUNTIME();
	if (!m_hThread)
	{
		// Create the thread
		m_hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hEvent)
		{
			m_hThread = CreateThread(NULL, 0, BackgroundLoadingThreadProc, (LPVOID)this, CREATE_SUSPENDED, &m_dwThreadID);
			if (m_hThread)
			{
				SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
				ResetEvent(m_hEvent);
				ResumeThread(m_hThread);
				WaitForMultipleObjects(1, &m_hEvent, TRUE, INFINITE);
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
		WaitForMultipleObjects(1, &m_hEvent, TRUE, INFINITE);
		PostThreadMessage(m_dwThreadID, WM_BL_STOP, 0, 0);

		WaitForMultipleObjects(1, &m_hThread, TRUE, INFINITE);

		CloseHandle(m_hThread);
		CloseHandle(m_hEvent);
		m_hThread = NULL;
		m_hEvent  = NULL;
	};
};

/* ------------------------------------------------------------------- */

BOOL	CBackgroundLoading::LoadImage(LPCTSTR szImage, CMemoryBitmap ** ppBitmap, C32BitsBitmap ** pphBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	BOOL				bFound = FALSE;

	// Check if the image is in the list first
	if (ppBitmap)
		*ppBitmap = NULL;
	if (pphBitmap)
		*pphBitmap = NULL;
	m_CriticalSection.Lock();
	for (LONG i = 0;i<m_vLoadedImages.size();i++)
	{
		if (!m_vLoadedImages[i].m_strName.CompareNoCase(szImage))
		{
			m_vLoadedImages[i].m_pBitmap.CopyTo(ppBitmap);
			m_vLoadedImages[i].m_hBitmap.CopyTo(pphBitmap);
			bFound = TRUE;
		}
		else
		{
			m_vLoadedImages[i].m_lLastUse++;
		};
	};
	if (bFound)
	{
		if (m_vLoadedImages.size()>MAXIMAGESINCACHE)
		{
			// Remove the last images from the cache
			std::sort(m_vLoadedImages.begin(), m_vLoadedImages.end());
			m_vLoadedImages.resize(MAXIMAGESINCACHE);
		};

		bResult = TRUE;
	}
	else
	{
		LoadImageInBackground(szImage);
		bResult = FALSE;
	};
	m_CriticalSection.Unlock();
	
	return bResult;
};

/* ------------------------------------------------------------------- */
