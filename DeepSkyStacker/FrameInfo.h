#ifndef __FRAMEINFO_H__
#define __FRAMEINFO_H__
#include <filesystem>

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
	fs::path filePath;  // m_strFileName;
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
		bool			bResult;

		bResult = (m_lWidth == lWidth) &&
				  (m_lHeight == lHeight) &&
				  (m_lBitPerChannels == lBitPerChannels);

		if (bResult)
		{
			bResult = (m_lNrChannels == lNrChannels) && (m_CFAType == CFAType);
			if (!bResult)
			{
				// Check that if CFA if Off then the number of channels may be
				// 3 instead of 1 if BayerDrizzle and SuperPixels are off
				if (!IsRawBayer() && !IsSuperPixels() && !IsFITSRawBayer() && !IsFITSSuperPixels())
				{
					if (m_CFAType != CFAType)
					{
						if ((m_CFAType != CFATYPE_NONE) && (m_lNrChannels==1))
							bResult = (CFAType != CFATYPE_NONE) && (lNrChannels == 3);
						else if ((CFAType == CFATYPE_NONE) && (lNrChannels == 1))
							bResult = (m_CFAType == CFATYPE_NONE) && (m_lNrChannels == 3);
					};
				};
			};
		};

		return  bResult;
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


#endif // __FRAMEINFO_H__