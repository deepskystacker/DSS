/********************************************************************************
** Form generated from reading UI file 'CheckAbove.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHECKABOVE_H
#define UI_CHECKABOVE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

namespace DSS {

class Ui_CheckAbove
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__CheckAbove)
    {
        if (DSS__CheckAbove->objectName().isEmpty())
            DSS__CheckAbove->setObjectName("DSS__CheckAbove");
        DSS__CheckAbove->resize(333, 94);
        gridLayout = new QGridLayout(DSS__CheckAbove);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(DSS__CheckAbove);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(DSS__CheckAbove);
        lineEdit->setObjectName("lineEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setMaximumSize(QSize(80, 16777215));

        gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

        buttonBox = new QDialogButtonBox(DSS__CheckAbove);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 1, 1, 1);


        retranslateUi(DSS__CheckAbove);

        QMetaObject::connectSlotsByName(DSS__CheckAbove);
    } // setupUi

    void retranslateUi(QDialog *DSS__CheckAbove)
    {
        DSS__CheckAbove->setWindowTitle(QCoreApplication::translate("DSS::CheckAbove", "Check all pictures above...", "IDD_CHECKABOVE"));
        label->setText(QCoreApplication::translate("DSS::CheckAbove", "Minimum quality:", "IDD_CHECKABOVE"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class CheckAbove: public Ui_CheckAbove {};
} // namespace Ui
} // namespace DSS

#endif // UI_CHECKABOVE_H
