#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include "StackSettings.h"
#include "ui/ui_StackSettings.h"
#include "Workspace.h"

StackSettings::StackSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StackSettings),
	pStackingTasks(nullptr)
{
    ui->setupUi(this);
    m_resultParameters = new ResultParameters();
    m_cometStacking = new CometStacking();
    m_alignmentParameters = new AlignmentParameters();
    m_intermediateFiles = new IntermediateFiles();
    m_postCalibration = new PostCalibration();
    m_outputTab = new OutputTab();

    m_lightFrames = new StackingParameters();
	m_lightFrames->init(PICTURETYPE_LIGHTFRAME);

    m_darkFrames = new StackingParameters();
	m_darkFrames->init(PICTURETYPE_DARKFRAME);

    m_flatFrames = new StackingParameters();
	m_flatFrames->init(PICTURETYPE_FLATFRAME);

    m_biasFrames = new StackingParameters();
	m_biasFrames->init(PICTURETYPE_OFFSETFRAME);

	resultTab = ui->tabWidget->addTab(m_resultParameters, m_resultParameters->windowTitle());
	cometTab = ui->tabWidget->addTab(m_cometStacking, m_cometStacking->windowTitle());;
	lightTab = ui->tabWidget->addTab(m_lightFrames, tr("Light"));;
	darkTab = ui->tabWidget->addTab(m_darkFrames, tr("Dark"));;
	flatTab = ui->tabWidget->addTab(m_flatFrames, tr("Flat"));;
	biasTab = ui->tabWidget->addTab(m_biasFrames, tr("Bias/Offset"));
	alignmentTab = ui->tabWidget->addTab(m_alignmentParameters, m_alignmentParameters->windowTitle());
	intermediateTab = ui->tabWidget->addTab(m_intermediateFiles, m_intermediateFiles->windowTitle());
	postCalibrationTab = ui->tabWidget->addTab(m_postCalibration, m_postCalibration->windowTitle());;
	outputTab  = ui->tabWidget->addTab(m_outputTab, m_outputTab->windowTitle());
	
	ui->tabWidget->setTabEnabled(resultTab, false); 
	ui->tabWidget->setTabEnabled(cometTab, false);
	ui->tabWidget->setTabEnabled(darkTab, false);
	ui->tabWidget->setTabEnabled(flatTab, false);
	ui->tabWidget->setTabEnabled(biasTab, false);
	ui->tabWidget->setTabEnabled(alignmentTab, false);
	ui->tabWidget->setTabEnabled(intermediateTab, false);
	ui->tabWidget->setTabEnabled(postCalibrationTab, false);
	ui->tabWidget->setTabEnabled(outputTab, false);

	// m_tabIntermediate.SetRegisteringOnly(registeringOnly);
	if (registeringOnly)
	{
		if (m_bEnableDark || enableAll)
			ui->tabWidget->setTabEnabled(darkTab, true);
		if (m_bEnableFlat || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		if (m_bEnableBias || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		ui->tabWidget->setTabEnabled(intermediateTab, false);
	}
	else
	{
		ui->tabWidget->setTabEnabled(resultTab, true);
		if (m_bEnableCometStacking || enableAll)
			ui->tabWidget->setTabEnabled(cometTab, true);
		if (m_bEnableDark || enableAll)
			ui->tabWidget->setTabEnabled(darkTab, true);
		if (m_bEnableFlat || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		if (m_bEnableBias || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		ui->tabWidget->setTabEnabled(alignmentTab, true);
		ui->tabWidget->setTabEnabled(postCalibrationTab, true);
		ui->tabWidget->setTabEnabled(outputTab, true);
	};

	// Init controls
	CWorkspace			workspace;
	DWORD				lIteration;
	DWORD				dwBackgroundCalibration = 1;
	DWORD				dwPerChannelBackgroundCalibration = 0;
	bool				fDarkOptimization;
	bool				fDarkFactor;
	QString				strDarkFactor = "1.0";
	bool				fHotPixels;
	bool				fBadColumns = 0;
	DWORD				dwMosaic = 0;
	bool				fCreateIntermediates;
	bool				fSaveCalibrated;
	bool				fSaveDebayered = 0;
	DWORD				dwSaveFormat = 1;
	DWORD				dwAlignment = 0;
	DWORD				dwDrizzle = 1;
	bool				fAlignChannels = 0;
	DWORD				dwCometStackingMode = 0;
	bool				fDebloom = false;
	double				fKappa;

	if (enableAll || !registeringOnly)
	{




		if (cometStacking || enableAll)
		{
			dwCometStackingMode = workspace.value("Stacking/CometStackingMode", (uint)CSM_STANDARD).toUInt();
			m_tabComet.SetCometStackingMode((COMETSTACKINGMODE)dwCometStackingMode);
		};

		CPostCalibrationSettings		PCSettings;

		CAllStackingTasks::GetPostCalibrationSettings(PCSettings);
		m_tabPostCalibration.SetPostCalibration(PCSettings);
		m_tabPostCalibration.setStackingTasks(pStackingTasks);

		COutputSettings					OutputSettings;

		CAllStackingTasks::GetOutputSettings(OutputSettings);
		m_tabOutput.SetOutputSettings(OutputSettings);
	};

	dwSaveFormat = workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();
	m_tabIntermediate.SetFileFormat((INTERMEDIATEFILEFORMAT)dwSaveFormat);

	fCreateIntermediates = workspace.value("Stacking/CreateIntermediates", false).toBool();
	m_tabIntermediate.SetCreateIntermediates(fCreateIntermediates);

	fSaveCalibrated = workspace.value("Stacking/SaveCalibrated", false).toBool();
	m_tabIntermediate.SetSaveCalibrated(fSaveCalibrated);

	fSaveDebayered = workspace.value("Stacking/SaveCalibratedDebayered", false).toBool();
	m_tabIntermediate.SetSaveDebayered(fSaveDebayered);


	if (enableAll || !registeringOnly)
	{
		dwAlignment = workspace.value("Stacking/AlignmentTransformation", 0).toUInt();
		m_tabAlignment.SetAlignment(dwAlignment);
	};


}

StackSettings::~StackSettings()
{
    delete ui;
    delete m_ResultParameters;
    delete m_CometStacking;
    delete m_IntermediateFiles;
    delete m_PostCalibration;
    delete m_OutputTab;
}
