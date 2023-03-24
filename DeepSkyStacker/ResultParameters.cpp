#include "stdafx.h"
#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <afx.h>

#include <QSettings>

#include <ZExcept.h>
#include <Ztrace.h>

#include "ResultParameters.h"
#include "ui/ui_ResultParameters.h"

#include "DSSCommon.h"
#include "StackSettings.h"
#include "Workspace.h"
#include "DeepSkyStacker.h"


ResultParameters::ResultParameters(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ResultParameters),
	workspace{ std::make_unique<Workspace>() },
	validCustomRectangle { DeepSkyStacker::instance()->getStackingDlg().customRectangleIsValid() }
{
    ui->setupUi(this);
}

ResultParameters::~ResultParameters()
{
    delete ui;
}

void ResultParameters::onSetActive()
{
	STACKINGMODE stackingMode = static_cast<STACKINGMODE>(workspace->value("Stacking/Mosaic", uint(0)).toUInt());

	//
	// Initially set the Custom Rectangle radio buttion to disabled - it should only be enabled if
	// a custom rectangle has been defined.
	//
	ui->customMode->setEnabled(false);

	//
	// Custom rectangle mode is actually Normal Mode but with a custom rectangle
	// selected in this dialog.
	//
	// It is only possible to do this if a custom rectangle has been defined.
	//
	// If this has been done,select and enable the custom rectangle stacking mode
	//
	if (validCustomRectangle)
	{
		ui->customMode->setEnabled(true);
		if (SM_CUSTOM != stackingMode)
		{
			stackingMode = SM_CUSTOM;
			workspace->setValue("Stacking/Mosaic", (uint)SM_CUSTOM);
		}
	}
	else
	{
		if (SM_CUSTOM == stackingMode)
		{
			stackingMode = SM_INTERSECTION;
			workspace->setValue("Stacking/Mosaic", (uint)SM_INTERSECTION);
		}
	}

	//
	// If using custom rectangle mode and StackSettings was opened from the Explorer Bar,
	// then disable all the radio buttons except custom mode
	//
	QObject* parent{ this->parent() };	// QStackedWidget tab of ui->tabWidget in StackSettings
	parent = parent->parent();			// QTabWidget ui->tabWidget in StackSettings
	parent = parent->parent();			// StackSettings
	parent = parent->parent();			// Parent of StackSettings (ExplorerBar, Register Settings, or StackingDlg)
	if (SM_CUSTOM == stackingMode && &(dynamic_cast<QObject&>(DeepSkyStacker::instance()->GetExplorerBar())) == parent)
	{
		ui->normalMode->setEnabled(false);
		ui->mosaicMode->setEnabled(false);
		ui->intersectionMode->setEnabled(false);
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
	workspace->setValue("Stacking/Mosaic", (uint)SM_CUSTOM);
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
