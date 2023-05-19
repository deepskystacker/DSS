#include "stdafx.h"
#include <atomic>
#include <Ztrace.h>
#include "foldermonitor.h"

namespace DSS
{
	FolderMonitor::FolderMonitor()
		: QObject(),
		stopped{ false },
		delay{ 5UL }
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");
		QString folder = settings.value("MonitoredFolder", "").toString();
		ZASSERT(!folder.isEmpty());

		folderToWatch = folder.toStdU16String();
		for (auto& file : fs::directory_iterator(folderToWatch))
		{
			//
			// Only interested in reqular files.
			//
			if (file.is_regular_file())
			{
				existing_.push_back(file);
				paths_[file] = fs::last_write_time(file);
			}
		}
	}

	FolderMonitor::~FolderMonitor()
	{ 
		disconnect(this, nullptr);
	}

	void FolderMonitor::run()
	{
		ZFUNCTRACE_RUNTIME();
		moveToThread(QThread::currentThread());
		emit existingFiles(existing_);		// Tell the boss about existing files

		while (!stopped)
		{
			// Wait for "delay" seconds
			QThread::sleep(delay);

			auto it = paths_.begin();
			while (it != paths_.end())
			{
				if (!fs::exists(it->first)) 
				{
					emit fileErased(it->first);
					it = paths_.erase(it);
				}
				else
				{
					it++;
				}
			}

			// Check if a file was created or modified
			for (auto& file : fs::directory_iterator(folderToWatch))
			{
				auto current_file_last_write_time = fs::last_write_time(file);

				if (!paths_.contains(file))
				{
					// File creation
					paths_[file] = current_file_last_write_time;
					emit fileCreated(file);
					// File modification
				}
				else
				{
					// File modification
					if (paths_[file] != current_file_last_write_time)
					{
						paths_[file] = current_file_last_write_time;
						emit fileChanged(file);
					}
				}
			}
		}
	}

	void FolderMonitor::stop()
	{
		ZFUNCTRACE_RUNTIME();
		stopped = true;
	}
}