#include "stdafx.h"
#include "FrameInfo.h"
#include "BitmapInfo.h"
#include "BitmapExt.h"


/* ------------------------------------------------------------------- */

bool CFrameInfo::InitFromFile(const fs::path& file, PICTURETYPE Type)
{
	bool				bResult = false;

	if (Type == PICTURETYPE_REFLIGHTFRAME)
		m_PictureType  = PICTURETYPE_LIGHTFRAME;
	else
		m_PictureType  = Type;
	filePath = file;

	CBitmapInfo			bmpInfo;

	bResult = GetPictureInfo(file ,bmpInfo);

	if (bResult)
	{
		m_lWidth			= bmpInfo.m_lWidth;
		m_lHeight			= bmpInfo.m_lHeight;
		m_lBitsPerChannel	= bmpInfo.m_lBitsPerChannel;
		m_lNrChannels		= bmpInfo.m_lNrChannels;
		m_CFAType			= bmpInfo.m_CFAType;
		m_bFITS16bit		= bmpInfo.m_bFITS16bit;
		m_DateTime			= bmpInfo.m_DateTime;
		m_strDateTime		= bmpInfo.m_strDateTime;
		m_ExtraInfo			= bmpInfo.m_ExtraInfo;

		bmpInfo.GetDescription(m_strInfos);

		m_lISOSpeed			= bmpInfo.m_lISOSpeed;
		m_lGain				= bmpInfo.m_lGain;
		m_fExposure			= bmpInfo.m_fExposure;
		m_fAperture			= bmpInfo.m_fAperture;

		m_bMaster			= bmpInfo.IsMaster();
		m_filterName		= bmpInfo.m_filterName;

		RefreshSuperPixel();
	};

	return bResult;
}

//void CFrameInfo::Reset()
//{
//	m_lISOSpeed = 0;
//	m_lGain = -1;
//	m_fExposure = 0.0;
//	m_fAperture = 0.0;
//	m_PictureType = PICTURETYPE_LIGHTFRAME;
//	m_CFAType = CFATYPE_NONE;
//	m_lNrChannels = 3;
//	m_lBitsPerChannel = 16;
//	m_bMaster = false;
//	m_bFITS16bit = false;
//	m_bSuperPixel = false;
//	m_ExtraInfo.Clear();
//	m_lWidth = 0;
//	m_lHeight = 0;
//}

bool CFrameInfo::IsCompatible(const CFrameInfo& cfi) const
{
	int width{ cfi.m_lWidth }, height{ cfi.m_lHeight }, bitsPerChannel{ cfi.m_lBitsPerChannel },
		nrChannels{ cfi.m_lNrChannels };
	CFATYPE CFAType{ cfi.m_CFAType };

	bool			result = true;

	if (m_lWidth != width)
	{
		incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Width mismatch");
		return false;
	}
	if (m_lHeight != height)
	{
		incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Height mismatch");
		return false;
	}
	if (! cfi.m_bMaster && m_lBitsPerChannel != bitsPerChannel)
	{
		incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Colour depth mismatch");
		return false;
	}

	if (result)
	{
		result = (m_lNrChannels == nrChannels) && (m_CFAType == CFAType);
		if (!result)
		{
			// Check that if CFA if Off then the number of channels may be
			// 3 instead of 1 if BayerDrizzle and SuperPixels are off
			if (!IsRawBayer() && !IsSuperPixels() && !IsFITSRawBayer() && !IsFITSSuperPixels())
			{
				if (m_CFAType != CFAType)
				{
					if ((m_CFAType != CFATYPE_NONE) && (m_lNrChannels == 1))
						result = (CFAType != CFATYPE_NONE) && (nrChannels == 3);
					else if ((CFAType == CFATYPE_NONE) && (nrChannels == 1))
						result = (m_CFAType == CFATYPE_NONE) && (m_lNrChannels == 3);
				};
				if (false == result)
					incompatibilityReason = QCoreApplication::translate("DSS::StackingDlg", "Number of channels mismatch");
			};
		};
	};

	return  result;
}


CFATYPE	CFrameInfo::GetCFAType() const
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
}

bool CFrameInfo::IsSuperPixel() const
{
	if (m_bFITS16bit)
		m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);

	return m_bSuperPixel;
}

void CFrameInfo::RefreshSuperPixel()
{
	m_bSuperPixel = false;
	if (m_bFITS16bit)
		m_bSuperPixel = IsFITSSuperPixels() && (GetCFAType() != CFATYPE_NONE);
	else if (m_CFAType != CFATYPE_NONE)
		m_bSuperPixel = ::IsSuperPixels();
}