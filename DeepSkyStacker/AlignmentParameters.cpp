#include "stdafx.h"
#include "AlignmentParameters.h"
#include "ui/ui_AlignmentParameters.h"
#include "Workspace.h"
#include "DSSCommon.h"

namespace DSS
{
    AlignmentParameters::AlignmentParameters(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::AlignmentParameters),
        workspace(new Workspace())
    {
        ui->setupUi(this);
        m_Alignment = workspace->value("Stacking/AlignmentTransformation", uint(0)).toUInt();
    }

    AlignmentParameters::~AlignmentParameters()
    {
        delete ui;
    }

    void AlignmentParameters::onSetActive()
    {
        updateControls();
    }

    void AlignmentParameters::updateControls()
    {
        switch (m_Alignment)
        {
        case ALIGN_AUTO:
            ui->text->setText(tr("Automatic\n\nThe alignment method is automatically selected depending on the number of available stars.",
                "IDS_ALIGNMENT_AUTO"));
            ui->alignAuto->setChecked(true);
            break;
        case ALIGN_BILINEAR:
            ui->text->setText(tr("Bilinear Alignment\n\nThe Bilinear Alignment is used in all cases.",
                "IDS_ALIGNMENT_BILINEAR"));
            ui->alignBilinear->setChecked(true);
            break;
        case ALIGN_BISQUARED:
            ui->text->setText(tr("Bisquared Alignment\n\nThe Bisquared Alignment is used when at least 25 stars are"
                "available, else the Bilinear Alignment method is used.",
                "IDS_ALIGNMENT_BISQUARED"));
            ui->alignBisquared->setChecked(true);
            break;
        case ALIGN_BICUBIC:
            ui->text->setText(tr("Bicubic Alignment\n\nThe Bicubic method is used when at least 40 stars are"
                "available, then the Bisquared method is used if 25 to 39 stars are available, then the"
                "Bilinear method is used when less than 25 stars are available.",
                "IDS_ALIGNMENT_BICUBIC"));
            ui->alignBicubic->setChecked(true);
            break;
        case ALIGN_NONE:
            ui->text->setText("");
            ui->alignNone->setChecked(true);
            break;
        }
    }

    void AlignmentParameters::setAlignment(uint wAlignment)
    {
        if (m_Alignment != wAlignment)
        {
            m_Alignment = wAlignment;
            workspace->setValue("Stacking/AlignmentTransformation", uint(m_Alignment));
            updateControls();
        }
    };


    void AlignmentParameters::on_alignAuto_clicked()
    {
        setAlignment(ALIGN_AUTO);
    }

    void AlignmentParameters::on_alignBilinear_clicked()
    {
        setAlignment(ALIGN_BILINEAR);
    }

    void AlignmentParameters::on_alignBisquared_clicked()
    {
        setAlignment(ALIGN_BISQUARED);
    }

    void AlignmentParameters::on_alignBicubic_clicked()
    {
        setAlignment(ALIGN_BICUBIC);
    }

    void AlignmentParameters::on_alignNone_clicked()
    {
        setAlignment(ALIGN_NONE);
    }
}