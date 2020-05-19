#ifndef STACKINGPARAMETERS_H
#define STACKINGPARAMETERS_H

#include <QWidget>

enum BACKGROUNDCALIBRATIONMODE : short;
enum MULTIBITMAPPROCESSMETHOD : short;
enum PICTURETYPE : short;
class CWorkspace;
class StackSettings;
class QAction;
class QMenu;

namespace Ui {
class StackingParameters;
}

class StackingParameters : public QWidget
{
    Q_OBJECT

public:
    explicit StackingParameters(QWidget *parent = nullptr);
    ~StackingParameters();

private:
    Ui::StackingParameters *ui;
	std::unique_ptr<CWorkspace> workspace;
	StackSettings * pStackSettings;
	PICTURETYPE type;
	QString kappaSigmaTip;
	QString medianKappaSigmaTip;
	QAction * nobgCal;
	QAction * pcbgCal;
	QAction * rgbbgCal;
	QMenu * backgroundCalibrationMenu;

private slots:
	void setBackgroundCalibration(BACKGROUNDCALIBRATIONMODE mode);

StackingParameters & init(PICTURETYPE rhs);
StackingParameters & setControls(MULTIBITMAPPROCESSMETHOD method, double kappa, uint iteration);
StackingParameters & createActions();
StackingParameters & createMenu();

void	on_backgroundCalibration_clicked();
};

#endif // STACKINGPARAMETERS_H
