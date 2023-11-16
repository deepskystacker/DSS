#pragma once
#include "cfa.h"
#include "DSSCommon.h"
#include "BitmapExtraInfo.h"
#include "SkyBackground.h"
#include "BilinearParameters.h"
#include "MatchingStars.h"

// From FITSUtils.h/.cpp
CFATYPE GetFITSCFATYPE();

bool IsSuperPixels();
bool IsRawBayer();
bool IsFITSRawBayer();
bool IsFITSSuperPixels();

class CFrameInfo
{
public :
	fs::path filePath; 
	int				m_lWidth,
						m_lHeight;
	int				m_lISOSpeed;
	int				m_lGain;
	double				m_fExposure;
	double				m_fAperture;
	PICTURETYPE			m_PictureType;
	int				m_lBitsPerChannel;
	int				m_lNrChannels;
	QString				m_strDateTime;
	QDateTime			m_DateTime;
	bool				m_bMaster;
	QString				m_strInfos;
	bool				m_bFITS16bit;
	CBitmapExtraInfo	m_ExtraInfo;
	QString				m_filterName;
    mutable	QString	incompatibilityReason;


private :
	mutable CFATYPE			m_CFAType;
	mutable bool			m_bSuperPixel;

protected :
	void CopyFrom(const CFrameInfo& cfi);
	void Reset();

public :
	CFrameInfo()
	{
		Reset();
	};
	CFrameInfo(const CFrameInfo & cbi)
	{
		CopyFrom(cbi);
	};

	CFrameInfo & operator = (const CFrameInfo & cbi)
	{
		CopyFrom(cbi);
		return (*this);
	};

	bool	IsLightFrame() const
	{
		return (m_PictureType == PICTURETYPE_LIGHTFRAME);
	};

	bool	IsDarkFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFRAME);
	};

	bool	IsDarkFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFLATFRAME);
	};

	bool	IsFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_FLATFRAME);
	};

	bool	IsOffsetFrame() const
	{
		return (m_PictureType == PICTURETYPE_OFFSETFRAME);
	};

	bool	IsMasterFrame() const
	{
		return m_bMaster;
	};

	int	RenderedWidth()
	{
		return m_lWidth/(m_bSuperPixel ? 2 : 1);
	};

	int	RenderedHeight()
	{
		return m_lHeight/(m_bSuperPixel ? 2 : 1);
	};

	bool IsCompatible(const CFrameInfo& cfi) const;

	bool	InitFromFile(const fs::path& file, PICTURETYPE Type);

	CFATYPE	GetCFAType() const;
	bool	IsSuperPixel() const;
	void	RefreshSuperPixel();
};

using FRAMEINFOVECTOR = std::vector<CFrameInfo>;


class ListBitMap : public CFrameInfo
{
public:
	uint16_t				m_groupId;
	bool					m_bUseAsStarting;
	QString					m_strType;
	QString					m_strPath;
	QString					m_strFile;
	bool					m_bRegistered;
	Qt::CheckState			m_bChecked;
	double					m_fOverallQuality;
	double					m_fFWHM;
	double					m_dX;
	double					m_dY;
	double					m_fAngle;
	CSkyBackground			m_SkyBackground;
	bool					m_bDeltaComputed;
	QString					m_strCFA;
	QString					m_strSizes;
	QString					m_strDepth;
	bool					m_bCompatible;
	CBilinearParameters		m_Transformation;
	VOTINGPAIRVECTOR		m_vVotedPairs;
	int					m_lNrStars;
	bool					m_bComet;


protected:
	void	CopyFrom(const ListBitMap& lb)
	{
		CFrameInfo::CopyFrom(lb);

		m_groupId = lb.m_groupId;
		m_bUseAsStarting = lb.m_bUseAsStarting;
		m_strType = lb.m_strType;
		m_strPath = lb.m_strPath;
		m_strFile = lb.m_strFile;
		m_bRegistered = lb.m_bRegistered;
		m_bChecked = lb.m_bChecked;
		m_fOverallQuality = lb.m_fOverallQuality;
		m_fFWHM = lb.m_fFWHM;
		m_dX = lb.m_dX;
		m_dY = lb.m_dY;
		m_fAngle = lb.m_fAngle;
		m_bDeltaComputed = lb.m_bDeltaComputed;
		m_strCFA = lb.m_strCFA;
		m_strSizes = lb.m_strSizes;
		m_strDepth = lb.m_strDepth;
		m_bCompatible = lb.m_bCompatible;
		m_Transformation = lb.m_Transformation;
		m_vVotedPairs = lb.m_vVotedPairs;
		m_lNrStars = lb.m_lNrStars;
		m_bComet = lb.m_bComet;
		m_SkyBackground = lb.m_SkyBackground;
	};

public:
	ListBitMap()
	{
		m_groupId = 0;
		m_bUseAsStarting = false;
		m_bRegistered = false;
		m_bChecked = Qt::Unchecked;
		m_fOverallQuality = 0;
		m_fFWHM = 0;
		m_dX = 0;
		m_dY = 0;
		m_fAngle = 0;
		m_bDeltaComputed = false;
		m_bCompatible = true;
		m_lNrStars = 0;
		m_bComet = 0;
	};

	ListBitMap(const ListBitMap& lb)
	{
		CopyFrom(lb);
	};

	ListBitMap& operator = (const ListBitMap& lb)
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
		fs::path path{ filePath };
		fs::remove(path);
		if (IsLightFrame())
		{
			path.replace_extension("Info.txt");
			fs::remove(path);
		};
	};
};

typedef std::vector<ListBitMap>		LISTBITMAPVECTOR;

/* ------------------------------------------------------------------- */
