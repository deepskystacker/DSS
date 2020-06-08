#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include <QFileDialog>
#include <QSettings>
#include <QShowEvent>

#include <ZExcept.h>
#include <Ztrace.h>

#include "StackSettings.h"
#include "ui/ui_StackSettings.h"
#include "Workspace.h"

StackSettings::StackSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StackSettings),
	initialised(false),
	pStackingTasks(nullptr),
	registeringOnly(false),
	cometStacking(false),
	enableDark(false),
	enableFlat(false),
	enableBias(false),
	enableAll(false),
	customRectangleSelected(false),
	customRectangleEnabled(false)
{
    ui->setupUi(this);

	setWindowTitle(tr("Stacking Settings"));

	m_resultParameters = new ResultParameters(this);
	m_cometStacking = new CometStacking(this);
	m_alignmentParameters = new AlignmentParameters(this);
	m_intermediateFiles = new IntermediateFiles(this, registeringOnly);
	m_postCalibration = new PostCalibration(this);
	m_outputTab = new OutputTab(this);

	m_lightFrames = new StackingParameters(this, PICTURETYPE_LIGHTFRAME);
	m_darkFrames = new StackingParameters(this, PICTURETYPE_DARKFRAME);
	m_flatFrames = new StackingParameters(this, PICTURETYPE_FLATFRAME);
	m_biasFrames = new StackingParameters(this, PICTURETYPE_OFFSETFRAME);

	resultTab = ui->tabWidget->addTab(m_resultParameters, m_resultParameters->windowTitle());
	cometTab = ui->tabWidget->addTab(m_cometStacking, m_cometStacking->windowTitle());;
	lightTab = ui->tabWidget->addTab(m_lightFrames, tr("Light"));;
	darkTab = ui->tabWidget->addTab(m_darkFrames, tr("Dark"));;
	flatTab = ui->tabWidget->addTab(m_flatFrames, tr("Flat"));;
	biasTab = ui->tabWidget->addTab(m_biasFrames, tr("Bias/Offset"));
	alignmentTab = ui->tabWidget->addTab(m_alignmentParameters, m_alignmentParameters->windowTitle());
	intermediateTab = ui->tabWidget->addTab(m_intermediateFiles, m_intermediateFiles->windowTitle());
	postCalibrationTab = ui->tabWidget->addTab(m_postCalibration, m_postCalibration->windowTitle());;
	outputTab = ui->tabWidget->addTab(m_outputTab, m_outputTab->windowTitle());

	ui->tabWidget->setTabEnabled(resultTab, false);
	ui->tabWidget->setTabEnabled(cometTab, false);
	ui->tabWidget->setTabEnabled(darkTab, false);
	ui->tabWidget->setTabEnabled(flatTab, false);
	ui->tabWidget->setTabEnabled(biasTab, false);
	ui->tabWidget->setTabEnabled(alignmentTab, false);
	ui->tabWidget->setTabEnabled(intermediateTab, false);
	ui->tabWidget->setTabEnabled(postCalibrationTab, false);
	ui->tabWidget->setTabEnabled(outputTab, false);

	//
	// If the user selects a tab we want to know.
	//
	connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	CWorkspace workspace;
	workspace.Push();
}

StackSettings::~StackSettings()
{
    delete ui;
}

void StackSettings::onInitDialog()
{
	//
	// Get number of processors we're allowed to use.   Normally this is the number of
	// real cores available, but this can artificially be limited by setting  
	// "MaxProcessors" to 1 in the application settings (registry or ini file).
	//
	// If this is done the the number used will be min("MaxProcessors", cores)
	//
	ui->reducePriority->setChecked(CMultitask::GetReducedThreadsPriority());

	if (CMultitask::GetNrProcessors(true) > 1)
	{
		ui->useAllProcessors->setChecked(CMultitask::GetNrProcessors() > 1);
	}
	else
		ui->useAllProcessors->setDisabled(true);

	//
	// Get the temporary files folder
	//
	QString folder;
	CAllStackingTasks::GetTemporaryFilesFolder(folder);
	ui->tempFilesFolder->setText(folder);

	updateControls();
}

void StackSettings::updateControls()
{
	if (registeringOnly)
	{
		if (enableDark || enableAll)
			ui->tabWidget->setTabEnabled(darkTab, true);
		if (enableFlat || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		if (enableBias || enableAll)
			ui->tabWidget->setTabEnabled(biasTab, true);
		ui->tabWidget->setTabEnabled(intermediateTab, false);
	}
	else
	{
		ui->tabWidget->setTabEnabled(resultTab, true);
		if (cometStacking || enableAll)
			ui->tabWidget->setTabEnabled(cometTab, true);
		if (enableDark || enableAll)
			ui->tabWidget->setTabEnabled(darkTab, true);
		if (enableFlat || enableAll)
			ui->tabWidget->setTabEnabled(flatTab, true);
		if (enableBias || enableAll)
			ui->tabWidget->setTabEnabled(biasTab, true);
		ui->tabWidget->setTabEnabled(alignmentTab, true);
		ui->tabWidget->setTabEnabled(intermediateTab, true);
		ui->tabWidget->setTabEnabled(postCalibrationTab, true);
		ui->tabWidget->setTabEnabled(outputTab, true);
	};

	if (ui->tabWidget->isTabEnabled(resultTab))
		ui->tabWidget->setCurrentIndex(resultTab);

}

void StackSettings::tabChanged(int tab)
{
	QWidget * which = ui->tabWidget->widget(tab);
	if (nullptr != which)
		QMetaObject::invokeMethod(which, "onSetActive");
}

void StackSettings::on_chooseFolder_clicked(bool value)
{
	value;
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Temprorary Files Folder"),
		ui->tempFilesFolder->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (dir.length() > 0)
	{
		ui->tempFilesFolder->setText(dir);
	}

}
void StackSettings::accept()
{
	CWorkspace workspace;

	//
	// Pop the preserved workspace setting and discard the saved values
	//
	workspace.Pop(false);

	//
	// Harden the workspace changes
	//
	workspace.saveSettings();

	// Save whether allowed to use all processors ane whether to run threads
	// at reduced priority
	if (CMultitask::GetNrProcessors(true) > 1)
		CMultitask::SetUseAllProcessors(ui->useAllProcessors->isChecked());
	CMultitask::SetReducedThreadsPriority(ui->reducePriority->isChecked());

	//
	// Save the temporary files folder
	//
	CAllStackingTasks::SetTemporaryFilesFolder(ui->tempFilesFolder->text());

	//
	// Ask the output tab to save the Output Settings direct to QSettings
	//
	m_outputTab->saveOutputSettings();

	Inherited::accept();
}

void StackSettings::reject()
{
	CWorkspace workspace;

	//
	// Pop the preserved workspace setting and restore the status quo ante 
	//
	workspace.Pop(true);

	Inherited::reject();
}

void StackSettings::showEvent(QShowEvent *event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}
