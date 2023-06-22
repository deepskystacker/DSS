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
// FileStacker.h : Header file
//
#include <deque>
#include <QThread>
#include "RunningStackingEngine.h"

class CLightFrameInfo;
class LoadedImage;

namespace DSS
{
	class ProgressLive;
	class LiveSettings;

	class FileStacker : public QThread
	{
		Q_OBJECT

		void run() override;

	signals:
		void writeToLog(const QString& message, bool addTimeStamp = false, bool bold = false, bool italic = false, QColor colour = Qt::yellow);
		void setImageOffsets(QString name, double dx, double dy , double angle);
		void fileStacked(std::shared_ptr<CLightFrameInfo> p);
		void fileNotStackable(fs::path file);
		void setImageInfo(QString name, STACKIMAGEINFO info);
		void setImageFootprint(QPointF p1, QPointF p2, QPointF p3, QPointF p4);
		void handleWarning(QString text);
		void showStackedImage(std::shared_ptr<LoadedImage> li, int count, double exposure);
		void stackedImageSaved();

	public:
		FileStacker(QObject* parent = nullptr, ProgressLive* progress = nullptr);
		~FileStacker();
		size_t registeredImageCount();

		void addFile(std::shared_ptr<CLightFrameInfo>& lfi);
		inline void enableStacking(bool enable = true)
		{
			stackingEnabled = enable;
			if (enable && !pending.empty())
			{
				condvar.wakeOne();
			}
		}

	public slots:
		void dropPendingImages();
		void clearStackedImage();
		void saveStackedImage(const std::shared_ptr<CMemoryBitmap> pBitmap);

	private:
		std::atomic_bool stackingEnabled;
		ProgressLive* pProgress;
		LiveSettings* liveSettings;
		QWaitCondition condvar;
		QMutex mutex;
		QMutex stacking;
		std::deque<std::shared_ptr<CLightFrameInfo>> pending;
		bool referenceFrameIsSet;
		CRunningStackingEngine stackingEngine;
		std::uint32_t unsavedImageCount;

		void stackNextImage();
		bool isImageStackable(const fs::path& file, double fdX, double fdY, double fAngle, QString& error);
		bool imageWarning(const fs::path& file, double fdX, double fdY, double fAngle, QString& warning);
		void emitStackedImage(const fs::path& file);
		std::shared_ptr<QImage> makeQImage(const std::shared_ptr<CMemoryBitmap>& pStackedImage);


	};
}