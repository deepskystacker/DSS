/****************************************************************************
**
** Copyright (C) 2025 David C. Partridge
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
// QualityChart.cpp : implementation file
//
#include "pch.h"
#include "FrameInfo.h"
#include "Stars.h"
#include "QualityChart.h"
#include "griddata.h"
#include <QwtLinearColorMap>
#include <QwtPlotSpectrogram>
#include <QWtLogScaleEngine>
#include <QwtMatrixRasterData>
#include <QwtScaleWidget>
#include <QwtScaleDraw>
//#include <QwtPlotZoomer>
//#include <QwtPlotPanner>
#include <QwtPlotLayout>
#include <QwtPlotRenderer>
#include <QwtPlotRescaler>
#include <QwtInterval>
#include <QwtPainter>
class FWHMColourMap : public QwtLinearColorMap
{
public:
	FWHMColourMap() : QwtLinearColorMap(Qt::black, Qt::white, QwtColorMap::RGB)
	{
		setMode(QwtLinearColorMap::Mode::FixedColors);
		addColorStop(0.05, QColor(0, 0, 96));
		addColorStop(0.10, QColorConstants::Svg::darkblue);
		addColorStop(0.15, QColorConstants::Svg::blue);
		addColorStop(0.20, QColorConstants::Svg::darkturquoise);
		addColorStop(0.25, QColorConstants::Svg::turquoise);
		addColorStop(0.30, QColorConstants::Svg::green);
		addColorStop(0.35, QColorConstants::Svg::forestgreen);
		addColorStop(0.40, QColorConstants::Svg::yellowgreen);
		addColorStop(0.45, QColorConstants::Svg::yellow);
		addColorStop(0.50, QColorConstants::Svg::gold);
		addColorStop(0.55, QColorConstants::Svg::orange);
		addColorStop(0.60, QColorConstants::Svg::orangered);
		addColorStop(0.65, QColorConstants::Svg::red);
		addColorStop(0.70, QColorConstants::Svg::brown);
		addColorStop(0.75, QColorConstants::Svg::saddlebrown);
		addColorStop(0.80, QColorConstants::Svg::dimgray);
		addColorStop(0.85, QColorConstants::Svg::darkgray);
		addColorStop(0.90, QColorConstants::Svg::silver);
		addColorStop(0.95, QColorConstants::Svg::white);
	}
};
class EccentricityColourMap : public QwtLinearColorMap
{
public:
	EccentricityColourMap() : QwtLinearColorMap(Qt::green, Qt::red, QwtColorMap::RGB)
	{
		setMode(QwtLinearColorMap::Mode::FixedColors);
		addColorStop(0.125, QColorConstants::Svg::forestgreen);
		addColorStop(0.250, QColorConstants::Svg::yellowgreen);
		addColorStop(0.375, QColorConstants::Svg::yellow);
		addColorStop(0.500, QColorConstants::Svg::gold);
		addColorStop(0.625, QColorConstants::Svg::orange);
		addColorStop(0.750, QColorConstants::Svg::orangered);
		addColorStop(0.875, QColorConstants::Svg::red);
	}
};
//class LogarithmicColourMap : public QualityColourMap
//{
//public:
//	LogarithmicColourMap() : QualityColourMap()
//	{
//	}
//
//	QRgb rgb(const QwtInterval& interval, double value) const
//	{
//		return QwtLinearColorMap::rgb(QwtInterval(std::log10(interval.minValue()),
//			std::log10(interval.maxValue())),
//			std::log10(value));
//	}
//};
namespace DSS
{
	QualityChart::QualityChart(const ListBitMap& lbmp, QWidget* parent) :
		QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint),
		lightFrameInfo(lbmp),
		gridData{ new GridData(this)},
		spectrogram{ new QwtPlotSpectrogram("Star Quality") },
		rasterData{ new QwtMatrixRasterData() }
	{
		setupUi(this);
		setWindowTitle("Star Quality Chart - " + lbmp.filePath.filename().generic_u16string());
		lightFrameInfo.SetBitmap(lbmp.filePath);

		setWindowTitle("Star Quality Chart - " + lbmp.filePath.filename().generic_u16string());		lightFrameInfo.SetBitmap(lbmp.filePath);
		//
		// Fill the data vectors
		//
		xValues.reserve(lightFrameInfo.m_vStars.size());
		yValues.reserve(lightFrameInfo.m_vStars.size());
		fwhmValues.reserve(lightFrameInfo.m_vStars.size());
		eccentricityValues.reserve(lightFrameInfo.m_vStars.size());
		for (const auto& star : lightFrameInfo.m_vStars)
		{
			xValues.emplace_back(star.m_fX);
			yValues.emplace_back(star.m_fY);
			fwhmValues.emplace_back((star.m_fMeanRadius / CRegisteredFrame::RadiusFactor));
			eccentricityValues.emplace_back(star.eccentricity);
		}

		spectrogram->setRenderThreadCount(0); // use system specific thread count
		spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);

		QList< double > contourLevels;
		for (double level = 0.5; level < 10.0; level += 1.0)
			contourLevels += level;
		spectrogram->setContourLevels(contourLevels);

		size_t width = lightFrameInfo.m_lWidth;
		size_t height = lightFrameInfo.m_lHeight;

		qualityPlot->setAxisTitle(QwtPlot::xBottom, "X");
		qualityPlot->setAxisTitle(QwtPlot::yLeft, "Y");
		rasterData->setInterval(Qt::XAxis, QwtInterval(0.0, static_cast<double>(width - 1)));
		rasterData->setInterval(Qt::YAxis, QwtInterval(0.0, static_cast<double>(height - 1)));
		qualityPlot->setAxisScale(QwtPlot::xBottom, 0.0, static_cast<double>(width - 1), 500);
		qualityPlot->setAxisScale(QwtPlot::yLeft, static_cast<double>(height - 1), 0.0, 500);

		spectrogram->setData(rasterData);
		spectrogram->attach(qualityPlot);

		//
		// Set up the grid arrays.
		//
		xg.reserve(width);
		yg.reserve(height);
		zgFWHM.reserve(width * height);
		zgEccentricity.reserve(width * height);
		qDebug() << "Grid size:" << width << "x" << height << "=" << (width * height);

		for (size_t x = 0; x < width; ++x)
			xg.emplace_back(static_cast<double>(x));
		for (size_t y = 0; y < height; ++y)
			yg.emplace_back(static_cast<double>(y));

		radioFWHM->setChecked(true);

		connectSignalsToSlots();

		//
		// Fake up a click on the FWHM button to compute and display the chart
		//
		QTimer::singleShot(100,
			[this]()
			{
				QMetaObject::invokeMethod(this->radioFWHM, "clicked", Qt::ConnectionType::QueuedConnection,
					Q_ARG(bool, true));
			});
	}

	void QualityChart::connectSignalsToSlots()
	{
		connect(radioEccentricity, &QRadioButton::clicked,
			this, &QualityChart::eccentricityButtonClicked);
		connect(radioFWHM, &QRadioButton::clicked,
			this, &QualityChart::fwhmButtonClicked);
		connect(cancelButton, &QPushButton::clicked,
			this, &QualityChart::cancelPressed);
		connect(gridData, &GridData::setProgressRange,
			progressBar, &QProgressBar::setRange);
		connect(gridData, &GridData::setProgressValue,
			progressBar, &QProgressBar::setValue);
	}

	//
	// Slots
	//
	void QualityChart::fwhmButtonClicked(bool checked)
	{
		ZFUNCTRACE_RUNTIME();
		if (checked)
		{
			if (zgFWHM.empty())
			{
				ZTRACE_RUNTIME("FWHM interpolation");
				message->setText(tr("Interpolating FWHM data.  Please be patient."));
				message->repaint();
				QCoreApplication::processEvents();

				interpolating = true;
				gridData->interpolate(xValues, yValues, fwhmValues, xg, yg, zgFWHM, GridData::InterpolationType::GRID_NNIDW, 10.f);
				interpolating = false;
				
				message->setText("");
				message->repaint();
				ZTRACE_RUNTIME("FWHM interpolation complete");
			}

			if (!cancelled)
			{
				//
				// Clear the color map data
				// 
				rasterData->setValueMatrix(QVector<double>{}, 0);

				auto p = std::minmax_element(zgFWHM.cbegin(), zgFWHM.cend());
				qDebug() << "zgFWHM Min:" << *p.first << "zgFWHM Max:" << *p.second;

				spectrogram->setColorMap(new FWHMColourMap);
				rasterData->setInterval(Qt::ZAxis, QwtInterval(*p.first, *p.second));
				// A color bar on the right axis
				QwtScaleWidget* rightAxis = qualityPlot->axisWidget(QwtAxis::YRight);
				rightAxis->setTitle("FWHM");
				rightAxis->setColorBarEnabled(true);
				rightAxis->setColorMap(rasterData->interval(Qt::ZAxis), new FWHMColourMap);
				qualityPlot->setAxisScale(QwtAxis::YRight, *p.first, *p.second);
				qualityPlot->setAxisVisible(QwtAxis::YRight);

				qualityPlot->plotLayout()->setAlignCanvasToScales(true);

				//
				// Update the color map with FWHM values from the interpolated grid
				//
				rasterData->setValueMatrix(QVector<double>(zgFWHM.cbegin(), zgFWHM.cend()), static_cast<int>(xg.size()));

				qualityPlot->replot();
				progressBar->reset();
			}
			else reject();
		}
	}

	void QualityChart::eccentricityButtonClicked(bool checked)
	{
		if (checked)
		{
			if (zgEccentricity.empty())
			{
				ZTRACE_RUNTIME("Star Eccentricity interpolation");
				message->setText(tr("Interpolating Eccentricity data.  Please be patient."));
				message->repaint();
				QCoreApplication::processEvents();

				interpolating = true;
				gridData->interpolate(xValues, yValues, eccentricityValues, xg, yg, zgEccentricity, GridData::InterpolationType::GRID_NNIDW, 10.f);
				interpolating = false;

				message->setText("");
				message->repaint();
				ZTRACE_RUNTIME("Star Eccentricity interpolation complete");
			}

			if (!cancelled)
			{
				//
				// Clear the color map data
				// 
				rasterData->setValueMatrix(QVector<double>{}, 0);

				auto p = std::minmax_element(zgEccentricity.cbegin(), zgEccentricity.cend());
				qDebug() << "zgEccentricity Min:" << *p.first << "zgEccentricity Max:" << *p.second;

				spectrogram->setColorMap(new EccentricityColourMap);
				rasterData->setInterval(Qt::ZAxis, QwtInterval(0.0, 1.0));

				// A color bar on the right axis
				// qualityPlot->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine());
				QwtScaleWidget* rightAxis = qualityPlot->axisWidget(QwtAxis::YRight);
				rightAxis->setTitle(tr("Star Eccentricity"));
				rightAxis->setColorBarEnabled(true);
				rightAxis->setColorMap(rasterData->interval(Qt::ZAxis), new EccentricityColourMap);
				qualityPlot->setAxisScale(QwtAxis::YRight, 1.0, 0.0);
				qualityPlot->setAxisVisible(QwtAxis::YRight);

				qualityPlot->plotLayout()->setAlignCanvasToScales(true);

				qDebug() << zgEccentricity[5202 * 3463]; // Test value
				//
				// Update the color map with Star Eccentricity values from the interpolated grid
				//
				rasterData->setValueMatrix(QVector<double>(zgEccentricity.cbegin(), zgEccentricity.cend()), static_cast<int>(xg.size()));

				qualityPlot->replot();
				progressBar->reset();
			}
			else reject();
		}
	}

	void QualityChart::cancelPressed()
	{
		if (interpolating)
		{
			gridData->cancel = true;		// Stop the interpolation
			cancelled = true;
		}
		else reject();
	}
}