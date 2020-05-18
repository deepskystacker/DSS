#ifndef STACKINGPARAMETERS_H
#define STACKINGPARAMETERS_H

#include <QWidget>
enum MULTIBITMAPPROCESSMETHOD : short;
enum PICTURETYPE : short;
class CWorkspace;
class StackSettings;

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

StackingParameters & init(PICTURETYPE rhs);
StackingParameters & setControls(MULTIBITMAPPROCESSMETHOD method, double kappa, uint iteration);
};

#endif // STACKINGPARAMETERS_H
