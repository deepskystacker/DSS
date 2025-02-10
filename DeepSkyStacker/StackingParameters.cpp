#include "stdafx.h"
#include "StackingParameters.h"
#include "ui/ui_StackingParameters.h"
#include "Workspace.h"
#include "StackSettings.h"
#include "zexcept.h"
#include "StackingTasks.h"
#include "BackgroundOptions.h"

namespace DSS
{
	StackingParameters::StackingParameters(QWidget* parent, PICTURETYPE theType) :
		QWidget(parent),
		ui(new Ui::StackingParameters),
		workspace(std::make_unique<Workspace>()),
		pStackSettings(dynamic_cast<StackSettings*>(parent)),
		type(theType),
		nobgCalString(tr("No Background Calibration", "ID_CALIBRATIONMENU_NOBACKGROUNDCALIBRATION")),
		pcbgCalString(tr("Per Channel Background Calibration", "ID_CALIBRATIONMENU_PERCHANNELBACKGROUNDCALIBRATION")),
		rgbbgCalString(tr("RGB Channels Background Calibration", "ID_CALIBRATIONMENU_RGBBACKGROUNDCALIBRATION"))
	{
		if (nullptr == pStackSettings)
		{
			delete ui;
			ZASSERTSTATE(nullptr != pStackSettings);
		}

		ui->setupUi(this);

		//connect(ui->backgroundCalibration, &QPushButton::linkActivated, this, &StackingParameters::on_backgroundCalibration_linkActivated);
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
		// Set up the validators for the input fields
		//
		darkFactorValidator = new QDoubleValidator(0.0, 5.0, 4, this);
		iterationValidator = new QRegularExpressionValidator(QRegularExpression("([1-9]|1[0-9]|2[0])"), ui->iterations);
		ui->iterations->setValidator(iterationValidator);
		kappaValidator = new QDoubleValidator(0.0, 5.0, 2, this);

		//
		// Set the tooltip text
		//
		kappaSigmaTip = tr("The pixels outside the range:\n[Mean-%1*%2, Mean+%1*%2]\n"
			"are iteratively removed.\n\nThe remaining pixels are averaged.",
			"IDS_TOOLTIP_KAPPASIGMA")
			// Greek character kappa (κ) = \xce\xba UTF8, Greek character sigma (σ) = \xcf\x83 UTF8
			.arg("\xce\xba").arg("\xcf\x83");
		ui->modeKS->setToolTip(kappaSigmaTip);
		ui->staticKappa->setToolTip(kappaSigmaTip);
		ui->kappa->setToolTip(kappaSigmaTip);

		medianKappaSigmaTip = tr("The pixels outside the range:\n[Mean-%1*%2, Mean+%1*%2]\n"
			"are iteratively replaced by the median value.\n\nThe pixels are then averaged.",
			"IDS_TOOLTIP_MEDIANKAPPASIGMA")
			// Greek character kappa (κ) = \xce\xba UTF8, Greek character sigma (σ) = \xcf\x83 UTF8
			.arg("\xce\xba").arg("\xcf\x83");
		ui->modeMKS->setToolTip(medianKappaSigmaTip);

		QString text = tr("The <b>weighted average</b> is obtained by\niteratively weighting each pixel\n"
			"based on its deviation from the mean\ncompared to the standard deviation (%1).",
			"IDS_TOOLTIP_AUTOADAPTIVE")
			// Greek character sigma (σ) = \xcf\x83 UTF8
			.arg("\xcf\x83");
		ui->modeAAWA->setToolTip(text);

		createActions().createMenus();

		connect(this, &StackingParameters::methodChanged, this, &StackingParameters::updateControls);
	}

	StackingParameters& StackingParameters::createActions()
	{
		nobgCal = new QAction(nobgCalString, this);
		connect(nobgCal, &QAction::triggered, this,
			[this]() { this->setBackgroundCalibration(BCM_NONE); });
		connect(nobgCal, &QAction::triggered, this,
			[this]() { ui->backgroundCalibration->setText(nobgCalString); });

		pcbgCal = new QAction(pcbgCalString, this);
		connect(pcbgCal, &QAction::triggered, this,
			[this]() { this->setBackgroundCalibration(BCM_PERCHANNEL); });
		connect(pcbgCal, &QAction::triggered, this,
			[this]() { ui->backgroundCalibration->setText(pcbgCalString); });

		rgbbgCal = new QAction(rgbbgCalString, this);
		connect(rgbbgCal, &QAction::triggered, this,
			[this]() { this->setBackgroundCalibration(BCM_RGB); });
		connect(rgbbgCal, &QAction::triggered, this,
			[this]() { ui->backgroundCalibration->setText(rgbbgCalString); });

		bgCalOptions = new QAction(tr("Options...", "ID_CALIBRATIONMENU_OPTIONS"), this);
		connect(bgCalOptions, &QAction::triggered, this, &StackingParameters::backgroundCalibrationOptions);

		return *this;
	}

	StackingParameters& StackingParameters::createMenus()
	{
		QMenu* menu = new QMenu(this);
		menu->addAction(nobgCal);
		menu->addAction(pcbgCal);
		menu->addAction(rgbbgCal);
		menu->addSeparator();
		menu->addAction(bgCalOptions);

		backgroundCalibrationMenu = menu;

		return *this;
	}

	StackingParameters::~StackingParameters()
	{
		delete ui; ui = nullptr;
	}

	void StackingParameters::onSetActive()
	{
		method = MBP_AVERAGE;
		mode = BCM_NONE;
		double value = 0.0;
		bool isChecked;
		QString string;

		switch (type)
		{
		case PICTURETYPE_LIGHTFRAME:
			ui->stackedWidget->setCurrentIndex((int)type - 1);
			// Make the Light frame specific controls visible
			ui->backgroundCalibration->setVisible(true);

			method = static_cast<MULTIBITMAPPROCESSMETHOD>
				(workspace->value("Stacking/Light_Method", (uint)MBP_AVERAGE).toUInt());
			iteration = workspace->value("Stacking/Light_Iteration", (uint)5).toUInt();
			kappa = workspace->value("Stacking/Light_Kappa", 2.0).toDouble();
			setControls();

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

			//
			// Set up the background calibration control
			//
			mode =
				CAllStackingTasks::GetBackgroundCalibrationMode();

			//
			// Set the text colour as for a Hyper-Link
			// 
			ui->backgroundCalibration->setForegroundRole(QPalette::Link);

			//
			// Set the text of the control depending on the calibration mode.
			//
			switch (mode)
			{
			case BCM_NONE:
				string = nobgCalString;
				break;
			case BCM_PERCHANNEL:
				string = pcbgCalString;
				break;
			case BCM_RGB:
				string = rgbbgCalString;
				break;
			}
			ui->backgroundCalibration->setText(string);

			break;


		case PICTURETYPE_DARKFRAME:
			ui->stackedWidget->setCurrentIndex((int)type - 1);

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
			kappa = workspace->value("Stacking/Dark_Kappa", 2.0).toDouble();
			setControls();

			isChecked = workspace->value("Stacking/DarkOptimization", false).toBool();
			ui->darkOptimisation->setChecked(isChecked);

			isChecked = workspace->value("Stacking/UseDarkFactor", false).toBool();
			ui->useDarkFactor->setChecked(isChecked);

			value = workspace->value("Stacking/DarkFactor", 1.0).toDouble();
			ui->darkMultiplicationFactor->setText(QString("%L1").arg(value, 0, 'f', 4));

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
			kappa = workspace->value("Stacking/Flat_Kappa", 2.0).toDouble();
			setControls();

			//
			// Disable Entropy-Weighted Average
			//
			ui->modeEWA->setEnabled(false);
			break;

		case PICTURETYPE_OFFSETFRAME:
			method = static_cast<MULTIBITMAPPROCESSMETHOD>
				(workspace->value("Stacking/Offset_Method", (uint)MBP_AVERAGE).toUInt());
			iteration = workspace->value("Stacking/Offset_Iteration", (uint)5).toUInt();
			kappa = workspace->value("Stacking/Offset_Kappa", 2.0).toDouble();
			setControls();

			//
			// Disable Entropy-Weighted Average
			//
			ui->modeEWA->setEnabled(false);
			break;
		};
		updateControls(method);
	}

	StackingParameters& StackingParameters::setControls()
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
		ui->kappa->setText(QString("%L1").arg(kappa, 0, 'f', 2));
		ui->iterations->setText(QString("%L1").arg(iteration));
		return *this;
	}

	void	StackingParameters::on_backgroundCalibration_clicked()
	{
		//
		// Show the popup menu 
		//
		backgroundCalibrationMenu->exec(QCursor::pos());
	}

	void StackingParameters::setBackgroundCalibration(BACKGROUNDCALIBRATIONMODE rhs)
	{
		mode = rhs;
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

	void StackingParameters::backgroundCalibrationOptions()
	{
		//
		// Show the background calibration options dialog
		// 
		BackgroundOptions dlg(this);

		dlg.SetBackgroundCalibrationMode(mode);
		if (dlg.exec() == QDialog::Accepted)
		{
			setBackgroundCalibration(dlg.GetBackgroundCalibrationMode());
		};
	}

	void StackingParameters::updateControls(MULTIBITMAPPROCESSMETHOD newMethod)
	{
		if (MBP_AUTOADAPTIVE == newMethod)
		{
			ui->staticKappa->setEnabled(false);
			ui->kappa->setEnabled(false);
			ui->staticIterations->setEnabled(true);
			ui->iterations->setEnabled(true);
		}
		else if (MBP_SIGMACLIP == newMethod || MBP_MEDIANSIGMACLIP == newMethod)
		{
			ui->staticKappa->setEnabled(true);
			ui->kappa->setEnabled(true);;
			ui->staticIterations->setEnabled(true);
			ui->iterations->setEnabled(true);
		}
		else
		{
			ui->staticKappa->setEnabled(false);
			ui->kappa->setEnabled(false);
			ui->staticIterations->setEnabled(false);
			ui->iterations->setEnabled(false);
		}
	}

	void StackingParameters::setMethod(MULTIBITMAPPROCESSMETHOD rhs)
	{
		if (method != rhs)
		{
			method = rhs;
			switch (type)
			{
			case PICTURETYPE_LIGHTFRAME:
				workspace->setValue("Stacking/Light_Method", (uint)method);
				break;
			case PICTURETYPE_DARKFRAME:
				workspace->setValue("Stacking/Dark_Method", (uint)method);
				break;
			case PICTURETYPE_FLATFRAME:
				workspace->setValue("Stacking/Flat_Method", (uint)method);
				break;
			case PICTURETYPE_OFFSETFRAME:
				workspace->setValue("Stacking/Offset_Method", (uint)method);
				break;
			}
			emit methodChanged(method);
		}
	}

	void StackingParameters::on_modeAverage_clicked()
	{
		setMethod(MBP_AVERAGE);
	}

	void StackingParameters::on_modeMedian_clicked()
	{
		setMethod(MBP_MEDIAN);
	}

	void StackingParameters::on_modeKS_clicked()
	{
		setMethod(MBP_SIGMACLIP);
	}

	void StackingParameters::on_modeMKS_clicked()
	{
		setMethod(MBP_MEDIANSIGMACLIP);
	}

	void StackingParameters::on_modeAAWA_clicked()
	{
		setMethod(MBP_AUTOADAPTIVE);
	}

	void StackingParameters::on_modeEWA_clicked()
	{
		setMethod(MBP_ENTROPYAVERAGE);
	}

	void StackingParameters::on_modeMaximum_clicked()
	{
		setMethod(MBP_MAXIMUM);
	}

	void StackingParameters::on_debloom_stateChanged(int state)
	{
		//
		// If it's checked then set Debloom to true
		//
		workspace->setValue("Stacking/Debloom", (Qt::Checked == state) ? true : false);
	}

	void StackingParameters::on_hotPixels_stateChanged(int state)
	{
		//
		// If it's checked then set Hot Pixel detection to true
		//
		workspace->setValue("Stacking/HotPixelsDetection", (Qt::Checked == state) ? true : false);
	}

	void StackingParameters::on_badColumns_stateChanged(int state)
	{
		//
		// If it's checked then set Bad Column detection to true
		//
		workspace->setValue("Stacking/BadLinesDetection", (Qt::Checked == state) ? true : false);
	}

	void StackingParameters::on_darkOptimisation_stateChanged(int state)
	{
		//
		// If it's checked then set Dark Optimisation to true
		//
		workspace->setValue("Stacking/DarkOptimization", (Qt::Checked == state) ? true : false);
	}

	void StackingParameters::on_useDarkFactor_stateChanged(int state)
	{
		//
		// If it's checked then want to use a user specified value for Dark Factor
		//
		workspace->setValue("Stacking/UseDarkFactor", (Qt::Checked == state) ? true : false);
	}

	void StackingParameters::on_darkMultiplicationFactor_editingFinished()
	{
		QLocale locale;
		QString string{ ui->darkMultiplicationFactor->text() };
		int unused{ 0 };
		bool OK = false;
		if (QValidator::Acceptable == darkFactorValidator->validate(string, unused))
		{
			const double value{ locale.toDouble(string, &OK) };
			if (OK) workspace->setValue("Stacking/DarkFactor", value);
		}
		else
		{
			QApplication::beep();
			const double value = workspace->value("Stacking/DarkFactor", 1.0).toDouble();
			ui->darkMultiplicationFactor->setText(QString("%L1").arg(value, 0, 'f', 4));
		}
	}

	void StackingParameters::on_iterations_editingFinished()
	{
		bool convertedOK{ false };
		uint value = ui->iterations->text().toUInt(&convertedOK);

		if (convertedOK)
		{
			switch (type)
			{
			case PICTURETYPE_LIGHTFRAME:
				workspace->setValue("Stacking/Light_Iteration", value);
				break;
			case PICTURETYPE_DARKFRAME:
				workspace->setValue("Stacking/Dark_Iteration", value);
				break;
			case PICTURETYPE_FLATFRAME:
				workspace->setValue("Stacking/Flat_Iteration", value);
				break;
			case PICTURETYPE_OFFSETFRAME:
				workspace->setValue("Stacking/Offset_Iteration", value);
				break;
			}
		}
	}

	void StackingParameters::on_kappa_editingFinished()
	{
		QLocale locale;
		QString string{ ui->kappa->text() };
		int unused{ 0 };
		double temp{ 0.0 };
		bool OK = false;
		if (QValidator::Acceptable == kappaValidator->validate(string, unused))
		{
			temp = locale.toDouble(string, &OK);
			if (OK)
			{
				kappa = temp;
				switch (type)
				{
				case PICTURETYPE_LIGHTFRAME:
					workspace->setValue("Stacking/Light_Kappa", kappa);
					break;
				case PICTURETYPE_DARKFRAME:
					workspace->setValue("Stacking/Dark_Kappa", kappa);
					break;
				case PICTURETYPE_FLATFRAME:
					workspace->setValue("Stacking/Flat_Kappa", kappa);
					break;
				case PICTURETYPE_OFFSETFRAME:
					workspace->setValue("Stacking/Offset_Kappa", kappa);
					break;
				}
			}
		}
		else
		{
			QApplication::beep();
			switch (type)
			{
			case PICTURETYPE_LIGHTFRAME:
				kappa = workspace->value("Stacking/Light_Kappa", 2.0).toDouble();
				break;
			case PICTURETYPE_DARKFRAME:
				kappa = workspace->value("Stacking/Dark_Kappa", 2.0).toDouble();
				break;
			case PICTURETYPE_FLATFRAME:
				kappa = workspace->value("Stacking/Flat_Kappa", 2.0).toDouble();
				break;
			case PICTURETYPE_OFFSETFRAME:
				kappa = workspace->value("Stacking/Offset_Kappa", 2.0).toDouble();
			}
			ui->kappa->setText(QString("%L1").arg(kappa, 0, 'f', 2));
		}
	}
}