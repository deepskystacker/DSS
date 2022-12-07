#include "stdafx.h"
#include "deepskystacker.h"
#include "QtProgressDlg.h"
#include <QMessageBox>
#include "qevent.h"
#include "ui/ui_ProgressDlg.h"

using namespace DSS;

ProgressDlg::ProgressDlg(QWidget* parent) : 
	QDialog(parent),
	ui(new Ui::ProgressDlg),
	m_bCancelInProgress(false)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint));
	connect(ui->StopButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

	retainHiddenWidgetSize(*ui->ProcessText1);
	retainHiddenWidgetSize(*ui->ProcessText2);
	retainHiddenWidgetSize(*ui->ProgressBar1);
	retainHiddenWidgetSize(*ui->ProgressBar2);
}
ProgressDlg::~ProgressDlg()
{
	if(ui)
		delete ui;
}

void ProgressDlg::retainHiddenWidgetSize(QWidget& rWidget)
{
	QSizePolicy sp_retain = rWidget.sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	rWidget.setSizePolicy(sp_retain);
}

const QString ProgressDlg::getStart1Text() const
{
	return ui->ProcessText1->text();
}
const QString ProgressDlg::getStart2Text() const
{
	return ui->ProcessText2->text();
}

void ProgressDlg::setStart1Text(const QString& strText)
{
	ui->ProcessText1->setText(strText);
}
void ProgressDlg::setStart2Text(const QString& strText)
{
	ui->ProcessText2->setText(strText);
}
void ProgressDlg::setProgress1(int lAchieved)
{
	ui->ProgressBar1->setValue(lAchieved);
}
void ProgressDlg::setProgress2(int lAchieved)
{
	ui->ProgressBar2->setValue(lAchieved);
}
void ProgressDlg::setTimeRemaining(const QString& strText)
{
	ui->TimeRemaining->setText(strText);
}
void ProgressDlg::setProcessorsUsed(int lNrProcessors)
{
	if (lNrProcessors > 1)
		ui->Processors->setText(QString::number(lNrProcessors) + " Processors Used");
	else
		ui->Processors->setText("");
}
void ProgressDlg::cancelPressed()
{
	if (QMessageBox::question(this, "Are You Sure?", "Are you sure you wish to cancel this operation?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		m_bCancelInProgress = true;
		ui->StopButton->setEnabled(false);
	}
}
void ProgressDlg::EnableCancelButton(bool bState)
{
	ui->StopButton->setEnabled(bState);
}
void ProgressDlg::SetTitleText(const QString& strText)
{
	setWindowTitle(strText);
}
void ProgressDlg::setProgress1Range(int nMin, int nMax)
{
	ui->ProgressBar1->setRange(nMin, nMax);
}
void ProgressDlg::setProgress2Range(int nMin, int nMax)
{
	ui->ProgressBar2->setRange(nMin, nMax);
}
void ProgressDlg::setItemVisibility(bool bSet1, bool bSet2)
{
	ui->ProcessText1->setVisible(bSet1);
	ui->ProgressBar1->setVisible(bSet1);
	
	ui->ProcessText2->setVisible(bSet2);
	ui->ProgressBar2->setVisible(bSet2);
}
void ProgressDlg::RunDialog()
{
	raise(); show(); 
	QApplication::processEvents();
}

void ProgressDlg::closeEvent(QCloseEvent* pEvent)
{
	cancelPressed();
	pEvent->ignore();
}

/////////////////////////////////////////////////////////////////
const QString DSSProgressDlg::sm_EmptyString;
DSSProgressDlg::DSSProgressDlg(QWidget* pParent/*= nullptr*/) :
	m_pParent(nullptr),
	m_bEnableCancel{ false },
	m_lTotal1{ 0 },
	m_lTotal2{ 0 },
	m_dwStartTime{ 0 },
	m_dwLastTime{ 0 },
	m_lLastTotal1{ 0 },
	m_lLastTotal2{ 0 },
	m_bFirstProgress{ false }
{}

DSSProgressDlg::~DSSProgressDlg()
{
	Close();
};

void DSSProgressDlg::SetNrUsedProcessors(int lNrProcessors/*=1*/)
{
	if (!m_pDlg)
		return;

	m_pDlg->setProcessorsUsed(lNrProcessors);
}

const QString DSSProgressDlg::GetStartText() const
{
	if (!m_pDlg)
		return sm_EmptyString;

	return m_pDlg->getStart1Text();
};

const QString DSSProgressDlg::GetStart2Text() const
{
	if (!m_pDlg)
		return sm_EmptyString;

	return m_pDlg->getStart2Text();
};

void DSSProgressDlg::Start(const QString& szTitle, int lTotal1, bool bEnableCancel/*=true*/)
{
	if(!CreateProgressDialog())
		return;

	m_lLastTotal1 = 0;
	m_lTotal1 = lTotal1;
	m_dwStartTime = GetTickCount64();
	m_dwLastTime = m_dwStartTime;
	m_bFirstProgress = true;
	m_bEnableCancel = bEnableCancel;
	m_pDlg->EnableCancelButton(bEnableCancel);
	m_pDlg->SetTitleText(szTitle);
	m_pDlg->setProgress1Range(0, lTotal1);
	m_pDlg->setItemVisibility(true, false);
	m_pDlg->setFocus();

	m_pDlg->RunDialog();
}

void DSSProgressDlg::Progress1(const QString& szText, int lAchieved1)
{	
	unsigned long long dwCurrentTime = GetTickCount64();
	m_pDlg->setStart1Text(szText);

	if (m_bFirstProgress || (static_cast<double>(lAchieved1 - m_lLastTotal1) > (m_lTotal1 / 100.0)) || ((dwCurrentTime - m_dwLastTime) > 1000))
	{
		m_bFirstProgress = false;
		m_lLastTotal1 = lAchieved1;
		m_dwLastTime = dwCurrentTime;
		m_pDlg->setProgress1(lAchieved1);

		if (m_lTotal1 > 1 && lAchieved1 > 1)
		{
			std::uint32_t dwRemainingTime = static_cast<std::uint32_t>(static_cast<double>(dwCurrentTime - m_dwStartTime) / static_cast<double>(lAchieved1 - 1) * static_cast<double>(m_lTotal1 - lAchieved1 + 1));
			dwRemainingTime /= 1000;

			const std::uint32_t dwHour = dwRemainingTime / 3600;
			dwRemainingTime -= dwHour * 3600;
			const std::uint32_t dwMin = dwRemainingTime / 60;
			dwRemainingTime -= dwMin * 60;
			const std::uint32_t dwSec = dwRemainingTime;

			QString qStrText;
			if (dwHour != 0)
				qStrText = QCoreApplication::translate("ProgressDlg", "Estimated remaining time: %1 hr %2 mn %3 s ",
										 "IDS_ESTIMATED3").arg(dwHour).arg(dwMin).arg(dwSec);
			else if (dwMin != 0)
				qStrText = QCoreApplication::translate("ProgressDlg", "Estimated remaining time: %1 mn %2 s ",
										 "IDS_ESTIMATED2").arg(dwMin).arg(dwSec);
			else if (dwSec != 0)
				qStrText = QCoreApplication::translate("ProgressDlg", "Estimated remaining time : %1 s ",
										"IDS_ESTIMATED1").arg(dwSec);
			else
				qStrText = QCoreApplication::translate("ProgressDlg", "Estimated remaining time: < 1 s ",
										"IDS_ESTIMATED0");
			
			m_pDlg->setTimeRemaining(qStrText);
		}
		else
		{
			const QString qStrText = QCoreApplication::translate("ProgressDlg", "Estimated remaining Time: Unknown",
												"IDS_ESTIMATEDUNKNOWN");
			m_pDlg->setTimeRemaining(qStrText);
		};

		m_pDlg->RunDialog();
	};
}

void DSSProgressDlg::Start2(const QString& szText, int lTotal2)
{
	if (!CreateProgressDialog())
		return;

	m_lLastTotal2 = 0;
	m_pDlg->setStart2Text(szText);

	m_pDlg->setProgress2Range(0, lTotal2);
	m_lTotal2 = lTotal2;
	if (lTotal2 == 0)
	{
		m_pDlg->setItemVisibility(true, false);
	}
	else
	{
		m_pDlg->setItemVisibility(true, true);
		m_pDlg->setProgress2(0);
	};

	if (m_bJointProgress)
	{
		Start(nullptr, lTotal2, m_bEnableCancel);
		m_pDlg->setStart1Text(szText);
	};

	m_pDlg->RunDialog();
}

void DSSProgressDlg::Progress2(const QString& szText, int lAchieved2)
{
	if (static_cast<double>(lAchieved2 - m_lLastTotal2) > (m_lTotal2 / 100.0))
	{
		m_lLastTotal2 = lAchieved2;
		m_pDlg->setStart2Text(szText);
		m_pDlg->setProgress2(lAchieved2);
		m_pDlg->RunDialog();
	};

	if (m_bJointProgress)
		Progress1(szText, lAchieved2);
}

void DSSProgressDlg::End2()
{
	m_pDlg->setItemVisibility(true, false);
}

bool DSSProgressDlg::IsCanceled()
{
	return m_pDlg->IsCancelled();
}

bool DSSProgressDlg::Warning(const QString& szText)
{
	return (QMessageBox::question(m_pDlg.get(), "", szText) == QMessageBox::Yes);
}

bool DSSProgressDlg::Close()
{
	if (!m_pDlg)
		return true;

	m_pDlg = nullptr;
	DeepSkyStacker::instance()->enableSubDialogs();
	return true;
}

bool DSSProgressDlg::CreateProgressDialog()
{
	if (m_pDlg)
		return true;
	
	m_pDlg = std::make_unique<ProgressDlg>(m_pParent);
	if (!m_pDlg)
		return false;

	// Disable child dialogs of DeepSkyStackerDlg
	DeepSkyStacker::instance()->disableSubDialogs();
	return true;
};