#include "pch.h"
#include "processingcontrols.h"

namespace DSS {
	ProcessingControls::ProcessingControls(QWidget* parent)
		: QWidget(parent)
	{
		setupUi(this);
	}

	ProcessingControls::~ProcessingControls()
	{
	}

	void ProcessingControls::changeEvent(QEvent* e)
	{
		if (e->type() == QEvent::LanguageChange) {
			retranslateUi(this);
		}
		else {
			QWidget::changeEvent(e);
		}
	}
}// namespace DSS
