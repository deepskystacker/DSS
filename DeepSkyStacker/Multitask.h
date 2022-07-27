#ifndef __MULTITASK_H__
#define __MULTITASK_H__

constexpr UINT WM_MT_PROCESS	= WM_USER + 1;
constexpr UINT WM_MT_STOP		= WM_USER + 2;

class CMultitask
{
protected:
	//CComAutoCriticalSection	m_CriticalSection;
	//std::vector<HANDLE>		m_vThreads;
	//std::vector<DWORD>		m_vThreadIds;
	//std::vector<HANDLE>		m_vEvents;

protected:
	//int GetNrThreads()
	//{
	//	return static_cast<int>(m_vThreads.size());
	//};

public:
	CMultitask() = default;

	//virtual ~CMultitask()
	//{
	//	CloseAllThreads();
	//};

	static int	GetNrProcessors(bool bReal = false);
	static void	SetUseAllProcessors(bool bUseAll);
	static bool	GetReducedThreadsPriority();
	static void	SetReducedThreadsPriority(bool bReduced);
	static bool GetUseSimd();
	static void SetUseSimd(const bool bUseSimd);

	//HANDLE GetThreadEvent(const DWORD dwThreadId)
	//{
	//	for (size_t i = 0; i < m_vThreadIds.size(); i++)
	//	{
	//		if (dwThreadId == m_vThreadIds[i])
	//			return m_vEvents[i];
	//	};
	//	return nullptr;
	//};

	//void CloseAllThreads();
	//void StartThreads(int lNrThreads = 0);
	//HANDLE GetAvailableThread();
	//DWORD GetAvailableThreadId();

	//virtual bool DoTask(HANDLE hEvent) = 0;
	//virtual bool Process() = 0;
};

#endif // __MULTITASK_H__