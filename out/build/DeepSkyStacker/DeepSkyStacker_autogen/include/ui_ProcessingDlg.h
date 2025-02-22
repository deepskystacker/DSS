/********************************************************************************
** Form generated from reading UI file 'ProcessingDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSINGDLG_H
#define UI_PROCESSINGDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "imageview.h"

namespace DSS {

class Ui_ProcessingDlg
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *information;
    DSS::ImageView *picture;
    QProgressBar *progressBar;

    void setupUi(QWidget *DSS__ProcessingDlg)
    {
        if (DSS__ProcessingDlg->objectName().isEmpty())
            DSS__ProcessingDlg->setObjectName("DSS__ProcessingDlg");
        DSS__ProcessingDlg->resize(784, 222);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DSS__ProcessingDlg->sizePolicy().hasHeightForWidth());
        DSS__ProcessingDlg->setSizePolicy(sizePolicy);
        DSS__ProcessingDlg->setWindowTitle(QString::fromUtf8("ProcessingDlg"));
        verticalLayout_2 = new QVBoxLayout(DSS__ProcessingDlg);
        verticalLayout_2->setObjectName("verticalLayout_2");
        information = new QLabel(DSS__ProcessingDlg);
        information->setObjectName("information");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(information->sizePolicy().hasHeightForWidth());
        information->setSizePolicy(sizePolicy1);
        information->setMinimumSize(QSize(275, 38));
        information->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))"));
        information->setText(QString::fromUtf8(""));

        verticalLayout_2->addWidget(information);

        picture = new DSS::ImageView(DSS__ProcessingDlg);
        picture->setObjectName("picture");
        sizePolicy.setHeightForWidth(picture->sizePolicy().hasHeightForWidth());
        picture->setSizePolicy(sizePolicy);
        picture->setMinimumSize(QSize(450, 100));

        verticalLayout_2->addWidget(picture);

        progressBar = new QProgressBar(DSS__ProcessingDlg);
        progressBar->setObjectName("progressBar");
        progressBar->setMaximumSize(QSize(16777215, 4));
        progressBar->setValue(24);
        progressBar->setTextVisible(false);

        verticalLayout_2->addWidget(progressBar);


        retranslateUi(DSS__ProcessingDlg);

        QMetaObject::connectSlotsByName(DSS__ProcessingDlg);
    } // setupUi

    void retranslateUi(QWidget *DSS__ProcessingDlg)
    {
        (void)DSS__ProcessingDlg;
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ProcessingDlg: public Ui_ProcessingDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_PROCESSINGDLG_H
