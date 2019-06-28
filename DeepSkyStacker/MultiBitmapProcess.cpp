#include <stdafx.h>
#include "BitmapExt.h"
#include "DSSTools.h"
#include "MultiBitmapProcess.h"
#include "StackingTasks.h"
#include "DSSProgress.h"
#include <algorithm>
#include "Multitask.h"

/* ------------------------------------------------------------------- */

static void GetTempFileName(CString & strFile)
{
	TCHAR			szTempFileName[1+_MAX_PATH];
	CString			strFolder;

	CAllStackingTasks::GetTemporaryFilesFolder(strFolder);
	GetTempFileName(strFolder, _T("DSS"), 0, szTempFileName);

	strFile = szTempFileName;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CMultiBitmap::SetBitmapModel(CMemoryBitmap * pBitmap)
{
	m_pBitmapModel.Attach(pBitmap->Clone(TRUE));
};

/* ------------------------------------------------------------------- */

void CMultiBitmap::DestroyTempFiles()
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (m_vFiles[i].m_strFile.GetLength())
			DeleteFile(m_vFiles[i].m_strFile);
		m_vFiles[i].m_strFile.Empty();
	};
	m_vFiles.clear();
};

/* ------------------------------------------------------------------- */

void CMultiBitmap::InitParts()
{
	ZFUNCTRACE_RUNTIME();
	LONG				lNrLinesPerFile;
	LONG				lNrLines;
	LONG				lNrParts;
	LONG				lLineSize;
	LONG				lNrRemainingLines;
	LONG				lNrOffsetLine = 0;

	// make files a maximum of 50 Mb

	lLineSize = (GetNrBytesPerChannel() * GetNrChannels() * m_lWidth);

	lNrLinesPerFile = 50000000L / lLineSize;
	lNrLines = lNrLinesPerFile / m_lNrBitmaps;
	lNrRemainingLines = lNrLinesPerFile % m_lNrBitmaps;

	if (!lNrLines)
		lNrLines = 1;

	lNrParts = m_lNrBitmaps * m_lHeight/lNrLinesPerFile;
	if ((m_lNrBitmaps * m_lHeight) % lNrLinesPerFile != 0)
		lNrParts++;

	m_vFiles.clear();

	LONG			lStartRow = -1;
	LONG			lEndRow	  = -1;

	while (lEndRow <m_lHeight-1)
	{
		CString			strFile;

		GetTempFileName(strFile);

		lStartRow = lEndRow+1;
		lEndRow   = lStartRow + lNrLines;
		if (lNrRemainingLines)
		{
			lEndRow++;
			lNrRemainingLines--;
		};
		lEndRow = min(lEndRow, m_lHeight-1);

		CBitmapPartFile		bp(strFile, lStartRow, lEndRow);

		m_vFiles.push_back(bp);
	};

	m_bInitDone = TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CMultiBitmap::AddBitmap(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	// Save the bitmap to the temporary file
	if (!m_bInitDone)
	{
		m_lWidth = pBitmap->RealWidth();
		m_lHeight = pBitmap->RealHeight();
		InitParts();
		m_lNrAddedBitmaps = 0;
	};

	{
		// Save the bitmap to the file
		void *				pScanLine = NULL;
		LONG				lScanLineSize;

		lScanLineSize = (pBitmap->BitPerSample() * (pBitmap->IsMonochrome() ? 1 : 3) * m_lWidth/8);

		pScanLine = (void*)malloc(lScanLineSize);

		if (pScanLine)
			bResult = TRUE;
		if (pProgress)
			pProgress->Start2(NULL, m_lHeight);

		for (LONG k = 0;k<m_vFiles.size() && bResult;k++)
		{
			FILE *				hFile;

			hFile = _tfopen(m_vFiles[k].m_strFile, _T("a+b"));
			if (hFile)
			{
				bResult = TRUE;
				fseek(hFile, 0, SEEK_END);
			};
			for (LONG j = m_vFiles[k].m_lStartRow;j<=m_vFiles[k].m_lEndRow && bResult;j++)
			{
				pBitmap->GetScanLine(j, pScanLine);
				bResult = (fwrite(pScanLine, lScanLineSize, 1, hFile) == 1);

				if (pProgress)
					pProgress->Progress2(NULL, j+1);
			};
			if (hFile)
				fclose(hFile);
		};

		if (pProgress)
			pProgress->End2();
		if (pScanLine)
			free(pScanLine);
		m_lNrAddedBitmaps++;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CCombineTask : public CMultitask
{
private :
	LONG						m_lStartRow;
	LONG						m_lEndRow;
	LONG						m_lScanLineSize;
	CDSSProgress *				m_pProgress;
	CMultiBitmap *				m_pMultiBitmap;
	void *						m_pBuffer;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<CMemoryBitmap>	m_pHomBitmap;

public :
	CCombineTask()
	{
	};

	virtual ~CCombineTask()
	{
	};

	void	Init(LONG lStartRow, LONG lEndRow, LONG lScanLineSize, void * pBuffer, CDSSProgress * pProgress, CMultiBitmap * pMultiBitmap, CMemoryBitmap * pBitmap, CMemoryBitmap * pHomBitmap = NULL)
	{
		m_lStartRow		= lStartRow;
		m_lEndRow		= lEndRow;
		m_lScanLineSize	= lScanLineSize;
		m_pProgress		= pProgress;
		m_pMultiBitmap	= pMultiBitmap;
		m_pBuffer		= pBuffer;
		m_pBitmap		= pBitmap;
		m_pHomBitmap	= pHomBitmap;
	};

	virtual BOOL	DoTask(HANDLE hEvent);
	virtual BOOL	Process();
};

/* ------------------------------------------------------------------- */

BOOL	CCombineTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	LONG				i;
	BOOL				bEnd = FALSE;
	MSG					msg;
	LONG				lNrBitmaps = m_pMultiBitmap->GetNrAddedBitmaps();
	std::vector<void *>	vScanLines;

	vScanLines.reserve(lNrBitmaps);
	// Create a message queue and signal the event
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			for (i = msg.wParam;i<msg.wParam+msg.lParam;i++)
			{
				void *				pScanLine;

				vScanLines.resize(0);

				for (LONG k = 0;k<lNrBitmaps;k++)
				{
					LONG			lOffset;

					lOffset = k * (m_lEndRow - m_lStartRow+1) * m_lScanLineSize
							  + (i - m_lStartRow) * m_lScanLineSize;
					pScanLine = (void*)(((BYTE*)m_pBuffer)+lOffset);

					vScanLines.push_back(pScanLine);
				};

				m_pMultiBitmap->SetScanLines(m_pBitmap, i, vScanLines);
			};

			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CCombineTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;
	LONG			i = m_lStartRow;
	LONG			lStep;
	LONG			lRemaining;

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	lStep		= max(1, (m_lEndRow-m_lStartRow+1)/50);
	lRemaining	= m_lEndRow-m_lStartRow+1;

	bResult = TRUE;
	while (i<=m_lEndRow)
	{
		DWORD			dwThreadId;
		LONG			lAdd = min(lStep, lRemaining);
		
		dwThreadId = GetAvailableThreadId();

		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+= lAdd;
		lRemaining	-= lAdd;
		
		if (m_pProgress)
			m_pProgress->Progress2(NULL, i);
	};

	CloseAllThreads();

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors();

	return bResult;
};

/* ------------------------------------------------------------------- */

static	void ComputeWeightedAverage(LONG x, LONG y, CMemoryBitmap * pBitmap, CMemoryBitmap * pHomBitmap, CMemoryBitmap * pOutBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bColor = pBitmap->IsMonochrome();
	LONG			lWidth = pBitmap->Width();
	LONG			lHeight = pBitmap->Height();

	if (bColor)
	{
		double		fRed = 0, fGreen = 0, fBlue = 0;
		double		fWRed = 0, fWGreen = 0, fWBlue = 0;

		for (LONG i = max(0, x-5);i<=min(lWidth-1, x+5);i++)
		{
			for (LONG j = max(0, y-5);j<=min(lHeight-1, y+5);j++)
			{
				double		fRed1, fGreen1, fBlue1;
				double		fWRed1, fWGreen1, fWBlue1;

				pBitmap->GetPixel(i, j, fRed1, fGreen1, fBlue1);
				pHomBitmap->GetPixel(i, j, fWRed1, fWGreen1, fWBlue1);

				fRed	+= fRed1/(1.0+fWRed1);
				fGreen	+= fGreen1/(1.0+fWGreen1);
				fBlue	+= fBlue1/(1.0+fWBlue1);

				fWRed   += 1.0/(1.0+fWRed1);
				fWGreen += 1.0/(1.0+fWGreen1);
				fWBlue  += 1.0/(1.0+fWBlue1);
			};
		};

		fRed   /= fWRed;
		fGreen /= fWGreen;
		fBlue  /= fWBlue;

		pOutBitmap->SetPixel(x, y, fRed, fGreen, fBlue);
	}
	else
	{
		double		fGray = 0;
		double		fWGray = 0;

		for (LONG i = max(0, x-5);i<=min(lWidth-1, x+5);i++)
		{
			for (LONG j = max(0, y-5);j<=min(lHeight-1, y+5);j++)
			{
				double		fGray1;
				double		fWGray1;

				pBitmap->GetPixel(i, j, fGray1);
				pHomBitmap->GetPixel(i, j, fWGray1);

				fGray	+= fGray1/(1.0+fWGray1);

				fWGray   += 1.0/(1.0+fWGray1);
			};
		};

		fGray   /= fWGray;

		pOutBitmap->SetPixel(x, y, fGray);
	};
};

/* ------------------------------------------------------------------- */

void	CMultiBitmap::SmoothOut(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppOutBitmap)
{
	if (m_pHomBitmap)
	{
		CSmartPtr<CMemoryBitmap>	pOutBitmap;

		pOutBitmap.Attach(pBitmap->Clone());

		for (LONG i = 0;i<m_lWidth;i++)
		{
			for (LONG j = 0;j<m_lHeight;j++)
			{
				// Compute the weighted average of a 11x11 area around each pixel
				// It can be lengthy!
				ComputeWeightedAverage(i, j, pBitmap, m_pHomBitmap, pOutBitmap);
			};
		};

		pOutBitmap.CopyTo(ppOutBitmap);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL CMultiBitmap::GetResult(CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
	LONG						lScanLineSize;
	LONG						/*i, k, */l;
	CSmartPtr<CMemoryBitmap>	pBitmap;
	LONG						lBufferSize = 0;
	void *						pBuffer = NULL;

	if (m_bInitDone && m_vFiles.size())
	{
		*ppBitmap = NULL;
		bResult = FALSE;

		CreateOutputMemoryBitmap(&pBitmap);
		if (pBitmap)
			bResult = pBitmap->Init(m_lWidth, m_lHeight);

		if (m_bHomogenization)
		{
			// Create the bitmap that will contain the standard deviation/average for each pixel
			// to remove remaining star trails from the comet only image
			if (pBitmap->IsMonochrome())
				m_pHomBitmap.Attach(new C32BitFloatGrayBitmap);
			else
				m_pHomBitmap.Attach(new C96BitFloatColorBitmap);
			if (m_pHomBitmap)
				bResult = m_pHomBitmap->Init(m_lWidth, m_lHeight);
		};

		if (pProgress && bResult)
			pProgress->Start2(NULL, m_lHeight);

		lScanLineSize = (GetNrBytesPerChannel() * GetNrChannels() * m_lWidth);

		//lScanLineSize = m_lWidth * GetNrChannels() * GetNrBytesPerChannel();
		for (l = 0;l<m_vFiles.size() && bResult;l++)
		{
			// Read the full bitmap in memory
			LONG					lFileSize;
			FILE *					hFile;

			lFileSize = lScanLineSize * m_lNrAddedBitmaps*
						(m_vFiles[l].m_lEndRow - m_vFiles[l].m_lStartRow+1);

			if (lFileSize > lBufferSize)
			{
				if (pBuffer)
					free(pBuffer);
				pBuffer = (void *)malloc(lFileSize);
				lBufferSize = lFileSize;
			};
			hFile = _tfopen(m_vFiles[l].m_strFile, _T("rb"));
			if (hFile)
			{
				bResult = (fread(pBuffer, 1, lFileSize, hFile) == lFileSize);
				fclose(hFile);
			}
			else
				bResult = FALSE;

			{
				CCombineTask		CombineTask;

				CombineTask.Init(m_vFiles[l].m_lStartRow, m_vFiles[l].m_lEndRow, lScanLineSize, 
								 pBuffer, pProgress, this, pBitmap);
				CombineTask.StartThreads();
				CombineTask.Process();
			};

			if (pProgress)
				pProgress->End2();
		};

		if (pBuffer)
		{
			free(pBuffer);
			pBuffer = NULL;
		};

		if (bResult)
		{
			if (m_pHomBitmap)
			{
				// At this point the m_pHomBitmap might be used to smooth out any remaining 
				// star trails with a large filter
				SmoothOut(pBitmap, ppBitmap);
			}
			else
				pBitmap.CopyTo(ppBitmap);
		};
	};
	if (pBuffer)
	{
		free(pBuffer);
		pBuffer = NULL;
	};

	DestroyTempFiles();

	return bResult;
};

/* ------------------------------------------------------------------- */
