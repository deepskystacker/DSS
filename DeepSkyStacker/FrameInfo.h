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
	LONG				m_lGain;
	double				m_fExposure;
	double				m_fAperture;
	PICTURETYPE			m_PictureType;
	LONG				m_lBitPerChannels;
	LONG				m_lNrChannels;
	SYSTEMTIME			m_FileTime;
	CString				m_strDateTime;
	SYSTEMTIME			m_DateTime;
	bool				m_bMaster;
	CString				m_strInfos;
	bool				m_bFITS16bit;
	CBitmapExtraInfo	m_ExtraInfo;
	CString				m_filterName;

private :
	mutable CFATYPE			m_CFAType;
	mutable bool			m_bSuperPixel;

protected :
	void CopyFrom(const CFrameInfo & cfi)
	{
		m_lWidth		  = cfi.m_lWidth;
		m_lHeight		  = cfi.m_lHeight;
		m_strFileName	  = cfi.m_strFileName;
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
		m_DateTime.wYear  = 0;
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

	LONG	RenderedWidth()
	{
		return m_lWidth/(m_bSuperPixel ? 2 : 1);
	};

	LONG	RenderedHeight()
	{
		return m_lHeight/(m_bSuperPixel ? 2 : 1);
	};

	bool	IsCompatible(LONG lWidth, LONG lHeight, LONG lBitPerChannels, LONG lNrChannels, CFATYPE CFAType)
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

	bool	IsCompatible(const CFrameInfo & cfi)
	{
		return IsCompatible(cfi.m_lWidth, cfi.m_lHeight, cfi.m_lBitPerChannels, cfi.m_lNrChannels, cfi.m_CFAType);
	};

	bool	InitFromFile(LPCTSTR szFile, PICTURETYPE Type);

	CFATYPE	GetCFAType() const
	{
		if (m_bFITS16bit)
			return m_CFAType = GetFITSCFATYPE();
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

/* ------------------------------------------------------------------- */

inline void	ExposureToString(double fExposure, CString & strText)
{
	// DELETE THIS ONE DAY
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

inline QString exposureToString(double fExposure)
{
	QString strText;

	if (fExposure)
	{
		qint64			exposure;

		if (fExposure >= 1)
		{
			exposure = fExposure;
			qint64			remainingTime = exposure;
			qint64			hours, mins, secs;

			hours = remainingTime / 3600;
			remainingTime -= hours * 3600;
			mins = remainingTime / 60;
			remainingTime -= mins * 60;
			secs = remainingTime;

			if (hours)
				strText = QString(QCoreApplication::translate("StackRecap", "%1 hr %2 mn %3 s ", "IDS_EXPOSURETIME3"))
					.arg(hours)
					.arg(mins)
					.arg(secs);
			else if (mins)
				strText = QString(QCoreApplication::translate("StackRecap", "%1 mn %2 s ", "IDS_EXPOSURETIME2"))
					.arg(mins)
					.arg(secs);
			else
				strText = QString(QCoreApplication::translate("StackRecap", "%1 s ", "IDS_EXPOSURETIME1"))
					.arg(secs);
		}
		else
		{
			exposure = 1.0 / fExposure + 0.5;
			strText = QString(QCoreApplication::translate("StackRecap", "1/%1 s", "IDS_EXPOSUREFORMAT_INF"))
				.arg(exposure);
		};
	}
	else
		strText = "-";

	return strText;
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

inline void	GainToString(LONG lGain, CString & strText)
{
	if (lGain >= 0)
		strText.Format(_T("%ld"), lGain);
	else
		strText = "-";
};

/* ------------------------------------------------------------------- */

#endif // __FRAMEINFO_H__