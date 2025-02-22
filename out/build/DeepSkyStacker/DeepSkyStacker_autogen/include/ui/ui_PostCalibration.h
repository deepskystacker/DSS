/********************************************************************************
** Form generated from reading UI file 'PostCalibration.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POSTCALIBRATION_H
#define UI_POSTCALIBRATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_PostCalibration
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *title;
    QGroupBox *cleanHotPixels;
    QHBoxLayout *horizontalLayout_2;
    QGridLayout *gridLayout;
    QLabel *hotFilterSize;
    QHBoxLayout *horizontalLayout;
    QLabel *weak1;
    QLabel *strong1;
    QLabel *label_2;
    QSlider *hotFilter;
    QLabel *label_7;
    QLabel *hotThresholdPercent;
    QSlider *hotThreshold;
    QGroupBox *cleanColdPixels;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *coldFilterSize;
    QHBoxLayout *horizontalLayout_4;
    QLabel *weak2;
    QLabel *strong2;
    QLabel *label_11;
    QSlider *coldFilter;
    QLabel *label_12;
    QLabel *coldThresholdPercent;
    QSlider *coldThreshold;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_14;
    QPushButton *replacementMethod;
    QSpacerItem *horizontalSpacer;
    QPushButton *testCosmetic;
    QCheckBox *saveDeltaImage;

    void setupUi(QWidget *DSS__PostCalibration)
    {
        if (DSS__PostCalibration->objectName().isEmpty())
            DSS__PostCalibration->setObjectName("DSS__PostCalibration");
        DSS__PostCalibration->resize(625, 353);
        verticalLayout = new QVBoxLayout(DSS__PostCalibration);
        verticalLayout->setObjectName("verticalLayout");
        title = new QLabel(DSS__PostCalibration);
        title->setObjectName("title");

        verticalLayout->addWidget(title);

        cleanHotPixels = new QGroupBox(DSS__PostCalibration);
        cleanHotPixels->setObjectName("cleanHotPixels");
        cleanHotPixels->setCheckable(true);
        horizontalLayout_2 = new QHBoxLayout(cleanHotPixels);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setVerticalSpacing(2);
        hotFilterSize = new QLabel(cleanHotPixels);
        hotFilterSize->setObjectName("hotFilterSize");
        hotFilterSize->setText(QString::fromUtf8("TextLabel"));
        hotFilterSize->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(hotFilterSize, 0, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        weak1 = new QLabel(cleanHotPixels);
        weak1->setObjectName("weak1");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(weak1->sizePolicy().hasHeightForWidth());
        weak1->setSizePolicy(sizePolicy);
        weak1->setMinimumSize(QSize(0, 20));
        weak1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout->addWidget(weak1);

        strong1 = new QLabel(cleanHotPixels);
        strong1->setObjectName("strong1");
        strong1->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        horizontalLayout->addWidget(strong1);


        gridLayout->addLayout(horizontalLayout, 1, 2, 1, 1);

        label_2 = new QLabel(cleanHotPixels);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        hotFilter = new QSlider(cleanHotPixels);
        hotFilter->setObjectName("hotFilter");
        hotFilter->setMinimum(1);
        hotFilter->setMaximum(6);
        hotFilter->setTracking(true);
        hotFilter->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(hotFilter, 0, 2, 1, 1);

        label_7 = new QLabel(cleanHotPixels);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 2, 0, 1, 1);

        hotThresholdPercent = new QLabel(cleanHotPixels);
        hotThresholdPercent->setObjectName("hotThresholdPercent");
        hotThresholdPercent->setText(QString::fromUtf8("TextLabel"));
        hotThresholdPercent->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(hotThresholdPercent, 2, 1, 1, 1);

        hotThreshold = new QSlider(cleanHotPixels);
        hotThreshold->setObjectName("hotThreshold");
        hotThreshold->setMinimum(10);
        hotThreshold->setMaximum(990);
        hotThreshold->setSingleStep(10);
        hotThreshold->setPageStep(100);
        hotThreshold->setValue(500);
        hotThreshold->setTracking(true);
        hotThreshold->setOrientation(Qt::Horizontal);
        hotThreshold->setInvertedAppearance(false);

        gridLayout->addWidget(hotThreshold, 2, 2, 1, 1);

        gridLayout->setColumnStretch(0, 20);
        gridLayout->setColumnStretch(1, 20);
        gridLayout->setColumnStretch(2, 60);

        horizontalLayout_2->addLayout(gridLayout);


        verticalLayout->addWidget(cleanHotPixels);

        cleanColdPixels = new QGroupBox(DSS__PostCalibration);
        cleanColdPixels->setObjectName("cleanColdPixels");
        cleanColdPixels->setCheckable(true);
        horizontalLayout_3 = new QHBoxLayout(cleanColdPixels);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setVerticalSpacing(2);
        coldFilterSize = new QLabel(cleanColdPixels);
        coldFilterSize->setObjectName("coldFilterSize");
        coldFilterSize->setText(QString::fromUtf8("TextLabel"));
        coldFilterSize->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(coldFilterSize, 0, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        weak2 = new QLabel(cleanColdPixels);
        weak2->setObjectName("weak2");
        weak2->setMinimumSize(QSize(0, 20));
        weak2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout_4->addWidget(weak2);

        strong2 = new QLabel(cleanColdPixels);
        strong2->setObjectName("strong2");
        strong2->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

        horizontalLayout_4->addWidget(strong2);


        gridLayout_2->addLayout(horizontalLayout_4, 1, 2, 1, 1);

        label_11 = new QLabel(cleanColdPixels);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 0, 0, 1, 1);

        coldFilter = new QSlider(cleanColdPixels);
        coldFilter->setObjectName("coldFilter");
        coldFilter->setMinimum(1);
        coldFilter->setMaximum(6);
        coldFilter->setTracking(true);
        coldFilter->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(coldFilter, 0, 2, 1, 1);

        label_12 = new QLabel(cleanColdPixels);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 2, 0, 1, 1);

        coldThresholdPercent = new QLabel(cleanColdPixels);
        coldThresholdPercent->setObjectName("coldThresholdPercent");
        coldThresholdPercent->setText(QString::fromUtf8("TextLabel"));
        coldThresholdPercent->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(coldThresholdPercent, 2, 1, 1, 1);

        coldThreshold = new QSlider(cleanColdPixels);
        coldThreshold->setObjectName("coldThreshold");
        coldThreshold->setMinimum(10);
        coldThreshold->setMaximum(990);
        coldThreshold->setSingleStep(10);
        coldThreshold->setPageStep(100);
        coldThreshold->setValue(500);
        coldThreshold->setTracking(true);
        coldThreshold->setOrientation(Qt::Horizontal);
        coldThreshold->setInvertedAppearance(false);

        gridLayout_2->addWidget(coldThreshold, 2, 2, 1, 1);

        gridLayout_2->setColumnStretch(0, 20);
        gridLayout_2->setColumnStretch(1, 20);
        gridLayout_2->setColumnStretch(2, 60);

        horizontalLayout_3->addLayout(gridLayout_2);


        verticalLayout->addWidget(cleanColdPixels);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        label_14 = new QLabel(DSS__PostCalibration);
        label_14->setObjectName("label_14");

        horizontalLayout_5->addWidget(label_14);

        replacementMethod = new QPushButton(DSS__PostCalibration);
        replacementMethod->setObjectName("replacementMethod");
        replacementMethod->setStyleSheet(QString::fromUtf8("color: blue;\n"
"text-align:left"));
        replacementMethod->setText(QString::fromUtf8(""));
        replacementMethod->setFlat(true);

        horizontalLayout_5->addWidget(replacementMethod);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        testCosmetic = new QPushButton(DSS__PostCalibration);
        testCosmetic->setObjectName("testCosmetic");
        testCosmetic->setStyleSheet(QString::fromUtf8("color: blue;\n"
"text-align:left"));
        testCosmetic->setFlat(true);

        horizontalLayout_5->addWidget(testCosmetic);


        verticalLayout->addLayout(horizontalLayout_5);

        saveDeltaImage = new QCheckBox(DSS__PostCalibration);
        saveDeltaImage->setObjectName("saveDeltaImage");

        verticalLayout->addWidget(saveDeltaImage);

        QWidget::setTabOrder(cleanHotPixels, cleanColdPixels);
        QWidget::setTabOrder(cleanColdPixels, hotThreshold);
        QWidget::setTabOrder(hotThreshold, coldFilter);
        QWidget::setTabOrder(coldFilter, saveDeltaImage);
        QWidget::setTabOrder(saveDeltaImage, hotFilter);
        QWidget::setTabOrder(hotFilter, coldThreshold);

        retranslateUi(DSS__PostCalibration);

        QMetaObject::connectSlotsByName(DSS__PostCalibration);
    } // setupUi

    void retranslateUi(QWidget *DSS__PostCalibration)
    {
        DSS__PostCalibration->setWindowTitle(QCoreApplication::translate("DSS::PostCalibration", "Cosmetic", nullptr));
        title->setText(QCoreApplication::translate("DSS::PostCalibration", "Post Calibration Cosmetic settings", nullptr));
        cleanHotPixels->setTitle(QCoreApplication::translate("DSS::PostCalibration", "Detect and Clean remaining Hot Pixels", "IDC_DETECTCLEANHOT"));
        weak1->setText(QCoreApplication::translate("DSS::PostCalibration", "<small>Effect weak</small>", "IDC_WEAK1"));
        strong1->setText(QCoreApplication::translate("DSS::PostCalibration", "<small>Strong</small>", "IDC_STRONG1"));
        label_2->setText(QCoreApplication::translate("DSS::PostCalibration", "Filter Size", nullptr));
        label_7->setText(QCoreApplication::translate("DSS::PostCalibration", "Detection Threshold", nullptr));
        cleanColdPixels->setTitle(QCoreApplication::translate("DSS::PostCalibration", "Detect and Clean remaining Cold Pixels", "IDC_DETECTCLEANCOLD"));
        weak2->setText(QCoreApplication::translate("DSS::PostCalibration", "<small>Effect weak</small>", "IDC_WEAK1"));
        strong2->setText(QCoreApplication::translate("DSS::PostCalibration", "<small>Strong</small>", "IDC_STRONG1"));
        label_11->setText(QCoreApplication::translate("DSS::PostCalibration", "Filter Size", nullptr));
        label_12->setText(QCoreApplication::translate("DSS::PostCalibration", "Detection Threshold", nullptr));
        label_14->setText(QCoreApplication::translate("DSS::PostCalibration", "Replace pixel value with", "IDC_REPLACETEXT"));
        testCosmetic->setText(QCoreApplication::translate("DSS::PostCalibration", "Test on first frame...", "IDC_TESTCOSMETIC"));
        saveDeltaImage->setText(QCoreApplication::translate("DSS::PostCalibration", "Save an image showing the cleaned pixels for each light frame", "IDC_SAVEDELTAIMAGE"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class PostCalibration: public Ui_PostCalibration {};
} // namespace Ui
} // namespace DSS

#endif // UI_POSTCALIBRATION_H
