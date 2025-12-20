#include "pch.h"
#include <QCoreApplication>
#include "DSSProgress.h"
#include "Multitask.h"

namespace DSS
{
	void OldProgressBase::UpdateProcessorsUsed()
	{
		int nCurrentThreadCount = Multitask::GetNrCurrentOmpThreads();
		applyProcessorsUsed(nCurrentThreadCount);
	}
	void ProgressBase::updateProcessorsUsed()
	{
		int threadCount = Multitask::GetNrCurrentOmpThreads();
		applyProcessorsUsed(threadCount);
	}
}
