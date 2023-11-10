#pragma once

#include <QWidget>
#include "ui_ChartTab.h"
#include <QLineSeries>
#include <QValueAxis>
#include <QScatterSeries>

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
		CT_SCORE = 1,
		CT_FWHM = 2,
		CT_STARS = 3,
		CT_DX = 4,
		CT_DY = 5,
		CT_ANGLE = 6,
		CT_SKYBACKGROUND = 7
	};

	class ChartTab : public QWidget, public Ui::ChartTab
	{
		Q_OBJECT

	private:
		std::vector<QString> files;
		std::map<QString, int> nameMap;
		QChart::ChartTheme theme;
		Qt::ColorScheme scheme;
		QLineSeries* scoreSeries;
		std::map<QString, int> scoreMap;
		QChart* scoreChart;
		QLineSeries* fwhmSeries;
		std::map<QString, int> fwhmMap;
		QChart* fwhmChart;
		QLineSeries* starsSeries;
		std::map<QString, int> starsMap;
		QChart* starsChart;
		QLineSeries* dXSeries;
		QLineSeries* dYSeries;
		std::map<QString, int> offsetMap;
		QChart* offsetChart;
		QLineSeries* angleSeries;
		std::map<QString, int> angleMap;
		QChart* angleChart;
		QLineSeries* skybgSeries;
		std::map<QString, int> skybgMap;
		QChart* skybgChart;

		void connectSignalsToSlots();

		inline QValueAxis* axisX(QChart* chart)
		{
			return qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal)[0]);
		}

		inline QValueAxis* axisY(QChart* chart)
		{
			return qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical)[0]);
		}

	private slots:
		void scoreButtonClicked(bool checked);
		void fwhmButtonClicked(bool checked);
		void starsButtonClicked(bool checked);
		void offsetButtonClicked(bool checked);
		void angleButtonClicked(bool checked);
		void skybgButtonClicked(bool checked);

		void scoreHovered(const QPointF& point, bool state);
		void fwhmHovered(const QPointF& point, bool state);
		void starsHovered(const QPointF& point, bool state);
		void dXHovered(const QPointF& point, bool state);
		void dYHovered(const QPointF& point, bool state);
		void angleHovered(const QPointF& point, bool state);
		void skybgHovered(const QPointF& point, bool state);
		void colorSchemeChanged(Qt::ColorScheme colorScheme);

	public slots:
		void setImageInfo(QString name, STACKIMAGEINFO info);
		void clear();

	public:
		ChartTab(QWidget* parent = nullptr);
		~ChartTab();
		void addScoreFWHMStars(const QString& name, double fScore, double fFWHM, double fStars, double fSkyBackground);
		void addOffsetAngle(const QString& name, double fdX, double fdY, double fAngle);
		void setPoint(const QString& name, POINTTYPE ptType, CHARTTYPE ctType);

	};
}
