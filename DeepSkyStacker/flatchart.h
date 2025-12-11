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
// FlatChart.h : header file
//
#include <QWidget>
#include "ui_FlatChart.h"
#include "griddata.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "RegisterEngine.h"
class QwtPlotRescaler;
class QwtPlotSpectrogram;
class QwtMatrixRasterData;
class QwtLinearColorMap;

namespace DSS
{
	class FlatChart final: public QDialog, public Ui::FlatChart
	{
		Q_OBJECT
	public:
		FlatChart(const ListBitMap& lbmp, QWidget* parent = nullptr);
		FlatChart(const FlatChart&) = delete;
		FlatChart& operator=(const FlatChart&) = delete;
		FlatChart(FlatChart&&) = delete;

		~FlatChart()
		{
		};

	private:
		size_t width{ 0 };
		std::unique_ptr<GridData> gridData;
		CAllDepthBitmap bitmap;

		QVector<double>valueData;

		QwtPlotSpectrogram* spectrogram{ nullptr };
		QwtMatrixRasterData* rasterData{ nullptr };
		void connectSignalsToSlots();

		void plotFlat();

	private slots:
		void cancelPressed();
	};
}