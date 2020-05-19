#include "BackgroundOptions.h"
#include "ui/ui_BackgroundOptions.h"

BackgroundOptions::BackgroundOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundOptions)
{
    ui->setupUi(this);
    m_CalibrationInterpolation = BACKGROUNDCALIBRATIONINTERPOLATION(-1);
    m_CalibrationMode = BACKGROUNDCALIBRATIONMODE(-1);
    m_RGBBackgroundCalibrationMethod = RGBBACKGROUNDCALIBRATIONMETHOD(-1);
}

BackgroundOptions::~BackgroundOptions()
{
    delete ui;
}

void BackgroundOptions::updateInterpolation()
{
    if (m_CalibrationInterpolation == BCI_LINEAR)
    {
        QPixmap pm(":/calibration/linear.bmp");
        ui->laCalibration->setPixmap(pm);
        ui->rbLinear->setChecked(true);
    }
    else
    {
        QPixmap pm(":/calibration/rational.bmp");
        ui->laCalibration->setPixmap(pm);
        ui->rbRational->setChecked(true);
    }
}

void BackgroundOptions::updateRGBCalibration()
{
    if (m_CalibrationMode != BCM_RGB)
    {
        QPixmap pm(":/rgbcalibration/none.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
    else if (m_RGBBackgroundCalibrationMethod == RBCM_MINIMUM)
    {
        QPixmap pm(":/rgbcalibration/minimum.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
    else if (m_RGBBackgroundCalibrationMethod == RBCM_MIDDLE)
    {
        QPixmap pm(":/rgbcalibration/middle.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
    else if (m_RGBBackgroundCalibrationMethod == RBCM_MAXIMUM)
    {
        QPixmap pm(":/rgbcalibration/maximum.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
}

void BackgroundOptions::onToggledLinear(bool on)
{
    if (on)
    {
        setBackgroundCalibrationInterpolation(BCI_LINEAR);
    }
}

void BackgroundOptions::onToggledRational(bool on)
{
    if (on)
    {
        setBackgroundCalibrationInterpolation(BCI_RATIONAL);
    }
}

void BackgroundOptions::onToggledNone(bool on)
{
    if (on)
    {
        QPixmap pm(":/rgbcalibration/none.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
}

void BackgroundOptions::onToggledMinimum(bool on)
{
    if (on)
    {
        QPixmap pm(":/rgbcalibration/minimum.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
}

void BackgroundOptions::onToggledMiddle(bool on)
{
    if (on)
    {
        QPixmap pm(":/rgbcalibration/middle.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
}

void BackgroundOptions::onToggledMaximum(bool on)
{
    if (on)
    {
        QPixmap pm(":/rgbcalibration/maximum.bmp");
        ui->laRGBCalibration->setPixmap(pm);
    }
}
