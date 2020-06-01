#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include <ZExcept.h>
#include <Ztrace.h>

#include "OutputTab.h"
#include "ui/ui_OutputTab.h"

#include <QString>
#include <QFileDialog>
#include <QStandardPaths>

#include "Workspace.h"

OutputTab::OutputTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputTab),
	workspace(new CWorkspace())
{
    ui->setupUi(this);

	//
	// Set the text colour as for a Hyper-Link
	// 
	ui->outputFolder->setForegroundRole(QPalette::Link);
}

OutputTab::~OutputTab()
{
    delete ui;
}

void OutputTab::onSetActive()
{
	bool enable = workspace->value("Output/Output", true).toBool();
	bool temp = false;

	ui->createOutput->setChecked(enable);
	ui->createHTML->setEnabled(enable);
	ui->autoSave->setEnabled(enable);
	ui->fileListName->setEnabled(enable);
	ui->appendNumber->setEnabled(enable);
	ui->refFrameFolder->setEnabled(enable);
	ui->fileListFolder->setEnabled(enable);
	ui->otherFolder->setEnabled(enable);
	ui->outputFolder->setEnabled(enable);

	noFolder = ui->outputFolder->text();

	ui->createHTML->setChecked(workspace->value("Output/OutputHTML", false).toBool());

	temp = workspace->value("Output/FileName", false).toBool();
	ui->autoSave->setChecked(!temp);		
	ui->fileListName->setChecked(temp);

	temp = workspace->value("Output/AppendNumber", true).toBool();
	ui->appendNumber->setChecked(temp);

	switch (workspace->value("Output/OutputFolder", 0).toUInt())
	{
	case 0:
		ui->refFrameFolder->setChecked(true);
		ui->fileListFolder->setChecked(false);
		ui->otherFolder->setChecked(false);
		break;
	case 1:		
		ui->refFrameFolder->setChecked(false);
		ui->fileListFolder->setChecked(true);
		ui->otherFolder->setChecked(false);
		break;
	case 2:
		ui->refFrameFolder->setChecked(false);
		ui->fileListFolder->setChecked(false);
		ui->otherFolder->setChecked(true);
		break;
	}

	QString folderName = workspace->value("Output/OutputFolderName").toString();
	if (folderName.length() > 0)
		ui->outputFolder->setText(folderName);
}

void  OutputTab::on_createOutput_stateChanged(int newState)
{
	bool enable(newState == Qt::Checked);
	ui->createOutput->setChecked(enable);
	ui->createHTML->setEnabled(enable);
	ui->autoSave->setEnabled(enable);
	ui->fileListName->setEnabled(enable);
	ui->appendNumber->setEnabled(enable);
	ui->refFrameFolder->setEnabled(enable);
	ui->fileListFolder->setEnabled(enable);
	ui->otherFolder->setEnabled(enable);
	ui->outputFolder->setEnabled(enable);

	workspace->setValue("Output/Output", enable);
}

void  OutputTab::on_createHTML_stateChanged(int newState)
{
	bool enable(newState == Qt::Checked);

	workspace->setValue("Output/OutputHTML", enable);
}

void OutputTab::on_autoSave_clicked()
{
	//
	// is this is selected, then fileListName isn't and vice-versa
	//
	on_fileListName_clicked();
}

void OutputTab::on_fileListName_clicked()
{
	bool checked = ui->fileListName->isChecked();

	workspace->setValue("Output/FileName", checked);

}

void OutputTab::on_appendNumber_stateChanged(int newState)
{
	bool checked(newState == Qt::Checked);

	workspace->setValue("Output/AppendNumber", checked);
}

void OutputTab::on_refFrameFolder_clicked()
{
	workspace->setValue("Output/OutputFolder", uint(0));
}
void OutputTab::on_fileListFolder_clicked()
{
	workspace->setValue("Output/OutputFolder", uint(1));
}
void OutputTab::on_otherFolder_clicked()
{
	workspace->setValue("Output/OutputFolder", uint(2));
}

void OutputTab::on_outputFolder_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"),
		QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (dir.length() > 0)
		workspace->setValue("Output/OutputFolderName", dir);
}

