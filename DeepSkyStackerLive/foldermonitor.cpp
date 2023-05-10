#include "stdafx.h"
#include <atomic>
#include "foldermonitor.h"

namespace DSS
{
	FolderMonitor::FolderMonitor(QObject* parent)
		: QObject(parent),
		stopped{ false }
	{}

	FolderMonitor::~FolderMonitor()
	{}

	void FolderMonitor::run()
	{
		while (!stopped)
		{

		}
	}

	void FolderMonitor::stop()
	{
		stopped = true;
	}
}