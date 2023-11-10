// CheckAbove.cpp : implementation file
//

#include "stdafx.h"
#include "CheckAbove.h"
#include "ui/ui_CheckAbove.h"

namespace DSS
{
	CheckAbove::CheckAbove(QWidget* parent /*=nullptr*/) :
		QDialog(parent),
		ui(new Ui::CheckAbove)
	{
		ui->setupUi(this);
		connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		ui->lineEdit->setValidator(new CheckAboveValidator(this));
	}

	CheckAbove::~CheckAbove()
	{
		delete ui;
	}

	void CheckAbove::accept()
	{
		QString			strValue = ui->lineEdit->text();
		
		ZASSERT(ui->lineEdit->validator());
		if (strValue.endsWith(ui->lineEdit->validator()->locale().percent()))
		{
			m_bPercent = true;
			strValue = strValue.left(strValue.length() - 1);
		}
		m_fThreshold = strValue.toDouble();

		Inherited::accept();
	}



	CheckAboveValidator::CheckAboveValidator(QObject* parent /*= nullptr*/) :
		QValidator(parent),
		m_doubleValueValidator{ new QDoubleValidator{this} },
		m_percentValueValidator{ new QDoubleValidator{0., 99.99, 2, this} }
	{
		m_doubleValueValidator->setNotation(QDoubleValidator::StandardNotation);
		//TODO: define top/bottom if known


		// Note: the only downside of using a QDoubleValidator and not creating a custom one is 
		// it will accept "weird" expressions like: '.%' or '+1.%'
		// But then they're valid expressions and they would be hard to avoid anyway.
		m_percentValueValidator->setNotation(QDoubleValidator::StandardNotation);
	}

	QValidator::State CheckAboveValidator::validate(QString& input, int&) const
	{
		QValidator::State state;
		auto tmpPos = 0;
		const auto percent = locale().percent();
		if (input.endsWith(percent))
		{
			auto tmpInput = input.left(input.length() - percent.length());
			if (!tmpInput.isEmpty())
			{
				state = m_percentValueValidator->validate(tmpInput, tmpPos);
				input = tmpInput + percent;
			}
			else
			{
				state = QValidator::State::Invalid;
			}
		}
		else
		{
			state = m_doubleValueValidator->validate(input, tmpPos);
		}
		return state;
	}

	void CheckAboveValidator::fixup(QString& input) const
	{
		const auto percent = locale().percent();
		if (input.endsWith(percent))
		{
			auto tmpInput = input.left(input.length() - percent.length());
			m_percentValueValidator->fixup(input);
			input = tmpInput + percent;
		}
		else
		{
			m_doubleValueValidator->fixup(input);
		}
	}
}
