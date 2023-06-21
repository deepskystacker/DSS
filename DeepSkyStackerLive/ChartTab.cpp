#include "stdafx.h"
#include "ChartTab.h"
#include <QValueAxis>

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
		setupUi(this);
		theme = (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
			? QChart::ChartTheme::ChartThemeBlueCerulean : QChart::ChartThemeBrownSand;

		scoreChart->setTheme(theme);
		fwhmChart->setTheme(theme);
		starsChart->setTheme(theme);
		offsetChart->setTheme(theme);
		angleChart->setTheme(theme);
		skybgChart->setTheme(theme);

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
		for (int i = 0; i < files.size(); i++)
		{
			if (files[i] == name)
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
					conf[QXYSeries::PointConfiguration::Color] = QColorConstants::Red;;
					conf[QXYSeries::PointConfiguration::Size] = 6;
					break;
				case PT_WARNING:
					conf[QXYSeries::PointConfiguration::Color] = QColor(255, 190, 75);
					conf[QXYSeries::PointConfiguration::Size] = 6;
					break;
				default:
					break;
				}

				switch (chartType)
				{
				case CT_SCORE:
					scoreSeries->setPointConfiguration(i, conf);
					break;
				case CT_FWHM:
					fwhmSeries->setPointConfiguration(i, conf);
					break;
				case CT_STARS:
					starsSeries->setPointConfiguration(i, conf);
					break;
				case CT_DX:
					dXSeries->setPointConfiguration(i, conf);
					break;
				case CT_DY:
					dYSeries->setPointConfiguration(i, conf);
					break;
				case CT_ANGLE:
					angleSeries->setPointConfiguration(i, conf);
					break;
				case CT_SKYBACKGROUND:
					skybgSeries->setPointConfiguration(i, conf);
					break;
				}

				break; // the necessary point has been updated
			}
		}
	}

	/* ------------------------------------------------------------------- */

	void ChartTab::addScoreFWHMStars(const QString& name, double fScore, double fFWHM, double fStars, double fSkyBackground)
	{
		QList<QAbstractAxis*> axes;

		// 
		// Add the file to the list of known files.
		//
		files.emplace_back(name);
		auto x{ static_cast<qreal>(files.size()) };

		//
		// Given the number of X values, choose a suitable interval for X axis tick marks
		//
		int size = static_cast<int>(files.size());
		qreal interval{ 10'000.0 };
		if (size <= 20) interval = 1.0;
		else if (size <= 100) interval = 5.0;
		else if (size <= 200) interval = 10.0;
		else if (size <= 1'000) interval = 50.0;
		else if (size <= 2'000) interval = 100.0;
		else if (size <= 10'000) interval = 500.0;
		else if (size <= 20'000) interval = 1'000.0;

		QHash<QXYSeries::PointConfiguration, QVariant> conf;
		conf[QXYSeries::PointConfiguration::Color] = QColor(Qt::green);
		conf[QXYSeries::PointConfiguration::Size] = 4;

		//
		// Add the score and update the score chart
		//
		if (nullptr != scoreSeries)
			scoreChart->removeSeries(scoreSeries);
		else
		{
			scoreSeries = new QLineSeries(this);
			scoreSeries->setName(tr("Score", "IDC_SCORE"));
			scoreSeries->setPointsVisible(true);
			connect(scoreSeries, &QLineSeries::hovered,
				this, &ChartTab::scoreHovered);
		}

		scoreSeries->append(x, fScore);
		scoreSeries->setPointConfiguration(static_cast<int>(x) - 1, conf);
		scoreChart->addSeries(scoreSeries);

		scoreChart->createDefaultAxes();
		axes = scoreChart->axes(Qt::Horizontal);
		for (const auto& p : axes)
		{
			QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
			if (axis)
			{
				axis->setRange(1.0, size);
				axis->setTickAnchor(1.0);
				axis->setTickType(QValueAxis::TicksDynamic);
				axis->setTickInterval(interval);
			}
		}

		//
		// Add the FWHM and update the FWHM chart
		//
		if (nullptr != fwhmSeries)
			fwhmChart->removeSeries(fwhmSeries);
		else
		{
			fwhmSeries = new QLineSeries(this);
			fwhmSeries->setName(tr("FWHM", "IDC_FWHM"));
			fwhmSeries->setPointsVisible(true);
		}

		fwhmSeries->append(x, fFWHM);
		fwhmSeries->setPointConfiguration(static_cast<int>(x) - 1, conf);
		fwhmChart->addSeries(fwhmSeries);

		fwhmChart->createDefaultAxes();
		axes = fwhmChart->axes(Qt::Horizontal);
		for (const auto& p : axes)
		{
			QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
			if (axis)
			{
				axis->setRange(1.0, size);
				axis->setTickAnchor(1.0);
				axis->setTickType(QValueAxis::TicksDynamic);
				axis->setTickInterval(interval);
			}
		}

		//
		// Add the star count and update the stars chart
		//
		if (nullptr != starsSeries)
			starsChart->removeSeries(starsSeries);
		else
		{
			starsSeries = new QLineSeries(this);
			starsSeries->setName(tr("#Stars", "IDC_STARS"));
			starsSeries->setPointsVisible(true);
		}

		starsSeries->append(x, fStars);
		starsSeries->setPointConfiguration(static_cast<int>(x) - 1, conf);
		starsChart->addSeries(starsSeries);

		starsChart->createDefaultAxes();
		axes = starsChart->axes(Qt::Horizontal);
		for (const auto& p : axes)
		{
			QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
			if (axis)
			{
				axis->setRange(1.0, size);
				axis->setTickAnchor(1.0);
				axis->setTickType(QValueAxis::TicksDynamic);
				axis->setTickInterval(interval);
			}
		}

		//
		// Add the sky background and update the sky background chart
		//
		if (nullptr != skybgSeries)
			skybgChart->removeSeries(skybgSeries);
		else
		{
			skybgSeries = new QLineSeries(this);
			skybgSeries->setName(tr("Sky Background", "IDC_SKYBACKGROUND"));
			skybgSeries->setPointsVisible(true);
		}

		skybgSeries->append(x, fSkyBackground);
		skybgSeries->setPointConfiguration(static_cast<int>(x) - 1, conf);
		skybgChart->addSeries(skybgSeries);

		skybgChart->createDefaultAxes();
		axes = skybgChart->axes(Qt::Horizontal);
		for (const auto& p : axes)
		{
			QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
			if (axis)
			{
				axis->setRange(1.0, size);
				axis->setTickAnchor(1.0);
				axis->setTickType(QValueAxis::TicksDynamic);
				axis->setTickInterval(interval);
			}
		}
	}

	void ChartTab::addOffsetAngle(const QString& name, double fdX, double fdY, double fAngle)
	{
		QList<QAbstractAxis*> axes;

		//
		// Given the number of X values, choose a suitable interval for X axis tick marks
		//
		int size = static_cast<int>(files.size());
		qreal interval{ 10'000.0 };
		if (size <= 20) interval = 1.0;
		else if (size <= 100) interval = 5.0;
		else if (size <= 200) interval = 10.0;
		else if (size <= 1'000) interval = 50.0;
		else if (size <= 2'000) interval = 100.0;
		else if (size <= 10'000) interval = 500.0;
		else if (size <= 20'000) interval = 1'000.0;

		QHash<QXYSeries::PointConfiguration, QVariant> conf;
		conf[QXYSeries::PointConfiguration::Color] = QColor(Qt::green);
		conf[QXYSeries::PointConfiguration::Size] = 4;

		for (int x = 0; x < files.size(); x++)
		{
			if (files[x] == name)
			{
				//
				// Add dX value 
				//
				if (nullptr != dXSeries)
					offsetChart->removeSeries(dXSeries);
				else
				{
					dXSeries = new QLineSeries(this);
					dXSeries->setColor(Qt::red);
					dXSeries->setName("dX");
					dXSeries->setPointsVisible(true);
				}

				dXSeries->append(1.0 + x, fdX);
				dXSeries->setPointConfiguration(x, conf);
				offsetChart->addSeries(dXSeries);

				//
				// Add the dY value
				//
				if (nullptr != dYSeries)
					offsetChart->removeSeries(dYSeries);
				else
				{
					dYSeries = new QLineSeries(this);
					dYSeries->setColor(Qt::blue);
					dYSeries->setName("dY");
					dYSeries->setPointsVisible(true);
				}

				dYSeries->append(1.0 + x, fdY);
				dYSeries->setPointConfiguration(x, conf);
				offsetChart->addSeries(dYSeries);

				//
				// Update the offset chart
				//
				offsetChart->createDefaultAxes();

				axes = offsetChart->axes(Qt::Vertical);
				for (const auto& p : axes)
				{
					QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
					if (axis)
						axis->applyNiceNumbers();
				}

				axes = offsetChart->axes(Qt::Horizontal);
				for (const auto& p : axes)
				{
					QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
					if (axis)
					{
						axis->setRange(1.0, size);
						axis->setTickAnchor(1.0);
						axis->setTickType(QValueAxis::TicksDynamic);
						axis->setTickInterval(interval);
					}
				}

				//
				// Add angle value 
				//
				if (nullptr != angleSeries)
					angleChart->removeSeries(angleSeries);
				else
				{
					angleSeries = new QLineSeries(this);
					angleSeries->setName("Angle");
					angleSeries->setPointsVisible(true);
				}

				angleSeries->append(1.0 + x, fAngle);
				angleSeries->setPointConfiguration(x, conf);
				angleChart->addSeries(angleSeries);

				//
				// Update the angle chart
				//
				angleChart->createDefaultAxes();

				axes = offsetChart->axes(Qt::Vertical);
				for (const auto& p : axes)
				{
					QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
					if (axis)
						axis->applyNiceNumbers();
				}

				axes = angleChart->axes(Qt::Horizontal);
				for (const auto& p : axes)
				{
					QValueAxis* axis{ dynamic_cast<QValueAxis*>(p) };
					if (axis)
					{
						axis->setRange(1.0, size);
						axis->setTickAnchor(1.0);
						axis->setTickType(QValueAxis::TicksDynamic);
						axis->setTickInterval(interval);
					}
				}


				break;
			}
		}
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
		case II_DONTSTACK_NONE:
			setPoint(name, PT_OK, CT_SCORE);
			setPoint(name, PT_OK, CT_FWHM);
			setPoint(name, PT_OK, CT_STARS);
			setPoint(name, PT_OK, CT_DX);
			setPoint(name, PT_OK, CT_DY);
			setPoint(name, PT_OK, CT_ANGLE);
			setPoint(name, PT_OK, CT_SKYBACKGROUND);
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
