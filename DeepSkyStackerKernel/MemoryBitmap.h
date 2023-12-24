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
	QDateTime m_DateTime;

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

	double GetExposure() const
	{
		return m_fExposure;
	}

	CMemoryBitmap& SetExposure(double fExposure)
	{
		m_fExposure = fExposure;
		return *this;
	}

	double GetAperture() const
	{
		return m_fAperture;
	}

	CMemoryBitmap& SetAperture(double fAperture)
	{
		m_fAperture = fAperture;
		return *this;
	}

	int GetISOSpeed() const
	{
		return m_lISOSpeed;
	}

	CMemoryBitmap& SetISOSpeed(int lISOSpeed)
	{
		m_lISOSpeed = lISOSpeed;
		return *this;
	}

	int GetGain() const
	{
		return m_lGain;
	}

	CMemoryBitmap& SetGain(int lGain)
	{
		m_lGain = lGain;
		return *this;
	}

	int GetNrFrames() const
	{
		return m_lNrFrames;
	}

	CMemoryBitmap& SetNrFrames(int lNrFrames)
	{
		m_lNrFrames = lNrFrames;
		return *this;
	}

	const CMemoryBitmap& GetDescription(QString& strDescription) const
	{
		strDescription = m_strDescription;
		return *this;
	}

	CMemoryBitmap& SetDescription(const QString& description)
	{
		m_strDescription = description;
		return *this;
	}

	const QString& filterName() const
	{
		return m_filterName;
	}

	CMemoryBitmap& setFilterName(const QString& name)
	{
		m_filterName = name;
		return *this;
	}

	virtual bool	Init(int lWidth, int lHeight) = 0;

	virtual void	SetPixel(size_t i, size_t j, double fRed, double fGreen, double fBlue) = 0;
	virtual void	SetPixel(size_t i, size_t j, double fGray) = 0;
	virtual void	GetPixel(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) const = 0;
	virtual void	GetPixel(size_t i, size_t j, double& fGray) const = 0;

	virtual double getValue(size_t, size_t) const
	{
		ZASSERTSTATE(false);
		return 0.0;
	}
	virtual std::tuple<double, double, double> getValues(size_t, size_t) const
	{
		ZASSERTSTATE(false);
		return { 0.0, 0.0, 0.0 };
	}
 	
	virtual void	SetValue(size_t, size_t, double, double, double) {};
	virtual void	GetValue(size_t, size_t, double&, double&, double&) const {};
	virtual void	SetValue(size_t, size_t, double) {};
	virtual void	GetValue(size_t, size_t, double&) const {};

	virtual bool	GetScanLine(size_t j, void* pScanLine) const = 0;
	virtual bool	SetScanLine(size_t j, void* pScanLine) = 0;

	void GetPixel16(const size_t i, const size_t j, COLORREF16& crResult) const;

	virtual int Width() const = 0;
	virtual int Height() const = 0;
	virtual int BitPerSample() const = 0;
	virtual bool IsFloat() const = 0;

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

	void SetMaster(bool bMaster)
	{
		m_bMaster = bMaster;
	}

	bool IsMaster() const
	{
		return m_bMaster;
	}

	bool isTopDown() const
	{
		return m_bTopDown;
	}

	virtual void	SetCFA(bool bCFA)
	{
		m_bCFA = bCFA;
	}

	bool IsCFA() const
	{
		return m_bCFA;
	}

	virtual BAYERCOLOR GetBayerColor(int, int) const
	{
		return BAYER_UNKNOWN;
	}

	bool	IsOk() const
	{
		return (Width() > 0) && (Height() > 0);
	}

	virtual CFATYPE	GetCFAType() const
	{
		return CFATYPE_NONE;
	}

	virtual std::unique_ptr<CMemoryBitmap> Clone(bool bEmpty = false) const = 0;

	virtual std::shared_ptr<CMultiBitmap> CreateEmptyMultiBitmap() const = 0;
	virtual void AverageBitmap(CMemoryBitmap*, DSS::ProgressBase*) {};
	virtual void RemoveHotPixels(DSS::ProgressBase* pProgress = nullptr) = 0;
	virtual std::shared_ptr<CMedianFilterEngine> GetMedianFilterEngine() const = 0;

	virtual double GetMaximumValue() const = 0;
	virtual void GetCharacteristics(CBitmapCharacteristics& bc) const = 0;

	virtual void InitIterator(void*& pRed, void*& pGreen, void*& pBlue, size_t& elementSize, const size_t x, const size_t y) = 0;
	virtual void InitIterator(const void*& pRed, const void*& pGreen, const void*& pBlue, size_t& elementSize, const size_t x, const size_t y) const = 0;
	virtual std::tuple<double, double, double> ConvertValue3(const void* pRed, const void* pGreen, const void* pBlue) const = 0;
	virtual double ConvertValue1(const void* pRed, const void* pGreen, const void* pBlue) const = 0;
	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double gray) const = 0;
	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double red, const double green, const double blue) const = 0;
};
