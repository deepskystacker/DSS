#include <stdafx.h>
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "MemoryBitmap.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"
#include "BitmapIterator.h"
#include "AHDDemosaicing.h"
#include "Multitask.h"
#include "Ztrace.h"
#include "ZExcBase.h"
#include "ZExcept.h"
#include "RationalInterpolation.h"
#include "RAWUtils.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "File.h"
#include "MedianFilterEngine.h"
#include "omp.h"
#include "dssbase.h"

using namespace DSS;

const QStringList rawFileExtensions{ "cr2", "cr3", "crw", "nef", "mrw", "orf", "raf", "pef", "x3f", "dcr",
		"kdc", "srf", "arw", "raw", "dng", "ia", "rw2" };

/* ------------------------------------------------------------------- */

void CopyBitmapToClipboard(HBITMAP hBitmap)
{
	ZFUNCTRACE_RUNTIME();
	HDC					hScreenDC;
	HDC					hSrcDC,
		hTgtDC;
	BITMAP				bmpInfo;
	HBITMAP				hTgtBmp;
	HBITMAP				hOldSrcBmp,
		hOldTgtBmp;

	GetObject(hBitmap, sizeof(BITMAP), &bmpInfo);

	hScreenDC = GetDC(nullptr);
	hSrcDC = CreateCompatibleDC(hScreenDC);
	hTgtDC = CreateCompatibleDC(hScreenDC);

	hTgtBmp = CreateCompatibleBitmap(hScreenDC, bmpInfo.bmWidth, bmpInfo.bmHeight);

	if (hTgtBmp)
	{
		hOldSrcBmp = (HBITMAP)SelectObject(hSrcDC, hBitmap);
		hOldTgtBmp = (HBITMAP)SelectObject(hTgtDC, hTgtBmp);

		BitBlt(hTgtDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight,
			hSrcDC, 0, 0, SRCCOPY);

		SelectObject(hSrcDC, hOldSrcBmp);
		SelectObject(hTgtDC, hOldTgtBmp);
	};

	DeleteDC(hSrcDC);
	DeleteDC(hTgtDC);
	ReleaseDC(nullptr, hScreenDC);

	if (hTgtBmp)
	{
		OpenClipboard(nullptr);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hTgtBmp);
		CloseClipboard();
	};
};

namespace
{

	template <template<class> class It, class BitmapPtr>
	struct ThreadVars
	{
		BitmapPtr bitmap; // E.g.: CMemoryBitmap*, std::shared_ptr<CMemoryBitmap>
		It<BitmapPtr> pixelIt;
		explicit ThreadVars(BitmapPtr pb) : bitmap{ pb }, pixelIt{ pb } {}
		ThreadVars(const ThreadVars& rhs) : bitmap{ rhs.bitmap }, pixelIt{ rhs.bitmap } {}
		ThreadVars& operator=(const ThreadVars&) = delete;
	};

}

/* ------------------------------------------------------------------- */
bool DebayerPicture(CMemoryBitmap* pInBitmap, std::shared_ptr<CMemoryBitmap>& rpOutBitmap, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && pInBitmap->IsCFA())
	{
		std::shared_ptr<CMemoryBitmap> pOutBitmap;
		C16BitGrayBitmap* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pInBitmap);
		const CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pInBitmap);

		ZASSERTSTATE(nullptr != pCFABitmapInfo);
		if (pGrayBitmap != nullptr && pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
		{
			// AHD Demosaicing of the image
			AHDDemosaicing(pGrayBitmap, pOutBitmap, pProgress);
		}
		else
		{
			// Transform the gray scale image to color image
			const int lWidth = pInBitmap->Width();
			const int lHeight = pInBitmap->Height();
			std::shared_ptr<C48BitColorBitmap> pColorBitmap = std::make_shared<C48BitColorBitmap>();
			pColorBitmap->Init(lWidth, lHeight);
			ThreadVars<BitmapIterator, CMemoryBitmap*> threadVars{ pColorBitmap.get() };

#pragma omp parallel for default(none) firstprivate(threadVars) if(CMultitask::GetNrProcessors() > 1)
			for (int j = 0; j < lHeight; j++)
			{
				threadVars.pixelIt.Reset(0, j);
				for (int i = 0; i < lWidth; ++i, ++threadVars.pixelIt)
				{
					double fRed, fGreen, fBlue;
					pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
					threadVars.pixelIt.SetPixel(fRed, fGreen, fBlue);
				}
			}

			pOutBitmap = pColorBitmap;
		}
		rpOutBitmap = pOutBitmap;
		bResult = static_cast<bool>(pOutBitmap);
	}

	return bResult;
}

/* ------------------------------------------------------------------- */
bool	CAllDepthBitmap::initQImage()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	const int width = m_pBitmap->Width();
	const int height = m_pBitmap->Height();
	const int numberOfProcessors = CMultitask::GetNrProcessors();

	m_Image = std::make_shared<QImage>(width, height, QImage::Format_RGB32);

	//
	//				**** W A R N I N G ***
	// 
	// Calling QImage::bits() on a non-const QImage causes a
	// deep copy of the image data on the assumption that it's
	// about to be changed. That's fine
	//
	// Unfortunately QImage::scanLine does the same thing, which
	// means that it's not safe to use it inside openmp loops, as
	// the deep copy code isn't thread safe.
	// 
	// In any case making a deep copy of the image data for every
	// row of the image data is hugely inefficient.
	//

	if (m_pBitmap->IsMonochrome() && m_pBitmap->IsCFA())
	{
		ZTRACE_RUNTIME("Slow Bitmap Copy to QImage");
		// Slow Method

		//
		// Point to the first RGB quad in the QImage which we
		// need to cast to QRgb* (which is unsigned int*) from
		// unsigned char * which is what QImage::bits() returns
		//

		auto pImageData = m_Image->bits();
		auto bytes_per_line = m_Image->bytesPerLine();

#pragma omp parallel for schedule(guided, 50) default(none) if(numberOfProcessors > 1)
		for (int j = 0; j < height; j++)
		{
			QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (j * bytes_per_line));
			for (int i = 0; i < width; i++)
			{
				double			fRed, fGreen, fBlue;
				m_pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

				*pOutPixel++ = qRgb(std::clamp(fRed, 0.0, 255.0),
					std::clamp(fGreen, 0.0, 255.0),
					std::clamp(fBlue, 0.0, 255.0));
			}
		}
	}
	else
	{
		ZTRACE_RUNTIME("Fast Bitmap Copy to QImage");
		// Fast Method

		//
		// Point to the first RGB quad in the QImage which we
		// need to cast to QRgb* (which is unsigned int*) from
		// unsigned char * which is what QImage::bits() returns
		//

		auto pImageData = m_Image->bits();
		auto bytes_per_line = m_Image->bytesPerLine();
		ThreadVars<BitmapIteratorConst, const CMemoryBitmap*> threadVars{ m_pBitmap.get() };

#pragma omp parallel for firstprivate(threadVars) default(none) if(numberOfProcessors > 1)
		for (int j = 0; j < height; j++)
		{
			QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (j * bytes_per_line));
			threadVars.pixelIt.Reset(0, j);

			for (int i = 0; i < width; i++, ++threadVars.pixelIt, ++pOutPixel)
			{
				double fRed, fGreen, fBlue;
				threadVars.pixelIt.GetPixel(fRed, fGreen, fBlue);

				*pOutPixel = qRgb(std::clamp(fRed, 0.0, 255.0),
					std::clamp(fGreen, 0.0, 255.0),
					std::clamp(fBlue, 0.0, 255.0));
			}
		}
	}

	bResult = true;

	return bResult;
}


bool LoadPicture(const fs::path& file, CAllDepthBitmap& AllDepthBitmap, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	try
	{
		AllDepthBitmap.Clear();

		if (FetchPicture(file, AllDepthBitmap.m_pBitmap, false, pProgress, AllDepthBitmap.m_Image))
		{
			std::shared_ptr<CMemoryBitmap> pBitmap = AllDepthBitmap.m_pBitmap;
			C16BitGrayBitmap* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap.get());
			CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(AllDepthBitmap.m_pBitmap.get());

			if (pBitmap->IsCFA())
			{
				ZASSERTSTATE(nullptr != pCFABitmapInfo);

				if (AllDepthBitmap.m_bDontUseAHD && pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
					pCFABitmapInfo->UseBilinear(true);

				if (pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
				{
					// AHD Demosaicing of the image
					std::shared_ptr<CMemoryBitmap> pColorBitmap;
					AHDDemosaicing(pGrayBitmap, pColorBitmap, nullptr);

					AllDepthBitmap.m_pBitmap = pColorBitmap;
				}
				else
				{
					// Transform the gray scale image to color image
					const int lWidth = pBitmap->Width();
					const int lHeight = pBitmap->Height();
					std::shared_ptr<C48BitColorBitmap>	pColorBitmap = std::make_shared<C48BitColorBitmap>();
					pColorBitmap->Init(lWidth, lHeight);

#pragma omp parallel for default(none) schedule(dynamic, 50) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
					for (int j = 0; j < lHeight; j++)
					{
						for (int i = 0; i < lWidth; i++)
						{
							double			fRed, fGreen, fBlue;

							pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
							pColorBitmap->SetPixel(i, j, fRed, fGreen, fBlue);
						}
					}
					AllDepthBitmap.m_pBitmap = pColorBitmap;
				}
			}

			//
			// Create a Windows bitmap for display purposes (wrapped in a C32BitsBitmap class).
			// (TODO) Delete this when Qt porting is done.
			//
			AllDepthBitmap.m_pWndBitmap = std::make_shared<C32BitsBitmap>();
			AllDepthBitmap.m_pWndBitmap->InitFrom(AllDepthBitmap.m_pBitmap.get());

			//
			// If FetchPicture didn't create a QImage (which it does when loading a jpeg or png file),
			// then we need to create a QImage from the raw bitmap data.
			//
			if (!AllDepthBitmap.m_Image) 
				AllDepthBitmap.initQImage();

			bResult = true;
		}
	}
	catch (std::exception& e)
	{
		const QString errorMessage(e.what());

		//
		// Report the error and terminate 
		//
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical, DSSBase::Method::QMessageBox, true);
	}
	catch (ZException& e)
	{
		QString errorMessage;
		if (e.locationAtIndex(0))
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from %2 Function : %3() Line : %4\n\n %5")
				.arg(e.name())
				.arg(e.locationAtIndex(0)->fileName())
				.arg(e.locationAtIndex(0)->functionName())
				.arg(e.locationAtIndex(0)->lineNumber())
				.arg(e.text(0));
		}
		else
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from an unknown Function.\n\n%2")
				.arg(e.name())
				.arg(e.text(0));
		}

		//
		// Report the error and terminate 
		//
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical, DSSBase::Method::QMessageBox, true);
	}
	catch (...)
	{
		const QString errorMessage(QCoreApplication::translate("Kernel", "Unknown exception caught"));

		//
		// Report the error and terminate 
		//
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical, DSSBase::Method::QMessageBox, true);
	}
	return bResult;
}


bool LoadOtherPicture(const fs::path& file, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* const pProgress, 
	std::shared_ptr<QImage>& pQImage )
{
	constexpr double scaleFactorInt16 = 1.0 + std::numeric_limits<std::uint8_t>::max();
	ZFUNCTRACE_RUNTIME();
	const int numberOfProcessors = CMultitask::GetNrProcessors();
	bool result = false;
	const QString name{ QString::fromStdU16String(file.generic_u16string()) };

	//
	// pQImage better be a nullptr
	//
	ZASSERTSTATE(!pQImage);		
	pQImage = std::make_shared<QImage>(name);	// load the file
	if (pQImage->isNull())		// If it failed ...
	{
		ZTRACE_RUNTIME("Failed to load file into QImage");
		pQImage.reset();
		return false;
	}
	std::shared_ptr<CMemoryBitmap> pBitmap;
	int bits { pQImage->bitPlaneCount() };
	switch (bits)
	{
	case 24:
		ZTRACE_RUNTIME("Creating 8 bit RGB memory bitmap %p (%s)", pBitmap.get(), file.generic_u8string().c_str());
		pBitmap = std::make_shared<C24BitColorBitmap>();
		break;
	case 48:
		ZTRACE_RUNTIME("Creating 16 bit RGB memory bitmap %p (%s)", pBitmap.get(), file.generic_u8string().c_str());
		pBitmap = std::make_shared<C48BitColorBitmap>();
		break;
	default:
		pQImage.reset();
		return false;
	}
	const int width{ pQImage->width() };
	const int height{ pQImage->height() };

	if (pProgress != nullptr)
		pProgress->Start2(height);

	pBitmap->Init(width, height);
	//
	// Point to the first RGB quad in the QImage which we
	// need to cast to QRgb* (which is unsigned int*) from
	// unsigned char * which is what QImage::bits() returns
	//

	auto pImageData{ pQImage->constBits() };
	auto bytes_per_line = pQImage->bytesPerLine();
	
	std::atomic_int loopCtr{ 0 };
	if (24 == bits)
	{
#pragma omp parallel for schedule(guided, 100) shared(loopCtr) default(none) if(numberOfProcessors > 1)
		for (int j = 0; j < height; j++)
		{
			const QRgb* pRgbPixel = reinterpret_cast<const QRgb*>(pImageData + (j * bytes_per_line));
			for (int i = 0; i < width; i++)
			{

				double	fRed{ 0 }, fGreen{ 0 }, fBlue{ 0 };
				fRed = qRed(*pRgbPixel);
				fGreen = qGreen(*pRgbPixel);
				fBlue = qBlue(*pRgbPixel);
				pBitmap->SetPixel(i, j,
					fRed,	// was std::clamp(fRed, 0.0, 255.0),
					fGreen, // was std::clamp(fGreen, 0.0, 255.0),
					fBlue);	// was std::clamp(fBlue, 0.0, 255.0));
				pRgbPixel++;

			}
			if (pProgress != nullptr)
				pProgress->Progress2(++loopCtr);
		}
	}
	else       // Must be a 48 bit image
	{
#pragma omp parallel for schedule(guided, 100) shared(loopCtr) default(none) if(numberOfProcessors > 1)
		for (int j = 0; j < height; j++)
		{
			const QRgba64* pRgba64Pixel = reinterpret_cast<const QRgba64*>(pImageData + (j * bytes_per_line));
			for (int i = 0; i < width; i++)
			{
				double	fRed{ 0 }, fGreen{ 0 }, fBlue{ 0 };
				fRed = pRgba64Pixel->red() / scaleFactorInt16;		// Returns quint16 == uint16_t
				fGreen = pRgba64Pixel->green() / scaleFactorInt16;
				fBlue = pRgba64Pixel->blue() / scaleFactorInt16;
				pBitmap->SetPixel(i, j,
					fRed,	// was std::clamp(fRed, 0.0, 255.0),
					fGreen, // was std::clamp(fGreen, 0.0, 255.0),
					fBlue);	// was std::clamp(fBlue, 0.0, 255.0));
				pRgba64Pixel++;
			}
			if (pProgress != nullptr)
				pProgress->Progress2(++loopCtr);
		}
	}

	if (pProgress != nullptr)
		pProgress->End2();

	rpBitmap = pBitmap;

	CBitmapInfo bmpInfo;
	if (RetrieveEXIFInfo(file, bmpInfo))
		pBitmap->m_DateTime = bmpInfo.m_DateTime;

	result = true;

	return result;
}

bool C32BitsBitmap::CopyToClipboard()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	const size_t dwSize = sizeof(BITMAPINFO) + 16 + static_cast<size_t>(m_dwByteWidth) * m_lHeight;

	HGLOBAL			hGlobal;
	char *			pGlobal;

	if (m_hBitmap)
	{
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		if (hGlobal)
		{
			BITMAPINFO		bmpInfo;

			memset(&bmpInfo, 0, sizeof(bmpInfo));
			bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
			bmpInfo.bmiHeader.biWidth = m_lWidth;
			bmpInfo.bmiHeader.biHeight= m_lHeight;
			bmpInfo.bmiHeader.biPlanes= 1;;
			bmpInfo.bmiHeader.biBitCount= 32;
			bmpInfo.bmiHeader.biCompression= BI_RGB;
			bmpInfo.bmiHeader.biSizeImage= 0;
			bmpInfo.bmiHeader.biXPelsPerMeter = (int)(96*100.0/2.54);
			bmpInfo.bmiHeader.biYPelsPerMeter = (int)(96*100.0/2.54);
			bmpInfo.bmiHeader.biClrUsed = 0;
			bmpInfo.bmiHeader.biClrImportant = 0;

			pGlobal = (char*)GlobalLock(hGlobal);
			memcpy(pGlobal, &bmpInfo, sizeof(BITMAPINFO));
			pGlobal += sizeof(BITMAPINFO);

			memcpy(pGlobal, m_lpBits, m_dwByteWidth*m_lHeight);

			GlobalUnlock(hGlobal);

			if (OpenClipboard(nullptr/*AfxGetMainWnd()->GetSafeHwnd()*/))
			{
				EmptyClipboard();
				if (!SetClipboardData(CF_DIB, hGlobal))
					GlobalFree(hGlobal);
				else
					bResult = true;
				CloseClipboard();
			}
			else
				GlobalFree(hGlobal);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool C32BitsBitmap::InitFrom(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	Free();
	if (pBitmap != nullptr)
	{
		HBITMAP hBitmap = Create(pBitmap->Width(), pBitmap->Height());
		if (hBitmap)
		{
			if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
			{
				ZTRACE_RUNTIME("Slow Bitmap Copy");
				// Slow Method
				for (int j = 0; j < m_lHeight; j++)
				{
					pByte lpOut;
					LPRGBQUAD& lpOutPixel = reinterpret_cast<LPRGBQUAD&>(lpOut);

					lpOut = GetPixelBase(0, j);
					for (int i = 0; i < m_lWidth; i++)
					{
						double			fRed, fGreen, fBlue;
						pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

						lpOutPixel->rgbRed		= std::min(std::max(0.0, fRed), 255.0);
						lpOutPixel->rgbGreen	= std::min(std::max(0.0, fGreen), 255.0);
						lpOutPixel->rgbBlue		= std::min(std::max(0.0, fBlue), 255.0);
						lpOutPixel->rgbReserved	= 0;

						lpOut += 4;
					}
				}
			}
			else
			{
				ZTRACE_RUNTIME("Fast Bitmap Copy");
				// Fast Method
				BitmapIteratorConst<const CMemoryBitmap*> it{ pBitmap };

				for (int j = 0; j < m_lHeight; j++)
				{
					pByte lpOut;
					LPRGBQUAD& lpOutPixel = reinterpret_cast<LPRGBQUAD&>(lpOut);

					it.Reset(0, j);
					lpOut = GetPixelBase(0, j);
					for (int i = 0; i < m_lWidth; i++)
					{
						double fRed, fGreen, fBlue;
						it.GetPixel(fRed, fGreen, fBlue);

						lpOutPixel->rgbRed = std::min(std::max(0.0, fRed), 255.0);
						lpOutPixel->rgbGreen = std::min(std::max(0.0, fGreen), 255.0);
						lpOutPixel->rgbBlue = std::min(std::max(0.0, fBlue), 255.0);
						lpOutPixel->rgbReserved = 0;

						lpOut += 4;
						++it;
					}
				}
			}
			bResult = true;
		}
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

namespace DSS
{
	void GammaTransformation::initTransformation(double fGamma)
	{
		ZFUNCTRACE_RUNTIME();
		double				fBlackPoint = 0.0,
			fWhitePoint = 1.0,
			fGrayPoint = 0.5;

		fGrayPoint = pow(0.5, 1.0 / fGamma);
		initTransformation(fBlackPoint, fGrayPoint, fWhitePoint);
	}

	/* ------------------------------------------------------------------- */

	void GammaTransformation::initTransformation(double fBlackPoint, double fGrayPoint, double fWhitePoint)
	{
		ZFUNCTRACE_RUNTIME();
		const int lBlackPoint = static_cast<int>(uint16Max_asDouble * fBlackPoint);
		const int lWhitePoint = static_cast<int>(uint16Max_asDouble * fWhitePoint);

		u8transform.resize(transformSize);
		u16transform.resize(transformSize);

		CRationalInterpolation ri;
		ri.Initialize(fBlackPoint, fGrayPoint, fWhitePoint, 0, 0.5, 1.0);

		// Perform rational interpolation for uint16_t
		for (int i = 0; i < transformSize; i++)
		{
			if (i <= lBlackPoint)
				u16transform[i] = 0;
			else if (i >= lWhitePoint)
				u16transform[i] = std::numeric_limits<uint16_t>::max();
			else
			{
				const double fValue = ri.Interpolate(i / static_cast<double>(std::numeric_limits<uint16_t>::max()));
				u16transform[i] = static_cast<double>(std::numeric_limits<uint16_t>::max()) * fValue;//pow(fValue, fGamma);
			}
		}

		// Perform rational interpolation for uint8_t
		for (int i = 0; i < transformSize; i++)
		{
			if (i <= lBlackPoint)
				u8transform[i] = 0;
			else if (i >= lWhitePoint)
				u8transform[i] = std::numeric_limits<uint8_t>::max();
			else
			{
				const double fValue = ri.Interpolate(i / static_cast<double>(std::numeric_limits<uint16_t>::max()));
				u8transform[i] = static_cast<double>(std::numeric_limits<uint8_t>::max()) * fValue;//pow(fValue, fGamma);
			}
		}

		valid = true;
	}
}

/* ------------------------------------------------------------------- */

template <template<class> class BitmapClass, class T>
bool ApplyGammaTransformation(QImage* pImage, BitmapClass<T>* pInBitmap, DSS::GammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.isInitialized())
	{
		const size_t width = pInBitmap->Width();
		const size_t height = pInBitmap->Height();

		// Check that the output bitmap size is matching the input bitmap
		ZASSERTSTATE ((pImage->width() == width) && (pImage->height() == height));

		double const fMultiplier = pInBitmap->GetMultiplier() / 256.0;
		//
		// Point to the first RGB quad in the QImage which we
		// need to cast to QRgb* (which is unsigned int*) from
		// unsigned char * which is what QImage::bits() returns
		//

		auto pImageData = pImage->bits();
		auto bytes_per_line = pImage->bytesPerLine();

		if (QImage::Format_RGB32 == pImage->format())
		{
#pragma omp parallel for default(none) schedule(dynamic, 50) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < height; j++)
			{
				QRgb* pOutPixel = reinterpret_cast<QRgb*>(pImageData + (j * bytes_per_line));
				if constexpr (std::is_same_v<BitmapClass<T>, CColorBitmapT<T>>)
				{
					// Init iterators
					T* pRed = pInBitmap->GetRedPixel(0, j);
					T* pGreen = pInBitmap->GetGreenPixel(0, j);
					T* pBlue = pInBitmap->GetBluePixel(0, j);

					for (int i = 0; i < width; i++)
					{
						*pOutPixel++ = qRgb(gammatrans.getTransformation(*pRed / fMultiplier),
							gammatrans.getTransformation(*pGreen / fMultiplier),
							gammatrans.getTransformation(*pBlue / fMultiplier));
						pRed++;
						pGreen++;
						pBlue++;
					}
				}
				if constexpr (std::is_same_v<BitmapClass<T>, CGrayBitmapT<T>>)
				{
					// Init iterators
					const T* pGray = pInBitmap->GetGrayPixel(0, j);
					unsigned char value = 0;

					for (int i = 0; i < width; i++)
					{
						value = gammatrans.getTransformation(*pGray / fMultiplier);
						*pOutPixel++ = qRgb(value, value, value);
						pGray++;
					}
				}
			}
			bResult = true;
		}
		else        // Must be RGB64
		{
#pragma omp parallel for default(none) schedule(dynamic, 50) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < height; j++)
			{
				QRgba64* pOutPixel = reinterpret_cast<QRgba64*>(pImageData + (j * bytes_per_line));
				if constexpr (std::is_same_v<BitmapClass<T>, CColorBitmapT<T>>)
				{
					// Init iterators
					T* pRed = pInBitmap->GetRedPixel(0, j);
					T* pGreen = pInBitmap->GetGreenPixel(0, j);
					T* pBlue = pInBitmap->GetBluePixel(0, j);

					for (int i = 0; i < width; i++)
					{
						*pOutPixel++ = QRgba64::fromRgba64(gammatrans.getTransformation16(*pRed / fMultiplier),
							gammatrans.getTransformation16(*pGreen / fMultiplier),
							gammatrans.getTransformation16(*pBlue / fMultiplier),
							std::numeric_limits<uint16_t>::max());
						pRed++;
						pGreen++;
						pBlue++;
					}
				}
				if constexpr (std::is_same_v<BitmapClass<T>, CGrayBitmapT<T>>)
				{
					// Init iterators
					const T* pGray = pInBitmap->GetGrayPixel(0, j);
					unsigned char value = 0;

					for (int i = 0; i < width; i++)
					{
						value = gammatrans.getTransformation16(*pGray / fMultiplier);
						*pOutPixel++ = qRgba64(value, value, value, std::numeric_limits<uint16_t>::max());
						pGray++;
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
}

/* ------------------------------------------------------------------- */
bool ApplyGammaTransformation(QImage* pImage, CMemoryBitmap* pInBitmap, GammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	C24BitColorBitmap* p24BitColorBitmap = dynamic_cast<C24BitColorBitmap*>(pInBitmap);
	C48BitColorBitmap* p48BitColorBitmap = dynamic_cast<C48BitColorBitmap*>(pInBitmap);
	C96BitColorBitmap* p96BitColorBitmap = dynamic_cast<C96BitColorBitmap*>(pInBitmap);
	C96BitFloatColorBitmap* p96BitFloatColorBitmap = dynamic_cast<C96BitFloatColorBitmap*>(pInBitmap);

	CGrayBitmap* pGrayBitmap = dynamic_cast<CGrayBitmap*>(pInBitmap);
	C8BitGrayBitmap* p8BitGrayBitmap = dynamic_cast<C8BitGrayBitmap*>(pInBitmap);
	C16BitGrayBitmap* p16BitGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pInBitmap);
	C32BitGrayBitmap* p32BitGrayBitmap = dynamic_cast<C32BitGrayBitmap*>(pInBitmap);
	C32BitFloatGrayBitmap* p32BitFloatGrayBitmap = dynamic_cast<C32BitFloatGrayBitmap*>(pInBitmap);

	if (p24BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p24BitColorBitmap, gammatrans);
	else if (p48BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p48BitColorBitmap, gammatrans);
	else if (p96BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p96BitColorBitmap, gammatrans);
	else if (p96BitFloatColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p96BitFloatColorBitmap, gammatrans);
	else if (pGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, pGrayBitmap, gammatrans);
	else if (p8BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p8BitGrayBitmap, gammatrans);
	else if (p16BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p16BitGrayBitmap, gammatrans);
	else if (p32BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p32BitGrayBitmap, gammatrans);
	else if (p32BitFloatGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pImage, p32BitFloatGrayBitmap, gammatrans);

	return bResult;
};


template <template<class> class BitmapClass, class T>
bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, BitmapClass<T>* pInBitmap, GammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.isInitialized())
	{
		bool bContinue;
		const int lWidth = pInBitmap->Width();
		const int lHeight = pInBitmap->Height();

		if (pOutBitmap->IsEmpty())
		{
			// Create the Bitmap
			pOutBitmap->Init(lWidth, lHeight);
		};

		// Check that the output bitmap size is matching the input bitmap
		bContinue = (pOutBitmap->Width() == lWidth) && (pOutBitmap->Height() == lHeight);

		if (bContinue)
		{
			double const fMultiplier = pInBitmap->GetMultiplier() / 256.0;

#pragma omp parallel for default(none) schedule(dynamic, 50) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < lHeight; j++)
			{
				if constexpr (std::is_same_v<BitmapClass<T>, CColorBitmapT<T>>)
				{
					// Init iterators
					T* pRed = pInBitmap->GetRedPixel(0, j);
					T* pGreen = pInBitmap->GetGreenPixel(0, j);
					T* pBlue = pInBitmap->GetBluePixel(0, j);

					std::uint8_t* pOut = pOutBitmap->GetPixelBase(0, j);
					LPRGBQUAD& pOutPixel = reinterpret_cast<LPRGBQUAD&>(pOut);
					for (int i = 0; i < lWidth; i++)
					{
						pOutPixel->rgbRed = gammatrans.getTransformation(*pRed / fMultiplier);
						pOutPixel->rgbGreen = gammatrans.getTransformation(*pGreen / fMultiplier);
						pOutPixel->rgbBlue = gammatrans.getTransformation(*pBlue / fMultiplier);
						pOutPixel->rgbReserved = 0;
						pRed++;
						pGreen++;
						pBlue++;
						pOut += 4;
					}
				}
				if constexpr (std::is_same_v<BitmapClass<T>, CGrayBitmapT<T>>)
				{
					// Init iterators
					const T* pGray = pInBitmap->GetGrayPixel(0, j);

					std::uint8_t* pOut = pOutBitmap->GetPixelBase(0, j);
					LPRGBQUAD& pOutPixel = reinterpret_cast<LPRGBQUAD&>(pOut);
					for (int i = 0; i < lWidth; i++)
					{
						pOutPixel->rgbRed = gammatrans.getTransformation(*pGray / fMultiplier);
						pOutPixel->rgbBlue = pOutPixel->rgbRed;
						pOutPixel->rgbGreen = pOutPixel->rgbRed;
						pOutPixel->rgbReserved = 0;
						pGray++;
						pOut += 4;
					}
				}
			}
			bResult = true;
		}
	}
	return bResult;
}

bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, CMemoryBitmap* pInBitmap, GammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	C24BitColorBitmap* p24BitColorBitmap = dynamic_cast<C24BitColorBitmap*>(pInBitmap);
	C48BitColorBitmap* p48BitColorBitmap = dynamic_cast<C48BitColorBitmap*>(pInBitmap);
	C96BitColorBitmap* p96BitColorBitmap = dynamic_cast<C96BitColorBitmap*>(pInBitmap);
	C96BitFloatColorBitmap* p96BitFloatColorBitmap = dynamic_cast<C96BitFloatColorBitmap*>(pInBitmap);

	CGrayBitmap* pGrayBitmap = dynamic_cast<CGrayBitmap*>(pInBitmap);
	C8BitGrayBitmap* p8BitGrayBitmap = dynamic_cast<C8BitGrayBitmap*>(pInBitmap);
	C16BitGrayBitmap* p16BitGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pInBitmap);
	C32BitGrayBitmap* p32BitGrayBitmap = dynamic_cast<C32BitGrayBitmap*>(pInBitmap);
	C32BitFloatGrayBitmap* p32BitFloatGrayBitmap = dynamic_cast<C32BitFloatGrayBitmap*>(pInBitmap);

	if (p24BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p24BitColorBitmap, gammatrans);
	else if (p48BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p48BitColorBitmap, gammatrans);
	else if (p96BitColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p96BitColorBitmap, gammatrans);
	else if (p96BitFloatColorBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p96BitFloatColorBitmap, gammatrans);
	else if (pGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, pGrayBitmap, gammatrans);
	else if (p8BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p8BitGrayBitmap, gammatrans);
	else if (p16BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p16BitGrayBitmap, gammatrans);
	else if (p32BitGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p32BitGrayBitmap, gammatrans);
	else if (p32BitFloatGrayBitmap != nullptr)
		bResult = ApplyGammaTransformation(pOutBitmap, p32BitFloatGrayBitmap, gammatrans);

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

namespace {
	//
	// Fowler/Noll/Vo hash
	// Do not copyright this code. This code is in the public domain.
	// www.isthe.com/chongo/tech/comp/fnv
	//
	inline size_t fnv1a_hash(const unsigned char* const pData, const size_t nBytes)
	{
		static_assert(sizeof(size_t) == 8);
		size_t digest = 14695981039346656037ULL;
		for (size_t i = 0; i < nBytes; ++i)
		{
			digest ^= static_cast<size_t>(pData[i]);
			digest *= 1099511628211ULL;
		}
		return digest;
	}

	template <class T> struct BitmapInfoHash;
	template<>
	struct BitmapInfoHash<CBitmapInfo>
	{
		size_t operator()(const CBitmapInfo& other) const
		{
			const auto& str = QString::fromStdU16String(other.m_strFileName.generic_u16string().c_str());
			const QByteArray data = str.toUtf8();
			const void* pRawData = data.constData();
			return fnv1a_hash(reinterpret_cast<const unsigned char*>(pRawData), data.length());
		}
	};

	//typedef std::set<CBitmapInfo> InfoCache;
	// We absolutely must use a thread-safe cache, otherwise GetPictureInfo() crashes if used concurrently (e.g. with OpenMP).

	using InfoCache = concurrency::concurrent_unordered_set<CBitmapInfo, BitmapInfoHash<CBitmapInfo>>;
	InfoCache g_sBitmapInfoCache;
	QDateTime g_BitmapInfoTime{ QDateTime::currentDateTime() };
	std::shared_mutex bitmapInfoMutex;
}

namespace little_endian {
	unsigned read_word(std::istream& ins)
	{
		unsigned a = ins.get();
		unsigned b = ins.get();
		return b << 8 | a;
	}

	unsigned read_dword(std::istream& ins)
	{
		unsigned a = ins.get();
		unsigned b = ins.get();
		unsigned c = ins.get();
		unsigned d = ins.get();
		return d << 24 | c << 16 | b << 8 | a;
	}
}

namespace big_endian {
	unsigned read_word(std::istream& ins)
	{
		unsigned a = ins.get();
		unsigned b = ins.get();
		return a << 8 | b;
	}

	unsigned read_dword(std::istream& ins)
	{
		unsigned a = ins.get();
		unsigned b = ins.get();
		unsigned c = ins.get();
		unsigned d = ins.get();
		return a << 24 | b << 16 | c << 8 | d;
	}
}

bool GetPictureInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	const fs::path strFilename(szFileName);
	return GetPictureInfo(strFilename, BitmapInfo);
}
bool GetPictureInfo(const fs::path& path, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();

	//const QString name = QString::fromStdU16String(path.generic_u16string());

	ZTRACE_RUNTIME("Getting image information for %s", path.generic_u8string().c_str());
	bool bResult = false;
	auto now{ QDateTime::currentDateTime() };	// local time

	// First try to find the info in the cache
	if (!g_sBitmapInfoCache.empty())
	{
		// Check that the cache is not old (more than 5 minutes)
		constexpr qint64 maxAge{ 300 };		// 300 seconds == 5 minutes
		auto age{ g_BitmapInfoTime.secsTo(now) };

		if (age > maxAge)
		{
			std::lock_guard<std::shared_mutex> writeLock(bitmapInfoMutex); // clear() is NOT thread-safe => need a write-lock.
			g_sBitmapInfoCache.clear();
		}
		else
		{
			std::shared_lock<std::shared_mutex> readLock(bitmapInfoMutex);
			InfoCache::const_iterator it = g_sBitmapInfoCache.find(CBitmapInfo(path));
			if (it != g_sBitmapInfoCache.cend())
			{
				BitmapInfo = *it;
				bResult = true;
			}
		}
	}

	QFileInfo info{ path };
	QString extension{ info.suffix().toLower() }; 

	if (!bResult)
	{
		QMimeDatabase mimeDB{ };
		auto mime = mimeDB.mimeTypeForFile(info);
		bool isJpeg{ false }, isPng{ false };
		if (mime.inherits("image/jpeg"))
		{
			isJpeg = true;
			BitmapInfo.m_strFileType = "JPEG";
		}
		else if (mime.inherits("image/png"))
		{
			isPng = true;
			BitmapInfo.m_strFileType = "PNG";
		}

		//
		// Check RAW image file types
		//
		if (rawFileExtensions.contains(extension) && IsRAWPicture(path, BitmapInfo))
			bResult = true;
		else if (mime.inherits("image/tiff") && IsTIFFPicture(path, BitmapInfo))
			bResult = true;
		else if (mime.inherits("image/fits") && IsFITSPicture(path, BitmapInfo))
			bResult = true;
		else if (isJpeg || isPng)
		{
			QFile file{ path };
			file.open(QIODevice::ReadOnly);
			if (file.isOpen())
			{
				//
				// Read the first 64K of the file into a buffer
				//
				const QByteArray data{ file.peek(65536LL) };
				const std::string dataString{ data.constData(), 65536ULL };
				std::istringstream f (dataString);
				if (isJpeg)
				{
					//
					// It is jpeg - we hope, but check it really is ...
					//
					if (big_endian::read_word(f) != 0xFFD8) return false;

					bool foundSOF{ false };
					while (f)
					{
						// seek for next marker
						int b { 0 };
						while (f and (f.get() != 0xFF)) /* no body */;
						while (f and ((b = f.get()) == 0xFF)) /* no body */;

						// the SOF marker contains the image dimensions
						if ((0xC0 <= b) and (b <= 0xCF) and (b != 0xC4) and (b != 0xC8) and (b != 0xCC))
						{
							f.seekg(2, std::ios::cur);
							BitmapInfo.m_lBitsPerChannel = f.get();
							BitmapInfo.m_lHeight = big_endian::read_word(f);
							BitmapInfo.m_lWidth = big_endian::read_word(f);
							BitmapInfo.m_lNrChannels = f.get();
							foundSOF = true;
							break;
						}

						// Otherwise we must skip stuff (like thumbnails...)
						else
						{
							f.seekg(big_endian::read_word(f) - 2, std::ios::cur);
						}
					}
					if (!foundSOF) return false;
				}
				else
				{
					char header[8] {};
					constexpr unsigned char pngheader[8]{137, 80, 78, 71, 13, 10, 26, 10};

					//
					// It must be PNG - but check anyway ...
					//
					f.read(header, sizeof(header));
					if (0 != memcmp(header, pngheader, sizeof(header))) return false;

					constexpr unsigned char IHDR[4]{73, 72, 68, 82}; // IHDR as ascii 
					char type[4]{};

					//
					// The first segment MUST be the IHDR segment
					//
					uint32_t chunkLength = big_endian::read_dword(f); chunkLength;
					f.read(type, sizeof(type));
					if (0 != memcmp(type, IHDR, sizeof(type))) return false;

					BitmapInfo.m_lWidth = big_endian::read_dword(f);
					BitmapInfo.m_lHeight = big_endian::read_dword(f);
					BitmapInfo.m_lBitsPerChannel = f.get();

					char colorType = f.get();
					switch (colorType)
					{
					case 0:
					case 4:
						BitmapInfo.m_lNrChannels = 1;
						break;
					case 2:		// RGB
					case 6:		// RGBA (strictly 4 channels but we say 3)
						BitmapInfo.m_lNrChannels = 3;
						break;
					default:
						return false;
					}
				}
				BitmapInfo.m_strFileName = path;
				BitmapInfo.m_CFAType = CFATYPE_NONE;
				BitmapInfo.m_bCanLoad = true;
				bResult = true;
				RetrieveEXIFInfo(path, BitmapInfo);
			}
			else return false;
		}

		if (bResult)
		{
			if (!BitmapInfo.m_DateTime.isValid())
			{

				//
				// This originally used the EXIF info but that wasn't always available, so it was
				// changed to use the file creation time.  Sadly that's not available in all cases
				// on Unix like systems so now the code has changed again to use the last modification
				// time if the creation time is unavailable.
				//
				QDateTime fileTime{ info.birthTime() };
				QDateTime lastModified{ info.lastModified() };
				if (!fileTime.isValid() || fileTime > lastModified)
				{
					fileTime = lastModified;
				}

				BitmapInfo.m_DateTime = fileTime;
			};

			BitmapInfo.m_InfoTime = now;

			//
			// Originally used ISO 8601 date format yyyy-MM-ddThh:mm:ss - change to use the more
			// familiar form: yyyy/MM/dd hh:mm:ss
			//
			BitmapInfo.m_strDateTime = BitmapInfo.m_DateTime.toString("yyyy/MM/dd hh:mm:ss"); 

			std::shared_lock<std::shared_mutex> readLock(bitmapInfoMutex);
			if (g_sBitmapInfoCache.empty())
				g_BitmapInfoTime = now;
			g_sBitmapInfoCache.insert(BitmapInfo);
		}
	}
	return bResult;
}

/* ------------------------------------------------------------------- */

bool FetchPicture(const fs::path filePath, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness,
	ProgressBase* const pProgress, std::shared_ptr<QImage>& pQImage)
{
	ZFUNCTRACE_RUNTIME();
	ZTRACE_RUNTIME("Processing file %s", filePath.generic_u8string().c_str());
	bool result{ false };
	QString name{ QString::fromStdU16String(filePath.generic_u16string().c_str()) };
	//const auto fileName = filePath.generic_u16string(); // Otherwise szFileName could be a dangling pointer.

	if (fs::status(filePath).type() != fs::file_type::regular)
	{
		ZTRACE_RUNTIME("File %s not found", filePath.generic_u8string().c_str());
		const QString errorMessage{ QCoreApplication::translate(
									"BitmapExt",
									"%1 does not exist or is not a file").arg(name) };

		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Warning);

		return false;
	}

	QFileInfo info{ name };
	QString extension{ info.suffix().toLower() };
	QMimeDatabase mimeDB{ };
	auto mime = mimeDB.mimeTypeForFile(info);


	do  // do { ... } while (false); to be able to leave with break;
	{
		CBitmapInfo BitmapInfo;
		int loadResult = 0;

		//
		// Is it a raw file?
		//
		if (rawFileExtensions.contains(extension))			// No need to call IsRawPicture here
		{
			result = LoadRAWPicture(filePath, rpBitmap, ignoreBrightness, pProgress);
			break;
		}

		//
		// Maybe it is a TIFF file?
		//
		else if (mime.inherits("image/tiff"))
		{
			// Meanings of loadResult:
			//
			//		-1		Not a file of the appropriate type
			//		0		File successfully loaded
			//		1		File failed to load
			//
			// If the file loaded or failed to load, leave the loop with an appropriate value of bResult set.

			loadResult = LoadTIFFPicture(filePath, BitmapInfo, rpBitmap, pProgress);
			if (0 == loadResult)
			{
				result = true;
				break; // All done - file has been loaded 
			}
			else break; // All done - file failed to load
		}

		//
		// It wasn't a TIFF file, so try to load a FITS file
		//
		else if (mime.inherits("image/fits"))
		{
			loadResult = LoadFITSPicture(filePath, BitmapInfo, rpBitmap, ignoreBrightness, pProgress);
			if (0 == loadResult)
			{
				result = true;
				break;		// All done - file has been loaded 
			}
			else break;		// All done - file failed to load
		}

		//
		// It wasn't a FITS file, so try to load other stuff ...
		//
		else result = LoadOtherPicture(filePath, rpBitmap, pProgress, pQImage);

	} while (false);
	return result;
}

class CSubtractTask
{
private :
	std::shared_ptr<CMemoryBitmap> m_pTarget;
	std::shared_ptr<const CMemoryBitmap> m_pSource;
	ProgressBase* m_pProgress;
	double m_fRedFactor;
	double m_fGreenFactor;
	double m_fBlueFactor;
	double m_fGrayFactor;
	double m_fXShift;
	double m_fYShift;
	double m_fMinimum;
	bool m_bMonochrome;
	bool m_bAddMode;

public :
    CSubtractTask() :
		m_pTarget{},
		m_pSource{},
		m_fXShift{ 0 },
        m_fYShift{ 0 },
        m_bAddMode{ false },
        m_fMinimum{ 0 },
        m_fRedFactor{ 0 },
        m_fGreenFactor{ 0 },
        m_fBlueFactor{ 0 },
        m_fGrayFactor{ 0 },
        m_bMonochrome{ false },
        m_pProgress{ nullptr }
	{}

	~CSubtractTask() = default;

	void Init(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, ProgressBase* pProgress, const double fRedFactor, const double fGreenFactor, const double fBlueFactor)
	{
		m_pProgress = pProgress;
		m_pTarget = pTarget;
		m_pSource = pSource;
		m_fRedFactor = fRedFactor;
		m_fGreenFactor = fGreenFactor;
		m_fBlueFactor = fBlueFactor;
		m_bMonochrome = pTarget->IsMonochrome();
		m_fGrayFactor = m_bMonochrome ? std::max(fRedFactor, std::max(fGreenFactor, fBlueFactor)) : 1.0;

		if (pProgress != nullptr)
			pProgress->Start2(pTarget->RealWidth());
	}

	CSubtractTask& SetShift(const double fXShift, const double fYShift)
	{
		m_fXShift = fXShift;
		m_fYShift = fYShift;
		return *this;
	}

	CSubtractTask& SetAddMode(const bool bSet)
	{
		m_bAddMode = bSet;
		return *this;
	}

	CSubtractTask& SetMinimumValue(const double fValue)
	{
		m_fMinimum = fValue;
		return *this;
	}

	void End()
	{
		if (m_pProgress != nullptr)
			m_pProgress->End2();
	}

	void process();
};

void CSubtractTask::process()
{
	ZFUNCTRACE_RUNTIME();
	const int height = m_pTarget->RealHeight() - (m_fYShift == 0 ? 0 : static_cast<int>(std::abs(m_fYShift) + 0.5));
	const int nrProcessors = CMultitask::GetNrProcessors();

	if (m_pProgress != nullptr)
		m_pProgress->Start2(height);

	const int extraWidth = m_fXShift == 0 ? 0 : static_cast<int>(std::abs(m_fXShift) + 0.5);
	const int width = m_pTarget->RealWidth() - extraWidth;

	ThreadVars<BitmapIteratorConst, const CMemoryBitmap*> sourceIt{ m_pSource.get() };
	ThreadVars<BitmapIterator, CMemoryBitmap*> targetIt{ m_pTarget.get() };

#pragma omp parallel for default(none) firstprivate(sourceIt, targetIt) if(nrProcessors > 1)
	for (int row = 0; row < height; ++row)
	{
		int lTgtStartX = 0, lTgtStartY = row, lSrcStartX = 0, lSrcStartY = row;

		if (m_fXShift > 0)
		{
			// Target is moved
			lTgtStartX += m_fXShift + 0.5;
		}
		else if (m_fXShift < 0)
		{
			// Source is moved
			lSrcStartX += std::abs(m_fXShift) + 0.5;
		}
		if (m_fYShift > 0)
		{
			// Target is moved
			lTgtStartY += m_fYShift + 0.5;
		}
		else
		{
			// Source is moved
			lSrcStartY += std::abs(m_fYShift) + 0.5;
		}

		targetIt.pixelIt.Reset(lTgtStartX, lTgtStartY);
		sourceIt.pixelIt.Reset(lSrcStartX, lSrcStartY);

		for (int col = 0; col < width; ++col)
		{
			if (m_bMonochrome)
			{
				double fTgtGray = targetIt.pixelIt.GetPixel();
				double fSrcGray = sourceIt.pixelIt.GetPixel();

				if (m_bAddMode)
					fTgtGray = std::min(std::max(0.0, fTgtGray + fSrcGray * m_fGrayFactor), 256.0);
				else
					fTgtGray = std::max(m_fMinimum, fTgtGray - fSrcGray * m_fGrayFactor);
				targetIt.pixelIt.SetPixel(fTgtGray);
			}
			else
			{
				double fSrcRed, fSrcGreen, fSrcBlue;
				double fTgtRed, fTgtGreen, fTgtBlue;
				targetIt.pixelIt.GetPixel(fTgtRed, fTgtGreen, fTgtBlue);
				sourceIt.pixelIt.GetPixel(fSrcRed, fSrcGreen, fSrcBlue);

				if (m_bAddMode)
				{
					fTgtRed = std::min(std::max(0.0, fTgtRed + fSrcRed * m_fRedFactor), 256.0);
					fTgtGreen = std::min(std::max(0.0, fTgtGreen + fSrcGreen * m_fGreenFactor), 256.0);
					fTgtBlue = std::min(std::max(0.0, fTgtBlue + fSrcBlue * m_fBlueFactor), 256.0);
				}
				else
				{
					fTgtRed = std::max(m_fMinimum, fTgtRed - fSrcRed * m_fRedFactor);
					fTgtGreen = std::max(m_fMinimum, fTgtGreen - fSrcGreen * m_fGreenFactor);
					fTgtBlue = std::max(m_fMinimum, fTgtBlue - fSrcBlue * m_fBlueFactor);
				}
				targetIt.pixelIt.SetPixel(fTgtRed, fTgtGreen, fTgtBlue);
			}

			++targetIt.pixelIt;
			++sourceIt.pixelIt;
		}
		targetIt.pixelIt += extraWidth;
		sourceIt.pixelIt += extraWidth;

		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(row);
	}

	if (m_pProgress != nullptr)
		m_pProgress->End2();
}


bool Subtract(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, ProgressBase* pProgress, const double fRedFactor, const double fGreenFactor, const double fBlueFactor)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	// Check and remove super pixel settings
	CFATRANSFORMATION CFATransform = CFAT_NONE;
	CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pTarget.get());
	if (pCFABitmapInfo != nullptr)
	{
		CFATransform = pCFABitmapInfo->GetCFATransformation();
		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseBilinear(true);
	}

	// Check that it is the same sizes
	if (static_cast<bool>(pTarget) && static_cast<bool>(pSource))
	{
		if ((pTarget->RealWidth() == pSource->RealWidth()) &&
			(pTarget->RealHeight() == pSource->RealHeight()) &&
			(pTarget->IsMonochrome() == pSource->IsMonochrome()))
		{

			CSubtractTask SubtractTask;
			SubtractTask.Init(pTarget, pSource, pProgress, fRedFactor, fGreenFactor, fBlueFactor);
			SubtractTask.process();
		}
		else
		{
			ZTRACE_RUNTIME("Target.Width = %d, Source.RealWidth = %d", pTarget->RealWidth(), pSource->RealWidth());
			ZTRACE_RUNTIME("Target.Height = %d, Source.RealHeight = %d", pTarget->RealHeight(), pSource->RealHeight());
			ZTRACE_RUNTIME("Subtraction skipped");
		}
	};

	if (CFATransform == CFAT_SUPERPIXEL)
		pCFABitmapInfo->UseSuperPixels(true);

	return bResult;
};

/* ------------------------------------------------------------------- */

bool ShiftAndSubtract(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, ProgressBase* pProgress, double fXShift, double fYShift)
{
	ZFUNCTRACE_RUNTIME();

	// Check that it is the same sizes
	if (static_cast<bool>(pTarget) && static_cast<bool>(pSource))
	{
		if (pTarget->RealWidth() == pSource->RealWidth() && pTarget->RealHeight() == pSource->RealHeight() && pTarget->IsMonochrome() == pSource->IsMonochrome())
		{
			CSubtractTask SubtractTask;

			SubtractTask.Init(pTarget, pSource, pProgress, 1.0, 1.0, 1.0);
			SubtractTask.SetShift(fXShift, fYShift);
			SubtractTask.SetMinimumValue(1.0);
			SubtractTask.process();
		}
	}

	return true;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool Add(std::shared_ptr<CMemoryBitmap> pTarget, std::shared_ptr<const CMemoryBitmap> pSource, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	// Check that it is the same sizes
	if (static_cast<bool>(pTarget) && static_cast<bool>(pSource))
	{
		if (pTarget->RealWidth() == pSource->RealWidth() && pTarget->RealHeight() == pSource->RealHeight() && pTarget->IsMonochrome() == pSource->IsMonochrome())
		{
			CSubtractTask AddTask;

			AddTask.SetAddMode(true);
			AddTask.Init(pTarget, pSource, pProgress, 1.0, 1.0, 1.0);
			AddTask.process();
		}
	}

	return bResult;
}





std::shared_ptr<CMemoryBitmap> GetFilteredImage(const CMemoryBitmap* pInBitmap, const int lFilterSize, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	if (pInBitmap == nullptr)
		return std::shared_ptr<CMemoryBitmap>{};
	else
		return pInBitmap->GetMedianFilterEngine()->GetFilteredImage(lFilterSize, pProgress);
}

//////////////////////////////////////////////////////////////////////////
// Moved from headers

void FormatFromMethod(QString& strText, MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
{
	strText = "";
	switch (Method)
	{
	case MBP_FASTAVERAGE:
	case MBP_AVERAGE:
		strText = QCoreApplication::translate("BitmapExt", "Average", "IDS_RECAP_AVERAGE");
		break;
	case MBP_MEDIAN:
		strText = QCoreApplication::translate("BitmapExt", "Median", "IDS_RECAP_MEDIAN");
		break;
	case MBP_MAXIMUM:
		strText = QCoreApplication::translate("BitmapExt", "Maximum", "IDS_RECAP_MAXIMUM");
		break;
	case MBP_SIGMACLIP:
		strText = QCoreApplication::translate("BitmapExt", "Kappa-Sigma (Kappa = %1, Iterations = %2)", "IDS_RECAP_KAPPASIGMA").arg(fKappa, 0, 'f', 2).arg(lNrIterations);
		break;
	case MBP_AUTOADAPTIVE:
		strText = QCoreApplication::translate("BitmapExt", "Auto Adaptive Weighted Average (Iterations = %1)", "IDS_RECAP_AUTOADAPTIVE").arg(lNrIterations);
		break;
	case MBP_ENTROPYAVERAGE:
		strText = QCoreApplication::translate("BitmapExt", "Entropy Weighted Average", "IDS_RECAP_ENTROPYAVERAGE");
		break;
	case MBP_MEDIANSIGMACLIP:
		strText = QCoreApplication::translate("BitmapExt", "Median Kappa-Sigma (Kappa = %1, Iterations = %2)", "IDS_RECAP_MEDIANSIGMACLIP").arg(fKappa, 0, 'f', 2).arg(lNrIterations);
	};
}

void FormatMethod(QString& strText, MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
{
	strText = "";
	switch (Method)
	{
	case MBP_FASTAVERAGE:
	case MBP_AVERAGE:
		strText = QCoreApplication::translate("StackRecap", "Average", "IDS_RECAP_AVERAGE");
		break;
	case MBP_MEDIAN:
		strText = QCoreApplication::translate("StackRecap", "Median", "IDS_RECAP_MEDIAN");
		break;
	case MBP_MAXIMUM:
		strText = QCoreApplication::translate("StackRecap", "Maximum", "IDS_RECAP_MAXIMUM");
		break;
	case MBP_SIGMACLIP:
		strText = QCoreApplication::translate("StackRecap", "Kappa-Sigma (Kappa = %1, Iterations = %2)",
			"IDS_RECAP_KAPPASIGMA")
			.arg(fKappa, 0, 'f', 2)
			.arg(lNrIterations);
		break;
	case MBP_AUTOADAPTIVE:
		strText = QCoreApplication::translate("StackRecap",
			"Auto Adaptive Weighted Average (Iterations = %1)",
			"IDS_RECAP_AUTOADAPTIVE")
			.arg(lNrIterations);
		break;
	case MBP_ENTROPYAVERAGE:
		strText = QCoreApplication::translate("StackRecap",
			"Entropy Weighted Average",
			"IDS_RECAP_ENTROPYAVERAGE");
		break;
	case MBP_MEDIANSIGMACLIP:
		strText = QCoreApplication::translate("StackRecap",
			"Median Kappa-Sigma (Kappa = %1, Iterations = %2)",
			"IDS_RECAP_MEDIANSIGMACLIP")
			.arg(fKappa, 0, 'f', 2)
			.arg(lNrIterations);
	};
	return;
}

void CYMGToRGB2(double fCyan, double fYellow, double fMagenta, double, double& fRed, double& fGreen, double& fBlue)
{
	double			fR, fG, fB;

	fR = (fMagenta + fYellow - fCyan) / 2.0;
	fG = (fYellow + fCyan - fMagenta) / 2.0;
	fB = (fMagenta + fCyan - fYellow) / 2.0;

	fRed = 2.088034662 * fR + -3.663103328 * fG + 3.069027325 * fB;
	fGreen = -0.28607719 * fR + 1.706598409 * fG + 0.24881043 * fB;
	fBlue = -0.180853396 * fR + -7.714219397 * fG + 9.438903145 * fB;

	fRed = max(0.0, min(255.0, fRed));
	fGreen = max(0.0, min(255.0, fGreen));
	fBlue = max(0.0, min(255.0, fBlue));
}

//////////////////////////////////////////////////////////////////////////
C32BitsBitmap::C32BitsBitmap()
{
	m_hBitmap = nullptr;
	m_lpBits = nullptr;
	m_lWidth = 0;
	m_lHeight = 0;
	m_pLine = nullptr;
	m_dwByteWidth = 0;
}
C32BitsBitmap::~C32BitsBitmap()
{
	Free();
}
void C32BitsBitmap::InitInternals()
{
	if (m_pLine)
		free(m_pLine);

	m_pLine = static_cast<pByte*>(malloc(m_lHeight * sizeof(pByte)));
	if (nullptr == m_pLine)
	{
		ZOutOfMemory e("Could not allocate storage for scanline pointers");
		ZTHROW(e);
	}

	m_dwByteWidth = (((m_lWidth * 32 + 31) & ~31) >> 3);
	int			y = m_lHeight - 1;

	for (int i = 0; y >= 0; y--, i++)
	{
		m_pLine[i] = static_cast<pByte>(m_lpBits) + y * m_dwByteWidth;
	}
}
void C32BitsBitmap::Init(int lWidth, int lHeight)
{
	Create(lWidth, lHeight);
}
HBITMAP	C32BitsBitmap::Create(int lWidth, int lHeight)
{
	Free();

	HBITMAP			hBitmap;
	BITMAPINFO		bmpInfo;
	void* pBits;

	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth = lWidth;
	bmpInfo.bmiHeader.biHeight = lHeight;
	bmpInfo.bmiHeader.biPlanes = 1;;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = (int)(96 * 100.0 / 2.54);
	bmpInfo.bmiHeader.biYPelsPerMeter = (int)(96 * 100.0 / 2.54);
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	HDC				hDC;
	hDC = GetDC(nullptr);
	hBitmap = CreateDIBSection(hDC, &bmpInfo, 0, &pBits, nullptr, 0);
	ReleaseDC(nullptr, hDC);

	if (hBitmap)
	{
		m_hBitmap = hBitmap;
		m_lpBits = pBits;
		m_lWidth = lWidth;
		m_lHeight = lHeight;
		InitInternals();
	};

	return hBitmap;
}
void C32BitsBitmap::Free()
{
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
	};
	m_hBitmap = nullptr;
	m_lpBits = nullptr;
	if (m_pLine)
		free(m_pLine);
	m_pLine = nullptr;
}
HBITMAP C32BitsBitmap::Detach()
{
	HBITMAP hResult = m_hBitmap;

	m_hBitmap = nullptr;
	Free();

	return hResult;
};

COLORREF C32BitsBitmap::GetPixel(int x, int y)
{
	COLORREF crColor = RGB(0, 0, 0);

	if ((x >= 0) && (x < m_lWidth) && (y >= 0) && (y < m_lHeight))
	{
		const pByte pPixel = m_pLine[y] + ((x * 32) >> 3);
		const auto dwPixel = *reinterpret_cast<std::uint32_t*>(pPixel);
		const RGBQUAD rgbq = *reinterpret_cast<const RGBQUAD*>(&dwPixel);

		crColor = RGB(rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue);
	};

	return crColor;
}
pByte C32BitsBitmap::GetPixelBase(int x, int y)
{
	return m_pLine[y] + x * 4;
}
void C32BitsBitmap::SetPixel(int x, int y, COLORREF crColor)
{
	if ((x >= 0) && (x < m_lWidth) && (y >= 0) && (y < m_lHeight))
	{
		pByte pPixel = m_pLine[y] + ((x * 32) >> 3);
		alignas(std::uint32_t) RGBQUAD rgbq;

		rgbq.rgbRed = GetRValue(crColor);
		rgbq.rgbGreen = GetGValue(crColor);
		rgbq.rgbBlue = GetBValue(crColor);
		rgbq.rgbReserved = 0;

		*reinterpret_cast<std::uint32_t*>(pPixel) = *reinterpret_cast<std::uint32_t*>(&rgbq);
	};
}

//////////////////////////////////////////////////////////////////////////
void CAllDepthBitmap::Clear()
{
	m_pBitmap.reset();
	m_pWndBitmap.reset();
	m_Image.reset();
}

//////////////////////////////////////////////////////////////////////////
CBitmapInfo::CBitmapInfo()
{
	Init();
}

CBitmapInfo::CBitmapInfo(const CBitmapInfo& bi)
{
	CopyFrom(bi);
}

CBitmapInfo::CBitmapInfo(const fs::path& fileName)
{
	Init();
	m_strFileName = fileName;
}

void CBitmapInfo::CopyFrom(const CBitmapInfo& bi)
{
	m_strFileName = bi.m_strFileName;
	m_strFileType = bi.m_strFileType;
	m_strModel = bi.m_strModel;
	m_lISOSpeed = bi.m_lISOSpeed;
	m_lGain = bi.m_lGain;
	m_fExposure = bi.m_fExposure;
	m_fAperture = bi.m_fAperture;
	m_lWidth = bi.m_lWidth;
	m_lHeight = bi.m_lHeight;
	m_lBitsPerChannel = bi.m_lBitsPerChannel;
	m_lNrChannels = bi.m_lNrChannels;
	m_bCanLoad = bi.m_bCanLoad;
	m_bFloat = bi.m_bFloat;
	m_CFAType = bi.m_CFAType;
	m_bMaster = bi.m_bMaster;
	m_bFITS16bit = bi.m_bFITS16bit;
	m_strDateTime = bi.m_strDateTime;
	m_DateTime = bi.m_DateTime;
	m_InfoTime = bi.m_InfoTime;
	m_ExtraInfo = bi.m_ExtraInfo;
	m_xBayerOffset = bi.m_xBayerOffset;
	m_yBayerOffset = bi.m_yBayerOffset;
	m_filterName = bi.m_filterName;
}

void CBitmapInfo::Init()
{
	m_lWidth = 0;
	m_lHeight = 0;
	m_lBitsPerChannel = 0;
	m_lNrChannels = 0;
	m_bCanLoad = false;
	m_CFAType = CFATYPE_NONE;
	m_bMaster = false;
	m_bFloat = false;
	m_lISOSpeed = 0;
	m_lGain = -1;
	m_fExposure = 0.0;
	m_fAperture = 0.0;
	m_bFITS16bit = false;
	m_xBayerOffset = 0;
	m_yBayerOffset = 0;
}

CBitmapInfo& CBitmapInfo::operator=(const CBitmapInfo& bi)
{
	CopyFrom(bi);
	return (*this);
}

bool CBitmapInfo::operator<(const CBitmapInfo& other) const
{
	return (m_strFileName.compare(other.m_strFileName) < 0);
}

bool CBitmapInfo::operator==(const CBitmapInfo& other) const
{
	return this->m_strFileName.compare(other.m_strFileName) == 0;
}
bool CBitmapInfo::CanLoad() const
{
	return m_bCanLoad;
}

bool CBitmapInfo::IsCFA()
{
	return (m_CFAType != CFATYPE_NONE);
};

bool CBitmapInfo::IsMaster()
{
	return m_bMaster;
};

void CBitmapInfo::GetDescription(QString& strDescription)
{
	strDescription = m_strFileType;
	if (m_strModel.length() > 0)
		strDescription = m_strFileType + " " + m_strModel;
};

bool CBitmapInfo::IsInitialized()
{
	return m_lWidth && m_lHeight;
};