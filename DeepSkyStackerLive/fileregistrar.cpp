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
// FolderRegistrar.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "fileregistrar.h"
namespace DSS
{
	FileRegistrar::FileRegistrar(QObject* parent)
		: QThread(parent)
	{
		ZTRACE_RUNTIME("File registrar active");
		start();
	}

	FileRegistrar::~FileRegistrar()
	{
		{
			QMutexLocker lock(&mutex);

			// Clear the work queue
			if (!pending.empty())
				pending.clear();

			// Add a null entry to the work queue to show we're done
			// and wake up the run() mf
			pending.emplace_back(fs::path());
			condvar.wakeOne();
		}
		
		//
		// Wait for run() to terminate
		//
		wait();
		ZTRACE_RUNTIME("File registrar deleted");
	}

	void FileRegistrar::run()
	{
		forever
		{
			fs::path file;
			{
				QMutexLocker lock(&mutex);
				//
				// Wait for work to arrive
				//
				if (pending.empty())
					condvar.wait(&mutex);

				//
				// Something to do?
				//
				file = pending.front(); pending.pop_front();

				if (file.empty()) break;
			}

			registerImage(file);
		}
	}
	 
	void FileRegistrar::addFile(fs::path file)
	{
		ZTRACE_RUNTIME(" Adding file %s to work queue", file.filename().generic_string().c_str());
		QMutexLocker lock(&mutex);
		pending.emplace_back(file);
		condvar.wakeOne();
	}

	void FileRegistrar::registerImage(fs::path file)
	{
		ZFUNCTRACE_RUNTIME();
		ZTRACE_RUNTIME("Registering %s", file.filename().generic_string().c_str());
		sleep(1);
		QString message{ tr("Image %1 registered: %n star(s) detected - FWHM = %2 - Score = %3", "IDS_LOG_REGISTERRESULTS", 10)
			.arg(QString::fromStdU16String(file.generic_u16string().c_str()))
			.arg(20., 0, 'f', 2)
			.arg(200., 0, 'f', 2) };

		ZTRACE_RUNTIME(message.toUtf8().constData());
		message += "\n";
		writeToLog(message,
			true);
		emit fileRegistered(file);
	}
}