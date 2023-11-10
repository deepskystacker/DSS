#pragma once

#include <QDialog>
#include "ui_EmailSettings.h"

namespace DSS
{
	class LiveSettings;

	class EmailSettings : public QDialog,
		public Ui_EmailSettings
	{
		Q_OBJECT

			using Inherited = QDialog;

	public:
		EmailSettings(QWidget* parent);
		~EmailSettings();

	protected:
		void showEvent(QShowEvent* event) override;

	private slots:
		void showPasswordClicked(bool checked);
		void save();
		void test();

	private:
		bool initialised;
		LiveSettings& liveSettings;

		void connectSignalsToSlots();
		void onInitDialog();

	};
}
