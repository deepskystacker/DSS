#include "stdafx.h"
#include <atomic>
#include "foldermonitor.h"

namespace DSS
{
	FolderMonitor::FolderMonitor(QObject* parent)
		: QObject(parent),
		stopped{ false },
		delay{ 5UL }
	{
		QSettings settings;
		settings.beginGroup("DeepSkyStackerLive");
		QString folder = settings.value("MonitoredFolder", "").toString();
		ZASSERT(!folder.isEmpty());

		folderToWatch = folder.toStdU16String();
		for (auto& file : fs::directory_iterator(path_to_watch))
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
		emit existingFiles(existing_);
	}

	FolderMonitor::~FolderMonitor()
	{}

	void FolderMonitor::run()
	{
		while (!stopped)

		{
			// Wait for "delay" milliseconds
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
		stopped = true;
	}
}