/********************************************************************************
** Form generated from reading UI file 'DropFilesDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DROPFILESDLG_H
#define UI_DROPFILESDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_DropFilesDlg
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *dropFiles;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *lightFrames;
    QRadioButton *darkFrames;
    QRadioButton *flatFrames;
    QRadioButton *darkFlatFrames;
    QRadioButton *biasFrames;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__DropFilesDlg)
    {
        if (DSS__DropFilesDlg->objectName().isEmpty())
            DSS__DropFilesDlg->setObjectName("DSS__DropFilesDlg");
        DSS__DropFilesDlg->resize(299, 225);
        verticalLayout = new QVBoxLayout(DSS__DropFilesDlg);
        verticalLayout->setObjectName("verticalLayout");
        dropFiles = new QGroupBox(DSS__DropFilesDlg);
        dropFiles->setObjectName("dropFiles");
        dropFiles->setTitle(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(dropFiles);
        verticalLayout_2->setObjectName("verticalLayout_2");
        lightFrames = new QRadioButton(dropFiles);
        lightFrames->setObjectName("lightFrames");
        lightFrames->setText(QString::fromUtf8("Light Frames"));

        verticalLayout_2->addWidget(lightFrames);

        darkFrames = new QRadioButton(dropFiles);
        darkFrames->setObjectName("darkFrames");
        darkFrames->setText(QString::fromUtf8("Dark Frames"));

        verticalLayout_2->addWidget(darkFrames);

        flatFrames = new QRadioButton(dropFiles);
        flatFrames->setObjectName("flatFrames");
        flatFrames->setText(QString::fromUtf8("Flat Frames"));

        verticalLayout_2->addWidget(flatFrames);

        darkFlatFrames = new QRadioButton(dropFiles);
        darkFlatFrames->setObjectName("darkFlatFrames");
        darkFlatFrames->setText(QString::fromUtf8("Dark Flat Frames"));

        verticalLayout_2->addWidget(darkFlatFrames);

        biasFrames = new QRadioButton(dropFiles);
        biasFrames->setObjectName("biasFrames");
        biasFrames->setText(QString::fromUtf8("Offset/Bias Frames"));

        verticalLayout_2->addWidget(biasFrames);


        verticalLayout->addWidget(dropFiles);

        buttonBox = new QDialogButtonBox(DSS__DropFilesDlg);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DSS__DropFilesDlg);

        QMetaObject::connectSlotsByName(DSS__DropFilesDlg);
    } // setupUi

    void retranslateUi(QDialog *DSS__DropFilesDlg)
    {
        DSS__DropFilesDlg->setWindowTitle(QCoreApplication::translate("DSS::DropFilesDlg", "Add files ...", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class DropFilesDlg: public Ui_DropFilesDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_DROPFILESDLG_H
