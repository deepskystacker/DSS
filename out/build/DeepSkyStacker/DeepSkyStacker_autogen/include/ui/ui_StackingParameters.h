/********************************************************************************
** Form generated from reading UI file 'StackingParameters.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STACKINGPARAMETERS_H
#define UI_STACKINGPARAMETERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_StackingParameters
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLineEdit *kappa;
    QLabel *staticIterations;
    QRadioButton *modeMedian;
    QSpacerItem *verticalSpacer_4;
    QLineEdit *iterations;
    QRadioButton *modeKS;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QRadioButton *modeMaximum;
    QRadioButton *modeMKS;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_5;
    QRadioButton *modeAAWA;
    QRadioButton *modeAverage;
    QLabel *staticKappa;
    QRadioButton *modeEWA;
    QSpacerItem *verticalSpacer_6;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *backgroundCalibration;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *debloom;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *deBloomSettings;
    QSpacerItem *horizontalSpacer_4;
    QWidget *page_2;
    QGridLayout *gridLayout_2;
    QCheckBox *darkOptimisation;
    QCheckBox *badColumns;
    QCheckBox *hotPixels;
    QLineEdit *darkMultiplicationFactor;
    QCheckBox *useDarkFactor;
    QSpacerItem *horizontalSpacer_5;

    void setupUi(QWidget *DSS__StackingParameters)
    {
        if (DSS__StackingParameters->objectName().isEmpty())
            DSS__StackingParameters->setObjectName("DSS__StackingParameters");
        DSS__StackingParameters->resize(522, 342);
        horizontalLayout = new QHBoxLayout(DSS__StackingParameters);
        horizontalLayout->setObjectName("horizontalLayout");
        groupBox = new QGroupBox(DSS__StackingParameters);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        kappa = new QLineEdit(groupBox);
        kappa->setObjectName("kappa");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(kappa->sizePolicy().hasHeightForWidth());
        kappa->setSizePolicy(sizePolicy);
        kappa->setMaximumSize(QSize(80, 16777215));

        gridLayout->addWidget(kappa, 5, 2, 1, 1);

        staticIterations = new QLabel(groupBox);
        staticIterations->setObjectName("staticIterations");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(staticIterations->sizePolicy().hasHeightForWidth());
        staticIterations->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(staticIterations, 7, 1, 1, 1);

        modeMedian = new QRadioButton(groupBox);
        modeMedian->setObjectName("modeMedian");

        gridLayout->addWidget(modeMedian, 3, 0, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_4, 4, 0, 1, 1);

        iterations = new QLineEdit(groupBox);
        iterations->setObjectName("iterations");
        sizePolicy.setHeightForWidth(iterations->sizePolicy().hasHeightForWidth());
        iterations->setSizePolicy(sizePolicy);
        iterations->setMaximumSize(QSize(80, 16777215));

        gridLayout->addWidget(iterations, 7, 2, 1, 1);

        modeKS = new QRadioButton(groupBox);
        modeKS->setObjectName("modeKS");

        gridLayout->addWidget(modeKS, 5, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 6, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 5, 3, 1, 1);

        modeMaximum = new QRadioButton(groupBox);
        modeMaximum->setObjectName("modeMaximum");

        gridLayout->addWidget(modeMaximum, 3, 1, 1, 3);

        modeMKS = new QRadioButton(groupBox);
        modeMKS->setObjectName("modeMKS");

        gridLayout->addWidget(modeMKS, 7, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 8, 0, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_5, 2, 0, 1, 1);

        modeAAWA = new QRadioButton(groupBox);
        modeAAWA->setObjectName("modeAAWA");

        gridLayout->addWidget(modeAAWA, 9, 0, 1, 1);

        modeAverage = new QRadioButton(groupBox);
        modeAverage->setObjectName("modeAverage");

        gridLayout->addWidget(modeAverage, 1, 0, 1, 1);

        staticKappa = new QLabel(groupBox);
        staticKappa->setObjectName("staticKappa");
        sizePolicy1.setHeightForWidth(staticKappa->sizePolicy().hasHeightForWidth());
        staticKappa->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(staticKappa, 5, 1, 1, 1);

        modeEWA = new QRadioButton(groupBox);
        modeEWA->setObjectName("modeEWA");

        gridLayout->addWidget(modeEWA, 1, 1, 1, 3);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_6, 0, 0, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        stackedWidget = new QStackedWidget(groupBox);
        stackedWidget->setObjectName("stackedWidget");
        page = new QWidget();
        page->setObjectName("page");
        verticalLayout = new QVBoxLayout(page);
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(page);
        label->setObjectName("label");
        label->setText(QString::fromUtf8(""));

        verticalLayout->addWidget(label);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        backgroundCalibration = new QPushButton(page);
        backgroundCalibration->setObjectName("backgroundCalibration");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(backgroundCalibration->sizePolicy().hasHeightForWidth());
        backgroundCalibration->setSizePolicy(sizePolicy2);
        backgroundCalibration->setStyleSheet(QString::fromUtf8("color:blue;\n"
"text-align:left"));
        backgroundCalibration->setText(QString::fromUtf8("No Background Calibration"));
        backgroundCalibration->setFlat(true);

        horizontalLayout_2->addWidget(backgroundCalibration);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        debloom = new QCheckBox(page);
        debloom->setObjectName("debloom");
        debloom->setMinimumSize(QSize(40, 0));

        horizontalLayout_2->addWidget(debloom);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        deBloomSettings = new QPushButton(page);
        deBloomSettings->setObjectName("deBloomSettings");

        horizontalLayout_2->addWidget(deBloomSettings);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        gridLayout_2 = new QGridLayout(page_2);
        gridLayout_2->setObjectName("gridLayout_2");
        darkOptimisation = new QCheckBox(page_2);
        darkOptimisation->setObjectName("darkOptimisation");

        gridLayout_2->addWidget(darkOptimisation, 1, 0, 1, 1);

        badColumns = new QCheckBox(page_2);
        badColumns->setObjectName("badColumns");

        gridLayout_2->addWidget(badColumns, 0, 1, 1, 2);

        hotPixels = new QCheckBox(page_2);
        hotPixels->setObjectName("hotPixels");

        gridLayout_2->addWidget(hotPixels, 0, 0, 1, 1);

        darkMultiplicationFactor = new QLineEdit(page_2);
        darkMultiplicationFactor->setObjectName("darkMultiplicationFactor");
        darkMultiplicationFactor->setMaximumSize(QSize(80, 16777215));

        gridLayout_2->addWidget(darkMultiplicationFactor, 1, 2, 1, 1);

        useDarkFactor = new QCheckBox(page_2);
        useDarkFactor->setObjectName("useDarkFactor");

        gridLayout_2->addWidget(useDarkFactor, 1, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 1, 3, 1, 1);

        stackedWidget->addWidget(page_2);

        verticalLayout_2->addWidget(stackedWidget);


        horizontalLayout->addWidget(groupBox);

        QWidget::setTabOrder(modeAverage, modeMedian);
        QWidget::setTabOrder(modeMedian, modeKS);
        QWidget::setTabOrder(modeKS, modeMKS);
        QWidget::setTabOrder(modeMKS, modeAAWA);
        QWidget::setTabOrder(modeAAWA, modeEWA);
        QWidget::setTabOrder(modeEWA, modeMaximum);
        QWidget::setTabOrder(modeMaximum, hotPixels);
        QWidget::setTabOrder(hotPixels, badColumns);
        QWidget::setTabOrder(badColumns, darkOptimisation);
        QWidget::setTabOrder(darkOptimisation, debloom);
        QWidget::setTabOrder(debloom, useDarkFactor);
        QWidget::setTabOrder(useDarkFactor, darkMultiplicationFactor);
        QWidget::setTabOrder(darkMultiplicationFactor, deBloomSettings);

        retranslateUi(DSS__StackingParameters);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DSS__StackingParameters);
    } // setupUi

    void retranslateUi(QWidget *DSS__StackingParameters)
    {
        DSS__StackingParameters->setWindowTitle(QCoreApplication::translate("DSS::StackingParameters", "Stacking", "IDD_STACKINGPARAMETERS"));
        groupBox->setTitle(QCoreApplication::translate("DSS::StackingParameters", "Stacking Mode", "IDC_TITLE"));
        staticIterations->setText(QCoreApplication::translate("DSS::StackingParameters", "Number of iterations:", "IDC_STATICITERATION"));
        modeMedian->setText(QCoreApplication::translate("DSS::StackingParameters", "Median", "IDC_MEDIAN"));
        modeKS->setText(QCoreApplication::translate("DSS::StackingParameters", "Kappa-Sigma clipping", "IDC_SIGMACLIPPING"));
        modeMaximum->setText(QCoreApplication::translate("DSS::StackingParameters", "Maximum", "IDC_MAXIMUM"));
        modeMKS->setText(QCoreApplication::translate("DSS::StackingParameters", "Median Kappa-Sigma clipping", "IDC_MEDIANSIGMACLIPPING,"));
        modeAAWA->setText(QCoreApplication::translate("DSS::StackingParameters", "Auto Adaptive Weighted Average", "IDC_AUTOADAPTIVEAVERAGE"));
        modeAverage->setText(QCoreApplication::translate("DSS::StackingParameters", "Average", "IDC_AVERAGE"));
        staticKappa->setText(QCoreApplication::translate("DSS::StackingParameters", "Kappa:", "IDC_STATICKAPPA"));
        modeEWA->setText(QCoreApplication::translate("DSS::StackingParameters", "Entropy Weighted Average\n"
"(High Dynamic Range)", "IDC_ENTROPYAVERAGE"));
        debloom->setText(QCoreApplication::translate("DSS::StackingParameters", "Debloom", "IDC_DEBLOOM"));
        deBloomSettings->setText(QCoreApplication::translate("DSS::StackingParameters", "Settings", "IDC_DEBLOOMSETTINGS"));
        darkOptimisation->setText(QCoreApplication::translate("DSS::StackingParameters", "Dark Optimisation", "IDC_DARKOPTIMIZATION"));
        badColumns->setText(QCoreApplication::translate("DSS::StackingParameters", "Bad column detection and removal", "IDC_BADCOLUMNREMOVAL"));
        hotPixels->setText(QCoreApplication::translate("DSS::StackingParameters", "Hot pixel detection and removal", "IDC_HOTPIXELS"));
        darkMultiplicationFactor->setText(QString());
        useDarkFactor->setText(QCoreApplication::translate("DSS::StackingParameters", "Dark Multiplication Factor", "IDC_USEDARKFACTOR"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class StackingParameters: public Ui_StackingParameters {};
} // namespace Ui
} // namespace DSS

#endif // UI_STACKINGPARAMETERS_H
