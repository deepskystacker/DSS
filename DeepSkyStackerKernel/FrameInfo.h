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
public:
	fs::path filePath; 
	int m_lWidth{ 0 };
	int m_lHeight{ 0 };
	int m_lISOSpeed{ 0 };
	int m_lGain{ -1 };
	double m_fExposure{ 0.0 };
	double m_fAperture{ 0.0 };
	PICTURETYPE m_PictureType{ PICTURETYPE_LIGHTFRAME };
	int m_lBitsPerChannel{ 16 };
	int m_lNrChannels{ 3 };
	QString m_strDateTime;
	QDateTime m_DateTime;
	bool m_bMaster{ false };
	QString m_strInfos;
	bool m_bFITS16bit{ false };
	CBitmapExtraInfo m_ExtraInfo{};
	QString m_filterName;
    mutable	QString incompatibilityReason;

private:
	mutable CFATYPE m_CFAType{ CFATYPE_NONE };
	mutable bool m_bSuperPixel{ false };

protected:
//	void Reset();

public:
	CFrameInfo() = default;
	CFrameInfo(const CFrameInfo&) = default;
	CFrameInfo& operator=(const CFrameInfo&) = default;

	bool IsLightFrame() const
	{
		return (m_PictureType == PICTURETYPE_LIGHTFRAME);
	}

	bool IsDarkFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFRAME);
	}

	bool IsDarkFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFLATFRAME);
	}

	bool IsFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_FLATFRAME);
	}

	bool IsOffsetFrame() const
	{
		return (m_PictureType == PICTURETYPE_OFFSETFRAME);
	}

	bool IsMasterFrame() const
	{
		return m_bMaster;
	}

	int RenderedWidth()
	{
		return m_lWidth/(m_bSuperPixel ? 2 : 1);
	}

	int RenderedHeight()
	{
		return m_lHeight/(m_bSuperPixel ? 2 : 1);
	}

	bool IsCompatible(const CFrameInfo& cfi) const;

	bool InitFromFile(const fs::path& file, PICTURETYPE Type);

	CFATYPE GetCFAType() const;
	bool IsSuperPixel() const;
	void RefreshSuperPixel();
};

using FRAMEINFOVECTOR = std::vector<CFrameInfo>;

class ListBitMap : public CFrameInfo
{
public:
	uint16_t				m_groupId{ 0 };
	bool					m_bUseAsStarting{ false };
	QString					m_strType;
	QString					m_strPath;
	QString					m_strFile;
	bool					m_bRegistered{ false };
	Qt::CheckState			m_bChecked{ Qt::Unchecked };
	double					m_fOverallQuality{ 0.0 };
	double					quality{ 0.0 };
	double					m_fFWHM{ 0.0 };
	double					m_dX{ 0.0 };
	double					m_dY{ 0.0 };
	double					m_fAngle{ 0.0 };
	CSkyBackground			m_SkyBackground;
	bool					m_bDeltaComputed{ false };
	QString					m_strCFA;
	QString					m_strSizes;
	QString					m_strDepth;
	bool					m_bCompatible{ true };
	CBilinearParameters		m_Transformation;
	VOTINGPAIRVECTOR		m_vVotedPairs;
	int						m_lNrStars{ 0 };
	bool					m_bComet{ false };

public:
	ListBitMap() = default;
	ListBitMap(const ListBitMap&) = default;
	ListBitMap& operator=(const ListBitMap&) = default;

	bool IsUseAsStarting() const
	{
		return m_bUseAsStarting;
	}

	bool IsDeltaComputed() const
	{
		return m_bDeltaComputed;
	}

	inline bool operator==(const ListBitMap& rhs) const
	{
		return (m_strPath == rhs.m_strPath && m_strFile == rhs.m_strFile);
	}

	inline bool operator!=(const ListBitMap& rhs) const
	{
		return !(*this == rhs);
	}

	void EraseFile()
	{
		fs::path path{ filePath };
		fs::remove(path);
		if (IsLightFrame())
		{
			path.replace_extension("Info.txt");
			fs::remove(path);
		}
	}
};

typedef std::vector<ListBitMap>		LISTBITMAPVECTOR;

/* ------------------------------------------------------------------- */
