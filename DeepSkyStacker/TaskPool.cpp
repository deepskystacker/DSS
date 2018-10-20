#include <stdafx.h>
#include <map>
#include <set>
#include <queue>
#include <mutex>
#include "Multitask.h"
#include "TaskPool.h"


class CTaskPool : public CMultitask
{
	typedef TaskPool::TaskId TaskId;
	typedef void *fundataT;
	typedef void(*funT)(fundataT);
	typedef std::pair<funT, fundataT> Deffered;
	typedef DWORD ThreadID;

	TaskId m_taskIdGen;
	const TaskId kInvalidTaskId = TaskPool::kInvalidTaskId;

	std::deque< std::tuple<TaskId, Deffered>> m_waitQueue;
	
	std::map<TaskId, Deffered>	m_completedMap;
	std::map<TaskId, DWORD>		m_threadForTask;
	std::map<DWORD, TaskId>		m_taskForThread;
	std::mutex m_taskStructuresMutex;


	DWORD ScheduleThis(funT fun, fundataT data)
	{
		DWORD			dwThreadId =  GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, (WPARAM)fun, (LPARAM)data);
		return dwThreadId;
	}

	void ExecuteThis(funT fun, fundataT data) const
	{
		try
		{
			fun(data);
		}
		catch (...)
		{
			// swallow exceptions. It's a threadpool, the thread is not allowed to die
		}

	}

	#define	LOCKTASKSTRUCTURES  std::lock_guard<std::mutex> tasksStructuresLock(m_taskStructuresMutex);

	void ScheduleFront()
	{
		TaskId taskId = kInvalidTaskId;
		Deffered task;

		{
			LOCKTASKSTRUCTURES
			if (m_waitQueue.size())
			{
				auto taskToRun = m_waitQueue.front();
				m_waitQueue.pop_front();
				taskId = std::get<0>(taskToRun);
				task = std::get<1>(taskToRun);
			}
		}
		if (taskId != kInvalidTaskId)
		{
			// The scheduleThis will wait, so we don't want to be holding the lock during the wait
			DWORD threadId = ScheduleThis(task.first, task.second);

			{
				LOCKTASKSTRUCTURES
				m_threadForTask[taskId] = threadId;
				m_taskForThread[threadId] = taskId;
			}
		}
	}

	virtual BOOL	DoTask(HANDLE hEvent) override
	{
		MSG					msg;
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		DWORD threadId = GetCurrentThreadId();

		SetEvent(hEvent);
		bool bEnd = false;
		while (!bEnd && GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_MT_PROCESS)
			{
				auto fun = (funT)msg.wParam;
				auto data = (fundataT)msg.lParam;
				ExecuteThis(fun, data);

				{
					LOCKTASKSTRUCTURES
					auto where = m_taskForThread.find(threadId);
					if (where == m_taskForThread.end())
						InternalError();
					TaskId taskId = where->second;
					m_taskForThread.erase(threadId);
					m_threadForTask.erase(taskId);
					m_completedMap[taskId] = std::make_pair(fun, data);
				}

				SetEvent(hEvent);
			}
			else if (msg.message == WM_MT_STOP)
				bEnd = true;
		}
		return true;
	}

	TaskId UngardedReturnCompleted(const std::set<TaskId>& waited)
	{
		for (auto waitedID : waited)
		{
			auto where = m_completedMap.find(waitedID);
			if (where != m_completedMap.end())
			{
				m_completedMap.erase(where);
				return waitedID;
			}
		}
		return kInvalidTaskId;
	}

	TaskId ReturnRunning(const std::set<TaskId>& waited)
	{
		std::vector<HANDLE> threadEventsToWait;
		{
			LOCKTASKSTRUCTURES
			for (auto waitedID : waited)
			{
				auto where = m_threadForTask.find(waitedID);
				if (where != m_threadForTask.end())
				{
					DWORD threadId = where->second;
					auto eventToWait = GetThreadEvent(where->second);
					threadEventsToWait.push_back(eventToWait);
				}
			}
		}
		if (threadEventsToWait.size())
		{
			WaitForMultipleObjects((DWORD)threadEventsToWait.size(), threadEventsToWait.data(), false, INFINITE);
			// we completed one, so schedule a new one
			ScheduleFront();
			{
				LOCKTASKSTRUCTURES
				return UngardedReturnCompleted(waited); // we completed one so return a completed
			}
		}
		return kInvalidTaskId;
	}
	void InternalError() const
	{
		throw std::logic_error("Logic error in the TaskPool");
		abort();
	}
public:
	CTaskPool() : m_taskIdGen(kInvalidTaskId + 1)
	{
		StartThreads(CMultitask::GetNrProcessors());
	}

	virtual ~CTaskPool()
	{

	}

	TaskId Async(void (*f)(void*), void*data) 
	{
		TaskId taskId = kInvalidTaskId;
		{
			LOCKTASKSTRUCTURES

			taskId = m_taskIdGen++;
			m_waitQueue.emplace_back(taskId, std::make_pair(f, data));
		}
		ScheduleFront();
	
		return taskId;
	}
	


	TaskId Wait(const std::set<TaskId>& waited)
	{
		if (waited.size() == 0)
			return kInvalidTaskId;
		TaskId completed = kInvalidTaskId;
		Deffered deffered;

		{
			LOCKTASKSTRUCTURES
			completed = UngardedReturnCompleted(waited);
			if (completed != kInvalidTaskId)
				return completed;
		}

		completed = ReturnRunning(waited);
		if (completed != kInvalidTaskId)
			return completed;

		{
			LOCKTASKSTRUCTURES
			auto taskId = *(waited.begin());
			for (auto scanWait = m_waitQueue.begin(); scanWait != m_waitQueue.end(); ++scanWait)
			{
				auto scannedTaskId = std::get<0>(*scanWait);
				if (taskId == scannedTaskId)
				{
					deffered = std::get<1>(*scanWait);
					m_waitQueue.erase(scanWait);
					completed = taskId;
					break;
				}
			}
		}
		if (completed != kInvalidTaskId)
		{
			ExecuteThis(deffered.first, deffered.second);
			return completed;

		}
		return kInvalidTaskId;
	}

private:

	virtual BOOL	Process()
	{
		InternalError();
		return TRUE;
	}
} g_taskPool;


namespace TaskPool
{
	TaskId Async(void(*f)(void*), void*data)
	{
		return g_taskPool.Async(f, data);

	}
	TaskId Wait(const std::set<TaskId>& waited)
	{
		return g_taskPool.Wait(waited);
	}
}