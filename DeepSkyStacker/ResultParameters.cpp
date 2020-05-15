#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include "ResultParameters.h"
#include "ui/ui_ResultParameters.h"

#include <ZExcept.h>
#include <Ztrace.h>

#include "DSSCommon.h"
#include "StackSettings.h"
#include "Workspace.h"


ResultParameters::ResultParameters(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ResultParameters),
	workspace(new CWorkspace()),
	pStackSettings(dynamic_cast<StackSettings *>(parent))
{
	if (nullptr == pStackSettings)
	{
		delete ui;
		ZASSERTSTATE(nullptr != pStackSettings);
	}
	
	//
	// Load up the images 
	//
	normalPic.load("qrc:///stacking/normalmode.bmp");
	mosaicPic.load("qrc:///stacking/mosaicmode.bmp");
	intersectionPic.load("qrc:///stacking/intersectionmode.bmp");
	customPic.load("qrc:///stacking/custommode.bmp");

    ui->setupUi(this);

	//
	// select the appropriate check box for stacking mode
	//
    STACKINGMODE stackingMode = static_cast<STACKINGMODE>(workspace->value("Stacking/Mosaic", uint(0)).toUInt());

	switch (stackingMode)
	{
	case SM_NORMAL:
		//
		// Custom rectangle mode is actually Normal Mode but with a custom 
		// rectangle enabled (not necessarily defined).
		//
		if (pStackSettings->isCustomRectangleEnabled())
		{
			ui->customMode->setChecked(true);
			ui->previewImage->setPicture(customPic);
			ui->modeText->setText("");
		}
		else
		{
			ui->normalMode->setChecked(true);
			ui->previewImage->setPicture(normalPic);
			ui->modeText->setText(tr("The result of the stacking process is framed by the reference light frame."));
		}
		break;
	case SM_MOSAIC:
		ui->mosaicMode->setChecked(true);
		ui->previewImage->setPicture(mosaicPic);
		ui->modeText->setText(tr("The result of the stacking process contains all the light frames of the stack."));
		break;
	case SM_INTERSECTION:
		ui->intersectionMode->setChecked(true);
		ui->previewImage->setPicture(intersectionPic);
		ui->modeText->setText(tr("The result of the stacking process is framed by the intersection of all the frames."));
		break;
	//
	// SM_CUSTOM isn't used here
	//
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

ResultParameters::~ResultParameters()
{
    delete ui;
}

void	ResultParameters::on_normalMode_clicked()
{
	workspace->setValue("Stacking/Mosaic", (uint)SM_NORMAL);
	pStackSettings->enableCustomRectangle(false);
}

void	ResultParameters::on_mosaicMode_clicked()
{
	workspace->setValue("Stacking/Mosaic", (uint)SM_MOSAIC);
	pStackSettings->enableCustomRectangle(false);
}

void	ResultParameters::on_intersectionMode_clicked()
{
	workspace->setValue("Stacking/Mosaic", (uint)SM_INTERSECTION);
	pStackSettings->enableCustomRectangle(false);
}

void	ResultParameters::on_customMode_clicked()
{
	//
	// SM_CUSTOM isn't used - instead we use normal stacking mode
	// and record the fact that a custom rectangle is being used
	// by calling a method on our parent dialog
	//
	workspace->setValue("Stacking/Mosaic", (uint)SM_NORMAL);
	pStackSettings->enableCustomRectangle(true);
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
