#ifndef __FRAMELIST_H__
#define __FRAMELIST_H__

#include "DSSProgress.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "DSSTools.h"
#include "MatchingStars.h"

// {89AEE9BB-89E3-47ef-BEB5-A0819D957C77}
const GUID MAINJOBID =
{ 0x89aee9bb, 0x89e3, 0x47ef, { 0xbe, 0xb5, 0xa0, 0x81, 0x9d, 0x95, 0x7c, 0x77 } };

#include "StackingTasks.h"
#include "RegisterEngine.h"

/* ------------------------------------------------------------------- */

class CMRUList
{
public :
	CString					m_strBasePath;
	std::vector<CString>	m_vLists;
	int					m_lMaxLists;

private :
	void	CopyFrom(const CMRUList & ml)
	{
		m_vLists		= ml.m_vLists;
		m_lMaxLists		= ml.m_lMaxLists;
	};

public :
	CMRUList(LPCTSTR szBasePath = nullptr)
	{
		if (szBasePath)
			m_strBasePath = szBasePath;
		else
			m_strBasePath = _T("FileLists");
		m_lMaxLists = 10;
	};

	void	SetBasePath(LPCTSTR szBasePath)
	{
		m_strBasePath = szBasePath;
	};

	CMRUList(const CMRUList & ml)
	{
		CopyFrom(ml);
	};

	~CMRUList() {};

	CMRUList & operator = (const CMRUList & ml)
	{
		CopyFrom(ml);
		return (*this);
	};

	void	readSettings();
	void	saveSettings();

	void	Add(LPCTSTR szList);
};

/* ------------------------------------------------------------------- */

class CListBitmap : public CFrameInfo
{
public :
	bool					m_bRemoved;
	DWORD					m_dwGroupID;
	GUID					m_JobID;
	bool					m_bUseAsStarting;
	CString					m_strType;
	CString					m_strPath;
	CString					m_strFile;
	bool					m_bRegistered;
	bool					m_bChecked;
	double					m_fOverallQuality;
	double					m_fFWHM;
	double					m_dX;
	double					m_dY;
	double					m_fAngle;
	CSkyBackground			m_SkyBackground;
	bool					m_bDeltaComputed;
	CString					m_strCFA;
	CString					m_strSizes;
	CString					m_strDepth;
	bool					m_bCompatible;
	CBilinearParameters		m_Transformation;
	VOTINGPAIRVECTOR		m_vVotedPairs;
	int					m_lNrStars;
	bool					m_bComet;


protected :
	void	CopyFrom(const CListBitmap & lb)
	{
		CFrameInfo::CopyFrom(lb);

		m_dwGroupID			= lb.m_dwGroupID;
		m_JobID				= lb.m_JobID;
		m_strFile			= lb.m_strFile;
		m_bRemoved			= lb.m_bRemoved;
		m_bUseAsStarting	= lb.m_bUseAsStarting;
		m_strType			= lb.m_strType;
		m_strPath			= lb.m_strPath;
		m_strFile			= lb.m_strFile;
		m_bRegistered		= lb.m_bRegistered;
		m_bChecked			= lb.m_bChecked;
		m_fOverallQuality	= lb.m_fOverallQuality;
		m_fFWHM				= lb.m_fFWHM;
		m_dX				= lb.m_dX;
		m_dY				= lb.m_dY;
		m_fAngle			= lb.m_fAngle;
		m_bDeltaComputed	= lb.m_bDeltaComputed;
		m_strCFA			= lb.m_strCFA;
		m_strSizes			= lb.m_strSizes;
		m_strDepth			= lb.m_strDepth;
		m_bCompatible		= lb.m_bCompatible;
		m_Transformation	= lb.m_Transformation;
		m_vVotedPairs		= lb.m_vVotedPairs;
		m_lNrStars			= lb.m_lNrStars;
		m_bComet			= lb.m_bComet;
		m_SkyBackground		= lb.m_SkyBackground;
	};

public :
	CListBitmap()
	{
		m_dwGroupID			= 0;
		m_JobID				= MAINJOBID;
		m_bRemoved			= false;
		m_bUseAsStarting	= false;
		m_bRegistered		= false;
		m_bChecked			= false;
		m_fOverallQuality	= 0;
		m_fFWHM				= 0;
		m_dX				= 0;
		m_dY				= 0;
		m_fAngle			= 0;
		m_bDeltaComputed	= false;
		m_bCompatible		= true;
		m_lNrStars			= 0;
		m_bComet			= 0;
	};

	CListBitmap(const CListBitmap & lb)
	{
		CopyFrom(lb);
	};

	CListBitmap & operator = (const CListBitmap & lb)
	{
		CopyFrom(lb);
		return (*this);
	};

	bool	IsUseAsStarting()
	{
		return m_bUseAsStarting;
	};

	bool	IsDeltaComputed()
	{
		return m_bDeltaComputed;
	};

	void	EraseFile()
	{
		DeleteFile(m_strFileName);
		if (IsLightFrame())
		{
			// Remove the Info.txt file if it exists
			TCHAR				szDrive[1+_MAX_DRIVE];
			TCHAR				szDir[1+_MAX_DIR];
			TCHAR				szFile[1+_MAX_FNAME];
			TCHAR				szExt[1+_MAX_EXT];
			TCHAR				szInfoName[1+_MAX_PATH];

			_tsplitpath(m_strFileName, szDrive, szDir, szFile, szExt);
			_tmakepath(szInfoName, szDrive, szDir, szFile, _T(".Info.txt"));

			DeleteFile(szInfoName);
		};
	};
};

typedef std::vector<CListBitmap>		LISTBITMAPVECTOR;

/* ------------------------------------------------------------------- */

class CJob
{
public :
	CString					m_strName;
	GUID					m_ID;
	GUID					m_RefID;

private :
	void	CopyFrom(const CJob & j)
	{
		m_strName	= j.m_strName;
		m_ID		= j.m_ID;
		m_RefID		= j.m_RefID;
	};

public :
	CJob()
	{
		m_strName.Empty();
		m_ID	= GUID_NULL;
		m_RefID = GUID_NULL;
	};

	~CJob() {};

	CJob(const CJob & j)
	{
		CopyFrom(j);
	};

	CJob & operator = (const CJob & j)
	{
		CopyFrom(j);
		return (*this);
	};

	bool	IsNullJob()
	{
		return (m_ID == GUID_NULL);
	};
};

/* ------------------------------------------------------------------- */

class CJobList
{
public :
	std::vector<CJob>		m_vJobs;
	CJob					m_NullJob;

private :
	void		CopyFrom(const CJobList & jl)
	{
		m_vJobs = jl.m_vJobs;
	};

public :
	CJobList() {};
	~CJobList() {};

	CJobList(const CJobList & jl)
	{
		CopyFrom(jl);
	};

	CJobList & operator = (const CJobList & jl)
	{
		CopyFrom(jl);
		return (*this);
	};

	CJob &	AddJob(LPCTSTR szName, GUID dwID = GUID_NULL)
	{
		// Check if the ID already exists
		bool				bFound = false;

		if (dwID == GUID_NULL)
			::CoCreateGuid(&dwID);

		for (int i = 0;i<m_vJobs.size();i++)
		{
			if (m_vJobs[i].m_ID == dwID)
				bFound = true;
		};
		if (bFound)
			::CoCreateGuid(&dwID);

		CJob		job;

		job.m_strName = szName;
		job.m_ID	  = dwID;

		m_vJobs.push_back(job);

		return m_vJobs[m_vJobs.size()-1];
	};

	CJob & GetJob(LPCTSTR szName)
	{
		int			lIndice = -1;

		for (int i = 0;i<m_vJobs.size() && lIndice<0;i++)
		{
			if (!m_vJobs[i].m_strName.CompareNoCase(szName))
				lIndice = i;
		};

		return lIndice>=0 ? m_vJobs[lIndice] : m_NullJob;
	};

	CJob & GetJob(GUID const& dwID)
	{
		int			lIndice = -1;

		for (int i = 0;i<m_vJobs.size() && lIndice<0;i++)
		{
			if (m_vJobs[i].m_ID == dwID)
				lIndice = i;
		};

		return lIndice>=0 ? m_vJobs[lIndice] : m_NullJob;
	};

	bool	RemoveJob(GUID const& dwID)
	{
		int			lIndice = -1;
		for (int i = 0;i<m_vJobs.size() && lIndice<0;i++)
		{
			if (m_vJobs[i].m_ID == dwID)
				lIndice = i;
		};

		if (lIndice>=0)
		{
			std::vector<CJob>::iterator it = m_vJobs.begin();
			it+=lIndice;
			m_vJobs.erase(it);
		};

		return lIndice>=0 ? true : false;
	};

	void	AddMainJob()
	{
		AddJob(nullptr, MAINJOBID);
	};
};

/* ------------------------------------------------------------------- */

class CFrameList
{
public :
	LISTBITMAPVECTOR	m_vFiles;
	CJobList			m_Jobs;
	bool				m_bDirty;

public :
	CFrameList()
	{
		m_Jobs.AddMainJob();
		m_bDirty = false;
	};

	virtual ~CFrameList()
	{
	};

	void	SaveListToFile(LPCTSTR szFile);
	void	LoadFilesFromList(LPCTSTR szFileList);

	CJobList & GetJobs()
	{
		return m_Jobs;
	};

	virtual bool	AddFile(LPCTSTR szFile, DWORD dwGroupID = 0, GUID dwJobID = GUID_NULL, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false)
	{
		return false;
	};

	void	FillTasks(CAllStackingTasks & tasks, GUID const& dwJobID = MAINJOBID);
	bool	GetReferenceFrame(CString & strReferenceFrame);
	int	GetNrUnregisteredCheckedLightFrames(int lGroupID = -1);

	bool	IsDirty(bool bReset = false)
	{
		bool			bResult = m_bDirty;

		if (bReset)
			m_bDirty = false;

		return bResult;
	};
};

/* ------------------------------------------------------------------- */

#endif // __FRAMELIST_H__