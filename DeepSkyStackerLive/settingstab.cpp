#include "stdafx.h"
#include "settingstab.h"

namespace DSS
{
	SettingsTab::SettingsTab(QWidget* parent)
		: QWidget{ parent },
		initialised{ false }
	{
		setupUi(this);

		connectSignalsToSlots();
	}

	/* ------------------------------------------------------------------- */

	SettingsTab::~SettingsTab()
	{}

	/* ------------------------------------------------------------------- */

	void SettingsTab::connectSignalsToSlots()
	{
	}
}
