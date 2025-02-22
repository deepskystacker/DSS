/********************************************************************************
** Form generated from reading UI file 'ExplorerBar.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPLORERBAR_H
#define UI_EXPLORERBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_ExplorerBar
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QScrollArea *explorerScrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *registerAndStack;
    QVBoxLayout *verticalLayout_8;
    QFrame *frame_1;
    QVBoxLayout *verticalLayout_9;
    QLabel *openLights;
    QLabel *openDarks;
    QLabel *openFlats;
    QLabel *openDarkFlats;
    QLabel *openBias;
    QSpacerItem *verticalSpacer_3;
    QLabel *openFilelist;
    QLabel *saveFilelist;
    QSpacerItem *verticalSpacer_4;
    QLabel *clearList;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_10;
    QLabel *checkAll;
    QLabel *checkAbove;
    QLabel *unCheckAll;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_11;
    QLabel *registerChecked;
    QLabel *computeOffsets;
    QLabel *stackChecked;
    QLabel *batchStacking;
    QSpacerItem *verticalSpacer_9;
    QGroupBox *processing;
    QVBoxLayout *verticalLayout_5;
    QLabel *openPicture;
    QLabel *copyPicture;
    QLabel *doStarMask;
    QLabel *savePicture;
    QSpacerItem *verticalSpacer_10;
    QGroupBox *options;
    QVBoxLayout *verticalLayout_4;
    QLabel *settings;
    QLabel *ddpSettings;
    QLabel *loadSettings;
    QLabel *saveSettings;
    QLabel *recommendedSettings;
    QSpacerItem *verticalSpacer_2;
    QLabel *about;
    QSpacerItem *verticalSpacer_5;
    QLabel *help;
    QSpacerItem *verticalSpacer_6;
    QCheckBox *keepTracefile;
    QSpacerItem *verticalSpacer_7;
    QCheckBox *enableSounds;
    QSpacerItem *verticalSpacer_8;
    QCheckBox *showClipping;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *DSS__ExplorerBar)
    {
        if (DSS__ExplorerBar->objectName().isEmpty())
            DSS__ExplorerBar->setObjectName("DSS__ExplorerBar");
        DSS__ExplorerBar->resize(310, 1052);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DSS__ExplorerBar->sizePolicy().hasHeightForWidth());
        DSS__ExplorerBar->setSizePolicy(sizePolicy);
        DSS__ExplorerBar->setMinimumSize(QSize(310, 454));
        DSS__ExplorerBar->setMaximumSize(QSize(310, 524287));
        DSS__ExplorerBar->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable|QDockWidget::DockWidgetFeature::DockWidgetMovable);
        DSS__ExplorerBar->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea);
        DSS__ExplorerBar->setWindowTitle(QString::fromUtf8("DeepSkyStacker"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        dockWidgetContents->setMinimumSize(QSize(260, 430));
        dockWidgetContents->setMaximumSize(QSize(350, 16777215));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(24);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        explorerScrollArea = new QScrollArea(dockWidgetContents);
        explorerScrollArea->setObjectName("explorerScrollArea");
        sizePolicy.setHeightForWidth(explorerScrollArea->sizePolicy().hasHeightForWidth());
        explorerScrollArea->setSizePolicy(sizePolicy);
        explorerScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 298, 859));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy1);
        scrollAreaWidgetContents->setMinimumSize(QSize(230, 814));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setObjectName("verticalLayout_2");
        registerAndStack = new QGroupBox(scrollAreaWidgetContents);
        registerAndStack->setObjectName("registerAndStack");
        sizePolicy1.setHeightForWidth(registerAndStack->sizePolicy().hasHeightForWidth());
        registerAndStack->setSizePolicy(sizePolicy1);
        registerAndStack->setMinimumSize(QSize(0, 0));
        registerAndStack->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_8 = new QVBoxLayout(registerAndStack);
        verticalLayout_8->setSpacing(5);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(5, 5, 5, 5);
        frame_1 = new QFrame(registerAndStack);
        frame_1->setObjectName("frame_1");
        sizePolicy1.setHeightForWidth(frame_1->sizePolicy().hasHeightForWidth());
        frame_1->setSizePolicy(sizePolicy1);
        frame_1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_9 = new QVBoxLayout(frame_1);
        verticalLayout_9->setSpacing(4);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(5, 5, 5, 5);
        openLights = new QLabel(frame_1);
        openLights->setObjectName("openLights");
        openLights->setStyleSheet(QString::fromUtf8(""));

        verticalLayout_9->addWidget(openLights);

        openDarks = new QLabel(frame_1);
        openDarks->setObjectName("openDarks");
        openDarks->setStyleSheet(QString::fromUtf8("margin-left: 30"));
        openDarks->setMargin(0);

        verticalLayout_9->addWidget(openDarks);

        openFlats = new QLabel(frame_1);
        openFlats->setObjectName("openFlats");
        openFlats->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_9->addWidget(openFlats);

        openDarkFlats = new QLabel(frame_1);
        openDarkFlats->setObjectName("openDarkFlats");
        openDarkFlats->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_9->addWidget(openDarkFlats);

        openBias = new QLabel(frame_1);
        openBias->setObjectName("openBias");
        openBias->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_9->addWidget(openBias);

        verticalSpacer_3 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_9->addItem(verticalSpacer_3);

        openFilelist = new QLabel(frame_1);
        openFilelist->setObjectName("openFilelist");

        verticalLayout_9->addWidget(openFilelist);

        saveFilelist = new QLabel(frame_1);
        saveFilelist->setObjectName("saveFilelist");

        verticalLayout_9->addWidget(saveFilelist);

        verticalSpacer_4 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_9->addItem(verticalSpacer_4);

        clearList = new QLabel(frame_1);
        clearList->setObjectName("clearList");

        verticalLayout_9->addWidget(clearList);


        verticalLayout_8->addWidget(frame_1);

        frame_2 = new QFrame(registerAndStack);
        frame_2->setObjectName("frame_2");
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_10 = new QVBoxLayout(frame_2);
        verticalLayout_10->setSpacing(4);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_10->setContentsMargins(5, 5, 5, 5);
        checkAll = new QLabel(frame_2);
        checkAll->setObjectName("checkAll");

        verticalLayout_10->addWidget(checkAll);

        checkAbove = new QLabel(frame_2);
        checkAbove->setObjectName("checkAbove");

        verticalLayout_10->addWidget(checkAbove);

        unCheckAll = new QLabel(frame_2);
        unCheckAll->setObjectName("unCheckAll");

        verticalLayout_10->addWidget(unCheckAll);


        verticalLayout_8->addWidget(frame_2);

        frame_3 = new QFrame(registerAndStack);
        frame_3->setObjectName("frame_3");
        sizePolicy1.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy1);
        frame_3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_11 = new QVBoxLayout(frame_3);
        verticalLayout_11->setSpacing(4);
        verticalLayout_11->setObjectName("verticalLayout_11");
        verticalLayout_11->setContentsMargins(5, 5, 5, 5);
        registerChecked = new QLabel(frame_3);
        registerChecked->setObjectName("registerChecked");

        verticalLayout_11->addWidget(registerChecked);

        computeOffsets = new QLabel(frame_3);
        computeOffsets->setObjectName("computeOffsets");

        verticalLayout_11->addWidget(computeOffsets);

        stackChecked = new QLabel(frame_3);
        stackChecked->setObjectName("stackChecked");

        verticalLayout_11->addWidget(stackChecked);

        batchStacking = new QLabel(frame_3);
        batchStacking->setObjectName("batchStacking");

        verticalLayout_11->addWidget(batchStacking);


        verticalLayout_8->addWidget(frame_3);

        verticalLayout_8->setStretch(0, 7);
        verticalLayout_8->setStretch(1, 3);
        verticalLayout_8->setStretch(2, 4);

        verticalLayout_2->addWidget(registerAndStack);

        verticalSpacer_9 = new QSpacerItem(20, 1, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_9);

        processing = new QGroupBox(scrollAreaWidgetContents);
        processing->setObjectName("processing");
        sizePolicy1.setHeightForWidth(processing->sizePolicy().hasHeightForWidth());
        processing->setSizePolicy(sizePolicy1);
        processing->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_5 = new QVBoxLayout(processing);
        verticalLayout_5->setSpacing(4);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(11, 6, 11, 6);
        openPicture = new QLabel(processing);
        openPicture->setObjectName("openPicture");

        verticalLayout_5->addWidget(openPicture);

        copyPicture = new QLabel(processing);
        copyPicture->setObjectName("copyPicture");

        verticalLayout_5->addWidget(copyPicture);

        doStarMask = new QLabel(processing);
        doStarMask->setObjectName("doStarMask");

        verticalLayout_5->addWidget(doStarMask);

        savePicture = new QLabel(processing);
        savePicture->setObjectName("savePicture");

        verticalLayout_5->addWidget(savePicture);


        verticalLayout_2->addWidget(processing);

        verticalSpacer_10 = new QSpacerItem(20, 1, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_10);

        options = new QGroupBox(scrollAreaWidgetContents);
        options->setObjectName("options");
        sizePolicy1.setHeightForWidth(options->sizePolicy().hasHeightForWidth());
        options->setSizePolicy(sizePolicy1);
        options->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_4 = new QVBoxLayout(options);
        verticalLayout_4->setSpacing(4);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(11, 6, 11, 6);
        settings = new QLabel(options);
        settings->setObjectName("settings");

        verticalLayout_4->addWidget(settings);

        ddpSettings = new QLabel(options);
        ddpSettings->setObjectName("ddpSettings");
        ddpSettings->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_4->addWidget(ddpSettings);

        loadSettings = new QLabel(options);
        loadSettings->setObjectName("loadSettings");
        loadSettings->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_4->addWidget(loadSettings);

        saveSettings = new QLabel(options);
        saveSettings->setObjectName("saveSettings");
        saveSettings->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_4->addWidget(saveSettings);

        recommendedSettings = new QLabel(options);
        recommendedSettings->setObjectName("recommendedSettings");
        recommendedSettings->setStyleSheet(QString::fromUtf8("margin-left: 30"));

        verticalLayout_4->addWidget(recommendedSettings);

        verticalSpacer_2 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_2);

        about = new QLabel(options);
        about->setObjectName("about");

        verticalLayout_4->addWidget(about);

        verticalSpacer_5 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_5);

        help = new QLabel(options);
        help->setObjectName("help");

        verticalLayout_4->addWidget(help);

        verticalSpacer_6 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_6);

        keepTracefile = new QCheckBox(options);
        keepTracefile->setObjectName("keepTracefile");

        verticalLayout_4->addWidget(keepTracefile);

        verticalSpacer_7 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_7);

        enableSounds = new QCheckBox(options);
        enableSounds->setObjectName("enableSounds");
        enableSounds->setChecked(false);

        verticalLayout_4->addWidget(enableSounds);

        verticalSpacer_8 = new QSpacerItem(20, 5, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_8);

        showClipping = new QCheckBox(options);
        showClipping->setObjectName("showClipping");

        verticalLayout_4->addWidget(showClipping);


        verticalLayout_2->addWidget(options);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        explorerScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(explorerScrollArea);

        DSS__ExplorerBar->setWidget(dockWidgetContents);

        retranslateUi(DSS__ExplorerBar);

        QMetaObject::connectSlotsByName(DSS__ExplorerBar);
    } // setupUi

    void retranslateUi(QDockWidget *DSS__ExplorerBar)
    {
        DSS__ExplorerBar->setStyleSheet(QString());
        dockWidgetContents->setWindowTitle(QString());
        registerAndStack->setTitle(QCoreApplication::translate("DSS::ExplorerBar", "Registering and Stacking", "IDC_STACKING_TITLE"));
        openLights->setText(QCoreApplication::translate("DSS::ExplorerBar", "Open picture files...", "IDC_STACKING_OPENFILES"));
        openDarks->setText(QCoreApplication::translate("DSS::ExplorerBar", "dark files...", "IDC_STACKING_OPENDARKS"));
        openFlats->setText(QCoreApplication::translate("DSS::ExplorerBar", "flat files...", "IDC_STACKING_OPENFLATS"));
        openDarkFlats->setText(QCoreApplication::translate("DSS::ExplorerBar", "dark flat files...", "IDC_STACKING_OPENDARKFLATS"));
        openBias->setText(QCoreApplication::translate("DSS::ExplorerBar", "offset/bias files...", "IDC_STACKING_OPENOFFSETS"));
        openFilelist->setText(QCoreApplication::translate("DSS::ExplorerBar", "Open a File List...", "IDC_STACKING_LOADLIST"));
        saveFilelist->setText(QCoreApplication::translate("DSS::ExplorerBar", "Save the File List...", "IDC_STACKING_SAVELIST"));
        clearList->setText(QCoreApplication::translate("DSS::ExplorerBar", "Clear List", "IDC_STACKING_CLEARLIST"));
        checkAll->setText(QCoreApplication::translate("DSS::ExplorerBar", "Check all", "IDC_STACKING_CHECKALL"));
        checkAbove->setText(QCoreApplication::translate("DSS::ExplorerBar", "Check above a threshold...", "IDC_STACKING_CHECKABOVE"));
        unCheckAll->setText(QCoreApplication::translate("DSS::ExplorerBar", "Uncheck all", "IDC_STACKING_UNCHECKALL"));
        registerChecked->setText(QCoreApplication::translate("DSS::ExplorerBar", "Register checked pictures...", "IDC_REGISTERING_REGISTERCHECKED"));
        computeOffsets->setText(QCoreApplication::translate("DSS::ExplorerBar", "Compute offsets...", "IDC_STACKING_COMPUTEOFFSETS"));
        stackChecked->setText(QCoreApplication::translate("DSS::ExplorerBar", "Stack checked pictures...", "IDC_STACKING_STACKCHECKED"));
        batchStacking->setText(QCoreApplication::translate("DSS::ExplorerBar", "Batch stacking...", "IDC_STACKING_BATCHSTACK"));
        processing->setTitle(QCoreApplication::translate("DSS::ExplorerBar", "Processing", "IDC_PROCESSING_TITLE"));
        openPicture->setText(QCoreApplication::translate("DSS::ExplorerBar", "Open picture file...", "IDC_PROCESSING_OPENFILE"));
        copyPicture->setText(QCoreApplication::translate("DSS::ExplorerBar", "Copy current picture to clipboard", "IDC_PROCESSING_COPYTOCLIPBOARD"));
        doStarMask->setText(QCoreApplication::translate("DSS::ExplorerBar", "Create a Star Mask...", "IDC_PROCESSING_CREATESTARMASK"));
        savePicture->setText(QCoreApplication::translate("DSS::ExplorerBar", "Save picture to file...", "IDC_PROCESSING_SAVEFILE"));
        options->setTitle(QCoreApplication::translate("DSS::ExplorerBar", "Options", "IDC_OPTIONS_TITLE"));
        settings->setText(QCoreApplication::translate("DSS::ExplorerBar", "Settings...", "IDC_OPTIONS_SETTINGS"));
        ddpSettings->setText(QCoreApplication::translate("DSS::ExplorerBar", "Raw/FITS DDP Settings...", "IDC_OPTIONS_RAWDDPSETTINGS"));
        loadSettings->setText(QCoreApplication::translate("DSS::ExplorerBar", "Load...", "IDC_OPTIONS_LOADSETTINGS"));
        saveSettings->setText(QCoreApplication::translate("DSS::ExplorerBar", "Save...", "IDC_OPTIONS_SAVESETTINGS"));
        recommendedSettings->setText(QCoreApplication::translate("DSS::ExplorerBar", "Recommended...", "IDC_OPTIONS_RECOMMANDEDSETTINGS"));
        about->setText(QCoreApplication::translate("DSS::ExplorerBar", "About DeepSkyStacker...", "IDC_ABOUT"));
        help->setText(QCoreApplication::translate("DSS::ExplorerBar", "DeepSkyStacker's Help...", "IDC_CONTEXTHELP"));
#if QT_CONFIG(tooltip)
        keepTracefile->setToolTip(QCoreApplication::translate("DSS::ExplorerBar", "The trace file is normally written to the DeepSkyStacker directory in the users\n"
"\"Documents\" directory, and is deleted on exit (unless the application crashes).\n"
"Tick this box to keep the trace file.", nullptr));
#endif // QT_CONFIG(tooltip)
        keepTracefile->setText(QCoreApplication::translate("DSS::ExplorerBar", "Retain the trace file on exit", nullptr));
        enableSounds->setText(QCoreApplication::translate("DSS::ExplorerBar", "Enable progress sounds", nullptr));
#if QT_CONFIG(tooltip)
        showClipping->setToolTip(QCoreApplication::translate("DSS::ExplorerBar", "In the Processing Panel, show pixels below the Black Level as blue and pixels above the White Level as red.", nullptr));
#endif // QT_CONFIG(tooltip)
        showClipping->setText(QCoreApplication::translate("DSS::ExplorerBar", "Show clipping for Black/White Points", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ExplorerBar: public Ui_ExplorerBar {};
} // namespace Ui
} // namespace DSS

#endif // UI_EXPLORERBAR_H
