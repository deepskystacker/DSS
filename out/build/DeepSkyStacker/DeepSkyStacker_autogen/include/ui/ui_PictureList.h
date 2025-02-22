/********************************************************************************
** Form generated from reading UI file 'PictureList.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PICTURELIST_H
#define UI_PICTURELIST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_PictureList
{
public:
    QVBoxLayout *verticalLayout;
    QTableView *tableView;
    QTabBar *tabBar;

    void setupUi(QWidget *DSS__PictureList)
    {
        if (DSS__PictureList->objectName().isEmpty())
            DSS__PictureList->setObjectName("DSS__PictureList");
        DSS__PictureList->resize(500, 156);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DSS__PictureList->sizePolicy().hasHeightForWidth());
        DSS__PictureList->setSizePolicy(sizePolicy);
        DSS__PictureList->setMinimumSize(QSize(500, 156));
        DSS__PictureList->setWindowTitle(QString::fromUtf8("PictureList"));
        DSS__PictureList->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(DSS__PictureList);
        verticalLayout->setObjectName("verticalLayout");
        tableView = new QTableView(DSS__PictureList);
        tableView->setObjectName("tableView");

        verticalLayout->addWidget(tableView);

        tabBar = new QTabBar(DSS__PictureList);
        tabBar->setObjectName("tabBar");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabBar->sizePolicy().hasHeightForWidth());
        tabBar->setSizePolicy(sizePolicy1);
        tabBar->setMinimumSize(QSize(0, 28));
        tabBar->setMaximumSize(QSize(16777215, 28));

        verticalLayout->addWidget(tabBar);


        retranslateUi(DSS__PictureList);

        QMetaObject::connectSlotsByName(DSS__PictureList);
    } // setupUi

    void retranslateUi(QWidget *DSS__PictureList)
    {
        (void)DSS__PictureList;
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class PictureList: public Ui_PictureList {};
} // namespace Ui
} // namespace DSS

#endif // UI_PICTURELIST_H
