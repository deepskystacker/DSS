// CheckAbove.cpp : implementation file
//

#include "stdafx.h"

#include "CheckAbove.h"
#include "ui/ui_CheckAbove.h"


namespace DSS
{
	CheckAbove::CheckAbove(QWidget* pParent /*=nullptr*/) :
		QDialog(pParent),
		ui(new Ui::CheckAbove)
	{
		ui->setupUi(this);
	}

	CheckAbove::~CheckAbove()
	{
		delete ui;
	}



	void CheckAbove::accept()
	{
		QString			strValue = ui->lineEdit->text();

		if (strValue.contains(QLatin1Char('%')))
		{
			m_bPercent = true;
			strValue = strValue.left(strValue.length() - 1);
		}
		m_fThreshold = strValue.toDouble();

		Inherited::accept();
	}
}
