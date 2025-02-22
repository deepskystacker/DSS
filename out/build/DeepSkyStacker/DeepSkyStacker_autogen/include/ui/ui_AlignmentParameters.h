/********************************************************************************
** Form generated from reading UI file 'AlignmentParameters.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGNMENTPARAMETERS_H
#define UI_ALIGNMENTPARAMETERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_AlignmentParameters
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QRadioButton *alignBisquared;
    QRadioButton *alignNone;
    QRadioButton *alignBilinear;
    QLabel *text;
    QRadioButton *alignBicubic;
    QRadioButton *alignAuto;
    QLabel *label;

    void setupUi(QWidget *DSS__AlignmentParameters)
    {
        if (DSS__AlignmentParameters->objectName().isEmpty())
            DSS__AlignmentParameters->setObjectName("DSS__AlignmentParameters");
        DSS__AlignmentParameters->resize(519, 318);
        verticalLayout = new QVBoxLayout(DSS__AlignmentParameters);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(DSS__AlignmentParameters);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        alignBisquared = new QRadioButton(groupBox);
        alignBisquared->setObjectName("alignBisquared");

        gridLayout->addWidget(alignBisquared, 2, 0, 1, 1);

        alignNone = new QRadioButton(groupBox);
        alignNone->setObjectName("alignNone");

        gridLayout->addWidget(alignNone, 4, 0, 1, 1);

        alignBilinear = new QRadioButton(groupBox);
        alignBilinear->setObjectName("alignBilinear");

        gridLayout->addWidget(alignBilinear, 1, 0, 1, 1);

        text = new QLabel(groupBox);
        text->setObjectName("text");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(text->sizePolicy().hasHeightForWidth());
        text->setSizePolicy(sizePolicy);
        text->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        text->setWordWrap(true);

        gridLayout->addWidget(text, 0, 2, 5, 1);

        alignBicubic = new QRadioButton(groupBox);
        alignBicubic->setObjectName("alignBicubic");

        gridLayout->addWidget(alignBicubic, 3, 0, 1, 1);

        alignAuto = new QRadioButton(groupBox);
        alignAuto->setObjectName("alignAuto");

        gridLayout->addWidget(alignAuto, 0, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 1, 1, 1);


        verticalLayout->addWidget(groupBox);


        retranslateUi(DSS__AlignmentParameters);

        QMetaObject::connectSlotsByName(DSS__AlignmentParameters);
    } // setupUi

    void retranslateUi(QWidget *DSS__AlignmentParameters)
    {
        DSS__AlignmentParameters->setWindowTitle(QCoreApplication::translate("DSS::AlignmentParameters", "Alignment", nullptr));
        groupBox->setTitle(QCoreApplication::translate("DSS::AlignmentParameters", "Transformation used during alignment", nullptr));
        alignBisquared->setText(QCoreApplication::translate("DSS::AlignmentParameters", "Bisquared", nullptr));
        alignNone->setText(QCoreApplication::translate("DSS::AlignmentParameters", "No alignment", nullptr));
        alignBilinear->setText(QCoreApplication::translate("DSS::AlignmentParameters", "Bilinear", nullptr));
        text->setText(QString());
        alignBicubic->setText(QCoreApplication::translate("DSS::AlignmentParameters", "Bicubic", nullptr));
        alignAuto->setText(QCoreApplication::translate("DSS::AlignmentParameters", "Automatic", nullptr));
        label->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class AlignmentParameters: public Ui_AlignmentParameters {};
} // namespace Ui
} // namespace DSS

#endif // UI_ALIGNMENTPARAMETERS_H
