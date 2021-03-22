#ifndef _STACKEDBITMAP_H__
#define _STACKEDBITMAP_H__

#include <math.h>
#include <vector>
#include <algorithm>
#include "BitmapExt.h"
#include "BezierAdjust.h"
#include "Histogram.h"
#include "DSSTools.h"

#ifndef PI
#define PI 3.141592654
#endif

/* ------------------------------------------------------------------- */

#pragma pack(push, HDPIXELINFO, 4)
/*
class CPixelInfo // 4 bytes structure for each pixel/plane
{
public :
	float					m_fSumValue;

private :
	void CopyFrom(const CPixelInfo & cpi)
	{
		m_fSumValue			= cpi.m_fSumValue;
	};

public :
	CPixelInfo()
	{
		m_fSumValue = 0.0;
	};
	virtual ~CPixelInfo() {};

	CPixelInfo(const CPixelInfo & cpi)
	{
		CopyFrom(cpi);
	};

	CPixelInfo & operator = (const CPixelInfo & cpi)
	{
		CopyFrom(cpi);
		return (*this);
	};

	float * operator & ()
	{
		return &m_fSumValue;
	};

	void	Save(FILE * hFile)
	{
		fwrite(&m_fSumValue, sizeof(m_fSumValue), 1, hFile);
	};

	void	Load(FILE * hFile)
	{
		fread(&m_fSumValue, sizeof(m_fSumValue), 1, hFile);
	};

};
*/
typedef std::vector<float>		PIXELINFOVECTOR;

typedef PIXELINFOVECTOR				CPixelVector;

#pragma pack(pop, HDPIXELINFO)

/* ------------------------------------------------------------------- */



/*
const	LONG	PIXELVECTORBLOCKSIZE = 100000L;

class CPixelVector
{
private :
	LONG							m_lSize;
	LONG							m_lNrBlocks;
	std::vector<PIXELINFOVECTOR>	m_vBlocks;

public :
	CPixelVector()
	{
		m_lSize		= 0;
		m_lNrBlocks = 0;
	};
	virtual ~CPixelVector() {};

	LONG	size()
	{
		return m_lSize;
	};

	void	clear()
	{
		m_vBlocks.clear();
		m_lSize		= 0;
		m_lNrBlocks = 0;
	};

	void	resize(LONG lSize)
	{
		LONG			lLastBlockSize;
		LONG			i;

		m_lNrBlocks = lSize / PIXELVECTORBLOCKSIZE;

		lLastBlockSize = lSize - m_lNrBlocks * PIXELVECTORBLOCKSIZE;
		m_lNrBlocks++; // At least one block

		m_vBlocks.resize(m_lNrBlocks);
		for (i = 0;i<m_lNrBlocks;i++)
		{
			if (i == m_lNrBlocks -1)
				m_vBlocks[i].resize(lLastBlockSize);
			else
				m_vBlocks[i].resize(PIXELVECTORBLOCKSIZE);
		};

		m_lSize = lSize;
	};

	CPixelInfo & operator [] (LONG lIndice)
	{
		LONG			lBlock = lIndice / PIXELVECTORBLOCKSIZE;
		LONG			lIndiceInBlock = lIndice - lBlock * PIXELVECTORBLOCKSIZE;
		return m_vBlocks[lBlock][lIndiceInBlock];
	};
};
*/
/* ------------------------------------------------------------------- */

class CTIFFReader;
class CTIFFWriter;
class CFITSReader;
class CFITSWriter;

/* ------------------------------------------------------------------- */

class CStackedBitmap
{
private :
	LONG						m_lWidth;
	LONG						m_lHeight;
	LONG						m_lOutputWidth,
								m_lOutputHeight;
	LONG						m_lNrBitmaps;
	CPixelVector				m_vRedPlane;
	CPixelVector				m_vGreenPlane;
	CPixelVector				m_vBluePlane;
	LONG						m_lISOSpeed;
	LONG						m_lGain;
	LONG						m_lTotalTime;
	bool						m_bMonochrome;

	CBezierAdjust				m_BezierAdjust;
	CRGBHistogramAdjust 		m_HistoAdjust;

private :
	bool	LoadDSImage(LPCTSTR szStackedFile, CDSSProgress * pProgress = nullptr);
	bool	LoadTIFF(LPCTSTR szStackedFile, CDSSProgress * pProgress = nullptr);
	bool	LoadFITS(LPCTSTR szStackedFile, CDSSProgress * pProgress = nullptr);

	COLORREF	GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings);

public :
	void	ReadSpecificTags(CTIFFReader * tiffReader);
	void	ReadSpecificTags(CFITSReader * fitsReader);
	void	WriteSpecificTags(CTIFFWriter * tiffWriter, bool bApplySettings);
	void	WriteSpecificTags(CFITSWriter * fitsWriter, bool bApplySettings);

public :
	CStackedBitmap() ;
	virtual ~CStackedBitmap() {};

	void	SetOutputSizes(LONG lWidth, LONG lHeight)
	{
		m_lOutputWidth  = lWidth;
		m_lOutputHeight = lHeight;
	};

	bool	Allocate(LONG lWidth, LONG lHeight, bool bMonochrome)
	{
		size_t			lSize;

		m_lWidth  = lWidth;
		m_lHeight = lHeight;

		m_bMonochrome = bMonochrome;
		lSize = m_lWidth * m_lHeight;
		m_vRedPlane.clear();
		m_vGreenPlane.clear();
		m_vBluePlane.clear();

		m_vRedPlane.resize(lSize);
		if (!m_bMonochrome)
		{
			m_vGreenPlane.resize(lSize);
			m_vBluePlane.resize(lSize);
		};

		if (m_bMonochrome)
			return (m_vRedPlane.size() == lSize);
		else
			return (m_vRedPlane.size() == lSize) &&
				   (m_vGreenPlane.size() == lSize) &&
				   (m_vBluePlane.size() == lSize);
	};

	void		SetHistogramAdjust(const CRGBHistogramAdjust & HistoAdjust)
	{
		m_HistoAdjust = HistoAdjust;
	};

	void	SetBezierAdjust(const CBezierAdjust & BezierAdjust)
	{
		m_BezierAdjust = BezierAdjust;
	};

	void		GetBezierAdjust(CBezierAdjust & BezierAdjust)
	{
		BezierAdjust = m_BezierAdjust;
	};

	void		GetHistogramAdjust(CRGBHistogramAdjust & HistoAdjust)
	{
		HistoAdjust = m_HistoAdjust;
	};

	COLORREF	GetPixel(LONG X, LONG Y, bool bApplySettings = true);
	COLORREF16	GetPixel16(LONG X, LONG Y, bool bApplySettings = true);
	COLORREF32	GetPixel32(LONG X, LONG Y, bool bApplySettings = true);


	void		SetPixel(LONG X, LONG Y, double fRed, double fGreen, double fBlue)
	{
		size_t		lOffset = m_lWidth * Y + X;

		m_vRedPlane[lOffset]	= fRed * m_lNrBitmaps;
		if (!m_bMonochrome)
		{
			m_vGreenPlane[lOffset]	= fGreen * m_lNrBitmaps;
			m_vBluePlane[lOffset]	= fBlue * m_lNrBitmaps;
		};
	};

	void		GetPixel(LONG X, LONG Y, double & fRed, double & fGreen, double & fBlue, bool bApplySettings);

	const auto& getRedPixels() const { return this->m_vRedPlane; }
	const auto& getGreenPixels() const { return this->m_vGreenPlane; }
	const auto& getBluePixels() const { return this->m_vBluePlane; }

	double		GetRedValue(LONG X, LONG Y)
	{
		return m_vRedPlane[static_cast<size_t>(m_lWidth * Y + X)]/m_lNrBitmaps*255.0;
	};
	double		GetGreenValue(LONG X, LONG Y)
	{
		if (!m_bMonochrome)
			return m_vGreenPlane[static_cast<size_t>(m_lWidth * Y + X)]/m_lNrBitmaps*255.0;
		else
			return GetRedValue(X, Y);
	};
	double		GetBlueValue(LONG X, LONG Y)
	{
		if (!m_bMonochrome)
			return m_vBluePlane[static_cast<size_t>(m_lWidth * Y + X)]/m_lNrBitmaps*255.0;
		else
			return GetRedValue(X, Y);
	};

	void	SetISOSpeed(LONG lISOSpeed)
	{
		m_lISOSpeed = lISOSpeed;
	};

	WORD	GetISOSpeed()
	{
		return (WORD)m_lISOSpeed;
	};

	void	SetGain(LONG lGain)
	{
		m_lGain = lGain;
	};

	LONG	GetGain()
	{
		return m_lGain;
	};

	LONG	GetTotalTime()
	{
		return m_lTotalTime;
	};

	LONG	GetNrStackedFrames()
	{
		return m_lNrBitmaps;
	};

	bool	Load(LPCTSTR szStackedFile, CDSSProgress * pProgress = nullptr);
	void	SaveDSImage(LPCTSTR szStackedFile, LPRECT pRect = nullptr, CDSSProgress * pProgress = nullptr);
	void	SaveTIFF16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect = nullptr, CDSSProgress * pProgress = nullptr, bool bApplySettings = true, TIFFCOMPRESSION TiffComp = TC_NONE);
	void	SaveTIFF32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect = nullptr, CDSSProgress * pProgress = nullptr, bool bApplySettings = true, bool bFloat = false, TIFFCOMPRESSION TiffComp = TC_NONE);
	void	SaveFITS16Bitmap(LPCTSTR szBitmapFile, LPRECT pRect = nullptr, CDSSProgress * pProgress = nullptr, bool bApplySettings = true);
	void	SaveFITS32Bitmap(LPCTSTR szBitmapFile, LPRECT pRect = nullptr, CDSSProgress * pProgress = nullptr, bool bApplySettings = true, bool bFloat = false);
#if !defined(PCL_PROJECT) && !defined(_CONSOLE)
	HBITMAP	GetBitmap(C32BitsBitmap & Bitmap, RECT * pRect = nullptr);
#endif
	bool	GetBitmap(CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress = nullptr);

	void	Clear()
	{
		m_lNrBitmaps = 0;
		m_lHeight = 0;
		m_lWidth  = 0;
		m_lOutputWidth = 0;
		m_lOutputHeight = 0;
		m_vRedPlane.clear();
		m_vGreenPlane.clear();
		m_vBluePlane.clear();
		m_lTotalTime = 0;
		m_lISOSpeed  = 0;
		m_lGain  = -1;
	};

	LONG	GetWidth()
	{
		return m_lWidth;
	};

	LONG	GetHeight()
	{
		return m_lHeight;
	};

	bool	IsMonochrome()
	{
		return m_bMonochrome;
	};
};

/* ------------------------------------------------------------------- */
#endif // _STACKEDBITMAP_H__