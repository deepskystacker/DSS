/********************************************************************************
** Form generated from reading UI file 'RecommendedSettings.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RECOMMENDEDSETTINGS_H
#define UI_RECOMMENDEDSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextBrowser>

namespace DSS {

class Ui_RecommendedSettings
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBox;

    void setupUi(QDialog *DSS__RecommendedSettings)
    {
        if (DSS__RecommendedSettings->objectName().isEmpty())
            DSS__RecommendedSettings->setObjectName("DSS__RecommendedSettings");
        DSS__RecommendedSettings->resize(532, 406);
        gridLayout = new QGridLayout(DSS__RecommendedSettings);
        gridLayout->setObjectName("gridLayout");
        textBrowser = new QTextBrowser(DSS__RecommendedSettings);
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

        buttonBox = new QDialogButtonBox(DSS__RecommendedSettings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 1, 1, 1);

        checkBox = new QCheckBox(DSS__RecommendedSettings);
        checkBox->setObjectName("checkBox");

        gridLayout->addWidget(checkBox, 1, 0, 1, 1);


        retranslateUi(DSS__RecommendedSettings);

        QMetaObject::connectSlotsByName(DSS__RecommendedSettings);
    } // setupUi

    void retranslateUi(QDialog *DSS__RecommendedSettings)
    {
        DSS__RecommendedSettings->setWindowTitle(QCoreApplication::translate("DSS::RecommendedSettings", "Recommended Settings", nullptr));
        checkBox->setText(QCoreApplication::translate("DSS::RecommendedSettings", "Show all recommendations", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class RecommendedSettings: public Ui_RecommendedSettings {};
} // namespace Ui
} // namespace DSS

#endif // UI_RECOMMENDEDSETTINGS_H
