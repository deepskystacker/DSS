/****************************************************************************
**
** Copyright (C) 2025 David C. Partridge
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
// ProgressDlg.cpp : Implements the NEW DeepSkyStacker Progress Dialog
//
#include "pch.h"
#include "progressdlg.h"
#include "DeepSkyStacker.h"

using namespace DSS;

/*!
  Constructs a progress dialog.

  Default settings:
  \list
  \li The top and bottom label texts are empty.
  \li The cancel button text is (translated) "Cancel".
  \li partialMinimum is 0
  \li partialMaximum is 100
  \li totalMinimum is 0
  \li totalMaximum is 100
  \endlist

  The \a parent argument is dialog's parent widget.

  The \a mode argument is the progress mode. It can be either \c ProgressMode::Single, or
  \c ProgressMode::Dual. The default is \c ProgressMode::Dual.  If \c ProgressMode::Single is
  specified, the first (partial) progress bar is hidden, and the second (total) progress bar is shown.

  The \a enableCancel argument specifies whether the cancel button is enabled or not.
  The default is \c true, which means the cancel button is enabled.

  \sa setTopText(), setBottomText(),
  setPartialMinimum(), setPartialMaximum(), setPartialRange(), setPartialValue(),
  setTotalMinimum() and setTotalMaximum(), setTotalRange(), setTotalValue()
*/
ProgressDlg::ProgressDlg(
	QWidget* parent,
	bool enableCancel,
	Qt::WindowFlags f) :
	QDialog{ parent, f },
	ProgressBase{ }
{
	Inherited::cancelEnabled = enableCancel;

	setupUi(this);

	// Don't mess with size of our hidden widgets
	retainHiddenWidgetSize(topLabel);
	retainHiddenWidgetSize(bottomLabel);
	retainHiddenWidgetSize(partialProgress);
	retainHiddenWidgetSize(totalProgess);

	//
	// Hide the partial text and progress bar if we are in single mode
	//
	if (mode == ProgressMode::Single)
	{
		topLabel->setVisible(false);
		partialProgress->setVisible(false);
	}
	else
	{
		topLabel->setVisible(true);
		partialProgress->setVisible(true);

	}


	connect(cancelButton, &QPushButton::clicked, this, &ProgressDlg::cancelPressed);
}

ProgressDlg::~ProgressDlg()
{
	hideProgress();
}

//
// Slots
//
void ProgressDlg::setMode(ProgressMode mode)
{
	//
	// Hide the partial text and progress bar if we are in single mode
	//
	if (mode == ProgressMode::Single)
	{
		topLabel->setVisible(false);
		partialProgress->setVisible(false);
	}
	else
	{
		topLabel->setVisible(true);
		partialProgress->setVisible(true);

	}
}

void ProgressDlg::setTitleText(const QString& title)
{
	setWindowTitle(title);
}

void ProgressDlg::setTopText(QString& text)
{
	topLabel->setText(text);
}

void ProgressDlg::setPartialMinimum(int minimum)
{
	partialMinimum = minimum;
	partialProgress->setMinimum(minimum);
}

void ProgressDlg::setPartialMaximum(int maximum)
{
	partialMaximum = maximum;
	partialProgress->setMaximum(maximum);
}

void ProgressDlg::setPartialRange(int minimum, int maximum)
{
	partialMinimum = minimum; partialMaximum = maximum;
	partialProgress->setRange(minimum, maximum);
}

void ProgressDlg::setPartialValue(int value)
{
	partialValue = value;
	partialProgress->setValue(value);
}

void ProgressDlg::setTotalMinimum(int minimum)
{
	totalMinimum = minimum;
	totalProgress->setMinimum(minimum);
}

void ProgressDlg::setTotalMaximum(int maximum)
{
	totalMaximum = maximum;
	totalProgress->setMaximum(maximum);
}

void ProgressDlg::setTotalRange(int minimum, int maximum)
{
	totalMinimum = minimum; totalMaximum = maximum;
	totalProgress->setRange(minimum, maximum);
}

void ProgressDlg::setTotalValue(int value)
{
	totalProgress->setValue(value);

	//
	// Guesstimate time remaining
	//
	ZASSERT(value > 0);
	std::uint32_t remainingTime = static_cast<std::uint32_t>(static_cast<double>(timer.elapsed()) / static_cast<double>(value - 1) * static_cast<double>(totalMaximum - value + 1));
	if (value > totalMaximum)	// If OpemMP tasks are not multiple of processors, this gets too large!
		remainingTime = 0;
	else
		remainingTime /= 1000;

	const std::uint32_t hours = remainingTime / 3600;
	remainingTime -= hours * 3600;
	const std::uint32_t minutes = remainingTime / 60;
	remainingTime -= minutes * 60;
	const std::uint32_t seconds = remainingTime;

	QString text;
	if (hours != 0)
		text = tr("Estimated remaining time: %1 hr %2 mn %3 s ",
			"IDS_ESTIMATED3").arg(hours).arg(minutes).arg(seconds);
	else if (minutes != 0)
		text = tr("Estimated remaining time: %1 mn %2 s ",
			"IDS_ESTIMATED2").arg(minutes).arg(seconds);
	else if (seconds != 0)
		text = tr("Estimated remaining time : %1 s ",
			"IDS_ESTIMATED1").arg(seconds);
	else
		text = tr("Estimated remaining time: < 1 s ",
			"IDS_ESTIMATED0");

	timeRemaining->setText(text);
}

void ProgressDlg::setBottomText(QString& text)
{
	bottomLabel->setText(text);
}


bool ProgressDlg::wasCanceled() const
{
	return canceled;
}


void ProgressDlg::retainHiddenWidgetSize(QWidget* widget)
{
	QSizePolicy sp_retain = widget->sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	widget->setSizePolicy(sp_retain);
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
		canceled = true;
		cancelButton->setEnabled(false);
	}
}

//////////////////////////////////////////////////////////////////////////
// ProgressBase
void ProgressDlg::showProgress()
{
	// Disable child dialogs of DeepSkyStackerDlg
	DeepSkyStacker::instance()->disableSubDialogs();

	enableCancel(true);
	setFocus();
	raise();
	show();
}

void ProgressDlg::hideProgress()
{
	DeepSkyStacker::instance()->enableSubDialogs();
	hide();
}

/************************************************************************************/
/* SLOTS                                                                            */
/************************************************************************************/
void ProgressDlg::slotSetTitleText(const QString& title)
{
	if (windowTitle().compare(title, Qt::CaseInsensitive) != 0)
	{
		setWindowTitle(title);
	}
	updateProcessorsUsed();
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
	updateProcessorsUsed();
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

	updateProcessorsUsed();
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
	updateProcessorsUsed();

	QCoreApplication::processEvents();
}

/************************************************************************************/

void ProgressDlg::slotEnd2()
{
	ProgressBase::Progress2(m_total2);	// Set to 100% is ending.
	updateProcessorsUsed();
	endProgress2();
}

/************************************************************************************/

void ProgressDlg::slotClose()
{
	closeProgress();
}