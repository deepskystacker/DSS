#ifndef __FRAMEINFO_H__
#define __FRAMEINFO_H__
#include <filesystem>
#include "MatchingStars.h"
#include "SkyBackground.h"

//#include "RAWUtils.h"

// From FITSUtils.h/.cpp
CFATYPE GetFITSCFATYPE();

bool IsSuperPixels();
bool IsRawBayer();
bool IsFITSRawBayer();
bool IsFITSSuperPixels();

namespace fs = std::filesystem;	

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
	int				m_lBitPerChannels;
	int				m_lNrChannels;
	SYSTEMTIME			m_FileTime;
	CString				m_strDateTime;
	SYSTEMTIME			m_DateTime;
	bool				m_bMaster;
	CString				m_strInfos;
	bool				m_bFITS16bit;
	CBitmapExtraInfo	m_ExtraInfo;
	QString				m_filterName;
    mutable	QString	incompatibilityReason;


private :
	mutable CFATYPE			m_CFAType;
	mutable bool			m_bSuperPixel;

protected :
	void CopyFrom(const CFrameInfo & cfi)
	{
		m_lWidth		  = cfi.m_lWidth;
		m_lHeight		  = cfi.m_lHeight;
		filePath  = cfi.filePath;
		m_lISOSpeed		  = cfi.m_lISOSpeed;
		m_lGain			  = cfi.m_lGain;
		m_fExposure		  = cfi.m_fExposure;
		m_fAperture = cfi.m_fAperture;
		m_PictureType	  = cfi.m_PictureType;
		m_CFAType		  = cfi.m_CFAType;
		m_lBitPerChannels = cfi.m_lBitPerChannels;
		m_lNrChannels	  = cfi.m_lNrChannels;
		m_FileTime		  = cfi.m_FileTime;
		m_strDateTime	  = cfi.m_strDateTime;
		m_DateTime		  = cfi.m_DateTime;
		m_bMaster		  = cfi.m_bMaster;
		m_strInfos		  = cfi.m_strInfos;
		m_bFITS16bit	  = cfi.m_bFITS16bit;
		m_bSuperPixel	  = cfi.m_bSuperPixel;
		m_ExtraInfo		  = cfi.m_ExtraInfo;
		m_filterName	  = cfi.m_filterName;
	};

	void	Reset()
	{
		m_lISOSpeed		  = 0;
		m_lGain			  = -1;
		m_fExposure		  = 0.0;
		m_fAperture = 0.0;
		m_PictureType     = PICTURETYPE_LIGHTFRAME;
		m_CFAType		  = CFATYPE_NONE;
		m_lNrChannels	  = 3;
		m_lBitPerChannels = 16;
		m_bMaster		  = false;
		m_bFITS16bit	  = false;
		m_bSuperPixel	  = false;
		m_DateTime = { 0 };
		m_FileTime = { 0 };
		m_ExtraInfo.Clear();
        m_lWidth = 0;
        m_lHeight = 0;
	};

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

	bool	IsCompatible(int lWidth, int lHeight, int lBitPerChannels, int lNrChannels, CFATYPE CFAType) const
	{
		bool			result = true;

		if (m_lWidth != lWidth)
		{
			incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Width mismatch");
			return false;
		}
		if (m_lHeight != lHeight)
		{
			incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Height mismatch");
			return false;
		}
		if (m_lBitPerChannels != lBitPerChannels)
		{
			incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Colour depth mismatch");
			return false;
		}

		if (result)
		{
			result = (m_lNrChannels == lNrChannels) && (m_CFAType == CFAType);
			if (!result)
			{
				// Check that if CFA if Off then the number of channels may be
				// 3 instead of 1 if BayerDrizzle and SuperPixels are off
				if (!IsRawBayer() && !IsSuperPixels() && !IsFITSRawBayer() && !IsFITSSuperPixels())
				{
					if (m_CFAType != CFAType)
					{
						if ((m_CFAType != CFATYPE_NONE) && (m_lNrChannels==1))
							result = (CFAType != CFATYPE_NONE) && (lNrChannels == 3);
						else if ((CFAType == CFATYPE_NONE) && (lNrChannels == 1))
							result = (m_CFAType == CFATYPE_NONE) && (m_lNrChannels == 3);
					};
					if (false == result)
						incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Number of channels mismatch");
				};
			};
		};

		return  result;
	};

	bool	IsCompatible(const CFrameInfo & cfi) const
	{
		return IsCompatible(cfi.m_lWidth, cfi.m_lHeight, cfi.m_lBitPerChannels, cfi.m_lNrChannels, cfi.m_CFAType);
	};

	bool	InitFromFile(const fs::path& file, PICTURETYPE Type);

	CFATYPE	GetCFAType() const
	{
		if (m_bFITS16bit)
		{
			//
			// On the old days this was always determined by the values read from the 
			// workspace by GetFITSCFATYPE().  Now however GetFITSInfo() may auto-detect
			// the CFA pattern and pre-populate CFAType, which we should now use.  If it's
			// not set then do it the old way.
			//
			if (m_CFAType != CFATYPE_NONE)
				return m_CFAType;
			else
				return m_CFAType = GetFITSCFATYPE();
		}
		else
			return m_CFAType;
	};

	bool	IsSuperPixel() const
	{
		if (m_bFITS16bit)
			m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);

		return m_bSuperPixel;
	};

	void	RefreshSuperPixel()
	{
		m_bSuperPixel = false;
		if (m_bFITS16bit)
			m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);
		else if (m_CFAType != CFATYPE_NONE)
			m_bSuperPixel = ::IsSuperPixels();
	};
};

typedef std::vector<CFrameInfo>				FRAMEINFOVECTOR;
typedef std::vector<CFrameInfo *>			PFRAMEINFOVECTOR;


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

#endif // __FRAMEINFO_H__