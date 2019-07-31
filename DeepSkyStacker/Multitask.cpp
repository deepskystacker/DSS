#include <stdafx.h>

#include "Multitask.h"
#include "Registry.h"

/* ------------------------------------------------------------------- */

LONG	CMultitask::GetNrProcessors(BOOL bReal)
{
	LONG				lResult = 1;
	CRegistry			reg;
	SYSTEM_INFO			SysInfo;
	DWORD				dwMaxProcessors  =0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("MaxProcessors"), dwMaxProcessors);

	GetSystemInfo(&SysInfo);
	lResult		= SysInfo.dwNumberOfProcessors;
	if (!bReal && dwMaxProcessors)
		lResult = min(static_cast<long>(dwMaxProcessors), lResult);

	return lResult;
};

/* ------------------------------------------------------------------- */

void	CMultitask::SetUseAllProcessors(BOOL bUseAll)
{
	CRegistry			reg;

	if (bUseAll)
		reg.SaveKey(REGENTRY_BASEKEY, _T("MaxProcessors"), (DWORD)0);
	else
		reg.SaveKey(REGENTRY_BASEKEY, _T("MaxProcessors"), (DWORD)1);
};

/* ------------------------------------------------------------------- */

BOOL	CMultitask::GetReducedThreadsPriority()
{
	CRegistry			reg;
	DWORD				dwReduced  =0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("ReducedThreadPriority"), dwReduced);

	return dwReduced;
};

/* ------------------------------------------------------------------- */

void	CMultitask::SetReducedThreadsPriority(BOOL bReduced)
{
	CRegistry			reg;

	if (bReduced)
		reg.SaveKey(REGENTRY_BASEKEY, _T("ReducedThreadPriority"), (DWORD)1);
	else
		reg.SaveKey(REGENTRY_BASEKEY, _T("ReducedThreadPriority"), (DWORD)0);
};

/* ------------------------------------------------------------------- */

DWORD	WINAPI	StartThreadProc(LPVOID lpParameter)
{
	DWORD				dwResult = 0;
	CMultitask *		pMultitask = reinterpret_cast<CMultitask *>(lpParameter);

	if (pMultitask)
		pMultitask->DoTask(pMultitask->GetThreadEvent(GetCurrentThreadId()));

	return dwResult;
};

/* ------------------------------------------------------------------- */

HANDLE	CMultitask::GetAvailableThread()
{
	HANDLE				hResult = NULL;
	DWORD				dwResult;

	dwResult = WaitForMultipleObjects((DWORD)m_vEvents.size(), &(m_vEvents[0]), FALSE, INFINITE);
	if ((dwResult >= WAIT_OBJECT_0) && (dwResult < WAIT_OBJECT_0+(DWORD)m_vEvents.size()))
	{
		// An event was triggered
		LONG			lIndice = dwResult-WAIT_OBJECT_0;

		ResetEvent(m_vEvents[lIndice]);
		hResult = m_vThreads[lIndice];
	};

	return hResult;
};
/* ------------------------------------------------------------------- */

DWORD	CMultitask::GetAvailableThreadId()
{
	DWORD				hResult = NULL;
	DWORD				dwResult;

	dwResult = WaitForMultipleObjects((DWORD)m_vEvents.size(), &(m_vEvents[0]), FALSE, INFINITE);
	if ((dwResult >= WAIT_OBJECT_0) && (dwResult < WAIT_OBJECT_0+m_vEvents.size()))
	{
		// An event was triggered
		LONG			lIndice = dwResult-WAIT_OBJECT_0;

		ResetEvent(m_vEvents[lIndice]);
		hResult = m_vThreadIds[lIndice];
	};

	return hResult;
};

/* ------------------------------------------------------------------- */

void	CMultitask::StartThreads(LONG lNrThreads)
{
	LONG				i;

	if (!lNrThreads)
		lNrThreads = GetNrProcessors();

	for (i = 0;i<lNrThreads;i++)
	{
		// Create a thread for each task
		DWORD			dwThreadID;
		HANDLE			hEvent;
		HANDLE			hThread;

		hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEvent)
		{
			m_vEvents.push_back(hEvent);
			hThread = CreateThread(NULL, 0, StartThreadProc, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID);
			if (hThread)
			{
				m_vThreads.push_back(hThread);
				m_vThreadIds.push_back(dwThreadID);
			};
		};
	};

	if (GetReducedThreadsPriority())
	{
		for (i = 0;i<lNrThreads;i++)
			SetThreadPriority(m_vThreads[i], THREAD_PRIORITY_BELOW_NORMAL);
	};

	for (i = 0;i<lNrThreads;i++)
		ResumeThread(m_vThreads[i]);
};

/* ------------------------------------------------------------------- */

void	CMultitask::CloseAllThreads()
{
	if (m_vThreads.size())
	{
		WaitForMultipleObjects((DWORD)m_vEvents.size(), &(m_vEvents[0]), TRUE, INFINITE);

		for (LONG i = 0;i<m_vThreads.size();i++)
		{
			PostThreadMessage(m_vThreadIds[i], WM_MT_STOP, 0, 0);
		};

		WaitForMultipleObjects((DWORD)m_vThreads.size(), &(m_vThreads[0]), TRUE, INFINITE);

		for (LONG i = 0;i<m_vThreads.size();i++)
		{
			CloseHandle(m_vThreads[i]);
			CloseHandle(m_vEvents[i]);
		};

		m_vThreads.clear();
		m_vThreadIds.clear();
		m_vEvents.clear();
	};
};

/* ------------------------------------------------------------------- */