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

#include "pch.h"
#include "ChartTab.h"
#include "dssliveenums.h"

//
// This was reimplemented to use Qwt instead of QtCharts because QtCharts is now deprecated. Qt offers as replacement, which
// is QtGraphs, but that requires QtQuick and is also licensed under the GPL license rather than the LGPL license which means
// we cannot use it without changing our license from the permissive BSD license to the GPL license.
// 
// This used to be a QChartView subclass which owned six QCharts, one per metric.  It has been replaced by a QStackedWidget
// with one ChartPage per metric.
// 
// The public API is unchanged, so the rest of the program does not need to know about the changes.
// 
// Each ChartPage is a QwtPlot subclass that draws one metric and has one line curve per series, plus four marker-only curves
// per series (one per POINTTYPE) drawn on top of the line curve.
// The marker-only curves hold only the points that have been given that type, and are drawn with a larger symbol in a colour
// appropriate to the type.
//

namespace DSS
{
	namespace
	{
		bool isDark(Qt::ColorScheme scheme)
		{
			return scheme == Qt::ColorScheme::Dark;
		}
	}

	ChartTab::ChartTab(QWidget* parent)
		: QWidget(parent)
	{
		setupUi(this);

		const auto makePage = [this](const std::vector<ChartPage::SeriesSpec>& specs, double yMin, double yMax)
		{
			auto* page = new ChartPage(specs, yMin, yMax, chartStack);
			chartStack->addWidget(page);
			return page;
		};

		qualityPage_ = makePage({ { tr("Quality"), QColor() } }, 0, 1000);
		fwhmPage_ = makePage({ { tr("FWHM", "IDC_FWHM"), QColor() } }, 5, 7);
		starsPage_ = makePage({ { tr("#Stars", "IDC_STARS"), QColor() } }, 0, 250);
		offsetPage_ = makePage({ { "dX", Qt::red }, { "dY", Qt::blue } }, -1.0, 1.0);
		anglePage_ = makePage({ { tr("Angle", "IDC_ANGLE"), QColor() } }, -0.005, 0.005);
		// Angles are tiny (radians), so two decimals would show 0.00
		anglePage_->setValueDecimals(5);
		skybgPage_ = makePage({ { tr("Sky Background", "IDC_SKYBACKGROUND"), QColor() } }, 0, 1.0);

		colorSchemeChanged(QGuiApplication::styleHints()->colorScheme());

		radioQuality->setChecked(true);
		chartStack->setCurrentWidget(qualityPage_);
		connectSignalsToSlots();
	}

	std::array<ChartPage*, 6> ChartTab::pages() const
	{
		return { qualityPage_, fwhmPage_, starsPage_, offsetPage_, anglePage_, skybgPage_ };
	}

	void ChartTab::showPageWhenChecked(QRadioButton* button, ChartPage* page)
	{
		connect(button, &QRadioButton::clicked, this, [this, page](bool checked)
			{
				if (checked)
					chartStack->setCurrentWidget(page);
			});
	}

	void ChartTab::connectSignalsToSlots()
	{
		showPageWhenChecked(radioQuality, qualityPage_);
		showPageWhenChecked(radioFWHM, fwhmPage_);
		showPageWhenChecked(radioStars, starsPage_);
		showPageWhenChecked(radioOffset, offsetPage_);
		showPageWhenChecked(radioAngle, anglePage_);
		showPageWhenChecked(radioBackground, skybgPage_);
		connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
			this, &ChartTab::colorSchemeChanged);
	}

	void ChartTab::setPoint(const QString& name, POINTTYPE pointType, CHARTTYPE chartType)
	{
		switch (chartType)
		{
		case CT_QUALITY:
			qualityPage_->setPoint(name, 0, pointType);
			break;
		case CT_FWHM:
			fwhmPage_->setPoint(name, 0, pointType);
			break;
		case CT_STARS:
			starsPage_->setPoint(name, 0, pointType);
			break;
		case CT_DX:
			offsetPage_->setPoint(name, 0, pointType);
			break;
		case CT_DY:
			offsetPage_->setPoint(name, 1, pointType);
			break;
		case CT_ANGLE:
			anglePage_->setPoint(name, 0, pointType);
			break;
		case CT_SKYBACKGROUND:
			skybgPage_->setPoint(name, 0, pointType);
			break;
		default: break;
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::addQualityFWHMStars(const QString& name, double fQuality, double fFWHM, double fStars, double fSkyBackground)
	{
		//
		// Add the file to the list of known files, and remember the index at which it was added
		//
		files_.emplace_back(name);
		const auto size = static_cast<int>(files_.size());
		nameMap_.emplace(name, size - 1);

		const auto x{ static_cast<double>(size) };

		for (ChartPage* page : { qualityPage_, fwhmPage_, starsPage_, skybgPage_ })
			page->setXExtent(size);

		qualityPage_->append(name, x, { fQuality });
		fwhmPage_->append(name, x, { fFWHM });
		starsPage_->append(name, x, { fStars });
		skybgPage_->append(name, x, { fSkyBackground });
	}

	void ChartTab::addOffsetAngle(const QString& name, double fdX, double fdY, double fAngle)
	{
		// find the index into the vector of file names for this name
		const auto iter = nameMap_.find(name);
		ZASSERT(iter != nameMap_.end());
		const auto index = static_cast<size_t>(iter->second);
		ZASSERT(files_[index] == name);

		const auto x = 1.0 + static_cast<double>(index);
		const auto size = static_cast<int>(files_.size());

		offsetPage_->setXExtent(size);
		anglePage_->setXExtent(size);

		offsetPage_->append(name, x, { fdX, fdY });
		anglePage_->append(name, x, { fAngle });
	}

	/* ------------------------------------------------------------------- */
	/* Slots                                                               */
	/* ------------------------------------------------------------------- */

	void ChartTab::setImageInfo(QString name, STACKIMAGEINFO info)
	{
		switch (info)
		{
		case II_SETREFERENCE:
			setPoint(name, PT_REFERENCE, CT_DX);
			setPoint(name, PT_REFERENCE, CT_DY);
			setPoint(name, PT_REFERENCE, CT_ANGLE);
			break;
		case II_DONTSTACK_REGISTER:
			setPoint(name, PT_OK, CT_QUALITY);
			setPoint(name, PT_OK, CT_FWHM);
			setPoint(name, PT_OK, CT_STARS);
			setPoint(name, PT_OK, CT_SKYBACKGROUND);
			break;
		case II_DONTSTACK_STACK:
			setPoint(name, PT_OK, CT_DX);
			setPoint(name, PT_OK, CT_DY);
			setPoint(name, PT_OK, CT_ANGLE);
			break;
		case II_DONTSTACK_QUALITY:
			setPoint(name, PT_WRONG, CT_QUALITY);
			break;
		case II_DONTSTACK_STARS:
			setPoint(name, PT_WRONG, CT_STARS);
			break;
		case II_DONTSTACK_FWHM:
			setPoint(name, PT_WRONG, CT_FWHM);
			break;
		case II_DONTSTACK_DX:
			setPoint(name, PT_WRONG, CT_DX);
			break;
		case II_DONTSTACK_DY:
			setPoint(name, PT_WRONG, CT_DY);
			break;
		case II_DONTSTACK_ANGLE:
			setPoint(name, PT_WRONG, CT_ANGLE);
			break;
		case II_DONTSTACK_SKYBACKGROUND:
			setPoint(name, PT_WRONG, CT_SKYBACKGROUND);
			break;
		case II_WARNING_NONE:
			break;
		case II_WARNING_QUALITY:
			setPoint(name, PT_WARNING, CT_QUALITY);
			break;
		case II_WARNING_STARS:
			setPoint(name, PT_WARNING, CT_STARS);
			break;
		case II_WARNING_FWHM:
			setPoint(name, PT_WARNING, CT_FWHM);
			break;
		case II_WARNING_DX:
			setPoint(name, PT_WARNING, CT_DX);
			break;
		case II_WARNING_DY:
			setPoint(name, PT_WARNING, CT_DY);
			break;
		case II_WARNING_ANGLE:
			setPoint(name, PT_WARNING, CT_ANGLE);
			break;
		case II_WARNING_SKYBACKGROUND:
			setPoint(name, PT_WARNING, CT_SKYBACKGROUND);
			break;
		default: break;
		};
	}

	void ChartTab::clear()
	{
		files_.clear();
		nameMap_.clear();

		for (ChartPage* page : pages())
			page->clear();
	}

	void ChartTab::colorSchemeChanged(Qt::ColorScheme colorScheme)
	{
		for (ChartPage* page : pages())
			page->setDarkTheme(isDark(colorScheme));
	}
}
