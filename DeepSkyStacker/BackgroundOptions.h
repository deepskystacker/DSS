#ifndef BACKGROUNDOPTIONS_H
#define BACKGROUNDOPTIONS_H

#include <QDialog>

enum BACKGROUNDCALIBRATIONMODE : short;
enum BACKGROUNDCALIBRATIONINTERPOLATION : short;
enum RGBBACKGROUNDCALIBRATIONMETHOD : short;

namespace Ui {
class BackgroundOptions;
}

class BackgroundOptions : public QDialog
{
    Q_OBJECT

typedef QDialog
		Inherited;

public:
    explicit BackgroundOptions(QWidget *parent = 0);
    ~BackgroundOptions();

	void	SetBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE Mode)
	{
		m_CalibrationMode = Mode;
	};

	BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode()
	{
		return m_CalibrationMode;
	};

private slots:
    void on_rbLinear_clicked();
    void on_rbRational_clicked();
	void on_rbNone_clicked();
	void on_rbMinimum_clicked();
	void on_rbMiddle_clicked();
	void on_rbMaximum_clicked();

	void accept();
	void reject();

private:
    Ui::BackgroundOptions *ui;
    BACKGROUNDCALIBRATIONMODE m_CalibrationMode;
    BACKGROUNDCALIBRATIONINTERPOLATION m_CalibrationInterpolation;
    RGBBACKGROUNDCALIBRATIONMETHOD m_RGBCalibrationMethod;

    void updateInterpolation(BACKGROUNDCALIBRATIONINTERPOLATION interpolation);
};

#endif // BACKGROUNDOPTIONS_H
