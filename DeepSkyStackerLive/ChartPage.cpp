#include "pch.h"
#include "ChartPage.h"

#include <algorithm>
#include <functional>

#include <QwtLegend>
#include <QwtLinearScaleEngine>
#include <QwtPickerTrackerMachine>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtPlotPicker>
#include <QwtSeriesData>
#include <QwtSymbol>
#include <QwtText>

namespace DSS
{
	namespace
	{
		// Pixel distance within which the mouse "hits" a point
		constexpr double hitDistance{ 10.0 };

		struct MarkerStyle
		{
			QColor colour;
			int size;
		};

		// Indexed by POINTTYPE - 1
		const std::array<MarkerStyle, 4> markerStyles{ {
			{ QColorConstants::Cyan, 6 },				// PT_REFERENCE
			{ QColorConstants::Green, 4 },				// PT_OK
			{ QColorConstants::Red, 6 },				// PT_WRONG
			{ QColorConstants::Svg::orange, 6 }			// PT_WARNING
		} };

		// Given the number of files, choose a suitable tick interval and
		// maximum for the X axis.  The axis range grows in steps so that
		// it does not need to be rescaled (and ticks do not jump around)
		// for every new file.
		void xScaleFor(int size, double& interval, double& rangeMax)
		{
			struct Step
			{
				int limit;
				double interval;
				double rangeMax;
			};
			static constexpr std::array<Step, 7> steps{ {
				{ 20, 1.0, 20.0 },
				{ 100, 5.0, 100.0 },
				{ 200, 10.0, 200.0 },
				{ 1'000, 50.0, 1'000.0 },
				{ 2'000, 100.0, 2'000.0 },
				{ 10'000, 500.0, 10'000.0 },
				{ 20'000, 1'000.0, 20'000.0 }
			} };

			interval = 10'000.0;
			rangeMax = 200'000.0;
			for (const auto& step : steps)
			{
				if (size <= step.limit)
				{
					interval = step.interval;
					rangeMax = step.rangeMax;
					break;
				}
			}
		}

		// Shows the Y value of the point nearest the mouse (if close enough)
		class PointTracker : public QwtPlotPicker
		{
		public:
			using Lookup = std::function<QwtText(const QPoint&)>;

			PointTracker(QWidget* canvas, Lookup lookup) :
				QwtPlotPicker(QwtAxis::XBottom, QwtAxis::YLeft, QwtPicker::NoRubberBand, QwtPicker::AlwaysOn, canvas),
				lookup_{ std::move(lookup) }
			{
				setStateMachine(new QwtPickerTrackerMachine());
			}

		protected:
			QwtText trackerText(const QPoint& pos) const override
			{
				return lookup_(pos);
			}

		private:
			Lookup lookup_;
		};
	}

	// Series data that can be appended to cheaply.  QwtPointSeriesData is
	// immutable, so growing it would mean copying the whole vector for
	// every new point.
	class ChartPage::PointStore : public QwtSeriesData<QPointF>
	{
	public:
		void append(const QPointF& p)
		{
			if (points_.isEmpty())
				bounds_ = QRectF(p, p);
			else
			{
				bounds_.setLeft(std::min(bounds_.left(), p.x()));
				bounds_.setRight(std::max(bounds_.right(), p.x()));
				bounds_.setTop(std::min(bounds_.top(), p.y()));
				bounds_.setBottom(std::max(bounds_.bottom(), p.y()));
			}
			points_.append(p);
		}

		void clear()
		{
			points_.clear();
		}

		size_t size() const override
		{
			return static_cast<size_t>(points_.size());
		}

		QPointF sample(size_t i) const override
		{
			return points_[static_cast<qsizetype>(i)];
		}

		QRectF boundingRect() const override
		{
			// QWT's convention for "no data"
			return points_.isEmpty() ? QRectF(1.0, 1.0, -2.0, -2.0) : bounds_;
		}

	private:
		QVector<QPointF> points_;
		QRectF bounds_;
	};

	ChartPage::ChartPage(const std::vector<SeriesSpec>& specs, double yMin, double yMax, QWidget* parent) :
		QwtPlot(parent),
		yMin0_{ yMin },
		yMax0_{ yMax },
		yMin_{ yMin },
		yMax_{ yMax }
	{
		grid_ = new QwtPlotGrid();
		grid_->attach(this);

		series_.reserve(specs.size());
		for (const auto& spec : specs)
		{
			Series& s = series_.emplace_back();
			s.name = spec.name;
			s.requestedColour = spec.colour;

			s.store = new PointStore();
			s.line = new QwtPlotCurve(spec.name);
			s.line->setData(s.store);		// takes ownership of store
			s.line->setRenderHint(QwtPlotItem::RenderAntialiased);
			s.line->setZ(1.0);
			s.line->attach(this);

			for (size_t m = 0; m < markerCount; ++m)
			{
				const auto& style = markerStyles[m];
				auto* marker = new QwtPlotCurve();
				marker->setStyle(QwtPlotCurve::NoCurve);
				marker->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(style.colour),
					QPen(style.colour), QSize(style.size, style.size)));
				marker->setItemAttribute(QwtPlotItem::Legend, false);
				marker->setZ(2.0);
				marker->attach(this);
				s.markers[m] = marker;
			}
		}

		insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

		// The X axis shows file numbers and the Y axis starts from the
		// range supplied (it will grow to fit the data).
		setXExtent(0);
		applyYScale();

		// The picker is a child of the canvas, so is deleted with it
		new PointTracker(canvas(), [this](const QPoint& pos)
			{
				const Series* best{ nullptr };
				int bestIndex{ -1 };
				double bestDistance{ hitDistance };
				for (const auto& s : series_)
				{
					double distance{ 0.0 };
					const int i = s.line->closestPoint(pos, &distance);
					if (i >= 0 && distance < bestDistance)
					{
						best = &s;
						bestIndex = i;
						bestDistance = distance;
					}
				}
				if (!best)
					return QwtText();
				return QwtText(QLocale().toString(best->store->sample(static_cast<size_t>(bestIndex)).y(), 'f', valueDecimals_));
			});

		setDarkTheme(false);
	}

	void ChartPage::setXExtent(int fileCount)
	{
		double interval{ 0.0 };
		double rangeMax{ 0.0 };
		xScaleFor(fileCount, interval, rangeMax);
		setAxisScale(QwtAxis::XBottom, 1.0, rangeMax, interval);
	}

	void ChartPage::append(const QString& name, double x, std::initializer_list<double> ys)
	{
		if (ys.size() != series_.size())
			return;

		index_.emplace(name, static_cast<int>(series_.front().types.size()));

		auto y = ys.begin();
		for (auto& s : series_)
		{
			s.store->append(QPointF(x, *y));
			s.types.push_back(0);
			growY(*y);
			++y;
		}
		applyYScale();
		for (auto& s : series_)
			s.line->itemChanged();
		replot();
	}

	void ChartPage::setPoint(const QString& name, size_t series, POINTTYPE type)
	{
		const auto iter = index_.find(name);
		if (iter == index_.end() || series >= series_.size() || type < PT_REFERENCE || type > PT_WARNING)
			return;

		Series& s = series_[series];
		const auto pos = static_cast<size_t>(iter->second);
		const unsigned char old = s.types[pos];
		if (old == type)
			return;

		s.types[pos] = static_cast<unsigned char>(type);
		if (old != 0)
			rebuildMarkers(s, old - 1U);
		rebuildMarkers(s, static_cast<size_t>(type) - 1U);
		replot();
	}

	void ChartPage::clear()
	{
		for (auto& s : series_)
		{
			s.store->clear();
			s.types.clear();
			for (size_t m = 0; m < markerCount; ++m)
			{
				s.markerPoints[m].clear();
				s.markers[m]->setSamples(s.markerPoints[m]);
			}
			s.line->itemChanged();
		}
		index_.clear();
		yMin_ = yMin0_;
		yMax_ = yMax0_;
		setXExtent(0);
		applyYScale();
		replot();
	}

	void ChartPage::setDarkTheme(bool dark)
	{
		dark_ = dark;

		const QColor window = dark ? QColor(30, 30, 40) : QColor(245, 235, 215);
		const QColor canvasColour = dark ? QColor(38, 40, 52) : QColor(255, 250, 240);
		const QColor text = dark ? QColor(220, 220, 225) : QColor(60, 40, 20);

		QPalette p = palette();
		p.setColor(QPalette::Window, window);
		p.setColor(QPalette::WindowText, text);
		p.setColor(QPalette::Text, text);
		setPalette(p);
		setAutoFillBackground(true);
		setCanvasBackground(canvasColour);

		QColor gridColour = text;
		gridColour.setAlpha(60);
		grid_->setPen(gridColour, 0.0, Qt::DotLine);

		// The picker's tracker text defaults to a white pen, which is
		// unreadable on a light canvas, so follow the theme.
		if (auto* picker = canvas()->findChild<QwtPlotPicker*>())
			picker->setTrackerPen(QPen(text));

		for (auto& s : series_)
			applyColours(s);
		replot();
	}

	void ChartPage::applyColours(Series& s)
	{
		s.colour = s.requestedColour.isValid() ? s.requestedColour
			: (dark_ ? QColor(79, 195, 247) : QColor(150, 90, 30));
		s.line->setPen(s.colour, 1.5);
		s.line->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QBrush(s.colour), QPen(s.colour), QSize(4, 4)));
	}

	void ChartPage::rebuildMarkers(Series& s, size_t marker)
	{
		auto& points = s.markerPoints[marker];
		points.clear();
		const auto wanted = static_cast<unsigned char>(marker + 1);
		for (size_t i = 0; i < s.types.size(); ++i)
			if (s.types[i] == wanted)
				points.append(s.store->sample(i));
		s.markers[marker]->setSamples(points);
	}

	// Widen the Y range (with a 10% margin) to include the value
	void ChartPage::growY(double value)
	{
		if (value > yMax_)
			yMax_ = value > 0 ? value * 1.1 : value * 0.9;
		if (value < yMin_)
			yMin_ = value > 0 ? value * 0.9 : value * 1.1;
	}

	// Round the Y range out to "nice" numbers (the equivalent of
	// QValueAxis::applyNiceNumbers) and apply it to the plot.
	void ChartPage::applyYScale()
	{
		double lo{ yMin_ };
		double hi{ yMax_ };
		double step{ 0.0 };
		QwtLinearScaleEngine engine;
		engine.autoScale(10, lo, hi, step);
		yMin_ = lo;
		yMax_ = hi;
		setAxisScale(QwtAxis::YLeft, lo, hi, step);
	}
}
