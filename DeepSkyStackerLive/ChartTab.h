#pragma once

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
