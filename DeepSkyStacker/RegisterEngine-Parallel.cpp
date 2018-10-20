
#include <stdafx.h>
#include <queue>
#include <memory>
#include "RegisterEngine.h"
#include "Registry.h"
#include "MasterFrames.h"
#include "BackgroundCalibration.h"
#include "PixelTransform.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "Filters.h"
#include "TaskPool.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <map>


struct LoadFrameToRegisterData
{
	struct
	{
		BOOL bForce;
		CDSSProgress * pProgress;
		CStackingInfo *pStackingInfo;
		CMasterFrames *pMasterFrames;
		LONG	lNrRegistered;
		const CFrameInfo* pFrameInfo;
		bool saveCalibrated;
		const CRegisterEngine* registerEngine;
	} input;

	struct
	{
		BOOL loaded;
	} RegisterOneFrameLoadPartResult;

	struct
	{
		std::unique_ptr<CLightFrameInfo>	pLfi;
		std::unique_ptr<CMemoryBitmap>		pBitmap;
	} temp;
};


void CRegisterEngine::RegisterOneFrameLoadPart(LoadFrameToRegisterData&data)
{
	auto bForce = data.input.bForce;
	auto pProgress = data.input.pProgress;
	auto pStackingInfo = data.input.pStackingInfo;
	CMasterFrames& MasterFrames = *(data.input.pMasterFrames);
	auto lNrRegistered = data.input.lNrRegistered;
	const CFrameInfo& frameInfo = *(data.input.pFrameInfo);

	data.temp.pLfi = std::make_unique< CLightFrameInfo>();

	CLightFrameInfo	&	lfi = *(data.temp.pLfi);

	CString					strText;
	LONG					lTotalRegistered = 0;
	BOOL					loaded  = FALSE;

	ZTRACE_RUNTIME("Register %s", (LPCTSTR)frameInfo.m_strFileName);
	lfi.SetProgress(pProgress);
	lfi.SetBitmap(frameInfo.m_strFileName, FALSE, FALSE);

	if (pProgress)
	{
		strText.Format(IDS_REGISTERINGPICTURE, lNrRegistered, lTotalRegistered);
		pProgress->Progress1(strText, lNrRegistered);
	};

	if (bForce || !lfi.IsRegistered())
	{
		// Load the bitmap
		CBitmapInfo		bmpInfo;
		if (GetPictureInfo(lfi.m_strFileName, bmpInfo) && bmpInfo.CanLoad())
		{
			CString						strText;
			CString						strDescription;

			bmpInfo.GetDescription(strDescription);

			if (bmpInfo.m_lNrChannels == 3)
				strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
			else
				strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
			if (pProgress)
				pProgress->Start2(strText, 0);

			CMemoryBitmap* pBitmap = nullptr;
			loaded = ::LoadPicture(lfi.m_strFileName, &pBitmap, pProgress);
			data.temp.pBitmap.reset(pBitmap);
		}
	}
	data.RegisterOneFrameLoadPartResult.loaded = loaded;
}

void CRegisterEngine::RegisterOneFrameProcessPart(LoadFrameToRegisterData&data)
{
	auto pProgress = data.input.pProgress;
	auto pStackingInfo = data.input.pStackingInfo;
	CMasterFrames& MasterFrames = *(data.input.pMasterFrames);

	CLightFrameInfo	&	lfi = *(data.temp.pLfi);
	auto pBitmap = data.temp.pBitmap.get();
	bool bSaveCalibrated = data.input.saveCalibrated;

	// Apply offset, dark and flat to lightframe
	MasterFrames.ApplyAllMasters(pBitmap, NULL, pProgress);

	CString				strCalibratedFile;

	if (bSaveCalibrated &&
		(pStackingInfo->m_pDarkTask || pStackingInfo->m_pDarkFlatTask ||
			pStackingInfo->m_pFlatTask || pStackingInfo->m_pOffsetTask))
		data.input.registerEngine->SaveCalibratedLightFrame(lfi, pBitmap, pProgress, strCalibratedFile);

	// Then register the light frame
	lfi.SetProgress(pProgress);
	lfi.RegisterPicture(pBitmap);
	lfi.SaveRegisteringInfo();

	if (strCalibratedFile.GetLength())
	{
		CString				strInfoFileName;
		TCHAR				szDrive[1 + _MAX_DRIVE];
		TCHAR				szDir[1 + _MAX_DIR];
		TCHAR				szFile[1 + _MAX_FNAME];

		_tsplitpath(strCalibratedFile, szDrive, szDir, szFile, NULL);
		strInfoFileName.Format(_T("%s%s%s%s"), szDrive, szDir, szFile, _T(".Info.txt"));
		lfi.CRegisteredFrame::SaveRegisteringInfo(strInfoFileName);
	};
}


void LoadTaskFunction(LoadFrameToRegisterData& data)
{
	CRegisterEngine::RegisterOneFrameLoadPart(data);
}

void RegisterTaskFunction(LoadFrameToRegisterData& data)
{
	CRegisterEngine::RegisterOneFrameProcessPart(data);
}

void LoadTaskFunction(void * data)
{
	LoadTaskFunction(*reinterpret_cast<LoadFrameToRegisterData*>(data));
}

void RegisterTaskFunction(void * data)
{
	RegisterTaskFunction(*reinterpret_cast<LoadFrameToRegisterData*>(data));
}

class ParallelRegistration
{
	size_t m_maxLoads;
	size_t m_maxRegisters;
	const CRegisterEngine& m_registerEngine;
	std::vector<std::unique_ptr<LoadFrameToRegisterData>> registrations;

public:
	ParallelRegistration(size_t maxLoads, size_t maxRegister, const CRegisterEngine& registerEngine):
		m_maxLoads(maxLoads)
		,m_maxRegisters(maxRegister)
		,m_registerEngine(registerEngine)
	{

	}

	void Enqueue(BOOL bForce, CDSSProgress * pProgress, CStackingInfo * pStackingInfo, CMasterFrames&	MasterFrames, LONG	lNrRegistered, const CFrameInfo& frameInfo)
	{
		auto pparams = std::make_unique<LoadFrameToRegisterData>();

		pparams->input.bForce = bForce;
		pparams->input.pProgress = pProgress;
		pparams->input.pStackingInfo = pStackingInfo;
		pparams->input.pMasterFrames = &MasterFrames;
		pparams->input.lNrRegistered = lNrRegistered;
		pparams->input.pFrameInfo = &frameInfo;
		pparams->input.saveCalibrated = false;
		pparams->input.registerEngine = &m_registerEngine;

		registrations.push_back( std::move(pparams));
	}



	void Process()
	{
		std::queue<size_t> loadQueue;
		std::queue<size_t> registerQueue;

		std::map<TaskPool::TaskId, size_t> loadTasks;
		std::map<TaskPool::TaskId, size_t> registerTasks;

		for (size_t index = 0; index!= registrations.size(); ++index)
			loadQueue.emplace(index);

		while (loadQueue.size() || loadTasks.size() || registerQueue.size() || registerTasks.size())
		{
			if (loadQueue.size() && registerQueue.size() < m_maxLoads)
			{
				auto index = loadQueue.front();
				loadQueue.pop();
				loadTasks[TaskPool::Async( LoadTaskFunction, registrations[index].get())] = index;
			}

			if (registerQueue.size() && registerTasks.size() < m_maxRegisters)
			{
				auto index = registerQueue.front();
				registerQueue.pop();
				registerTasks[TaskPool::Async(RegisterTaskFunction, registrations[index].get())] = index;
			}

			std::set<TaskPool::TaskId> tasksToWait;
			for (auto task : loadTasks)
				tasksToWait.insert(task.first);
			for (auto task : registerTasks)
				tasksToWait.insert(task.first);

			TaskPool::TaskId task = TaskPool::Wait(tasksToWait);
			if (loadTasks.find(task) != loadTasks.end())
			{
				registerQueue.push(loadTasks[task]);
				loadTasks.erase(task);
			}
			if (registerTasks.find(task) != registerTasks.end())
			{
				auto index = registerTasks[task];
				registrations[index].reset(nullptr);
				registerTasks.erase(task);
			}
		}
	}
};



BOOL CRegisterEngine::RegisterLightFramesParallel(CAllStackingTasks & tasks, BOOL bForce, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = TRUE;
	LONG					i, j;
	CString					strText;
	LONG					lTotalRegistered = 0;
	LONG					lNrRegistered = 0;

	for (i = 0; i < tasks.m_vStacks.size(); i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);
		if (pStackingInfo)
			lTotalRegistered += (LONG)pStackingInfo->m_pLightTask->m_vBitmaps.size();
	};

	strText.LoadString(IDS_REGISTERING);
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	bResult = tasks.DoAllPreTasks(pProgress);

	// Do it again in case pretasks change the progress
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	for (i = 0; i < tasks.m_vStacks.size() && bResult; i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);

		if (pStackingInfo)
		{
			CMasterFrames				MasterFrames;

			MasterFrames.LoadMasters(pStackingInfo, pProgress);

			size_t maxStagedLoads = 2;
			size_t maxRegistrationsInFlight = CMultitask::GetNrProcessors();
			
			ParallelRegistration parallelRegistration(maxStagedLoads, maxRegistrationsInFlight, *this);

			for (j = 0; j < pStackingInfo->m_pLightTask->m_vBitmaps.size() && bResult; j++)
			{
				parallelRegistration.Enqueue(bForce, pProgress, pStackingInfo, MasterFrames, lNrRegistered, pStackingInfo->m_pLightTask->m_vBitmaps[j]);
			}
			parallelRegistration.Process();
		}
	}

	// Clear stuff
	tasks.ClearCache();

	return bResult;
};
/* ------------------------------------------------------------------- */
BOOL CRegisterEngine::RegisterOneFrame(BOOL bForce, CDSSProgress * pProgress, CStackingInfo * pStackingInfo, CMasterFrames&	MasterFrames, LONG	lNrRegistered, const CFrameInfo& frameInfo) const
{
	CString					strText;
	LONG					lTotalRegistered = 0;
	BOOL					bResult = TRUE;

	// Register this bitmap
	CLightFrameInfo		lfi;

	ZTRACE_RUNTIME("Register %s", (LPCTSTR)frameInfo.m_strFileName);

	lfi.SetProgress(pProgress);
	lfi.SetBitmap(frameInfo.m_strFileName, FALSE, FALSE);


	if (pProgress)
	{
		strText.Format(IDS_REGISTERINGPICTURE, lNrRegistered, lTotalRegistered);
		pProgress->Progress1(strText, lNrRegistered);
	};

	if (bForce || !lfi.IsRegistered())
	{
		CBitmapInfo		bmpInfo;
		// Load the bitmap
		if (GetPictureInfo(lfi.m_strFileName, bmpInfo) && bmpInfo.CanLoad())
		{
			CSmartPtr<CMemoryBitmap>	pBitmap;
			CString						strText;
			CString						strDescription;

			bmpInfo.GetDescription(strDescription);

			if (bmpInfo.m_lNrChannels == 3)
				strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
			else
				strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
			if (pProgress)
				pProgress->Start2(strText, 0);

			if (::LoadPicture(lfi.m_strFileName, &pBitmap, pProgress))
			{
				// Apply offset, dark and flat to lightframe
				MasterFrames.ApplyAllMasters(pBitmap, NULL, pProgress);

				CString				strCalibratedFile;

				if (m_bSaveCalibrated &&
					(pStackingInfo->m_pDarkTask || pStackingInfo->m_pDarkFlatTask ||
						pStackingInfo->m_pFlatTask || pStackingInfo->m_pOffsetTask))
					SaveCalibratedLightFrame(lfi, pBitmap, pProgress, strCalibratedFile);

				// Then register the light frame
				lfi.SetProgress(pProgress);
				lfi.RegisterPicture(pBitmap);
				lfi.SaveRegisteringInfo();

				if (strCalibratedFile.GetLength())
				{
					CString				strInfoFileName;
					TCHAR				szDrive[1 + _MAX_DRIVE];
					TCHAR				szDir[1 + _MAX_DIR];
					TCHAR				szFile[1 + _MAX_FNAME];

					_tsplitpath(strCalibratedFile, szDrive, szDir, szFile, NULL);
					strInfoFileName.Format(_T("%s%s%s%s"), szDrive, szDir, szFile, _T(".Info.txt"));
					lfi.CRegisteredFrame::SaveRegisteringInfo(strInfoFileName);
				};
			};

			if (pProgress)
			{
				pProgress->End2();
				bResult = !pProgress->IsCanceled();
			};
		};
	};
	return bResult;
}
BOOL CRegisterEngine::RegisterLightFrames2(CAllStackingTasks & tasks, BOOL bForce, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = TRUE;
	LONG					i, j;
	CString					strText;
	LONG					lTotalRegistered = 0;
	LONG					lNrRegistered = 0;

	for (i = 0; i < tasks.m_vStacks.size(); i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);
		if (pStackingInfo)
			lTotalRegistered += (LONG)pStackingInfo->m_pLightTask->m_vBitmaps.size();
	};

	strText.LoadString(IDS_REGISTERING);
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	bResult = tasks.DoAllPreTasks(pProgress);

	// Do it again in case pretasks change the progress
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	for (i = 0; i < tasks.m_vStacks.size() && bResult; i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);

		if (pStackingInfo)
		{
			CMasterFrames				MasterFrames;

			MasterFrames.LoadMasters(pStackingInfo, pProgress);

			for (j = 0; j < pStackingInfo->m_pLightTask->m_vBitmaps.size() && bResult; j++)
			{
				lNrRegistered++;
				RegisterOneFrame(bForce, pProgress, pStackingInfo, MasterFrames, lNrRegistered, pStackingInfo->m_pLightTask->m_vBitmaps[j]);
			};
		};
	};

	// Clear stuff
	tasks.ClearCache();

	return bResult;
};