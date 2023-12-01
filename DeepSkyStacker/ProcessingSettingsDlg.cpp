#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "ProcessingSettingsDlg.h"

#define dssApp DeepSkyStacker::instance()
namespace DSS {
	ProcessingSettingsDlg::ProcessingSettingsDlg(QWidget* parent)
		: BaseDialog(BaseDialog::Behaviour::PersistGeometry, parent)
	{
		ZFUNCTRACE_RUNTIME();
		setupUi(this);

		connectSignalsToSlots();
	}

	void ProcessingSettingsDlg::connectSignalsToSlots()
	{
		connect(addButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::addPressed);
		connect(deleteButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::deletePressed);
		connect(loadButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::loadPressed);
		connect(closeButton, &QPushButton::pressed, this, &ProcessingSettingsDlg::closePressed);
	}


	ProcessingSettingsDlg::~ProcessingSettingsDlg()
	{}

	bool ProcessingSettingsDlg::isValidFilename(const QString& name)
	{
		bool result = true;
		if (name.contains(QRegularExpression("[/\\]")))
		{
			result = false;
			QApplication::beep();
			QMessageBox::critical(this, "DeepSkyStacker",
				tr("The Processing Settings name may not contain either a / or \\.\n"
					"Please change the name."));
		}
		return result;
	}

	//
	// Slots
	//
	void ProcessingSettingsDlg::addPressed()
	{
		QString name{ settingsName->text()};
	}

	void ProcessingSettingsDlg::deletePressed()
	{

	}

	void ProcessingSettingsDlg::loadPressed()
	{

	}

	void ProcessingSettingsDlg::closePressed()
	{
		dssApp->processingSettingsSet().saveSettings();
		QDialog::accept();
	}
} // namespace DSS
