/********************************************************************************
** Form generated from reading UI file 'BackgroundOptions.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BACKGROUNDOPTIONS_H
#define UI_BACKGROUNDOPTIONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_BackgroundOptions
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *rbLinear;
    QRadioButton *rbRational;
    QLabel *laCalibration;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *rbNone;
    QRadioButton *rbMinimum;
    QRadioButton *rbMiddle;
    QRadioButton *rbMaximum;
    QLabel *laRGBCalibration;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__BackgroundOptions)
    {
        if (DSS__BackgroundOptions->objectName().isEmpty())
            DSS__BackgroundOptions->setObjectName("DSS__BackgroundOptions");
        DSS__BackgroundOptions->resize(400, 345);
        verticalLayout = new QVBoxLayout(DSS__BackgroundOptions);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(DSS__BackgroundOptions);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        rbLinear = new QRadioButton(groupBox);
        rbLinear->setObjectName("rbLinear");

        verticalLayout_2->addWidget(rbLinear);

        rbRational = new QRadioButton(groupBox);
        rbRational->setObjectName("rbRational");

        verticalLayout_2->addWidget(rbRational);


        horizontalLayout->addLayout(verticalLayout_2);

        laCalibration = new QLabel(groupBox);
        laCalibration->setObjectName("laCalibration");
        laCalibration->setMinimumSize(QSize(204, 97));

        horizontalLayout->addWidget(laCalibration);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(DSS__BackgroundOptions);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        rbNone = new QRadioButton(groupBox_2);
        rbNone->setObjectName("rbNone");

        verticalLayout_3->addWidget(rbNone);

        rbMinimum = new QRadioButton(groupBox_2);
        rbMinimum->setObjectName("rbMinimum");

        verticalLayout_3->addWidget(rbMinimum);

        rbMiddle = new QRadioButton(groupBox_2);
        rbMiddle->setObjectName("rbMiddle");

        verticalLayout_3->addWidget(rbMiddle);

        rbMaximum = new QRadioButton(groupBox_2);
        rbMaximum->setObjectName("rbMaximum");

        verticalLayout_3->addWidget(rbMaximum);


        horizontalLayout_2->addLayout(verticalLayout_3);

        laRGBCalibration = new QLabel(groupBox_2);
        laRGBCalibration->setObjectName("laRGBCalibration");
        laRGBCalibration->setMinimumSize(QSize(204, 97));

        horizontalLayout_2->addWidget(laRGBCalibration);


        verticalLayout->addWidget(groupBox_2);

        buttonBox = new QDialogButtonBox(DSS__BackgroundOptions);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DSS__BackgroundOptions);

        QMetaObject::connectSlotsByName(DSS__BackgroundOptions);
    } // setupUi

    void retranslateUi(QDialog *DSS__BackgroundOptions)
    {
        DSS__BackgroundOptions->setWindowTitle(QCoreApplication::translate("DSS::BackgroundOptions", "Background Options", nullptr));
        groupBox->setTitle(QCoreApplication::translate("DSS::BackgroundOptions", "Calibration Method", nullptr));
        rbLinear->setText(QCoreApplication::translate("DSS::BackgroundOptions", "Linear", nullptr));
        rbRational->setText(QCoreApplication::translate("DSS::BackgroundOptions", "Rational", nullptr));
        laCalibration->setText(QString());
        groupBox_2->setTitle(QCoreApplication::translate("DSS::BackgroundOptions", "RGB Background Calibration Method", nullptr));
        rbNone->setText(QCoreApplication::translate("DSS::BackgroundOptions", "None", nullptr));
        rbMinimum->setText(QCoreApplication::translate("DSS::BackgroundOptions", "Minimum", nullptr));
        rbMiddle->setText(QCoreApplication::translate("DSS::BackgroundOptions", "Middle", nullptr));
        rbMaximum->setText(QCoreApplication::translate("DSS::BackgroundOptions", "Maximum", nullptr));
        laRGBCalibration->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class BackgroundOptions: public Ui_BackgroundOptions {};
} // namespace Ui
} // namespace DSS

#endif // UI_BACKGROUNDOPTIONS_H
