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
			pending.emplace_back(std::make_shared<CLightFrameInfo>());
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
	
				if (pending.front()->filePath.empty()) break;
			}

			stackNextImage();
		}
	}

	void FileStacker::addFile(std::shared_ptr<CLightFrameInfo>&& lfi)
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
#if (0)
		if (!referenceFrameIsSet)
		{
			if (liveSettings->IsDontStack_Until() && pending.size() >= liveSettings->GetMinImages())
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
					};
				};

				if (bestit != pending.end())
				{
					CLightFrameInfo& lfi{ *(*bestit) };
					stackingEngine.ComputeOffset(lfi);
					PostUpdateImageOffsets(lfi.filePath.c_str(), 0, 0, 0);
					stackingEngine.AddImage(lfi, this);
					PostChangeImageStatus(lfi.filePath.c_str(), IS_STACKED);
					PostChangeImageInfo(lfi.filePath.c_str(), II_SETREFERENCE);
					pending.erase(bestit);
					referenceFrameIsSet = true;
					PostStackedImage();
				}
				else
					pending.clear();
			};
		}
		else
		{
			CLightFrameInfo			lfi;
			double					fdX, fdY, fAngle;
			CString					strError;
			CString					strText;
			BOOL					bError = false;
			BOOL					bWarning = false;
			CString					strWarning;

			lfi = pending.front();
			pending.pop_front();
			if (stackingEngine.ComputeOffset(lfi))
			{
				lfi.m_BilinearParameters.Offsets(fdX, fdY);
				fAngle = lfi.m_BilinearParameters.Angle(lfi.RenderedWidth()) * 180.0 / M_PI;
				PostUpdateImageOffsets(lfi.filePath.c_str(), fdX, fdY, fAngle);
				PostChangeImageInfo(lfi.filePath.c_str(), II_DONTSTACK_NONE);
				bWarning = IsImageWarning2(lfi.filePath.c_str(), fdX, fdY, fAngle, strWarning);
				if (bWarning)
					PostWarning(strWarning);
				if (IsImageStackable2(lfi.filePath.c_str(), fdX, fdY, fAngle, strError))
				{
					stackingEngine.AddImage(lfi, this);
					PostChangeImageStatus(lfi.filePath.c_str(), IS_STACKED);

					QPointF		pt1, pt2, pt3, pt4;

					lfi.m_BilinearParameters.Footprint(pt1, pt2, pt3, pt4);
					PostFootprint(pt1, pt2, pt3, pt4);
					PostStackedImage();
				}
				else
				{
					bError = true;
				};
			}
			else
			{
				// Can't find transformation - impossible to stack the image
				strError.LoadString(IDS_NOSTACK_NOTRANSFORMATION);
				bError = true;
			};

			if (bWarning)
			{
				strText.Format(IDS_LOG_WARNING, (LPCTSTR)lfi.filePath.c_str(), (LPCTSTR)strWarning);
				PostToLog(QString::fromWCharArray(strText.GetString()), true, false, true, RGB(208, 127, 0));
			};
			if (bError)
			{
				strText.Format(IDS_LOG_IMAGENOTSTACKABLE1, (LPCTSTR)lfi.filePath.c_str(), (LPCTSTR)strError);
				PostToLog(QString::fromWCharArray(strText.GetString()), true, true, false, RGB(255, 0, 0));
				PostChangeImageStatus(lfi.filePath.c_str(), IS_NOTSTACKABLE);
				MoveImage(lfi.filePath.c_str());
			};

			bResult = true;
		};
#endif
	}

}
