/********************************************************************************
** Form generated from reading UI file 'ResultParameters.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTPARAMETERS_H
#define UI_RESULTPARAMETERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_ResultParameters
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *modeGroup;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *normalMode;
    QSpacerItem *verticalSpacer;
    QRadioButton *mosaicMode;
    QSpacerItem *verticalSpacer_2;
    QRadioButton *intersectionMode;
    QSpacerItem *verticalSpacer_3;
    QRadioButton *customMode;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *drizzle2x;
    QCheckBox *drizzle3x;
    QHBoxLayout *horizontalLayout;
    QCheckBox *alignRGB;
    QVBoxLayout *verticalLayout;
    QLabel *previewImage;
    QLabel *modeText;

    void setupUi(QWidget *DSS__ResultParameters)
    {
        if (DSS__ResultParameters->objectName().isEmpty())
            DSS__ResultParameters->setObjectName("DSS__ResultParameters");
        DSS__ResultParameters->resize(559, 268);
        horizontalLayout_3 = new QHBoxLayout(DSS__ResultParameters);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        modeGroup = new QGroupBox(DSS__ResultParameters);
        modeGroup->setObjectName("modeGroup");
        verticalLayout_4 = new QVBoxLayout(modeGroup);
        verticalLayout_4->setObjectName("verticalLayout_4");
        normalMode = new QRadioButton(modeGroup);
        normalMode->setObjectName("normalMode");

        verticalLayout_4->addWidget(normalMode);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        mosaicMode = new QRadioButton(modeGroup);
        mosaicMode->setObjectName("mosaicMode");

        verticalLayout_4->addWidget(mosaicMode);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        intersectionMode = new QRadioButton(modeGroup);
        intersectionMode->setObjectName("intersectionMode");

        verticalLayout_4->addWidget(intersectionMode);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        customMode = new QRadioButton(modeGroup);
        customMode->setObjectName("customMode");

        verticalLayout_4->addWidget(customMode);


        verticalLayout_2->addWidget(modeGroup);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, -1, -1, -1);
        drizzle2x = new QCheckBox(DSS__ResultParameters);
        drizzle2x->setObjectName("drizzle2x");

        horizontalLayout_2->addWidget(drizzle2x);

        drizzle3x = new QCheckBox(DSS__ResultParameters);
        drizzle3x->setObjectName("drizzle3x");

        horizontalLayout_2->addWidget(drizzle3x);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, 0, -1, 0);
        alignRGB = new QCheckBox(DSS__ResultParameters);
        alignRGB->setObjectName("alignRGB");

        horizontalLayout->addWidget(alignRGB);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout_3->addLayout(verticalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        previewImage = new QLabel(DSS__ResultParameters);
        previewImage->setObjectName("previewImage");
        previewImage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(previewImage);

        modeText = new QLabel(DSS__ResultParameters);
        modeText->setObjectName("modeText");
        modeText->setAlignment(Qt::AlignHCenter|Qt::AlignTop);
        modeText->setWordWrap(true);

        verticalLayout->addWidget(modeText);

        verticalLayout->setStretch(0, 80);
        verticalLayout->setStretch(1, 20);

        horizontalLayout_3->addLayout(verticalLayout);


        retranslateUi(DSS__ResultParameters);

        QMetaObject::connectSlotsByName(DSS__ResultParameters);
    } // setupUi

    void retranslateUi(QWidget *DSS__ResultParameters)
    {
        DSS__ResultParameters->setWindowTitle(QCoreApplication::translate("DSS::ResultParameters", "Result", nullptr));
        normalMode->setText(QCoreApplication::translate("DSS::ResultParameters", "Standard Mode", nullptr));
        mosaicMode->setText(QCoreApplication::translate("DSS::ResultParameters", "\"Mosaic\" Mode", nullptr));
        intersectionMode->setText(QCoreApplication::translate("DSS::ResultParameters", "Intersection Mode", nullptr));
        customMode->setText(QCoreApplication::translate("DSS::ResultParameters", "Custom Rectangle", nullptr));
        drizzle2x->setText(QCoreApplication::translate("DSS::ResultParameters", "Enable 2x Drizzle", nullptr));
        drizzle3x->setText(QCoreApplication::translate("DSS::ResultParameters", "Enable 3x Drizzle", nullptr));
        alignRGB->setText(QCoreApplication::translate("DSS::ResultParameters", "Align RGB Channels in final image", nullptr));
        previewImage->setText(QString());
        modeText->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ResultParameters: public Ui_ResultParameters {};
} // namespace Ui
} // namespace DSS

#endif // UI_RESULTPARAMETERS_H
