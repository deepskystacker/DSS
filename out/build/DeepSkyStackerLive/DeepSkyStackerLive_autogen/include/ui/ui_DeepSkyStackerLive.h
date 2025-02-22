/********************************************************************************
** Form generated from reading UI file 'DeepSkyStackerLive.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEEPSKYSTACKERLIVE_H
#define UI_DEEPSKYSTACKERLIVE_H

#include <ChartTab.h>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "imageviewer.h"
#include "settingstab.h"

QT_BEGIN_NAMESPACE

class Ui_DeepSkyStackerLive
{
public:
    QAction *actionMonitor;
    QAction *actionStack;
    QAction *actionStop;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *monitorText;
    QLabel *folderName;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QToolBar *toolbar;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QProgressBar *progressBar;
    QLabel *statusMessage;
    QTabWidget *tabWidget;
    DSS::ImageViewer *stackedImage;
    DSS::ImageViewer *lastImage;
    DSS::ChartTab *chartTab;
    QTableWidget *imageList;
    QTextEdit *messageLog;
    DSS::SettingsTab *settingsTab;

    void setupUi(QWidget *DeepSkyStackerLive)
    {
        if (DeepSkyStackerLive->objectName().isEmpty())
            DeepSkyStackerLive->setObjectName("DeepSkyStackerLive");
        DeepSkyStackerLive->resize(757, 748);
        DeepSkyStackerLive->setMinimumSize(QSize(757, 748));
        DeepSkyStackerLive->setWindowTitle(QString::fromUtf8("DeepSkyStackerLive"));
        actionMonitor = new QAction(DeepSkyStackerLive);
        actionMonitor->setObjectName("actionMonitor");
        actionMonitor->setCheckable(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Monitor-Up.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon.addFile(QString::fromUtf8(":/Monitor-Down.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        actionMonitor->setIcon(icon);
        actionStack = new QAction(DeepSkyStackerLive);
        actionStack->setObjectName("actionStack");
        actionStack->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Stack-Up.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1.addFile(QString::fromUtf8(":/Stack-Down.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        actionStack->setIcon(icon1);
        actionStop = new QAction(DeepSkyStackerLive);
        actionStop->setObjectName("actionStop");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/StopSign.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionStop->setIcon(icon2);
        verticalLayout_4 = new QVBoxLayout(DeepSkyStackerLive);
        verticalLayout_4->setObjectName("verticalLayout_4");
        widget_2 = new QWidget(DeepSkyStackerLive);
        widget_2->setObjectName("widget_2");
        widget_2->setMaximumSize(QSize(16777215, 138));
        widget_2->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        verticalLayout_2 = new QVBoxLayout(widget_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        monitorText = new QLabel(widget_2);
        monitorText->setObjectName("monitorText");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(monitorText->sizePolicy().hasHeightForWidth());
        monitorText->setSizePolicy(sizePolicy);
        monitorText->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));

        horizontalLayout->addWidget(monitorText);

        folderName = new QLabel(widget_2);
        folderName->setObjectName("folderName");
        folderName->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));

        horizontalLayout->addWidget(folderName);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        toolbar = new QToolBar(widget_2);
        toolbar->setObjectName("toolbar");
        sizePolicy.setHeightForWidth(toolbar->sizePolicy().hasHeightForWidth());
        toolbar->setSizePolicy(sizePolicy);
        toolbar->setMinimumSize(QSize(0, 0));
        toolbar->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));
        toolbar->setMovable(false);
        toolbar->setIconSize(QSize(48, 48));
        toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        toolbar->setFloatable(false);

        horizontalLayout_3->addWidget(toolbar);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer = new QSpacerItem(20, 11, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        progressBar = new QProgressBar(widget_2);
        progressBar->setObjectName("progressBar");
        progressBar->setMinimumSize(QSize(0, 36));
        progressBar->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        statusMessage = new QLabel(widget_2);
        statusMessage->setObjectName("statusMessage");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(statusMessage->sizePolicy().hasHeightForWidth());
        statusMessage->setSizePolicy(sizePolicy1);
        statusMessage->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));
        statusMessage->setText(QString::fromUtf8(""));

        verticalLayout->addWidget(statusMessage);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_4->addWidget(widget_2);

        tabWidget = new QTabWidget(DeepSkyStackerLive);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setMinimumSize(QSize(730, 598));
        tabWidget->setMaximumSize(QSize(16777215, 16777215));
        stackedImage = new DSS::ImageViewer();
        stackedImage->setObjectName("stackedImage");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(stackedImage->sizePolicy().hasHeightForWidth());
        stackedImage->setSizePolicy(sizePolicy2);
        tabWidget->addTab(stackedImage, QString());
        lastImage = new DSS::ImageViewer();
        lastImage->setObjectName("lastImage");
        sizePolicy2.setHeightForWidth(lastImage->sizePolicy().hasHeightForWidth());
        lastImage->setSizePolicy(sizePolicy2);
        tabWidget->addTab(lastImage, QString());
        chartTab = new DSS::ChartTab();
        chartTab->setObjectName("chartTab");
        tabWidget->addTab(chartTab, QString());
        imageList = new QTableWidget();
        imageList->setObjectName("imageList");
        sizePolicy2.setHeightForWidth(imageList->sizePolicy().hasHeightForWidth());
        imageList->setSizePolicy(sizePolicy2);
        imageList->setAlternatingRowColors(true);
        tabWidget->addTab(imageList, QString());
        messageLog = new QTextEdit();
        messageLog->setObjectName("messageLog");
        sizePolicy2.setHeightForWidth(messageLog->sizePolicy().hasHeightForWidth());
        messageLog->setSizePolicy(sizePolicy2);
        messageLog->setReadOnly(true);
        messageLog->setHtml(QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>"));
        tabWidget->addTab(messageLog, QString());
        settingsTab = new DSS::SettingsTab();
        settingsTab->setObjectName("settingsTab");
        tabWidget->addTab(settingsTab, QString());

        verticalLayout_4->addWidget(tabWidget);


        toolbar->addAction(actionMonitor);
        toolbar->addAction(actionStack);
        toolbar->addAction(actionStop);

        retranslateUi(DeepSkyStackerLive);

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(DeepSkyStackerLive);
    } // setupUi

    void retranslateUi(QWidget *DeepSkyStackerLive)
    {
        actionMonitor->setText(QCoreApplication::translate("DeepSkyStackerLive", "Monitor", "IDC_MONITOR"));
        actionMonitor->setIconText(QCoreApplication::translate("DeepSkyStackerLive", "Monitor", "IDC_MONITOR"));
        actionStack->setText(QCoreApplication::translate("DeepSkyStackerLive", "Stack", "IDC_STACK_DSSLIVE"));
        actionStack->setIconText(QCoreApplication::translate("DeepSkyStackerLive", "Stack", "IDC_STACK_DSSLIVE"));
        actionStop->setText(QCoreApplication::translate("DeepSkyStackerLive", "Stop", "IDC_STOP"));
        actionStop->setIconText(QCoreApplication::translate("DeepSkyStackerLive", "Stop", "IDC_STOP"));
        monitorText->setText(QCoreApplication::translate("DeepSkyStackerLive", "Monitoring", "IDC_MONITORINGRECT"));
#if QT_CONFIG(tooltip)
        folderName->setToolTip(QCoreApplication::translate("DeepSkyStackerLive", "Click here to select the Monitored Folder", "IDC_MONITOREDFOLDER"));
#endif // QT_CONFIG(tooltip)
        folderName->setText(QCoreApplication::translate("DeepSkyStackerLive", "Click here to select the Monitored Folder", "IDC_MONITOREDFOLDER"));
        progressBar->setFormat(QString());
        tabWidget->setTabText(tabWidget->indexOf(stackedImage), QCoreApplication::translate("DeepSkyStackerLive", "Stacked Image", "IDC_STACKEDIMAGE"));
        tabWidget->setTabText(tabWidget->indexOf(lastImage), QCoreApplication::translate("DeepSkyStackerLive", "Last Image", "IDC_LASTIMAGE"));
        tabWidget->setTabText(tabWidget->indexOf(chartTab), QCoreApplication::translate("DeepSkyStackerLive", "Graphs", "IDC_GRAPHS"));
        tabWidget->setTabText(tabWidget->indexOf(imageList), QCoreApplication::translate("DeepSkyStackerLive", "Image List", "IDC_IMAGELIST"));
        tabWidget->setTabText(tabWidget->indexOf(messageLog), QCoreApplication::translate("DeepSkyStackerLive", "Log", "IDC_LOGLIST"));
        tabWidget->setTabText(tabWidget->indexOf(settingsTab), QCoreApplication::translate("DeepSkyStackerLive", "Settings", "IDC_WARNINGS"));
        (void)DeepSkyStackerLive;
    } // retranslateUi

};

namespace Ui {
    class DeepSkyStackerLive: public Ui_DeepSkyStackerLive {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEEPSKYSTACKERLIVE_H
