#pragma once
#include "BezierAdjust.h"
#include "histogram.h"
#include "ColorRef.h"
#include "BitmapInfo.h"
#include "dssrect.h"


namespace DSS { class OldProgressBase; }
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

#pragma pack(pop, HDPIXELINFO)


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
		int m_lWidth{ 0 };
		int m_lHeight{ 0 };
		int m_lOutputWidth{ 0 };
		int m_lOutputHeight{ 0 };
		int m_lNrBitmaps{ 0 };
		std::vector<float> m_vRedPlane{};
		std::vector<float> m_vGreenPlane{};
		std::vector<float> m_vBluePlane{};
		int m_lISOSpeed{ 0 };
		int m_lGain{ -1 };
		int m_lTotalTime{ 0 };
		bool m_bMonochrome{ false };

		BezierAdjust m_BezierAdjust{};
		RGBHistogramAdjust m_HistoAdjust{};
		CBitmapInfo bmpInfo{};
	public:
		StackedBitmap();
		~StackedBitmap() = default;

	private:
		bool LoadTIFF(const fs::path& file, DSS::OldProgressBase* pProgress = nullptr);
		bool LoadFITS(const fs::path& file, DSS::OldProgressBase* pProgress = nullptr);

	//	COLORREF	GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings);

	public:
		void ReadSpecificTags(CTIFFReader* tiffReader);
		void ReadSpecificTags(CFITSReader* fitsReader);
		void WriteSpecificTags(CTIFFWriter* tiffWriter, bool bApplySettings);
		void WriteSpecificTags(CFITSWriter* fitsWriter, bool bApplySettings);

		void SetOutputSizes(int lWidth, int lHeight);
		bool Allocate(int lWidth, int lHeight, bool bMonochrome);
		void SetHistogramAdjust(const RGBHistogramAdjust& HistoAdjust);
		void SetBezierAdjust(const DSS::BezierAdjust& BezierAdjust);
		void GetBezierAdjust(DSS::BezierAdjust& BezierAdjust) const;
		void GetHistogramAdjust(RGBHistogramAdjust& HistoAdjust) const;

//		COLORREF	GetPixel(int X, int Y, bool bApplySettings = true);
//		COLORREF16	GetPixel16(int X, int Y, bool bApplySettings = true);
//		COLORREF32	GetPixel32(int X, int Y, bool bApplySettings = true);

		std::tuple<double, double, double> getValues(size_t X, size_t Y) const;
		double getValue(size_t X, size_t Y) const;
		void SetPixel(int X, int Y, double fRed, double fGreen, double fBlue);
		void GetPixel(int X, int Y, double& fRed, double& fGreen, double& fBlue, bool bApplySettings) const;

		const auto& getRedPixels() const { return this->m_vRedPlane; }
		const auto& getGreenPixels() const { return this->m_vGreenPlane; }
		const auto& getBluePixels() const { return this->m_vBluePlane; }

		double GetRedValue(int X, int Y) const;
		double GetGreenValue(int X, int Y) const;
		double GetBlueValue(int X, int Y) const;
		void SetISOSpeed(int lISOSpeed);
		std::uint16_t GetISOSpeed() const;
		void SetGain(int lGain);
		int GetGain() const;
		int GetTotalTime() const;
		int	GetNrStackedFrames() const;

		bool Load(const fs::path& file, DSS::OldProgressBase* pProgress = nullptr);
		void SaveTIFF16Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bApplySettings = true, TIFFCOMPRESSION TiffComp = TC_NONE);
		void SaveTIFF32Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bApplySettings = true, bool bFloat = false, TIFFCOMPRESSION TiffComp = TC_NONE);
		void SaveFITS16Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bApplySettings = true);
		void SaveFITS32Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bApplySettings = true, bool bFloat = false);
		std::shared_ptr<CMemoryBitmap> GetBitmap(DSS::OldProgressBase* const pProgress = nullptr);

		void updateQImage(uchar* pImageData, qsizetype bytes_per_line, DSSRect* pRect = nullptr) const;

		void Clear();
		int	GetWidth() const;
		int	GetHeight() const;
		bool IsMonochrome() const;
	};

} // namespace DSS