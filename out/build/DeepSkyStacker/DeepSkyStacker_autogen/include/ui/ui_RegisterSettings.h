/********************************************************************************
** Form generated from reading UI file 'RegisterSettings.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERSETTINGS_H
#define UI_REGISTERSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_RegisterSettings
{
public:
    QGridLayout *gridLayout;
    QPushButton *stackingSettings;
    QDialogButtonBox *buttonBox;
    QPushButton *recommendedSettings;
    QTabWidget *tabWidget;
    QWidget *actionsTab;
    QVBoxLayout *verticalLayout_5;
    QCheckBox *forceRegister;
    QSpacerItem *verticalSpacer_2;
    QCheckBox *hotPixels;
    QSpacerItem *verticalSpacer_3;
    QGroupBox *stackAfter;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *percentStack;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer;
    QLabel *stackWarning;
    QSpacerItem *verticalSpacer;
    QWidget *advancedTab;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *luminancePercent;
    QSlider *luminanceThreshold;
    QCheckBox *checkBox_autoThreshold;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer_4;
    QPushButton *computeDetectedStars;
    QSpacerItem *verticalSpacer_5;
    QLabel *starCount;
    QSpacerItem *verticalSpacer_6;
    QCheckBox *medianFilter;

    void setupUi(QDialog *DSS__RegisterSettings)
    {
        if (DSS__RegisterSettings->objectName().isEmpty())
            DSS__RegisterSettings->setObjectName("DSS__RegisterSettings");
        DSS__RegisterSettings->resize(414, 327);
        gridLayout = new QGridLayout(DSS__RegisterSettings);
        gridLayout->setObjectName("gridLayout");
        stackingSettings = new QPushButton(DSS__RegisterSettings);
        stackingSettings->setObjectName("stackingSettings");
        stackingSettings->setMinimumSize(QSize(150, 0));

        gridLayout->addWidget(stackingSettings, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DSS__RegisterSettings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout->addWidget(buttonBox, 2, 1, 1, 1);

        recommendedSettings = new QPushButton(DSS__RegisterSettings);
        recommendedSettings->setObjectName("recommendedSettings");
        recommendedSettings->setMinimumSize(QSize(150, 0));
        recommendedSettings->setFlat(false);

        gridLayout->addWidget(recommendedSettings, 1, 0, 1, 1);

        tabWidget = new QTabWidget(DSS__RegisterSettings);
        tabWidget->setObjectName("tabWidget");
        actionsTab = new QWidget();
        actionsTab->setObjectName("actionsTab");
        verticalLayout_5 = new QVBoxLayout(actionsTab);
        verticalLayout_5->setObjectName("verticalLayout_5");
        forceRegister = new QCheckBox(actionsTab);
        forceRegister->setObjectName("forceRegister");

        verticalLayout_5->addWidget(forceRegister);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_2);

        hotPixels = new QCheckBox(actionsTab);
        hotPixels->setObjectName("hotPixels");

        verticalLayout_5->addWidget(hotPixels);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_3);

        stackAfter = new QGroupBox(actionsTab);
        stackAfter->setObjectName("stackAfter");
        stackAfter->setMinimumSize(QSize(361, 130));
        stackAfter->setCheckable(true);
        verticalLayout = new QVBoxLayout(stackAfter);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(stackAfter);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        percentStack = new QLineEdit(stackAfter);
        percentStack->setObjectName("percentStack");
        percentStack->setMinimumSize(QSize(30, 0));
        percentStack->setMaximumSize(QSize(30, 16777215));
        percentStack->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout->addWidget(percentStack);

        label_2 = new QLabel(stackAfter);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        stackWarning = new QLabel(stackAfter);
        stackWarning->setObjectName("stackWarning");
        stackWarning->setFrameShape(QFrame::Shape::Panel);
        stackWarning->setFrameShadow(QFrame::Shadow::Sunken);
        stackWarning->setLineWidth(1);
        stackWarning->setTextFormat(Qt::TextFormat::PlainText);
        stackWarning->setAlignment(Qt::AlignmentFlag::AlignCenter);
        stackWarning->setWordWrap(true);
        stackWarning->setMargin(0);

        verticalLayout->addWidget(stackWarning);


        verticalLayout_5->addWidget(stackAfter);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        tabWidget->addTab(actionsTab, QString());
        advancedTab = new QWidget();
        advancedTab->setObjectName("advancedTab");
        verticalLayout_4 = new QVBoxLayout(advancedTab);
        verticalLayout_4->setObjectName("verticalLayout_4");
        label_4 = new QLabel(advancedTab);
        label_4->setObjectName("label_4");

        verticalLayout_4->addWidget(label_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        luminancePercent = new QLabel(advancedTab);
        luminancePercent->setObjectName("luminancePercent");
        luminancePercent->setMaximumSize(QSize(30, 16777215));

        horizontalLayout_2->addWidget(luminancePercent);

        luminanceThreshold = new QSlider(advancedTab);
        luminanceThreshold->setObjectName("luminanceThreshold");
        luminanceThreshold->setMinimum(1);
        luminanceThreshold->setMaximum(98);
        luminanceThreshold->setValue(20);
        luminanceThreshold->setTracking(true);
        luminanceThreshold->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_2->addWidget(luminanceThreshold);


        verticalLayout_4->addLayout(horizontalLayout_2);

        checkBox_autoThreshold = new QCheckBox(advancedTab);
        checkBox_autoThreshold->setObjectName("checkBox_autoThreshold");

        verticalLayout_4->addWidget(checkBox_autoThreshold);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        computeDetectedStars = new QPushButton(advancedTab);
        computeDetectedStars->setObjectName("computeDetectedStars");

        verticalLayout_3->addWidget(computeDetectedStars);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_5);

        starCount = new QLabel(advancedTab);
        starCount->setObjectName("starCount");
        starCount->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_3->addWidget(starCount);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_6);

        medianFilter = new QCheckBox(advancedTab);
        medianFilter->setObjectName("medianFilter");

        verticalLayout_3->addWidget(medianFilter);


        verticalLayout_4->addLayout(verticalLayout_3);

        tabWidget->addTab(advancedTab, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 2);

        QWidget::setTabOrder(tabWidget, percentStack);
        QWidget::setTabOrder(percentStack, computeDetectedStars);
        QWidget::setTabOrder(computeDetectedStars, medianFilter);
        QWidget::setTabOrder(medianFilter, recommendedSettings);
        QWidget::setTabOrder(recommendedSettings, stackingSettings);

        retranslateUi(DSS__RegisterSettings);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DSS__RegisterSettings);
    } // setupUi

    void retranslateUi(QDialog *DSS__RegisterSettings)
    {
        DSS__RegisterSettings->setWindowTitle(QCoreApplication::translate("DSS::RegisterSettings", "Register Settings", nullptr));
        stackingSettings->setText(QCoreApplication::translate("DSS::RegisterSettings", "Stacking Settings...", "IDC_STACKINGPARAMETERS"));
        recommendedSettings->setText(QCoreApplication::translate("DSS::RegisterSettings", "Recommended Settings...", "IDC_RECOMMANDEDSETTINGS"));
        forceRegister->setText(QCoreApplication::translate("DSS::RegisterSettings", "Register already registered pictures", "IDC_FORCEREGISTER"));
        hotPixels->setText(QCoreApplication::translate("DSS::RegisterSettings", "Automatic detection of hot pixels", "IDC_HOTPIXELS"));
        stackAfter->setTitle(QCoreApplication::translate("DSS::RegisterSettings", "Stack after registering", "IDC_STACK"));
        label->setText(QCoreApplication::translate("DSS::RegisterSettings", "Select the best", nullptr));
        percentStack->setText(QString());
        label_2->setText(QCoreApplication::translate("DSS::RegisterSettings", "% of the pictures and stack them", nullptr));
        stackWarning->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(actionsTab), QCoreApplication::translate("DSS::RegisterSettings", "Actions", nullptr));
        label_4->setText(QCoreApplication::translate("DSS::RegisterSettings", "Star detection threshold", nullptr));
        luminancePercent->setText(QString());
        checkBox_autoThreshold->setText(QCoreApplication::translate("DSS::RegisterSettings", "Use automatic threshold", nullptr));
        computeDetectedStars->setText(QCoreApplication::translate("DSS::RegisterSettings", "Compute the number of detected stars", "IDC_COMPUTEDETECTEDSTARS"));
        starCount->setText(QString());
        medianFilter->setText(QCoreApplication::translate("DSS::RegisterSettings", "Reduce noise by using a median filter", "IDC_MEDIANFILTER"));
        tabWidget->setTabText(tabWidget->indexOf(advancedTab), QCoreApplication::translate("DSS::RegisterSettings", "Advanced", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class RegisterSettings: public Ui_RegisterSettings {};
} // namespace Ui
} // namespace DSS

#endif // UI_REGISTERSETTINGS_H
