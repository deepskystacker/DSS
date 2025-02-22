/********************************************************************************
** Form generated from reading UI file 'SettingsTab.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSTAB_H
#define UI_SETTINGSTAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_SettingsTab
{
public:
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *stackingGroup;
    QGridLayout *gridLayout;
    QCheckBox *dontStackUntil;
    QLineEdit *minImages;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_19;
    QSpacerItem *horizontalSpacer_9;
    QLabel *label;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *warnQuality;
    QCheckBox *dontStackQuality;
    QLineEdit *quality;
    QSpacerItem *horizontalSpacer_13;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *warnStars;
    QCheckBox *dontStackStars;
    QLineEdit *stars;
    QSpacerItem *horizontalSpacer_14;
    QSpacerItem *horizontalSpacer_5;
    QCheckBox *warnSkyBackground;
    QCheckBox *dontStackSkyBackground;
    QLineEdit *skyBackground;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_15;
    QSpacerItem *horizontalSpacer_6;
    QCheckBox *warnFWHM;
    QCheckBox *dontStackFWHM;
    QLineEdit *FWHM;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_16;
    QSpacerItem *horizontalSpacer_7;
    QCheckBox *warnOffset;
    QCheckBox *dontStackOffset;
    QLineEdit *offset;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_17;
    QSpacerItem *horizontalSpacer_8;
    QCheckBox *warnAngle;
    QCheckBox *dontStackAngle;
    QLineEdit *angle;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_18;
    QCheckBox *moveNonStackable;
    QLabel *label_7;
    QGroupBox *warningGroup;
    QGridLayout *gridLayout_2;
    QCheckBox *warnFlash;
    QSpacerItem *horizontalSpacer_10;
    QLabel *emailAddress;
    QCheckBox *warnSound;
    QCheckBox *warnEmail;
    QLabel *warnFileFolder;
    QCheckBox *warnFile;
    QSpacerItem *horizontalSpacer_20;
    QPushButton *resetEmailCount;
    QGroupBox *optionsGroup;
    QGridLayout *gridLayout_3;
    QCheckBox *saveStackedImage;
    QLineEdit *imageCount;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_9;
    QLabel *stackedOutputFolder;
    QSpacerItem *horizontalSpacer_12;
    QCheckBox *retainTraceFile;
    QPushButton *resetOutputFolder;
    QGroupBox *filtersGroup;
    QGridLayout *gridLayout_4;
    QLabel *label_11;
    QCheckBox *processRaw;
    QCheckBox *processTIFF;
    QSpacerItem *horizontalSpacer_21;
    QCheckBox *processFITS;
    QCheckBox *processOther;
    QSpacerItem *horizontalSpacer_22;
    QSpacerItem *verticalSpacer_2;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QPushButton *Apply;
    QPushButton *Cancel;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *DSS__SettingsTab)
    {
        if (DSS__SettingsTab->objectName().isEmpty())
            DSS__SettingsTab->setObjectName("DSS__SettingsTab");
        DSS__SettingsTab->resize(917, 625);
        horizontalLayout = new QHBoxLayout(DSS__SettingsTab);
        horizontalLayout->setObjectName("horizontalLayout");
        scrollArea = new QScrollArea(DSS__SettingsTab);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setMinimumSize(QSize(0, 537));
        scrollArea->setMaximumSize(QSize(16777215, 16777215));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 769, 623));
        verticalLayout_5 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_5->setObjectName("verticalLayout_5");
        stackingGroup = new QGroupBox(scrollAreaWidgetContents);
        stackingGroup->setObjectName("stackingGroup");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackingGroup->sizePolicy().hasHeightForWidth());
        stackingGroup->setSizePolicy(sizePolicy);
        stackingGroup->setMinimumSize(QSize(0, 290));
        stackingGroup->setMaximumSize(QSize(16777215, 290));
        gridLayout = new QGridLayout(stackingGroup);
        gridLayout->setObjectName("gridLayout");
        dontStackUntil = new QCheckBox(stackingGroup);
        dontStackUntil->setObjectName("dontStackUntil");

        gridLayout->addWidget(dontStackUntil, 0, 0, 1, 3);

        minImages = new QLineEdit(stackingGroup);
        minImages->setObjectName("minImages");
        minImages->setMaximumSize(QSize(60, 30));
        minImages->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(minImages, 0, 3, 1, 1);

        label_2 = new QLabel(stackingGroup);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 4, 1, 3);

        horizontalSpacer_19 = new QSpacerItem(166, 15, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_19, 0, 7, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_9, 1, 0, 1, 1);

        label = new QLabel(stackingGroup);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 1, 1, 2);

        horizontalSpacer_3 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 2, 0, 1, 1);

        warnQuality = new QCheckBox(stackingGroup);
        warnQuality->setObjectName("warnQuality");
        warnQuality->setMinimumSize(QSize(27, 0));
        warnQuality->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(warnQuality, 2, 1, 1, 1);

        dontStackQuality = new QCheckBox(stackingGroup);
        dontStackQuality->setObjectName("dontStackQuality");

        gridLayout->addWidget(dontStackQuality, 2, 2, 1, 1);

        quality = new QLineEdit(stackingGroup);
        quality->setObjectName("quality");
        quality->setMaximumSize(QSize(60, 16777215));
        quality->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(quality, 2, 3, 1, 1);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_13, 2, 4, 1, 3);

        horizontalSpacer_4 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 3, 0, 1, 1);

        warnStars = new QCheckBox(stackingGroup);
        warnStars->setObjectName("warnStars");
        warnStars->setMinimumSize(QSize(27, 0));
        warnStars->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(warnStars, 3, 1, 1, 1);

        dontStackStars = new QCheckBox(stackingGroup);
        dontStackStars->setObjectName("dontStackStars");

        gridLayout->addWidget(dontStackStars, 3, 2, 1, 1);

        stars = new QLineEdit(stackingGroup);
        stars->setObjectName("stars");
        stars->setMaximumSize(QSize(60, 16777215));
        stars->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(stars, 3, 3, 1, 1);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_14, 3, 4, 1, 3);

        horizontalSpacer_5 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 4, 0, 1, 1);

        warnSkyBackground = new QCheckBox(stackingGroup);
        warnSkyBackground->setObjectName("warnSkyBackground");
        warnSkyBackground->setMinimumSize(QSize(27, 0));
        warnSkyBackground->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(warnSkyBackground, 4, 1, 1, 1);

        dontStackSkyBackground = new QCheckBox(stackingGroup);
        dontStackSkyBackground->setObjectName("dontStackSkyBackground");

        gridLayout->addWidget(dontStackSkyBackground, 4, 2, 1, 1);

        skyBackground = new QLineEdit(stackingGroup);
        skyBackground->setObjectName("skyBackground");
        skyBackground->setMaximumSize(QSize(60, 16777215));
        skyBackground->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(skyBackground, 4, 3, 1, 1);

        label_3 = new QLabel(stackingGroup);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 4, 4, 1, 2);

        horizontalSpacer_15 = new QSpacerItem(241, 15, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_15, 4, 6, 1, 2);

        horizontalSpacer_6 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 5, 0, 1, 1);

        warnFWHM = new QCheckBox(stackingGroup);
        warnFWHM->setObjectName("warnFWHM");
        warnFWHM->setMinimumSize(QSize(27, 0));
        warnFWHM->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(warnFWHM, 5, 1, 1, 1);

        dontStackFWHM = new QCheckBox(stackingGroup);
        dontStackFWHM->setObjectName("dontStackFWHM");

        gridLayout->addWidget(dontStackFWHM, 5, 2, 1, 1);

        FWHM = new QLineEdit(stackingGroup);
        FWHM->setObjectName("FWHM");
        FWHM->setMaximumSize(QSize(60, 16777215));
        FWHM->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(FWHM, 5, 3, 1, 1);

        label_4 = new QLabel(stackingGroup);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 5, 4, 1, 2);

        horizontalSpacer_16 = new QSpacerItem(204, 15, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_16, 5, 6, 1, 2);

        horizontalSpacer_7 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_7, 6, 0, 1, 1);

        warnOffset = new QCheckBox(stackingGroup);
        warnOffset->setObjectName("warnOffset");
        warnOffset->setMinimumSize(QSize(27, 0));
        warnOffset->setMaximumSize(QSize(20, 16777215));

        gridLayout->addWidget(warnOffset, 6, 1, 1, 1);

        dontStackOffset = new QCheckBox(stackingGroup);
        dontStackOffset->setObjectName("dontStackOffset");

        gridLayout->addWidget(dontStackOffset, 6, 2, 1, 1);

        offset = new QLineEdit(stackingGroup);
        offset->setObjectName("offset");
        offset->setMaximumSize(QSize(60, 16777215));
        offset->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(offset, 6, 3, 1, 1);

        label_5 = new QLabel(stackingGroup);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 6, 4, 1, 2);

        horizontalSpacer_17 = new QSpacerItem(24, 18, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_17, 6, 6, 1, 2);

        horizontalSpacer_8 = new QSpacerItem(13, 17, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_8, 7, 0, 1, 1);

        warnAngle = new QCheckBox(stackingGroup);
        warnAngle->setObjectName("warnAngle");
        warnAngle->setMinimumSize(QSize(27, 0));

        gridLayout->addWidget(warnAngle, 7, 1, 1, 1);

        dontStackAngle = new QCheckBox(stackingGroup);
        dontStackAngle->setObjectName("dontStackAngle");

        gridLayout->addWidget(dontStackAngle, 7, 2, 1, 1);

        angle = new QLineEdit(stackingGroup);
        angle->setObjectName("angle");
        angle->setMaximumSize(QSize(60, 16777215));
        angle->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(angle, 7, 3, 1, 1);

        label_6 = new QLabel(stackingGroup);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 7, 4, 1, 1);

        horizontalSpacer_18 = new QSpacerItem(149, 14, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_18, 7, 5, 1, 3);

        moveNonStackable = new QCheckBox(stackingGroup);
        moveNonStackable->setObjectName("moveNonStackable");

        gridLayout->addWidget(moveNonStackable, 8, 0, 1, 4);

        label_7 = new QLabel(stackingGroup);
        label_7->setObjectName("label_7");
        label_7->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(label_7, 9, 1, 1, 3);


        verticalLayout_5->addWidget(stackingGroup);

        warningGroup = new QGroupBox(scrollAreaWidgetContents);
        warningGroup->setObjectName("warningGroup");
        sizePolicy.setHeightForWidth(warningGroup->sizePolicy().hasHeightForWidth());
        warningGroup->setSizePolicy(sizePolicy);
        warningGroup->setMinimumSize(QSize(0, 82));
        warningGroup->setMaximumSize(QSize(16777215, 82));
        gridLayout_2 = new QGridLayout(warningGroup);
        gridLayout_2->setObjectName("gridLayout_2");
        warnFlash = new QCheckBox(warningGroup);
        warnFlash->setObjectName("warnFlash");

        gridLayout_2->addWidget(warnFlash, 1, 0, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(90, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_10, 0, 5, 1, 1);

        emailAddress = new QLabel(warningGroup);
        emailAddress->setObjectName("emailAddress");
#if QT_CONFIG(tooltip)
        emailAddress->setToolTip(QString::fromUtf8(""));
#endif // QT_CONFIG(tooltip)

        gridLayout_2->addWidget(emailAddress, 0, 2, 1, 2);

        warnSound = new QCheckBox(warningGroup);
        warnSound->setObjectName("warnSound");

        gridLayout_2->addWidget(warnSound, 0, 0, 1, 1);

        warnEmail = new QCheckBox(warningGroup);
        warnEmail->setObjectName("warnEmail");

        gridLayout_2->addWidget(warnEmail, 0, 1, 1, 1);

        warnFileFolder = new QLabel(warningGroup);
        warnFileFolder->setObjectName("warnFileFolder");
#if QT_CONFIG(tooltip)
        warnFileFolder->setToolTip(QString::fromUtf8(""));
#endif // QT_CONFIG(tooltip)

        gridLayout_2->addWidget(warnFileFolder, 1, 2, 1, 1);

        warnFile = new QCheckBox(warningGroup);
        warnFile->setObjectName("warnFile");

        gridLayout_2->addWidget(warnFile, 1, 1, 1, 1);

        horizontalSpacer_20 = new QSpacerItem(134, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_20, 1, 3, 1, 3);

        resetEmailCount = new QPushButton(warningGroup);
        resetEmailCount->setObjectName("resetEmailCount");

        gridLayout_2->addWidget(resetEmailCount, 0, 6, 1, 1);


        verticalLayout_5->addWidget(warningGroup);

        optionsGroup = new QGroupBox(scrollAreaWidgetContents);
        optionsGroup->setObjectName("optionsGroup");
        sizePolicy.setHeightForWidth(optionsGroup->sizePolicy().hasHeightForWidth());
        optionsGroup->setSizePolicy(sizePolicy);
        optionsGroup->setMinimumSize(QSize(0, 105));
        optionsGroup->setMaximumSize(QSize(16777215, 105));
        gridLayout_3 = new QGridLayout(optionsGroup);
        gridLayout_3->setObjectName("gridLayout_3");
        saveStackedImage = new QCheckBox(optionsGroup);
        saveStackedImage->setObjectName("saveStackedImage");

        gridLayout_3->addWidget(saveStackedImage, 0, 0, 1, 2);

        imageCount = new QLineEdit(optionsGroup);
        imageCount->setObjectName("imageCount");
        imageCount->setMaximumSize(QSize(60, 16777215));
        imageCount->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_3->addWidget(imageCount, 0, 2, 1, 1);

        label_8 = new QLabel(optionsGroup);
        label_8->setObjectName("label_8");

        gridLayout_3->addWidget(label_8, 0, 3, 1, 1);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_11, 0, 5, 1, 1);

        label_9 = new QLabel(optionsGroup);
        label_9->setObjectName("label_9");

        gridLayout_3->addWidget(label_9, 1, 0, 1, 1);

        stackedOutputFolder = new QLabel(optionsGroup);
        stackedOutputFolder->setObjectName("stackedOutputFolder");
#if QT_CONFIG(tooltip)
        stackedOutputFolder->setToolTip(QString::fromUtf8(""));
#endif // QT_CONFIG(tooltip)

        gridLayout_3->addWidget(stackedOutputFolder, 1, 1, 1, 3);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_12, 1, 5, 1, 1);

        retainTraceFile = new QCheckBox(optionsGroup);
        retainTraceFile->setObjectName("retainTraceFile");

        gridLayout_3->addWidget(retainTraceFile, 2, 0, 1, 1);

        resetOutputFolder = new QPushButton(optionsGroup);
        resetOutputFolder->setObjectName("resetOutputFolder");
#if QT_CONFIG(tooltip)
        resetOutputFolder->setToolTip(QString::fromUtf8(""));
#endif // QT_CONFIG(tooltip)

        gridLayout_3->addWidget(resetOutputFolder, 1, 4, 1, 1);


        verticalLayout_5->addWidget(optionsGroup);

        filtersGroup = new QGroupBox(scrollAreaWidgetContents);
        filtersGroup->setObjectName("filtersGroup");
        sizePolicy.setHeightForWidth(filtersGroup->sizePolicy().hasHeightForWidth());
        filtersGroup->setSizePolicy(sizePolicy);
        filtersGroup->setMinimumSize(QSize(0, 104));
        filtersGroup->setMaximumSize(QSize(16777215, 104));
#if QT_CONFIG(tooltip)
        filtersGroup->setToolTip(QString::fromUtf8(""));
#endif // QT_CONFIG(tooltip)
        gridLayout_4 = new QGridLayout(filtersGroup);
        gridLayout_4->setObjectName("gridLayout_4");
        label_11 = new QLabel(filtersGroup);
        label_11->setObjectName("label_11");

        gridLayout_4->addWidget(label_11, 0, 0, 1, 1);

        processRaw = new QCheckBox(filtersGroup);
        processRaw->setObjectName("processRaw");

        gridLayout_4->addWidget(processRaw, 1, 0, 1, 1);

        processTIFF = new QCheckBox(filtersGroup);
        processTIFF->setObjectName("processTIFF");

        gridLayout_4->addWidget(processTIFF, 1, 1, 1, 1);

        horizontalSpacer_21 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_21, 1, 2, 1, 2);

        processFITS = new QCheckBox(filtersGroup);
        processFITS->setObjectName("processFITS");

        gridLayout_4->addWidget(processFITS, 2, 0, 1, 1);

        processOther = new QCheckBox(filtersGroup);
        processOther->setObjectName("processOther");

        gridLayout_4->addWidget(processOther, 2, 1, 1, 2);

        horizontalSpacer_22 = new QSpacerItem(211, 17, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_22, 2, 3, 1, 1);


        verticalLayout_5->addWidget(filtersGroup);

        verticalSpacer_2 = new QSpacerItem(20, 217, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_2);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);

        frame = new QFrame(DSS__SettingsTab);
        frame->setObjectName("frame");
        frame->setMaximumSize(QSize(16777215, 16777215));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setObjectName("verticalLayout_3");
        Apply = new QPushButton(frame);
        Apply->setObjectName("Apply");

        verticalLayout_3->addWidget(Apply);

        Cancel = new QPushButton(frame);
        Cancel->setObjectName("Cancel");

        verticalLayout_3->addWidget(Cancel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout->addWidget(frame);


        retranslateUi(DSS__SettingsTab);

        QMetaObject::connectSlotsByName(DSS__SettingsTab);
    } // setupUi

    void retranslateUi(QWidget *DSS__SettingsTab)
    {
        DSS__SettingsTab->setWindowTitle(QCoreApplication::translate("DSS::SettingsTab", "Form", nullptr));
        stackingGroup->setTitle(QCoreApplication::translate("DSS::SettingsTab", "Stacking", nullptr));
        dontStackUntil->setText(QCoreApplication::translate("DSS::SettingsTab", "Don't stack until at least", "IDC_DONTSTACK"));
        label_2->setText(QCoreApplication::translate("DSS::SettingsTab", "images are available", nullptr));
        label->setText(QCoreApplication::translate("DSS::SettingsTab", "Warn  Don't stack if...", nullptr));
        warnQuality->setText(QString());
        dontStackQuality->setText(QCoreApplication::translate("DSS::SettingsTab", "... quality is less than", nullptr));
        warnStars->setText(QString());
        dontStackStars->setText(QCoreApplication::translate("DSS::SettingsTab", "... star count is less than", "IDC_DONTSTACK_STARS"));
        warnSkyBackground->setText(QString());
        dontStackSkyBackground->setText(QCoreApplication::translate("DSS::SettingsTab", "... sky background is greater than", "IDC_DONTSTACK_SKYBACKGROUND"));
        label_3->setText(QCoreApplication::translate("DSS::SettingsTab", "%", nullptr));
        warnFWHM->setText(QString());
        dontStackFWHM->setText(QCoreApplication::translate("DSS::SettingsTab", "... FWHM is greater than", "IDC_DONTSTACK_FWHM"));
        label_4->setText(QCoreApplication::translate("DSS::SettingsTab", "pixels", nullptr));
        warnOffset->setText(QString());
        dontStackOffset->setText(QCoreApplication::translate("DSS::SettingsTab", "... dX or dY is greater than", "IDC_DONTSTACK_OFFSET"));
        label_5->setText(QCoreApplication::translate("DSS::SettingsTab", "pixels", nullptr));
        warnAngle->setText(QString());
        dontStackAngle->setText(QCoreApplication::translate("DSS::SettingsTab", "... angle is greater than", nullptr));
        label_6->setText(QCoreApplication::translate("DSS::SettingsTab", "\302\260", nullptr));
        moveNonStackable->setText(QCoreApplication::translate("DSS::SettingsTab", "Move non-stackable files to the 'NonStackable' sub-folder", nullptr));
        label_7->setText(QCoreApplication::translate("DSS::SettingsTab", "(the 'NonStackable' sub folder will be created if necessary)", nullptr));
        warningGroup->setTitle(QCoreApplication::translate("DSS::SettingsTab", "Warnings", nullptr));
        warnFlash->setText(QCoreApplication::translate("DSS::SettingsTab", "Flash Application", "IDC_WARN_FLASH"));
        emailAddress->setText(QCoreApplication::translate("DSS::SettingsTab", "Click here to set or change the email address", "IDC_EMAIL"));
        warnSound->setText(QCoreApplication::translate("DSS::SettingsTab", "Sound", "IDC_WARN_SOUND"));
        warnEmail->setText(QCoreApplication::translate("DSS::SettingsTab", "Send email to:", "IDC_WARN_EMAIL"));
        warnFileFolder->setText(QCoreApplication::translate("DSS::SettingsTab", "Click here to select the warning file folder", "IDC_WARNINGFILEFOLDER"));
        warnFile->setText(QCoreApplication::translate("DSS::SettingsTab", "Create warning file in:", nullptr));
        resetEmailCount->setText(QCoreApplication::translate("DSS::SettingsTab", "Reset email count", "IDC_RESETEMAILCOUNT"));
        optionsGroup->setTitle(QCoreApplication::translate("DSS::SettingsTab", "Options", "IDC_OPTIONS"));
        saveStackedImage->setText(QCoreApplication::translate("DSS::SettingsTab", "Save stacked image to file every", "IDC_SAVESTACKEDIMAGE"));
        label_8->setText(QCoreApplication::translate("DSS::SettingsTab", "images", nullptr));
        label_9->setText(QCoreApplication::translate("DSS::SettingsTab", "Output folder:", nullptr));
        stackedOutputFolder->setText(QCoreApplication::translate("DSS::SettingsTab", "Click here to select the stacked image output folder", "IDC_STACKEDOUTPUTFOLDER"));
#if QT_CONFIG(tooltip)
        retainTraceFile->setToolTip(QCoreApplication::translate("DSS::SettingsTab", "The trace file is normally written to the DeepSkyStacker directory in the users\n"
"\"Documents\" directory, and is deleted on exit (unless the application crashes).\n"
"Tick this box to keep the trace file.", nullptr));
#endif // QT_CONFIG(tooltip)
        retainTraceFile->setText(QCoreApplication::translate("DSS::SettingsTab", "Retain the trace file on exit", nullptr));
        resetOutputFolder->setText(QCoreApplication::translate("DSS::SettingsTab", "Reset", nullptr));
        filtersGroup->setTitle(QCoreApplication::translate("DSS::SettingsTab", "Filters", "IDC_FILTERS"));
        label_11->setText(QCoreApplication::translate("DSS::SettingsTab", "Process only ...", nullptr));
        processRaw->setText(QCoreApplication::translate("DSS::SettingsTab", "RAW images (CR2, NEF, ORF, DNG...)", "IDC_PROCESS_RAW"));
        processTIFF->setText(QCoreApplication::translate("DSS::SettingsTab", "TIFF images", "IDC_PROCESS_TIFF"));
        processFITS->setText(QCoreApplication::translate("DSS::SettingsTab", "FITS images", "IDC_PROCESS_FITS"));
        processOther->setText(QCoreApplication::translate("DSS::SettingsTab", "Other images (JPEG, PNG)", "IDC_PROCESS_OTHERS"));
        Apply->setText(QCoreApplication::translate("DSS::SettingsTab", "Apply changes", "IDC_APPLYCHANGES"));
        Cancel->setText(QCoreApplication::translate("DSS::SettingsTab", "Cancel changes", "IDC_CANCELCHANGES"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class SettingsTab: public Ui_SettingsTab {};
} // namespace Ui
} // namespace DSS

#endif // UI_SETTINGSTAB_H
