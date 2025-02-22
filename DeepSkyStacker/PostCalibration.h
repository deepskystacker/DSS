#pragma once
#include "StackingTasks.h"

class Workspace;
class CAllStackingTasks;
class QAction;
class QString;
class QMenu;

namespace DSS
{
	class StackSettings;

	namespace Ui {
		class PostCalibration;
	}

	class PostCalibration : public QWidget
	{
		Q_OBJECT

			typedef QWidget
			Inherited;
	public:
		explicit PostCalibration(QWidget* parent = nullptr);
		~PostCalibration();

	public slots:
		void onSetActive();

	private:
		Ui::PostCalibration* ui;
		std::unique_ptr<Workspace> workspace;
		StackSettings* pStackSettings;
		CPostCalibrationSettings pcs;
		CAllStackingTasks* pStackingTasks;
		QAction* onMedian;
		QAction* onGaussian;
		QMenu* replacementMenu;
		QString medianString;
		QString gaussianString;
		PostCalibration& createActions();
		PostCalibration& createMenus();
		PostCalibration& setReplacementMethod(int);


	private slots:
		void on_cleanHotPixels_toggled(bool);
		void on_hotFilter_valueChanged(int);
		void on_hotThreshold_valueChanged(int);

		void on_cleanColdPixels_toggled(bool);
		void on_coldFilter_valueChanged(int);
		void on_coldThreshold_valueChanged(int);

		void on_replacementMethod_clicked();
		void on_saveDeltaImage_toggled(bool);
		void on_testCosmetic_clicked();
	};
}