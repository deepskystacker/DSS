/********************************************************************************
** Form generated from reading UI file 'OutputTab.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTPUTTAB_H
#define UI_OUTPUTTAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_OutputTab
{
public:
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *createOutput;
    QCheckBox *createHTML;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *autoSave;
    QRadioButton *fileListName;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *appendNumber;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *refFrameFolder;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *fileListFolder;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *otherFolder;
    QPushButton *outputFolder;
    QLabel *label1;

    void setupUi(QWidget *DSS__OutputTab)
    {
        if (DSS__OutputTab->objectName().isEmpty())
            DSS__OutputTab->setObjectName("DSS__OutputTab");
        DSS__OutputTab->resize(524, 377);
        verticalLayout_4 = new QVBoxLayout(DSS__OutputTab);
        verticalLayout_4->setObjectName("verticalLayout_4");
        label = new QLabel(DSS__OutputTab);
        label->setObjectName("label");

        verticalLayout_4->addWidget(label);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        createOutput = new QCheckBox(DSS__OutputTab);
        createOutput->setObjectName("createOutput");

        verticalLayout_3->addWidget(createOutput);

        createHTML = new QCheckBox(DSS__OutputTab);
        createHTML->setObjectName("createHTML");

        verticalLayout_3->addWidget(createHTML);


        verticalLayout_4->addLayout(verticalLayout_3);

        groupBox = new QGroupBox(DSS__OutputTab);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        autoSave = new QRadioButton(groupBox);
        autoSave->setObjectName("autoSave");

        horizontalLayout->addWidget(autoSave);

        fileListName = new QRadioButton(groupBox);
        fileListName->setObjectName("fileListName");

        horizontalLayout->addWidget(fileListName);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        appendNumber = new QCheckBox(groupBox);
        appendNumber->setObjectName("appendNumber");

        horizontalLayout_5->addWidget(appendNumber);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_4->addWidget(groupBox);

        groupBox_2 = new QGroupBox(DSS__OutputTab);
        groupBox_2->setObjectName("groupBox_2");
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        refFrameFolder = new QRadioButton(groupBox_2);
        refFrameFolder->setObjectName("refFrameFolder");

        horizontalLayout_4->addWidget(refFrameFolder);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        fileListFolder = new QRadioButton(groupBox_2);
        fileListFolder->setObjectName("fileListFolder");

        horizontalLayout_3->addWidget(fileListFolder);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        otherFolder = new QRadioButton(groupBox_2);
        otherFolder->setObjectName("otherFolder");

        horizontalLayout_2->addWidget(otherFolder);

        outputFolder = new QPushButton(groupBox_2);
        outputFolder->setObjectName("outputFolder");
        outputFolder->setStyleSheet(QString::fromUtf8("color: blue"));
        outputFolder->setFlat(true);

        horizontalLayout_2->addWidget(outputFolder);

        label1 = new QLabel(groupBox_2);
        label1->setObjectName("label1");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label1->sizePolicy().hasHeightForWidth());
        label1->setSizePolicy(sizePolicy);
        label1->setLineWidth(0);
        label1->setIndent(0);

        horizontalLayout_2->addWidget(label1);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(2, 99);

        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout_4->addWidget(groupBox_2);

        verticalLayout_4->setStretch(0, 1);
        verticalLayout_4->setStretch(1, 10);
        verticalLayout_4->setStretch(2, 20);
        verticalLayout_4->setStretch(3, 20);

        retranslateUi(DSS__OutputTab);

        QMetaObject::connectSlotsByName(DSS__OutputTab);
    } // setupUi

    void retranslateUi(QWidget *DSS__OutputTab)
    {
        DSS__OutputTab->setWindowTitle(QCoreApplication::translate("DSS::OutputTab", "Output", nullptr));
        label->setText(QCoreApplication::translate("DSS::OutputTab", "Output Files Settings", nullptr));
        createOutput->setText(QCoreApplication::translate("DSS::OutputTab", "Create Output file", "IDC_CREATEOUTPUT"));
        createHTML->setText(QCoreApplication::translate("DSS::OutputTab", "Create HTML Description file", "IDC_CREATEHTML"));
        groupBox->setTitle(QCoreApplication::translate("DSS::OutputTab", "Output File Name", nullptr));
        autoSave->setText(QCoreApplication::translate("DSS::OutputTab", "Autosave.tif/fits", "IDC_AUTOSAVE"));
        fileListName->setText(QCoreApplication::translate("DSS::OutputTab", "<file list name>.tif/fits", "IDC_FILELIST"));
        appendNumber->setText(QCoreApplication::translate("DSS::OutputTab", "Append a number to avoid file overwrite (001, 002, ...)", "IDC_APPENDNUMBER"));
        groupBox_2->setTitle(QCoreApplication::translate("DSS::OutputTab", "Output Location", nullptr));
        refFrameFolder->setText(QCoreApplication::translate("DSS::OutputTab", "Create Output file in the folder of the reference frame", "IDC_USEREFERENCEFRAMEFOLDER"));
        fileListFolder->setText(QCoreApplication::translate("DSS::OutputTab", "Create Output file in the folder of the file list", "IDC_USEFILELISTFOLDER"));
        otherFolder->setText(QCoreApplication::translate("DSS::OutputTab", "Create Output file in", "IDC_USEANOTHERFOLDER"));
        outputFolder->setText(QCoreApplication::translate("DSS::OutputTab", "<Output Folder>", "IDC_OUTPUTFOLDER"));
        label1->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class OutputTab: public Ui_OutputTab {};
} // namespace Ui
} // namespace DSS

#endif // UI_OUTPUTTAB_H
