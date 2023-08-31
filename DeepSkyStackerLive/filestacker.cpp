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
#include "Multitask.h"
#include "BitmapIterator.h"
#include "TIFFUtil.h"
#include "dssliveenums.h"

namespace DSS
{
	FileStacker::FileStacker(QObject* parent, ProgressLive* progress) :
		QThread{ parent },
		stackingEnabled{ false },
		pProgress{ progress },
		liveSettings{ DSSLive::instance()->liveSettings.get() },
		referenceFrameIsSet {false},
		unsavedImageCount{ 0 }
	{
		ZTRACE_RUNTIME("File stacker active");
		start();
	}

	FileStacker::~FileStacker()
	{
		{
			//
			// Prevent new work being added to or removed from the queue
			//
			QMutexLocker lock(&mutex);
			//
			// Now it is safe to wait for the current image stack to complete (if one is active).
			//
			QMutexLocker stackLock(&stacking);


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
				// Wait for stacking to be be enabled, and if enabled, for work to arrive
				//
				if (!stackingEnabled || pending.empty())
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

	size_t FileStacker::registeredImageCount()
	{
		QMutexLocker lock(&mutex);
		return pending.size();
	}

	void FileStacker::dropPendingImages()
	{
		QMutexLocker lock(&mutex);
		pending.clear();
	}

	void FileStacker::clearStackedImage()
	{
		//
		// Wait until stackNextImage() has finished
		//
		QMutexLocker lock(&stacking);

		stackingEngine.Clear();
		referenceFrameIsSet = false;
		fs::path path;
		emitStackedImage(path);
	}

	void FileStacker::stackNextImage()
	{
		ZFUNCTRACE_RUNTIME();
		QMutexLocker stackLock(&stacking);		// Seize the mutex to show stacking is active

		if (!referenceFrameIsSet)
		{
			if (liveSettings->IsDontStack_Until() && pending.size() < liveSettings->GetMinImages())
				return;
			else
			{
				std::shared_ptr<CLightFrameInfo> lfi;
				bool bestFrameFound{ false };
				{
					QMutexLocker lock(&mutex);

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
						bestFrameFound = true;
						lfi = *bestit;
						pending.erase(bestit);
					}
					else pending.clear();
				}

				if (bestFrameFound)
				{
					QString name{ QString::fromStdU16String(lfi->filePath.filename().generic_u16string()) };

					stackingEngine.ComputeOffset(*lfi);
					emit setImageOffsets(name, 0, 0, 0);

					stackingEngine.AddImage(*lfi, pProgress);
					emit fileStacked(lfi);
					referenceFrameIsSet = true;
					//
					// Call the mf that will save the stacked image and emit a signal o
					//
					emitStackedImage(lfi->filePath);

					emit setImageInfo(name, II_SETREFERENCE);
				}

			}
		}
		else
		{
			double					dX, dY, angle;
			QString					strError;
			QString					strText;
			bool					error = false;
			QString					warning;

			std::shared_ptr<CLightFrameInfo> pInfo;

			{
				QMutexLocker lock(&mutex);
				pInfo = pending.front();
				pending.pop_front();
			}

			CLightFrameInfo	lfi{ *pInfo };

			QString name{ QString::fromStdU16String(lfi.filePath.filename().generic_u16string()) };

			if (stackingEngine.ComputeOffset(lfi))
			{
				lfi.m_BilinearParameters.Offsets(dX, dY);
				angle = lfi.m_BilinearParameters.Angle(lfi.RenderedWidth()) * 180.0 / M_PI;
				emit setImageOffsets(name, dX, dY, angle);
				emit setImageInfo(name, II_DONTSTACK_STACK);

				if(imageWarning(lfi.filePath, dX, dY, angle, warning))
				{
					strText = tr("Warning: Image %1 -> %2\n", "IDS_LOG_WARNING")
						.arg(name)
						.arg(warning);
					emit writeToLog(strText, true, false, true, QColorConstants::Svg::orange);
					emit handleWarning(strText);
				}

				if (isImageStackable(lfi.filePath, dX, dY, angle, strError))
				{
					stackingEngine.AddImage(lfi, pProgress);
					emit fileStacked(pInfo);

					QPointF		pt1, pt2, pt3, pt4;

					lfi.m_BilinearParameters.Footprint(pt1, pt2, pt3, pt4);
					emit setImageFootprint(pt1, pt2, pt3, pt4);
					//
					// Call the mf that will save the stacked image and emit a signal o
					//
					emitStackedImage(lfi.filePath);			
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

		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };
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
				emit setImageInfo(name, II_DONTSTACK_DX);
			if (fabs(dY) > maxOffset)
				emit setImageInfo(name, II_DONTSTACK_DY);
		}

		if (result && liveSettings->IsDontStack_Angle())
		{
			double maxAngle{ liveSettings->GetAngle() };
			if (fabs(angle) > maxAngle)
			{
				result = false;
				error = tr("Angle (%L1\xc2\xb0) is greater than %L2\xc2\xb0", "IDS_NOSTACK_ANGLE")
					.arg(angle, 0, 'f', 2).arg(maxAngle, 0, 'f', 2);
				emit setImageInfo(name, II_DONTSTACK_ANGLE);
			}
		}

		return result;
	}

	bool FileStacker::imageWarning(const fs::path& file, double dX, double dY, double angle, QString& warning)
	{
		bool result = false;

		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };
		if (liveSettings->IsWarning_Offset())
		{
			double maxOffset{ liveSettings->GetOffset() };
			if ((fabs(dX) > maxOffset) || (fabs(dY) > maxOffset))
			{
				result = true;
				warning = tr("dX (%L1 pixels) or dY (%L2 pixels) is greater than %L3 pixels", "IDS_NOSTACK_OFFSET")
					.arg(dX, 0, 'f', 2).arg(dX, 0, 'f', 2).arg(maxOffset, 0, 'f', 2);
			};
			if (fabs(dX) > maxOffset)
			{
				emit setImageInfo(name, II_WARNING_DX);
			};
			if (fabs(dY) > liveSettings->GetOffset())
			{
				emit setImageInfo(name, II_WARNING_DY);
			};
		};

		if (liveSettings->IsWarning_Angle())
		{
			double maxAngle{ liveSettings->GetAngle() };
			if (fabs(angle) > maxAngle)
			{
				result = true;
				warning = tr("Angle (%L1\xc2\xb0) is greater than %L2\xc2\xb0", "IDS_NOSTACK_ANGLE")
					.arg(angle, 0, 'f', 2).arg(maxAngle, 0, 'f', 2);
				emit setImageInfo(name, II_WARNING_ANGLE);
			};
		};

		return result;
	};

	/* ------------------------------------------------------------------- */

	void FileStacker::saveStackedImage(const std::shared_ptr<CMemoryBitmap> pStackedImage)
	{
		ZFUNCTRACE_RUNTIME();
		if (pStackedImage)
		{
			QString	outputFile{ liveSettings->GetStackedOutputFolder() };
			if (!outputFile.isEmpty())
			{
				outputFile += "/Autostack.tif";

				fs::path file{ outputFile.toStdU16String() };
				std::error_code ec;
				fs::remove(file, ec);

				const QString description("Autostacked Image");
				pProgress->Start2("Saving Stacked Image", 0);
				WriteTIFF(file, pStackedImage.get(), pProgress, description, 0, -1, stackingEngine.GetTotalExposure(), 0.0);
				pProgress->End2();
				QString message{ tr("The stacked image has been saved", "IDS_STACKEDIMAGESAVED") + "\n" };
				emit writeToLog(message, true);
				emit stackedImageSaved();
			}
		}
	};

	/* ------------------------------------------------------------------- */

	void FileStacker::emitStackedImage(const fs::path& file)
	{
		ZFUNCTRACE_RUNTIME();
		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };
		std::shared_ptr<CMemoryBitmap>	pStackedImage{ stackingEngine.getStackedImage() };

		std::shared_ptr<LoadedImage> loadedImage{ std::make_shared<LoadedImage>() };
		loadedImage->fileName = file;
		loadedImage->m_pBitmap = pStackedImage;
		
		if (pStackedImage)
		{
			auto image = makeQImage(pStackedImage);
			loadedImage->m_Image = image;
		}

		emit showStackedImage(loadedImage, stackingEngine.GetNrStackedImages(), stackingEngine.GetTotalExposure());
		emit writeToLog(tr("Image %1 has been added to the stack\n").arg(name), true);
		//
		// If there's anything to save...
		// 
		if (pStackedImage)
		{
			++unsavedImageCount;
			if (liveSettings->IsStack_Save() &&
				liveSettings->GetSaveCount() <= unsavedImageCount)
			{
				// Save the stacked image in the output folder
				saveStackedImage(pStackedImage);

				unsavedImageCount = 0;
			}
		}
	}

	/* ------------------------------------------------------------------- */

	std::shared_ptr<QImage>	FileStacker::makeQImage(const std::shared_ptr<CMemoryBitmap>& pStackedImage)
	{
		ZFUNCTRACE_RUNTIME();

		size_t width = pStackedImage->Width(), height = pStackedImage->Height();
		const int numberOfProcessors = CMultitask::GetNrProcessors();
		uchar* pImageData{ nullptr };

		std::shared_ptr<QImage> image = std::make_shared<QImage>((int)width, (int)height, QImage::Format_RGB32);

		struct thread_vars {
			const CMemoryBitmap* source;
			BitmapIteratorConst<const CMemoryBitmap*> pixelItSrc;
			explicit thread_vars(const CMemoryBitmap* s) : source{ s }, pixelItSrc{ source }
			{}
			thread_vars(const thread_vars& rhs) : source{ rhs.source }, pixelItSrc{ rhs.source }
			{}
		};

		//
		//				**** W A R N I N G ***
		// 
		// Calling QImage::bits() on a non-const QImage causes a
		// deep copy of the image data on the assumption that it's
		// about to be changed. That's fine
		//
		// Unfortunately QImage::scanLine does the same thing, which
		// means that it's not safe to use it inside openmp loops, as
		// the deep copy code isn't thread safe.
		// 
		// In any case making a deep copy of the image data for every
		// row of the image data is hugely inefficient.
		//

		if (pStackedImage->IsMonochrome() && pStackedImage->IsCFA())
		{
			// Slow Method

			//
			// Point to the first RGB quad in the QImage which we
			// need to cast to QRgb* (which is unsigned int*) from
			// unsigned char * which is what QImage::bits() returns
			//

			pImageData = image->bits();
			auto bytes_per_line = image->bytesPerLine();

#pragma omp parallel for schedule(guided, 50) default(none) if(numberOfProcessors > 1)
			for (int j = 0; j < height; j++)
			{
				QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (j * bytes_per_line));
				for (int i = 0; i < width; i++)
				{
					double			fRed, fGreen, fBlue;
					pStackedImage->GetPixel(i, j, fRed, fGreen, fBlue);

					*pOutPixel++ = qRgb(std::clamp(fRed, 0.0, 255.0),
						std::clamp(fGreen, 0.0, 255.0),
						std::clamp(fBlue, 0.0, 255.0));
				}
			}
		}
		else
		{
			// Fast Method

			//
			// Point to the first RGB quad in the QImage which we
			// need to cast to QRgb* (which is unsigned int*) from
			// unsigned char * which is what QImage::bits() returns
			//

			pImageData = image->bits();
			auto bytes_per_line = image->bytesPerLine();
			thread_vars threadVars(pStackedImage.get());

#pragma omp parallel for schedule(guided, 50) firstprivate(threadVars) default(none) if(numberOfProcessors > 1)
			for (int j = 0; j < height; j++)
			{
				QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (j * bytes_per_line));
				threadVars.pixelItSrc.Reset(0, j);

				for (int i = 0; i < width; i++, ++threadVars.pixelItSrc, ++pOutPixel)
				{
					double			fRed, fGreen, fBlue;
					threadVars.pixelItSrc.GetPixel(fRed, fGreen, fBlue);

					*pOutPixel = qRgb(std::clamp(fRed, 0.0, 255.0),
						std::clamp(fGreen, 0.0, 255.0),
						std::clamp(fBlue, 0.0, 255.0));
				}
			}
		};

		return image;
	};
}
