#ifndef __FRAMELIST_H__
#define __FRAMELIST_H__

#include "DSSProgress.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "DSSTools.h"
#include "MatchingStars.h"

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

class ListBitMap : public CFrameInfo
{
public :
	bool					m_bRemoved;
	uint16_t				m_groupId;
	bool					m_bUseAsStarting;
	CString					m_strType;
	CString					m_strPath;
	CString					m_strFile;
	bool					m_bRegistered;
	Qt::CheckState			m_bChecked;
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
	void	CopyFrom(const ListBitMap & lb)
	{
		CFrameInfo::CopyFrom(lb);

		m_groupId			= lb.m_groupId;
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
	ListBitMap()
	{
		m_groupId			= 0;
		m_bRemoved			= false;
		m_bUseAsStarting	= false;
		m_bRegistered		= false;
		m_bChecked			= Qt::Unchecked;
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

	ListBitMap(const ListBitMap & lb)
	{
		CopyFrom(lb);
	};

	ListBitMap & operator = (const ListBitMap & lb)
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

	inline bool operator ==(const ListBitMap& rhs) const
	{
		return (m_strPath == rhs.m_strPath && m_strFile == rhs.m_strFile);
	}

	inline bool operator !=(const ListBitMap& rhs) const
	{
		return !(*this == rhs);
	}

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

typedef std::vector<ListBitMap>		LISTBITMAPVECTOR;

/* ------------------------------------------------------------------- */

class CFrameList
{
public :
	LISTBITMAPVECTOR	m_vFiles;
	bool				m_bDirty;
	std::uint16_t		m_groupId;

public :
	CFrameList() :
		m_bDirty(false),
		m_groupId(0)
	{
	};

	virtual ~CFrameList()
	{
	};

	std::uint16_t	currentGroupId()
	{
		return m_groupId;
	}

	void	SaveListToFile(LPCTSTR szFile);
	void	LoadFilesFromList(LPCTSTR szFileList);

	bool	AddFile(LPCTSTR szFile, uint16_t groupId = 0, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, bool bCheck = false)
	{
		//AddFileToList(szFile, groupId, PictureType, bCheck);
		return true;
	};

	void	FillTasks(CAllStackingTasks & tasks);
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