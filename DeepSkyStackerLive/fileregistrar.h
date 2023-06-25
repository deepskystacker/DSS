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
// FileRegistrar.h : Header file
//
#include <deque>
#include <QObject>

class CMemoryBitmap;
class CLightFrameInfo;
class LoadedImage;

namespace DSS
{
	class ProgressLive;
	class FileRegistrar : public QThread
	{
		Q_OBJECT

	void run() override;

	signals:
		void writeToLog(const QString& message, bool addTimeStamp = false, bool bold = false, bool italic = false, QColor colour = Qt::green);
		void addImageToList(fs::path file);
		void fileLoaded(std::shared_ptr<LoadedImage> image);
		void fileRegistered(std::shared_ptr<CLightFrameInfo> lfi);
		void addToStackingQueue(std::shared_ptr<CLightFrameInfo> lfi);
		void fileNotStackable(fs::path file);
		void setImageInfo(QString name, STACKIMAGEINFO info);
		void handleWarning(QString text);

	public:
		FileRegistrar(QObject* parent = nullptr, ProgressLive* progress = nullptr);
		~FileRegistrar();

		void addFile(fs::path file);
		inline void enableRegistration(bool enable = true)
		{
			registrationEnabled = enable;
			if (enable && !pending.empty())
			{
				condvar.wakeOne();
			}
		}
		size_t pendingImageCount();


	public slots:
		void dropPendingImages();

	private:
		std::atomic_bool registrationEnabled;
		std::atomic_bool closing;
		ProgressLive* pProgress;
		QWaitCondition condvar;
		QMutex mutex;
		std::deque<fs::path> pending;

		void registerImage(fs::path file);
		bool isImageStackable(const fs::path& file, double fStarCount, double fFWHM, double fScore, double fSkyBackground, QString& error);
		bool imageWarning(const fs::path& file, double fStarCount, double fFWHM, double fScore, double fSkyBackground, QString& warning);
	};
}

