/********************************************************************************
** Form generated from reading UI file 'ProgressDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSDLG_H
#define UI_PROGRESSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_ProgressDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *ProcessText1;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_4;
    QProgressBar *ProgressBar2;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *TimeRemaining;
    QLabel *Processors;
    QPushButton *StopButton;
    QHBoxLayout *horizontalLayoutProgress1;
    QSpacerItem *horizontalSpacer_2;
    QProgressBar *ProgressBar1;
    QSpacerItem *horizontalSpacer;
    QLabel *ProcessText2;

    void setupUi(QDialog *DSS__ProgressDlg)
    {
        if (DSS__ProgressDlg->objectName().isEmpty())
            DSS__ProgressDlg->setObjectName("DSS__ProgressDlg");
        DSS__ProgressDlg->resize(653, 180);
        verticalLayout = new QVBoxLayout(DSS__ProgressDlg);
        verticalLayout->setObjectName("verticalLayout");
        ProcessText1 = new QLabel(DSS__ProgressDlg);
        ProcessText1->setObjectName("ProcessText1");

        verticalLayout->addWidget(ProcessText1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        ProgressBar2 = new QProgressBar(DSS__ProgressDlg);
        ProgressBar2->setObjectName("ProgressBar2");
        ProgressBar2->setMinimumSize(QSize(500, 0));
        ProgressBar2->setValue(24);

        horizontalLayout_3->addWidget(ProgressBar2);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        TimeRemaining = new QLabel(DSS__ProgressDlg);
        TimeRemaining->setObjectName("TimeRemaining");

        horizontalLayout_4->addWidget(TimeRemaining);

        Processors = new QLabel(DSS__ProgressDlg);
        Processors->setObjectName("Processors");

        horizontalLayout_4->addWidget(Processors);

        StopButton = new QPushButton(DSS__ProgressDlg);
        StopButton->setObjectName("StopButton");

        horizontalLayout_4->addWidget(StopButton);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayoutProgress1 = new QHBoxLayout();
        horizontalLayoutProgress1->setSpacing(6);
        horizontalLayoutProgress1->setObjectName("horizontalLayoutProgress1");
        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutProgress1->addItem(horizontalSpacer_2);

        ProgressBar1 = new QProgressBar(DSS__ProgressDlg);
        ProgressBar1->setObjectName("ProgressBar1");
        ProgressBar1->setMinimumSize(QSize(500, 0));
        ProgressBar1->setValue(24);

        horizontalLayoutProgress1->addWidget(ProgressBar1);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayoutProgress1->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayoutProgress1);

        ProcessText2 = new QLabel(DSS__ProgressDlg);
        ProcessText2->setObjectName("ProcessText2");

        verticalLayout->addWidget(ProcessText2);


        retranslateUi(DSS__ProgressDlg);

        QMetaObject::connectSlotsByName(DSS__ProgressDlg);
    } // setupUi

    void retranslateUi(QDialog *DSS__ProgressDlg)
    {
        DSS__ProgressDlg->setWindowTitle(QCoreApplication::translate("DSS::ProgressDlg", "Processing ...", nullptr));
        ProcessText1->setText(QString());
        TimeRemaining->setText(QString());
        Processors->setText(QString());
        StopButton->setText(QCoreApplication::translate("DSS::ProgressDlg", "Cancel", nullptr));
        ProcessText2->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ProgressDlg: public Ui_ProgressDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_PROGRESSDLG_H
