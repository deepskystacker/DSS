
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
// FileStacker.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "filestacker.h"
#include "progresslive.h"
#include "RegisterEngine.h"
#include "LiveSettings.h"
#include "DeepSkyStackerLive.h"

namespace DSS
{
	FileStacker::FileStacker(QObject* parent, ProgressLive* progress) :
		QThread{ parent },
		stackingEnabled{ false },
		pProgress{ progress },
		liveSettings{ DSSLive::instance()->liveSettings.get() },
		referenceFrameIsSet {false}
	{
		ZTRACE_RUNTIME("File stacker active");
		start();
	}

	FileStacker::~FileStacker()
	{
		{
			QMutexLocker lock(&mutex);

			// Clear the work queue
			if (!pending.empty())
				pending.clear();

			// Add a null entry to the work queue to show we're done
			// and wake up the run() mf
			std::shared_ptr<CLightFrameInfo> nullSharedPtr;
			pending.emplace_back(nullSharedPtr);
			condvar.wakeOne();
		}

		//
		// Wait for run() to terminate
		//
		wait();
		ZTRACE_RUNTIME("File stacker deleted");
	}

	void FileStacker::run()
	{
		forever
		{
			{
				QMutexLocker lock(&mutex);
				//
				// Wait for work to arrive
				//
				if (pending.empty())
					condvar.wait(&mutex);
	
				if (nullptr == pending.front()) break;
			}

			stackNextImage();
		}
	}

	void FileStacker::addFile(std::shared_ptr<CLightFrameInfo>& lfi)
	{
		QMutexLocker lock(&mutex);
		pending.emplace_back(lfi);
		if (stackingEnabled)
		{
			condvar.wakeOne();
		}

	}

	void FileStacker::stackNextImage()
	{
		ZFUNCTRACE_RUNTIME();
		if (!referenceFrameIsSet)
		{
			if (liveSettings->IsDontStack_Until() && pending.size() < liveSettings->GetMinImages())
				return;
			else
			{
				// Select the best reference frame from all the available images
				// (best score)
				double maxScore = 0;
				std::deque<std::shared_ptr<CLightFrameInfo>>::iterator	it,
					bestit = pending.end();

				for (it = pending.begin(); it != pending.end(); it++)
				{
					if ((*it)->m_fOverallQuality > maxScore)
					{
						bestit = it;
						maxScore = (*it)->m_fOverallQuality;
					}
				}

				if (bestit != pending.end())
				{
					CLightFrameInfo& lfi{ *(*bestit) };
					QString name{ QString::fromStdU16String(lfi.filePath.filename().generic_u16string()) };

					stackingEngine.ComputeOffset(lfi);
					emit setImageOffsets(name, 0, 0, 0);

					stackingEngine.AddImage(lfi, pProgress);
					emit fileStacked(*bestit);
					emit setImageInfo(lfi.filePath, II_SETREFERENCE);

					pending.erase(bestit);
					referenceFrameIsSet = true;
				}
				else
					pending.clear();
			}
		}
		else
		{
			CLightFrameInfo			lfi;
			double					dX, dY, angle;
			QString					strError;
			QString					strText;
			bool					error = false;
			bool					warning = false;
			QString					strWarning;
			std::shared_ptr<CLightFrameInfo> pInfo{ pending.front() };

			lfi = *pInfo;

			QString name{ QString::fromStdU16String(lfi.filePath.filename().generic_u16string()) };

			pending.pop_front();
			if (stackingEngine.ComputeOffset(lfi))
			{
				lfi.m_BilinearParameters.Offsets(dX, dY);
				angle = lfi.m_BilinearParameters.Angle(lfi.RenderedWidth()) * 180.0 / M_PI;
				emit setImageOffsets(name, dX, dY, angle);
				emit setImageInfo(lfi.filePath, II_DONTSTACK_NONE);

				warning = imageWarning(lfi.filePath, dX, dY, angle, strWarning);
				if (warning)
					emit handleWarning(strWarning);

				if (isImageStackable(lfi.filePath, dX, dY, angle, strError))
				{
					stackingEngine.AddImage(lfi, pProgress);
					emit fileStacked(pInfo);

					QPointF		pt1, pt2, pt3, pt4;

					lfi.m_BilinearParameters.Footprint(pt1, pt2, pt3, pt4);
					emit setImageFootprint(pt1, pt2, pt3, pt4);
				}
				else
				{
					error = true;
				}
			}
			else
			{
				// Can't find transformation - impossible to stack the image
				strError = tr("No transformation found from reference frame", "IDS_NOSTACK_NOTRANSFORMATION");
				error = true;
			}

			if (warning)
			{
				strText = tr("Warning: Image %s -> %s\n", "IDS_LOG_WARNING")
					.arg(name)
					.arg(strWarning);
				emit writeToLog(strText, true, false, true, qRgb(208, 127, 0));
			}
			if (error)
			{
				strText = tr("Image %1 is not stackable (%2)\n", "IDS_LOG_IMAGENOTSTACKABLE1").arg(name).arg(strError);
				emit writeToLog(strText, true, true, false, Qt::red);
				emit fileNotStackable(lfi.filePath);
			}
		}
	}

	bool FileStacker::isImageStackable(const fs::path& file, double dX, double dY, double angle, QString& error)
	{
		bool result = true;

		if (liveSettings->IsDontStack_Offset())
		{
			double maxOffset{ liveSettings->GetOffset() };
			if ((fabs(dX) > maxOffset) || (fabs(dY) > maxOffset))
			{
				result = false;
				error = tr("dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels", "IDS_NOSTACK_OFFSET")
					.arg(dX, 0, 'f', 2).arg(dX, 0, 'f', 2).arg(maxOffset, 0, 'f', 2);
			}
			if (fabs(dX) > maxOffset)
				emit setImageInfo(file, II_DONTSTACK_DX);
			if (fabs(dY) > maxOffset)
				emit setImageInfo(file, II_DONTSTACK_DY);
		}

		if (result && liveSettings->IsDontStack_Angle())
		{
			double maxAngle{ liveSettings->GetAngle() };
			if (fabs(angle) > maxAngle)
			{
				result = false;
				error = tr("Angle (%L1°) is greater than %L2°", "IDS_NOSTACK_ANGLE")
					.arg(angle, 0, 'f', 2).arg(maxAngle, 0, 'f', 2);
				emit setImageInfo(file, II_DONTSTACK_ANGLE);
			}
		}

		return result;
	}

	bool FileStacker::imageWarning(const fs::path& file, double dX, double dY, double angle, QString& warning)
	{
		bool result = false;

		if (liveSettings->IsWarning_Offset())
		{
			double maxOffset{ liveSettings->GetOffset() };
			if ((fabs(dX) > maxOffset) || (fabs(dY) > maxOffset))
			{
				result = true;
				warning = tr("dX (%1 pixels) or dY (%2 pixels) is greater than %3f pixels", "IDS_NOSTACK_OFFSET")
					.arg(dX, 0, 'f', 2).arg(dX, 0, 'f', 2).arg(maxOffset, 0, 'f', 2);
			};
			if (fabs(dX) > maxOffset)
			{
				emit setImageInfo(file, II_WARNING_DX);
			};
			if (fabs(dY) > liveSettings->GetOffset())
			{
				emit setImageInfo(file, II_WARNING_DY);
			};
		};

		if (liveSettings->IsWarning_Angle())
		{
			double maxAngle{ liveSettings->GetAngle() };
			if (fabs(angle) > maxAngle)
			{
				result = true;
				warning = tr("Angle (%1°) is greater than %2°", "IDS_NOSTACK_ANGLE")
					.arg(angle, 0, 'f', 2).arg(maxAngle, 0, 'f', 2);
				emit setImageInfo(file, II_WARNING_ANGLE);
			};
		};

		return result;
	};
}
