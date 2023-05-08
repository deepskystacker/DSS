#include "stdafx.h"
#include "DeepSkyStackerLive.h"
#include "LiveSettings.h"
#include "emailsettings.h"
#include <SmtpMime/SmtpMime>

namespace DSS
{
	EmailSettings::EmailSettings(QWidget* parent)
		: QDialog(parent),
		initialised{ false },
		liveSettings{ *(DeepSkyStacker::instance()->liveSettings) }
	{
		setupUi(this);

		connectSignalsToSlots();

	}

	EmailSettings::~EmailSettings()
	{}

	void EmailSettings::connectSignalsToSlots()
	{
		connect(buttonBox, &QDialogButtonBox::accepted, this, &EmailSettings::save);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &EmailSettings::reject);
		connect(testButton, &QPushButton::pressed, this, &EmailSettings::test);
		connect(showPassword, &QCheckBox::clicked, this, &EmailSettings::showPasswordClicked);
	}


	void EmailSettings::showEvent(QShowEvent* event)
	{
		if (!event->spontaneous())
		{
			if (!initialised)
			{
				initialised = true;
				onInitDialog();
			}
		}
		// Invoke base class showEvent()
		return Inherited::showEvent(event);
	}

	void EmailSettings::onInitDialog()
	{
		QString	to{};
		QString	subject{};
		QString	server{};
		int		port;
		uint	encryption;
		QString	account{};
		QString	password{};

		liveSettings.getEmailSettings(to, subject, server, port, encryption, account, password);

		//
		// De-obfuscate the password
		//
		for (auto& character : password)
		{
			character = QChar(static_cast<uint16_t>(character.unicode()) ^ 0x82U);
		}

		emailTo->setText(to);
		emailSubject->setText(subject);
		smtpServer->setText(server);

		switch (port)
		{
		case 2525:
			smtpPort->setCurrentIndex(2);
			break;
		case 465:
			smtpPort->setCurrentIndex(0);
			break;
		case 587:
		default:
			smtpPort->setCurrentIndex(1);
			break;
		}

		switch (encryption)
		{
		case 0:
		default:
			smtpEncryption->setCurrentIndex(0);
			break;
		case 1:
			smtpEncryption->setCurrentIndex(1);
			break;
		case 2:
			smtpEncryption->setCurrentIndex(2);
			break;
		}

		emailAccount->setText(account);
		emailPassword->setText(password);
	}

	/* ------------------------------------------------------------------- */
	/* Slots                                                               */
	/* ------------------------------------------------------------------- */
	void EmailSettings::showPasswordClicked(bool checked)
	{
		if (checked)
		{
			emailPassword->setEchoMode(QLineEdit::Normal);
		}
		else
		{
			emailPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);
		}
		update();
	}

	/* ------------------------------------------------------------------- */

	void EmailSettings::save()
	{
		int port{ 587 };	// default is 587
		switch (smtpPort->currentIndex())
		{
		case 0:
			port = 465;
			break;
		case 2:
			port = 2525;
			break;
		default:
			port = 587;
			break;
		}

		uint encryption{ static_cast<uint>(smtpEncryption->currentIndex()) };
		QString password{ emailPassword->text() };

		//
		// Obfuscate the password prior to saving it
		//
		for (auto& character : password)
		{
			character = QChar(static_cast<uint16_t>(character.unicode()) ^ 0x82U);
		}

		liveSettings.setEmailSettings(emailTo->text(), emailSubject->text(),
			smtpServer->text(), port, encryption,
			emailAccount->text(), password);

		liveSettings.save();
		accept();
	}

	/* ------------------------------------------------------------------- */

	void EmailSettings::test()
	{
		int port{ 587 };	// default is 587
		if (1 == smtpPort->currentIndex())
			port = 2525;

		SmtpClient::ConnectionType connectionType{ static_cast<SmtpClient::ConnectionType>(smtpEncryption->currentIndex()) };

		MimeMessage message;

		EmailAddress sender("david.partridge@perdrix.co.uk", "DeepSkyStackerLive");
		message.setSender(sender);

		EmailAddress to(emailTo->text(), "");
		message.addRecipient(to);

		message.setSubject(emailSubject->text());

		// Now add some text to the email.
		// First we create a MimeText object.

		MimeText text;

		text.setText("Hi,\nThis is a simple email message.\n");

		// Now add it to the mail

		message.addPart(&text);

		// Now we can send the mail
		SmtpClient smtp(smtpServer->text(), port, connectionType);

		smtp.connectToHost();
		if (!smtp.waitForReadyConnected())
		{
			QString errorMessage{ "Failed to connect to host %1 (%2)!" };
			QMessageBox::warning(this, "SMTP", errorMessage.arg(smtpServer->text()).arg(port));
			return;
		}

		smtp.login(emailAccount->text(), emailPassword->text());
		if (!smtp.waitForAuthenticated())
		{
			QString errorMessage{ "Failed to login as %1!" };

			QMessageBox::warning(this, "SMTP", errorMessage.arg(emailAccount->text()));
			return;
		}

		smtp.sendMail(message);
		if (!smtp.waitForMailSent())
		{
			QMessageBox::warning(this, "SMTP", "Failed to send mail!");
			return;
		}

		smtp.quit();
		QMessageBox::information(this, "SMTP", "Test email sent OK!");
	}
}
