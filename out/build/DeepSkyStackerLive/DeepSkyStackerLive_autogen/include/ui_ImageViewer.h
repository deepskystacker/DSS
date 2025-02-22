/********************************************************************************
** Form generated from reading UI file 'ImageViewer.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEVIEWER_H
#define UI_IMAGEVIEWER_H

#include <QLinearGradientCtrl.h>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "imageview.h"

QT_BEGIN_NAMESPACE

class Ui_ImageViewer
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QLabel *information;
    QLabel *copyToClipboard;
    QToolButton *fourCorners;
    QLinearGradientCtrl *gamma;
    QHBoxLayout *horizontalLayout_2;
    DSS::ImageView *picture;

    void setupUi(QWidget *ImageViewer)
    {
        if (ImageViewer->objectName().isEmpty())
            ImageViewer->setObjectName("ImageViewer");
        ImageViewer->resize(667, 620);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ImageViewer->sizePolicy().hasHeightForWidth());
        ImageViewer->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(ImageViewer);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout->setContentsMargins(-1, -1, -1, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        widget = new QWidget(ImageViewer);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setMinimumSize(QSize(275, 0));
        widget->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(-1, 2, 9, 2);
        information = new QLabel(widget);
        information->setObjectName("information");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(information->sizePolicy().hasHeightForWidth());
        information->setSizePolicy(sizePolicy2);
        information->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));

        verticalLayout_2->addWidget(information);

        copyToClipboard = new QLabel(widget);
        copyToClipboard->setObjectName("copyToClipboard");
        sizePolicy2.setHeightForWidth(copyToClipboard->sizePolicy().hasHeightForWidth());
        copyToClipboard->setSizePolicy(sizePolicy2);
        copyToClipboard->setStyleSheet(QString::fromUtf8("background: qcolor(rgba(0,0,0,0))"));

        verticalLayout_2->addWidget(copyToClipboard);


        horizontalLayout->addWidget(widget);

        fourCorners = new QToolButton(ImageViewer);
        fourCorners->setObjectName("fourCorners");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(fourCorners->sizePolicy().hasHeightForWidth());
        fourCorners->setSizePolicy(sizePolicy3);
        fourCorners->setMinimumSize(QSize(38, 38));
        fourCorners->setMaximumSize(QSize(64, 64));
        fourCorners->setStyleSheet(QString::fromUtf8("border: none"));
        fourCorners->setText(QString::fromUtf8("..."));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/4Corners.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        fourCorners->setIcon(icon);
        fourCorners->setIconSize(QSize(38, 38));

        horizontalLayout->addWidget(fourCorners);

        gamma = new QLinearGradientCtrl(ImageViewer);
        gamma->setObjectName("gamma");
        gamma->setMinimumSize(QSize(170, 0));

        horizontalLayout->addWidget(gamma);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        picture = new DSS::ImageView(ImageViewer);
        picture->setObjectName("picture");
        sizePolicy.setHeightForWidth(picture->sizePolicy().hasHeightForWidth());
        picture->setSizePolicy(sizePolicy);
        picture->setMinimumSize(QSize(450, 300));

        horizontalLayout_2->addWidget(picture);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(ImageViewer);

        QMetaObject::connectSlotsByName(ImageViewer);
    } // setupUi

    void retranslateUi(QWidget *ImageViewer)
    {
        information->setText(QString());
        copyToClipboard->setText(QCoreApplication::translate("ImageViewer", "<a href='.' style='text-decoration: none;'>Copy current picture to clipboard</a>", "IDC_COPYTOCLIPBOARD"));
        (void)ImageViewer;
    } // retranslateUi

};

namespace Ui {
    class ImageViewer: public Ui_ImageViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEVIEWER_H
