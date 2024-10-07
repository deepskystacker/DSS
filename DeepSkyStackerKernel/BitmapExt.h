#pragma once

#include "BitmapConstants.h"
#include "DSSCommon.h"
#include "BitmapExtraInfo.h"
#include "ColorRef.h"
#include "cfa.h"
#include "BitmapInfo.h"

namespace DSS { class ProgressBase; }
class CMemoryBitmap;
class CBitmapCharacteristics;

/* ------------------------------------------------------------------- */

void FormatFromMethod(QString& strText, MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations);
void FormatMethod(QString& strText, MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations);
bool Subtract(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, DSS::ProgressBase* pProgress = nullptr, double fRedFactor = 1.0, double fGreenFactor = 1.0, double fBlueFactor = 1.0);
bool Add(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, DSS::ProgressBase* pProgress = nullptr);
bool ShiftAndSubtract(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, DSS::ProgressBase* pProgress = nullptr, double fXShift = 0, double fYShift = 0);

bool FetchPicture(const fs::path filePath, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness,
	DSS::ProgressBase* const pProgress, std::shared_ptr<QImage>& pQImage);

bool GetPictureInfo(const fs::path& szFileName, CBitmapInfo& BitmapInfo);
std::shared_ptr<CMemoryBitmap> GetFilteredImage(const CMemoryBitmap* pInBitmap, const int lFilterSize, DSS::ProgressBase* pProgress = nullptr);

//////////////////////////////////////////////////////////////////////////

bool IsFITSRawBayer();		// From FITSUtil.h
bool IsFITSSuperPixels();	// From FITSUtil.h

namespace DSS
{
	class	GammaTransformation
	{
	private:
		std::vector<uint8_t> u8transform;
		std::vector<uint16_t> u16transform;
		constexpr static int transformSize{ 1 + std::numeric_limits<uint16_t>::max() };
		constexpr static double uint16Max_asDouble{ std::numeric_limits<uint16_t>::max() };
		bool valid;

	public:
		GammaTransformation() : valid{ false }
		{};
		~GammaTransformation() {};

		//
		// Don't intend this to be copied or assigned.
		//
		GammaTransformation(const GammaTransformation&) = delete;
		GammaTransformation& operator=(const GammaTransformation&) = delete;
		GammaTransformation(GammaTransformation&& rhs) noexcept;
		GammaTransformation& operator=(GammaTransformation&& rhs) noexcept;

		void initTransformation(double fBlackPoint, double fGrayPoint, double fWhitePoint);

		void initTransformation(double fGamma);

		uint8_t getTransformation(int index)
		{
			return u8transform[index];
		}

		uint16_t getTransformation16(int index)
		{
			return u16transform[index];
		}

		bool isInitialized() { return valid; }
	};
}

class CAllDepthBitmap
{
public:
	bool m_bDontUseAHD;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;
	std::shared_ptr<QImage> m_Image;

    CAllDepthBitmap() : m_bDontUseAHD(false) {};
	~CAllDepthBitmap() {};
	CAllDepthBitmap(const CAllDepthBitmap& adb) = default;
	CAllDepthBitmap& operator=(const CAllDepthBitmap& adb) = default;

	void Clear();
	void SetDontUseAHD(bool bSet){m_bDontUseAHD = bSet;}
	bool initQImage();
};

bool LoadPicture(const fs::path& file, CAllDepthBitmap & AllDepthBitmap, DSS::ProgressBase* pProgress = nullptr);
bool DebayerPicture(CMemoryBitmap* pInBitmap, std::shared_ptr<CMemoryBitmap>& rpOutBitmap, DSS::ProgressBase* pProgress);

bool	ApplyGammaTransformation(QImage* pImage, CMemoryBitmap* pInBitmap, DSS::GammaTransformation& gammatrans);

