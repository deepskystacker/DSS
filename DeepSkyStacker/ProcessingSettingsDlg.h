#pragma once
#include <QDialog>
#include "BaseDialog.h"
#include "ProcessingSettings.h"
#include "ui_ProcessingSettingsDlg.h"


namespace DSS {
	class ProcessingSettingsDlg  : public BaseDialog, public Ui::ProcessingSettingsDlg
	{
		Q_OBJECT

	public:
		ProcessingSettingsDlg(QWidget *parent);
		~ProcessingSettingsDlg();

		ProcessingSettingsDlg(const ProcessingSettings& rhs) = delete;

		ProcessingSettingsDlg& operator = (const ProcessingSettingsDlg& rhs) = delete;

		inline void setParameters(ProcessingSettingsSet& set, ProcessingSettings current ) 
		{ 
			settingsSet = set;
			currentSettings = current;

			const int count = settingsSet.Count();
			ProcessingSettings settings;

			//
			// Populate the list of named settings
			//
			for (int i = 0; i < count; i++)
			{
				if (settingsSet.GetItem(i, settings))
					settingsList->addItem(settings.name_);
			}
		}

		inline const ProcessingSettingsSet& parameterSet() const { return settingsSet; }

	private slots:
		void addPressed();
		void deletePressed();
		void loadPressed();
		void closePressed();

		void nameEdited(const QString& text);
		void listItemDoubleClicked(QListWidgetItem* item);

	private:
		ProcessingSettingsSet& settingsSet;
		ProcessingSettings currentSettings;

		bool isValidFilename(const QString& name);

		void connectSignalsToSlots();
	};
} // namespace DSS
