/********************************************************************************
** Form generated from reading UI file 'AskRegistering.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ASKREGISTERING_H
#define UI_ASKREGISTERING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_AskRegistering
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QRadioButton *registerOne;
    QRadioButton *registerAll;
    QRadioButton *registerNone;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__AskRegistering)
    {
        if (DSS__AskRegistering->objectName().isEmpty())
            DSS__AskRegistering->setObjectName("DSS__AskRegistering");
        DSS__AskRegistering->resize(387, 213);
        verticalLayout_2 = new QVBoxLayout(DSS__AskRegistering);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 0, 20, 0);
        label = new QLabel(DSS__AskRegistering);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        registerOne = new QRadioButton(DSS__AskRegistering);
        registerOne->setObjectName("registerOne");
        registerOne->setChecked(true);

        verticalLayout->addWidget(registerOne);

        registerAll = new QRadioButton(DSS__AskRegistering);
        registerAll->setObjectName("registerAll");

        verticalLayout->addWidget(registerAll);

        registerNone = new QRadioButton(DSS__AskRegistering);
        registerNone->setObjectName("registerNone");

        verticalLayout->addWidget(registerNone);


        verticalLayout_2->addLayout(verticalLayout);

        buttonBox = new QDialogButtonBox(DSS__AskRegistering);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(DSS__AskRegistering);

        QMetaObject::connectSlotsByName(DSS__AskRegistering);
    } // setupUi

    void retranslateUi(QDialog *DSS__AskRegistering)
    {
        DSS__AskRegistering->setWindowTitle(QCoreApplication::translate("DSS::AskRegistering", "Unregistered light frame", nullptr));
        label->setText(QCoreApplication::translate("DSS::AskRegistering", "This light frame is not registered\n"
"(the stars have not been detected).\n"
"\n"
"Do you want...", nullptr));
        registerOne->setText(QCoreApplication::translate("DSS::AskRegistering", "to register this light frame", nullptr));
        registerAll->setText(QCoreApplication::translate("DSS::AskRegistering", "to register all the light frames", nullptr));
        registerNone->setText(QCoreApplication::translate("DSS::AskRegistering", "to continue without registering the light frame", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class AskRegistering: public Ui_AskRegistering {};
} // namespace Ui
} // namespace DSS

#endif // UI_ASKREGISTERING_H
