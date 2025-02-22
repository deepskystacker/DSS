/********************************************************************************
** Form generated from reading UI file 'SaveEditChanges.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVEEDITCHANGES_H
#define UI_SAVEEDITCHANGES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_SaveEditChanges
{
public:
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *saveDontAsk;
    QRadioButton *discardDontAsk;
    QRadioButton *askAlways;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__SaveEditChanges)
    {
        if (DSS__SaveEditChanges->objectName().isEmpty())
            DSS__SaveEditChanges->setObjectName("DSS__SaveEditChanges");
        DSS__SaveEditChanges->resize(400, 254);
        verticalLayout_2 = new QVBoxLayout(DSS__SaveEditChanges);
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(DSS__SaveEditChanges);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);

        groupBox = new QGroupBox(DSS__SaveEditChanges);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        saveDontAsk = new QRadioButton(groupBox);
        saveDontAsk->setObjectName("saveDontAsk");

        verticalLayout->addWidget(saveDontAsk);

        discardDontAsk = new QRadioButton(groupBox);
        discardDontAsk->setObjectName("discardDontAsk");

        verticalLayout->addWidget(discardDontAsk);

        askAlways = new QRadioButton(groupBox);
        askAlways->setObjectName("askAlways");

        verticalLayout->addWidget(askAlways);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setMinimumSize(QSize(0, 0));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_2->setWordWrap(true);

        verticalLayout->addWidget(label_2);


        verticalLayout_2->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(DSS__SaveEditChanges);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Discard|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(DSS__SaveEditChanges);

        QMetaObject::connectSlotsByName(DSS__SaveEditChanges);
    } // setupUi

    void retranslateUi(QDialog *DSS__SaveEditChanges)
    {
        DSS__SaveEditChanges->setWindowTitle(QCoreApplication::translate("DSS::SaveEditChanges", "Save Changes?", nullptr));
        label->setText(QCoreApplication::translate("DSS::SaveEditChanges", "You have made some changes (stars or comet positions)\n"
"\n"
"Do you want to save them?", "IDD_SAVEEDITCHANGES"));
        groupBox->setTitle(QCoreApplication::translate("DSS::SaveEditChanges", "The next time...", "IDD_SAVEEDITCHANGES"));
        saveDontAsk->setText(QCoreApplication::translate("DSS::SaveEditChanges", "Save changes without asking", "IDC_SAVEWITHOUTASKING"));
        discardDontAsk->setText(QCoreApplication::translate("DSS::SaveEditChanges", "Discard changes without asking", "IDC_DONTSAVEWITHOUTASKING"));
        askAlways->setText(QCoreApplication::translate("DSS::SaveEditChanges", "Always ask", "IDC_ASKAGAIN"));
        label_2->setText(QCoreApplication::translate("DSS::SaveEditChanges", "You can access these options by right clicking on the Save button", "IDD_SAVEEDITCHANGES"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class SaveEditChanges: public Ui_SaveEditChanges {};
} // namespace Ui
} // namespace DSS

#endif // UI_SAVEEDITCHANGES_H
