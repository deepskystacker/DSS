/********************************************************************************
** Form generated from reading UI file 'EmailSettings.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EMAILSETTINGS_H
#define UI_EMAILSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_EmailSettings
{
public:
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QCheckBox *sendOnce;
    QLineEdit *emailSubject;
    QLabel *label_2;
    QLineEdit *emailTo;
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLineEdit *smtpServer;
    QLabel *label_4;
    QComboBox *smtpPort;
    QLabel *label_7;
    QComboBox *smtpEncryption;
    QLabel *label_5;
    QLineEdit *emailAccount;
    QLabel *label_6;
    QLineEdit *emailPassword;
    QCheckBox *showPassword;
    QPushButton *testButton;

    void setupUi(QDialog *EmailSettings)
    {
        if (EmailSettings->objectName().isEmpty())
            EmailSettings->setObjectName("EmailSettings");
        EmailSettings->resize(491, 299);
        gridLayout_3 = new QGridLayout(EmailSettings);
        gridLayout_3->setObjectName("gridLayout_3");
        groupBox = new QGroupBox(EmailSettings);
        groupBox->setObjectName("groupBox");
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName("gridLayout_2");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        sendOnce = new QCheckBox(groupBox);
        sendOnce->setObjectName("sendOnce");

        gridLayout_2->addWidget(sendOnce, 2, 0, 1, 2);

        emailSubject = new QLineEdit(groupBox);
        emailSubject->setObjectName("emailSubject");

        gridLayout_2->addWidget(emailSubject, 1, 1, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        emailTo = new QLineEdit(groupBox);
        emailTo->setObjectName("emailTo");

        gridLayout_2->addWidget(emailTo, 0, 1, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(EmailSettings);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout_3->addWidget(buttonBox, 0, 1, 1, 1);

        groupBox_2 = new QGroupBox(EmailSettings);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName("gridLayout");
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        smtpServer = new QLineEdit(groupBox_2);
        smtpServer->setObjectName("smtpServer");

        gridLayout->addWidget(smtpServer, 0, 1, 1, 3);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        smtpPort = new QComboBox(groupBox_2);
        smtpPort->addItem(QString::fromUtf8("465"));
        smtpPort->addItem(QString::fromUtf8("587"));
        smtpPort->addItem(QString::fromUtf8("2525"));
        smtpPort->setObjectName("smtpPort");
        smtpPort->setMaximumSize(QSize(55, 16777215));
        smtpPort->setCurrentText(QString::fromUtf8("465"));
        smtpPort->setMaxVisibleItems(4);
        smtpPort->setMaxCount(4);

        gridLayout->addWidget(smtpPort, 1, 1, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 1, 2, 1, 1);

        smtpEncryption = new QComboBox(groupBox_2);
        smtpEncryption->addItem(QString());
        smtpEncryption->addItem(QString());
        smtpEncryption->addItem(QString());
        smtpEncryption->setObjectName("smtpEncryption");

        gridLayout->addWidget(smtpEncryption, 1, 3, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");
        label_5->setWordWrap(false);

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        emailAccount = new QLineEdit(groupBox_2);
        emailAccount->setObjectName("emailAccount");

        gridLayout->addWidget(emailAccount, 2, 1, 1, 3);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        emailPassword = new QLineEdit(groupBox_2);
        emailPassword->setObjectName("emailPassword");
        emailPassword->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);

        gridLayout->addWidget(emailPassword, 3, 1, 1, 2);

        showPassword = new QCheckBox(groupBox_2);
        showPassword->setObjectName("showPassword");

        gridLayout->addWidget(showPassword, 3, 3, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 1, 0, 1, 1);

        testButton = new QPushButton(EmailSettings);
        testButton->setObjectName("testButton");

        gridLayout_3->addWidget(testButton, 1, 1, 1, 1);


        retranslateUi(EmailSettings);

        smtpPort->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(EmailSettings);
    } // setupUi

    void retranslateUi(QDialog *EmailSettings)
    {
        EmailSettings->setWindowTitle(QCoreApplication::translate("EmailSettings", "Email Settings", nullptr));
        groupBox->setTitle(QCoreApplication::translate("EmailSettings", "Sent email settings", nullptr));
        label->setText(QCoreApplication::translate("EmailSettings", "To:", nullptr));
        sendOnce->setText(QCoreApplication::translate("EmailSettings", "Send an Email only for the first warning", "IDC_SENDONCE"));
        label_2->setText(QCoreApplication::translate("EmailSettings", "Subject:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("EmailSettings", "Email Account settings", nullptr));
        label_3->setText(QCoreApplication::translate("EmailSettings", "SMTP server:", nullptr));
        label_4->setText(QCoreApplication::translate("EmailSettings", "Port:", nullptr));

        label_7->setText(QCoreApplication::translate("EmailSettings", "Encryption:", nullptr));
        smtpEncryption->setItemText(0, QCoreApplication::translate("EmailSettings", "None", nullptr));
        smtpEncryption->setItemText(1, QCoreApplication::translate("EmailSettings", "SSL/TLS", nullptr));
        smtpEncryption->setItemText(2, QCoreApplication::translate("EmailSettings", "STARTTLS", nullptr));

        label_5->setText(QCoreApplication::translate("EmailSettings", "Account (From):", nullptr));
        label_6->setText(QCoreApplication::translate("EmailSettings", "Password:", nullptr));
        showPassword->setText(QCoreApplication::translate("EmailSettings", "Show password", nullptr));
        testButton->setText(QCoreApplication::translate("EmailSettings", "Test", "IDC_TEST"));
    } // retranslateUi

};

namespace Ui {
    class EmailSettings: public Ui_EmailSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EMAILSETTINGS_H
