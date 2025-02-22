/********************************************************************************
** Form generated from reading UI file 'StackRecap.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STACKRECAP_H
#define UI_STACKRECAP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

namespace DSS {

class Ui_StackRecap
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QPushButton *recommended;
    QPushButton *stackSettings;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__StackRecap)
    {
        if (DSS__StackRecap->objectName().isEmpty())
            DSS__StackRecap->setObjectName("DSS__StackRecap");
        DSS__StackRecap->resize(532, 406);
        gridLayout = new QGridLayout(DSS__StackRecap);
        gridLayout->setObjectName("gridLayout");
        textBrowser = new QTextBrowser(DSS__StackRecap);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setHtml(QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Tahoma'; font-size:8.25pt;\"><br /></p></body></html>"));

        gridLayout->addWidget(textBrowser, 0, 0, 1, 2);

        recommended = new QPushButton(DSS__StackRecap);
        recommended->setObjectName("recommended");

        gridLayout->addWidget(recommended, 1, 0, 1, 1);

        stackSettings = new QPushButton(DSS__StackRecap);
        stackSettings->setObjectName("stackSettings");

        gridLayout->addWidget(stackSettings, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DSS__StackRecap);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 1, 1, 1);


        retranslateUi(DSS__StackRecap);

        QMetaObject::connectSlotsByName(DSS__StackRecap);
    } // setupUi

    void retranslateUi(QDialog *DSS__StackRecap)
    {
        DSS__StackRecap->setWindowTitle(QCoreApplication::translate("DSS::StackRecap", "Stacking Steps", nullptr));
        recommended->setText(QCoreApplication::translate("DSS::StackRecap", "Recommended Settings ...", nullptr));
        stackSettings->setText(QCoreApplication::translate("DSS::StackRecap", "Stacking Settings ...", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class StackRecap: public Ui_StackRecap {};
} // namespace Ui
} // namespace DSS

#endif // UI_STACKRECAP_H
