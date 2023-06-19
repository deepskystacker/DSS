#pragma once

#include <QWidget>
#include "ui_ChartTab.h"

namespace DSS
{
	class ChartTab : public QWidget, public Ui::ChartTab
	{
		Q_OBJECT

	public:
		ChartTab(QWidget* parent = nullptr);
		~ChartTab();
	};
}
