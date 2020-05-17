#ifndef STACKSETTINGS_H
#define STACKSETTINGS_H

#include <QDialog>
#include "ResultParameters.h"
#include "CometStacking.h"
#include "PostCalibration.h"
#include "OutputTab.h"
#include "IntermediateFiles.h"
#include "AlignmentParameters.h"
#include "StackingParameters.h"
#include "StackingTasks.h"

namespace Ui {
class StackSettings;
}

class StackSettings : public QDialog
{
    Q_OBJECT

public:
    explicit StackSettings(QWidget *parent = nullptr);
    ~StackSettings();

	StackSettings & SetStartingTab(LONG lStartingTab)
	{
		m_lStartingTab = lStartingTab;
		return *this;
	};

	StackSettings & SetRegisteringOnly(bool bRegisteringOnly)
	{
		m_bRegisteringOnly = bRegisteringOnly;
		return *this;
	};

	StackSettings & EnableCometStacking(bool bEnable)
	{
		m_bEnableCometStacking = bEnable;
		return *this;
	};

	StackSettings & SetStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
		return *this;
	};

	StackSettings & SetDarkFlatBiasTabsVisibility(bool bDark, bool bFlat, bool bBias)
	{
		m_bEnableDark = bDark;
		m_bEnableFlat = bFlat;
		m_bEnableBias = bBias;
		return *this;
	};

	StackSettings & SetEnableAll(bool bEnableAll)
	{
		m_bEnableAll = bEnableAll;
		return *this;
	};


	StackSettings & enableCustomRectangle(bool value)
	{
		customRectangleEnabled = value;
		return *this;
	}

	bool isCustomRectangleEnabled()
	{
		return customRectangleEnabled;
	}

	bool		useCustomRectangle()
	{
		return customRectangleEnabled && customRectangleAvailable;
	};


private:
    Ui::StackSettings *ui;
    ResultParameters * m_resultParameters;
    CometStacking * m_cometStacking;
    AlignmentParameters * m_alignmentParameters;
    IntermediateFiles * m_intermediateFiles;
    PostCalibration * m_postCalibration;
    OutputTab * m_outputTab;
    StackingParameters * m_lightFrames;
    StackingParameters * m_darkFrames;
    StackingParameters * m_flatFrames;
	StackingParameters * m_biasFrames;

	int	resultTab;
	int cometTab;
	int lightTab;
	int darkTab;
	int flatTab;
	int biasTab;
	int alignmentTab;
	int intermediateTab;
	int postCalibrationTab;
	int outputTab;

	LONG					m_lStartingTab;
	bool					m_bRegisteringOnly;
	bool					m_bEnableCometStacking;
	bool					m_bEnableDark;
	bool					m_bEnableFlat;
	bool					m_bEnableBias;
	bool					m_bEnableAll;
	bool					customRectangleAvailable;
	bool					customRectangleEnabled;
	CAllStackingTasks *		m_pStackingTasks;

};
#endif // STACKSETTINGS_H
