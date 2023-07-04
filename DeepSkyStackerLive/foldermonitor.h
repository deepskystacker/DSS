#pragma once
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
// FolderMonitor.h : Header file
//
#include <QObject>
namespace DSS
{
	class FolderMonitor : public QObject, public QRunnable
	{
		Q_OBJECT

	void run() override;

	public:
		FolderMonitor();
		~FolderMonitor();

		//
		// Don't intend this to be copied or assigned.
		//
		FolderMonitor(const FolderMonitor&) = delete;
		FolderMonitor& operator=(const FolderMonitor&) = delete;
		FolderMonitor(FolderMonitor&& rhs) = delete;
		FolderMonitor& operator=(FolderMonitor&& rhs) = delete;


	public slots:
		void stop();

	private:
		volatile bool stopped;
		unsigned long delay;
		std::u16string folderToWatch;
		std::unordered_map<fs::path, fs::file_time_type> paths_;
		std::vector<fs::path> existing_;

	signals:
		void existingFiles(const std::vector<fs::path>&);
		void fileChanged(const fs::path& file);
		void fileCreated(const fs::path& file);
		void fileErased(const fs::path& file);

	};
}
