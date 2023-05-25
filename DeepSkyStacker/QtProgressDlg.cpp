#include "stdafx.h"
#include "QtProgressDlg.h"
#include "ui/ui_ProgressDlg.h"
#include "DeepSkyStacker.h"

using namespace DSS;

const QString ProgressDlg::m_emptyString;

namespace {
	QMainWindow* GetMainApplicationWindow()
	{
		for (QWidget* pWnd : QApplication::topLevelWidgets())
			if (pWnd && pWnd->inherits(QMainWindow::staticMetaObject.className()))
				return dynamic_cast<QMainWindow*>(pWnd);
		return nullptr;
	}
}


ProgressDlg::ProgressDlg(QObject* parent) :
	ProgressBase{ parent },
	theDialog{ new QDialog(GetMainApplicationWindow()) },   // make the dlg application modal
	ui{ new Ui::ProgressDlg },
	m_cancelInProgress{ false }
{
	ui->setupUi(theDialog);
	theDialog->setWindowFlags(theDialog->windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint));
	ProgressDlg::connect(ui->StopButton, &QPushButton::clicked, this, &ProgressDlg::cancelPressed);

	retainHiddenWidgetSize(*ui->ProcessText1);
	retainHiddenWidgetSize(*ui->ProcessText2);
	retainHiddenWidgetSize(*ui->ProgressBar1);
	retainHiddenWidgetSize(*ui->ProgressBar2);
}

ProgressDlg::~ProgressDlg()
{
	Close();
}

void ProgressDlg::retainHiddenWidgetSize(QWidget& rWidget)
{
	QSizePolicy sp_retain = rWidget.sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	rWidget.setSizePolicy(sp_retain);
}

void ProgressDlg::EnableCancelButton(bool bState)
{
	ui->StopButton->setEnabled(bState);
}
void ProgressDlg::applyTitleText(const QString& strText)
{
	if (!strText.isEmpty())
		theDialog->setWindowTitle(strText);
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

void ProgressDlg::closeEvent(QCloseEvent* pEvent)
{
	cancelPressed();
	pEvent->ignore();
}

void ProgressDlg::cancelPressed()
{
	if (QMessageBox::question(theDialog, "DeepSkyStacker", tr("Are you sure you wish to cancel this operation?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		m_cancelInProgress = true;
		ui->StopButton->setEnabled(false);
	}
}

void ProgressDlg::setTimeRemaining(const QString& strText)
{
	ui->TimeRemaining->setText(strText);
	QApplication::processEvents();
}

//////////////////////////////////////////////////////////////////////////
// ProgressBase
void ProgressDlg::initialise()
{
	// Disable child dialogs of DeepSkyStackerDlg
	DeepSkyStacker::instance()->disableSubDialogs();

	EnableCancelButton(m_enableCancel);
	setProgress1Range(0, m_total1);
	setItemVisibility(true, false);
	theDialog->setFocus();
	applyTitleText(GetTitleText());

	theDialog->raise();
	theDialog->show();
	QApplication::processEvents();
}

void ProgressDlg::applyStart1Text(const QString& strText)
{
	ui->ProcessText1->setText(strText);
	theDialog->raise();
	theDialog->show();
	QApplication::processEvents();
}

void ProgressDlg::applyStart2Text(const QString& strText)
{
	ui->ProcessText2->setText(strText);
	setProgress2Range(0, m_total2);
	if (m_total2 == 0)
	{
		setItemVisibility(true, false);
	}
	else
	{
		setItemVisibility(true, true);
		applyProgress2(0);
	}
	theDialog->raise();
	theDialog->show();
	QApplication::processEvents();
}

void ProgressDlg::applyProgress1(int lAchieved)
{
	ui->ProgressBar1->setValue(lAchieved);

	// Now do time remaining as well
	if (m_total1 > 1 && lAchieved > 1)
	{
		std::uint32_t dwRemainingTime = static_cast<std::uint32_t>(static_cast<double>(m_timer.elapsed()) / static_cast<double>(lAchieved - 1) * static_cast<double>(m_total1 - lAchieved + 1));
		if (lAchieved > m_total1)	// If OpemMP tasks are not multiple of processors, this gets too large!
			dwRemainingTime = 0;
		else
			dwRemainingTime /= 1000;

		const std::uint32_t dwHour = dwRemainingTime / 3600;
		dwRemainingTime -= dwHour * 3600;
		const std::uint32_t dwMin = dwRemainingTime / 60;
		dwRemainingTime -= dwMin * 60;
		const std::uint32_t dwSec = dwRemainingTime;

		QString qStrText;
		if (dwHour != 0)
			qStrText = tr("Estimated remaining time: %1 hr %2 mn %3 s ",
				"IDS_ESTIMATED3").arg(dwHour).arg(dwMin).arg(dwSec);
		else if (dwMin != 0)
			qStrText = tr("Estimated remaining time: %1 mn %2 s ",
				"IDS_ESTIMATED2").arg(dwMin).arg(dwSec);
		else if (dwSec != 0)
			qStrText = tr("Estimated remaining time : %1 s ",
				"IDS_ESTIMATED1").arg(dwSec);
		else
			qStrText = tr("Estimated remaining time: < 1 s ",
				"IDS_ESTIMATED0");

		setTimeRemaining(qStrText);
	}
	else
	{
		const QString qStrText = tr("Estimated remaining Time: Unknown",
			"IDS_ESTIMATEDUNKNOWN");
		setTimeRemaining(qStrText);
	};
}

void ProgressDlg::applyProgress2(int lAchieved)
{
	ui->ProgressBar2->setValue(lAchieved);
	QApplication::processEvents();
}

void ProgressDlg::applyProcessorsUsed(int nCount)
{
	ui->Processors->setText(tr("%n Processor(s) Used", nullptr, nCount));
	QApplication::processEvents();
}

void ProgressDlg::endProgress2()
{
	setItemVisibility(true, false);
}

bool ProgressDlg::hasBeenCanceled()
{
	return IsCancelled();
}

bool ProgressDlg::doWarning(const QString& szText)
{
	return (QMessageBox::question(theDialog, "", szText) == QMessageBox::Yes);
}

void ProgressDlg::closeProgress()
{
	DeepSkyStacker::instance()->enableSubDialogs();
	theDialog->hide();
}
