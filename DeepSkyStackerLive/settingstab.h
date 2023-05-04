#pragma once

#include <QWidget>
#include "ui_settingstab.h"

namespace DSS
{
	class SettingsTab : public QWidget, public Ui::SettingsTab
	{
		Q_OBJECT

	public:
		SettingsTab(QWidget* parent = nullptr);
		~SettingsTab();

	private:
		bool initialised;

		void connectSignalsToSlots();
	};
}