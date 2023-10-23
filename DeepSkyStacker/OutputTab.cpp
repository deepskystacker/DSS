#include "stdafx.h"
#include "OutputTab.h"
#include "ui/ui_OutputTab.h"

#include "Workspace.h"

OutputTab::OutputTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputTab)
{
    ui->setupUi(this);

	//
	// Set the text colour as for a Hyper-Link
	// 
	ui->outputFolder->setForegroundRole(QPalette::Link);
	CAllStackingTasks::GetOutputSettings(os);
}

OutputTab::~OutputTab()
{
    delete ui;
}

void OutputTab::onSetActive()
{

	bool enable = os.m_bOutput;

	ui->createOutput->setChecked(enable);
	ui->createHTML->setEnabled(enable);
	ui->autoSave->setEnabled(enable);
	ui->fileListName->setEnabled(enable);
	ui->appendNumber->setEnabled(enable);
	ui->refFrameFolder->setEnabled(enable);
	ui->fileListFolder->setEnabled(enable);
	ui->otherFolder->setEnabled(enable);

	ui->createHTML->setChecked(os.m_bOutputHTML);

	ui->autoSave->setChecked(os.m_bAutosave);		
	ui->fileListName->setChecked(os.m_bFileList);

	ui->appendNumber->setChecked(os.m_bAppend);

	ui->refFrameFolder->setChecked(os.m_bRefFrameFolder);
	ui->fileListFolder->setChecked(os.m_bFileListFolder);
	ui->otherFolder->setChecked(os.m_bOtherFolder);

	ui->outputFolder->setEnabled(os.m_bOtherFolder);

	if (os.m_strFolder.length() > 0)
	{
		ui->outputFolder->setText(os.m_strFolder);
	}
}

void  OutputTab::on_createOutput_stateChanged(int newState)
{
	bool enable(newState == Qt::Checked);

	ui->createHTML->setEnabled(enable);
	ui->autoSave->setEnabled(enable);
	ui->fileListName->setEnabled(enable);
	ui->appendNumber->setEnabled(enable);
	ui->refFrameFolder->setEnabled(enable);
	ui->fileListFolder->setEnabled(enable);
	ui->otherFolder->setEnabled(enable);


	if (ui->otherFolder->isChecked() && enable)
		ui->outputFolder->setEnabled(enable);
	else
		ui->outputFolder->setEnabled(false);

	os.m_bOutput = enable;
}

void  OutputTab::on_createHTML_stateChanged(int newState)
{
	bool enable(newState == Qt::Checked);

	os.m_bOutputHTML = enable;
}

void OutputTab::on_autoSave_clicked()
{
	//
	// If Autosave is selected then FileListName cannot be and
	// vice-versa
	//
	on_fileListName_clicked();
}

void OutputTab::on_fileListName_clicked()
{
	bool checked = ui->fileListName->isChecked();
	os.m_bFileList = checked;
	os.m_bAutosave = !checked;
}

void OutputTab::on_appendNumber_stateChanged(int newState)
{
	bool checked(newState == Qt::Checked);

	os.m_bAppend = checked;
}

void OutputTab::on_refFrameFolder_clicked()
{
	os.m_bRefFrameFolder = true;
	os.m_bFileListFolder = false;
	os.m_bOtherFolder = false;
	ui->outputFolder->setEnabled(false);
}
void OutputTab::on_fileListFolder_clicked()
{
	os.m_bRefFrameFolder = false;
	os.m_bFileListFolder = true;
	os.m_bOtherFolder = false;
	ui->outputFolder->setEnabled(false);
}
void OutputTab::on_otherFolder_clicked()
{
	os.m_bRefFrameFolder = false;
	os.m_bFileListFolder = false;
	os.m_bOtherFolder = true;
	ui->outputFolder->setEnabled(true);
}

void OutputTab::on_outputFolder_pressed()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Folder", "IDS_SELECTOUTPUTFOLDER"),
		QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (dir.length() > 0)
	{
		ui->outputFolder->setText(dir);
		os.m_strFolder = dir;
	}
}

void OutputTab::saveOutputSettings()
{
	CAllStackingTasks::SetOutputSettings(os);
}

