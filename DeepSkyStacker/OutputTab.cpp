#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#include <afx.h>

#include <ZExcept.h>
#include <Ztrace.h>

#include <QString>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>

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
}

OutputTab::~OutputTab()
{
    delete ui;
}

void OutputTab::onSetActive()
{
	bool enable = settings.value("Output/Output", true).toBool();
	bool temp = false;

	ui->createOutput->setChecked(enable);
	ui->createHTML->setEnabled(enable);
	ui->autoSave->setEnabled(enable);
	ui->fileListName->setEnabled(enable);
	ui->appendNumber->setEnabled(enable);
	ui->refFrameFolder->setEnabled(enable);
	ui->fileListFolder->setEnabled(enable);
	ui->otherFolder->setEnabled(enable);
	ui->outputFolder->setEnabled(false);

	noFolder = ui->outputFolder->text();

	ui->createHTML->setChecked(settings.value("Output/OutputHTML", false).toBool());

	temp = settings.value("Output/FileName", false).toBool();
	ui->autoSave->setChecked(!temp);		
	ui->fileListName->setChecked(temp);

	temp = settings.value("Output/AppendNumber", true).toBool();
	ui->appendNumber->setChecked(temp);

	switch (settings.value("Output/OutputFolder", 0).toUInt())
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
		ui->outputFolder->setEnabled(true);
		break;
	}

	QString dir = settings.value("Output/OutputFolderName").toString();
	if (dir.length() > 0)
	{
		QString temp("<a href=\" \">");
		temp += dir + QString("</a>");
		ui->outputFolder->setText(temp);
	}
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

	if (ui->otherFolder->isChecked() && enable)
		ui->outputFolder->setEnabled(enable);
	else
		ui->outputFolder->setEnabled(false);

	settings.setValue("Output/Output", enable);
}

void  OutputTab::on_createHTML_stateChanged(int newState)
{
	bool enable(newState == Qt::Checked);

	settings.setValue("Output/OutputHTML", enable);
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

	settings.setValue("Output/FileName", checked);
}

void OutputTab::on_appendNumber_stateChanged(int newState)
{
	bool checked(newState == Qt::Checked);

	settings.setValue("Output/AppendNumber", checked);
}

void OutputTab::on_refFrameFolder_clicked()
{
	settings.setValue("Output/OutputFolder", uint(0));
	ui->outputFolder->setEnabled(false);
}
void OutputTab::on_fileListFolder_clicked()
{
	settings.setValue("Output/OutputFolder", uint(1));
	ui->outputFolder->setEnabled(false);
}
void OutputTab::on_otherFolder_clicked()
{
	settings.setValue("Output/OutputFolder", uint(2));
	ui->outputFolder->setEnabled(true);
}

void OutputTab::on_outputFolder_linkActivated(const QString & str)
{
	str;

	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"),
		QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (dir.length() > 0)
	{
		QString temp("<a href=\" \">");
		temp += dir + QString("</a>");
		ui->outputFolder->setText(temp);
		settings.setValue("Output/OutputFolderName", dir);
	}
}

