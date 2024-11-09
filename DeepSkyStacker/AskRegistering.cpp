#include "stdafx.h"
#include "AskRegistering.h"
#include "ui_AskRegistering.h"

namespace DSS
{
	AskRegistering::AskRegistering(QWidget* parent) :
		QDialog(parent),
		ui(new Ui::AskRegistering)
	{
		ui->setupUi(this);
		connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	}

	AskRegistering::~AskRegistering()
	{
		delete ui;
	}

	void AskRegistering::reject()
	{
		ui->registerNone->setChecked(true);// so that ::desiredAction return Answer::ARA_CONTINUE
		QDialog::reject();
	}

	AskRegistering::Answer AskRegistering::desiredAction() const
	{
		std::map<QRadioButton*, Answer> answerMap{
			{ui->registerAll,Answer::ARA_ALL},
			{ui->registerOne,Answer::ARA_ONE},
			{ui->registerNone,Answer::ARA_CONTINUE},
		};

		for (auto& pair : answerMap) {
			if (pair.first->isChecked()) {
				return pair.second;
			}
		}

		ZASSERT(false);
		return Answer::ARA_CONTINUE;
	}
}