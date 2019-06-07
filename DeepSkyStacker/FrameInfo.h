#ifndef __FRAMEINFO_H__
#define __FRAMEINFO_H__

// From FITSUtils.h/.cpp
CFATYPE GetFITSCFATYPE();

class CFrameInfo
{
public :
	CString				m_strFileName;
	LONG				m_lWidth,
						m_lHeight;
	LONG				m_lISOSpeed;
	double				m_fExposure;
	PICTURETYPE			m_PictureType;
	LONG				m_lBitPerChannels;
	LONG				m_lNrChannels;
	SYSTEMTIME			m_FileTime;
	CString				m_strDateTime;
	SYSTEMTIME			m_DateTime;
	BOOL				m_bMaster;
	CString				m_strInfos;
	BOOL				m_bFITS16bit;
	CBitmapExtraInfo	m_ExtraInfo;

private :
	mutable CFATYPE			m_CFAType;
	mutable BOOL			m_bSuperPixel;

protected :
	void CopyFrom(const CFrameInfo & cfi)
	{
		m_lWidth		  = cfi.m_lWidth;
		m_lHeight		  = cfi.m_lHeight;
		m_strFileName	  = cfi.m_strFileName;
		m_lISOSpeed		  = cfi.m_lISOSpeed;
		m_fExposure		  = cfi.m_fExposure;
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
	};

	void	Reset()
	{
		m_lISOSpeed		  = 0;
		m_fExposure		  = 0.0;
		m_PictureType     = PICTURETYPE_LIGHTFRAME;
		m_CFAType		  = CFATYPE_NONE;
		m_lNrChannels	  = 3;
		m_lBitPerChannels = 16;
		m_bMaster		  = FALSE;
		m_bFITS16bit	  = FALSE;
		m_bSuperPixel	  = FALSE;
		m_DateTime.wYear  = 0;
		m_ExtraInfo.Clear();
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

	BOOL	IsLightFrame() const
	{
		return (m_PictureType == PICTURETYPE_LIGHTFRAME);
	};

	BOOL	IsDarkFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFRAME);
	};

	BOOL	IsDarkFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_DARKFLATFRAME);
	};

	BOOL	IsFlatFrame() const
	{
		return (m_PictureType == PICTURETYPE_FLATFRAME);
	};

	BOOL	IsOffsetFrame() const
	{
		return (m_PictureType == PICTURETYPE_OFFSETFRAME);
	};

	BOOL	IsMasterFrame() const
	{
		return m_bMaster;
	};

	LONG	RenderedWidth()
	{
		return m_lWidth/(m_bSuperPixel ? 2 : 1);
	};

	LONG	RenderedHeight()
	{
		return m_lHeight/(m_bSuperPixel ? 2 : 1);
	};

	BOOL	IsCompatible(LONG lWidth, LONG lHeight, LONG lBitPerChannels, LONG lNrChannels, CFATYPE CFAType)
	{
		BOOL			bResult;

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

	BOOL	IsCompatible(const CFrameInfo & cfi)
	{
		return IsCompatible(cfi.m_lWidth, cfi.m_lHeight, cfi.m_lBitPerChannels, cfi.m_lNrChannels, cfi.m_CFAType);
	};

	BOOL	InitFromFile(LPCTSTR szFile, PICTURETYPE Type);

	CFATYPE	GetCFAType() const
	{
		if (m_bFITS16bit)
			return m_CFAType = GetFITSCFATYPE();
		else
			return m_CFAType;
	};

	BOOL	IsSuperPixel() const
	{
		if (m_bFITS16bit)
			m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);

		return m_bSuperPixel;
	};

	void	RefreshSuperPixel()
	{
		m_bSuperPixel = FALSE;
		if (m_bFITS16bit)
			m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);
		else if (m_CFAType != CFATYPE_NONE)
			m_bSuperPixel = ::IsSuperPixels();
	};
};

typedef std::vector<CFrameInfo>				FRAMEINFOVECTOR;
typedef std::vector<CFrameInfo *>			PFRAMEINFOVECTOR;

/* ------------------------------------------------------------------- */

inline void	ExposureToString(double fExposure, CString & strText)
{
	if (fExposure)
	{
		LONG			lExposure;

		if (fExposure >= 1)
		{
			lExposure = fExposure;
			DWORD			dwRemainingTime = lExposure;
			DWORD			dwHour,
							dwMin,
							dwSec;

			dwHour = dwRemainingTime / 3600;
			dwRemainingTime -= dwHour * 3600;
			dwMin = dwRemainingTime / 60;
			dwRemainingTime -= dwMin * 60;
			dwSec = dwRemainingTime;

			if (dwHour)
				strText.Format(IDS_EXPOSURETIME3, dwHour, dwMin, dwSec);
			else if (dwMin)
				strText.Format(IDS_EXPOSURETIME2, dwMin, dwSec);
			else
				strText.Format(IDS_EXPOSURETIME1, dwSec);
		}
		else
		{
			lExposure = 1.0/fExposure+0.5;
			strText.Format(IDS_EXPOSUREFORMAT_INF, lExposure);
		};
	}
	else
		strText = "-";
};

/* ------------------------------------------------------------------- */

inline void	ISOToString(LONG lISOSpeed, CString & strText)
{
	if (lISOSpeed)
		strText.Format(_T("%ld"), lISOSpeed);
	else
		strText = "-";
};

/* ------------------------------------------------------------------- */

#endif // __FRAMEINFO_H__