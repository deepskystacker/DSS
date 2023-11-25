#include "stdafx.h"
#include "IntermediateFiles.h"
#include "ui/ui_IntermediateFiles.h"
#include "Workspace.h"
#include "DSSCommon.h"

namespace DSS
{
	IntermediateFiles::IntermediateFiles(QWidget* parent, bool regOnly) :
		QWidget(parent),
		registerOnly(regOnly),
		ui(new Ui::IntermediateFiles),
		workspace(new Workspace())
	{
		ui->setupUi(this);
	}

	void IntermediateFiles::onSetActive()
	{
		bool	value = false;
		uint	fileFormat;

		value = workspace->value("Stacking/SaveCalibrated", false).toBool();
		ui->saveCalibrated->setChecked(value);
		ui->saveDebayered->setEnabled(value);

		value = workspace->value("Stacking/SaveCalibratedDebayered", false).toBool();
		ui->saveDebayered->setChecked(value);

		value = workspace->value("Stacking/CreateIntermediates", false).toBool();
		ui->saveIntermediate->setChecked(value);
		ui->saveIntermediate->setDisabled(registerOnly);

		fileFormat = workspace->value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();

		switch (fileFormat)
		{
		case IFF_TIFF:
			ui->formatTIFF->setChecked(true);
			ui->formatFITS->setChecked(false);
			break;
		case IFF_FITS:
			ui->formatTIFF->setChecked(false);
			ui->formatFITS->setChecked(true);
			break;
		}
	}

	IntermediateFiles::~IntermediateFiles()
	{
		delete ui;
	}

	void IntermediateFiles::on_formatFITS_clicked()
	{
		workspace->setValue("Stacking/IntermediateFileFormat", (uint)IFF_FITS);
	}
	void IntermediateFiles::on_formatTIFF_clicked()
	{
		workspace->setValue("Stacking/IntermediateFileFormat", (uint)IFF_TIFF);
	}

	void IntermediateFiles::on_saveCalibrated_stateChanged(int state)
	{
		switch (state)
		{
		case Qt::Unchecked:
			workspace->setValue("Stacking/SaveCalibrated", false);
			ui->saveDebayered->setEnabled(false);
			break;
		case Qt::Checked:
			workspace->setValue("Stacking/SaveCalibrated", true);
			ui->saveDebayered->setEnabled(true);
			break;
		}
	}
	void IntermediateFiles::on_saveDebayered_stateChanged(int state)
	{
		switch (state)
		{
		case Qt::Unchecked:
			workspace->setValue("Stacking/SaveCalibratedDebayered", false);
			break;
		case Qt::Checked:
			workspace->setValue("Stacking/SaveCalibratedDebayered", true);
			break;
		}

	}
	void IntermediateFiles::on_saveIntermediate_stateChanged(int state)
	{
		switch (state)
		{
		case Qt::Unchecked:
			workspace->setValue("Stacking/CreateIntermediates", false);
			break;
		case Qt::Checked:
			workspace->setValue("Stacking/CreateIntermediates", true);
			break;
		}
	}
}