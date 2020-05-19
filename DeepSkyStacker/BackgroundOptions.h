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
    Q_PROPERTY(BACKGROUNDCALIBRATIONMODE backgroundCalibrationMode READ backgroundCalibrationMode WRITE setBackgroundCalibrationMode NOTIFY backgroundCalibrationModeChanged);
    Q_PROPERTY(BACKGROUNDCALIBRATIONINTERPOLATION backgroundCalibrationInterpolation READ backgroundCalibrationInterpolation WRITE setBackgroundCalibrationInterpolation NOTIFY backgroundCalibrationInterpolationChanged);
    Q_PROPERTY(RGBBACKGROUNDCALIBRATIONMETHOD RGBBackgroundCalibrationMethod READ RGBBackgroundCalibrationMethod WRITE setRGBBackgroundCalibrationMethod NOTIFY RGBBackgroundCalibrationMethodChanged);

public:
    explicit BackgroundOptions(QWidget *parent = 0);
    ~BackgroundOptions();

    void setBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE mode)
    {
        if (mode != m_CalibrationMode)
        {
            m_CalibrationMode = mode;
            emit backgroundCalibrationModeChanged();
        }
    }

    BACKGROUNDCALIBRATIONMODE backgroundCalibrationMode()
    {
        return m_CalibrationMode;
    }

    void setBackgroundCalibrationInterpolation(BACKGROUNDCALIBRATIONINTERPOLATION interpolation)
    {
        if (interpolation != m_CalibrationInterpolation)
        {
            m_CalibrationInterpolation = interpolation;
            updateInterpolation();
            emit backgroundCalibrationInterpolationChanged();
        }
    }

    BACKGROUNDCALIBRATIONINTERPOLATION backgroundCalibrationInterpolation()
    {
        return m_CalibrationInterpolation;
    }

    void setRGBBackgroundCalibrationMethod(RGBBACKGROUNDCALIBRATIONMETHOD method)
    {
        if (method != m_RGBBackgroundCalibrationMethod)
        {
            m_RGBBackgroundCalibrationMethod = method;
            emit RGBBackgroundCalibrationMethodChanged();
        }
    }

    RGBBACKGROUNDCALIBRATIONMETHOD RGBBackgroundCalibrationMethod()
    {
        return m_RGBBackgroundCalibrationMethod;
    }

signals:
    void backgroundCalibrationModeChanged();
    void backgroundCalibrationInterpolationChanged();
    void RGBBackgroundCalibrationMethodChanged();

private slots:
    void onToggledLinear(bool);
    void onToggledRational(bool);
    void onToggledNone(bool);
    void onToggledMinimum(bool);
    void onToggledMiddle(bool);
    void onToggledMaximum(bool);

private:
    Ui::BackgroundOptions *ui;
    BACKGROUNDCALIBRATIONMODE m_CalibrationMode;
    BACKGROUNDCALIBRATIONINTERPOLATION m_CalibrationInterpolation;
    RGBBACKGROUNDCALIBRATIONMETHOD m_RGBBackgroundCalibrationMethod;

    void updateInterpolation();
    void updateRGBCalibration();
};

#endif // BACKGROUNDOPTIONS_H
