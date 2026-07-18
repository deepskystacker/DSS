#pragma once
#include "DSSCommon.h"
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
	//
// Define a structure to hold the parameters for the MTF autostretch, which can be used to store the computed
// shadow (black point), midtone, and highlight (white point) values for each channel (R=0, G=1, B=2).
// 
	struct MTFStretchParameters
	{
		float clipPoint[3];
		float midtoneBalance[3];
		float whitePoint[3];
	};

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

		BitmapInfo bmpInfo{};
	public:
		StackedBitmap();
		~StackedBitmap() = default;
		StackedBitmap(const StackedBitmap& rhs) = default;
		StackedBitmap(StackedBitmap&& rhs) = default;

		StackedBitmap& operator = (const StackedBitmap& rhs) = default;
		StackedBitmap& operator = (StackedBitmap&& rhs) = default;

	private:
		bool LoadTIFF(const fs::path& file, DSS::OldProgressBase* pProgress = nullptr);
		bool LoadFITS(const fs::path& file, DSS::OldProgressBase* pProgress = nullptr);

	//	COLORREF	GetPixel(float fRed, float fGreen, float fBlue, bool bApplySettings);

	public:
		void ReadSpecificTags(CTIFFReader* tiffReader);
		void ReadSpecificTags(CFITSReader* fitsReader);
		void WriteSpecificTags(CTIFFWriter* tiffWriter);
		void WriteSpecificTags(CFITSWriter* fitsWriter);

		void SetOutputSizes(int lWidth, int lHeight);
		bool Allocate(int lWidth, int lHeight, bool bMonochrome);

//		COLORREF	GetPixel(int X, int Y, bool bApplySettings = true);
//		COLORREF16	GetPixel16(int X, int Y, bool bApplySettings = true);
//		COLORREF32	GetPixel32(int X, int Y, bool bApplySettings = true);

		std::tuple<double, double, double> getValues(size_t X, size_t Y) const;
		std::tuple<double, double, double> getValues(size_t offset) const;
		double getValue(size_t X, size_t Y) const;
		double getValue(size_t offset) const;
		void SetPixel(int X, int Y, double fRed, double fGreen, double fBlue);
		void GetPixel(int X, int Y, double& fRed, double& fGreen, double& fBlue) const;

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
		void SaveTIFF16Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, TIFFCOMPRESSION TiffComp = TC_NONE);
		void SaveTIFF32Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bFloat = false, TIFFCOMPRESSION TiffComp = TC_NONE);
		void SaveFITS16Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bApplySettings = true);
		void SaveFITS32Bitmap(const fs::path& file, const DSSRect& rect, DSS::OldProgressBase* pProgress = nullptr, bool bFloat = false);
		std::shared_ptr<CMemoryBitmap> GetBitmap(DSS::OldProgressBase* const pProgress = nullptr);

		void updateQImage(uchar* pImageData, qsizetype bytes_per_line, DSSRect& rect) const;

		//
		// Normalise the image data to a range of [0.0, 1.0], which is required for
		// the ASinH stretch and colour balance processing 
		//
		// Source is in asinhstretch.cpp
		//
		void normalise();

		//
		// The asinh, or inverse hyperbolic sine, stretch is a non-linear stretch that can be used to bring out
		// faint details in an image while preserving the overall structure and color balance.
		// 
		// The beta parameter controls the strength of the stretch, with higher values resulting in a more
		// pronounced stretch.
		// 
		// The offset parameter can be used to adjust the point at which the stretch begins (i.e the black point),
		// allowing for further fine-tuning of the final image.
		//
		// The human_luminance option allows the stretch to be applied in a way that preserves the perceived
		// luminance of the image, which can help to maintain a more natural appearance.
		//
		// Source is in asinhstretch.cpp
		//
		void asinhStretch(float beta, float offset, bool human_luminance);

		//
		// Adjust the image colour balance according to the values of:
		//   
		//   redShift
		//   greenShift
		//   blueShift
		// 
		// Whose values have a range of [-1.0, 1.0]
		// 
		// The image data is expected to have been normalised to the range [0, 1.0]
		// 
		// The function does nothing if the image is monochrome, as colour balance
		// adjustments are not applicable to monochrome images.
		// 
		// Source is in colourbalance.cpp
		//
		void adjustColourBalance(float redShift, float greenShift, float blueShift);

		//
		// De-normalise the image data after the ASinH stretch and colour balance processing, to bring it back to the
		// normal range of pixel values.
		//
		// Source is in asinhstretch.cpp
		//
		void deNormalise();

		//
		// Adjust saturation allows adjustment of both overall colour saturation and also vibrance which allows
		// low colour saturation areas of the image to have the colour saturation boosted.
		// As with the other adjustments, the image data is expected to have been normalised to the range [0.0, 1.0]
		//
		// Source is in saturation.cpp
		//
		void adjustSaturation(float saturation, float vibrance);

		// 
		//
		// The MTF (Midtone Transfer Function) autostretch is a non-linear stretch that automatically 
		// evaluates image statistics to determine the optimal black point and midtone balance,
		// making faint detail visible while preserving overall structure.
		//
		// The 'linked' parameter controls whether the stretch is applied identically across all
		// channels to preserve colour balance, or independently per channel to neutralise colour casts.
		//
		// The image data is expected to have been normalised to the range [0.0, 1.0]
		//
		// Source is in mtfstretch.cpp
		//
		void mtfComputeAutoStretchParameters(bool linked, MTFStretchParameters& parameters, float targetMedian = 0.125f, float shadowClipFactor = 2.8f);


		//
		// Apply a manual MTF stretch using specific shadow (black point), midtone, and highlight (white point)
		// parameters, similar to a Levels adjustment.
		//
		// Source is in mtfstretch.cpp
		//
		void mtfStretch(const MTFStretchParameters& parameters);

		void Clear();
		int	GetWidth() const;
		int	GetHeight() const;
		inline bool isMonochrome() const
		{
			return m_bMonochrome;
		}


	};

} // namespace DSS
