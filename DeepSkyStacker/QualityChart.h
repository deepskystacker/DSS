#pragma once
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
// QualityChart.h : header file
//
#include <QWidget>
#include "ui_QualityChart.h"
#include "RegisterEngine.h"
class ListBitMap;
namespace DSS
{
	class QualityChart final: public QDialog, public Ui::QualityChart
	{
		Q_OBJECT
	public:
		QualityChart(const ListBitMap& lbmp, QWidget* parent = nullptr);
		QualityChart(const QualityChart&) = delete;
		QualityChart& operator=(const QualityChart&) = delete;
		QualityChart(QualityChart&&) = delete;

		~QualityChart()
		{
			if (colorMap)
				customPlot->removePlottable(colorMap);
		};

	private:
		CLightFrameInfo lightFrameInfo;

	private:
		std::vector<std::uint32_t> xValues;
		std::vector<std::uint32_t> yValues;
		std::vector<double> fwhmValues;
		std::vector<double> circularityValues;

		QCPColorMap* colorMap{ nullptr };
		QCPColorScale* colorScale{ nullptr };

		void connectSignalsToSlots();
	
	private slots:
		void fwhmButtonClicked(bool checked);
		void circularityButtonClicked(bool checked);


	};
}