#include <stdafx.h>
#include "Multitask.h"
#include <QSettings>


int CMultitask::GetNrProcessors(bool bReal)
{
	const auto dwMaxProcessors = QSettings{}.value("MaxProcessors", uint{ 0 }).toUInt();

	SYSTEM_INFO SysInfo;
	GetSystemInfo(&SysInfo);
	int lResult = SysInfo.dwNumberOfProcessors;
	if (!bReal && dwMaxProcessors != 0)
		lResult = std::min(static_cast<int>(dwMaxProcessors), lResult);

	return lResult;
};

void CMultitask::SetUseAllProcessors(bool bUseAll)
{
	QSettings settings;
	if (bUseAll)
		settings.setValue("MaxProcessors", uint{ 0 });
	else
		settings.setValue("MaxProcessors", uint{ 1 });
};

bool CMultitask::GetReducedThreadsPriority()
{
	return QSettings{}.value("ReducedThreadPriority", true).toBool();
};

void CMultitask::SetReducedThreadsPriority(bool bReduced)
{
	QSettings{}.setValue("ReducedThreadPriority", bReduced);
};

bool CMultitask::GetUseSimd()
{
	return QSettings{}.value("UseSimd", true).toBool();
}

void CMultitask::SetUseSimd(const bool bUseSimd)
{
	QSettings{}.setValue("UseSimd", bUseSimd);
}


//DWORD WINAPI StartThreadProc(LPVOID lpParameter)
//{
//	if (CMultitask* pMultitask = static_cast<CMultitask*>(lpParameter))
//		pMultitask->DoTask(pMultitask->GetThreadEvent(GetCurrentThreadId()));
//	return 0;
//};


//HANDLE CMultitask::GetAvailableThread()
//{
//	const auto dwResult = WaitForMultipleObjects(static_cast<std::uint32_t>(m_vEvents.size()), m_vEvents.data(), false, INFINITE);
//	if ((dwResult >= WAIT_OBJECT_0) && (dwResult < WAIT_OBJECT_0 + static_cast<decltype(dwResult)>(m_vEvents.size())))
//	{
//		// An event was triggered
//		const ptrdiff_t lIndice = dwResult - WAIT_OBJECT_0;
//
//		ResetEvent(m_vEvents[lIndice]);
//		return m_vThreads[lIndice];
//	};
//
//	return nullptr;
//};

//DWORD CMultitask::GetAvailableThreadId()
//{
//	const auto dwResult = WaitForMultipleObjects(static_cast<std::uint32_t>(m_vEvents.size()), m_vEvents.data(), false, INFINITE);
//	if ((dwResult >= WAIT_OBJECT_0) && (dwResult < WAIT_OBJECT_0+m_vEvents.size()))
//	{
//		// An event was triggered
//		const ptrdiff_t lIndice = dwResult - WAIT_OBJECT_0;
//
//		ResetEvent(m_vEvents[lIndice]);
//		return m_vThreadIds[lIndice];
//	};
//
//	return 0;
//};

//void CMultitask::StartThreads(int lNrThreads)
//{
//	if (lNrThreads == 0)
//		lNrThreads = GetNrProcessors();
//
//	for (int i = 0; i < lNrThreads; i++)
//	{
//		// Create a thread for each task
//		if (void* hEvent = CreateEvent(nullptr, true, false, nullptr))
//		{
//			m_vEvents.push_back(hEvent);
//			DWORD dwThreadID;
//			if (void* hThread = CreateThread(nullptr, 0, StartThreadProc, (LPVOID)this, CREATE_SUSPENDED, &dwThreadID))
//			{
//				m_vThreads.push_back(hThread);
//				m_vThreadIds.push_back(dwThreadID);
//			};
//		};
//	};
//
//	if (GetReducedThreadsPriority())
//	{
//		for (int i = 0; i < lNrThreads; i++)
//			SetThreadPriority(m_vThreads[i], THREAD_PRIORITY_BELOW_NORMAL);
//	};
//
//	for (int i = 0; i < lNrThreads; i++)
//		ResumeThread(m_vThreads[i]);
//};

//void CMultitask::CloseAllThreads()
//{
//	if (!m_vThreads.empty())
//	{
//		WaitForMultipleObjects(static_cast<std::uint32_t>(m_vEvents.size()), m_vEvents.data(), true, INFINITE);
//
//		for (const auto threadId : m_vThreadIds)
//		{
//			PostThreadMessage(threadId, WM_MT_STOP, 0, 0);
//		};
//
//		WaitForMultipleObjects(static_cast<std::uint32_t>(m_vThreads.size()), m_vThreads.data(), true, INFINITE);
//
//		for (size_t i = 0; i < m_vThreads.size(); i++)
//		{
//			CloseHandle(m_vThreads[i]);
//			CloseHandle(m_vEvents[i]);
//		};
//
//		m_vThreads.clear();
//		m_vThreadIds.clear();
//		m_vEvents.clear();
//	};
//};
