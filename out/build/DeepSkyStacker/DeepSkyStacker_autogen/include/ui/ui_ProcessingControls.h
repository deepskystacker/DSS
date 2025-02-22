/********************************************************************************
** Form generated from reading UI file 'ProcessingControls.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSINGCONTROLS_H
#define UI_PROCESSINGCONTROLS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QLinearGradientCtrl.h"

namespace DSS {

class Ui_ProcessingControls
{
public:
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *rgbTab;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *redHAC;
    QLinearGradientCtrl *redGradient;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *greenHAC;
    QLinearGradientCtrl *greenGradient;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *blueHAC;
    QLinearGradientCtrl *blueGradient;
    QCheckBox *linkedSettings;
    QWidget *luminanceTab;
    QVBoxLayout *verticalLayout_7;
    QLabel *darkness;
    QHBoxLayout *horizontalLayout_6;
    QLabel *darkLabel;
    QVBoxLayout *verticalLayout_4;
    QSlider *darkAngle;
    QSlider *darkPower;
    QLabel *midtone;
    QHBoxLayout *horizontalLayout_7;
    QLabel *midLabel;
    QVBoxLayout *verticalLayout_5;
    QSlider *midAngle;
    QSlider *midTone;
    QLabel *highlight;
    QHBoxLayout *horizontalLayout_8;
    QLabel *highLabel;
    QVBoxLayout *verticalLayout_6;
    QSlider *highAngle;
    QSlider *highPower;
    QWidget *saturationTab;
    QVBoxLayout *verticalLayout_8;
    QLabel *saturationTitle;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_9;
    QLabel *saturationLabel;
    QSlider *saturation;
    QSpacerItem *verticalSpacer_2;
    QWidget *buttonWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *applyButton;
    QPushButton *undoButton;
    QPushButton *settingsButton;
    QPushButton *redoButton;
    QPushButton *resetButton;
    QLabel *histogram;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *DSS__ProcessingControls)
    {
        if (DSS__ProcessingControls->objectName().isEmpty())
            DSS__ProcessingControls->setObjectName("DSS__ProcessingControls");
        DSS__ProcessingControls->resize(832, 284);
        horizontalLayout_2 = new QHBoxLayout(DSS__ProcessingControls);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(-1, 1, -1, 1);
        widget = new QWidget(DSS__ProcessingControls);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(352, 0));
        widget->setMaximumSize(QSize(352, 16777215));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(-1, 1, -1, 1);
        tabWidget = new QTabWidget(widget);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setMinimumSize(QSize(350, 240));
        tabWidget->setMaximumSize(QSize(350, 240));
        rgbTab = new QWidget();
        rgbTab->setObjectName("rgbTab");
        verticalLayout_3 = new QVBoxLayout(rgbTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(-1, 1, -1, 1);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        redHAC = new QPushButton(rgbTab);
        redHAC->setObjectName("redHAC");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(redHAC->sizePolicy().hasHeightForWidth());
        redHAC->setSizePolicy(sizePolicy1);
        redHAC->setMinimumSize(QSize(22, 48));
        redHAC->setMaximumSize(QSize(22, 48));
        redHAC->setText(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/processing/logsqrt-dark.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        redHAC->setIcon(icon);
        redHAC->setIconSize(QSize(40, 40));
        redHAC->setFlat(true);

        horizontalLayout_3->addWidget(redHAC);

        redGradient = new QLinearGradientCtrl(rgbTab);
        redGradient->setObjectName("redGradient");
        sizePolicy.setHeightForWidth(redGradient->sizePolicy().hasHeightForWidth());
        redGradient->setSizePolicy(sizePolicy);
        redGradient->setMinimumSize(QSize(0, 48));

        horizontalLayout_3->addWidget(redGradient);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        greenHAC = new QPushButton(rgbTab);
        greenHAC->setObjectName("greenHAC");
        sizePolicy1.setHeightForWidth(greenHAC->sizePolicy().hasHeightForWidth());
        greenHAC->setSizePolicy(sizePolicy1);
        greenHAC->setMinimumSize(QSize(22, 48));
        greenHAC->setMaximumSize(QSize(22, 48));
        greenHAC->setText(QString::fromUtf8(""));
        greenHAC->setIcon(icon);
        greenHAC->setIconSize(QSize(40, 40));
        greenHAC->setFlat(true);

        horizontalLayout_4->addWidget(greenHAC);

        greenGradient = new QLinearGradientCtrl(rgbTab);
        greenGradient->setObjectName("greenGradient");
        sizePolicy.setHeightForWidth(greenGradient->sizePolicy().hasHeightForWidth());
        greenGradient->setSizePolicy(sizePolicy);
        greenGradient->setMinimumSize(QSize(0, 48));

        horizontalLayout_4->addWidget(greenGradient);


        verticalLayout_3->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        blueHAC = new QPushButton(rgbTab);
        blueHAC->setObjectName("blueHAC");
        sizePolicy1.setHeightForWidth(blueHAC->sizePolicy().hasHeightForWidth());
        blueHAC->setSizePolicy(sizePolicy1);
        blueHAC->setMinimumSize(QSize(22, 48));
        blueHAC->setMaximumSize(QSize(22, 48));
        blueHAC->setText(QString::fromUtf8(""));
        blueHAC->setIcon(icon);
        blueHAC->setIconSize(QSize(40, 40));
        blueHAC->setFlat(true);

        horizontalLayout_5->addWidget(blueHAC);

        blueGradient = new QLinearGradientCtrl(rgbTab);
        blueGradient->setObjectName("blueGradient");
        sizePolicy.setHeightForWidth(blueGradient->sizePolicy().hasHeightForWidth());
        blueGradient->setSizePolicy(sizePolicy);
        blueGradient->setMinimumSize(QSize(0, 48));

        horizontalLayout_5->addWidget(blueGradient);


        verticalLayout_3->addLayout(horizontalLayout_5);

        linkedSettings = new QCheckBox(rgbTab);
        linkedSettings->setObjectName("linkedSettings");

        verticalLayout_3->addWidget(linkedSettings);

        tabWidget->addTab(rgbTab, QString());
        luminanceTab = new QWidget();
        luminanceTab->setObjectName("luminanceTab");
        verticalLayout_7 = new QVBoxLayout(luminanceTab);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setContentsMargins(-1, 1, -1, 1);
        darkness = new QLabel(luminanceTab);
        darkness->setObjectName("darkness");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(darkness->sizePolicy().hasHeightForWidth());
        darkness->setSizePolicy(sizePolicy2);
        darkness->setMinimumSize(QSize(275, 15));
        darkness->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        darkness->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_7->addWidget(darkness);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        darkLabel = new QLabel(luminanceTab);
        darkLabel->setObjectName("darkLabel");
        darkLabel->setMinimumSize(QSize(49, 37));
        darkLabel->setText(QString::fromUtf8("TextLabel"));

        horizontalLayout_6->addWidget(darkLabel);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        darkAngle = new QSlider(luminanceTab);
        darkAngle->setObjectName("darkAngle");
        darkAngle->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_4->addWidget(darkAngle);

        darkPower = new QSlider(luminanceTab);
        darkPower->setObjectName("darkPower");
        darkPower->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_4->addWidget(darkPower);


        horizontalLayout_6->addLayout(verticalLayout_4);


        verticalLayout_7->addLayout(horizontalLayout_6);

        midtone = new QLabel(luminanceTab);
        midtone->setObjectName("midtone");
        sizePolicy2.setHeightForWidth(midtone->sizePolicy().hasHeightForWidth());
        midtone->setSizePolicy(sizePolicy2);
        midtone->setMinimumSize(QSize(275, 15));
        midtone->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        midtone->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_7->addWidget(midtone);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        midLabel = new QLabel(luminanceTab);
        midLabel->setObjectName("midLabel");
        midLabel->setMinimumSize(QSize(49, 37));
        midLabel->setText(QString::fromUtf8("TextLabel"));

        horizontalLayout_7->addWidget(midLabel);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName("verticalLayout_5");
        midAngle = new QSlider(luminanceTab);
        midAngle->setObjectName("midAngle");
        midAngle->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_5->addWidget(midAngle);

        midTone = new QSlider(luminanceTab);
        midTone->setObjectName("midTone");
        midTone->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_5->addWidget(midTone);


        horizontalLayout_7->addLayout(verticalLayout_5);


        verticalLayout_7->addLayout(horizontalLayout_7);

        highlight = new QLabel(luminanceTab);
        highlight->setObjectName("highlight");
        sizePolicy2.setHeightForWidth(highlight->sizePolicy().hasHeightForWidth());
        highlight->setSizePolicy(sizePolicy2);
        highlight->setMinimumSize(QSize(275, 15));
        highlight->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        highlight->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_7->addWidget(highlight);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        highLabel = new QLabel(luminanceTab);
        highLabel->setObjectName("highLabel");
        highLabel->setMinimumSize(QSize(49, 37));
        highLabel->setText(QString::fromUtf8("TextLabel"));

        horizontalLayout_8->addWidget(highLabel);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName("verticalLayout_6");
        highAngle = new QSlider(luminanceTab);
        highAngle->setObjectName("highAngle");
        highAngle->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_6->addWidget(highAngle);

        highPower = new QSlider(luminanceTab);
        highPower->setObjectName("highPower");
        highPower->setOrientation(Qt::Orientation::Horizontal);

        verticalLayout_6->addWidget(highPower);


        horizontalLayout_8->addLayout(verticalLayout_6);


        verticalLayout_7->addLayout(horizontalLayout_8);

        tabWidget->addTab(luminanceTab, QString());
        saturationTab = new QWidget();
        saturationTab->setObjectName("saturationTab");
        verticalLayout_8 = new QVBoxLayout(saturationTab);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(-1, 1, -1, 1);
        saturationTitle = new QLabel(saturationTab);
        saturationTitle->setObjectName("saturationTitle");
        sizePolicy2.setHeightForWidth(saturationTitle->sizePolicy().hasHeightForWidth());
        saturationTitle->setSizePolicy(sizePolicy2);
        saturationTitle->setMinimumSize(QSize(275, 15));
        saturationTitle->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        saturationTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_8->addWidget(saturationTitle);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout_8->addItem(verticalSpacer);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        saturationLabel = new QLabel(saturationTab);
        saturationLabel->setObjectName("saturationLabel");
        saturationLabel->setMinimumSize(QSize(49, 18));
        saturationLabel->setText(QString::fromUtf8("TextLabel"));

        horizontalLayout_9->addWidget(saturationLabel);

        saturation = new QSlider(saturationTab);
        saturation->setObjectName("saturation");
        saturation->setOrientation(Qt::Orientation::Horizontal);
        saturation->setTickPosition(QSlider::TickPosition::TicksBelow);
        saturation->setTickInterval(10);

        horizontalLayout_9->addWidget(saturation);


        verticalLayout_8->addLayout(horizontalLayout_9);

        verticalSpacer_2 = new QSpacerItem(20, 123, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_2);

        tabWidget->addTab(saturationTab, QString());

        verticalLayout->addWidget(tabWidget);

        buttonWidget = new QWidget(widget);
        buttonWidget->setObjectName("buttonWidget");
        buttonWidget->setMinimumSize(QSize(350, 0));
        buttonWidget->setMaximumSize(QSize(350, 16777215));
        horizontalLayout = new QHBoxLayout(buttonWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(-1, 1, -1, 1);
        applyButton = new QPushButton(buttonWidget);
        applyButton->setObjectName("applyButton");

        horizontalLayout->addWidget(applyButton);

        undoButton = new QPushButton(buttonWidget);
        undoButton->setObjectName("undoButton");
        undoButton->setText(QString::fromUtf8(""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/processing/undo.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        undoButton->setIcon(icon1);
        undoButton->setFlat(true);

        horizontalLayout->addWidget(undoButton);

        settingsButton = new QPushButton(buttonWidget);
        settingsButton->setObjectName("settingsButton");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(settingsButton->sizePolicy().hasHeightForWidth());
        settingsButton->setSizePolicy(sizePolicy3);
        settingsButton->setText(QString::fromUtf8(""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/processing/settings.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        settingsButton->setIcon(icon2);
        settingsButton->setIconSize(QSize(24, 24));
        settingsButton->setFlat(true);

        horizontalLayout->addWidget(settingsButton);

        redoButton = new QPushButton(buttonWidget);
        redoButton->setObjectName("redoButton");
        redoButton->setText(QString::fromUtf8(""));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/processing/redo.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        redoButton->setIcon(icon3);
        redoButton->setFlat(true);

        horizontalLayout->addWidget(redoButton);

        resetButton = new QPushButton(buttonWidget);
        resetButton->setObjectName("resetButton");

        horizontalLayout->addWidget(resetButton);


        verticalLayout->addWidget(buttonWidget);


        horizontalLayout_2->addWidget(widget);

        histogram = new QLabel(DSS__ProcessingControls);
        histogram->setObjectName("histogram");
        sizePolicy2.setHeightForWidth(histogram->sizePolicy().hasHeightForWidth());
        histogram->setSizePolicy(sizePolicy2);
        histogram->setMinimumSize(QSize(450, 280));
        histogram->setMaximumSize(QSize(450, 280));

        horizontalLayout_2->addWidget(histogram);

        horizontalSpacer = new QSpacerItem(124, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        retranslateUi(DSS__ProcessingControls);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DSS__ProcessingControls);
    } // setupUi

    void retranslateUi(QWidget *DSS__ProcessingControls)
    {
        DSS__ProcessingControls->setWindowTitle(QCoreApplication::translate("DSS::ProcessingControls", "Form", nullptr));
#if QT_CONFIG(tooltip)
        redHAC->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Set Red Histogram Adjustment curve", "IDS_TT_REDADJUST"));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        greenHAC->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Set Green Histogram Adjustment curve", "IDS_TT_GREENADJUST"));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        blueHAC->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Set Blue Histogram Adjustment curve", "IDS_TT_BLUEADJUST"));
#endif // QT_CONFIG(tooltip)
        linkedSettings->setText(QCoreApplication::translate("DSS::ProcessingControls", "Linked settings", "IDC_LINK"));
        tabWidget->setTabText(tabWidget->indexOf(rgbTab), QCoreApplication::translate("DSS::ProcessingControls", "RGB/K Levels", "IDD_RGB"));
        darkness->setText(QCoreApplication::translate("DSS::ProcessingControls", "Darkness", "IDC_TEXT_DARKNESS"));
        midtone->setText(QCoreApplication::translate("DSS::ProcessingControls", "Midtone", "IDC_TEXT_MIDTONE"));
        highlight->setText(QCoreApplication::translate("DSS::ProcessingControls", "Highlight", "IDC_TEXT_HIGHLIGHT"));
        tabWidget->setTabText(tabWidget->indexOf(luminanceTab), QCoreApplication::translate("DSS::ProcessingControls", "Luminance", "IDD_LUMINANCE"));
        saturationTitle->setText(QCoreApplication::translate("DSS::ProcessingControls", "Saturation shift", "IDC_TEXT_SATURATION"));
        tabWidget->setTabText(tabWidget->indexOf(saturationTab), QCoreApplication::translate("DSS::ProcessingControls", "Saturation", "IDD_SATURATION"));
        applyButton->setText(QCoreApplication::translate("DSS::ProcessingControls", "Apply", "IDC_PROCESS"));
#if QT_CONFIG(tooltip)
        undoButton->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Undo last applied settings", "IDS_UNDOLASTSETTINGS"));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        settingsButton->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Save, Load, Manage settings ...", "IDS_MANAGESETTINGS"));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        redoButton->setToolTip(QCoreApplication::translate("DSS::ProcessingControls", "Redo last applied settings", "IDS_REDOLASTSETTINGS"));
#endif // QT_CONFIG(tooltip)
        resetButton->setText(QCoreApplication::translate("DSS::ProcessingControls", "Reset", "IDC_RESET"));
        histogram->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ProcessingControls: public Ui_ProcessingControls {};
} // namespace Ui
} // namespace DSS

#endif // UI_PROCESSINGCONTROLS_H
