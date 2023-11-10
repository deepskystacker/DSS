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
// FileRegistrar.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "fileregistrar.h"
#include "progresslive.h"
#include "BitmapExt.h"
#include "BitmapInfo.h"
#include "RegisterEngine.h"
#include "LiveSettings.h"
#include "dssliveenums.h"

namespace DSS
{
	FileRegistrar::FileRegistrar(QObject* parent, ProgressLive* progress) :
		QThread{ parent },
		registrationEnabled{ false },
		closing{false},
		pProgress{progress}
	{
		ZTRACE_RUNTIME("File registrar active");
		start();
	}

	FileRegistrar::~FileRegistrar()
	{
		closing = true;
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

	void FileRegistrar::dropPendingImages()
	{
		QMutexLocker lock(&mutex);
		pending.clear();
	}

	size_t FileRegistrar::pendingImageCount()
	{
		if (closing) return 0;
		QMutexLocker lock(&mutex);
		return pending.size();
	}


	void FileRegistrar::run()
	{
		forever
		{
			fs::path file;
			{
				QMutexLocker lock(&mutex);
				//
				// Wait for registration to be be enabled, and if enabled, for work to arrive
				//
				if (!registrationEnabled || pending.empty())
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
		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };
		ZTRACE_RUNTIME(" Adding file %s to work queue", name.toUtf8().constData());
		emit addImageToList(file);		// Add the image to the image list in Pending state
		QMutexLocker lock(&mutex);
		emit writeToLog(tr("-> New file: %1\n","IDS_LOG_NEWFILE").arg(name), false, false, false, Qt::green);
		pending.emplace_back(file);
		if (registrationEnabled)
		{
			condvar.wakeOne();
		}
	}

	void FileRegistrar::registerImage(fs::path file)
	{
		ZFUNCTRACE_RUNTIME();
		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };

		ZTRACE_RUNTIME("Registering %s", name.toUtf8().constData());
		CBitmapInfo			bmpInfo;
		bool result{ false };

		if (GetPictureInfo(file, bmpInfo) && bmpInfo.CanLoad())
		{
			QString strText;
			QString strDescription;

			bmpInfo.GetDescription(strDescription);
			if (bmpInfo.m_lNrChannels == 3)
				strText = tr("Loading %1 bit/ch %2 light frame\n%3", "IDS_LOADRGBLIGHT").arg(bmpInfo.m_lBitsPerChannel).arg(strDescription).arg(name);
			else
				strText = tr("Loading %1 bits gray %2 light frame\n%3", "IDS_LOADGRAYLIGHT").arg(bmpInfo.m_lBitsPerChannel).arg(strDescription).arg(name);

			pProgress->Start2(strText, 0);
			CAllDepthBitmap				adb;
			adb.SetDontUseAHD(true);

			result = LoadPicture(file, adb, pProgress);
			pProgress->End2();

			if (result)
			{
				std::shared_ptr<LoadedImage> loadedImage{ std::make_shared<LoadedImage>() };
				loadedImage->fileName = file;
				loadedImage->m_pBitmap = adb.m_pBitmap;
				loadedImage->m_Image = adb.m_Image;

				emit fileLoaded(loadedImage);

				std::shared_ptr<CLightFrameInfo>  lfi {std::make_shared<CLightFrameInfo>()};
				// Now register the image

				lfi->SetBitmap(file, false, false);
				lfi->SetProgress(pProgress);
				lfi->RegisterPicture(adb.m_pBitmap.get());
				lfi->SaveRegisteringInfo();
				lfi->m_lISOSpeed = bmpInfo.m_lISOSpeed;
				lfi->m_lGain = bmpInfo.m_lGain;
				lfi->m_fExposure = bmpInfo.m_fExposure;

				int count = static_cast<int>(lfi->m_vStars.size()); // Work round LUpdate bug
				strText = tr("Image %1 registered: %n star(s) detected - FWHM = %L2 - Score = %L3\n", "IDS_LOG_REGISTERRESULTS", count)
					.arg(name)
					.arg(lfi->m_fFWHM, 0, 'f', 2)
					.arg(lfi->m_fOverallQuality, 0, 'f', 2);

				ZTRACE_RUNTIME(strText.toUtf8().constData());
				strText += "\n";
				emit writeToLog(strText,
					true);
				emit fileRegistered(lfi);
				emit setImageInfo(name, II_DONTSTACK_REGISTER);

				QString warning;

				if (imageWarning(file, lfi->m_vStars.size(), lfi->m_fFWHM, lfi->m_fOverallQuality, lfi->m_SkyBackground.m_fLight * 100.0, warning))
				{
					strText = tr("Warning: Image %1 -> %2\n", "IDS_LOG_WARNING").arg(name).arg(warning);
					emit writeToLog(strText, true, false, false, QColorConstants::Svg::orange);
					emit handleWarning(strText);
				};
				
				if (isImageStackable(file, lfi->m_vStars.size(), lfi->m_fFWHM, lfi->m_fOverallQuality, lfi->m_SkyBackground.m_fLight * 100.0, strText))
				{
					// Check against stacking conditions before adding it to
					// the stack list
					emit addToStackingQueue(lfi);
				}
				else
				{
					strText = tr("Image %1 is not stackable (%2)\n", "IDS_LOG_IMAGENOTSTACKABLE1").arg(name).arg(strText);
					emit writeToLog(strText, true, true, false, Qt::red);
					emit fileNotStackable(file);
				}

			}
			else
			{
				strText = tr("Error loading file %1\n", "IDS_LOG_ERRORLOADINGFILE").arg(name);
				emit writeToLog(strText, true, true, false, Qt::red);
				emit fileNotStackable(file);
			};
		};
	}

	bool FileRegistrar::isImageStackable(const fs::path& file, double fStarCount, double fFWHM, double fScore, double fSkyBackground, QString& error)
	{
		bool result = true;
		LiveSettings* liveSettings { DSSLive::instance()->liveSettings.get() };
		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };
		if (liveSettings->IsDontStack_Score())
		{
			if (fScore < liveSettings->GetScore())
			{
				result = false;
				error = tr("Score (%L1) is less than %L2", "IDS_NOSTACK_SCORE").arg(fScore, 0, 'f', 2).arg(liveSettings->GetScore(), 0, 'f', 2);
				emit setImageInfo(name, II_DONTSTACK_SCORE);
			};
		};

		if (liveSettings->IsDontStack_Stars())
		{
			if (fStarCount < liveSettings->GetStars())
			{
				result = false;
				error = tr("Star count(%L1) is less than %L2").arg(fStarCount, 0, 'f').arg((double)liveSettings->GetStars(), 0, 'f');
				emit setImageInfo(name, II_DONTSTACK_STARS);
			};
		};

		if (liveSettings->IsDontStack_FWHM())
		{
			if (fFWHM > liveSettings->GetFWHM())
			{
				result = false;
				error = tr("FWHM (%L1 pixels) is greater than %L2 pixels", "IDS_NOSTACK_FWHM").arg(fFWHM, 0, 'f', 2).arg((double)liveSettings->GetFWHM(), 0, 'f', 2);
				emit setImageInfo(name, II_DONTSTACK_FWHM);
			};
		};

		if (liveSettings->IsDontStack_SkyBackground())
		{
			if (fSkyBackground > liveSettings->GetSkyBackground())
			{
				result = false;
				error = tr("Sky Background (%L1%) is greater than %L2%", "IDS_NOSTACK_SKYBACKGROUND").arg(fSkyBackground, 0, 'f', 2).arg((double)liveSettings->GetSkyBackground(), 0, 'f', 2);
				emit setImageInfo(name, II_DONTSTACK_SKYBACKGROUND);
			};
		};


		return result;
	};


	bool FileRegistrar::imageWarning(const fs::path& file, double fStarCount, double fFWHM, double fScore, double fSkyBackground, QString& warning)
	{
		bool result = false;
		LiveSettings* liveSettings{ DSSLive::instance()->liveSettings.get() };
		QString name{ QString::fromStdU16String(file.filename().generic_u16string()) };

		if (liveSettings->IsWarning_Score())
		{
			if (fScore < liveSettings->GetScore())
			{
				result = true;
				warning = tr("Score (%L1) is less than %L2", "IDS_NOSTACK_SCORE").arg(fScore, 0, 'f', 2).arg(liveSettings->GetScore());
				emit setImageInfo(name, II_WARNING_SCORE);
			};
		};

		if (liveSettings->IsWarning_Stars())
		{
			if (fStarCount < liveSettings->GetStars())
			{
				result = true;
				warning = tr("Star count(%L1) is less than %L2").arg(fStarCount, 0, 'f').arg((double)liveSettings->GetStars(), 0, 'f');
				emit setImageInfo(name, II_WARNING_STARS);
			};
		};

		if (liveSettings->IsWarning_FWHM())
		{
			if (fFWHM > liveSettings->GetFWHM())
			{
				result = true;
				warning = tr("FWHM (%L1 pixels) is greater than %L2 pixels", "IDS_NOSTACK_FWHM").arg(fFWHM, 0, 'f', 2).arg((double)liveSettings->GetFWHM(), 0, 'f', 2);
				emit setImageInfo(name, II_WARNING_FWHM);
			};
		};

		if (liveSettings->IsWarning_SkyBackground())
		{
			if (fSkyBackground > liveSettings->GetSkyBackground())
			{
				result = true;
				warning = tr("Sky Background (%L1%) is greater than %L2%", "IDS_NOSTACK_SKYBACKGROUND").arg(fSkyBackground, 0, 'f', 2).arg((double)liveSettings->GetSkyBackground(), 0, 'f', 2);
				emit setImageInfo(name, II_WARNING_SKYBACKGROUND);
			};
		};

		return result;
	};

}