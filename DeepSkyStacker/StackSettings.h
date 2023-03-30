#pragma once
#include "dssrect.h"

namespace Ui {
class StackSettings;
}

class ResultParameters;
class CometStacking;
class AlignmentParameters;
class IntermediateFiles;
class PostCalibration;
class OutputTab;
class StackingParameters;
class CAllStackingTasks;

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

	inline StackSettings & setStartingTab(int lStartingTab) noexcept
	{
		startingTab = lStartingTab;
		return *this;
	};

	inline StackSettings & setRegisteringOnly(bool bRegisteringOnly)
	{
		registeringOnly = bRegisteringOnly;
		updateControls();
		return *this;
	};

	inline StackSettings & enableCometStacking(bool value)
	{
		cometStacking = value;
		updateControls();
		return *this;
	};

	StackSettings & setStackingTasks(CAllStackingTasks * tasks) noexcept;

	inline StackSettings & setTabVisibility(bool bDark, bool bFlat, bool bBias)
	{
		enableDark = bDark;
		enableFlat = bFlat;
		enableBias = bBias;
		updateControls();
		return *this;
	};

	inline StackSettings & setEnableAll(bool value)
	{
		enableAll = value;
		updateControls();
		return *this;
	};

private:
    Ui::StackSettings *ui;
	bool initialised;
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

	int					startingTab;
	bool					registeringOnly;
	bool					cometStacking;
	bool					enableDark;
	bool					enableFlat;
	bool					enableBias;
	bool					enableAll;
	bool					customRectEnabled;
	DSSRect					customRect;
	CAllStackingTasks *		pStackingTasks;

	void showEvent(QShowEvent *event) override;
	void changeEvent(QEvent *event) override;

	void onInitDialog();
	void setCustomRectEnabled(bool value)
	{
		customRectEnabled = value;
	}

private slots:
	void tabChanged(int tab);
	void on_chooseFolder_clicked(bool value);
};
