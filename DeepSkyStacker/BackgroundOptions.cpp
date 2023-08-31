#include "stdafx.h"
#include "BackgroundOptions.h"
#include "ui/ui_BackgroundOptions.h"
#include "StackingTasks.h"
#include "Workspace.h"


BackgroundOptions::BackgroundOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundOptions)
{
    ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	m_CalibrationMode = CAllStackingTasks::GetBackgroundCalibrationMode();
	m_RGBCalibrationMethod = CAllStackingTasks::GetRGBBackgroundCalibrationMethod();

	if ((m_CalibrationMode == BCM_NONE) || (m_CalibrationMode == BCM_PERCHANNEL))
	{
		ui->rbNone->setChecked(true);
		on_rbNone_clicked();
	}
	else if (m_RGBCalibrationMethod == RBCM_MINIMUM)
	{
		ui->rbMinimum->setChecked(true);
		on_rbMinimum_clicked();
	}
	else if (m_RGBCalibrationMethod == RBCM_MIDDLE)
	{
		ui->rbMiddle->setChecked(true);
		on_rbMiddle_clicked();
	}
	else
	{
		ui->rbMaximum->setChecked(true);
		on_rbMaximum_clicked();
	}

	updateInterpolation(CAllStackingTasks::GetBackgroundCalibrationInterpolation());
}

BackgroundOptions::~BackgroundOptions()
{
    delete ui;
}

void BackgroundOptions::updateInterpolation(BACKGROUNDCALIBRATIONINTERPOLATION interpolation)
{
	m_CalibrationInterpolation = interpolation;
    if (m_CalibrationInterpolation == BCI_LINEAR)
    {
		if (pxLinear.isNull())
		{
			pxLinear.load(":/calibration/linear.bmp");
		}
        ui->laCalibration->setPixmap(pxLinear);
        ui->rbLinear->setChecked(true);
    }
    else
    {
		if (pxRational.isNull())
		{
			pxRational.load(":/calibration/rational.bmp");
		}
        ui->laCalibration->setPixmap(pxRational);
        ui->rbRational->setChecked(true);
    }
}

void BackgroundOptions::on_rbLinear_clicked()
{
	updateInterpolation(BCI_LINEAR);
}

void BackgroundOptions::on_rbRational_clicked()
{
	updateInterpolation(BCI_RATIONAL);
}

void BackgroundOptions::on_rbNone_clicked()
{
	if (pxNone.isNull())
	{
		pxNone.load(":/rgbcalibration/none.bmp");
	}
	ui->laRGBCalibration->setPixmap(pxNone);
}

void BackgroundOptions::on_rbMinimum_clicked()
{
	if (pxMin.isNull())
	{
		pxMin.load(":/rgbcalibration/minimum.bmp");
	}
	ui->laRGBCalibration->setPixmap(pxMin);
}

void BackgroundOptions::on_rbMiddle_clicked()
{
	if (pxMid.isNull())
	{
		pxMid.load(":/rgbcalibration/middle.bmp");
	}
	ui->laRGBCalibration->setPixmap(pxMid);
}

void BackgroundOptions::on_rbMaximum_clicked()
{
	if (pxMax.isNull())
	{
		pxMax.load(":/rgbcalibration/maximum.bmp");
	}
	ui->laRGBCalibration->setPixmap(pxMax);
}

void BackgroundOptions::accept()
{
	Workspace			workspace;

	if (ui->rbLinear->isChecked())
		m_CalibrationInterpolation = BCI_LINEAR;
	else
		m_CalibrationInterpolation = BCI_RATIONAL;

	workspace.setValue("Stacking/BackgroundCalibrationInterpolation", (uint)m_CalibrationInterpolation);

	if (!ui->rbNone->isChecked())
	{
		m_CalibrationMode = BCM_RGB;
		if (ui->rbMinimum->isChecked())
			m_RGBCalibrationMethod = RBCM_MINIMUM;
		else if (ui->rbMiddle->isChecked())
			m_RGBCalibrationMethod = RBCM_MIDDLE;
		else
			m_RGBCalibrationMethod = RBCM_MAXIMUM;


		workspace.setValue("Stacking/RGBBackgroundCalibrationMethod", (uint)m_RGBCalibrationMethod);
	}
	else if (m_CalibrationMode == BCM_RGB)
	{
		m_CalibrationMode = BCM_PERCHANNEL;
	};

	Inherited::accept();
}

void BackgroundOptions::reject()
{
	Inherited::reject();
}
