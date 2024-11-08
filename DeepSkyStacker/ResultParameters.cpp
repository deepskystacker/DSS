#include "stdafx.h"
#include "ResultParameters.h"
#include "ui/ui_ResultParameters.h"
#include "Workspace.h"
#include "zexcept.h"
#include "DSSCommon.h"
#include "StackSettings.h"
#include "StackingTasks.h"

namespace DSS
{
	ResultParameters::ResultParameters(QWidget* parent) :
		QWidget(parent),
		ui(new Ui::ResultParameters),
		workspace{ std::make_unique<Workspace>() },
		pStackingTasks{ nullptr },
		customRectEnabled{ false }
	{
		ui->setupUi(this);
	}

	ResultParameters::~ResultParameters()
	{
		delete ui;
	}

	void ResultParameters::setStackingTasks(CAllStackingTasks* pTasks)
	{
		pStackingTasks = pTasks;
		customRectEnabled = pTasks->getCustomRectangle(customRect);
	}

	void ResultParameters::onSetActive()
	{
		STACKINGMODE stackingMode{ static_cast<STACKINGMODE>(workspace->value("Stacking/Mosaic", uint(0)).toUInt()) };

		//
		// Initially set the Custom Rectangle radio buttion to disabled - it should only be enabled if
		// a custom rectangle has been defined.
		//
		ui->customMode->setEnabled(false);

		//
		// Custom rectangle mode is actually Normal Mode but with a custom rectangle
		// selected.
		//
		// If this has been done, enable the custom rectangle stacking mode
		// If the custom rectangle is also enabled (as it will be initially)
		// then set stacking mode to Custom Rectangle
		//
		if (!customRect.isEmpty())
		{
			ui->customMode->setEnabled(true);
			if (customRectEnabled) stackingMode = SM_CUSTOM;
		}


		//
		// select the appropriate check box for stacking mode
		//
		switch (stackingMode)
		{
		case SM_NORMAL:
			ui->normalMode->setChecked(true);
			if (normalPix.isNull())
			{
				normalPix.load(":/stacking/normalmode.bmp");
			}
			ui->previewImage->setPixmap(normalPix);
			ui->modeText->setText(tr("The result of the stacking process is framed by the reference light frame.",
				"IDS_STACKINGMODE_NORMAL"));
			break;
		case SM_MOSAIC:
			ui->mosaicMode->setChecked(true);
			if (mosaicPix.isNull())
			{
				mosaicPix.load(":/stacking/mosaicmode.bmp");
			}
			ui->previewImage->setPixmap(mosaicPix);
			ui->modeText->setText(tr("The result of the stacking process contains all the light frames of the stack.",
				"IDS_STACKINGMODE_MOSAIC"));
			break;
		case SM_INTERSECTION:
			ui->intersectionMode->setChecked(true);
			if (intersectionPix.isNull())
			{
				intersectionPix.load(":/stacking/intersectionmode.bmp");
			}
			ui->previewImage->setPixmap(intersectionPix);
			ui->modeText->setText(tr("The result of the stacking process is framed by the intersection of all the frames.",
				"IDS_STACKINGMODE_INTERSECTION"));
			break;
		case SM_CUSTOM:
			ui->customMode->setChecked(true);
			if (customPix.isNull())
			{
				customPix.load(":/stacking/custommode.bmp");
			}
			ui->previewImage->setPixmap(customPix);
			ui->modeText->setText("");
			break;
		default:
			break;
		}

		uint drizzle = workspace->value("Stacking/PixelSizeMultiplier", uint(0)).toUInt();
		switch (drizzle)
		{
		case 2:
			ui->drizzle2x->setChecked(true);
			break;
		case 3:
			ui->drizzle3x->setChecked(true);
			break;
		default:
			break;
		}

		bool alignRGB = workspace->value("Stacking/AlignChannels", false).toBool();
		ui->alignRGB->setChecked(alignRGB);
	}

	void	ResultParameters::on_normalMode_clicked()
	{
		if (pStackingTasks) pStackingTasks->enableCustomRect(false);
		workspace->setValue("Stacking/Mosaic", (uint)SM_NORMAL);
		if (normalPix.isNull())
		{
			normalPix.load(":/stacking/normalmode.bmp");
		}
		ui->previewImage->setPixmap(normalPix);
		ui->modeText->setText(tr("The result of the stacking process is framed by the reference light frame.",
			"IDS_STACKINGMODE_NORMAL"));
	}

	void	ResultParameters::on_mosaicMode_clicked()
	{
		if (pStackingTasks) pStackingTasks->enableCustomRect(false);
		workspace->setValue("Stacking/Mosaic", (uint)SM_MOSAIC);
		if (mosaicPix.isNull())
		{
			mosaicPix.load(":/stacking/mosaicmode.bmp");
		}
		ui->previewImage->setPixmap(mosaicPix);
		ui->modeText->setText(tr("The result of the stacking process contains all the light frames of the stack.",
			"IDS_STACKINGMODE_MOSAIC"));

	}

	void	ResultParameters::on_intersectionMode_clicked()
	{
		if (pStackingTasks) pStackingTasks->enableCustomRect(false);
		workspace->setValue("Stacking/Mosaic", (uint)SM_INTERSECTION);
		if (intersectionPix.isNull())
		{
			intersectionPix.load(":/stacking/intersectionmode.bmp");
		}
		ui->previewImage->setPixmap(intersectionPix);
		ui->modeText->setText(tr("The result of the stacking process is framed by the intersection of all the frames.",
			"IDS_STACKINGMODE_INTERSECTION"));
	}

	void	ResultParameters::on_customMode_clicked()
	{
		ZASSERT(nullptr != pStackingTasks);
		pStackingTasks->enableCustomRect();
		// Note well: DO NOT set workspace value "Stacking/Mosaic" to SM_CUSTOM
		if (customPix.isNull())
		{
			customPix.load(":/stacking/custommode.bmp");
		}
		ui->previewImage->setPixmap(customPix);
		ui->modeText->setText("");
	}

	void	ResultParameters::on_drizzle2x_clicked()
	{
		if (ui->drizzle2x->isChecked())
		{
			ui->drizzle3x->setChecked(false);
			workspace->setValue("Stacking/PixelSizeMultiplier", uint(2));
		}
		else
			workspace->setValue("Stacking/PixelSizeMultiplier", uint(1));
	}

	void	ResultParameters::on_drizzle3x_clicked()
	{
		if (ui->drizzle3x->isChecked())
		{
			ui->drizzle2x->setChecked(false);
			workspace->setValue("Stacking/PixelSizeMultiplier", uint(3));
		}
		else
			workspace->setValue("Stacking/PixelSizeMultiplier", uint(1));
	}

	void	ResultParameters::on_alignRGB_clicked()
	{
		if (ui->alignRGB->isChecked())
		{
			workspace->setValue("Stacking/AlignChannels", true);
		}
		else
		{
			workspace->setValue("Stacking/AlignChannels", false);
		}
	}
}