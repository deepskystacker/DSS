#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include <ZExcept.h>
#include <Ztrace.h>

#include <QAction>
#include <QMenu>
#include <QPalette>
#include <QSettings>
#include <QString>
#include <QSlider>

#include "PostCalibration.h"
#include "ui/ui_PostCalibration.h"

#include "StackSettings.h"
#include "StackingTasks.h"
#include "Workspace.h"

PostCalibration::PostCalibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostCalibration),
	workspace(new CWorkspace()),
	pStackSettings(dynamic_cast<StackSettings *>(parent)),
	// Use our friendship with StackSettings to get at the stacking tasks pointer
	pStackingTasks(pStackSettings->pStackingTasks),
	medianString(tr("<a href=\" \">the median</a>")),
	gaussianString(tr("<a href=\" \">a gaussian filter</a>"))
{
	if (nullptr == pStackSettings)
	{
		delete ui;
		ZASSERTSTATE(nullptr != pStackSettings);
	}
    ui->setupUi(this);

	int value = workspace->value("Stacking/PCS_ReplaceMethod", (int)CR_MEDIAN).toInt();
	switch (value)
	{
	case CR_MEDIAN:
		ui->replacementMethod->setText(medianString);
		break;
	case CR_GAUSSIAN:
		ui->replacementMethod->setText(gaussianString);
		break;
	}

	createActions().createMenus();
}

PostCalibration & PostCalibration::createActions()
{
	onMedian = new QAction(tr("the median"), this);
	connect(onMedian, &QAction::triggered, this,
		[=]() { this->setReplacementMethod(CR_MEDIAN); });
	connect(onMedian, &QAction::triggered, this,
		[=]() { ui->replacementMethod->setText(medianString); });

	onGaussian = new QAction(tr("a gaussian filter"), this);
	connect(onGaussian, &QAction::triggered, this,
		[=]() { this->setReplacementMethod(CR_GAUSSIAN); });
	connect(onGaussian, &QAction::triggered, this,
		[=]() { ui->replacementMethod->setText(gaussianString); });

	return *this;
}

void PostCalibration::on_replacementMethod_linkActivated(const QString & str)
{
	str;
	//
	// Show the popup menu 
	//
	replacementMenu->exec(QCursor::pos());
}

PostCalibration & PostCalibration::createMenus()
{
	QMenu * menu = new QMenu(this);
	menu->addAction(onMedian);
	menu->addAction(onGaussian);

	replacementMenu = menu;

	return *this;
}

PostCalibration::~PostCalibration()
{
    delete ui;
}

void PostCalibration::onSetActive()
{
	CAllStackingTasks::GetPostCalibrationSettings(pcs);

	ui->cleanHotPixels->setChecked(pcs.m_bHot);
	ui->hotFilterSize->setEnabled(pcs.m_bHot);
	ui->hotFilter->setEnabled(pcs.m_bHot);
	ui->weak1->setEnabled(pcs.m_bHot);
	ui->strong1->setEnabled(pcs.m_bHot);
	ui->hotThresholdPercent->setEnabled(pcs.m_bHot);
	ui->hotThreshold->setEnabled(pcs.m_bHot);

	ui->hotFilterSize->setText(QString("%L1").arg(pcs.m_lHotFilter));
	ui->hotFilter->setSliderPosition(pcs.m_lHotFilter);
	//
	// Display the Hot filter Detection Threshold in the user's Locale with one digit
	// after the decimal point
	//
	ui->hotThresholdPercent->setText(QString("%L1%").arg(pcs.m_fHotDetection, 0, 'f', 1));
	ui->hotThreshold->setSliderPosition(1000 - pcs.m_fHotDetection*10.0);

	ui->cleanColdPixels->setChecked(pcs.m_bCold);
	ui->coldFilterSize->setEnabled(pcs.m_bCold);
	ui->coldFilter->setEnabled(pcs.m_bCold);
	ui->weak2->setEnabled(pcs.m_bCold);
	ui->strong2->setEnabled(pcs.m_bCold);
	ui->coldThresholdPercent->setEnabled(pcs.m_bCold);
	ui->coldThreshold->setEnabled(pcs.m_bCold);

	ui->coldFilterSize->setText(QString("%L1").arg(pcs.m_lColdFilter));
	ui->coldFilter->setSliderPosition(pcs.m_lColdFilter);
	//
	// Display the Cold filter Detection Threshold in the user's Locale with one digit
	// after the decimal point
	//
	ui->coldThresholdPercent->setText(QString("%L1%").arg(pcs.m_fColdDetection, 0, 'f', 1));
	ui->coldThreshold->setSliderPosition(1000 - pcs.m_fColdDetection*10.0);

	//
	// Set the text colour as for a Hyper-Link
	// 
	ui->replacementMethod->setForegroundRole(QPalette::Link);

	//
	// Enable/Disable the test cosmetics settings depending on whether we're stacking 
	// of just setting the settings.
	//
	if (nullptr != pStackingTasks && (pcs.m_bHot || pcs.m_bCold))
	{
		ui->testCosmetic->setForegroundRole(QPalette::Link);
	}
	else
	{
		ui->testCosmetic->setVisible(false);
	}

	ui->saveDeltaImage->setChecked(pcs.m_bSaveDeltaImage);
}

void PostCalibration::on_cleanHotPixels_toggled(bool onOff)
{
	if (onOff != pcs.m_bHot)
	{
		//
		// Value has changed, so set the the new value
		//
		pcs.m_bHot = onOff;
		workspace->setValue("Stacking/PCS_DetectCleanHot", onOff);

		//
		// Set enabled state of controls accordingly
		//
		ui->hotFilterSize->setEnabled(onOff);
		ui->hotFilter->setEnabled(onOff);
		ui->weak1->setEnabled(onOff);
		ui->strong1->setEnabled(onOff);
		ui->hotThresholdPercent->setEnabled(onOff);
		ui->hotThreshold->setEnabled(onOff);

		//
		// Enable/Disable the test cosmetics settings depending on whether we're stacking 
		// of just setting the settings.
		//
		if (nullptr != pStackingTasks && (pcs.m_bHot || pcs.m_bCold))
		{
			ui->testCosmetic->setForegroundRole(QPalette::Link);
			ui->testCosmetic->setEnabled(true);
		}
		else
		{
			ui->testCosmetic->setForegroundRole(QPalette::Text);
			ui->testCosmetic->setEnabled(false);
		}
	}
}

void PostCalibration::on_hotFilter_sliderReleased()
{
	int newValue = ui->hotFilter->value();
	if (pcs.m_lHotFilter != newValue)
	{
		//
		// Value has changed
		//
		pcs.m_lHotFilter = newValue;
		workspace->setValue("Stacking/PCS_HotFilter", newValue);
	}
}

void PostCalibration::on_hotFilter_valueChanged(int newValue)
{
	//
	// Display the new value
	//
	ui->hotFilterSize->setText(QString("%L1").arg(newValue));
}

void PostCalibration::on_hotThreshold_sliderReleased()
{
	double newValue = 100.0 - (double)ui->hotThreshold->value() / 10.0;
	if (pcs.m_fHotDetection != newValue)
	{
		//
		// Value has changed
		//
		pcs.m_fHotDetection = newValue;
		workspace->setValue("Stacking/PCS_HotDetection", newValue*10.0);
	}
}

void PostCalibration::on_hotThreshold_valueChanged(int value)
{
	double newValue = 100.0 - (double)value / 10.0;

	//
	// Display the new value
	//
	ui->hotThresholdPercent->setText(QString("%L1%").arg(newValue, 0, 'f', 1));
}

void PostCalibration::on_cleanColdPixels_toggled(bool onOff)
{
	if (onOff != pcs.m_bCold)
	{
		//
		// Value has changed, so set the the new value
		//
		pcs.m_bCold = onOff;
		workspace->setValue("Stacking/PCS_DetectCleanCold", onOff);

		//
		// Set enabled state of controls accordingly
		//
		ui->coldFilterSize->setEnabled(onOff);
		ui->coldFilter->setEnabled(onOff);
		ui->weak2->setEnabled(onOff);
		ui->strong2->setEnabled(onOff);
		ui->coldThresholdPercent->setEnabled(onOff);
		ui->coldThreshold->setEnabled(onOff);

		//
		// Enable/Disable the test cosmetics settings depending on whether we're stacking 
		// of just setting the settings.
		//
		if (nullptr != pStackingTasks && (pcs.m_bHot || pcs.m_bCold))
		{
			ui->testCosmetic->setForegroundRole(QPalette::Link);
			ui->testCosmetic->setEnabled(true);
		}
		else
		{
			ui->testCosmetic->setForegroundRole(QPalette::Text);
			ui->testCosmetic->setEnabled(false);
		}
	}
}

void PostCalibration::on_coldFilter_sliderReleased()
{
	int newValue = ui->hotFilter->value();
	if (pcs.m_lColdFilter != newValue)
	{
		//
		// Value has changed
		//
		pcs.m_lColdFilter = newValue;
		workspace->setValue("Stacking/PCS_ColdFilter", newValue);
	}
}
void PostCalibration::on_coldFilter_valueChanged(int newValue)
{
	//
	// Display the new value
	//
	ui->coldFilterSize->setText(QString("%L1").arg(newValue));
}

void PostCalibration::on_coldThreshold_sliderReleased()
{
	double newValue = 100.0 - (double)ui->coldThreshold->value() / 10.0;
	if (pcs.m_fColdDetection != newValue)
	{
		//
		// Value has changed
		//
		pcs.m_fColdDetection = newValue;
		workspace->setValue("Stacking/PCS_ColdDetection", newValue*10.0);
	}
}

void PostCalibration::on_coldThreshold_valueChanged(int value)
{
	double newValue = 100.0 - (double)value / 10.0;

	//
	// Display the new value
	//
	ui->coldThresholdPercent->setText(QString("%L1%").arg(newValue, 0, 'f', 1));
}

PostCalibration& PostCalibration::setReplacementMethod(int value)
{
	if (pcs.m_Replace != value)
	{
		pcs.m_Replace = static_cast<COSMETICREPLACE>(value);
		workspace->setValue("Stacking/PCS_ReplaceMethod", value);
	}
	return *this;
}

void PostCalibration::on_saveDeltaImage_toggled(bool onOff)
{
	if (pcs.m_bSaveDeltaImage != onOff)
	{
		pcs.m_bSaveDeltaImage = onOff;
		workspace->setValue("Stacking/PCS_SaveDeltaImage", onOff);
	}
}