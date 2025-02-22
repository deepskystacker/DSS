/********************************************************************************
** Form generated from reading UI file 'ImageProperties.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEPROPERTIES_H
#define UI_IMAGEPROPERTIES_H

#include <QElidedLabel.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_ImageProperties
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *dateStamp;
    QSpacerItem *horizontalSpacer_3;
    QLabel *imageCFA;
    QElidedLabel *fileName;
    QLabel *tstamp;
    QLabel *information;
    QTimeEdit *timeEdit;
    QLabel *depth;
    QLabel *size;
    QLabel *imageSize;
    QSpacerItem *horizontalSpacer_2;
    QLabel *exposure;
    QComboBox *typeCombo;
    QLabel *colourDepth;
    QSpacerItem *horizontalSpacer;
    QLabel *info;
    QLabel *type;
    QSpacerItem *horizontalSpacer_4;
    QComboBox *isoCombo;
    QLabel *file;
    QLabel *isogain;
    QLabel *cfa;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__ImageProperties)
    {
        if (DSS__ImageProperties->objectName().isEmpty())
            DSS__ImageProperties->setObjectName("DSS__ImageProperties");
        DSS__ImageProperties->resize(478, 299);
        verticalLayout = new QVBoxLayout(DSS__ImageProperties);
        verticalLayout->setObjectName("verticalLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        dateStamp = new QLabel(DSS__ImageProperties);
        dateStamp->setObjectName("dateStamp");

        gridLayout->addWidget(dateStamp, 1, 3, 1, 4);

        horizontalSpacer_3 = new QSpacerItem(17, 17, QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 2, 9, 1, 1);

        imageCFA = new QLabel(DSS__ImageProperties);
        imageCFA->setObjectName("imageCFA");

        gridLayout->addWidget(imageCFA, 2, 7, 1, 1);

        fileName = new QElidedLabel(DSS__ImageProperties);
        fileName->setObjectName("fileName");
        fileName->setMinimumSize(QSize(0, 0));
        fileName->setFrameShape(QFrame::NoFrame);
        fileName->setFrameShadow(QFrame::Plain);

        gridLayout->addWidget(fileName, 0, 3, 1, 7);

        tstamp = new QLabel(DSS__ImageProperties);
        tstamp->setObjectName("tstamp");

        gridLayout->addWidget(tstamp, 1, 1, 1, 1);

        information = new QLabel(DSS__ImageProperties);
        information->setObjectName("information");

        gridLayout->addWidget(information, 7, 3, 1, 2);

        timeEdit = new QTimeEdit(DSS__ImageProperties);
        timeEdit->setObjectName("timeEdit");
        timeEdit->setCurrentSection(QDateTimeEdit::HourSection);
        timeEdit->setDisplayFormat(QString::fromUtf8("hh:mm:ss.zzz"));

        gridLayout->addWidget(timeEdit, 10, 3, 1, 1);

        depth = new QLabel(DSS__ImageProperties);
        depth->setObjectName("depth");
        depth->setWordWrap(true);

        gridLayout->addWidget(depth, 5, 1, 1, 1);

        size = new QLabel(DSS__ImageProperties);
        size->setObjectName("size");

        gridLayout->addWidget(size, 2, 1, 1, 1);

        imageSize = new QLabel(DSS__ImageProperties);
        imageSize->setObjectName("imageSize");

        gridLayout->addWidget(imageSize, 2, 3, 1, 2);

        horizontalSpacer_2 = new QSpacerItem(5, 17, QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 2, 1, 1);

        exposure = new QLabel(DSS__ImageProperties);
        exposure->setObjectName("exposure");

        gridLayout->addWidget(exposure, 10, 1, 1, 1);

        typeCombo = new QComboBox(DSS__ImageProperties);
        typeCombo->setObjectName("typeCombo");
        typeCombo->setEditable(false);

        gridLayout->addWidget(typeCombo, 8, 3, 1, 1);

        colourDepth = new QLabel(DSS__ImageProperties);
        colourDepth->setObjectName("colourDepth");

        gridLayout->addWidget(colourDepth, 5, 3, 1, 2);

        horizontalSpacer = new QSpacerItem(10, 17, QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 5, 0, 1, 1);

        info = new QLabel(DSS__ImageProperties);
        info->setObjectName("info");

        gridLayout->addWidget(info, 7, 1, 1, 1);

        type = new QLabel(DSS__ImageProperties);
        type->setObjectName("type");

        gridLayout->addWidget(type, 8, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(10, 13, QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 5, 6, 1, 1);

        isoCombo = new QComboBox(DSS__ImageProperties);
        isoCombo->setObjectName("isoCombo");
        isoCombo->setEditable(true);

        gridLayout->addWidget(isoCombo, 9, 3, 1, 1);

        file = new QLabel(DSS__ImageProperties);
        file->setObjectName("file");

        gridLayout->addWidget(file, 0, 1, 1, 1);

        isogain = new QLabel(DSS__ImageProperties);
        isogain->setObjectName("isogain");

        gridLayout->addWidget(isogain, 9, 1, 1, 1);

        cfa = new QLabel(DSS__ImageProperties);
        cfa->setObjectName("cfa");

        gridLayout->addWidget(cfa, 2, 5, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        spacerItem = new QSpacerItem(131, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(spacerItem);

        buttonBox = new QDialogButtonBox(DSS__ImageProperties);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(DSS__ImageProperties);

        QMetaObject::connectSlotsByName(DSS__ImageProperties);
    } // setupUi

    void retranslateUi(QDialog *DSS__ImageProperties)
    {
        DSS__ImageProperties->setWindowTitle(QCoreApplication::translate("DSS::ImageProperties", "DeepSkyStacker - Image Properties", nullptr));
        dateStamp->setText(QString());
        imageCFA->setText(QString());
        tstamp->setText(QCoreApplication::translate("DSS::ImageProperties", "Date/Time", nullptr));
        information->setText(QString());
        depth->setText(QCoreApplication::translate("DSS::ImageProperties", "Depth", nullptr));
        size->setText(QCoreApplication::translate("DSS::ImageProperties", "Size", nullptr));
        imageSize->setText(QString());
        exposure->setText(QCoreApplication::translate("DSS::ImageProperties", "Exposure", nullptr));
        colourDepth->setText(QString());
        info->setText(QCoreApplication::translate("DSS::ImageProperties", "Information", nullptr));
        type->setText(QCoreApplication::translate("DSS::ImageProperties", "Type", nullptr));
        file->setText(QCoreApplication::translate("DSS::ImageProperties", "File", nullptr));
        isogain->setText(QCoreApplication::translate("DSS::ImageProperties", "ISO/Gain", nullptr));
        cfa->setText(QCoreApplication::translate("DSS::ImageProperties", "CFA", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class ImageProperties: public Ui_ImageProperties {};
} // namespace Ui
} // namespace DSS

#endif // UI_IMAGEPROPERTIES_H
