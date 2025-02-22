/********************************************************************************
** Form generated from reading UI file 'ProcessingSettingsDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROCESSINGSETTINGSDLG_H
#define UI_PROCESSINGSETTINGSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_ProcessingSettingsDlg
{
public:
    QGridLayout *gridLayout;
    QLineEdit *settingsName;
    QVBoxLayout *vboxLayout;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QSpacerItem *spacerItem;
    QPushButton *loadButton;
    QPushButton *closeButton;
    QListWidget *settingsList;

    void setupUi(QDialog *DSS__ProcessingSettingsDlg)
    {
        if (DSS__ProcessingSettingsDlg->objectName().isEmpty())
            DSS__ProcessingSettingsDlg->setObjectName("DSS__ProcessingSettingsDlg");
        DSS__ProcessingSettingsDlg->setWindowModality(Qt::WindowModal);
        DSS__ProcessingSettingsDlg->resize(414, 300);
        gridLayout = new QGridLayout(DSS__ProcessingSettingsDlg);
        gridLayout->setObjectName("gridLayout");
        settingsName = new QLineEdit(DSS__ProcessingSettingsDlg);
        settingsName->setObjectName("settingsName");

        gridLayout->addWidget(settingsName, 0, 0, 1, 1);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName("vboxLayout");
        addButton = new QPushButton(DSS__ProcessingSettingsDlg);
        addButton->setObjectName("addButton");

        vboxLayout->addWidget(addButton);

        deleteButton = new QPushButton(DSS__ProcessingSettingsDlg);
        deleteButton->setObjectName("deleteButton");

        vboxLayout->addWidget(deleteButton);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout->addItem(spacerItem);

        loadButton = new QPushButton(DSS__ProcessingSettingsDlg);
        loadButton->setObjectName("loadButton");

        vboxLayout->addWidget(loadButton);

        closeButton = new QPushButton(DSS__ProcessingSettingsDlg);
        closeButton->setObjectName("closeButton");

        vboxLayout->addWidget(closeButton);


        gridLayout->addLayout(vboxLayout, 0, 1, 2, 1);

        settingsList = new QListWidget(DSS__ProcessingSettingsDlg);
        settingsList->setObjectName("settingsList");

        gridLayout->addWidget(settingsList, 1, 0, 1, 1);


        retranslateUi(DSS__ProcessingSettingsDlg);
        QObject::connect(addButton, &QPushButton::clicked, DSS__ProcessingSettingsDlg, qOverload<>(&QDialog::accept));
        QObject::connect(deleteButton, &QPushButton::clicked, DSS__ProcessingSettingsDlg, qOverload<>(&QDialog::reject));

        loadButton->setDefault(true);


        QMetaObject::connectSlotsByName(DSS__ProcessingSettingsDlg);
    } // setupUi

    void retranslateUi(QDialog *DSS__ProcessingSettingsDlg)
    {
        DSS__ProcessingSettingsDlg->setWindowTitle(QCoreApplication::translate("DSS::ProcessingSettingsDlg", "Processing Settings", nullptr));
        addButton->setText(QCoreApplication::translate("DSS::ProcessingSettingsDlg", "Add", nullptr));
        deleteButton->setText(QCoreApplication::translate("DSS::ProcessingSettingsDlg", "Delete", nullptr));
        loadButton->setText(QCoreApplication::translate("DSS::ProcessingSettingsDlg", "Load", nullptr));
        closeButton->setText(QCoreApplication::translate("DSS::ProcessingSettingsDlg", "Close", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ProcessingSettingsDlg: public Ui_ProcessingSettingsDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_PROCESSINGSETTINGSDLG_H
