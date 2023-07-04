#include "stdafx.h"
#include "RAWUtils.h"
#include "libraw/libraw.h"
#include "Ztrace.h"
#include "Workspace.h"
#include "DSSProgress.h"
//#include "resource.h"
#include "MemoryBitmap.h"
#include "Multitask.h"
#include "MedianFilterEngine.h"
#include "ZExcBase.h"
#include "BitmapInfo.h"

// #include <zexcept.h>
// #include <ztrace.h>
// 
// #include "resource.h"
// #include "BitmapExt.h"
// #include "DSSTools.h"
// #include "DSSProgress.h"
// 
// #include "RAWUtils.h"
// #include "Multitask.h"
// #include "Workspace.h"
// 
// #include "libraw/libraw.h"

using namespace DSS;

namespace {
	class Timer
	{
	private:
		std::chrono::steady_clock::time_point last;
	public:
		Timer()
			: last{ std::chrono::steady_clock::now() } {
		}
		void printDiff(const std::string& msg = "Timer diff: ") {
			auto now{ std::chrono::steady_clock::now() };
			std::chrono::duration<double, std::milli> diff{ now - last };
			ZTRACE_RUNTIME("%s\t%fms", msg.c_str(), diff.count());
			last = std::chrono::steady_clock::now();
		}
		void reset()
		{
			last = std::chrono::steady_clock::now();
		}
	};

	class CRAWSettings
	{
		public :
			bool		m_bRawBayer;
			bool		m_bSuperPixel;
	
		private:
			void CopyFrom(const CRAWSettings& rs) noexcept
			{
				m_bRawBayer = rs.m_bRawBayer;
				m_bSuperPixel = rs.m_bSuperPixel;
			}
	
		public:
			CRAWSettings() noexcept
			{
				m_bRawBayer = false;
				m_bSuperPixel = false;
			}
			CRAWSettings(const bool rawBay, const bool superPix) :
				m_bRawBayer{ rawBay },
				m_bSuperPixel{ superPix }
			{}
	
			CRAWSettings(const CRAWSettings& rs) = default;
	
			const CRAWSettings& operator=(const CRAWSettings& rs)
			{
				CopyFrom(rs);
				return (*this);
			}
		};

	std::list<CRAWSettings> g_RawSettingsStack;

}

/* ------------------------------------------------------------------- */

static bool IsRegistrySuperPixels()
{
	return Workspace{}.value("RawDDP/SuperPixels", false).toBool();
};

/* ------------------------------------------------------------------- */

static bool IsRegistryRawBayer()
{
	return Workspace{}.value("RawDDP/RawBayer", false).toBool();
};

/* ------------------------------------------------------------------- */

bool IsSuperPixels()
{
	if (!g_RawSettingsStack.empty())
		return g_RawSettingsStack.back().m_bSuperPixel;
	else
		return IsRegistrySuperPixels();
};

/* ------------------------------------------------------------------- */

bool IsRawBayer()
{
	if (!g_RawSettingsStack.empty())
		return g_RawSettingsStack.back().m_bRawBayer;
	else
		return IsRegistryRawBayer();
};

/* ------------------------------------------------------------------- */

bool IsRawBilinear()
{
	const QString strInterpolation = Workspace{}.value("RawDDP/Interpolation", {}).toString();
	return strInterpolation.isEmpty() || (strInterpolation == "Bilinear");
}

/* ------------------------------------------------------------------- */

bool IsRawAHD()
{
	const QString strInterpolation = Workspace{}.value("RawDDP/Interpolation", {}).toString();
	return (strInterpolation.isEmpty() || (strInterpolation == "AHD"));
}

/* ------------------------------------------------------------------- */

void PushRAWSettings(bool bSuperPixel, bool bRawBayer)
{
	if (IsRegistryRawBayer() || IsRegistrySuperPixels())
	{
		CRAWSettings rs;
		rs.m_bRawBayer = bRawBayer;
		rs.m_bSuperPixel = bSuperPixel;

		g_RawSettingsStack.push_back(rs);
	};
};

/* ------------------------------------------------------------------- */

void PopRAWSettings()
{
	if (!g_RawSettingsStack.empty())
		g_RawSettingsStack.pop_back();
};

/* ------------------------------------------------------------------- */

#include "BitMapFiller.h"

#define Thread   __declspec( thread )

namespace { // Only use in this .cpp file

	static Thread ProgressBase* g_Progress;

	class DSSLibRaw : public LibRaw
	{
	public:
		DSSLibRaw() = default;
		~DSSLibRaw() = default;

		void setBitMapFiller(BitmapFillerInterface* pFiller) noexcept
		{
			pDSSBitMapFiller = pFiller;
		};

		int dcraw_ppm_tiff_writer(const char* filename);
		inline unsigned get_fuji_layout() noexcept
		{
			return libraw_internal_data.unpacker_data.fuji_layout;
		};

		void adjust_bl() { this->LibRaw::adjust_bl(); }
		virtual int is_phaseone_compressed() noexcept override {
			return LibRaw::is_phaseone_compressed();
		}

	protected:
		void write_ppm_tiff();

	private:
		BitmapFillerInterface* pDSSBitMapFiller = nullptr;
	};

	/* ------------------------------------------------------------------- */

	class CRawDecod
	{
	private:
		std::unique_ptr<DSSLibRaw> pLibRaw;
		DSSLibRaw& rawProcessor;

		fs::path		file;
		QString			m_strModel;
		QString			m_strMake;
		int				m_lISOSpeed;
		double			m_fExposureTime;
		double			m_fAperture;
		int				m_lHeight;
		int				m_lWidth;
		bool			m_bColorRAW;
		CFATYPE			m_CFAType;
		QDateTime		m_DateTime;
		bool            m_isRawFile;

#define P1		rawProcessor.imgdata.idata
#define P2		rawProcessor.imgdata.other

#define mnLens rawProcessor.imgdata.lens.makernotes
#define exifLens rawProcessor.imgdata.lens
#define ShootingInfo rawProcessor.imgdata.shootinginfo

#define S		rawProcessor.imgdata.sizes
#define O		rawProcessor.imgdata.params
#define C		rawProcessor.imgdata.color
#define T		rawProcessor.imgdata.thumbnail

#define Canon	rawProcessor.imgdata.makernotes.canon
#define Fuji	rawProcessor.imgdata.makernotes.fuji
#define Oly		rawProcessor.imgdata.makernotes.olympus

#define RawData	rawProcessor.imgdata.rawdata
#define IOParams	rawProcessor.imgdata.rawdata.ioparams

	public:
		CRawDecod(fs::path path) :
			pLibRaw {std::make_unique<DSSLibRaw>()},
			rawProcessor{ *pLibRaw },
			file{ path },
			m_bColorRAW{ false },
			m_CFAType{ CFATYPE_NONE },
			m_lISOSpeed{ 0 },
			m_fExposureTime{ 0.0 },
			m_fAperture{ 0.0 },
			m_lHeight{ 0 },
			m_lWidth{ 0 },
			m_isRawFile{ rawProcessor.open_file(file.wstring().c_str()) == LIBRAW_SUCCESS }
		{
			ZFUNCTRACE_RUNTIME();

			if (m_isRawFile)
			{
				m_strMake = P1.normalized_make;
				m_strModel = P1.normalized_model;
				m_lHeight = S.iheight;
				m_lWidth = S.iwidth;
				m_lISOSpeed = P2.iso_speed;

				m_fExposureTime = std::isfinite(P2.shutter) ? P2.shutter : 0.0;
				m_fAperture = std::isfinite(P2.aperture) ? P2.aperture : 0.0;

				// Retrieve the Date/Time
				if (P2.timestamp)
				{
					m_DateTime = QDateTime::fromSecsSinceEpoch(P2.timestamp);
				}

				m_bColorRAW = P1.is_foveon || !(P1.filters);
				if (1 == P1.filters || 9 == P1.filters)
				{
					//
					// This is somewhat of a lie as the only the Foveon sensors
					// create full color raw files.  However by telling this lie
					// we can use libraw to decode and interpolate Fujitsu X-Trans
					// and Leaf Catchlight images
					//
					if (1 == P1.filters)
						ZTRACE_RUNTIME("Image is from a Leaf Catchlight");
					else
						ZTRACE_RUNTIME("Image is from a Fujitsu X-Trans Sensor");
					m_bColorRAW = true;
				}

				m_CFAType = GetCurrentCFAType();
			};
		}

		virtual ~CRawDecod()
		{
			ZFUNCTRACE_RUNTIME();
			rawProcessor.recycle();
		};

		bool IsRawFile() const;
		bool LoadRawFile(CMemoryBitmap* pBitmap, const bool ignoreBrightness, ProgressBase* pProgress);

		QString getModel()
		{
			return (m_strMake + " " + m_strModel);
		};

		void checkCameraSupport(const QString& strModel);

		int	GetISOSpeed() noexcept
		{
			return m_lISOSpeed;
		};

		double GetExposureTime() noexcept
		{
			return m_fExposureTime;
		};

		double getAperture() noexcept
		{
			return m_fAperture;
		};

		int Width() noexcept
		{
			return m_lWidth;
		};

		int Height() noexcept
		{
			return m_lHeight;
		};

		bool IsColorRAW() noexcept
		{
			return m_bColorRAW;
		};

		CFATYPE GetCFAType()
		{
			return m_CFAType;
		};

		CFATYPE GetCurrentCFAType()
		{
			if (P1.colors == 4 && IOParams.mix_green == 0) // CYMG Bayer pattern
			{
				switch (P1.filters)
				{
				case 0xb4b4b4b4: return CFATYPE_GMYC;
				case 0xe1e4e1e4: return CFATYPE_GMCYMGCY;
				case 0x1e4e1e4e: return CFATYPE_CYGMCYMG;
				}
			}
			else // RGB Bayer Pattern
			{
				switch (P1.filters)
				{
				case 0x1e1e1e1e:
				case 0x16161616: return CFATYPE_BGGR;
				case 0xe1e1e1e1:
				case 0x61616161: return CFATYPE_GRBG;
				case 0x4b4b4b4b:
				case 0x49494949: return CFATYPE_GBRG;
				case 0xb4b4b4b4:
				case 0x94949494: return CFATYPE_RGGB;
				default: return CFATYPE_NONE;
				}
			}

			return CFATYPE_NONE;
		};

		/* ------------------------------------------------------------------- */

		QDateTime GetDateTime()
		{
			return m_DateTime;
		};
	};

	void CRawDecod::checkCameraSupport(const QString& strModel)
	{
		bool result = false;
		const std::string cameraString{ strModel.toLatin1().constData() };

		static std::set<std::string> checkedCameras;

		//
		// If we've already checked this camera type, then just bail out so
		// complaints about unsupported cameras are only issued once.
		//
		if (checkedCameras.find(cameraString) != checkedCameras.end())
			return;

		static std::vector<std::string> supportedCameras;

		if (supportedCameras.empty())
		{
			const char** cameraList = rawProcessor.cameraList();
			const size_t count = rawProcessor.cameraCount();
			supportedCameras.reserve(count);

			//
			// Copy LibRaw's supported camera list
			//
			for (size_t i = 0; i < count; ++i)
			{
				if (nullptr != cameraList[i])
				{
					supportedCameras.emplace_back(cameraList[i]);
				}
			}

			//
			// Sort the camera names using std::sort and a case independent comparison lambda function
			// See: https://stackoverflow.com/questions/33379846/case-insensitive-sorting-of-an-array-of-strings
			//
			sort(supportedCameras.begin(), supportedCameras.end(),
				[](const auto& lhs, const auto& rhs)
				{
					const auto result = mismatch(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
						[](const auto& lhs, const auto& rhs) { return tolower(lhs) == tolower(rhs); }
					);
					return result.second != rhs.cend() && (result.first == lhs.cend() || tolower(*result.first) < tolower(*result.second));
				});
		}

		//
		// The camera type hasn't already been checked, so search the LibRaw supported camera list
		//
		result = std::binary_search(supportedCameras.begin(), supportedCameras.end(), cameraString,
			[](const std::string& lhs, const std::string& rhs) noexcept
			{
				const char* pclhs = lhs.c_str();
				const char* pcrhs = rhs.c_str();
				// choose the shorter length
				const size_t len = std::min(strlen(pclhs), strlen(pcrhs));
				return _strnicmp(pclhs, pcrhs, len) < 0;
			}
		);

		//
		// Now we know whether this camera is supported or not, remember we've seen it before
		//
		checkedCameras.insert(cameraString);

		//
		// If the camera isn't supported complain, but only once
		//
		if (false == result)
		{
			const QString errorMessage(QCoreApplication::translate("RawUtils", "Sorry, LibRaw doesn't support your %1 camera", "IDS_CAMERA_NOT_SUPPORTED").arg(strModel));
			DSSBase::instance()->reportError(errorMessage, "");
		}

		return;
	};
	/* ------------------------------------------------------------------- */


	bool CRawDecod::LoadRawFile(CMemoryBitmap* pBitmap, const bool ignoreBrightness, ProgressBase* pProgress)
	{
		ZFUNCTRACE_RUNTIME();

		bool bResult = true;
		int ret = 0;

		pBitmap->Init(m_lWidth, m_lHeight);
		pBitmap->SetISOSpeed(m_lISOSpeed);
		pBitmap->SetExposure(m_fExposureTime);
		pBitmap->SetAperture(m_fAperture);
		pBitmap->m_DateTime = m_DateTime;

		QString strDescription{ getModel() };

		//
		// If it's a DNG file, we don't need to check for camera support, but if
		// we're processing a true raw file then check that the camera is supported.
		//
		if (0 == P1.dng_version)
		{
			checkCameraSupport(strDescription);
		};

		pBitmap->SetDescription(strDescription);

		// const int maxargs = 50;
		Workspace workspace;
		const double fGreenScale = ignoreBrightness ? 1.0 : workspace.value("RawDDP/Brightness").toDouble();
		const double fRedScale = ignoreBrightness ? 1.0 : (fGreenScale * workspace.value("RawDDP/RedScale").toDouble());
		const double fBlueScale = ignoreBrightness ? 1.0 : (fGreenScale * workspace.value("RawDDP/BlueScale").toDouble());

		do	// Do once!
		{
			const int numberOfProcessors = CMultitask::GetNrProcessors();

			//bSuperPixels = IsSuperPixels();
			//bRawBayer    = IsRawBayer();
			//bBilinear	 = !bSuperPixels && !bRawBayer && IsRawBilinear();
			//bAHD		 = !bSuperPixels && !bRawBayer && IsRawAHD();

			// No Auto White Balance
			O.use_auto_wb = false;

			//
			// Do we disable all White Balance processing?
			//
			const auto setWbMult = [&](const float factor)
			{
				O.user_mul[0] = factor;
				O.user_mul[1] = factor;
				O.user_mul[2] = factor;
				O.user_mul[3] = factor;
			};
			if (workspace.value("RawDDP/NoWB", false).toBool())
				setWbMult(1.0f); // Yes, so set the user white balance multipliers to 1.0
			else
				setWbMult(0.0f); // No, so set the user white balance multipliers to 0.0

			O.use_camera_wb = workspace.value("RawDDP/CameraWB", false).toBool() ? 1 : 0;

			// Don't stretch or rotate raw pixels (equivalent to dcraw -j)
			O.use_fuji_rotate = 0;

			// Don't flip the image (equivalent to dcraw -t 0)
			O.user_flip = 0;

			// Output color space : raw-> sRGB (default)
			/*
			argv[argc] = _T("-o");
			argc++;
			argv[argc] = _T("0");
			argc++;*/

			O.user_black = workspace.value("RawDDP/BlackPointTo0", false).toBool() ? 0 : -1;

			// Output is 16 bits (equivalent of dcraw flag -4)
			O.gamm[0] = O.gamm[1] = O.no_auto_bright = 1;
			O.output_bps = 16;

			g_Progress = pProgress;

			ZTRACE_RUNTIME("Calling LibRaw::unpack()");
			if ((ret = rawProcessor.unpack()) != LIBRAW_SUCCESS)
			{
				bResult = false;
				ZTRACE_RUNTIME("Cannot unpack %s: %s", file.generic_u8string().c_str(), libraw_strerror(ret));
			}
			if (!bResult)
				break;

			//
			// Create the class that populates the bitmap
			//
			CopyableSmartPtr<BitmapFillerInterface> pFiller = BitmapFillerInterface::makeBitmapFiller(pBitmap, pProgress, fRedScale, fGreenScale, fBlueScale);
			// Get the Colour Filter Array type and set into the bitmap filler
			m_CFAType = GetCurrentCFAType();
			pFiller->SetCFAType(m_CFAType);

#define RAW(row,col) raw_image[(row) * S.width + (col)]

			//
			// Get our endian-ness so we can swap bytes if needed (always on Windows).
			//
			const bool littleEndian = htons(0x55aa) != 0x55aa; // big_endian = htons(host_byte_order)

			if (!m_bColorRAW)
			{
				ZTRACE_DEVELOP("Processing Bayer pattern raw image data");
				//
				// The initial openmp changes were made by David Partridge, but it was
				// Vitali Pelenjow who made it work without the critical sections
				// killing the performance.
				//
				// Set up a temporary image array of unsigned short that will be:
				//
				// 1) Filled in from the Fujitsu Super-CCD image array in
				//    Rawdata.raw_image, or
				//
				// 2) will be copied from the image portion of Rawdata.raw_image
				//    excluding the frame (Top margin, Left Margin).
				//
				std::vector<std::uint16_t> dataBuffer;
				try {
					dataBuffer.resize(static_cast<size_t>(S.height) * static_cast<size_t>(S.width), 0);
				}
				catch (...) {
					ZOutOfMemory e("Could not allocate storage for RAW image");
					ZTHROW(e);
				}
				std::uint16_t* raw_image = dataBuffer.data();
				void* buffer = nullptr; // Used for debugging only (memory window)

				const int fuji_width = rawProcessor.is_fuji_rotated();
				const unsigned fuji_layout = rawProcessor.get_fuji_layout();
				buffer = raw_image;		// only for memory window debugging

				if (fuji_width)   // Are we processing a Fuji Super-CCD image?
				{
					ZTRACE_RUNTIME("Converting Fujitsu Super-CCD image to regular raw image");

#pragma omp parallel for default(none) schedule(dynamic, 50) if(numberOfProcessors > 1)
					for (int row = 0; row < S.raw_height - S.top_margin * 2; row++)
					{
						for (int col = 0; col < fuji_width << int(!fuji_layout); col++)
						{
							unsigned r, c;

							if (fuji_layout)
							{
								r = fuji_width - 1 - col + (row >> 1);
								c = col + ((row + 1) >> 1);
							}
							else
							{
								r = fuji_width - 1 + row - (col >> 1);
								c = row + ((col + 1) >> 1);
							}
							if (r < S.height && c < S.width)
								RAW(r, c) = RawData.raw_image[(row + S.top_margin) * S.raw_pitch / 2 + (col + S.left_margin)];
						}
					}
				}
				else
				{
					ZTRACE_DEVELOP("Extracting real image data (excluding the frame) from RawData.raw_image");

					//
					// This is a regular RAW file so no Fuji Super-CCD stuff
					//
					// Just copy the "real image" portion of the data excluding
					// the frame
					//
					buffer = raw_image;

#pragma omp parallel for default(none) schedule(dynamic, 50) if(numberOfProcessors > 1)
					for (int row = 0; row < S.height; row++)
					{
						for (int col = 0; col < S.width; col++)
						{
							RAW(row, col) = RawData.raw_image[(row + S.top_margin) * S.raw_pitch / 2 + (col + S.left_margin)];
						}
					}
				}

				//
				// Now process the data that raw_image points to which is either
				//
				// 1) The output of post processing the Fuji Super-CCD raw,
				//    stored in the USHORT array hung off raw_image, or
				//
				// 2) Normal common or garden raw Bayer matrix data that's been
				//    copied from RawData.raw_image to raw_image (less the frame)
				//
				// Either way we should now be processing a regular greyscale 16-bit
				// pixel array which has an associated Bayer Matrix
				//
				pFiller->setGrey(true);
				pFiller->setWidth(S.width);
				pFiller->setHeight(S.height);
				pFiller->setMaxColors((1 << 16) - 1);

				// Report User Black Point over-ride
				if (0 == O.user_black)
					ZTRACE_RUNTIME("User set Black Point to 0");

				//
				// Before doing dark subtraction, normalise C.black / C.cblack[]
				//
				ZTRACE_DEVELOP("Before adjust_bl() C.black = %d.", C.black);
				ZTRACE_DEVELOP("First 10 C.cblack elements\n  %d, %d, %d, %d\n  %d, %d\n  %d, %d, %d, %d",
					C.cblack[0], C.cblack[1], C.cblack[2], C.cblack[3],
					C.cblack[4], C.cblack[5],
					C.cblack[6], C.cblack[7], C.cblack[8], C.cblack[9]);
				rawProcessor.adjust_bl();

				//
				// This code is based on code from LibRaw Version 19.2, specifically method:
				//
				//   int LibRaw::subtract_black_internal()
				//
				// found at line 4532 in source file libraw_cxx.cpp
				//
				// Do dark subtraction on the image.   If a user defined black level has
				// been set (it will be zero) then use that, otherwise just use the black
				// level for the camera.
				//
				// Note that this is only done on real image data, not the frame
				//
				// While doing so collect the largest value in the image data.
				//
				ZTRACE_DEVELOP("Subtracting black level of C.black = %d from raw_image data.", C.black);
				ZTRACE_DEVELOP("First 10 C.cblack elements\n  %d, %d, %d, %d\n  %d, %d\n  %d, %d, %d, %d",
					C.cblack[0], C.cblack[1], C.cblack[2], C.cblack[3],
					C.cblack[4], C.cblack[5],
					C.cblack[6], C.cblack[7], C.cblack[8], C.cblack[9]);

				const int size = static_cast<int>(S.height) * static_cast<int>(S.width);

				if (!rawProcessor.is_phaseone_compressed() && (C.cblack[0] || C.cblack[1] || C.cblack[2] || C.cblack[3] || (C.cblack[4] && C.cblack[5])))
				{
					const int cblk[4] = { static_cast<int>(C.cblack[0]), static_cast<int>(C.cblack[1]), static_cast<int>(C.cblack[2]), static_cast<int>(C.cblack[3]) };

					int dmax = 0;	// Maximum value of pixels in entire image.
					int lmax = 0;	// Local (or Loop) maximum value found in the 'for' loops below. For OMP.
					if (C.cblack[4] && C.cblack[5])
					{
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax) if(numberOfProcessors > 1)
						{
#pragma omp for schedule(dynamic, 1'000'000) 
							for (int i = 0; i < size; i++)
							{
								int val = raw_image[i];
								val -= C.cblack[6 + i / S.width % C.cblack[4] * C.cblack[5] + i % S.width % C.cblack[5]];
								val -= cblk[i & 3];
								raw_image[i] = static_cast<std::uint16_t>(std::clamp(val, 0, 65535));
								lmax = std::max(val, lmax);
							}
#pragma omp critical
							dmax = std::max(lmax, dmax); // For non-OMP case this is equal to dmax = lmax.
						}
					}
					else
					{
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax) if(numberOfProcessors > 1)
						{
#pragma omp for schedule(dynamic, 1'000'000) 
							for (int i = 0; i < size; i++)
							{
								int val = raw_image[i];
								val -= cblk[i & 3];
								raw_image[i] = static_cast<std::uint16_t>(std::clamp(val, 0, 65535));
								lmax = std::max(val, lmax);
							}
#pragma omp critical
							dmax = std::max(lmax, dmax); // For non-OMP case this is equal to dmax = lmax.
						}
					}
					C.data_maximum = dmax & 0xffff;
					C.maximum -= C.black;
					memset(&C.cblack, 0, sizeof(C.cblack)); // Yeah, we used cblack[6+] values too!
					C.black = 0;
				}
				else
				{
					// Nothing to Do, maximum is already calculated, black level is 0, so no change
					// only calculate channel maximum;
					int dmax = 0;	// Maximum value of pixels in entire image.
					int lmax = 0;	// Local (or Loop) maximum value found in the 'for' loop below. For OMP.
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax) if(numberOfProcessors > 1)
					{
#pragma omp for schedule(dynamic, 1'000'000) 
						for (int i = 0; i < size; i++)
							if (lmax < raw_image[i])
								lmax = raw_image[i];
#pragma omp critical
						dmax = std::max(lmax, dmax); // For non-OMP case this is equal to dmax = lmax.
					}

					C.data_maximum = dmax;
				}

				//
				// The image data needs to be scaled to the "white balance co-efficients"
				// Currently do not handle "Auto White Balance"
				//
				float pre_mul[4] = { 0.0, 0.0, 0.0, 0.0 };
				if (1.0 == O.user_mul[0])
				{
					ZTRACE_RUNTIME("No White Balance processing.");
					memcpy(pre_mul, O.user_mul, sizeof pre_mul);
				}
				else if (1 == O.use_camera_wb && -1 != C.cam_mul[0])
				{
					ZTRACE_RUNTIME("Using Camera White Balance (as shot).");
					memcpy(pre_mul, C.cam_mul, sizeof pre_mul);
				}
				else
				{
					ZTRACE_RUNTIME("Using Daylight White Balance.");
					memcpy(pre_mul, C.pre_mul, sizeof pre_mul);
				}
				ZTRACE_RUNTIME("White balance co-efficients being used are %f, %f, %f, %f",
					pre_mul[0], pre_mul[1], pre_mul[2], pre_mul[3]);

#if (0)
				qDebug() << "Colour Correction Matrix:";
				for (int c = 0; c < 3; c++)
				{
					qDebug() << " "
						<< C.ccm[c][0] << " "
						<< C.ccm[c][1] << " "
						<< C.ccm[c][2] << " "
						<< C.ccm[c][3];
				}
#endif

				if (0 == pre_mul[3]) pre_mul[3] = P1.colors < 4 ? pre_mul[1] : 1;

				//
				// Now apply a linear stretch to the raw data, scale to the "saturation" level
				// not to the value of the pixel with the greatest value (which may be higher
				// than the saturation level).
				//

				// const double dmax = *std::max_element(&pre_mul[0], &pre_mul[4]);
				const double dmin = *std::min_element(&pre_mul[0], &pre_mul[4]);

				float scale_mul[4];
				for (int c = 0; c < 4; c++)
					scale_mul[c] = (pre_mul[c] /= dmin) * (65535.0 / C.maximum);

				ZTRACE_DEVELOP("Maximum value pixel has value %d", C.data_maximum);
				ZTRACE_DEVELOP("Saturation level is %d", C.maximum);
				ZTRACE_DEVELOP("Applying linear stretch to raw data.  Scale values %f, %f, %f, %f",
					scale_mul[0], scale_mul[1], scale_mul[2], scale_mul[3]);

				//Timer timer;
#pragma omp parallel default(none) if(numberOfProcessors > 1) // No OPENMP: 240ms, with OPENMP: 92ms, schedule static: 78ms, schedule dynamic: 35ms
				{
#pragma omp master // There is no implied barrier.
					ZTRACE_RUNTIME("RAW file processing with %d OpenMP threads, little_endian is %s", omp_get_num_threads(), littleEndian ? "true" : "false");
#pragma omp for schedule(dynamic, 50)
					for (int row = 0; row < S.height; row++)
					{
						for (int col = 0; col < S.width; col++)
						{
							// What colour will this pixel become
							const int colour = rawProcessor.COLOR(row, col);
							const float val = scale_mul[colour] * static_cast<float>(RAW(row, col));
							RAW(row, col) = static_cast<std::uint16_t>(std::clamp(static_cast<int>(val), 0, 65535));
						}
					}
				}
				//timer.printDiff(); 

				// Convert raw data to big-endian
				if (littleEndian)
#pragma omp parallel for default(none) schedule(static, 1'000'000) if(numberOfProcessors > 1)
					for (int i = 0; i < size; i++)
					{
						raw_image[i] = _byteswap_ushort(raw_image[i]);
					}

				const int imageWidth = S.width;
				const int imageHeight = S.height;
				const bool bitmapFillerIsThreadSafe = pFiller->isThreadSafe();
#pragma omp parallel for default(none) schedule(dynamic, 50) firstprivate(pFiller) if(numberOfProcessors > 1 && bitmapFillerIsThreadSafe)
				for (int row = 0; row < imageHeight; ++row)
				{
					// Write raw pixel data into our private bitmap format
					pFiller->Write(&raw_image[row * imageWidth], sizeof(unsigned short), imageWidth, row); // Gray, 16 bits per pixel.
				}
			}
			else
			{
				//
				// This is a "full colour" RAW file, so we can use full libraw
				// processing and capture the PPM file output.
				//
				if (LIBRAW_SUCCESS != (ret = rawProcessor.dcraw_process()))
				{
					ZTRACE_RUNTIME("Cannot do postprocessing on %s: %s", file.generic_u8string().c_str(), libraw_strerror(ret));
					if (LIBRAW_FATAL_ERROR(ret))
						bResult = false;
				}
				if (!bResult)
					break;

				ZTRACE_RUNTIME("Processing Foveon or Fuji X-Trans raw image data");
				//
				// Now capture the output using our over-ridden methods
				//
				// Set up the intercept code to write the image data to our bitmap instead of
				// to an external file, and invoke the overridden dcraw_ppm_tiff_writer()
				//
				rawProcessor.setBitMapFiller(pFiller.get());
				if (LIBRAW_SUCCESS != (ret = rawProcessor.dcraw_ppm_tiff_writer("")))
				{
					bResult = false;
					ZTRACE_RUNTIME("Cannot write image data to bitmap %s", libraw_strerror(ret));
				}
			}
#undef RAW

		} while (0);

		g_Progress = nullptr;

		return bResult;
	}

	/* ------------------------------------------------------------------- */

	bool CRawDecod::IsRawFile() const
	{
		return m_isRawFile;
	}

}

/* ------------------------------------------------------------------- */

bool IsRAWPicture(const fs::path& szFileName, QString& strModel)
{
	ZFUNCTRACE_RUNTIME();

    CRawDecod dcr(szFileName);
    bool bResult = dcr.IsRawFile();

	if (bResult)
	{
		strModel = dcr.getModel();
	}

	return bResult;
};

/* ------------------------------------------------------------------- */

bool IsRAWPicture(const fs::path& path, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	CRawDecod dcr(path);
	bResult = dcr.IsRawFile();

	if (bResult)
	{
		BitmapInfo.m_strFileName	 = path;
		BitmapInfo.m_strFileType	 = "RAW";
		if (dcr.IsColorRAW())
			BitmapInfo.m_CFAType	 = CFATYPE_NONE;
		else
			BitmapInfo.m_CFAType	 = dcr.GetCFAType();
		BitmapInfo.m_lWidth			 = dcr.Width();
		BitmapInfo.m_lHeight		 = dcr.Height();
		BitmapInfo.m_lBitsPerChannel  = 16;
		BitmapInfo.m_lNrChannels	 = dcr.IsColorRAW() ? 3 : 1;
		BitmapInfo.m_bCanLoad		 = true;
		BitmapInfo.m_strModel = dcr.getModel();
		BitmapInfo.m_lISOSpeed		 = dcr.GetISOSpeed();
		BitmapInfo.m_fExposure		 = dcr.GetExposureTime();
		BitmapInfo.m_fAperture       = dcr.getAperture();
		BitmapInfo.m_DateTime		 = dcr.GetDateTime();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool LoadRAWPicture(const fs::path& file, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	CRawDecod dcr(file);
    if (dcr.IsRawFile())
    {
        std::shared_ptr<CMemoryBitmap> pBitmap;
        bool bColorRAW = dcr.IsColorRAW();

        if ((IsSuperPixels() || IsRawBayer() || IsRawBilinear() || IsRawAHD()) && !bColorRAW)
        {
			pBitmap = std::make_shared<C16BitGrayBitmap>();
            ZTRACE_RUNTIME("Creating 16 bit gray memory bitmap %p (%s)", pBitmap.get(), file.generic_u8string().c_str());
        }
        else
        {
			pBitmap = std::make_shared<C48BitColorBitmap>();
            ZTRACE_RUNTIME("Creating 16 bit RGB memory bitmap %p (%s)", pBitmap.get(), file.generic_u8string().c_str());
        }

        bResult = dcr.LoadRawFile(pBitmap.get(), ignoreBrightness, pProgress);

        if (bResult)
        {
            if (C16BitGrayBitmap* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap.get()))
            {
                if (IsSuperPixels())
                    pGrayBitmap->UseSuperPixels(true);
                else if (IsRawBayer())
                    pGrayBitmap->UseRawBayer(true);
                else if (IsRawBilinear())
                    pGrayBitmap->UseBilinear(true);
                else if (IsRawAHD())
                    pGrayBitmap->UseAHD(true);
            }
			rpBitmap = pBitmap;
        }
    }
	return bResult;
}

#define EXCEPTION_HANDLER(e) do{                        \
    /* fprintf(stderr,"Exception %d caught\n",e);*/     \
    switch(e)                                           \
      {                                                 \
      case LIBRAW_EXCEPTION_ALLOC:                      \
        recycle();                                      \
        return LIBRAW_UNSUFFICIENT_MEMORY;              \
      case LIBRAW_EXCEPTION_DECODE_RAW:                 \
      case LIBRAW_EXCEPTION_DECODE_JPEG:                \
        recycle();                                      \
        return LIBRAW_DATA_ERROR;                       \
      case LIBRAW_EXCEPTION_DECODE_JPEG2000:            \
        recycle();                                      \
        return LIBRAW_DATA_ERROR;                       \
      case LIBRAW_EXCEPTION_IO_EOF:                     \
      case LIBRAW_EXCEPTION_IO_CORRUPT:                 \
        recycle();                                      \
        return LIBRAW_IO_ERROR;                                 \
      case LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK:              \
        recycle();                                              \
        return LIBRAW_CANCELLED_BY_CALLBACK;                    \
      case LIBRAW_EXCEPTION_BAD_CROP:                           \
        recycle();                                              \
        return LIBRAW_BAD_CROP;                                 \
      default:                                                  \
        return LIBRAW_UNSPECIFIED_ERROR;                        \
      }                                                         \
  }while(0)

int DSSLibRaw::dcraw_ppm_tiff_writer(const char*)
{
	ZFUNCTRACE_RUNTIME();
	CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

	if (!imgdata.image)
		return LIBRAW_OUT_OF_ORDER_CALL;

	try {
		if (!libraw_internal_data.output_data.histogram)
		{
			libraw_internal_data.output_data.histogram =
				(int(*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram) * 4);
		//	merror(libraw_internal_data.output_data.histogram, "LibRaw::dcraw_ppm_tiff_writer()");
		}
		write_ppm_tiff();
		SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
		return 0;
	}
	catch (LibRaw_exceptions err) {
		EXCEPTION_HANDLER(err);
	}
}

#include "internal/var_defines.h"
#define FORCC for (c=0; c < colors && c < 4; c++)
#define SWAP(a,b) { a=a+b; b=a-b; a=a-b; }

void DSSLibRaw::write_ppm_tiff()
{
	ZFUNCTRACE_RUNTIME();
	uchar *ppm;
	ushort *ppm2;
	int c, row, col, soff, rstep, cstep;
	int perc, val, total, t_white = 0x2000;

#ifdef LIBRAW_LIBRARY_BUILD
	perc = width * height * auto_bright_thr;
#else
	perc = width * height * 0.01;		/* 99th percentile white level */
#endif
	if (fuji_width) perc /= 2;
	if (!((highlight & ~2) || no_auto_bright))
		for (t_white = c = 0; c < colors; c++) {
			for (val = 0x2000, total = 0; --val > 32; )
				if ((total += histogram[c][val]) > perc) break;
			if (t_white < val) t_white = val;
		}
	gamma_curve(gamm[0], gamm[1], 2, (t_white << 3) / bright);
	iheight = height;
	iwidth = width;
	if (flip & 4) SWAP(height, width);
	ppm = (uchar *)calloc(width, colors*output_bps / 8);
	if (nullptr == ppm) throw LIBRAW_EXCEPTION_ALLOC;
	ppm2 = (ushort *)ppm;
	// merror(ppm, "write_ppm_tiff()");

	//
	// Neither need nor want to write TIFF/PPM headers
	//
#if (0)
	if (output_tiff) {
		tiff_head(&th, 1);
		fwrite(&th, sizeof th, 1, ofp);
		if (oprof)
			fwrite(oprof, ntohl(oprof[0]), 1, ofp);
	}
	else if (colors > 3)
		fprintf(ofp,
			"P7\nWIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL %d\nTUPLTYPE %s\nENDHDR\n",
			width, height, colors, (1 << output_bps) - 1, cdesc);
	else
		fprintf(ofp, "P%d\n%d %d\n%d\n",
			colors / 2 + 5, width, height, (1 << output_bps) - 1);
#endif
	//
	// Populate some Bitmap loader variables with data that would
	// have been written to the PGM/PPM header
	//
	pDSSBitMapFiller->setGrey(1 == colors);
	pDSSBitMapFiller->setWidth(width);
	pDSSBitMapFiller->setHeight(height);
	pDSSBitMapFiller->setMaxColors((1 << output_bps) - 1);

	soff = flip_index(0, 0);
	cstep = flip_index(0, 1) - soff;
	rstep = flip_index(1, 0) - flip_index(0, width);
	for (row = 0; row < height; row++, soff += rstep)
	{
		for (col = 0; col < width; col++, soff += cstep)
		{
			if (output_bps == 8)
				FORCC ppm[col*colors + c] = curve[image[soff][c]] >> 8;
			else
				FORCC ppm2[col*colors + c] = curve[image[soff][c]];
		}
		if (output_bps == 16 && !output_tiff && htons(0x55aa) != 0x55aa)
			_swab((char*)ppm2, (char*)ppm2, width*colors * 2);

		//
		// Instead of writing the bitmap data to an output file
		// send it to our Bitmap loader class
		//
		pDSSBitMapFiller->Write(ppm, colors*output_bps / 8, width, row); // Gray or color, 8 or 16 bits per sample.
	}
	free(ppm);
}
