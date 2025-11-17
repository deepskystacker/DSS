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
		for (uint32_t i = 0; i < lightFrameInfo.m_vStars.size(); ++i)
		{
			const CStar& star = lightFrameInfo.m_vStars[i];
			xValues.emplace_back(static_cast<uint32_t>(star.m_fX));
			yValues.emplace_back(static_cast<uint32_t>(star.m_fY));
			fwhmValues.emplace_back((star.m_fMeanRadius / CRegisteredFrame::RadiusFactor));
			circularityValues.emplace_back(star.m_fCircularity);
		}

		radioFWHM->setChecked(true);

		customPlot->xAxis->setLabel("x");
		customPlot->yAxis->setLabel("y");

		colorScale = new QCPColorScale(customPlot);

		colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);

		colorMap->data()->setSize(static_cast<int>(lightFrameInfo.m_lWidth), static_cast<int>(lightFrameInfo.m_lHeight));
		colorMap->data()->setRange(QCPRange(0, static_cast<double>(lightFrameInfo.m_lWidth)),
			QCPRange(0, static_cast<double>(lightFrameInfo.m_lHeight)));

		connectSignalsToSlots();

		fwhmButtonClicked(true);		// Display FWHM by default

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
		if (checked)
		{
			//
			// Clear the color map data
			// 
			colorMap->data()->fill(0.0);

			auto p = std::minmax_element(fwhmValues.cbegin(), fwhmValues.cend());
			qDebug() << "FWHM Min:" << *p.first << " Max:" << *p.second;
			colorMap->setDataRange(QCPRange(*p.first, *p.second));
			colorScale->axis()->setLabel("FWHM");

			//
			// Update the color map with FWHM values
			//
			for (uint32_t i = 0; i < lightFrameInfo.m_vStars.size(); ++i)
			{
				colorMap->data()->setCell(static_cast<int>(xValues[i]), static_cast<int>(yValues[i]), fwhmValues[i]);
			}

			customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
			colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
			colorMap->setColorScale(colorScale); // associate the color map with the color scale
			colorMap->setGradient(QCPColorGradient::gpGrayscale);

			// make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
			QCPMarginGroup* marginGroup = new QCPMarginGroup(customPlot);
			customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
			colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

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

			// make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
			QCPMarginGroup* marginGroup = new QCPMarginGroup(customPlot);
			customPlot->axisRect()->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);
			colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

			colorMap->rescaleAxes();
			customPlot->replot();
		}
	}
}