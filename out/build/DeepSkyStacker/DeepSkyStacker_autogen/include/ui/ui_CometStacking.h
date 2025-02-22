/********************************************************************************
** Form generated from reading UI file 'CometStacking.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMETSTACKING_H
#define UI_COMETSTACKING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_CometStacking
{
public:
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QRadioButton *modeStandard;
    QLabel *label_3;
    QSpacerItem *verticalSpacer;
    QRadioButton *modeComet;
    QLabel *label_4;
    QSpacerItem *verticalSpacer_2;
    QRadioButton *modeAdvanced;
    QLabel *label_5;
    QLabel *laComet;

    void setupUi(QWidget *DSS__CometStacking)
    {
        if (DSS__CometStacking->objectName().isEmpty())
            DSS__CometStacking->setObjectName("DSS__CometStacking");
        DSS__CometStacking->resize(597, 306);
        verticalLayout_2 = new QVBoxLayout(DSS__CometStacking);
        verticalLayout_2->setObjectName("verticalLayout_2");
        groupBox = new QGroupBox(DSS__CometStacking);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        modeStandard = new QRadioButton(groupBox);
        modeStandard->setObjectName("modeStandard");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(modeStandard->sizePolicy().hasHeightForWidth());
        modeStandard->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(modeStandard);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        label_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_3->setWordWrap(true);

        verticalLayout->addWidget(label_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        modeComet = new QRadioButton(groupBox);
        modeComet->setObjectName("modeComet");
        sizePolicy.setHeightForWidth(modeComet->sizePolicy().hasHeightForWidth());
        modeComet->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(modeComet);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);
        label_4->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        label_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_4->setWordWrap(true);

        verticalLayout->addWidget(label_4);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        modeAdvanced = new QRadioButton(groupBox);
        modeAdvanced->setObjectName("modeAdvanced");
        sizePolicy.setHeightForWidth(modeAdvanced->sizePolicy().hasHeightForWidth());
        modeAdvanced->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(modeAdvanced);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);
        label_5->setStyleSheet(QString::fromUtf8("margin-left: 15"));
        label_5->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_5->setWordWrap(true);
        label_5->setOpenExternalLinks(true);

        verticalLayout->addWidget(label_5);


        horizontalLayout->addLayout(verticalLayout);

        laComet = new QLabel(groupBox);
        laComet->setObjectName("laComet");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(laComet->sizePolicy().hasHeightForWidth());
        laComet->setSizePolicy(sizePolicy2);
        laComet->setMinimumSize(QSize(275, 0));
        laComet->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(laComet);

        horizontalLayout->setStretch(0, 99);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addWidget(groupBox);


        retranslateUi(DSS__CometStacking);

        QMetaObject::connectSlotsByName(DSS__CometStacking);
    } // setupUi

    void retranslateUi(QWidget *DSS__CometStacking)
    {
        DSS__CometStacking->setWindowTitle(QCoreApplication::translate("DSS::CometStacking", "Comet", nullptr));
        groupBox->setTitle(QCoreApplication::translate("DSS::CometStacking", "Comet Stacking Parameters", nullptr));
        modeStandard->setText(QCoreApplication::translate("DSS::CometStacking", "Standard Stacking", nullptr));
        label_3->setText(QCoreApplication::translate("DSS::CometStacking", "The comet position is ignored.\n"
"The comet will be fuzzy.", nullptr));
        modeComet->setText(QCoreApplication::translate("DSS::CometStacking", "Comet Stacking", nullptr));
        label_4->setText(QCoreApplication::translate("DSS::CometStacking", "The comet position is used.\n"
"The comet will be sharp and the stars will have trails.", nullptr));
        modeAdvanced->setText(QCoreApplication::translate("DSS::CometStacking", "Stars + Comet Stacking", nullptr));
        label_5->setText(QCoreApplication::translate("DSS::CometStacking", "The comet position is used.\n"
"The comet and the stars will be sharp. This process takes twice as long.", nullptr));
        laComet->setText(QString());
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class CometStacking: public Ui_CometStacking {};
} // namespace Ui
} // namespace DSS

#endif // UI_COMETSTACKING_H
