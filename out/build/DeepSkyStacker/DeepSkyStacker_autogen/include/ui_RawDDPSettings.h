/********************************************************************************
** Form generated from reading UI file 'RawDDPSettings.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RAWDDPSETTINGS_H
#define UI_RAWDDPSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_RawDDPSettings
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *rawFilesTab;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox_9;
    QFormLayout *formLayout_5;
    QLabel *label_21;
    QLineEdit *brightness;
    QLabel *label_22;
    QLineEdit *redScale;
    QLabel *label_23;
    QLineEdit *blueScale;
    QGroupBox *groupBox_10;
    QVBoxLayout *verticalLayout_10;
    QCheckBox *noWB;
    QCheckBox *cameraWB;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox_11;
    QVBoxLayout *verticalLayout_11;
    QRadioButton *bilinear;
    QSpacerItem *verticalSpacer_13;
    QRadioButton *AHD;
    QSpacerItem *verticalSpacer_14;
    QRadioButton *rawBayer;
    QLabel *label_24;
    QSpacerItem *verticalSpacer_15;
    QRadioButton *superPixels;
    QLabel *label_25;
    QWidget *fitsFilesTab;
    QVBoxLayout *verticalLayout_22;
    QHBoxLayout *horizontalLayout_17;
    QCheckBox *isFITSRaw;
    QLabel *label_65;
    QHBoxLayout *horizontalLayout_15;
    QLabel *textDSLRs;
    QComboBox *DSLRs;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_16;
    QGroupBox *colourAdjustment;
    QFormLayout *formLayout_11;
    QLineEdit *brightness_2;
    QLineEdit *redScale_2;
    QLineEdit *blueScale_2;
    QLabel *label_59;
    QLabel *label_60;
    QLabel *label_61;
    QGroupBox *bayerGroup;
    QHBoxLayout *horizontalLayout_18;
    QLabel *bayerPattern;
    QGroupBox *groupBox_20;
    QVBoxLayout *verticalLayout_21;
    QRadioButton *bilinear_2;
    QSpacerItem *verticalSpacer_25;
    QRadioButton *AHD_2;
    QSpacerItem *verticalSpacer_26;
    QRadioButton *rawBayer_2;
    QLabel *bilinear_2_desc;
    QSpacerItem *verticalSpacer_27;
    QRadioButton *superPixels_2;
    QLabel *superPixels_2_desc;
    QCheckBox *forceUnsigned;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__RawDDPSettings)
    {
        if (DSS__RawDDPSettings->objectName().isEmpty())
            DSS__RawDDPSettings->setObjectName("DSS__RawDDPSettings");
        DSS__RawDDPSettings->resize(456, 574);
        gridLayout = new QGridLayout(DSS__RawDDPSettings);
        gridLayout->setObjectName("gridLayout");
        tabWidget = new QTabWidget(DSS__RawDDPSettings);
        tabWidget->setObjectName("tabWidget");
        rawFilesTab = new QWidget();
        rawFilesTab->setObjectName("rawFilesTab");
        verticalLayout = new QVBoxLayout(rawFilesTab);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        groupBox_9 = new QGroupBox(rawFilesTab);
        groupBox_9->setObjectName("groupBox_9");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_9->sizePolicy().hasHeightForWidth());
        groupBox_9->setSizePolicy(sizePolicy);
        groupBox_9->setMaximumSize(QSize(150, 16777215));
        formLayout_5 = new QFormLayout(groupBox_9);
        formLayout_5->setObjectName("formLayout_5");
        formLayout_5->setFormAlignment(Qt::AlignCenter);
        label_21 = new QLabel(groupBox_9);
        label_21->setObjectName("label_21");

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_21);

        brightness = new QLineEdit(groupBox_9);
        brightness->setObjectName("brightness");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(brightness->sizePolicy().hasHeightForWidth());
        brightness->setSizePolicy(sizePolicy1);
        brightness->setMinimumSize(QSize(50, 0));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, brightness);

        label_22 = new QLabel(groupBox_9);
        label_22->setObjectName("label_22");

        formLayout_5->setWidget(1, QFormLayout::LabelRole, label_22);

        redScale = new QLineEdit(groupBox_9);
        redScale->setObjectName("redScale");
        sizePolicy1.setHeightForWidth(redScale->sizePolicy().hasHeightForWidth());
        redScale->setSizePolicy(sizePolicy1);
        redScale->setMinimumSize(QSize(50, 0));

        formLayout_5->setWidget(1, QFormLayout::FieldRole, redScale);

        label_23 = new QLabel(groupBox_9);
        label_23->setObjectName("label_23");

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_23);

        blueScale = new QLineEdit(groupBox_9);
        blueScale->setObjectName("blueScale");
        sizePolicy1.setHeightForWidth(blueScale->sizePolicy().hasHeightForWidth());
        blueScale->setSizePolicy(sizePolicy1);
        blueScale->setMinimumSize(QSize(50, 0));

        formLayout_5->setWidget(2, QFormLayout::FieldRole, blueScale);


        horizontalLayout_5->addWidget(groupBox_9);

        groupBox_10 = new QGroupBox(rawFilesTab);
        groupBox_10->setObjectName("groupBox_10");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_10->sizePolicy().hasHeightForWidth());
        groupBox_10->setSizePolicy(sizePolicy2);
        verticalLayout_10 = new QVBoxLayout(groupBox_10);
        verticalLayout_10->setObjectName("verticalLayout_10");
        noWB = new QCheckBox(groupBox_10);
        noWB->setObjectName("noWB");

        verticalLayout_10->addWidget(noWB);

        cameraWB = new QCheckBox(groupBox_10);
        cameraWB->setObjectName("cameraWB");

        verticalLayout_10->addWidget(cameraWB);


        horizontalLayout_5->addWidget(groupBox_10);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_5);

        groupBox_11 = new QGroupBox(rawFilesTab);
        groupBox_11->setObjectName("groupBox_11");
        verticalLayout_11 = new QVBoxLayout(groupBox_11);
        verticalLayout_11->setObjectName("verticalLayout_11");
        bilinear = new QRadioButton(groupBox_11);
        bilinear->setObjectName("bilinear");

        verticalLayout_11->addWidget(bilinear);

        verticalSpacer_13 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_13);

        AHD = new QRadioButton(groupBox_11);
        AHD->setObjectName("AHD");

        verticalLayout_11->addWidget(AHD);

        verticalSpacer_14 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_14);

        rawBayer = new QRadioButton(groupBox_11);
        rawBayer->setObjectName("rawBayer");

        verticalLayout_11->addWidget(rawBayer);

        label_24 = new QLabel(groupBox_11);
        label_24->setObjectName("label_24");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_24->sizePolicy().hasHeightForWidth());
        label_24->setSizePolicy(sizePolicy3);
        label_24->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        label_24->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_24->setWordWrap(true);

        verticalLayout_11->addWidget(label_24);

        verticalSpacer_15 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_15);

        superPixels = new QRadioButton(groupBox_11);
        superPixels->setObjectName("superPixels");

        verticalLayout_11->addWidget(superPixels);

        label_25 = new QLabel(groupBox_11);
        label_25->setObjectName("label_25");
        sizePolicy3.setHeightForWidth(label_25->sizePolicy().hasHeightForWidth());
        label_25->setSizePolicy(sizePolicy3);
        label_25->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        label_25->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_25->setWordWrap(true);

        verticalLayout_11->addWidget(label_25);


        verticalLayout->addWidget(groupBox_11);

        tabWidget->addTab(rawFilesTab, QString());
        fitsFilesTab = new QWidget();
        fitsFilesTab->setObjectName("fitsFilesTab");
        verticalLayout_22 = new QVBoxLayout(fitsFilesTab);
        verticalLayout_22->setObjectName("verticalLayout_22");
        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName("horizontalLayout_17");
        isFITSRaw = new QCheckBox(fitsFilesTab);
        isFITSRaw->setObjectName("isFITSRaw");
        sizePolicy1.setHeightForWidth(isFITSRaw->sizePolicy().hasHeightForWidth());
        isFITSRaw->setSizePolicy(sizePolicy1);

        horizontalLayout_17->addWidget(isFITSRaw);

        label_65 = new QLabel(fitsFilesTab);
        label_65->setObjectName("label_65");
        label_65->setTextFormat(Qt::RichText);
        label_65->setWordWrap(true);

        horizontalLayout_17->addWidget(label_65);


        verticalLayout_22->addLayout(horizontalLayout_17);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        textDSLRs = new QLabel(fitsFilesTab);
        textDSLRs->setObjectName("textDSLRs");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(textDSLRs->sizePolicy().hasHeightForWidth());
        textDSLRs->setSizePolicy(sizePolicy4);

        horizontalLayout_15->addWidget(textDSLRs);

        DSLRs = new QComboBox(fitsFilesTab);
        DSLRs->setObjectName("DSLRs");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(DSLRs->sizePolicy().hasHeightForWidth());
        DSLRs->setSizePolicy(sizePolicy5);

        horizontalLayout_15->addWidget(DSLRs);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_2);


        verticalLayout_22->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName("horizontalLayout_16");
        colourAdjustment = new QGroupBox(fitsFilesTab);
        colourAdjustment->setObjectName("colourAdjustment");
        sizePolicy1.setHeightForWidth(colourAdjustment->sizePolicy().hasHeightForWidth());
        colourAdjustment->setSizePolicy(sizePolicy1);
        formLayout_11 = new QFormLayout(colourAdjustment);
        formLayout_11->setObjectName("formLayout_11");
        formLayout_11->setVerticalSpacing(4);
        brightness_2 = new QLineEdit(colourAdjustment);
        brightness_2->setObjectName("brightness_2");
        sizePolicy1.setHeightForWidth(brightness_2->sizePolicy().hasHeightForWidth());
        brightness_2->setSizePolicy(sizePolicy1);
        brightness_2->setMinimumSize(QSize(50, 0));
        brightness_2->setMaximumSize(QSize(70, 16777215));

        formLayout_11->setWidget(1, QFormLayout::FieldRole, brightness_2);

        redScale_2 = new QLineEdit(colourAdjustment);
        redScale_2->setObjectName("redScale_2");
        sizePolicy1.setHeightForWidth(redScale_2->sizePolicy().hasHeightForWidth());
        redScale_2->setSizePolicy(sizePolicy1);
        redScale_2->setMinimumSize(QSize(50, 0));
        redScale_2->setMaximumSize(QSize(70, 16777215));

        formLayout_11->setWidget(3, QFormLayout::FieldRole, redScale_2);

        blueScale_2 = new QLineEdit(colourAdjustment);
        blueScale_2->setObjectName("blueScale_2");
        sizePolicy1.setHeightForWidth(blueScale_2->sizePolicy().hasHeightForWidth());
        blueScale_2->setSizePolicy(sizePolicy1);
        blueScale_2->setMinimumSize(QSize(50, 0));
        blueScale_2->setMaximumSize(QSize(70, 16777215));

        formLayout_11->setWidget(5, QFormLayout::FieldRole, blueScale_2);

        label_59 = new QLabel(colourAdjustment);
        label_59->setObjectName("label_59");

        formLayout_11->setWidget(1, QFormLayout::LabelRole, label_59);

        label_60 = new QLabel(colourAdjustment);
        label_60->setObjectName("label_60");

        formLayout_11->setWidget(3, QFormLayout::LabelRole, label_60);

        label_61 = new QLabel(colourAdjustment);
        label_61->setObjectName("label_61");

        formLayout_11->setWidget(5, QFormLayout::LabelRole, label_61);


        horizontalLayout_16->addWidget(colourAdjustment);

        bayerGroup = new QGroupBox(fitsFilesTab);
        bayerGroup->setObjectName("bayerGroup");
        sizePolicy2.setHeightForWidth(bayerGroup->sizePolicy().hasHeightForWidth());
        bayerGroup->setSizePolicy(sizePolicy2);
        bayerGroup->setMaximumSize(QSize(200, 16777215));
        bayerGroup->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        horizontalLayout_18 = new QHBoxLayout(bayerGroup);
        horizontalLayout_18->setObjectName("horizontalLayout_18");
        bayerPattern = new QLabel(bayerGroup);
        bayerPattern->setObjectName("bayerPattern");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(bayerPattern->sizePolicy().hasHeightForWidth());
        bayerPattern->setSizePolicy(sizePolicy6);
        bayerPattern->setMinimumSize(QSize(50, 50));
        bayerPattern->setMaximumSize(QSize(50, 50));
        bayerPattern->setAlignment(Qt::AlignCenter);

        horizontalLayout_18->addWidget(bayerPattern);


        horizontalLayout_16->addWidget(bayerGroup);


        verticalLayout_22->addLayout(horizontalLayout_16);

        groupBox_20 = new QGroupBox(fitsFilesTab);
        groupBox_20->setObjectName("groupBox_20");
        verticalLayout_21 = new QVBoxLayout(groupBox_20);
        verticalLayout_21->setSpacing(0);
        verticalLayout_21->setObjectName("verticalLayout_21");
        bilinear_2 = new QRadioButton(groupBox_20);
        bilinear_2->setObjectName("bilinear_2");

        verticalLayout_21->addWidget(bilinear_2);

        verticalSpacer_25 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_21->addItem(verticalSpacer_25);

        AHD_2 = new QRadioButton(groupBox_20);
        AHD_2->setObjectName("AHD_2");

        verticalLayout_21->addWidget(AHD_2);

        verticalSpacer_26 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_21->addItem(verticalSpacer_26);

        rawBayer_2 = new QRadioButton(groupBox_20);
        rawBayer_2->setObjectName("rawBayer_2");

        verticalLayout_21->addWidget(rawBayer_2);

        bilinear_2_desc = new QLabel(groupBox_20);
        bilinear_2_desc->setObjectName("bilinear_2_desc");
        sizePolicy3.setHeightForWidth(bilinear_2_desc->sizePolicy().hasHeightForWidth());
        bilinear_2_desc->setSizePolicy(sizePolicy3);
        bilinear_2_desc->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        bilinear_2_desc->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        bilinear_2_desc->setWordWrap(true);

        verticalLayout_21->addWidget(bilinear_2_desc);

        verticalSpacer_27 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_21->addItem(verticalSpacer_27);

        superPixels_2 = new QRadioButton(groupBox_20);
        superPixels_2->setObjectName("superPixels_2");

        verticalLayout_21->addWidget(superPixels_2);

        superPixels_2_desc = new QLabel(groupBox_20);
        superPixels_2_desc->setObjectName("superPixels_2_desc");
        sizePolicy3.setHeightForWidth(superPixels_2_desc->sizePolicy().hasHeightForWidth());
        superPixels_2_desc->setSizePolicy(sizePolicy3);
        superPixels_2_desc->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        superPixels_2_desc->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        superPixels_2_desc->setWordWrap(true);

        verticalLayout_21->addWidget(superPixels_2_desc);


        verticalLayout_22->addWidget(groupBox_20);

        forceUnsigned = new QCheckBox(fitsFilesTab);
        forceUnsigned->setObjectName("forceUnsigned");

        verticalLayout_22->addWidget(forceUnsigned);

        tabWidget->addTab(fitsFilesTab, QString());

        gridLayout->addWidget(tabWidget, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DSS__RawDDPSettings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 1);


        retranslateUi(DSS__RawDDPSettings);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(DSS__RawDDPSettings);
    } // setupUi

    void retranslateUi(QDialog *DSS__RawDDPSettings)
    {
        DSS__RawDDPSettings->setWindowTitle(QCoreApplication::translate("DSS::RawDDPSettings", "RAW/FITS Digital Development Process Settings", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "Colour Adjustment", nullptr));
        label_21->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Brightness", nullptr));
        label_22->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Red Scale", nullptr));
        label_23->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Blue Scale", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "White Balance", nullptr));
        noWB->setText(QCoreApplication::translate("DSS::RawDDPSettings", "No White Balance Processing", nullptr));
        cameraWB->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Use Camera White Balance", nullptr));
        groupBox_11->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "Bayer Matrix Transformation", nullptr));
        bilinear->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Bilinear Interpolation", nullptr));
        AHD->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Adaptive Homogeneity-Directed (AHD) Interpolation", nullptr));
        rawBayer->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Use Bayer Drizzle algorithm (no interpolation, no debayering", nullptr));
        label_24->setText(QCoreApplication::translate("DSS::RawDDPSettings", "This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.", nullptr));
        superPixels->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Create super-pixels from the raw Bayer matrix (no interpolation)", nullptr));
        label_25->setText(QCoreApplication::translate("DSS::RawDDPSettings", "This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(rawFilesTab), QCoreApplication::translate("DSS::RawDDPSettings", "Raw Files", nullptr));
        isFITSRaw->setText(QString());
        label_65->setText(QCoreApplication::translate("DSS::RawDDPSettings", "<html><head/><body><p>Single plane 16 bit FITS Files are from a &quot;One-Shot-Colour&quot; camera (DSLR, mirrorless, CCD or CMOS) that need to be de-Bayered.<br/>Leave <span style=\" font-style:italic;\">unchecked</span> to attempt automatic detection based on FITS keywords.</p></body></html>", nullptr));
        textDSLRs->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Camera:", nullptr));
        colourAdjustment->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "Colour Adjustment", nullptr));
        label_59->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Brightness", nullptr));
        label_60->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Red scale", nullptr));
        label_61->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Blue scale", nullptr));
        bayerGroup->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "Bayer Pattern Filter used", nullptr));
        bayerPattern->setText(QString());
        groupBox_20->setTitle(QCoreApplication::translate("DSS::RawDDPSettings", "Bayer Matrix Transformation", nullptr));
        bilinear_2->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Bilinear Interpolation", nullptr));
        AHD_2->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Adaptive Homogeneity-Directed (AHD) Interpolation", nullptr));
        rawBayer_2->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Use Bayer Drizzle algorithm (no interpolation, no debayering", nullptr));
        bilinear_2_desc->setText(QCoreApplication::translate("DSS::RawDDPSettings", "This option uses the Bayer matrix as is. No interpolation is done and each pixel is given only primary components from the matrix.", nullptr));
        superPixels_2->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Create super-pixels from the raw Bayer matrix (no interpolation)", nullptr));
        superPixels_2_desc->setText(QCoreApplication::translate("DSS::RawDDPSettings", "This option uses the Bayer matrix to create one super-pixel from each group of 4 pixels (RGBG). The sizes of the resulting image are thus divided by two.", nullptr));
        forceUnsigned->setText(QCoreApplication::translate("DSS::RawDDPSettings", "Force use of unsigned values when signed values are stored", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(fitsFilesTab), QCoreApplication::translate("DSS::RawDDPSettings", "FITS Files", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class RawDDPSettings: public Ui_RawDDPSettings {};
} // namespace Ui
} // namespace DSS

#endif // UI_RAWDDPSETTINGS_H
