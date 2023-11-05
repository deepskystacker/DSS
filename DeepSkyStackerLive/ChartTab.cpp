#include "stdafx.h"
#include "ChartTab.h"
#include <QValueAxis>
#include "dssliveenums.h"

namespace DSS
{
	ChartTab::ChartTab(QWidget* parent)
		: QWidget(parent),
		scoreSeries{nullptr},
		scoreChart{new QChart()},
		fwhmSeries{nullptr},
		fwhmChart{new QChart()},
		starsSeries{nullptr},
		starsChart{new QChart()},
		dXSeries{nullptr},
		dYSeries{nullptr},
		offsetChart{new QChart()},
		angleSeries{nullptr},
		angleChart{new QChart()},
		skybgSeries{nullptr},
		skybgChart{new QChart()}
	{
		QValueAxis* xAxis;
		QValueAxis* yAxis;

		setupUi(this);
		theme = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
			? QChart::ChartTheme::ChartThemeBlueCerulean : QChart::ChartThemeBrownSand;

		scoreChart->setTheme(theme);
		fwhmChart->setTheme(theme);
		starsChart->setTheme(theme);
		offsetChart->setTheme(theme);
		angleChart->setTheme(theme);
		skybgChart->setTheme(theme);

		scoreSeries = new QLineSeries(this);
		scoreSeries->setName(tr("Score", "IDC_SCORE"));
		scoreSeries->setPointsVisible(true);
		scoreChart->addSeries(scoreSeries);
		connect(scoreSeries, &QLineSeries::hovered,
			this, &ChartTab::scoreHovered);

		fwhmSeries = new QLineSeries(this);
		fwhmSeries->setName(tr("FWHM", "IDC_FWHM"));
		fwhmSeries->setPointsVisible(true);
		fwhmChart->addSeries(fwhmSeries);
		connect(fwhmSeries, &QLineSeries::hovered,
			this, &ChartTab::fwhmHovered);

		starsSeries = new QLineSeries(this);
		starsSeries->setName(tr("#Stars", "IDC_STARS"));
		starsSeries->setPointsVisible(true);
		starsChart->addSeries(starsSeries);
		connect(starsSeries, &QLineSeries::hovered,
			this, &ChartTab::starsHovered);


		dXSeries = new QLineSeries(this);
		dXSeries->setColor(Qt::red);
		dXSeries->setName("dX");
		dXSeries->setPointsVisible(true);
		offsetChart->addSeries(dXSeries);
		connect(dXSeries, &QLineSeries::hovered,
			this, &ChartTab::dXHovered);

		dYSeries = new QLineSeries(this);
		dYSeries->setColor(Qt::blue);
		dYSeries->setName("dY");
		dYSeries->setPointsVisible(true);
		offsetChart->addSeries(dYSeries);
		connect(dYSeries, &QLineSeries::hovered,
			this, &ChartTab::dYHovered);

		angleSeries = new QLineSeries(this);
		angleSeries->setName(tr("Angle", "IDC_ANGLE"));
		angleSeries->setPointsVisible(true);
		angleChart->addSeries(angleSeries);
		connect(angleSeries, &QLineSeries::hovered,
			this, &ChartTab::angleHovered);


		skybgSeries = new QLineSeries(this);
		skybgSeries->setName(tr("Sky Background", "IDC_SKYBACKGROUND"));
		skybgSeries->setPointsVisible(true);
		skybgChart->addSeries(skybgSeries);
		connect(skybgSeries, &QLineSeries::hovered,
			this, &ChartTab::skybgHovered);

		//
		// Create default axes for all charts
		//
		scoreChart->createDefaultAxes();
		fwhmChart->createDefaultAxes();
		starsChart->createDefaultAxes();
		offsetChart->createDefaultAxes();
		angleChart->createDefaultAxes();
		skybgChart->createDefaultAxes();

		xAxis = axisX(scoreChart);
		yAxis = axisY(scoreChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(0, 1000);

		xAxis = axisX(fwhmChart);
		yAxis = axisY(fwhmChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(5, 7);

		xAxis = axisX(starsChart);
		yAxis = axisY(starsChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(0, 250);

		xAxis = axisX(offsetChart);
		yAxis = axisY(offsetChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(-1.0, 1.0);

		xAxis = axisX(angleChart);
		yAxis = axisY(angleChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(-0.005, 0.005);

		xAxis = axisX(skybgChart);
		yAxis = axisY(skybgChart);
		xAxis->setLabelFormat("%d");
		xAxis->setTickType(QValueAxis::TicksDynamic);
		yAxis->setRange(0, 1.0);

		radioScore->setChecked(true);
		chartView->setChart(scoreChart);
		connectSignalsToSlots();
	}

	ChartTab::~ChartTab()
	{
		delete scoreSeries;
		delete scoreChart;
		delete fwhmSeries;
		delete fwhmChart;
		delete starsSeries;
		delete starsChart;
		delete dXSeries;
		delete dYSeries;
		delete offsetChart;
		delete angleSeries;
		delete angleChart;
		delete skybgSeries;
		delete skybgChart;
	}

	void ChartTab::connectSignalsToSlots()
	{
		connect(radioScore, &QRadioButton::clicked,
			this, &ChartTab::scoreButtonClicked);
		connect(radioFWHM, &QRadioButton::clicked,
			this, &ChartTab::fwhmButtonClicked);
		connect(radioStars, &QRadioButton::clicked,
			this, &ChartTab::starsButtonClicked);
		connect(radioOffset, &QRadioButton::clicked,
			this, &ChartTab::offsetButtonClicked);
		connect(radioAngle, &QRadioButton::clicked,
			this, &ChartTab::angleButtonClicked);
		connect(radioBackground, &QRadioButton::clicked,
			this, &ChartTab::skybgButtonClicked);
		connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
			this, &ChartTab::colorSchemeChanged);
	}


	void ChartTab::setPoint([[maybe_unused]] const QString& name, [[maybe_unused]] POINTTYPE pointType, [[maybe_unused]] CHARTTYPE chartType)
	{
		//
		// i is the index into the list of files
		//
		// Set up for the "OK" condition
		//
		QHash<QXYSeries::PointConfiguration, QVariant> conf;
		conf[QXYSeries::PointConfiguration::Color] = QColorConstants::Green;
		conf[QXYSeries::PointConfiguration::Size] = 4;
		switch (pointType)
		{
		case PT_REFERENCE:
			conf[QXYSeries::PointConfiguration::Color] = QColorConstants::Cyan;
			conf[QXYSeries::PointConfiguration::Size] = 6;
			break;
		case PT_WRONG:
			conf[QXYSeries::PointConfiguration::Color] = QColorConstants::Red;
			conf[QXYSeries::PointConfiguration::Size] = 6;
			break;
		case PT_WARNING:
			conf[QXYSeries::PointConfiguration::Color] = QColorConstants::Svg::orange;
			conf[QXYSeries::PointConfiguration::Size] = 6;
			break;
		default:
			break;
		}

		switch (chartType)
		{
		case CT_SCORE:
			if (scoreSeries)
			{
				auto iter = scoreMap.find(name);
				if (iter != scoreMap.end())
				{
					auto i = iter->second;
					scoreSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_FWHM:
			if (fwhmSeries)
			{
				auto iter = fwhmMap.find(name);
				if (iter != fwhmMap.end())
				{
					auto i = iter->second;
					fwhmSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_STARS:
			if (starsSeries)
			{
				auto iter = starsMap.find(name);
				if (iter != starsMap.end())
				{
					auto i = iter->second;
					starsSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_DX:
			if (dXSeries)
			{
				auto iter = offsetMap.find(name);
				if (iter != offsetMap.end())
				{
					auto i = iter->second;
					dXSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_DY:
			if (dYSeries)
			{
				auto iter = offsetMap.find(name);
				if (iter != offsetMap.end())
				{
					auto i = iter->second;
					dYSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_ANGLE:
			if (angleSeries)
			{
				auto iter = angleMap.find(name);
				if (iter != angleMap.end())
				{
					auto i = iter->second;
					angleSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		case CT_SKYBACKGROUND:
			if (skybgSeries)
			{
				auto iter = skybgMap.find(name);
				if (iter != skybgMap.end())
				{
					auto i = iter->second;
					skybgSeries->setPointConfiguration(i, conf);
				}
			}
			break;
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::addScoreFWHMStars(const QString& name, double fScore, double fFWHM, double fStars, double fSkyBackground)
	{
		QValueAxis* xAxis;
		QValueAxis* yAxis;

		// 
		// Add the file to the list of known files, and remember the index at which it was added
		//
		files.emplace_back(name);
		auto size = static_cast<int>(files.size());
		nameMap.emplace(name, size - 1);

		auto x{ static_cast<qreal>(size) };

		//
		// Given the number of X values, choose a suitable interval and range 
		// for X axis tick marks
		//
		qreal interval{ 10'000.0 };
		qreal rangeMax{ 200'000.0 };

		if (size <= 20)
		{
			interval = 1.0;
			rangeMax = 20;
		}
		else if (size <= 100)
		{
			interval = 5.0;
			rangeMax = 100;
		}
		else if (size <= 200)
		{
			interval = 10.0;
			rangeMax = 200;
		}
		else if (size <= 1'000)
		{
			interval = 50.0;
			rangeMax = 1'000;
		}
		else if (size <= 2'000)
		{
			interval = 100.0;
			rangeMax = 2'000;
		}
		else if (size <= 10'000)
		{
			interval = 500.0;
			rangeMax = 10'000;
		}
		else if (size <= 20'000)
		{
			interval = 1'000.0;
			rangeMax = 20'000;
		}

		//
		// Adjust score axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(scoreChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(scoreChart);
		if (fScore > yAxis->max())
			yAxis->setMax(fScore > 0 ? fScore * 1.1 : fScore * 0.9);
		if (fScore < yAxis->min())
			yAxis->setMin(fScore > 0 ? fScore * 0.9 : fScore * 1.1);

		//
		// Add the score and update the score chart
		//
		scoreSeries->append(x, fScore);
		scoreMap.emplace(name, size - 1);
		yAxis->applyNiceNumbers();

		//
		// Adjust FWHM axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(fwhmChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(fwhmChart);
		if (fFWHM > yAxis->max())
			yAxis->setMax(fFWHM > 0 ? fFWHM * 1.1 : fFWHM * 0.9);
		if (fFWHM < yAxis->min())
			yAxis->setMin(fFWHM > 0 ? fFWHM * 0.9 : fFWHM * 1.1);

		//
		// Add the FWHM and update the FWHM chart
		//
		fwhmSeries->append(x, fFWHM);
		fwhmMap.emplace(name, size - 1);
		yAxis->applyNiceNumbers();

		//
		// Adjust stars axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(starsChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(starsChart);
		if (fStars > yAxis->max())
			yAxis->setMax(fStars > 0 ? fStars * 1.1 : fStars * 0.9);
		if (fStars < yAxis->min())
			yAxis->setMin(fStars > 0 ? fStars * 0.9 : fStars * 1.1);

		//
		// Add the star count and update the stars chart
		//
		starsSeries->append(x, fStars);
		starsMap.emplace(name, size - 1);
		yAxis->applyNiceNumbers();

		//
		// Adjust skybg axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(skybgChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(skybgChart);
		if (fSkyBackground > yAxis->max())
			yAxis->setMax(fSkyBackground > 0 ? fSkyBackground * 1.1 : fSkyBackground * 0.9);
		if (fSkyBackground < yAxis->min())
			yAxis->setMin(fSkyBackground > 0 ? fSkyBackground * 0.9 : fSkyBackground * 1.1);

		//
		// Add the sky background and update the sky background chart
		//
		skybgSeries->append(x, fSkyBackground);
		skybgMap.emplace(name, size - 1);
		yAxis->applyNiceNumbers();
	}

	void ChartTab::addOffsetAngle(const QString& name, double fdX, double fdY, double fAngle)
	{
		QValueAxis* xAxis;
		QValueAxis* yAxis;

		//
		// Given the number of X values, choose a suitable interval for X axis tick marks
		//
		auto size = static_cast<int>(files.size());
		//
		// Given the number of X values, choose a suitable interval and range 
		// for X axis tick marks
		//
		qreal interval{ 10'000.0 };
		qreal rangeMax{ 200'000.0 };

		if (size <= 20)
		{
			interval = 1.0;
			rangeMax = 20;
		}
		else if (size <= 100)
		{
			interval = 5.0;
			rangeMax = 100;
		}
		else if (size <= 200)
		{
			interval = 10.0;
			rangeMax = 200;
		}
		else if (size <= 1'000)
		{
			interval = 50.0;
			rangeMax = 1'000;
		}
		else if (size <= 2'000)
		{
			interval = 100.0;
			rangeMax = 2'000;
		}
		else if (size <= 10'000)
		{
			interval = 500.0;
			rangeMax = 10'000;
		}
		else if (size <= 20'000)
		{
			interval = 1'000.0;
			rangeMax = 20'000;
		}


		// find the index into the vector of file names for this name
		auto iter = nameMap.find(name);
		ZASSERT(iter != nameMap.end());
		size_t x = iter->second;
		ZASSERT(files[x] == name);

		//
		// Adjust offset axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(offsetChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(offsetChart);
		if (fdX > yAxis->max())
			yAxis->setMax(fdX > 0 ? fdX * 1.1 : fdX * 0.9);
		if (fdX < yAxis->min())
			yAxis->setMin(fdX > 0 ? fdX * 0.9 : fdX * 1.1);

		if (fdY > yAxis->max())
			yAxis->setMax(fdY > 0 ? fdY * 1.1 : fdY * 0.9);
		if (fdY < yAxis->min())
			yAxis->setMin(fdY > 0 ? fdY * 0.9 : fdY * 1.1);

		//
		// Add dX and dY values and update the offset chart
		//
		dXSeries->append(1.0 + x, fdX);
		dYSeries->append(1.0 + x, fdY);
		// Remember where in the series the point for this name was inserted
		offsetMap.emplace(name, dXSeries->count() - 1);
		yAxis->applyNiceNumbers();

		//
		// Adjust angle axes if necessary (data won't display if we don't)
		//
		xAxis = axisX(angleChart);
		xAxis->setRange(1.0, rangeMax);
		xAxis->setTickAnchor(1.0);
		xAxis->setTickInterval(interval);

		yAxis = axisY(angleChart);
		if (fAngle > yAxis->max())
			yAxis->setMax(fAngle > 0 ? fAngle * 1.1 : fAngle * 0.9);
		if (fAngle < yAxis->min())
			yAxis->setMin(fAngle > 0 ? fAngle * 0.9 : fAngle * 1.1);

		//
		// Add angle value 
		//
		angleSeries->append(1.0 + x, fAngle);
		// Remember where in the series the point for this name was inserted
		angleMap.emplace(name, angleSeries->count() - 1);
		yAxis->applyNiceNumbers();
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
			setPoint(name, PT_OK, CT_SCORE);
			setPoint(name, PT_OK, CT_FWHM);
			setPoint(name, PT_OK, CT_STARS);
			setPoint(name, PT_OK, CT_SKYBACKGROUND);
			break;
		case II_DONTSTACK_STACK:
			setPoint(name, PT_OK, CT_DX);
			setPoint(name, PT_OK, CT_DY);
			setPoint(name, PT_OK, CT_ANGLE);
			break;
		case II_DONTSTACK_SCORE:
			setPoint(name, PT_WRONG, CT_SCORE);
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
		case II_WARNING_SCORE:
			setPoint(name, PT_WARNING, CT_SCORE);
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
		};
	}

	void ChartTab::clear()
	{
		files.clear();
		nameMap.clear();

		scoreSeries->clear();

		fwhmSeries->clear();

		starsSeries->clear();

		dXSeries->clear();
		dYSeries->clear();

		angleSeries->clear();

		skybgSeries->clear();
	}
	
	/* ------------------------------------------------------------------- */

	void ChartTab::scoreButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(scoreChart);
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::fwhmButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(fwhmChart);
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::starsButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(starsChart);
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::offsetButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(offsetChart);
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::angleButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(angleChart);
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::skybgButtonClicked(bool checked)
	{
		if (checked)
			chartView->setChart(skybgChart);
	}

	void ChartTab::scoreHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::fwhmHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::starsHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::dXHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::dYHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::angleHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::skybgHovered(const QPointF& point, bool state)
	{
		if (state)
		{
			const QPoint globalMouseLocation(QCursor::pos());
			QLocale locale;
			QToolTip::showText(globalMouseLocation, locale.toString(point.y(), 'f', 2), this);
		}
	}

	void ChartTab::colorSchemeChanged(Qt::ColorScheme colorScheme)
	{
		theme = (colorScheme == Qt::ColorScheme::Dark)
			? QChart::ChartTheme::ChartThemeBlueCerulean : QChart::ChartThemeBrownSand;

		scoreChart->setTheme(theme);
		fwhmChart->setTheme(theme);
		starsChart->setTheme(theme);
		offsetChart->setTheme(theme);
		angleChart->setTheme(theme);
		skybgChart->setTheme(theme);
	}

}
