#include "PostCalibration.h"
#include "ui/ui_PostCalibration.h"

PostCalibration::PostCalibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostCalibration)
{
    ui->setupUi(this);
}

PostCalibration::~PostCalibration()
{
    delete ui;
}
