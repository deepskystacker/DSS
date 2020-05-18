#include "IntermediateFiles.h"
#include "ui/ui_IntermediateFiles.h"

IntermediateFiles::IntermediateFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntermediateFiles)
{
    ui->setupUi(this);
}

IntermediateFiles::~IntermediateFiles()
{
    delete ui;
}
