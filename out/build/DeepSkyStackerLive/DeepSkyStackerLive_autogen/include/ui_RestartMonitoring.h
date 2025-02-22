/********************************************************************************
** Form generated from reading UI file 'RestartMonitoring.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESTARTMONITORING_H
#define UI_RESTARTMONITORING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_RestartMonitoring
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacer;
    QRadioButton *createNew;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *spacer_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *spacer_3;
    QRadioButton *useCurrent;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *spacer_4;
    QRadioButton *dropPending;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *spacer_5;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *spacer_6;
    QRadioButton *usePending;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;
    QButtonGroup *buttonGroup2;
    QButtonGroup *buttonGroup1;

    void setupUi(QDialog *DSS__RestartMonitoring)
    {
        if (DSS__RestartMonitoring->objectName().isEmpty())
            DSS__RestartMonitoring->setObjectName("DSS__RestartMonitoring");
        DSS__RestartMonitoring->resize(400, 254);
        verticalLayout_2 = new QVBoxLayout(DSS__RestartMonitoring);
        verticalLayout_2->setObjectName("verticalLayout_2");
        groupBox = new QGroupBox(DSS__RestartMonitoring);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        spacer = new QSpacerItem(13, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(spacer);

        createNew = new QRadioButton(groupBox);
        buttonGroup1 = new QButtonGroup(DSS__RestartMonitoring);
        buttonGroup1->setObjectName("buttonGroup1");
        buttonGroup1->addButton(createNew);
        createNew->setObjectName("createNew");

        horizontalLayout->addWidget(createNew);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        spacer_2 = new QSpacerItem(30, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(spacer_2);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        spacer_3 = new QSpacerItem(13, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(spacer_3);

        useCurrent = new QRadioButton(groupBox);
        buttonGroup1->addButton(useCurrent);
        useCurrent->setObjectName("useCurrent");

        horizontalLayout_3->addWidget(useCurrent);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        spacer_4 = new QSpacerItem(13, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(spacer_4);

        dropPending = new QRadioButton(groupBox);
        buttonGroup2 = new QButtonGroup(DSS__RestartMonitoring);
        buttonGroup2->setObjectName("buttonGroup2");
        buttonGroup2->addButton(dropPending);
        dropPending->setObjectName("dropPending");

        horizontalLayout_4->addWidget(dropPending);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        spacer_5 = new QSpacerItem(30, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(spacer_5);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        horizontalLayout_5->addWidget(label_2);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        spacer_6 = new QSpacerItem(13, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(spacer_6);

        usePending = new QRadioButton(groupBox);
        buttonGroup2->addButton(usePending);
        usePending->setObjectName("usePending");

        horizontalLayout_6->addWidget(usePending);


        verticalLayout->addLayout(horizontalLayout_6);


        verticalLayout_2->addWidget(groupBox);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName("hboxLayout");
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hboxLayout->addItem(spacerItem);

        buttonBox = new QDialogButtonBox(DSS__RestartMonitoring);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        hboxLayout->addWidget(buttonBox);


        verticalLayout_2->addLayout(hboxLayout);


        retranslateUi(DSS__RestartMonitoring);

        QMetaObject::connectSlotsByName(DSS__RestartMonitoring);
    } // setupUi

    void retranslateUi(QDialog *DSS__RestartMonitoring)
    {
        DSS__RestartMonitoring->setWindowTitle(QCoreApplication::translate("DSS::RestartMonitoring", "Restart Monitoring", "IDD_RESTARTMONITORING"));
        groupBox->setTitle(QCoreApplication::translate("DSS::RestartMonitoring", "Before restarting do you want to", nullptr));
        createNew->setText(QCoreApplication::translate("DSS::RestartMonitoring", "Create a new empty stacked image", "IDC_CREATENEWSTACKEDIMAGE"));
        label->setText(QCoreApplication::translate("DSS::RestartMonitoring", "or", nullptr));
        useCurrent->setText(QCoreApplication::translate("DSS::RestartMonitoring", "Start from the current stacked image", "IDC_STARTFROMCURRENTSTACKEDIMAGE"));
        dropPending->setText(QCoreApplication::translate("DSS::RestartMonitoring", "Drop all the pending images", "IDC_DROPPENDINGIMAGES"));
        label_2->setText(QCoreApplication::translate("DSS::RestartMonitoring", "or", nullptr));
        usePending->setText(QCoreApplication::translate("DSS::RestartMonitoring", "Use and process the pending images", "IDC_USEPENDINGIMAGES"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class RestartMonitoring: public Ui_RestartMonitoring {};
} // namespace Ui
} // namespace DSS

#endif // UI_RESTARTMONITORING_H
