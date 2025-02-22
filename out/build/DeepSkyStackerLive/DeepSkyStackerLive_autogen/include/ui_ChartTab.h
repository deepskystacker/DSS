/********************************************************************************
** Form generated from reading UI file 'ChartTab.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTTAB_H
#define UI_CHARTTAB_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_ChartTab
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioQuality;
    QRadioButton *radioFWHM;
    QRadioButton *radioStars;
    QRadioButton *radioOffset;
    QRadioButton *radioAngle;
    QRadioButton *radioBackground;
    QSpacerItem *horizontalSpacer;
    QChartView *chartView;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *DSS__ChartTab)
    {
        if (DSS__ChartTab->objectName().isEmpty())
            DSS__ChartTab->setObjectName("DSS__ChartTab");
        DSS__ChartTab->resize(743, 388);
        verticalLayout = new QVBoxLayout(DSS__ChartTab);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        radioQuality = new QRadioButton(DSS__ChartTab);
        buttonGroup = new QButtonGroup(DSS__ChartTab);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(radioQuality);
        radioQuality->setObjectName("radioQuality");
        radioQuality->setMaximumSize(QSize(16777215, 20));

        horizontalLayout->addWidget(radioQuality);

        radioFWHM = new QRadioButton(DSS__ChartTab);
        buttonGroup->addButton(radioFWHM);
        radioFWHM->setObjectName("radioFWHM");
        radioFWHM->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(radioFWHM);

        radioStars = new QRadioButton(DSS__ChartTab);
        buttonGroup->addButton(radioStars);
        radioStars->setObjectName("radioStars");
        radioStars->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(radioStars);

        radioOffset = new QRadioButton(DSS__ChartTab);
        buttonGroup->addButton(radioOffset);
        radioOffset->setObjectName("radioOffset");
        radioOffset->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(radioOffset);

        radioAngle = new QRadioButton(DSS__ChartTab);
        buttonGroup->addButton(radioAngle);
        radioAngle->setObjectName("radioAngle");
        radioAngle->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(radioAngle);

        radioBackground = new QRadioButton(DSS__ChartTab);
        buttonGroup->addButton(radioBackground);
        radioBackground->setObjectName("radioBackground");
        radioBackground->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(radioBackground);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        chartView = new QChartView(DSS__ChartTab);
        chartView->setObjectName("chartView");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(chartView->sizePolicy().hasHeightForWidth());
        chartView->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(chartView);


        retranslateUi(DSS__ChartTab);

        QMetaObject::connectSlotsByName(DSS__ChartTab);
    } // setupUi

    void retranslateUi(QWidget *DSS__ChartTab)
    {
        DSS__ChartTab->setWindowTitle(QCoreApplication::translate("DSS::ChartTab", "Form", nullptr));
        radioQuality->setText(QCoreApplication::translate("DSS::ChartTab", "Quality", nullptr));
        radioFWHM->setText(QCoreApplication::translate("DSS::ChartTab", "FWHM", "IDC_FWHM"));
        radioStars->setText(QCoreApplication::translate("DSS::ChartTab", "#Stars", "IDC_STARS"));
        radioOffset->setText(QCoreApplication::translate("DSS::ChartTab", "dX/dY", "IDC_OFFSET"));
        radioAngle->setText(QCoreApplication::translate("DSS::ChartTab", "Angle", "IDC_ANGLE"));
        radioBackground->setText(QCoreApplication::translate("DSS::ChartTab", "Sky Background", "IDC_SKYBACKGROUND"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ChartTab: public Ui_ChartTab {};
} // namespace Ui
} // namespace DSS

#endif // UI_CHARTTAB_H
