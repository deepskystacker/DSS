#include "stdafx.h"
#include "PostCalibration.h"
#include "ui/ui_PostCalibration.h"
#include "Workspace.h"
#include "StackSettings.h"
#include "zexcept.h"
#include "ztrace.h"
#include "progressdlg.h"
#include "MasterFrames.h"
#include "BitmapInfo.h"
#include "BitmapExt.h"
#include "CosmeticEngine.h"

extern bool	g_bShowRefStars;

namespace DSS
{
	PostCalibration::PostCalibration(QWidget* parent) :
		QWidget(parent),
		ui(new Ui::PostCalibration),
		workspace(new Workspace()),
		pStackSettings(dynamic_cast<StackSettings*>(parent)),
		medianString(tr("the median", "ID_COSMETICMETHOD_MEDIAN")),
		gaussianString(tr("a gaussian filter", "ID_COSMETICMETHOD_GAUSSIAN"))
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

	PostCalibration& PostCalibration::createActions()
	{
		onMedian = new QAction(medianString, this);
		connect(onMedian, &QAction::triggered, this,
			[this]() { this->setReplacementMethod(CR_MEDIAN); });
		connect(onMedian, &QAction::triggered, this,
			[this]() { ui->replacementMethod->setText(medianString); });

		onGaussian = new QAction(gaussianString, this);
		connect(onGaussian, &QAction::triggered, this,
			[this]() { this->setReplacementMethod(CR_GAUSSIAN); });
		connect(onGaussian, &QAction::triggered, this,
			[this]() { ui->replacementMethod->setText(gaussianString); });

		return *this;
	}

	PostCalibration& PostCalibration::createMenus()
	{
		QMenu* menu = new QMenu(this);
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

		// Use our friendship with StackSettings to get at the stacking tasks pointer
		pStackingTasks = pStackSettings->pStackingTasks;

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
		ui->hotThreshold->setSliderPosition(1000 - pcs.m_fHotDetection * 10.0);

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
		ui->coldThreshold->setSliderPosition(1000 - pcs.m_fColdDetection * 10.0);

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
			ui->testCosmetic->setVisible(true);
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
				ui->testCosmetic->setVisible(true);
			}
			else
			{
				ui->testCosmetic->setVisible(false);
			}
		}
	}

	void PostCalibration::on_hotFilter_valueChanged(int newValue)
	{
		if (pcs.m_lHotFilter != newValue)
		{
			//
			// Value has changed
			//
			pcs.m_lHotFilter = newValue;
			workspace->setValue("Stacking/PCS_HotFilter", newValue);

			//
			// Display the new value
			//
			ui->hotFilterSize->setText(QString("%L1").arg(newValue));
		}
	}

	void PostCalibration::on_hotThreshold_valueChanged(int value)
	{
		double newValue = 100.0 - (double)value / 10.0;
		if (pcs.m_fHotDetection != newValue)
		{
			//
			// Value has changed
			//
			pcs.m_fHotDetection = newValue;
			workspace->setValue("Stacking/PCS_HotDetection", newValue * 10.0);
			//
			// Display the new value
			//
			ui->hotThresholdPercent->setText(QString("%L1%").arg(newValue, 0, 'f', 1));
		}

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
				ui->testCosmetic->setVisible(true);
			}
			else
			{
				ui->testCosmetic->setVisible(false);
			}
		}
	}

	void PostCalibration::on_coldFilter_valueChanged(int newValue)
	{
		if (pcs.m_lColdFilter != newValue)
		{
			//
			// Value has changed
			//
			pcs.m_lColdFilter = newValue;
			workspace->setValue("Stacking/PCS_ColdFilter", newValue);
			//
			// Display the new value
			//
			ui->coldFilterSize->setText(QString("%L1").arg(newValue));
		}
	}

	void PostCalibration::on_coldThreshold_valueChanged(int value)
	{
		double newValue = 100.0 - (double)value / 10.0;
		if (pcs.m_fColdDetection != newValue)
		{
			//
			// Value has changed
			//
			pcs.m_fColdDetection = newValue;
			workspace->setValue("Stacking/PCS_ColdDetection", newValue * 10.0);

			//
			// Display the new value
			//
			ui->coldThresholdPercent->setText(QString("%L1%").arg(newValue, 0, 'f', 1));
		}
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

	void PostCalibration::on_replacementMethod_clicked()
	{
		//
		// Show the popup menu 
		//
		replacementMenu->exec(QCursor::pos());
	}

	void PostCalibration::on_testCosmetic_clicked()
	{
		ZFUNCTRACE_RUNTIME();
		// Load the reference light frame
		if (pStackingTasks)
		{
			CAllStackingTasks			tasks = *(pStackingTasks);

			// Retrieve the first light frame
			tasks.ResolveTasks();
			if (tasks.m_vStacks.size())
			{
				DSS::ProgressDlg				dlg;
				CStackingInfo& StackingInfo = tasks.m_vStacks[0];

				if (StackingInfo.m_pLightTask &&
					StackingInfo.m_pLightTask->m_vBitmaps.size())
				{
					// Keep Only the first light frame
					StackingInfo.m_pLightTask->m_vBitmaps.resize(1);
					const fs::path& filePath = StackingInfo.m_pLightTask->m_vBitmaps[0].filePath;

					CMasterFrames	MasterFrames;

					// Disable all the tasks except the one used by StackingInfo
					for (auto& task : tasks.m_vTasks) {
						task.m_bDone = true;
					}
					if (StackingInfo.m_pDarkFlatTask)
						StackingInfo.m_pDarkFlatTask->m_bDone = false;
					if (StackingInfo.m_pOffsetTask)
						StackingInfo.m_pOffsetTask->m_bDone = false;
					if (StackingInfo.m_pDarkTask)
						StackingInfo.m_pDarkTask->m_bDone = false;
					if (StackingInfo.m_pFlatTask)
						StackingInfo.m_pFlatTask->m_bDone = false;
					if (StackingInfo.m_pLightTask)
						StackingInfo.m_pLightTask->m_bDone = false;

					QString strText(QCoreApplication::translate("PostCalibration", "Computing Cosmetic", "IDS_COMPUTINGCOSMETICSTATS"));
					dlg.Start1(strText, 0, false);

					dlg.SetJointProgress(true);
					tasks.DoAllPreTasks(&dlg);
					MasterFrames.LoadMasters(StackingInfo, &dlg);

					// Load the image
					CBitmapInfo		bmpInfo;
					// Load the bitmap
					if (GetPictureInfo(filePath, bmpInfo) && bmpInfo.CanLoad())
					{
						QString	strDescription;
						bmpInfo.GetDescription(strDescription);
						QString name{ QString::fromStdU16String(filePath.generic_u16string()) };
						if (bmpInfo.m_lNrChannels == 3)
							strText = QCoreApplication::translate("PostCalibration", "Loading %1 bit/ch %2 light frame\n%3", "IDS_LOADRGBLIGHT").arg(bmpInfo.m_lBitsPerChannel).arg(strDescription).arg(name);
						else
							strText = QCoreApplication::translate("PostCalibration", "Loading %1 bits gray %2 light frame\n%3", "IDS_LOADGRAYLIGHT").arg(bmpInfo.m_lBitsPerChannel).arg(strDescription).arg(name);
						dlg.Start2(strText, 0);

						std::shared_ptr<CMemoryBitmap> pBitmap;
						std::shared_ptr<QImage> pQImage;
						if (::FetchPicture(filePath, pBitmap, false, &dlg, pQImage))
						{
							// Apply offset, dark and flat to lightframe
							MasterFrames.ApplyAllMasters(pBitmap, nullptr, &dlg);

							// Then simulate the cosmetic on this image
							CCosmeticStats Stats;
							SimulateCosmetic(pBitmap, pcs, Stats, &dlg);

							// Show the results
							const double fHotPct = static_cast<double>(Stats.m_lNrDetectedHotPixels) / Stats.m_lNrTotalPixels * 100.0;
							const double fColdPct = static_cast<double>(Stats.m_lNrDetectedColdPixels) / Stats.m_lNrTotalPixels * 100.0;

							QString message{ tr("Cosmetic\nDetected Hot Pixels: %L1 (%L2%)\nDetected Cold Pixels: %L3 (%L4%)\n",
								"IDS_COSMETICSTATS")
								.arg(Stats.m_lNrDetectedHotPixels)
								.arg(fHotPct, 0, 'f', 2)
								.arg(Stats.m_lNrDetectedColdPixels)
								.arg(fColdPct, 0, 'f', 2)
							};

							QMessageBox::information(this, "DeepSkyStacker", message, QMessageBox::Ok, QMessageBox::Ok);
						}

						dlg.End2();
					}
				}
			}
		}
	}
}