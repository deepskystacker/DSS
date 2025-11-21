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
#include "stars.h"
#include "QualityChart.h"
#include "griddata.h"

namespace DSS
{
	QualityChart::QualityChart(const ListBitMap& lbmp, QWidget* parent) :
		QDialog(parent, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint),
		lightFrameInfo(lbmp)	
	{
		setupUi(this);
		setWindowTitle("Star Quality Chart - " + lbmp.filePath.filename().generic_u16string());
		lightFrameInfo.SetBitmap(lbmp.filePath);

		//
		// Fill the data vectors
		//
		xValues.reserve(lightFrameInfo.m_vStars.size());
		yValues.reserve(lightFrameInfo.m_vStars.size());
		fwhmValues.reserve(lightFrameInfo.m_vStars.size());
		circularityValues.reserve(lightFrameInfo.m_vStars.size());
		for (const auto& star : lightFrameInfo.m_vStars)
		{
			xValues.emplace_back(star.m_fX);
			yValues.emplace_back(star.m_fY);
			fwhmValues.emplace_back((star.m_fMeanRadius / CRegisteredFrame::RadiusFactor));
			circularityValues.emplace_back(star.m_fCircularity);
		}

		//
		// Set up the grid arrays.
		//
		int width = lightFrameInfo.m_lWidth;
		int height = lightFrameInfo.m_lHeight;

		//int xgSize = 1 + lightFrameInfo.m_lWidth / 10; 
		//if (lightFrameInfo.m_lWidth % 10) ++xgSize;
		//int ygSize = 1 + lightFrameInfo.m_lHeight / 10;
		//if (lightFrameInfo.m_lHeight % 10) ++ygSize;
		//xg.reserve(static_cast<size_t>(xgSize));
		//yg.reserve(static_cast<size_t>(ygSize));
		//zg.reserve(static_cast<size_t>(xgSize * ygSize));
		xg.reserve(static_cast<size_t>(width));
		yg.reserve(static_cast<size_t>(height));
		zg.reserve(static_cast<size_t>(width * height));
#if(0)
		for (int x = 0; x < 10 * xgSize; x += 10)
			xg.emplace_back(static_cast<double>((x==0) ? 0:(x-1)));
		for (int y = 0; y < 10*ygSize; y += 10)
			yg.emplace_back(static_cast<double>((y==0) ? 0:(y-1)));
#endif
		for (int x = 0; x < width; ++x)
			xg.emplace_back(static_cast<double>(x));
		for (int y = 0; y < height; ++y)
			yg.emplace_back(static_cast<double>(y));

		radioFWHM->setChecked(true);

		customPlot->xAxis->setLabel("x");	
		customPlot->yAxis->setLabel("y"); customPlot->yAxis->setRangeReversed(true);

		colorScale = new QCPColorScale(customPlot);

		colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);

		gradient = new QCPColorGradient(QCPColorGradient::gpSpectrum);

		marginGroup = new QCPMarginGroup(customPlot);

		colorMap->data()->setSize(width,  height);
		colorMap->data()->setRange(QCPRange(0, width-1), QCPRange(0, height-1));

		gradient->setNanHandling(QCPColorGradient::nhTransparent);
		gradient->setLevelCount(10);
		//gradient->loadPreset(QCPColorGradient::gpSpectrum);
		colorMap->setGradient(*gradient);

		customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
		colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (default)
		colorScale->setDataRange(colorMap->dataRange());
		colorMap->setColorScale(colorScale); // associate the color map with the color scale

		// make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
		customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
		colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

		connectSignalsToSlots();

		//
		// Fake up a click on the FWHM button to initialize the chart
		//
		QMetaObject::invokeMethod(radioFWHM, "clicked", Qt::ConnectionType::QueuedConnection,
			Q_ARG(bool, true));
	}

	void QualityChart::connectSignalsToSlots()
	{
		connect(radioCircularity, &QRadioButton::clicked,
			this, &QualityChart::circularityButtonClicked);
		connect(radioFWHM, &QRadioButton::clicked,
			this, &QualityChart::fwhmButtonClicked);
	}

	//
	// Slots
	//
	void QualityChart::fwhmButtonClicked(bool checked)
	{
		ZFUNCTRACE_RUNTIME();
		if (checked)
		{
			//
			// Clear the color map data
			// 
			colorMap->data()->fill(std::numeric_limits<double>::quiet_NaN());
			ZTRACE_RUNTIME("FWHM interpolation");
			//GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_CSA);
			GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_NNIDW, 10.f);
			//GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_NNLI, 1.001f);
			//GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_NNAIDW);
			//GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_DTLI);
			//GridData::interpolate(xValues, yValues, fwhmValues, xg, yg, zg, GridData::InterpolationType::GRID_NNI, -std::numeric_limits<float>::max());
			ZTRACE_RUNTIME("FWHM interpolation complete");

			auto p = std::minmax_element(fwhmValues.cbegin(), fwhmValues.cend());
			qDebug() << "FWHM Min:" << *p.first << " Max:" << *p.second;
			p = std::minmax_element(zg.cbegin(), zg.cend());
			qDebug() << "zg Min:" << *p.first << "zg Max:" << *p.second;
			colorMap->setDataRange(QCPRange(*p.first, *p.second));

			colorScale->axis()->setLabel("FWHM");

			//
			// Update the color map with FWHM values from the interpolated grid
			//
			for (size_t i = 0; i < xg.size(); ++i)
			{
				for (size_t j = 0; j < yg.size(); ++j)
				{
					colorMap->data()->setCell(static_cast<int>(xg[i]), static_cast<int>(yg[j]), zg[i + j * xg.size()]);
				}
			}

			colorMap->rescaleAxes();
			customPlot->replot();
		}
	}

	void QualityChart::circularityButtonClicked(bool checked)
	{
		if (checked)
		{
			//
			// Clear the color map data
			// 
			colorMap->data()->fill(0.0);

			auto p = std::minmax_element(circularityValues.cbegin(), circularityValues.cend());
			qDebug() << "Circularity Min:" << *p.first << " Max:" << *p.second;
			colorMap->setDataRange(QCPRange(*p.first, *p.second));
			colorScale->axis()->setLabel("Circularity");

			//
			// Update the color map with circularity values
			//
			for (uint32_t i = 0; i < lightFrameInfo.m_vStars.size(); ++i)
			{
				colorMap->data()->setCell(static_cast<int>(xValues[i]), static_cast<int>(yValues[i]), circularityValues[i]);
			}

			customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
			colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
			colorMap->setColorScale(colorScale); // associate the color map with the color scale
			colorMap->setGradient(QCPColorGradient::gpGrayscale);

			colorMap->rescaleAxes();
			customPlot->replot();
		}
	}
}