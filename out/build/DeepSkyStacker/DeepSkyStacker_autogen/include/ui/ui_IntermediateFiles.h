/********************************************************************************
** Form generated from reading UI file 'IntermediateFiles.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERMEDIATEFILES_H
#define UI_INTERMEDIATEFILES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_IntermediateFiles
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *saveCalibrated;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QCheckBox *saveDebayered;
    QSpacerItem *verticalSpacer;
    QCheckBox *saveIntermediate;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *formatTIFF;
    QRadioButton *formatFITS;

    void setupUi(QWidget *DSS__IntermediateFiles)
    {
        if (DSS__IntermediateFiles->objectName().isEmpty())
            DSS__IntermediateFiles->setObjectName("DSS__IntermediateFiles");
        DSS__IntermediateFiles->resize(458, 222);
        verticalLayout_2 = new QVBoxLayout(DSS__IntermediateFiles);
        verticalLayout_2->setObjectName("verticalLayout_2");
        widget = new QWidget(DSS__IntermediateFiles);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(0, 13));
        verticalLayout_4 = new QVBoxLayout(widget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        label = new QLabel(widget);
        label->setObjectName("label");

        verticalLayout_4->addWidget(label);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        saveCalibrated = new QCheckBox(widget);
        saveCalibrated->setObjectName("saveCalibrated");

        verticalLayout_3->addWidget(saveCalibrated);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");
        label_2->setMinimumSize(QSize(20, 0));

        horizontalLayout->addWidget(label_2);

        saveDebayered = new QCheckBox(widget);
        saveDebayered->setObjectName("saveDebayered");
        saveDebayered->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(saveDebayered->sizePolicy().hasHeightForWidth());
        saveDebayered->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(saveDebayered);


        verticalLayout_3->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        saveIntermediate = new QCheckBox(widget);
        saveIntermediate->setObjectName("saveIntermediate");

        verticalLayout_3->addWidget(saveIntermediate);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);


        verticalLayout_4->addLayout(verticalLayout_3);

        groupBox = new QGroupBox(widget);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        formatTIFF = new QRadioButton(groupBox);
        formatTIFF->setObjectName("formatTIFF");

        verticalLayout->addWidget(formatTIFF);

        formatFITS = new QRadioButton(groupBox);
        formatFITS->setObjectName("formatFITS");

        verticalLayout->addWidget(formatFITS);


        verticalLayout_4->addWidget(groupBox);


        verticalLayout_2->addWidget(widget);

        QWidget::setTabOrder(saveCalibrated, saveDebayered);
        QWidget::setTabOrder(saveDebayered, saveIntermediate);
        QWidget::setTabOrder(saveIntermediate, formatTIFF);
        QWidget::setTabOrder(formatTIFF, formatFITS);

        retranslateUi(DSS__IntermediateFiles);

        QMetaObject::connectSlotsByName(DSS__IntermediateFiles);
    } // setupUi

    void retranslateUi(QWidget *DSS__IntermediateFiles)
    {
        DSS__IntermediateFiles->setWindowTitle(QCoreApplication::translate("DSS::IntermediateFiles", "Intermediate Files", nullptr));
        label->setText(QCoreApplication::translate("DSS::IntermediateFiles", "Intermediate files creation settings", nullptr));
        saveCalibrated->setText(QCoreApplication::translate("DSS::IntermediateFiles", "Create a calibrated file for each light frame", nullptr));
        label_2->setText(QString());
        saveDebayered->setText(QCoreApplication::translate("DSS::IntermediateFiles", "Save a debayered image when processing RAW images", nullptr));
        saveIntermediate->setText(QCoreApplication::translate("DSS::IntermediateFiles", "Create a registered/calibrated file for each light frame", nullptr));
        groupBox->setTitle(QCoreApplication::translate("DSS::IntermediateFiles", "Intermediate and Final Image File Format", nullptr));
        formatTIFF->setText(QCoreApplication::translate("DSS::IntermediateFiles", "TIFF Files", nullptr));
        formatFITS->setText(QCoreApplication::translate("DSS::IntermediateFiles", "FITS Files", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class IntermediateFiles: public Ui_IntermediateFiles {};
} // namespace Ui
} // namespace DSS

#endif // UI_INTERMEDIATEFILES_H
