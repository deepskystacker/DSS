#pragma once
#include "BitmapExtraInfo.h"
#include "cfa.h"

class CBitmapInfo
{
public:
	QString m_strFileName;
	QString m_strFileType;
	QString m_strModel;
	int m_lISOSpeed;
	int m_lGain;
	double m_fExposure;
	double m_fAperture;
	int m_lWidth;
	int m_lHeight;
	int m_lBitPerChannel;
	int m_lNrChannels;
	bool m_bCanLoad;
	bool m_bFloat;
	CFATYPE m_CFAType;
	bool m_bMaster;
	bool m_bFITS16bit;
	QString m_strDateTime;
	SYSTEMTIME m_DateTime;
	SYSTEMTIME m_InfoTime;
	CBitmapExtraInfo m_ExtraInfo;
	int m_xBayerOffset;
	int m_yBayerOffset;
	QString m_filterName;

private:
	void CopyFrom(const CBitmapInfo& bi);
	void Init();

public:
	CBitmapInfo();
	CBitmapInfo(const CBitmapInfo& bi);
	CBitmapInfo(LPCTSTR szFileName);

	virtual ~CBitmapInfo() = default;

	CBitmapInfo& operator = (const CBitmapInfo& bi);
	bool operator<(const CBitmapInfo& other) const;
	bool operator==(const CBitmapInfo& other) const;

	bool CanLoad() const;
	bool IsCFA();
	bool IsMaster();
	void GetDescription(QString& strDescription);
	bool IsInitialized();
};

namespace Gdiplus { class Bitmap; }
bool RetrieveEXIFInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo);
bool RetrieveEXIFInfo(Gdiplus::Bitmap* pBitmap, CBitmapInfo& BitmapInfo);
