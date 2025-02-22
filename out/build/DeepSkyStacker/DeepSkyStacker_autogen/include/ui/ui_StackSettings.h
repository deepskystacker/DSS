/********************************************************************************
** Form generated from reading UI file 'StackSettings.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STACKSETTINGS_H
#define UI_STACKSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_StackSettings
{
public:
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *tempFilesFolder;
    QPushButton *chooseFolder;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *reducePriority;
    QCheckBox *useAllProcessors;
    QCheckBox *useSimd;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__StackSettings)
    {
        if (DSS__StackSettings->objectName().isEmpty())
            DSS__StackSettings->setObjectName("DSS__StackSettings");
        DSS__StackSettings->resize(686, 364);
        verticalLayout_3 = new QVBoxLayout(DSS__StackSettings);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(-1, -1, -1, 12);
        tabWidget = new QTabWidget(DSS__StackSettings);
        tabWidget->setObjectName("tabWidget");

        verticalLayout_3->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(4, -1, -1, -1);
        label = new QLabel(DSS__StackSettings);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        tempFilesFolder = new QLabel(DSS__StackSettings);
        tempFilesFolder->setObjectName("tempFilesFolder");

        horizontalLayout->addWidget(tempFilesFolder);

        chooseFolder = new QPushButton(DSS__StackSettings);
        chooseFolder->setObjectName("chooseFolder");
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(chooseFolder->sizePolicy().hasHeightForWidth());
        chooseFolder->setSizePolicy(sizePolicy);
        chooseFolder->setMinimumSize(QSize(40, 0));

        horizontalLayout->addWidget(chooseFolder);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1000);
        horizontalLayout->setStretch(2, 1);

        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName("verticalLayout");
        reducePriority = new QCheckBox(DSS__StackSettings);
        reducePriority->setObjectName("reducePriority");

        verticalLayout->addWidget(reducePriority);

        useAllProcessors = new QCheckBox(DSS__StackSettings);
        useAllProcessors->setObjectName("useAllProcessors");

        verticalLayout->addWidget(useAllProcessors);

        useSimd = new QCheckBox(DSS__StackSettings);
        useSimd->setObjectName("useSimd");

        verticalLayout->addWidget(useSimd);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer = new QSpacerItem(20, 80, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(DSS__StackSettings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);

        verticalLayout_2->setStretch(0, 99);
        verticalLayout_2->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalLayout_2->setStretch(0, 99);
        horizontalLayout_2->setStretch(1, 1);

        verticalLayout_3->addLayout(horizontalLayout_2);

        verticalLayout_3->setStretch(0, 999);
        verticalLayout_3->setStretch(1, 1);
        verticalLayout_3->setStretch(2, 1);

        retranslateUi(DSS__StackSettings);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(DSS__StackSettings);
    } // setupUi

    void retranslateUi(QDialog *DSS__StackSettings)
    {
        DSS__StackSettings->setWindowTitle(QCoreApplication::translate("DSS::StackSettings", "Stacking Settings", nullptr));
        label->setText(QCoreApplication::translate("DSS::StackSettings", "Temporary files folder:", nullptr));
        tempFilesFolder->setText(QString());
        chooseFolder->setText(QCoreApplication::translate("DSS::StackSettings", "...", nullptr));
        reducePriority->setText(QCoreApplication::translate("DSS::StackSettings", "Reduce worker threads priority", nullptr));
        useAllProcessors->setText(QCoreApplication::translate("DSS::StackSettings", "Use all available processors", nullptr));
        useSimd->setText(QCoreApplication::translate("DSS::StackSettings", "SIMD vectorisation", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class StackSettings: public Ui_StackSettings {};
} // namespace Ui
} // namespace DSS

#endif // UI_STACKSETTINGS_H
