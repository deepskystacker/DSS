/********************************************************************************
** Form generated from reading UI file 'StarMaskDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARMASKDLG_H
#define UI_STARMASKDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DSS {

class Ui_StarMaskDlg
{
public:
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;
    QGroupBox *detectionGroup;
    QGridLayout *gridLayout;
    QLabel *thresholdLabel;
    QLabel *threshold;
    QSlider *thresholdSlider;
    QCheckBox *detectHotPixels;
    QLabel *minSizeLabel;
    QLabel *minSize;
    QSlider *minSizeSlider;
    QLabel *maxSizeLabel;
    QLabel *maxSize;
    QSlider *maxSizeSlider;
    QGroupBox *adjustmentGroup;
    QGridLayout *gridLayout_2;
    QLabel *percentLabel;
    QLabel *percent;
    QSlider *percentSlider;
    QLabel *pixelLabel;
    QLabel *pixels;
    QSlider *pixelsSlider;
    QWidget *widget;
    QFormLayout *formLayout;
    QVBoxLayout *verticalLayout;
    QLabel *starShapeLabel;
    QComboBox *starShape;
    QLabel *starShapePreview;

    void setupUi(QDialog *DSS__StarMaskDlg)
    {
        if (DSS__StarMaskDlg->objectName().isEmpty())
            DSS__StarMaskDlg->setObjectName("DSS__StarMaskDlg");
        DSS__StarMaskDlg->resize(451, 509);
        layoutWidget1 = new QWidget(DSS__StarMaskDlg);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(10, 470, 431, 26));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(layoutWidget1);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        horizontalLayout->addWidget(buttonBox);

        detectionGroup = new QGroupBox(DSS__StarMaskDlg);
        detectionGroup->setObjectName("detectionGroup");
        detectionGroup->setGeometry(QRect(11, 230, 431, 128));
        gridLayout = new QGridLayout(detectionGroup);
        gridLayout->setObjectName("gridLayout");
        thresholdLabel = new QLabel(detectionGroup);
        thresholdLabel->setObjectName("thresholdLabel");
        thresholdLabel->setMinimumSize(QSize(190, 0));

        gridLayout->addWidget(thresholdLabel, 0, 0, 1, 1);

        threshold = new QLabel(detectionGroup);
        threshold->setObjectName("threshold");
        threshold->setText(QString::fromUtf8("10%"));
        threshold->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(threshold, 0, 2, 1, 1);

        thresholdSlider = new QSlider(detectionGroup);
        thresholdSlider->setObjectName("thresholdSlider");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(thresholdSlider->sizePolicy().hasHeightForWidth());
        thresholdSlider->setSizePolicy(sizePolicy);
        thresholdSlider->setMinimumSize(QSize(160, 0));
        thresholdSlider->setMaximumSize(QSize(200, 16777215));
        thresholdSlider->setMinimum(2);
        thresholdSlider->setMaximum(100);
        thresholdSlider->setValue(10);
        thresholdSlider->setOrientation(Qt::Orientation::Horizontal);

        gridLayout->addWidget(thresholdSlider, 0, 3, 1, 1);

        detectHotPixels = new QCheckBox(detectionGroup);
        detectHotPixels->setObjectName("detectHotPixels");

        gridLayout->addWidget(detectHotPixels, 1, 0, 1, 2);

        minSizeLabel = new QLabel(detectionGroup);
        minSizeLabel->setObjectName("minSizeLabel");
        minSizeLabel->setMinimumSize(QSize(190, 0));

        gridLayout->addWidget(minSizeLabel, 2, 0, 1, 1);

        minSize = new QLabel(detectionGroup);
        minSize->setObjectName("minSize");
        minSize->setMinimumSize(QSize(0, 0));
        minSize->setText(QString::fromUtf8("2 pixels"));
        minSize->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(minSize, 2, 2, 1, 1);

        minSizeSlider = new QSlider(detectionGroup);
        minSizeSlider->setObjectName("minSizeSlider");
        minSizeSlider->setMinimumSize(QSize(160, 0));
        minSizeSlider->setMinimum(2);
        minSizeSlider->setMaximum(10);
        minSizeSlider->setOrientation(Qt::Orientation::Horizontal);

        gridLayout->addWidget(minSizeSlider, 2, 3, 1, 1);

        maxSizeLabel = new QLabel(detectionGroup);
        maxSizeLabel->setObjectName("maxSizeLabel");
        maxSizeLabel->setMinimumSize(QSize(190, 0));

        gridLayout->addWidget(maxSizeLabel, 3, 0, 1, 1);

        maxSize = new QLabel(detectionGroup);
        maxSize->setObjectName("maxSize");
        maxSize->setText(QString::fromUtf8("25 pixels"));
        maxSize->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(maxSize, 3, 1, 1, 2);

        maxSizeSlider = new QSlider(detectionGroup);
        maxSizeSlider->setObjectName("maxSizeSlider");
        sizePolicy.setHeightForWidth(maxSizeSlider->sizePolicy().hasHeightForWidth());
        maxSizeSlider->setSizePolicy(sizePolicy);
        maxSizeSlider->setMinimumSize(QSize(160, 0));
        maxSizeSlider->setMaximumSize(QSize(200, 16777215));
        maxSizeSlider->setMinimum(10);
        maxSizeSlider->setMaximum(200);
        maxSizeSlider->setValue(25);
        maxSizeSlider->setOrientation(Qt::Orientation::Horizontal);

        gridLayout->addWidget(maxSizeSlider, 3, 3, 1, 1);

        adjustmentGroup = new QGroupBox(DSS__StarMaskDlg);
        adjustmentGroup->setObjectName("adjustmentGroup");
        adjustmentGroup->setGeometry(QRect(11, 370, 431, 78));
        gridLayout_2 = new QGridLayout(adjustmentGroup);
        gridLayout_2->setObjectName("gridLayout_2");
        percentLabel = new QLabel(adjustmentGroup);
        percentLabel->setObjectName("percentLabel");
        percentLabel->setMinimumSize(QSize(190, 0));

        gridLayout_2->addWidget(percentLabel, 0, 0, 1, 2);

        percent = new QLabel(adjustmentGroup);
        percent->setObjectName("percent");
        percent->setText(QString::fromUtf8("100%"));
        percent->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_2->addWidget(percent, 0, 2, 1, 1);

        percentSlider = new QSlider(adjustmentGroup);
        percentSlider->setObjectName("percentSlider");
        sizePolicy.setHeightForWidth(percentSlider->sizePolicy().hasHeightForWidth());
        percentSlider->setSizePolicy(sizePolicy);
        percentSlider->setMinimumSize(QSize(160, 0));
        percentSlider->setMaximumSize(QSize(200, 16777215));
        percentSlider->setMinimum(10);
        percentSlider->setMaximum(200);
        percentSlider->setValue(100);
        percentSlider->setOrientation(Qt::Orientation::Horizontal);

        gridLayout_2->addWidget(percentSlider, 0, 3, 1, 1);

        pixelLabel = new QLabel(adjustmentGroup);
        pixelLabel->setObjectName("pixelLabel");
        pixelLabel->setMinimumSize(QSize(190, 0));

        gridLayout_2->addWidget(pixelLabel, 1, 0, 1, 1);

        pixels = new QLabel(adjustmentGroup);
        pixels->setObjectName("pixels");
        pixels->setText(QString::fromUtf8("0 pixels"));
        pixels->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout_2->addWidget(pixels, 1, 1, 1, 2);

        pixelsSlider = new QSlider(adjustmentGroup);
        pixelsSlider->setObjectName("pixelsSlider");
        sizePolicy.setHeightForWidth(pixelsSlider->sizePolicy().hasHeightForWidth());
        pixelsSlider->setSizePolicy(sizePolicy);
        pixelsSlider->setMinimumSize(QSize(160, 0));
        pixelsSlider->setMaximumSize(QSize(200, 16777215));
        pixelsSlider->setMinimum(0);
        pixelsSlider->setMaximum(10);
        pixelsSlider->setOrientation(Qt::Orientation::Horizontal);

        gridLayout_2->addWidget(pixelsSlider, 1, 3, 1, 1);

        widget = new QWidget(DSS__StarMaskDlg);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 20, 431, 182));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        starShapeLabel = new QLabel(widget);
        starShapeLabel->setObjectName("starShapeLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(starShapeLabel->sizePolicy().hasHeightForWidth());
        starShapeLabel->setSizePolicy(sizePolicy1);
        starShapeLabel->setMinimumSize(QSize(220, 0));

        verticalLayout->addWidget(starShapeLabel);

        starShape = new QComboBox(widget);
        starShape->setObjectName("starShape");

        verticalLayout->addWidget(starShape);


        formLayout->setLayout(0, QFormLayout::LabelRole, verticalLayout);

        starShapePreview = new QLabel(widget);
        starShapePreview->setObjectName("starShapePreview");
        starShapePreview->setMinimumSize(QSize(180, 180));
        starShapePreview->setFrameShape(QFrame::Shape::StyledPanel);
        starShapePreview->setText(QString::fromUtf8(""));
        starShapePreview->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout->setWidget(0, QFormLayout::FieldRole, starShapePreview);

        QWidget::setTabOrder(starShape, thresholdSlider);
        QWidget::setTabOrder(thresholdSlider, detectHotPixels);
        QWidget::setTabOrder(detectHotPixels, minSizeSlider);
        QWidget::setTabOrder(minSizeSlider, maxSizeSlider);
        QWidget::setTabOrder(maxSizeSlider, percentSlider);
        QWidget::setTabOrder(percentSlider, pixelsSlider);

        retranslateUi(DSS__StarMaskDlg);

        QMetaObject::connectSlotsByName(DSS__StarMaskDlg);
    } // setupUi

    void retranslateUi(QDialog *DSS__StarMaskDlg)
    {
        DSS__StarMaskDlg->setWindowTitle(QCoreApplication::translate("DSS::StarMaskDlg", "Create a Star Mask", "IDD_STARMASK"));
        detectionGroup->setTitle(QCoreApplication::translate("DSS::StarMaskDlg", "Star detection", nullptr));
        thresholdLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Star detection threshold", nullptr));
        detectHotPixels->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Hot pixel detection", "IDC_HOTPIXELS"));
        minSizeLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Minimum size of a star", nullptr));
        maxSizeLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Maximum size of a star", nullptr));
        adjustmentGroup->setTitle(QCoreApplication::translate("DSS::StarMaskDlg", "Enlarge/Shrink", nullptr));
        percentLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Percentage of the star size", nullptr));
        pixelLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Increase size by  ", nullptr));
        starShapeLabel->setText(QCoreApplication::translate("DSS::StarMaskDlg", "Shape of the stars in the mask", nullptr));
    } // retranslateUi

};

} // namespace DSS

namespace DSS {
namespace Ui {
    class StarMaskDlg: public Ui_StarMaskDlg {};
} // namespace Ui
} // namespace DSS

#endif // UI_STARMASKDLG_H
