#include "pch.h"
#include <QCoreApplication>
#include "DSSProgress.h"
#include "Multitask.h"

namespace DSS
{
	void ProgressBase::UpdateProcessorsUsed()
	{
		int nCurrentThreadCount = CMultitask::GetNrCurrentOmpThreads();
		applyProcessorsUsed(nCurrentThreadCount);
	}
}