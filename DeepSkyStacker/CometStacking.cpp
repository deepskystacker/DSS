#include "stdafx.h"
#include "CometStacking.h"
#include "ui/ui_CometStacking.h"
#include "Workspace.h"

CometStacking::CometStacking(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CometStacking),
	workspace(new Workspace())
{
    ui->setupUi(this);
}

CometStacking::~CometStacking()
{
    delete ui;
}

void CometStacking::onSetActive()
{
	m_CometStackingMode = static_cast<COMETSTACKINGMODE>
		(workspace->value("Stacking/CometStackingMode", (uint)CSM_STANDARD).toUInt());

	switch (m_CometStackingMode)
	{
	case CSM_STANDARD:
		ui->modeStandard->setChecked(true);
		break;
	case CSM_COMETONLY:
		ui->modeComet->setChecked(true);
		break;
	case CSM_COMETSTAR:
		ui->modeAdvanced->setChecked(true);
		break;
	}
	updateImage();
}

void CometStacking::setCometStackingMode(COMETSTACKINGMODE mode)
{
	if (mode != m_CometStackingMode)
	{
		m_CometStackingMode = mode;
		workspace->setValue("Stacking/CometStackingMode", static_cast<uint>(mode));
		updateImage();
	}
}

void CometStacking::on_modeStandard_clicked()
{
	setCometStackingMode(CSM_STANDARD);
}

void CometStacking::on_modeComet_clicked()
{
	setCometStackingMode(CSM_COMETONLY);
}

void CometStacking::on_modeAdvanced_clicked()
{
	setCometStackingMode(CSM_COMETSTAR);
}

void CometStacking::updateImage()
{
    if (m_CometStackingMode == CSM_STANDARD)
    {
		if (standardPix.isNull())
		{
			standardPix.load(":/comet/normal.bmp");
		}
		ui->laComet->setPixmap(standardPix);
	}
    else if (m_CometStackingMode == CSM_COMETONLY)
    {
		if (cometPix.isNull())
		{
			cometPix.load(":/comet/trails.bmp");
		}
		ui->laComet->setPixmap(cometPix);
	}
    else 
    {
		if (advancedPix.isNull())
		{
			advancedPix.load(":/comet/freeze.bmp");
		}
		ui->laComet->setPixmap(advancedPix);
	}
}