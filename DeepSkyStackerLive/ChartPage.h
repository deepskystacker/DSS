#pragma once
/****************************************************************************
**
** Copyright (C) 2026 David C. Partridge
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

#include <array>
#include <initializer_list>
#include <map>
#include <vector>

#include <QColor>
#include <QPointF>
#include <QString>
#include <QVector>
#include <QwtPlot>

class QwtPlotCurve;
class QwtPlotGrid;

namespace DSS
{
	enum POINTTYPE
	{
		PT_REFERENCE = 1,
		PT_OK = 2,
		PT_WRONG = 3,
		PT_WARNING = 4
	};

	enum CHARTTYPE
	{
		CT_QUALITY = 1,
		CT_FWHM = 2,
		CT_STARS = 3,
		CT_DX = 4,
		CT_DY = 5,
		CT_ANGLE = 6,
		CT_SKYBACKGROUND = 7
	};

	// One chart: a plot holding one or more line series (e.g. dX and dY).
	//
	// QWT has no per-point styling, so each series is drawn as
	//  - a "line" curve holding every point (line + default symbol), and
	//  - one marker-only curve per POINTTYPE, holding just the points that
	//    have been given that type, drawn on top with a larger/coloured symbol.
	class ChartPage : public QwtPlot
	{
	public:
		struct SeriesSpec
		{
			QString name;
			// Invalid colour means "use the theme's line colour"
			QColor colour;
		};

		ChartPage(const std::vector<SeriesSpec>& specs, double yMin, double yMax, QWidget* parent = nullptr);

		// Tell the page how many files there are, so that it can pick
		// a sensible X range and tick interval.
		void setXExtent(int fileCount);
		// Add one point to every series (one y value per series).
		void append(const QString& name, double x, std::initializer_list<double> ys);
		void setPoint(const QString& name, size_t series, POINTTYPE type);
		void clear();
		void setDarkTheme(bool dark);
		// Digits after the decimal point in the hover value (default 2)
		void setValueDecimals(int decimals) { valueDecimals_ = decimals; }

	private:
		static constexpr size_t markerCount{ 4 };

		class PointStore;

		struct Series
		{
			QString name;
			QColor requestedColour;
			QColor colour;
			QwtPlotCurve* line{ nullptr };
			// Owned by 'line'
			PointStore* store{ nullptr };
			std::array<QwtPlotCurve*, markerCount> markers{};
			std::array<QVector<QPointF>, markerCount> markerPoints;
			// Per point: 0 = default, otherwise POINTTYPE
			std::vector<unsigned char> types;
		};

		// Owned by the plot
		QwtPlotGrid* grid_{ nullptr };
		std::vector<Series> series_;
		// File name -> position in the series (shared by all series)
		std::map<QString, int> index_;
		double yMin0_;
		double yMax0_;
		double yMin_;
		double yMax_;
		bool dark_{ false };
		int valueDecimals_{ 2 };

		void applyColours(Series& s);
		void rebuildMarkers(Series& s, size_t marker);
		void growY(double value);
		void applyYScale();
	};
}
