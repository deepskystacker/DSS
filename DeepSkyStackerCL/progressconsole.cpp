/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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
// ProgressConsole.cpp : Defines the entry point for the console progress display.
//
#include <stdafx.h>
#include "progressconsole.h"
namespace DSS
{
	/************************************************************************************/
	/* SLOTS                                                                            */
	/************************************************************************************/
	void ProgressConsole::slotStart1(const QString& title, int total1, bool enableCancel /* = true */)
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

	void ProgressConsole::slotProgress1(const QString& szText, int lAchieved1)
	{
		// Always update on first loop, then only if a second has passed or a min progress has occurred.
		if (!(m_firstProgress ||
			(static_cast<double>(lAchieved1 - m_lastTotal1) > (m_total1 / 100.0) * m_minProgressStep) ||	// Update only if diff is sm_fMinProgressStep %age change
			(m_timer.elapsed() > 1000)
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

	void ProgressConsole::slotStart2(const QString& szText, int lTotal2)
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

	void ProgressConsole::slotProgress2(const QString& szText, int lAchieved2)
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

	void ProgressConsole::slotEnd2()
	{
		ProgressBase::Progress2(m_total2);	// Set to 100% is ending.
		UpdateProcessorsUsed();
		endProgress2();
	}

	/************************************************************************************/

	void ProgressConsole::slotClose()
	{
		closeProgress();
	}
}