/********************************************************************************
** Form generated from reading UI file 'BatchStacking.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHSTACKING_H
#define UI_BATCHSTACKING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace DSS {

class Ui_BatchStacking
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QPushButton *addLists;
    QSpacerItem *horizontalSpacer;
    QPushButton *clearLists;
    QListView *fileLists;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DSS__BatchStacking)
    {
        if (DSS__BatchStacking->objectName().isEmpty())
            DSS__BatchStacking->setObjectName("DSS__BatchStacking");
        DSS__BatchStacking->setWindowModality(Qt::WindowModal);
        DSS__BatchStacking->resize(443, 362);
        verticalLayout = new QVBoxLayout(DSS__BatchStacking);
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(DSS__BatchStacking);
        label->setObjectName("label");
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        addLists = new QPushButton(DSS__BatchStacking);
        addLists->setObjectName("addLists");

        horizontalLayout->addWidget(addLists);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        clearLists = new QPushButton(DSS__BatchStacking);
        clearLists->setObjectName("clearLists");

        horizontalLayout->addWidget(clearLists);


        verticalLayout->addLayout(horizontalLayout);

        fileLists = new QListView(DSS__BatchStacking);
        fileLists->setObjectName("fileLists");
        fileLists->setEditTriggers(QAbstractItemView::NoEditTriggers);
        fileLists->setDragDropMode(QAbstractItemView::NoDragDrop);
        fileLists->setMovement(QListView::Static);
        fileLists->setFlow(QListView::TopToBottom);
        fileLists->setViewMode(QListView::IconMode);

        verticalLayout->addWidget(fileLists);

        buttonBox = new QDialogButtonBox(DSS__BatchStacking);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(DSS__BatchStacking);

        QMetaObject::connectSlotsByName(DSS__BatchStacking);
    } // setupUi

    void retranslateUi(QDialog *DSS__BatchStacking)
    {
        DSS__BatchStacking->setWindowTitle(QCoreApplication::translate("DSS::BatchStacking", "Batch Stacking", nullptr));
        label->setText(QCoreApplication::translate("DSS::BatchStacking", "This dialog is used to launch the stacking process from existing image lists.\n"
"\n"
"Add the file lists you want to stack and click OK to start the stacking processes.", "IDC_STATIC"));
        addLists->setText(QCoreApplication::translate("DSS::BatchStacking", "Add File Lists...", "IDC_ADDLISTS"));
        clearLists->setText(QCoreApplication::translate("DSS::BatchStacking", "Clear List", "IDC_CLEARLIST"));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class BatchStacking: public Ui_BatchStacking {};
} // namespace Ui
} // namespace DSS

#endif // UI_BATCHSTACKING_H
