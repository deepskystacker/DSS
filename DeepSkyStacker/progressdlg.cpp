/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// ProgressDlg.cpp : Implements the DeepSkyStacker Progress Dialog
//
#include "stdafx.h"
#include "progressdlg.h"
#include "ui/ui_ProgressDlg.h"
#include "DeepSkyStacker.h"

using namespace DSS;

ProgressDlg::ProgressDlg(QWidget* parent) :
	QDialog { parent },
	ProgressBase{ },
	ui{ new Ui::ProgressDlg },
	m_cancelInProgress{ false }
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~(Qt::WindowContextHelpButtonHint | Qt::WindowCloseButtonHint));
	ProgressDlg::connect(ui->StopButton, &QPushButton::clicked, this, &ProgressDlg::cancelPressed);

	retainHiddenWidgetSize(*ui->ProcessText1);
	retainHiddenWidgetSize(*ui->ProcessText2);
	retainHiddenWidgetSize(*ui->ProgressBar1);
	retainHiddenWidgetSize(*ui->ProgressBar2);
}

ProgressDlg::~ProgressDlg()
{
	Close();
	delete ui;
}

void ProgressDlg::Start1(const QString& title, int total1, bool enableCancel /* = true */)
{
	QMetaObject::invokeMethod(this, "slotStart1", Qt::AutoConnection,
		Q_ARG(const QString&, title),
		Q_ARG(int, total1),
		Q_ARG(bool, enableCancel));
}

void ProgressDlg::Progress1(const QString& text, int achieved)
{
	QMetaObject::invokeMethod(this, "slotProgress1", Qt::AutoConnection,
		Q_ARG(const QString&, text),
		Q_ARG(int, achieved));
}

void ProgressDlg::Start2(const QString& title, int total2)
{
	QMetaObject::invokeMethod(this, "slotStart2", Qt::AutoConnection,
		Q_ARG(const QString&, title),
		Q_ARG(int, total2));
}

void ProgressDlg::Progress2(const QString& text, int achieved)
{
	QMetaObject::invokeMethod(this, "slotProgress2", Qt::AutoConnection,
		Q_ARG(const QString&, text),
		Q_ARG(int, achieved));
}

void ProgressDlg::End2()
{
	QMetaObject::invokeMethod(this, "slotEnd2", Qt::AutoConnection);
}

void ProgressDlg::Close()
{
	QMetaObject::invokeMethod(this, "slotClose", Qt::AutoConnection);
}

bool ProgressDlg::Warning(const QString& szText)
{
	return doWarning(szText);
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

void ProgressDlg::closeEvent(QCloseEvent* pEvent)
{
	cancelPressed();
	pEvent->ignore();
}

void ProgressDlg::cancelPressed()
{
	if (QMessageBox::question(this, "DeepSkyStacker", tr("Are you sure you wish to cancel this operation?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
	{
		m_cancelInProgress = true;
		ui->StopButton->setEnabled(false);
	}
}

void ProgressDlg::setTimeRemaining(const QString& strText)
{
	ui->TimeRemaining->setText(strText);
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
	setFocus();
	applyTitleText(GetTitleText());

	raise();
	show();
}

void ProgressDlg::applyStart1Text(const QString& strText)
{
	ui->ProcessText1->setText(strText);
	raise();
	show();
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
	raise();
	show();
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
}

void ProgressDlg::applyProcessorsUsed(int nCount)
{
	ui->Processors->setText(tr("%n Processor(s) Used", nullptr, nCount));
}

void ProgressDlg::endProgress2()
{
	setItemVisibility(true, false);
}

bool ProgressDlg::doWarning(const QString& szText)
{
	return (QMessageBox::question(this, "", szText) == QMessageBox::Yes);
}

void ProgressDlg::closeProgress()
{
	DeepSkyStacker::instance()->enableSubDialogs();
	hide();
}

/************************************************************************************/
/* SLOTS                                                                            */
/************************************************************************************/
void ProgressDlg::slotStart1(const QString& title, int total1, bool enableCancel /* = true */)
{
	m_lastTotal1 = 0;
	m_total1 = total1;
	m_timer.start();
	m_firstProgress = true;
	m_enableCancel = enableCancel;

	if (GetTitleText().compare(title, Qt::CaseInsensitive) != 0)
	{
		m_strLastOut[OT_TITLE] = title;
		applyTitleText(GetTitleText());
	}
	UpdateProcessorsUsed();
	initialise();
	QCoreApplication::processEvents();
}

/************************************************************************************/

void ProgressDlg::slotProgress1(const QString& szText, int lAchieved1)
{
	// Always update on first loop, then only if 100ms has passed or a min progress has occurred.
	if (!(m_firstProgress ||
		(static_cast<double>(lAchieved1 - m_lastTotal1) > (m_total1 / 100.0) * m_minProgressStep) ||	// Update only if diff is sm_fMinProgressStep %age change
		(m_timer.elapsed() > 100)		// Was 1s (1000)
		))
		return;

	m_firstProgress = false;
	m_lastTotal1 = lAchieved1;

	if (GetStart1Text().compare(szText, Qt::CaseInsensitive) != 0)
	{
		if (!szText.isEmpty())
			m_strLastOut[OT_TEXT1] = szText;
		applyStart1Text(GetStart1Text());
	}

	if (m_total1)
	{
		double percentage = (double)m_lastTotal1 / (double)m_total1 * 100.0;
		m_strLastOut[OT_PROGRESS1] = QString("%1%").arg(percentage, 0, 'f', 0);
		applyProgress1(lAchieved1);
	}
	UpdateProcessorsUsed();
	QCoreApplication::processEvents();
}

/************************************************************************************/

void ProgressDlg::slotStart2(const QString& szText, int lTotal2)
{
	m_lastTotal2 = 0;
	m_total2 = lTotal2;
	if (GetStart2Text().compare(szText, Qt::CaseInsensitive) != 0)
	{
		if (!szText.isEmpty())
			m_strLastOut[OT_TEXT2] = szText;
		applyStart2Text(GetStart2Text());
	}

	if (m_jointProgress)
		Start1(GetStart2Text(), m_total2, m_enableCancel);

	UpdateProcessorsUsed();
	QCoreApplication::processEvents();
}

/************************************************************************************/

void ProgressDlg::slotProgress2(const QString& szText, int lAchieved2)
{
	// Always update after a min progress has occurred.
	float fAmountSoFar = (float)m_lastTotal2 / ((float)((m_total2 / 100.0) * m_minProgressStep));
	float fRoundedSoFar = ceil(fAmountSoFar);

	float fAmountGoingTo = (float)lAchieved2 / ((float)((m_total2 / 100.0) * m_minProgressStep));
	float fRoundedGoingTo = ceil(fAmountGoingTo);

	if (fRoundedGoingTo <= fRoundedSoFar &&
		lAchieved2 < m_total2)
		return;

	if (lAchieved2 > m_total2)
		m_total2 = lAchieved2;
	m_lastTotal2 = lAchieved2;

	double percentage = 0.0f;
	if (m_total2)
		percentage = (double)m_lastTotal2 / (double)m_total2 * 100.0;

	if (GetStart2Text().compare(szText, Qt::CaseInsensitive) != 0)
	{
		if (!szText.isEmpty())
			m_strLastOut[OT_TEXT2] = szText;
		if (!m_jointProgress)
			applyStart2Text(GetStart2Text());
	}

	if (m_jointProgress)
	{
		m_strLastOut[OT_PROGRESS1] = QString("%1%").arg(percentage, 0, 'f', 0);
		applyProgress1(lAchieved2);
		m_total2 = 0;
	}
	else if (m_total2)
	{
		m_strLastOut[OT_PROGRESS2] = QString("%1%").arg(percentage, 0, 'f', 0);
		applyProgress2(lAchieved2);
	}
	UpdateProcessorsUsed();

	QCoreApplication::processEvents();
}

/************************************************************************************/

void ProgressDlg::slotEnd2()
{
	ProgressBase::Progress2(m_total2);	// Set to 100% is ending.
	UpdateProcessorsUsed();
	endProgress2();
}

/************************************************************************************/

void ProgressDlg::slotClose()
{
	closeProgress();
}