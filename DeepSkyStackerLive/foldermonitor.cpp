/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// FolderMonitor.cpp : Defines the class behaviors for the application.
//
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