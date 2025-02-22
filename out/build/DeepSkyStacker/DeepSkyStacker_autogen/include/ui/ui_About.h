/********************************************************************************
** Form generated from reading UI file 'About.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_About
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *html;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *comboBox;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QLabel *credits;
    QHBoxLayout *horizontalLayout;
    QCheckBox *cbCheckUpdate;
    QPushButton *aboutQt;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__About)
    {
        if (DSS__About->objectName().isEmpty())
            DSS__About->setObjectName("DSS__About");
        DSS__About->resize(480, 600);
        verticalLayout = new QVBoxLayout(DSS__About);
        verticalLayout->setObjectName("verticalLayout");
        html = new QLabel(DSS__About);
        html->setObjectName("html");
        html->setTextFormat(Qt::RichText);
        html->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        html->setWordWrap(true);
        html->setOpenExternalLinks(true);

        verticalLayout->addWidget(html);

        groupBox = new QGroupBox(DSS__About);
        groupBox->setObjectName("groupBox");
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName("comboBox");

        horizontalLayout_2->addWidget(comboBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        scrollArea = new QScrollArea(groupBox);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 442, 408));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        credits = new QLabel(scrollAreaWidgetContents);
        credits->setObjectName("credits");
        credits->setOpenExternalLinks(true);

        verticalLayout_3->addWidget(credits);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);


        verticalLayout->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        cbCheckUpdate = new QCheckBox(DSS__About);
        cbCheckUpdate->setObjectName("cbCheckUpdate");

        horizontalLayout->addWidget(cbCheckUpdate);

        aboutQt = new QPushButton(DSS__About);
        aboutQt->setObjectName("aboutQt");

        horizontalLayout->addWidget(aboutQt);

        buttonBox = new QDialogButtonBox(DSS__About);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);

        verticalLayout->setStretch(0, 6);
        verticalLayout->setStretch(1, 40);

        retranslateUi(DSS__About);

        QMetaObject::connectSlotsByName(DSS__About);
    } // setupUi

    void retranslateUi(QDialog *DSS__About)
    {
        DSS__About->setWindowTitle(QCoreApplication::translate("DSS::About", "About DeepSkyStacker", nullptr));
        html->setText(QString());
        groupBox->setTitle(QCoreApplication::translate("DSS::About", "Language", nullptr));
        label->setText(QCoreApplication::translate("DSS::About", "Select language:", nullptr));
        credits->setText(QString());
        cbCheckUpdate->setText(QCoreApplication::translate("DSS::About", "Check if newer version is available at startup", nullptr));
        aboutQt->setText(QCoreApplication::translate("DSS::About", "About &Qt", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class About: public Ui_About {};
} // namespace Ui
} // namespace DSS

#endif // UI_ABOUT_H
