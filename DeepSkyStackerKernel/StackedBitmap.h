#pragma once
#include "BezierAdjust.h"
#include "histogram.h"
#include "ColorRef.h"
#include "BitmapInfo.h"
#include "dssrect.h"


namespace DSS { class ProgressBase; }
class CMemoryBitmap;
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
const	int	PIXELVECTORBLOCKSIZE = 100000L;

class CPixelVector
{
private :
	int							m_lSize;
	int							m_lNrBlocks;
	std::vector<PIXELINFOVECTOR>	m_vBlocks;

public :
	CPixelVector()
	{
		m_lSize		= 0;
		m_lNrBlocks = 0;
	};
	virtual ~CPixelVector() {};

	int	size()
	{
		return m_lSize;
	};

	void	clear()
	{
		m_vBlocks.clear();
		m_lSize		= 0;
		m_lNrBlocks = 0;
	};

	void	resize(int lSize)
	{
		int			lLastBlockSize;
		int			i;

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

	CPixelInfo & operator [] (int lIndice)
	{
		int			lBlock = lIndice / PIXELVECTORBLOCKSIZE;
		int			lIndiceInBlock = lIndice - lBlock * PIXELVECTORBLOCKSIZE;
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
namespace DSS
{
	class StackedBitmap final
	{
	private:
		int						m_lWidth;
		int						m_lHeight;
		int						m_lOutputWidth,
			m_lOutputHeight;
		int						m_lNrBitmaps;
		CPixelVector				m_vRedPlane;
		CPixelVector				m_vGreenPlane;
		CPixelVector				m_vBluePlane;
		int						m_lISOSpeed;
		int						m_lGain;
		int						m_lTotalTime;
		bool						m_bMonochrome;

		BezierAdjust			m_BezierAdjust;
		RGBHistogramAdjust 		m_HistoAdjust;
		CBitmapInfo	bmpInfo;

	private:
		bool	LoadTIFF(const fs::path& file, DSS::ProgressBase* pProgress = nullptr);
		bool	LoadFITS(const fs::path& file, DSS::ProgressBase* pProgress = nullptr);

	//	COLORREF	GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings);

	public:
		void	ReadSpecificTags(CTIFFReader* tiffReader);
		void	ReadSpecificTags(CFITSReader* fitsReader);
		void	WriteSpecificTags(CTIFFWriter* tiffWriter, bool bApplySettings);
		void	WriteSpecificTags(CFITSWriter* fitsWriter, bool bApplySettings);

	public:
		StackedBitmap();
		~StackedBitmap() = default;

		void	SetOutputSizes(int lWidth, int lHeight)
		{
			m_lOutputWidth = lWidth;
			m_lOutputHeight = lHeight;
		};

		bool	Allocate(int lWidth, int lHeight, bool bMonochrome)
		{
			size_t			lSize;

			m_lWidth = lWidth;
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

		void		SetHistogramAdjust(const RGBHistogramAdjust& HistoAdjust)
		{
			m_HistoAdjust = HistoAdjust;
		};

		void	SetBezierAdjust(const DSS::BezierAdjust& BezierAdjust)
		{
			m_BezierAdjust = BezierAdjust;
		};

		void		GetBezierAdjust(DSS::BezierAdjust& BezierAdjust)
		{
			BezierAdjust = m_BezierAdjust;
		};

		void		GetHistogramAdjust(RGBHistogramAdjust& HistoAdjust)
		{
			HistoAdjust = m_HistoAdjust;
		};

//		COLORREF	GetPixel(int X, int Y, bool bApplySettings = true);
//		COLORREF16	GetPixel16(int X, int Y, bool bApplySettings = true);
//		COLORREF32	GetPixel32(int X, int Y, bool bApplySettings = true);

		std::tuple<double, double, double> getValues(size_t X, size_t Y) const
		{
			const size_t lOffset{ m_lWidth * Y + X };

			return {
				m_vRedPlane[lOffset] / m_lNrBitmaps * 256.0,
				m_vGreenPlane[lOffset] / m_lNrBitmaps * 256.0,
				m_vBluePlane[lOffset] / m_lNrBitmaps * 256.0 };
		}

		double getValue(size_t X, size_t Y) const
		{
			const size_t lOffset{ m_lWidth * Y + X };
			return  m_vRedPlane[lOffset] / m_lNrBitmaps * 256.0;
		}

		void SetPixel(int X, int Y, double fRed, double fGreen, double fBlue)
		{
			const size_t lOffset{ m_lWidth * (size_t)Y + (size_t)X };

			m_vRedPlane[lOffset] = fRed * m_lNrBitmaps;
			if (!m_bMonochrome)
			{
				m_vGreenPlane[lOffset] = fGreen * m_lNrBitmaps;
				m_vBluePlane[lOffset] = fBlue * m_lNrBitmaps;
			};
		};

		void		GetPixel(int X, int Y, double& fRed, double& fGreen, double& fBlue, bool bApplySettings);

		const auto& getRedPixels() const { return this->m_vRedPlane; }
		const auto& getGreenPixels() const { return this->m_vGreenPlane; }
		const auto& getBluePixels() const { return this->m_vBluePlane; }

		double		GetRedValue(int X, int Y)
		{
			return m_vRedPlane[static_cast<size_t>(static_cast<size_t>(m_lWidth) * Y + X)] / m_lNrBitmaps * 256.0;
		};
		double		GetGreenValue(int X, int Y)
		{
			if (!m_bMonochrome)
				return m_vGreenPlane[static_cast<size_t>(static_cast<size_t>(m_lWidth) * Y + X)] / m_lNrBitmaps * 256.0;
			else
				return GetRedValue(X, Y);
		};
		double		GetBlueValue(int X, int Y)
		{
			if (!m_bMonochrome)
				return m_vBluePlane[static_cast<size_t>(static_cast<size_t>(m_lWidth) * Y + X)] / m_lNrBitmaps * 256.0;
			else
				return GetRedValue(X, Y);
		};

		void	SetISOSpeed(int lISOSpeed)
		{
			m_lISOSpeed = lISOSpeed;
		};

		std::uint16_t GetISOSpeed()
		{
			return static_cast<std::uint16_t>(m_lISOSpeed);
		};

		void	SetGain(int lGain)
		{
			m_lGain = lGain;
		};

		int	GetGain()
		{
			return m_lGain;
		};

		int	GetTotalTime()
		{
			return m_lTotalTime;
		};

		int	GetNrStackedFrames()
		{
			return m_lNrBitmaps;
		};

		bool	Load(const fs::path& file, DSS::ProgressBase* pProgress = nullptr);
		void	SaveTIFF16Bitmap(const fs::path& file, const DSSRect& rect, DSS::ProgressBase* pProgress = nullptr, bool bApplySettings = true, TIFFCOMPRESSION TiffComp = TC_NONE);
		void	SaveTIFF32Bitmap(const fs::path& file, const DSSRect& rect, DSS::ProgressBase* pProgress = nullptr, bool bApplySettings = true, bool bFloat = false, TIFFCOMPRESSION TiffComp = TC_NONE);
		void	SaveFITS16Bitmap(const fs::path& file, const DSSRect& rect, DSS::ProgressBase* pProgress = nullptr, bool bApplySettings = true);
		void	SaveFITS32Bitmap(const fs::path& file, const DSSRect& rect, DSS::ProgressBase* pProgress = nullptr, bool bApplySettings = true, bool bFloat = false);
		std::shared_ptr<CMemoryBitmap> GetBitmap(DSS::ProgressBase* const pProgress = nullptr);

		void	updateQImage(uchar* pImageData, qsizetype bytes_per_line, DSSRect* pRect = nullptr);

		void Clear()
		{
			m_lNrBitmaps = 0;
			m_lHeight = 0;
			m_lWidth = 0;
			m_lOutputWidth = 0;
			m_lOutputHeight = 0;
			m_vRedPlane.clear();
			m_vGreenPlane.clear();
			m_vBluePlane.clear();
			m_lTotalTime = 0;
			m_lISOSpeed = 0;
			m_lGain = -1;
		};

		int	GetWidth()
		{
			return m_lWidth;
		};

		int	GetHeight()
		{
			return m_lHeight;
		};

		bool	IsMonochrome()
		{
			return m_bMonochrome;
		};
	};
} // namespace DSS