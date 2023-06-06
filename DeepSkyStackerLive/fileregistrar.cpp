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
#include "BitmapExt.h"
#include "BitmapInfo.h"
namespace DSS
{
	FileRegistrar::FileRegistrar(QObject* parent, ProgressLive* progress) :
		QThread{ parent },
		pProgress{progress}
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
		CBitmapInfo			bmpInfo;

		if (GetPictureInfo(file, bmpInfo) && bmpInfo.CanLoad())
		{
			QString strText;
			QString strDescription;

			bmpInfo.GetDescription(strDescription);
			if (bmpInfo.m_lNrChannels == 3)
				strText = QCoreApplication::translate("LiveEngine", "Loading %1 bit/ch %2 light frame\n%3", "IDS_LOADRGBLIGHT").arg(bmpInfo.m_lBitPerChannel).arg(strDescription).arg(file.generic_u16string().c_str());
			else
				strText = QCoreApplication::translate("LiveEngine", "Loading %1 bits gray %2 light frame\n%3", "IDS_LOADGRAYLIGHT").arg(bmpInfo.m_lBitPerChannel).arg(strDescription).arg(file.generic_u16string().c_str());
#if 0
			Start2(strText, 0);
			CAllDepthBitmap				adb;
			adb.SetDontUseAHD(true);

			bResult = LoadPicture(szFileName, adb, this);
			End2();
			if (bResult)
			{
				PostFileLoaded(adb.m_pBitmap, adb.m_pWndBitmap, szFileName);
				PostChangeImageStatus(szFileName, IS_LOADED);

				// Now register the image
				CLightFrameInfo			lfi;

				strText = QCoreApplication::translate("LiveEngine", "Registering %1", "IDS_REGISTERINGNAME").arg(QString::fromWCharArray(szFileName));
				Start2(strText, 0);
				lfi.SetBitmap(szFileName, false, false);
				lfi.SetProgress(this);
				lfi.RegisterPicture(adb.m_pBitmap.get());
				lfi.SaveRegisteringInfo();
				lfi.m_lISOSpeed = bmpInfo.m_lISOSpeed;
				lfi.m_lGain = bmpInfo.m_lGain;
				lfi.m_fExposure = bmpInfo.m_fExposure;
				End2();
				PostFileRegistered(szFileName);
				PostChangeImageStatus(szFileName, IS_REGISTERED);

				TCHAR					szName[_MAX_FNAME];
				TCHAR					szExt[_MAX_EXT];
				CString					strName;

				_tsplitpath(szFileName, nullptr, nullptr, szName, szExt);
				strName.Format(_T("%s%s"), szName, szExt);
				int count = static_cast<int>(lfi.m_vStars.size()); // Work round LUpdate bug
				strText = QCoreApplication::translate("LiveEngine", "Image %1 registered: %n star(s) detected - FWHM = %2 - Score = %3\n", "IDS_LOG_REGISTERRESULTS", count)
					.arg(QString::fromWCharArray(strName))
					.arg(lfi.m_fFWHM, 0, 'f', 2)
					.arg(lfi.m_fOverallQuality, 0, 'f', 2);

				PostToLog(strText, true);

				CString					strError;
				BOOL					bWarning;
				CString					strWarning;
				bWarning = IsImageWarning1(szFileName, lfi.m_vStars.size(), lfi.m_fFWHM, lfi.m_fOverallQuality, lfi.m_SkyBackground.m_fLight * 100.0, strWarning);
				PostChangeImageInfo(szFileName, II_DONTSTACK_NONE);
				if (bWarning)
				{
					strText = QCoreApplication::translate("LiveEngine", "Warning: Image %1 -> %2\n", "IDS_LOG_WARNING").arg(QString::fromWCharArray(szFileName)).arg(QString::fromWCharArray(strWarning));
					PostToLog(strText, true, false, false, RGB(208, 127, 0));
					PostWarning(strWarning);
				};
				if (IsImageStackable1(szFileName, lfi.m_vStars.size(), lfi.m_fFWHM, lfi.m_fOverallQuality, lfi.m_SkyBackground.m_fLight * 100.0, strError))
				{
					// Check against stacking conditions before adding it to
					// the stack list
					m_qToStack.push_back(lfi);
				}
				else
				{
					strText = QCoreApplication::translate("LiveEngine", "Image %1 is not stackable (%2)\n", "IDS_LOG_IMAGENOTSTACKABLE1").arg(QString::fromWCharArray(szFileName)).arg(QString::fromWCharArray(strError));
					PostToLog(strText, true, true, false, RGB(255, 0, 0));
					PostChangeImageStatus(szFileName, IS_NOTSTACKABLE);
					MoveImage(szFileName);
				};
			}
			else
			{
				strText = QCoreApplication::translate("LiveEngine", "Error loading file %1\n", "IDS_LOG_ERRORLOADINGFILE").arg(QString::fromWCharArray(szFileName));
				PostToLog(strText, true, true, false, RGB(255, 0, 0));
				MoveImage(szFileName);
			};
#endif
		};

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