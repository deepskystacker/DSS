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

		inline void setParameterSet(const ProcessingSettings& set) { parameterSet_ = set; };
		inline const ProcessingSettings& parameterSet() const { return parameterSet_; }

	private slots:
		void addPressed();
		void deletePressed();
		void loadPressed();
		void closePressed();

	private:
		ProcessingSettings parameterSet_;

		bool isValidFilename(const QString& name);

		void connectSignalsToSlots();
	};
} // namespace DSS
