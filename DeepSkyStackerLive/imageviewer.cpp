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
#include "DeepSkyStackerLive.h"
#include "LiveSettings.h"
#include "footprint.h"

namespace DSS
{
	ImageViewer::ImageViewer(QWidget *parent)
		: QWidget{ parent },
		initialised{ false },
		liveSettings{ DSSLive::instance()->liveSettings.get() }
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
		connect(fourCorners, &QToolButton::clicked, picture, &DSS::ImageView::on_fourCorners_clicked);
		connect(gamma, &QLinearGradientCtrl::pegMove, this, &ImageViewer::gammaChanging);
		connect(gamma, &QLinearGradientCtrl::pegMoved, this, &ImageViewer::gammaChanged);
		connect(information, &QLabel::linkActivated, this, &ImageViewer::saveClicked);
		connect(copyToClipboard, &QLabel::linkActivated, this, &ImageViewer::copyStackedImage);
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
		ZASSERT(5 == stops.size());

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
		ZASSERT(5 == stops.size());

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

	void ImageViewer::setLoadedImage(std::shared_ptr<LoadedImage> p)
	{
		loadedImage = *p;
		if (loadedImage.m_Image->isNull())
		{
			picture->clear();
			clearFootprint();
			return;
		}
		if (gammaTransformation.isInitialized())
			ApplyGammaTransformation(loadedImage.m_Image.get(), loadedImage.m_pBitmap.get(), gammaTransformation);
		picture->setPixmap(QPixmap::fromImage(*(loadedImage.m_Image)));
	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::saveClicked()
	{
		QString outputFolder{ liveSettings->GetStackedOutputFolder() };
		if (!outputFolder.isEmpty())
		{
			information->setText(tr("The stacked image will be saved as soon as possible",
				"IDS_STACKEDIMAGEWILLBESAVED"));
			//
			// Save the pointer to bitmap in a weak pointer, so that when it is saved
			// we can check if we should update the message (if the pointer is still valid).
			//
			bitmapPendingSave = loadedImage.m_pBitmap;
			emit saveStackedImage(loadedImage.m_pBitmap);
		}
		else
		{
			QMessageBox::information(this, "DeepSkyStackerLive",
				tr("You must select an output folder first.\nGo to the Settings tab to select the Stacked Image Output Folder.",
					"IDS_NOSTACKEDIMAGEFOLDER"));

		}
	}

	void ImageViewer::stackedImageSaved()
	{
		if (!bitmapPendingSave.expired())
		{
			QString text{ tr("The stacked image has been saved", "IDS_STACKEDIMAGESAVED") };
			information->setText(text);
		}
	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::copyStackedImage()
	{
		QGuiApplication::clipboard()->setImage(*(loadedImage.m_Image));
	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::enableFootprint()
	{
		footPrint = new Footprint(picture);
	}

	/* ------------------------------------------------------------------- */

	void ImageViewer::setFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
	{
		if (footPrint)
			footPrint->setFootprint(pt1, pt2, pt3, pt4);
	};

	/* ------------------------------------------------------------------- */

	void ImageViewer::clearFootprint()
	{
		if (footPrint)
			footPrint->clearFootprint();
	}; }
