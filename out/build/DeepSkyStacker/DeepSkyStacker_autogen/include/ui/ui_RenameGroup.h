/********************************************************************************
** Form generated from reading UI file 'RenameGroup.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENAMEGROUP_H
#define UI_RENAMEGROUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_RenameGroup
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__RenameGroup)
    {
        if (DSS__RenameGroup->objectName().isEmpty())
            DSS__RenameGroup->setObjectName("DSS__RenameGroup");
        DSS__RenameGroup->resize(311, 78);
        verticalLayout = new QVBoxLayout(DSS__RenameGroup);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(DSS__RenameGroup);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(DSS__RenameGroup);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(DSS__RenameGroup);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DSS__RenameGroup);

        QMetaObject::connectSlotsByName(DSS__RenameGroup);
    } // setupUi

    void retranslateUi(QDialog *DSS__RenameGroup)
    {
        DSS__RenameGroup->setWindowTitle(QCoreApplication::translate("DSS::RenameGroup", "Rename Group", nullptr));
        label->setText(QCoreApplication::translate("DSS::RenameGroup", "Group Name:", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class RenameGroup: public Ui_RenameGroup {};
} // namespace Ui
} // namespace DSS

#endif // UI_RENAMEGROUP_H
