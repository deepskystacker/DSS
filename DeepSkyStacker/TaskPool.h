#pragma once
#include <set>
namespace TaskPool
{
	typedef size_t TaskId;
	const TaskId kInvalidTaskId = 0;
	TaskId Async(void(*f)(void*), void*data);
	TaskId Wait(const std::set<TaskId>& waited);
}