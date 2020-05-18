#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include "CometStacking.h"
#include "ui/ui_CometStacking.h"

#include "DSSCommon.h"
#include "Workspace.h"

CometStacking::CometStacking(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CometStacking),
	workspace(new CWorkspace())
{
    ui->setupUi(this);

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
}

CometStacking::~CometStacking()
{
    delete ui;
}

void CometStacking::onBnClickedStandardStacking(bool on)
{
    if (on) {
        setCometStackingMode(CSM_STANDARD);
    }
}

void CometStacking::onBnClickedCometStacking(bool on)
{
    if (on) {
        setCometStackingMode(CSM_COMETONLY);
    }
}

void CometStacking::onBnClickedAdvancedStacking(bool on)
{
    if (on) {
        setCometStackingMode(CSM_COMETSTAR);
    }
}

void CometStacking::updateImage()
{
    if (m_CometStackingMode == CSM_STANDARD)
    {
        QPixmap pm(":/comet/normal.bmp");
        ui->laComet->setPixmap(pm);
    }
    else if (m_CometStackingMode == CSM_COMETONLY)
    {
        QPixmap pm(":/comet/trails.bmp");
        ui->laComet->setPixmap(pm);
    }
    else 
    {
        QPixmap pm(":/comet/freeze.bmp");
        ui->laComet->setPixmap(pm);
    }
}