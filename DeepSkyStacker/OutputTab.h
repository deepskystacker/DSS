#pragma once
#include "StackingTasks.h"

class QString;

namespace DSS
{
	namespace Ui {
		class OutputTab;
	}

	class OutputTab : public QWidget
	{
		Q_OBJECT

			typedef QWidget
			Inherited;

	public:
		explicit OutputTab(QWidget* parent = nullptr);
		~OutputTab();

		void saveOutputSettings();

	public slots:
		void onSetActive();

	private:
		Ui::OutputTab* ui;
		COutputSettings	os;

	private slots:
		void on_createOutput_stateChanged(int);
		void on_createHTML_stateChanged(int);
		void on_autoSave_clicked();
		void on_fileListName_clicked();
		void on_appendNumber_stateChanged(int);
		void on_refFrameFolder_clicked();
		void on_fileListFolder_clicked();
		void on_otherFolder_clicked();
		void on_outputFolder_pressed();
	};
}