#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include "StackingParameters.h"
#include "ui/ui_StackingParameters.h"

#include <QAction>

#include <ZExcept.h>
#include <Ztrace.h>

#include "DSSCommon.h"
#include "StackSettings.h"
#include "StackingTasks.h"
#include "Workspace.h"

StackingParameters::StackingParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackingParameters),
	workspace(new CWorkspace()),
	pStackSettings(dynamic_cast<StackSettings *>(parent))
{
	if (nullptr == pStackSettings)
	{
		delete ui;
		ZASSERTSTATE(nullptr != pStackSettings);
	}

    ui->setupUi(this);

	//
	// Make all the "optional" controls invisible
	// 
	// First the light settings
	//
	ui->backgroundCalibration->setVisible(false);
	ui->debloom->setVisible(false);
	ui->deBloomSettings->setVisible(false);
	//
	// Then the dark settings
	//
	ui->hotPixels->setVisible(false);
	ui->badColumns->setVisible(false);
	ui->darkOptimisation->setVisible(false);
	ui->useDarkFactor->setVisible(false);
	ui->darkMultiplicationFactor->setVisible(false);

	//
	// Set the tooltip text
	//
	kappaSigmaTip = tr("The pixels outside the range:\n[Mean-%1*%2, Mean+%1*%2]\n"
		"are iteratively removed.\n\nThe remaining pixels are averaged.")
		// Greek character kappa (κ) = \xce\xba UTF8, Greek character sigma (σ) = \xcf\x83 UTF8
		.arg("\xce\xba").arg("\xcf\x83");		
	ui->modeKS->setToolTip(kappaSigmaTip);
	ui->staticKappa->setToolTip(kappaSigmaTip);
	ui->kappa->setToolTip(kappaSigmaTip);

	medianKappaSigmaTip = tr("The pixels outside the range:\n[Mean-%1*%2, Mean+%1*%2]\n"
		"are iteratively replaced by the median value.\n\nThe pixels are then averaged.")
		// Greek character kappa (κ) = \xce\xba UTF8, Greek character sigma (σ) = \xcf\x83 UTF8
		.arg("\xce\xba").arg("\xcf\x83");
	ui->modeMKS->setToolTip(medianKappaSigmaTip);

	QString text = tr("The <b>weighted average</b> is obtained by\niteratively weighting each pixel\n"
		"from the deviation from the mean\ncomparatively to the standard deviation (%1).")
		// Greek character sigma (σ) = \xcf\x83 UTF8
		.arg("\xce\xba");
	ui->modeAAWA->setToolTip(text);

	createActions().createMenu();
	
}

StackingParameters & StackingParameters::createActions()
{
	nobgCal = new QAction(tr("No Background Calibration"), this);
	connect(nobgCal, &QAction::triggered, this,
		[=]() { this->setBackgroundCalibration(BCM_NONE); });
	connect(nobgCal, &QAction::triggered, this,
		[=]() { ui->backgroundCalibration->setText(nobgCal->text()); });

	pcbgCal = new QAction(tr("Per Channel Background Calibration"), this);
	connect(pcbgCal, &QAction::triggered, this,
		[=]() { this->setBackgroundCalibration(BCM_PERCHANNEL); });
	connect(pcbgCal, &QAction::triggered, this,
		[=]() { ui->backgroundCalibration->setText(pcbgCal->text()); });


	rgbbgCal = new QAction(tr("RGB Channels Background Calibration"), this);
	connect(rgbbgCal, &QAction::triggered, this,
		[=]() { this->setBackgroundCalibration(BCM_RGB); });
	connect(pcbgCal, &QAction::triggered, this,
		[=]() { ui->backgroundCalibration->setText(rgbbgCal->text()); });

	return *this;
}

StackingParameters & StackingParameters::createMenu()
{

	return *this;
}

StackingParameters::~StackingParameters()
{
    delete ui;
}

StackingParameters & StackingParameters::init(PICTURETYPE rhs)
{
	type = rhs;
	MULTIBITMAPPROCESSMETHOD method;
	uint iteration;
	double kappa;
	bool isChecked;
	QString string;

	switch (type)
	{
	case PICTURETYPE_LIGHTFRAME:
		// Make the Light frame specific controls visible
		ui->backgroundCalibration->setVisible(true);

		method = static_cast<MULTIBITMAPPROCESSMETHOD>
			(workspace->value("Stacking/Light_Method", (uint)MBP_AVERAGE).toUInt());
		iteration = workspace->value("Stacking/Light_Iteration", (uint)5).toUInt();
		kappa = workspace->value("Stacking/Light_Kappa", "2.0").toDouble();
		setControls(method, kappa, iteration);

		//
		// Use our friendship with StackSettings to get at pStackingTasks pointer
		//
		if ((nullptr == pStackSettings->pStackingTasks) ||
			(!pStackSettings->pStackingTasks->AreBayerImageUsed() &&
				!pStackSettings->pStackingTasks->AreColorImageUsed()))
		{
			ui->debloom->setVisible(true);
			// ui->deBloomSettings->setVisible(false); 
			ui->debloom->setChecked(workspace->value("Stacking/Debloom", false).toBool());
		};

		BACKGROUNDCALIBRATIONMODE		calibrationMode;

		calibrationMode = CAllStackingTasks::GetBackgroundCalibrationMode();

		//
		// Set the text colour as for a Hyper-Link
		// 
		ui->backgroundCalibration->setForegroundRole(QPalette::Link);

		// Set the text of the control
		switch (calibrationMode)
		{
		case BCM_NONE:
			string = tr("No Background Calibration");
			break;
		case BCM_PERCHANNEL:
			string = tr("Per Channel Background Calibration");
			break;
		case BCM_RGB:
			string = tr("RGB Channels Background Calibration");
			break;
		}
		ui->backgroundCalibration->setText(string);

		break;
	

	case PICTURETYPE_DARKFRAME:
		// Make the Dark frame specific controls visible
		ui->hotPixels->setVisible(true);
		ui->darkOptimisation->setVisible(true);
		ui->useDarkFactor->setVisible(true);
		ui->darkMultiplicationFactor->setVisible(true);

		//
		// Disable Entropy-Weighted Average
		//
		ui->modeEWA->setEnabled(false);

		method = static_cast<MULTIBITMAPPROCESSMETHOD>
			(workspace->value("Stacking/Dark_Method", (uint)MBP_AVERAGE).toUInt());
		iteration = workspace->value("Stacking/Dark_Iteration", (uint)5).toUInt();
		kappa = workspace->value("Stacking/Dark_Kappa", "2.0").toDouble();
		setControls(method, kappa, iteration);

		isChecked = workspace->value("Stacking/DarkOptimization", false).toBool();
		ui->darkOptimisation->setChecked(isChecked);

		isChecked = workspace->value("Stacking/UseDarkFactor", false).toBool();
		ui->useDarkFactor->setChecked(isChecked);

		string = workspace->value("Stacking/DarkFactor", "1.0").toString();
		ui->darkMultiplicationFactor->setText(string);

		isChecked = workspace->value("Stacking/HotPixelsDetection", true).toBool();
		ui->hotPixels->setChecked(isChecked);

		//
		// Use our friendship with StackSettings to get at pStackingTasks pointer
		//
		if ((nullptr == pStackSettings->pStackingTasks) ||
			(!pStackSettings->pStackingTasks->AreBayerImageUsed() &&
				!pStackSettings->pStackingTasks->AreColorImageUsed()))
		{
			ui->badColumns->setVisible(true);
			isChecked = workspace->value("Stacking/BadLinesDetection").toBool();
			ui->badColumns->setChecked(isChecked);
		};

		break;

	case PICTURETYPE_FLATFRAME:
		method = static_cast<MULTIBITMAPPROCESSMETHOD>
			(workspace->value("Stacking/Flat_Method", (uint)MBP_AVERAGE).toUInt());
				iteration = workspace->value("Stacking/Flat_Iteration", (uint)5).toUInt();
		kappa = workspace->value("Stacking/Flat_Kappa", "2.0").toDouble();
		setControls(method, kappa, iteration);

		//
		// Disable Entropy-Weighted Average
		//
		ui->modeEWA->setEnabled(false);
		break;
	case PICTURETYPE_OFFSETFRAME:
		method = static_cast<MULTIBITMAPPROCESSMETHOD>
			(workspace->value("Stacking/Offset_Method", (uint)MBP_AVERAGE).toUInt());
				iteration = workspace->value("Stacking/Offset_Iteration", (uint)5).toUInt();
		kappa = workspace->value("Stacking/FlatOffset_Kappa", "2.0").toDouble();
		setControls(method, kappa, iteration);

		//
		// Disable Entropy-Weighted Average
		//
		ui->modeEWA->setEnabled(false);
		break;
	};
	return *this;
}

StackingParameters & StackingParameters::setControls(MULTIBITMAPPROCESSMETHOD method, double kappa, uint iteration)
{
	QString text;
	switch (method)
	{
	case MBP_AVERAGE:
		ui->modeAverage->setChecked(true);
		break;
	case MBP_MEDIAN:
		ui->modeMedian->setChecked(true);
		break;
	case MBP_MAXIMUM:
		ui->modeMaximum->setChecked(true);
		break;
	case MBP_SIGMACLIP:
		ui->modeKS->setChecked(true);
		ui->modeKS->setToolTip(kappaSigmaTip);
		ui->staticKappa->setToolTip(kappaSigmaTip);
		ui->kappa->setToolTip(kappaSigmaTip);

		break;
	case MBP_MEDIANSIGMACLIP:
		ui->modeMKS->setChecked(true);
		ui->modeMKS->setToolTip(medianKappaSigmaTip);
		ui->staticKappa->setToolTip(medianKappaSigmaTip);
		ui->kappa->setToolTip(medianKappaSigmaTip);
		break;
	case MBP_AUTOADAPTIVE:
		ui->modeAAWA->setChecked(true);
		break;
	case MBP_ENTROPYAVERAGE:
		ui->modeEWA->setChecked(true);
		break;
	};

	QString string;
	string = QString::asprintf("%.2f", kappa);
	ui->kappa->setText(string);
	string = QString::asprintf("%ld", iteration);
	return *this;
}

void	StackingParameters::on_backgroundCalibration_clicked()
{
	
}

void StackingParameters::setBackgroundCalibration(BACKGROUNDCALIBRATIONMODE mode)
{
	switch (mode)
	{
	case BCM_NONE:
		workspace->setValue("Stacking/BackgroundCalibration", false);
		workspace->setValue("Stacking/PerChannelBackgroundCalibration", false);
		break;
	case BCM_PERCHANNEL:
		workspace->setValue("Stacking/BackgroundCalibration", false);
		workspace->setValue("Stacking/PerChannelBackgroundCalibration", true);
		break;
	case BCM_RGB:
		workspace->setValue("Stacking/BackgroundCalibration", true);
		workspace->setValue("Stacking/PerChannelBackgroundCalibration", false);
		break;
	}
}
