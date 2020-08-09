#include <stdafx.h>
#include "DSSTools.h"
#include "BitmapExt.h"
#include "BackgroundCalibration.h"
#include "DSSProgress.h"
#include "Multitask.h"

/* ------------------------------------------------------------------- */

class CBackgroundCalibrationTask : public CMultitask
{
private :
	CSmartPtr<CMemoryBitmap>		m_pBitmap;
	CDSSProgress *					m_pProgress;
	CBackgroundCalibration *		m_pBackgroundCalibration;

public :
	std::vector<LONG>				m_vRedHisto;
	std::vector<LONG>				m_vGreenHisto;
	std::vector<LONG>				m_vBlueHisto;

public :
	CBackgroundCalibrationTask()
	{
        m_pProgress = nullptr;
        m_pBackgroundCalibration = nullptr;
	}

	virtual ~CBackgroundCalibrationTask()
	{
	};

	void	AddToMainHistograms(const std::vector<LONG> & vRedHisto, const std::vector<LONG> & vGreenHisto, const std::vector<LONG> & vBlueHisto);

	void	Init(CBackgroundCalibration * pBackgroundCalibration, CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
	{
		m_pBackgroundCalibration = pBackgroundCalibration;
		m_pBitmap				 = pBitmap;
		m_pProgress				 = pProgress;

		m_vRedHisto.resize((LONG)MAXWORD+1);
		m_vGreenHisto.resize((LONG)MAXWORD+1);
		m_vBlueHisto.resize((LONG)MAXWORD+1);
	};

	virtual bool	DoTask(HANDLE hEvent);
	virtual bool	Process();
};

/* ------------------------------------------------------------------- */

void	CBackgroundCalibrationTask::AddToMainHistograms(const std::vector<LONG> & vRedHisto, const std::vector<LONG> & vGreenHisto, const std::vector<LONG> & vBlueHisto)
{
	m_CriticalSection.Lock();
	for (LONG i = 0;i<m_vRedHisto.size();i++)
	{
		m_vRedHisto[i]		+= vRedHisto[i];
		m_vGreenHisto[i]	+= vGreenHisto[i];
		m_vBlueHisto[i]		+= vBlueHisto[i];
	};
	m_CriticalSection.Unlock();
};

/* ------------------------------------------------------------------- */

bool	CBackgroundCalibrationTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;

	LONG				i, j;
	bool				bEnd = false;
	MSG					msg;
	LONG				lWidth = m_pBitmap->Width();
	double				fMultiplier = m_pBackgroundCalibration->m_fMultiplier;
	std::vector<LONG>	vRedHisto;
	std::vector<LONG>	vGreenHisto;
	std::vector<LONG>	vBlueHisto;

	vRedHisto.resize((LONG)MAXWORD+1);
	vGreenHisto.resize((LONG)MAXWORD+1);
	vBlueHisto.resize((LONG)MAXWORD+1);

	// Create a message queue and signal the event
	PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					COLORREF16		crColor;
					double			fRed, fGreen, fBlue;

					m_pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
					fRed   *= fMultiplier * 256.0;
					fGreen *= fMultiplier * 256.0;
					fBlue  *= fMultiplier * 256.0;

					crColor.red = min(fRed, static_cast<double>(MAXWORD));
					crColor.blue = min(fBlue, static_cast<double>(MAXWORD));
					crColor.green = min(fGreen, static_cast<double>(MAXWORD));

					vRedHisto[crColor.red]++;
					vGreenHisto[crColor.green]++;
					vBlueHisto[crColor.blue]++;
				};
			};

			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = true;
	};

	AddToMainHistograms(vRedHisto, vGreenHisto, vBlueHisto);

	return true;
};

/* ------------------------------------------------------------------- */

bool	CBackgroundCalibrationTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = true;
	LONG				lHeight = m_pBitmap->Height();
	LONG				i = 0;
	LONG				lStep;
	LONG				lRemaining;

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	lStep		= max(1L, lHeight/50);
	lRemaining	= lHeight;

	while (i<lHeight)
	{
		LONG			lAdd = min(lStep, lRemaining);
		DWORD			dwThreadId;

		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+=lAdd;
		lRemaining	-= lAdd;
		if (m_pProgress)
			m_pProgress->Progress2(nullptr, i);
	};

	CloseAllThreads();

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors();

	double				fMax = 0;

	for (i = (LONG)m_vRedHisto.size()-1;i>=0 && !fMax;i--)
		if (m_vRedHisto[i])
			fMax = i;

	m_pBackgroundCalibration->m_fSrcRedMax = fMax;

	fMax = 0;
	for (i = (LONG)m_vGreenHisto.size()-1;i>=0 && !fMax;i--)
		if (m_vGreenHisto[i])
			fMax = i;
	m_pBackgroundCalibration->m_fSrcGreenMax = fMax;

	fMax = 0;
	for (i = (LONG)m_vBlueHisto.size()-1;i>=0 && !fMax;i--)
		if (m_vBlueHisto[i])
			fMax = i;
	m_pBackgroundCalibration->m_fSrcBlueMax = fMax;

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CBackgroundCalibration::ComputeBackgroundCalibration(CMemoryBitmap * pBitmap, bool bFirst, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CBackgroundCalibrationTask		task;

	m_fSrcRedMax	= 0;
	m_fSrcGreenMax	= 0;
	m_fSrcBlueMax	= 0;

	if (pProgress)
		pProgress->Start2(nullptr, pBitmap->Height());

	task.Init(this, pBitmap, pProgress);
	task.StartThreads();
	task.Process();

/*
	LONG				i, j;
	std::vector<LONG>	vRedHisto;
	std::vector<LONG>	vGreenHisto;
	std::vector<LONG>	vBlueHisto;

	ZTRACE_RUNTIME("Compute Background Calibration\n");



	vRedHisto.resize((LONG)MAXWORD+1);
	vGreenHisto.resize((LONG)MAXWORD+1);
	vBlueHisto.resize((LONG)MAXWORD+1);

	if (pProgress)
		pProgress->Start2(nullptr, pBitmap->Height());

	for (j = 0;j<pBitmap->Height();j++)
	{
		for (i = 0;i<pBitmap->Width();i++)
		{
			COLORREF16		crColor;
			double			fRed, fGreen, fBlue;

			pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			fRed   *= m_fMultiplier * 256.0;
			fGreen *= m_fMultiplier * 256.0;
			fBlue  *= m_fMultiplier * 256.0;

			crColor.red = min(fRed, MAXWORD);
			crColor.blue = min(fBlue, MAXWORD);
			crColor.green = min(fGreen, MAXWORD);

			//crColor = pBitmap->GetPixel16(i, j);

			vRedHisto[crColor.red]++;
			vGreenHisto[crColor.green]++;
			vBlueHisto[crColor.blue]++;
			m_fSrcRedMax	= max(m_fSrcRedMax, crColor.red);
			m_fSrcGreenMax	= max(m_fSrcGreenMax, crColor.green);
			m_fSrcBlueMax	= max(m_fSrcBlueMax, crColor.blue);
		};

		if (pProgress)
			pProgress->Progress2(nullptr, j+1);
	};
*/
	// Find median value in each histogram
	LONG			lNrTotalValues;
	LONG			lNrValues;
	LONG			lIndice;

	lNrTotalValues = pBitmap->Width() * pBitmap->Height();
	lNrTotalValues /= 2;

	lNrValues = 0;
	lIndice   = 0;
	while (lNrValues < lNrTotalValues)
	{
		lNrValues += task.m_vRedHisto[lIndice];
		lIndice++;
	};
	m_fSrcRedBk = (double)lIndice/m_fMultiplier;

	lNrValues = 0;
	lIndice   = 0;
	while (lNrValues < lNrTotalValues)
	{
		lNrValues += task.m_vGreenHisto[lIndice];
		lIndice++;
	};
	m_fSrcGreenBk = (double)lIndice/m_fMultiplier;

	lNrValues = 0;
	lIndice   = 0;
	while (lNrValues < lNrTotalValues)
	{
		lNrValues += task.m_vBlueHisto[lIndice];
		lIndice++;
	};
	m_fSrcBlueBk = (double)lIndice/m_fMultiplier;

	ZTRACE_RUNTIME("Background Calibration: Median Red = %.2f - Green = %.2f - Blue = %.2f", m_fSrcRedBk/256.0, m_fSrcGreenBk/256.0, m_fSrcBlueBk/256.0);

	if (bFirst)
	{
		if (m_BackgroundCalibrationMode == BCM_PERCHANNEL)
		{
			m_fTgtRedBk		= m_fSrcRedBk;
			m_fTgtGreenBk	= m_fSrcGreenBk;
			m_fTgtBlueBk	= m_fSrcBlueBk;
		}
		else if (m_BackgroundCalibrationMode == BCM_RGB)
		{
			double			fTgtBk;

			if (m_RGBBackgroundMethod == RBCM_MAXIMUM)
				fTgtBk = max(m_fSrcRedBk, max(m_fSrcGreenBk, m_fSrcBlueBk));
			else if (m_RGBBackgroundMethod == RBCM_MINIMUM)
				fTgtBk = min(m_fSrcRedBk, min(m_fSrcGreenBk, m_fSrcBlueBk));
			else
				fTgtBk = Median(m_fSrcRedBk, m_fSrcGreenBk, m_fSrcBlueBk);

			if (m_fSrcRedMax > fTgtBk)
				m_fTgtRedBk = fTgtBk;
			else
				m_fTgtRedBk = m_fSrcRedBk;

			if (m_fSrcGreenMax > fTgtBk)
				m_fTgtGreenBk = fTgtBk;
			else
				m_fTgtGreenBk = m_fSrcGreenBk;

			if (m_fSrcBlueMax > fTgtBk)
				m_fTgtBlueBk = fTgtBk;
			else
				m_fTgtBlueBk = m_fSrcBlueBk;

			//m_fTgtRedBk = m_fTgtGreenBk = m_fTgtBlueBk = fTgtBk;
		};
		ZTRACE_RUNTIME("Target Background : Red = %.2f - Green = %.2f - Blue = %.2f", m_fTgtRedBk/256.0, m_fTgtGreenBk/256.0, m_fTgtBlueBk/256.0);
	};

	m_riRed.Initialize(0, m_fSrcRedBk, m_fSrcRedMax, 0, m_fTgtRedBk, m_fSrcRedMax);
	m_riGreen.Initialize(0, m_fSrcGreenBk, m_fSrcGreenMax, 0, m_fTgtGreenBk, m_fSrcGreenMax);
	m_riBlue.Initialize(0, m_fSrcBlueBk, m_fSrcBlueMax, 0, m_fTgtBlueBk, m_fSrcBlueMax);

	m_liRed.Initialize(0, m_fSrcRedBk, m_fSrcRedMax, 0, m_fTgtRedBk, m_fSrcRedMax);
	m_liGreen.Initialize(0, m_fSrcGreenBk, m_fSrcGreenMax, 0, m_fTgtGreenBk, m_fSrcGreenMax);
	m_liBlue.Initialize(0, m_fSrcBlueBk, m_fSrcBlueMax, 0, m_fTgtBlueBk, m_fSrcBlueMax);

	if (pProgress)
		pProgress->End2();

	m_bInitOk = true;
};

/* ------------------------------------------------------------------- */
