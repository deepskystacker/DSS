#pragma once

#include "BitmapExtraInfo.h"
#include "ColorRef.h"
#include "Bayer.h"

class CMultiBitmap;
class CMedianFilterEngine;
class CBitmapCharacteristics;
namespace DSS { class ProgressBase; }

class CMemoryBitmap
{
public:
	CBitmapExtraInfo m_ExtraInfo;
	SYSTEMTIME m_DateTime;

protected:
	bool m_bTopDown;
	bool m_bMaster;
	bool m_bCFA;
	double m_fExposure;
	double m_fAperture;
	int m_lISOSpeed;
	int m_lGain;
	int m_lNrFrames;
	QString m_strDescription;
	QString m_filterName;

protected:
	void CopyFrom(const CMemoryBitmap& mb);

public:
	CMemoryBitmap();
	virtual ~CMemoryBitmap() = default;

	virtual double	GetExposure()
	{
		return m_fExposure;
	};

	virtual CMemoryBitmap& SetExposure(double fExposure)
	{
		m_fExposure = fExposure;
		return *this;
	};

	virtual double	GetAperture()
	{
		return m_fAperture;
	};

	virtual CMemoryBitmap& SetAperture(double fAperture)
	{
		m_fAperture = fAperture;
		return *this;
	};

	virtual int GetISOSpeed()
	{
		return m_lISOSpeed;
	};

	virtual CMemoryBitmap& SetISOSpeed(int lISOSpeed)
	{
		m_lISOSpeed = lISOSpeed;
		return *this;
	};

	virtual int GetGain()
	{
		return m_lGain;
	};

	virtual CMemoryBitmap& SetGain(int lGain)
	{
		m_lGain = lGain;
		return *this;
	};

	virtual int GetNrFrames()
	{
		return m_lNrFrames;
	};

	virtual CMemoryBitmap& SetNrFrames(int lNrFrames)
	{
		m_lNrFrames = lNrFrames;
		return *this;
	};

	virtual CMemoryBitmap& SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = QString::fromWCharArray(szDescription);
		return *this;
	};

	virtual CMemoryBitmap& GetDescription(QString& strDescription)
	{
		strDescription = m_strDescription;
		return *this;
	};

	virtual CMemoryBitmap& setFilterName(const QString& name)
	{
		m_filterName = name;
		return *this;
	}

	virtual QString filterName()
	{
		return m_filterName;
	}

	virtual bool	Init(int lWidth, int lHeight) = 0;

	virtual void	SetPixel(size_t i, size_t j, double fRed, double fGreen, double fBlue) = 0;
	virtual void	SetPixel(size_t i, size_t j, double fGray) = 0;
	virtual void	GetPixel(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) = 0;
	virtual void	GetPixel(size_t i, size_t j, double& fGray) = 0;

	virtual void	SetValue(size_t i, size_t j, double fRed, double fGreen, double fBlue) {};
	virtual void	GetValue(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) const {};
	virtual void	SetValue(size_t i, size_t j, double fGray) {};
	virtual void	GetValue(size_t i, size_t j, double& fGray) const {};

	virtual bool	GetScanLine(size_t j, void* pScanLine) = 0;
	virtual bool	SetScanLine(size_t j, void* pScanLine) = 0;

	void GetPixel16(const size_t i, const size_t j, COLORREF16& crResult);

	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual int BitPerSample() = 0;
	virtual int IsFloat() = 0;

	virtual int RealWidth() const
	{
		return Width();
	}

	virtual int RealHeight() const
	{
		return Height();
	}

	void	SetOrientation(bool bTopDown)
	{
		m_bTopDown = bTopDown;
	}

	virtual bool	IsMonochrome() const = 0;

	virtual void	SetMaster(bool bMaster)
	{
		m_bMaster = bMaster;
	}

	virtual bool	IsMaster()
	{
		return m_bMaster;
	}

	virtual bool isTopDown()
	{
		return m_bTopDown;
	}

	virtual void	SetCFA(bool bCFA)
	{
		m_bCFA = bCFA;
	}

	virtual bool	IsCFA() const
	{
		return m_bCFA;
	}

	virtual BAYERCOLOR GetBayerColor(int x, int y)
	{
		return BAYER_UNKNOWN;
	}

	bool	IsOk()
	{
		return (Width() > 0) && (Height() > 0);
	}

	virtual CFATYPE	GetCFAType() const
	{
		return CFATYPE_NONE;
	}

	virtual std::unique_ptr<CMemoryBitmap> Clone(bool bEmpty = false) const = 0;

	virtual std::shared_ptr<CMultiBitmap> CreateEmptyMultiBitmap() const = 0;
	virtual void AverageBitmap(CMemoryBitmap* pBitmap, DSS::ProgressBase* pProgress) {};
	virtual void RemoveHotPixels(DSS::ProgressBase* pProgress = nullptr) = 0;
	virtual std::shared_ptr<CMedianFilterEngine> GetMedianFilterEngine() const = 0;

	virtual double GetMaximumValue() const = 0;
	virtual void GetCharacteristics(CBitmapCharacteristics& bc) = 0;

	virtual void InitIterator(void*& pRed, void*& pGreen, void*& pBlue, size_t& elementSize, const size_t x, const size_t y) = 0;
	virtual void InitIterator(const void*& pRed, const void*& pGreen, const void*& pBlue, size_t& elementSize, const size_t x, const size_t y) const = 0;
	virtual std::tuple<double, double, double> ConvertValue3(const void* pRed, const void* pGreen, const void* pBlue) const = 0;
	virtual double ConvertValue1(const void* pRed, const void* pGreen, const void* pBlue) const = 0;
	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double gray) const = 0;
	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double red, const double green, const double blue) const = 0;
};
