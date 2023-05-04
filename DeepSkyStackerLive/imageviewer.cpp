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
// ImageViewer.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "imageviewer.h"

namespace DSS
{
	ImageViewer::ImageViewer(QWidget *parent)
		: QWidget{ parent },
		initialised{ false }
	{
		setupUi(this);

		gamma->setColorAt(sqrt(0.5), QColor(qRgb(128, 128, 128)));
		gamma->setPegsOnLeftOrBottom(true).
			setOrientation(QLinearGradientCtrl::Orientation::ForceHorizontal);

		connectSignalsToSlots();
	}

	/* ------------------------------------------------------------------- */

	ImageViewer::~ImageViewer()
	{}

	/* ------------------------------------------------------------------- */

	void ImageViewer::connectSignalsToSlots()
	{
		connect(fourCorners, SIGNAL(clicked(bool)), picture, SLOT(on_fourCorners_clicked(bool)));
		connect(&imageLoader, SIGNAL(imageLoaded()), this, SLOT(imageLoad()));
		connect(&imageLoader, SIGNAL(imageLoadFailed()), this, SLOT(imageLoadFailed()));
		connect(gamma, SIGNAL(pegMove(int)), this, SLOT(gammaChanging(int)));
		connect(gamma, SIGNAL(pegMoved(int)), this, SLOT(gammaChanged(int)));

	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::gammaChanging(int peg)
	{
		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ gamma->gradient() };
		QGradientStops stops{ gradient.stops() };

		//
		// Adjust stop values if necessary
		//
		Q_ASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;
		bool adjust{ false };

		switch (peg)
		{
		case 1:
			// Black point moving
			if (blackPoint > whitePoint - 0.02)
			{
				blackPoint = whitePoint - 0.02;
				adjust = true;
			};
			if (blackPoint > greyPoint - 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			break;
		case 2:
			// Gray point moving
			if (greyPoint < blackPoint + 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			if (greyPoint > whitePoint - 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		case 3:
			// White point moving
			if (whitePoint < blackPoint + 0.02)
			{
				whitePoint = blackPoint + 0.02;
				adjust = true;
			};
			if (whitePoint < greyPoint + 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		};
		if (adjust)
		{
			stops[1].first = blackPoint;
			stops[2].first = greyPoint;
			stops[3].first = whitePoint;
			gradient.setStops(stops);
			gamma->update();
		};
	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::gammaChanged(int peg)
	{
		//
		// Before applying the changes, make any corrections necessary by invoking gammaChanging 
		// one final time
		//
		gammaChanging(peg);

		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ gamma->gradient() };
		QGradientStops stops{ gradient.stops() };
		//
		// Adjust stop values if necessary
		//
		Q_ASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;

		// Adjust Gamma
		gammaTransformation.initTransformation(blackPoint * blackPoint, greyPoint * greyPoint, whitePoint * whitePoint);

		if (loadedImage.m_pBitmap)
		{
			ApplyGammaTransformation(loadedImage.m_Image.get(), loadedImage.m_pBitmap.get(), gammaTransformation);
			// Refresh
			picture->setPixmap(QPixmap::fromImage(*(loadedImage.m_Image)));
		}
	}

	/* ------------------------------------------------------------------- */

	//
	// This member function/Slot is invoked under two conditions:
	//
	// 1. To request the loading of an image file which may or may not already be loadedImage,
	//    by invoking imageLoader.load().
	// 
	//    If the image was previously loadedImage is still available in the image cache then the result will be true and 
	//    both pBitMap and pImage will be set.
	//
	//    If the image is not in the cache, then the result will be false, and imageLoader.load will load the image
	//    into the cache in a background thread running in the default Qt threadpool.
	//
	// 2. On completion of image loading by the background thread.  In this case the image will now be available in 
	//    the cache, so invoking imageLoader.load() will now return true.
	//
	void ImageViewer::imageLoad()
	{
		std::shared_ptr<CMemoryBitmap>	pBitmap;
		std::shared_ptr<QImage>	pImage;
		QString fileName;
		if (!fileToShow.empty()) fileName = QString::fromStdU16String(fileToShow.generic_u16string());

		try
		{
			if (!fileToShow.empty() && imageLoader.load(fileToShow, pBitmap, pImage))
			{
				//
				// The image we want is available in the cache
				//
				loadedImage.m_Image = pImage;
				loadedImage.m_pBitmap = pBitmap;
				if (gammaTransformation.isInitialized())
					ApplyGammaTransformation(loadedImage.m_Image.get(), loadedImage.m_pBitmap.get(), gammaTransformation);
				picture->setPixmap(QPixmap::fromImage(*(loadedImage.m_Image)));

				information->setStyleSheet(
					"QLabel { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
					"stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255)) }");
				information->setText(fileName);
			}
			else if (!fileToShow.empty())
			{
				information->setStyleSheet(
					"QLabel { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,"
					"stop:0 rgba(252, 251, 222, 0), stop:1 rgba(255, 151, 154, 255)) }");
				information->setText(tr("Loading %1", "IDS_LOADPICTURE")
					.arg(fileName));
			}
			else
			{
				//
				// Display the blue gradient with no text
				//
				information->setStyleSheet(
					"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
					"stop:0 rgb(224, 244, 252), stop:1 rgb(138, 185, 242)) }");
				information->setText("");
			}
		}
		catch (ZAccessError&)
		{
			QApplication::beep();
			QMessageBox::warning(this,
				"DeepSkyStacker",
				tr("%1 does not exist or is not a file").arg(fileName));
		}
	};

	/* ------------------------------------------------------------------- */

	void ImageViewer::imageLoadFailed()
	{
		QApplication::beep();
		QMessageBox::warning(this,
			"DeepSkyStacker",
			tr("Failed to load image %1").arg(QString::fromStdU16String(fileToShow.generic_u16string())));
	}

}
