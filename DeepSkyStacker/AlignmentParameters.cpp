#include "AlignmentParameters.h"
#include "ui/ui_AlignmentParameters.h"

AlignmentParameters::AlignmentParameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlignmentParameters)
{
    ui->setupUi(this);
}

AlignmentParameters::~AlignmentParameters()
{
    delete ui;
}

void AlignmentParameters::updateText()
{
    switch (m_Alignment)
    {
        case 0:
            /* Passthrough */
        case 1:
            ui->label_2->setText(QCoreApplication::translate("AlignmentParameters", "Automatic\n\nThe alignment method is automatically selected depending on the number of available stars."));
            break;
        case 2:
            ui->label_2->setText(QCoreApplication::translate("AlignmentParameters", "Bilinear Alignment\n\nThe Bilinear Alignment is used in all cases."));
            break;
        case 3:
            ui->label_2->setText(QCoreApplication::translate("AlignmentParameters", "Bisquared Alignment\n\nThe Bisquared Alignment is used when at least 25 stars are available, else the Bilinear Alignment method is used."));
            break;
        case 4:
            ui->label_2->setText(QCoreApplication::translate("AlignmentParameters", "Bicubic Alignment\n\nThe Bicubic method is used when at least 40 stars are available, then the Bisquared method is used if 25 to 39 stars are available, then the Bilinear method is used when less than 25 stars are available."));
            break;
        case 5:
            ui->label_2->setText("");
            break;
    }
}

void AlignmentParameters::onAutomaticClicked()
{
    setAlignment(0);
}

void AlignmentParameters::onBilinearClicked()
{
    setAlignment(2);
}

void AlignmentParameters::onBisquaredClicked()
{
    setAlignment(3);
}

void AlignmentParameters::onBicubicClicked()
{
    setAlignment(4);
}

void AlignmentParameters::onNoAlignemtnClicked()
{
    setAlignment(5);
}