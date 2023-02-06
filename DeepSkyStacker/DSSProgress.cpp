#include <stdafx.h>
#include "DSSProgress.h"
#include "boost/winapi/time.hpp"

using namespace DSS;

const QString ProgressBase::m_strEmptyString("");

void ProgressBase::Start1(const QString& szTitle, int lTotal1, bool bEnableCancel /* = true */)
{
	m_lastTotal1 = 0;
	m_total1 = lTotal1;
	m_timer.start();
	m_firstProgress = true;
	m_enableCancel = bEnableCancel;	

	if (GetTitleText().compare(szTitle, Qt::CaseInsensitive) != 0)
	{
		m_strLastOut[OT_TITLE] = szTitle;
		applyTitleText(GetTitleText());
	}
	initialise();
}

void ProgressBase::Start2(const QString& szText, int lTotal2)
{
	m_lastTotal2 = 0;
	m_total2 = lTotal2;
	if (GetStart2Text().compare(szText, Qt::CaseInsensitive) != 0)
	{
		if(!szText.isEmpty())
			m_strLastOut[OT_TEXT2] = szText;
		applyStart2Text(GetStart2Text());
	}

	if (m_jointProgress)
		Start1(GetStart2Text(), m_total2, m_enableCancel);
		//Start(GetTitleText(), m_total2, m_enableCancel);
}

bool ProgressBase::Progress1(const QString& szText, int lAchieved1)
{
	// Always update on first loop, then only if a second has passed or a min progress has occurred.
	if (!(m_firstProgress ||
		(static_cast<double>(lAchieved1 - m_lastTotal1) > (m_total1 / 100.0) * m_minProgressStep) ||	// Update only if diff is sm_fMinProgressStep %age change
		(m_timer.elapsed() > 1000)
		))
		return false;

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
	return true;
}

bool ProgressBase::Progress2(const QString& szText, int lAchieved2)
{
	// Always update after a min progress has occurred.
	float fAmountSoFar = (float)m_lastTotal2 / ((float)((m_total2 / 100.0) * m_minProgressStep));
	float fRoundedSoFar = ceil(fAmountSoFar);

	float fAmountGoingTo = (float)lAchieved2 / ((float)((m_total2 / 100.0) * m_minProgressStep));
	float fRoundedGoingTo = ceil(fAmountGoingTo);

	if (fRoundedGoingTo <= fRoundedSoFar &&
		lAchieved2 < m_total2)
		return false;

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

	return true;
}

void ProgressBase::End2()
{
	Progress2(m_total2);	// Set to 100% is ending.
	endProgress2();
}
bool ProgressBase::IsCanceled()
{
	return hasBeenCanceled();
}
void ProgressBase::Close()
{
	closeProgress();
}
bool ProgressBase::Warning(const QString& szText)
{
	return doWarning(szText);
}
void ProgressBase::SetNrUsedProcessors(int lNrProcessors /* = 1 */)
{
	setProcessorsUsed(lNrProcessors);
}