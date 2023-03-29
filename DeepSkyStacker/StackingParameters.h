#pragma once

class Workspace;
class StackSettings;
class QAction;
class QMenu;
class QValidator;

#include "DSSCommon.h"

namespace Ui {
class StackingParameters;
}

class StackingParameters : public QWidget
{
    Q_OBJECT

typedef QWidget
		Inherited;

public:
    explicit StackingParameters(QWidget *parent = nullptr, PICTURETYPE = PICTURETYPE_UNKNOWN);
    ~StackingParameters();

public slots:
	void onSetActive();

private:
    Ui::StackingParameters *ui;
	std::unique_ptr<Workspace> workspace;
	StackSettings * pStackSettings;
	PICTURETYPE type;
	BACKGROUNDCALIBRATIONMODE mode;
	MULTIBITMAPPROCESSMETHOD method;
	double	kappa;
	uint	iteration;
	QString kappaSigmaTip;
	QString medianKappaSigmaTip;
	QString nobgCalString;
	QString pcbgCalString;
	QString rgbbgCalString;
	QAction * nobgCal;
	QAction * pcbgCal;
	QAction * rgbbgCal;
	QAction * bgCalOptions;
	QMenu   * backgroundCalibrationMenu;

	QValidator * darkFactorValidator;
	QValidator * iterationValidator;
	QValidator * kappaValidator;

	StackingParameters & setControls();
	StackingParameters & createActions();
	StackingParameters & createMenus();

	void setMethod(MULTIBITMAPPROCESSMETHOD method);
	void setBackgroundCalibration(BACKGROUNDCALIBRATIONMODE mode);

signals:
	void methodChanged(MULTIBITMAPPROCESSMETHOD newMethod);

private slots:
	void on_modeAverage_clicked();
	void on_modeMedian_clicked();
	void on_modeKS_clicked();
	void on_modeMKS_clicked();
	void on_modeAAWA_clicked();
	void on_modeEWA_clicked();
	void on_modeMaximum_clicked();

	void on_backgroundCalibration_clicked();
	void backgroundCalibrationOptions();

	void on_iterations_editingFinished();
	void on_kappa_editingFinished();

	void on_debloom_stateChanged(int);
	void on_hotPixels_stateChanged(int);
	void on_badColumns_stateChanged(int);
	void on_darkOptimisation_stateChanged(int);
	void on_useDarkFactor_stateChanged(int);
	void on_darkMultiplicationFactor_editingFinished();

	void updateControls(MULTIBITMAPPROCESSMETHOD newMethod);

};
