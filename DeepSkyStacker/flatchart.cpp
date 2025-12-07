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
// FlatChart.cpp : implementation file
//
#include "pch.h"
#include "FrameInfo.h"
#include "Stars.h"
#include "FlatChart.h"
#include "GrayBitmap.h"
#include "RAWUtils.h"
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QwtLinearColorMap>
#include <QwtPlotSpectrogram>
#include <QwtLogScaleEngine>
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
class FlatColourMap : public QwtLinearColorMap
{
public:
	FlatColourMap() : QwtLinearColorMap(Qt::black, Qt::white, QwtColorMap::RGB)
	{
		setMode(QwtLinearColorMap::Mode::FixedColors);
		addColorStop(0.125, QColorConstants::Svg::dimgray);
		addColorStop(0.250, QColorConstants::Svg::gray);
		addColorStop(0.375, QColorConstants::Svg::darkgray);
		addColorStop(0.500, QColorConstants::Svg::silver);
		addColorStop(0.625, QColorConstants::Svg::lightgray);
		addColorStop(0.750, QColorConstants::Svg::gainsboro);
		addColorStop(0.875, QColorConstants::Svg::whitesmoke);
	}
};

namespace DSS
{
	FlatChart::FlatChart(const ListBitMap& lbmp, QWidget* parent) :
		QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint),
		gridData{ std::make_unique<GridData>()},
		spectrogram{ new QwtPlotSpectrogram(tr("Flat Contour Chart")) },
		rasterData{ new QwtMatrixRasterData() }
	{
		setupUi(this);
		setWindowTitle(tr("Flat Contour Chart") + " - " + lbmp.filePath.filename().generic_u16string());

		spectrogram->setRenderThreadCount(0); // use system default thread count
		spectrogram->setCachePolicy(QwtPlotRasterItem::PaintCache);

		width = lbmp.m_lWidth;
		size_t height = lbmp.m_lHeight;

		flatPlot->setAxisTitle(QwtPlot::xBottom, "X");
		flatPlot->setAxisTitle(QwtPlot::yLeft, "Y");
		rasterData->setInterval(Qt::XAxis, QwtInterval(0.0, static_cast<double>(width - 1)));
		rasterData->setInterval(Qt::YAxis, QwtInterval(0.0, static_cast<double>(height - 1)));
		flatPlot->setAxisScale(QwtPlot::xBottom, 0.0, static_cast<double>(width - 1), 500);
		flatPlot->setAxisScale(QwtPlot::yLeft, static_cast<double>(height - 1), 0.0, 500);

		spectrogram->setData(rasterData);
		spectrogram->attach(flatPlot);


		//
		// Set up the data array.  LoadPicture will fill the bitmap and QImage with the image data
		//
		::LoadPicture(lbmp.filePath, bitmap, nullptr);
	
		valueData.reserve(width * height);
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				valueData.push_back(qGray(bitmap.m_Image->pixel(x,y)));
			}

		connectSignalsToSlots();

		//
		// Drive the plotFlat member function
		//
		plotFlat();
	}

	void FlatChart::connectSignalsToSlots()
	{
		connect(cancelButton, &QPushButton::clicked,
			this, &FlatChart::cancelPressed);
	}

	//
	// Slots
	//
	void FlatChart::cancelPressed()
	{
		reject();
	}

	void FlatChart::plotFlat()
	{
		auto p = std::minmax_element(valueData.cbegin(), valueData.cend());
		rasterData->setValueMatrix(valueData, static_cast<int>(width));

		spectrogram->setColorMap(new FlatColourMap);
		rasterData->setInterval(Qt::ZAxis, QwtInterval(*p.first, *p.second));

		// A color bar on the right axis
		QwtScaleWidget* rightAxis = flatPlot->axisWidget(QwtAxis::YRight);
		rightAxis->setTitle(tr("Intensity"));
		rightAxis->setColorBarEnabled(true);
		rightAxis->setColorMap(rasterData->interval(Qt::ZAxis), new FlatColourMap);
		flatPlot->setAxisScale(QwtAxis::YRight, *p.first, *p.second);
		flatPlot->setAxisVisible(QwtAxis::YRight);

		flatPlot->plotLayout()->setAlignCanvasToScales(true);

		flatPlot->replot();
	}
}