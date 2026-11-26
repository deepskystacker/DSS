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
#include <map>
#include <vector>

#include <QWidget>
#include "ui_ChartTab.h"
#include "ChartPage.h"

namespace DSS
{
	class ChartTab : public QWidget, public Ui::ChartTab
	{
		Q_OBJECT

	private:
		std::vector<QString> files_;
		std::map<QString, int> nameMap_;

		// One page per metric (the offset page holds both dX and dY).
		// They are children of the stacked widget, which owns them.
		ChartPage* qualityPage_;
		ChartPage* fwhmPage_;
		ChartPage* starsPage_;
		ChartPage* offsetPage_;
		ChartPage* anglePage_;
		ChartPage* skybgPage_;

		void connectSignalsToSlots();
		void showPageWhenChecked(QRadioButton* button, ChartPage* page);
		std::array<ChartPage*, 6> pages() const;

	private slots:
		void colorSchemeChanged(Qt::ColorScheme colorScheme);

	public slots:
		void setImageInfo(QString name, STACKIMAGEINFO info);
		void clear();

	public:
		ChartTab(QWidget* parent = nullptr);
		~ChartTab() override = default;
		void addQualityFWHMStars(const QString& name, double fQuality, double fFWHM, double fStars, double fSkyBackground);
		void addOffsetAngle(const QString& name, double fdX, double fdY, double fAngle);
		void setPoint(const QString& name, POINTTYPE ptType, CHARTTYPE ctType);
	};
}
