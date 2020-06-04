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
	friend class StackingParameters;
	friend class PostCalibration;

	typedef QDialog
		Inherited;

	Q_OBJECT

public:
    explicit StackSettings(QWidget *parent = nullptr);
    ~StackSettings();

	void accept() override;
	void reject() override;

	inline StackSettings & setStartingTab(LONG lStartingTab) noexcept
	{
		startingTab = lStartingTab;
		return *this;
	};

	inline StackSettings & setRegisteringOnly(bool bRegisteringOnly) noexcept
	{
		registeringOnly = bRegisteringOnly;
		updateControls();
		return *this;
	};

	inline StackSettings & enableCometStacking(bool value) noexcept
	{
		cometStacking = value;
		updateControls();
		return *this;
	};

	inline StackSettings & setStackingTasks(CAllStackingTasks * tasks) noexcept
	{
		pStackingTasks = tasks;
		return *this;
	};

	inline StackSettings & setTabVisibility(bool bDark, bool bFlat, bool bBias) noexcept
	{
		enableDark = bDark;
		enableFlat = bFlat;
		enableBias = bBias;
		updateControls();
		return *this;
	};

	inline StackSettings & setEnableAll(bool value) noexcept
	{
		enableAll = value;
		updateControls();
		return *this;
	};

	inline StackSettings & enableCustomRectangle(bool value) noexcept
	{
		customRectangleEnabled = value;
		return *this;
	}

	inline StackSettings & selectCustomRectangle(bool value) noexcept
	{
		customRectangleSelected = value;
		return *this;
	}

	inline bool isCustomRectangleEnabled() noexcept
	{
		return customRectangleEnabled;
	}

	inline bool isCustomRectangleSelected() noexcept
	{
		return customRectangleSelected;
	}

	inline bool	useCustomRectangle() noexcept
	{
		return (customRectangleEnabled && customRectangleSelected);
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

	void updateControls();

	LONG					startingTab;
	bool					registeringOnly;
	bool					cometStacking;
	bool					enableDark;
	bool					enableFlat;
	bool					enableBias;
	bool					enableAll;
	bool					customRectangleSelected;
	bool					customRectangleEnabled;
	CAllStackingTasks *		pStackingTasks;

private slots:
	void tabChanged(int tab);
};
#endif // STACKSETTINGS_H
