#include "OutputTab.h"
#include "ui/ui_OutputTab.h"

OutputTab::OutputTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputTab)
{
    ui->setupUi(this);
}

OutputTab::~OutputTab()
{
    delete ui;
}
