/********************************************************************************
** Form generated from reading UI file 'StackingDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STACKINGDLG_H
#define UI_STACKINGDLG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QLinearGradientCtrl.h"
#include "imageview.h"

namespace DSS {

class Ui_StackingDlg
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *information;
    QToolButton *fourCorners;
    QLinearGradientCtrl *gamma;
    QHBoxLayout *horizontalLayout_2;
    DSS::ImageView *picture;

    void setupUi(QWidget *DSS__StackingDlg)
    {
        if (DSS__StackingDlg->objectName().isEmpty())
            DSS__StackingDlg->setObjectName("DSS__StackingDlg");
        DSS__StackingDlg->resize(840, 266);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DSS__StackingDlg->sizePolicy().hasHeightForWidth());
        DSS__StackingDlg->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(DSS__StackingDlg);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout->setContentsMargins(-1, -1, -1, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        information = new QLabel(DSS__StackingDlg);
        information->setObjectName("information");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(information->sizePolicy().hasHeightForWidth());
        information->setSizePolicy(sizePolicy1);
        information->setMinimumSize(QSize(275, 38));
        information->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        information->setText(QString::fromUtf8(""));

        horizontalLayout->addWidget(information);

        fourCorners = new QToolButton(DSS__StackingDlg);
        fourCorners->setObjectName("fourCorners");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(fourCorners->sizePolicy().hasHeightForWidth());
        fourCorners->setSizePolicy(sizePolicy2);
        fourCorners->setMinimumSize(QSize(38, 38));
        fourCorners->setMaximumSize(QSize(64, 64));
        fourCorners->setStyleSheet(QString::fromUtf8("border: none"));
        fourCorners->setText(QString::fromUtf8("..."));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/stacking/4corners.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        fourCorners->setIcon(icon);
        fourCorners->setIconSize(QSize(38, 38));

        horizontalLayout->addWidget(fourCorners);

        gamma = new QLinearGradientCtrl(DSS__StackingDlg);
        gamma->setObjectName("gamma");
        gamma->setMinimumSize(QSize(170, 0));

        horizontalLayout->addWidget(gamma);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        picture = new DSS::ImageView(DSS__StackingDlg);
        picture->setObjectName("picture");
        sizePolicy.setHeightForWidth(picture->sizePolicy().hasHeightForWidth());
        picture->setSizePolicy(sizePolicy);
        picture->setMinimumSize(QSize(450, 100));

        horizontalLayout_2->addWidget(picture);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(DSS__StackingDlg);

        QMetaObject::connectSlotsByName(DSS__StackingDlg);
    } // setupUi

    void retranslateUi(QWidget *DSS__StackingDlg)
    {
        (void)DSS__StackingDlg;
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class StackingDlg: public Ui_StackingDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_STACKINGDLG_H
