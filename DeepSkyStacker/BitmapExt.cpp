#include <stdafx.h>
#include <tiffio.h>
#include "BitmapExt.h"
#include "DSSTools.h"
#include "DSSProgress.h"

#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "RAWUtils.h"
#include <list>
#include <float.h>
#include "Multitask.h"
#include "Workspace.h"
#include <iostream>
#include <zexcept.h>
#include <omp.h>


#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef PCL_PROJECT
#include <PCLTools.h>
#endif

/* ------------------------------------------------------------------- */

void	CYMGToRGB12(double fCyan, double fYellow, double fMagenta, double fGreen2, double & fRed, double & fGreen, double & fBlue)
{
	double			Y, U, V;
	double			R, G, B;

	Y = (fCyan+fYellow+fMagenta+fGreen2)/2.0;
	U = -(fMagenta+fCyan-fYellow-fGreen2);//*0.492;
	V = (fMagenta+fYellow-fCyan-fGreen2);//*0.877;

	R   = 1.164*Y + 1.596 * (V-128.0);
	G = 1.164*Y -0.813*(V-128.0)-0.391*(U-128);
	B  = 1.164*Y + 2.018*(U-128.0);


	fRed	= R * 1.29948 + G * 0.0289296 - B * 0.934432;
	fGreen	= -0.409754*R + 1.31042 *G  - 0.523692*B;
	fBlue	= 0.110277*R  - 0.339351*G + 2.45812*B;

//	fRed = (Y+1.13983*V)*255.0;
//	fGreen = (Y-0.39465*U-0.5806*V)*255.0;
//	fBlue  = (Y+2.03211*U)*255.0;

	fRed = std::max(0.0, std::min (255.0, fRed));
	fGreen = std::max(0.0, std::min (255.0, fGreen));
	fBlue = std::max(0.0, std::min (255.0, fBlue));
};


void	CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double & fRed, double & fGreen, double & fBlue)
{
	fRed = fGreen = fBlue = 0;
	// Basic formulae
	// M = R + B
	// Y = R + G
	// C = B + G


	// RGB from CYM
	// R = (M+Y-C)/2
	// G = (Y+C-M)/2
	// B = (M+C-Y)/2
	fRed   = std::max(0.0, fMagenta+fYellow-fCyan)/2.0;
	fGreen = std::max(0.0, fYellow+fCyan-fMagenta)/2.0;
	fBlue  = std::max(0.0 ,fMagenta+fCyan-fYellow)/2.0;

/*	if (fGreen2)
	{
		fRed  *= fGreen2/fGreen;
		fBlue *= fGreen2/fGreen;
		fGreen = fGreen2;
	};
	fRed = std::min(fRed, 255);
	fBlue = std::min(fBlue, 255);
	fGreen = std::min(fGreen, 255);*/

	// RGB from CYG
	// G = G
	// R = Y - G
	// B = C - G
	fGreen += fGreen2;
	fRed   += fYellow-fGreen2;
	fBlue  += fCyan-fGreen2;

	// RGB from CMG
	// G = G
	// B = C - G
	// R = M - B = M - C + G
//	fGreen += fGreen2;
	fBlue  += fCyan-fGreen2;
	fRed   += fMagenta - fCyan + fGreen2;

	// RGB from YMG
	// G = G
	// R = Y - G
	// B = M - R = M - Y + G
//	fGreen += fGreen2;
	fRed   += fYellow - fGreen2;
	fBlue  += fMagenta - fYellow + fGreen2;

	// Average the results
	fRed /= 4.0;
	fBlue /= 4.0;
	fGreen /= 2.0;

	double	R = fRed;
	double	B = fBlue;
	double	G = fGreen;

	fRed	= R * 1.29948 + G * 0.0289296 - B * 0.934432;
	fGreen	= -0.409754*R + 1.31042 *G  - 0.523692*B;
	fBlue	= 0.110277*R  - 0.339351*G + 2.45812*B;

	fRed = std::max(0.0, std::min (255.0, fRed));
	fGreen = std::max(0.0, std::min (255.0, fGreen));
	fBlue = std::max(0.0, std::min (255.0, fBlue));
};


/* ------------------------------------------------------------------- */

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	ZFUNCTRACE_RUNTIME();
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = nullptr;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == nullptr)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

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

/* ------------------------------------------------------------------- */
#if DSSFILEDECODING==1

bool	DebayerPicture(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool						bResult = false;
	CSmartPtr<CMemoryBitmap>	pOutBitmap;

	*ppOutBitmap = nullptr;
	if (pInBitmap && pInBitmap->IsCFA())
	{
		C16BitGrayBitmap *			pGrayBitmap;
		CCFABitmapInfo *			pCFABitmapInfo;

		pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(pInBitmap);
		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pInBitmap);

		if (pGrayBitmap && pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
		{
			// AHD Demosaicing of the image
			AHDDemosaicing(pGrayBitmap, &pOutBitmap, pProgress);
		}
		else
		{
			// Transform the gray scale image to color image
			CSmartPtr<C48BitColorBitmap>	pColorBitmap;
			int							lWidth = pInBitmap->Width(),
											lHeight = pInBitmap->Height();
			PixelIterator					it;

			pColorBitmap.Create();
			pColorBitmap->Init(lWidth, lHeight);
			pColorBitmap->GetIterator(&it);

//#if defined(_OPENMP)	Don't use OpenMP here - doesn't mix with Pixel Iterator
//#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1)
//#endif
			for (int j = 0;j<lHeight;j++)
			{
				for (int i = 0;i<lWidth;i++)
				{
					double			fRed, fGreen, fBlue;

					pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
					it->SetPixel(fRed, fGreen, fBlue);
					(*it)++;
				};
			};

			pOutBitmap = pColorBitmap;
		};
		bResult = pOutBitmap.CopyTo(ppOutBitmap);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
bool	CAllDepthBitmap::initQImage()
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	int64_t		i, j;
	size_t			width = m_pBitmap->Width(), height = m_pBitmap->Height();
	const int numberOfProcessors = CMultitask::GetNrProcessors();

	m_Image = std::make_shared<QImage>((int)width, (int)height, QImage::Format_RGB32);
	//
	// Point to the first RGB quad in the QImage
	//
	QRgb* pOutPixel = (QRgb*)(m_Image->bits());

	if (m_pBitmap->IsMonochrome() && m_pBitmap->IsCFA())
	{
		ZTRACE_RUNTIME("Slow Bitmap Copy to Qimage");
		// Slow Method
#pragma omp parallel for default(none) if(numberOfProcessors > 1)
		for (j = 0; j < height; j++)
		{
			for (i = 0; i < width; i++)
			{
				double			fRed, fGreen, fBlue;
				m_pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

				*pOutPixel++ = qRgb(std::clamp(fRed, 0.0, 255.0),
					std::clamp(fGreen, 0.0, 255.0),
					std::clamp(fBlue, 0.0, 255.0));

			};
		};
	}
	else
	{
		ZTRACE_RUNTIME("Fast Bitmap Copy to QImage");
		// Fast Method
		PixelIterator			it;
		m_pBitmap->GetIterator(&it);

#pragma omp parallel for default(none) if(numberOfProcessors > 1)
		for (j = 0; j < height; j++)
		{
			it->Reset(0, j);
			for (i = 0; i < width; i++)
			{
				double			fRed, fGreen, fBlue;
				it->GetPixel(fRed, fGreen, fBlue);

				*pOutPixel++ = qRgb(std::clamp(fRed, 0.0, 255.0),
					std::clamp(fGreen, 0.0, 255.0),
					std::clamp(fBlue, 0.0, 255.0));
				(*it)++;
			};
		};
	};
	bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */
bool	LoadPicture(LPCTSTR szFileName, CAllDepthBitmap & AllDepthBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool						bResult = false;
	size_t lWidth, lHeight;

	try
	{
		AllDepthBitmap.Clear();

		if (LoadPicture(szFileName, &(AllDepthBitmap.m_pBitmap), pProgress))
		{
			C16BitGrayBitmap *			pGrayBitmap;
			CSmartPtr<CMemoryBitmap>	pBitmap = AllDepthBitmap.m_pBitmap;
			CCFABitmapInfo *			pCFABitmapInfo;

			lWidth = pBitmap->Width(),
			lHeight = pBitmap->Height();

			pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(AllDepthBitmap.m_pBitmap.m_p);
			pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pBitmap.m_p);
			if (pBitmap->IsCFA())
			{
				if (AllDepthBitmap.m_bDontUseAHD &&
					(pCFABitmapInfo->GetCFATransformation() == CFAT_AHD))
					pCFABitmapInfo->UseBilinear(true);

				if (pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
				{
					// AHD Demosaicing of the image
					CSmartPtr<CMemoryBitmap>		pColorBitmap;

					AHDDemosaicing(pGrayBitmap, &pColorBitmap, nullptr);

					AllDepthBitmap.m_pBitmap = pColorBitmap;
				}
				else
				{
					// Transform the gray scale image to color image
					CSmartPtr<C48BitColorBitmap>	pColorBitmap;

					pColorBitmap.Create();
					pColorBitmap->Init((int)lWidth, (int)lHeight);

#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
					for (int j = 0; j < lHeight; j++)
					{
						for (int i = 0; i < lWidth; i++)
						{
							double			fRed, fGreen, fBlue;

							pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
							pColorBitmap->SetPixel(i, j, fRed, fGreen, fBlue);
						};
					};

					AllDepthBitmap.m_pBitmap = pColorBitmap;
				};
			};

			//
			// Create a Windows bitmap for display purposes (wrapped in a C32BitsBitmap class).
			// (TODO) Delete this when Qt porting is done.
			//
			AllDepthBitmap.m_pWndBitmap.Create();
			AllDepthBitmap.m_pWndBitmap->InitFrom(AllDepthBitmap.m_pBitmap);

			//
			// Create a QImage from the raw data
			//
			AllDepthBitmap.initQImage();

			bResult = true;
		};
	}
	catch (std::exception & e)
	{
		CString errorMessage(static_cast<LPCTSTR>(CA2CT(e.what())));
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);
	}
#ifndef _CONSOLE
	catch (CException & e)
	{
		e.ReportError();
		e.Delete();
		exit(1);
	}
#endif
	catch (ZException & ze)
	{
		CString errorMessage;
		CString name(CA2CT(ze.name()));
		CString fileName(CA2CT(ze.locationAtIndex(0)->fileName()));
		CString functionName(CA2CT(ze.locationAtIndex(0)->functionName()));
		CString text(CA2CT(ze.text(0)));

		errorMessage.Format(
			_T("Exception %s thrown from %s Function: %s() Line: %lu\n\n%s"),
			name,
			fileName,
			functionName,
			ze.locationAtIndex(0)->lineNumber(),
			text);
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);
	}
	catch (...)
	{
		CString errorMessage(_T("Unknown exception caught"));
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
		exit(1);
	}



	return bResult;
};

/* ------------------------------------------------------------------- */

bool LoadOtherPicture(LPCTSTR szFileName, CMemoryBitmap** ppBitmap, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool							bResult = false;
	CSmartPtr<C24BitColorBitmap>	pBitmap;

	std::unique_ptr<Gdiplus::Bitmap> pSrcBitmap = std::make_unique<Gdiplus::Bitmap>(CComBSTR(szFileName));
	if (pSrcBitmap.get() != nullptr) // This is actually useless, because make_unique throws on out-of-memory.
	{
		pBitmap.Attach(new C24BitColorBitmap());
		ZTRACE_RUNTIME("Creating 8 bit RGB memory bitmap %p (%s)", pBitmap.m_p, szFileName);
		if (pBitmap)
		{
			const int lWidth = static_cast<int>(pSrcBitmap->GetWidth());
			const int lHeight = static_cast<int>(pSrcBitmap->GetHeight());

			Gdiplus::Rect rc(0, 0, lWidth - 1, lHeight - 1);
			Gdiplus::BitmapData bitmapData;

			if (pProgress)
				pProgress->Start2(nullptr, lHeight);
			pBitmap->Init(lWidth, lHeight);

			if (pSrcBitmap->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &bitmapData) == Gdiplus::Status::Ok)
			{
				std::uint8_t* pBasePixels = static_cast<std::uint8_t*>(bitmapData.Scan0);
					  
				std::uint8_t* pRedPixel	= pBitmap->GetRedPixel(0, 0);
				std::uint8_t* pGreenPixel = pBitmap->GetGreenPixel(0, 0);
				std::uint8_t* pBluePixel	= pBitmap->GetBluePixel(0, 0);

				for (int j = 0; j < lHeight; j++)
				{
					std::uint8_t* pPixel = pBasePixels;

					for (int i = 0; i < lWidth; i++)
					{
						*pBluePixel++ = *pPixel++;
						*pGreenPixel++ = *pPixel++;
						*pRedPixel++ = *pPixel++;
					};
					if (pProgress)
						pProgress->Progress2(nullptr, j+1);
					pBasePixels += std::abs(bitmapData.Stride);
				};

				if (pProgress)
					pProgress->End2();

				pSrcBitmap->UnlockBits(&bitmapData);

				C24BitColorBitmap* p24Bitmap;
				pBitmap.CopyTo(&p24Bitmap);
				*ppBitmap = dynamic_cast<CMemoryBitmap*>(p24Bitmap);

				CBitmapInfo bmpInfo;
				if (RetrieveEXIFInfo(szFileName, bmpInfo))
					pBitmap->m_DateTime = bmpInfo.m_DateTime;

				bResult = true;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool RetrieveEXIFInfo(Gdiplus::Bitmap* pBitmap, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	const auto getExifItem = [pBitmap, &bResult](const PROPID propertyId, const unsigned short type, auto& field) -> void
	{
		const UINT dwPropertySize = pBitmap->GetPropertyItemSize(propertyId);
		if (dwPropertySize != 0)
		{
			auto buffer = std::make_unique<std::uint8_t[]>(dwPropertySize);
			Gdiplus::PropertyItem* const propertyItem = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.get());

			if (propertyItem->type == type && pBitmap->GetPropertyItem(propertyId, dwPropertySize, propertyItem) == Gdiplus::Status::Ok)
			{
				if (propertyItem->type == PropertyTagTypeRational)
				{
					const std::uint32_t* pValues = static_cast<std::uint32_t*>(propertyItem->value);
					const std::uint32_t dwNumerator = *pValues;
					const std::uint32_t dwDenominator = *(pValues + 1);
					if (dwDenominator != 0)
					{
						if constexpr (std::is_same_v<decltype(field), double&>)
						{
							field = static_cast<double>(dwNumerator) / static_cast<double>(dwDenominator);
							bResult = true;
						}
					}
				}
				else if (propertyItem->type == PropertyTagTypeShort)
				{
					if constexpr (std::is_same_v<decltype(field), int&>)
					{
						const std::uint16_t* pValue = static_cast<std::uint16_t*>(propertyItem->value);
						field = static_cast<int>(*pValue);
						bResult = true;
					}
				}
				else if (propertyItem->type == PropertyTagTypeASCII)
				{
					if constexpr (std::is_same_v<decltype(field), CString&>)
					{
						field = static_cast<char*>(propertyItem->value);
						bResult = true;
					}
				}
			}
		}
	};

	if (pBitmap != nullptr)
	{
		getExifItem(PropertyTagExifExposureTime, PropertyTagTypeRational, BitmapInfo.m_fExposure);
		getExifItem(PropertyTagExifFNumber, PropertyTagTypeRational, BitmapInfo.m_fAperture);
		getExifItem(PropertyTagExifISOSpeed, PropertyTagTypeShort, BitmapInfo.m_lISOSpeed);

		getExifItem(PropertyTagEquipModel, PropertyTagTypeASCII, BitmapInfo.m_strModel);
		BitmapInfo.m_strModel.TrimRight();
		BitmapInfo.m_strModel.TrimLeft();

		CString strDateTime;
		getExifItem(PropertyTagDateTime, PropertyTagTypeASCII, strDateTime);
		// Parse the string : YYYY/MM/DD hh:mm:ss
		//                    0123456789012345678
		BitmapInfo.m_DateTime.wYear = _ttol(strDateTime.Left(4));
		BitmapInfo.m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
		BitmapInfo.m_DateTime.wDay = _ttol(strDateTime.Mid(8, 2));
		BitmapInfo.m_DateTime.wHour = _ttol(strDateTime.Mid(11, 2));
		BitmapInfo.m_DateTime.wMinute = _ttol(strDateTime.Mid(14, 2));
		BitmapInfo.m_DateTime.wSecond = _ttol(strDateTime.Mid(17, 2));

/*		UINT dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifExposureTime);
		if (dwPropertySize != 0)
		{
			auto buffer = std::make_unique<std::uint8_t[]>(dwPropertySize);
			// PropertyTagTypeRational
			Gdiplus::PropertyItem* const propertyItem = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.get());

			if (pBitmap->GetPropertyItem(PropertyTagExifExposureTime, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeRational)
				{
					std::uint32_t* pValues = static_cast<std::uint32_t*>(propertyItem->value);
					std::uint32_t dwNumerator, dwDenominator;

					dwNumerator = *pValues;
					dwDenominator = *(pValues + 1);

					if (dwDenominator != 0)
					{
						BitmapInfo.m_fExposure = static_cast<double>(dwNumerator) / static_cast<double>(dwDenominator);
						bResult = true;
					};
				};
			};
		};
*/
/*		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifFNumber);
		if (dwPropertySize)
		{
			// PropertyTagTypeRational
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);

			if (pBitmap->GetPropertyItem(PropertyTagExifFNumber, dwPropertySize, propertyItem) == Ok)
			{
				if (propertyItem->type == PropertyTagTypeRational)
				{
					UINT *			pValues = (UINT*)propertyItem->value;
					UINT			dwNumerator,
						dwDenominator;

					dwNumerator = *pValues;
					pValues++;
					dwDenominator = *pValues;

					if (dwDenominator)
					{
						BitmapInfo.m_fAperture = (double)dwNumerator / (double)dwDenominator;
						bResult = true;
					};
				};
			};

			free(propertyItem);
		};
*/
/*		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifISOSpeed);
		if (dwPropertySize)
		{
			// PropertyTagTypeShort
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);

			if (pBitmap->GetPropertyItem(PropertyTagExifISOSpeed, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeShort)
				{
					BitmapInfo.m_lISOSpeed = *((WORD*)propertyItem->value);
					bResult = true;
				};
			};

			free(propertyItem);
		};
*/
/*		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagEquipModel);
		if (dwPropertySize)
		{
			// PropertyTagTypeASCII
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);
			if (pBitmap->GetPropertyItem(PropertyTagEquipModel, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeASCII)
				{
					BitmapInfo.m_strModel = (char*)propertyItem->value;
					BitmapInfo.m_strModel.TrimRight();
					BitmapInfo.m_strModel.TrimLeft();
					bResult = true;
				};
			};

			free(propertyItem);
		};
*/
/*		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagDateTime);
		if (dwPropertySize)
		{
			// PropertyTagTypeASCII
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);
			if (pBitmap->GetPropertyItem(PropertyTagDateTime, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeASCII)
				{
					CString				strDateTime = (char*)propertyItem->value;

					// Parse the string : YYYY/MM/DD hh:mm:ss
					//                    0123456789012345678
					BitmapInfo.m_DateTime.wYear  = _ttol(strDateTime.Left(4));
					BitmapInfo.m_DateTime.wMonth = _ttol(strDateTime.Mid(5, 2));
					BitmapInfo.m_DateTime.wDay   = _ttol(strDateTime.Mid(8, 2));
					BitmapInfo.m_DateTime.wHour	 = _ttol(strDateTime.Mid(11, 2));
					BitmapInfo.m_DateTime.wMinute= _ttol(strDateTime.Mid(14, 2));
					BitmapInfo.m_DateTime.wSecond= _ttol(strDateTime.Mid(17, 2));

					bResult = true;
				};
			};

			free(propertyItem);
		};
*/
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool RetrieveEXIFInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	auto pBitmap = std::make_unique<Gdiplus::Bitmap>(CComBSTR(szFileName));
	return RetrieveEXIFInfo(pBitmap.get(), BitmapInfo);
};

/* ------------------------------------------------------------------- */

bool IsOtherPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	auto pBitmap = std::make_unique<Gdiplus::Bitmap>(CComBSTR(szFileName));

	if (pBitmap.get() != nullptr) // Useless, because make_unique (same as operator new) throws on out-of-memory.
	{
		GUID rawformat;

		if ((pBitmap->GetType() == ImageTypeBitmap) &&
			(pBitmap->GetRawFormat(&rawformat) == Ok))
		{
			bResult = true;
			if (rawformat == ImageFormatBMP)
				BitmapInfo.m_strFileType	= "Windows BMP";
			else if (rawformat == ImageFormatGIF)
				BitmapInfo.m_strFileType	= "GIF";
			else if (rawformat == ImageFormatJPEG)
				BitmapInfo.m_strFileType	= "JPEG";
			else if (rawformat == ImageFormatPNG)
				BitmapInfo.m_strFileType	= "PNG";
			else
				bResult = false;

			RetrieveEXIFInfo(pBitmap.get(), BitmapInfo);

			if (bResult)
			{
				BitmapInfo.m_strFileName	= szFileName;
				BitmapInfo.m_CFAType		= CFATYPE_NONE;
				BitmapInfo.m_lWidth			= pBitmap->GetWidth();
				BitmapInfo.m_lHeight		= pBitmap->GetHeight();
				BitmapInfo.m_lBitPerChannel	= 8;
				BitmapInfo.m_lNrChannels	= 3;
				BitmapInfo.m_bCanLoad		= true;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	C32BitsBitmap::CopyToClipboard()
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

bool	C32BitsBitmap::InitFrom(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	int			i, j;

	Free();
	if (pBitmap)
	{
		HBITMAP		hBitmap;
		hBitmap = Create(pBitmap->Width(), pBitmap->Height());
		if (hBitmap)
		{
			if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
			{
				ZTRACE_RUNTIME("Slow Bitmap Copy");
				// Slow Method
				for (j = 0;j<m_lHeight;j++)
				{
					LPBYTE			lpOut;
					LPRGBQUAD &		lpOutPixel = (LPRGBQUAD &)lpOut;

					lpOut = GetPixelBase(0, j);
					for (i = 0;i<m_lWidth;i++)
					{
						double			fRed, fGreen, fBlue;
						pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

						lpOutPixel->rgbRed		= std::min(std::max(0.0, fRed), 255.0);
						lpOutPixel->rgbGreen	= std::min(std::max(0.0, fGreen), 255.0);
						lpOutPixel->rgbBlue		= std::min(std::max(0.0, fBlue), 255.0);
						lpOutPixel->rgbReserved	= 0;

						lpOut += 4;
					};
				};
			}
			else
			{
				ZTRACE_RUNTIME("Fast Bitmap Copy");
				// Fast Method
				PixelIterator			it;

				pBitmap->GetIterator(&it);
				for (j = 0;j<m_lHeight;j++)
				{
					LPBYTE			lpOut;
					LPRGBQUAD &		lpOutPixel = (LPRGBQUAD &)lpOut;

					it->Reset(0, j);
					lpOut = GetPixelBase(0, j);
					for (i = 0;i<m_lWidth;i++)
					{
						double			fRed, fGreen, fBlue;
						it->GetPixel(fRed, fGreen, fBlue);

						lpOutPixel->rgbRed		= std::min(std::max(0.0, fRed), 255.0);
						lpOutPixel->rgbGreen	= std::min(std::max(0.0, fGreen), 255.0);
						lpOutPixel->rgbBlue		= std::min(std::max(0.0, fBlue), 255.0);
						lpOutPixel->rgbReserved	= 0;

						lpOut += 4;
						(*it)++;
					};
				};
			};
			bResult = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CGammaTransformation::InitTransformation(double fBlackPoint, double fGrayPoint, double fWhitePoint)
{
	ZFUNCTRACE_RUNTIME();
	const int lBlackPoint = static_cast<int>(65535.0 * fBlackPoint);
	const int lWhitePoint = static_cast<int>(65535.0 * fWhitePoint);

	constexpr int TransformSize = 65537;
	m_vTransformation.resize(TransformSize);

	CRationalInterpolation ri;
	ri.Initialize(fBlackPoint, fGrayPoint, fWhitePoint, 0, 0.5, 1.0);

	// Perform rational interpolation
	for (int i = 0; i < TransformSize; i++)
	{
		if (i <= lBlackPoint)
			m_vTransformation[i] = 0;
		else if (i >= lWhitePoint)
			m_vTransformation[i] = 255;
		else
		{
			const double fValue = ri.Interpolate(i / 65535.0);
			m_vTransformation[i] = 255.0 * fValue;//pow(fValue, fGamma);
		};
	};
};

/* ------------------------------------------------------------------- */

void CGammaTransformation::InitTransformation(double fGamma)
{
	ZFUNCTRACE_RUNTIME();
	double				fBlackPoint = 0.0,
						fWhitePoint = 1.0,
						fGrayPoint	= 0.5;

	fGrayPoint = pow(0.5, 1.0 / fGamma);
	InitTransformation(fBlackPoint, fGrayPoint, fWhitePoint);
};

/* ------------------------------------------------------------------- */

template <template<class> class BitmapClass, class T>
bool ApplyGammaTransformation(QImage* pImage, BitmapClass<T>* pInBitmap, CGammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.IsInitialized())
	{
		const size_t width = pInBitmap->Width();
		const size_t height = pInBitmap->Height();

		// Check that the output bitmap size is matching the input bitmap
		ZASSERTSTATE ((pImage->Width() == width) && (pImage->Height() == height));

		double const fMultiplier = pInBitmap->GetMultiplier() / 256.0;
		//
		// Point to the first RGB quad in the QImage
		//
		QRgb* pOutPixel = (QRgb*)(m_Image->bits());

#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
		for (int j = 0; j < lHeight; j++)
		{
			if constexpr (std::is_same_v<BitmapClass<T>, CColorBitmapT<T>>)
			{
				// Init iterators
				T* pRed = pInBitmap->GetRedPixel(0, j);
				T* pGreen = pInBitmap->GetGreenPixel(0, j);
				T* pBlue = pInBitmap->GetBluePixel(0, j);

				for (int i = 0; i < lWidth; i++)
				{
					*pOutPixel++ = qRgb(gammatrans.m_vTransformation[*pRed / fMultiplier],
										gammatrans.m_vTransformation[*pGreen / fMultiplier],
										gammatrans.m_vTransformation[*pBlue / fMultiplier]);
					pRed++;
					pGreen++;
					pBlue++;
				}
			}
			if constexpr (std::is_same_v<BitmapClass<T>, CGrayBitmapT<T>>)
			{
				// Init iterators
				T* pGray = pInBitmap->GetGrayPixel(0, j);
				unsigned char value = 0;

				for (int i = 0; i < lWidth; i++)
				{
					value = gammatrans.m_vTransformation[*pGray / fMultiplier];
					*pOutPixel++ = qRgb(value, value, value);
					pGray++;
				}
			}
		}
		bResult = true;
	}
	return bResult;
}

/* ------------------------------------------------------------------- */

template <template<class> class BitmapClass, class T>
bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, BitmapClass<T>* pInBitmap, CGammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.IsInitialized())
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

#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < lHeight; j++)
			{
				if constexpr (std::is_same_v<BitmapClass<T>, CColorBitmapT<T>>)
				{
					// Init iterators
					T* pRed = pInBitmap->GetRedPixel(0, j);
					T* pGreen = pInBitmap->GetGreenPixel(0, j);
					T* pBlue = pInBitmap->GetBluePixel(0, j);

					LPBYTE			pOut = pOutBitmap->GetPixelBase(0, j);
					LPRGBQUAD& pOutPixel = (LPRGBQUAD&)pOut;
					for (int i = 0; i < lWidth; i++)
					{
						pOutPixel->rgbRed = gammatrans.m_vTransformation[*pRed / fMultiplier];
						pOutPixel->rgbGreen = gammatrans.m_vTransformation[*pGreen / fMultiplier];
						pOutPixel->rgbBlue = gammatrans.m_vTransformation[*pBlue / fMultiplier];
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
					T* pGray = pInBitmap->GetGrayPixel(0, j);

					LPBYTE			pOut = pOutBitmap->GetPixelBase(0, j);
					LPRGBQUAD& pOutPixel = (LPRGBQUAD&)pOut;
					for (int i = 0; i < lWidth; i++)
					{
						pOutPixel->rgbRed = gammatrans.m_vTransformation[*pGray / fMultiplier];
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
/*
template <class TType>
bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, CColorBitmapT<TType>* pInBitmap, CGammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.IsInitialized())
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
			const double fMultiplier = pInBitmap->GetMultiplier() / 256.0;

#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < lHeight; j++)
			{
				// Init iterators
				TType*			pRed = pInBitmap->GetRedPixel(0, j);
				TType*			pGreen = pInBitmap->GetGreenPixel(0, j);
				TType*			pBlue = pInBitmap->GetBluePixel(0, j);

				LPBYTE			pOut = pOutBitmap->GetPixelBase(0, j);
				LPRGBQUAD&		pOutPixel = (LPRGBQUAD&)pOut;
				for (int i = 0; i < lWidth; i++)
				{
					pOutPixel->rgbRed   = gammatrans.m_vTransformation[*pRed / fMultiplier];
					pOutPixel->rgbGreen = gammatrans.m_vTransformation[*pGreen / fMultiplier];
					pOutPixel->rgbBlue  = gammatrans.m_vTransformation[*pBlue / fMultiplier];
					pOutPixel->rgbReserved = 0;
					pRed++;
					pGreen++;
					pBlue++;
					pOut += 4;
				};
			};
			bResult = true;
		};
	};

	return bResult;
};

template <class TType>
bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, CGrayBitmapT<TType>* pInBitmap, CGammaTransformation& gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (pInBitmap != nullptr && gammatrans.IsInitialized())
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

#pragma omp parallel for default(none) if(CMultitask::GetNrProcessors() > 1) // Returns 1 if multithreading disabled by user, otherwise # HW threads
			for (int j = 0; j < lHeight; j++)
			{
				// Init iterators
				TType *			pGray = pInBitmap->GetGrayPixel(0, j);

				LPBYTE			pOut = pOutBitmap->GetPixelBase(0, j);
				LPRGBQUAD &		pOutPixel = (LPRGBQUAD &)pOut;
				for (int i = 0; i < lWidth; i++)
				{
					pOutPixel->rgbRed   = gammatrans.m_vTransformation[*pGray / fMultiplier];
					pOutPixel->rgbBlue  = pOutPixel->rgbRed;
					pOutPixel->rgbGreen = pOutPixel->rgbRed;
					pOutPixel->rgbReserved = 0;
					pGray++;
					pOut += 4;
				};
			};
			bResult = true;
		};
	};

	return bResult;
};
*/
/* ------------------------------------------------------------------- */
/*
bool ApplyGammaTransformation(C32BitsBitmap* pOutBitmap, CMemoryBitmap* pInBitmap, CGammaTransformation& gammatrans)
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
*/

/* ------------------------------------------------------------------- */

#endif // DSSFILEDECODING

/* ------------------------------------------------------------------- */
#pragma warning( push )
#pragma warning( disable: 4996 )

#include <concurrent_unordered_set.h>
#include <shared_mutex>

#pragma warning( pop )

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
			const auto& str = other.m_strFileName;
			return fnv1a_hash((const unsigned char*)(LPCWSTR)str, str.GetLength() * sizeof(CString::XCHAR));
		}
	};

	//typedef std::set<CBitmapInfo> InfoCache;
	// We absolutely must use a thread-safe cache, otherwise GetPictureInfo() crashes if used concurrently (e.g. with OpenMP).
	typedef concurrency::concurrent_unordered_set<CBitmapInfo, BitmapInfoHash<CBitmapInfo>> InfoCache;
	InfoCache g_sBitmapInfoCache;
	SYSTEMTIME g_BitmapInfoTime;
	std::shared_mutex bitmapInfoMutex;
}
/* ------------------------------------------------------------------- */

bool GetPictureInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

#if DSSFILEDECODING==0
	if (IsPCLPicture(szFileName, BitmapInfo))
		bResult = true;
#else
	// First try to find the info in the cache
	if (!g_sBitmapInfoCache.empty())
	{
		// Check that the cache is not old (more than 5 minutes)
		SYSTEMTIME			st;
		FILETIME			ft1, ft2;
		ULARGE_INTEGER		ulft1, ulft2;

		GetSystemTime(&st);

		SystemTimeToFileTime(&g_BitmapInfoTime, &ft1);
		SystemTimeToFileTime(&st, &ft2);

		ulft1.LowPart = ft1.dwLowDateTime;
		ulft1.HighPart = ft1.dwHighDateTime;

		ulft2.LowPart = ft2.dwLowDateTime;
		ulft2.HighPart = ft2.dwHighDateTime;

		const std::int64_t diff = ulft2.QuadPart - ulft1.QuadPart;

		// diff is in 100 of nanoseconds (1e6 millisecond = 1 nanosecond)
		constexpr std::int64_t MaxDiff = std::int64_t{ 10000 } * 1000 * 60 * 5;
		if (diff > MaxDiff)
		{
			std::lock_guard<std::shared_mutex> writeLock(bitmapInfoMutex); // clear() is NOT thread-safe => need a write-lock.
			g_sBitmapInfoCache.clear();
		}
		else
		{
			std::shared_lock<std::shared_mutex> readLock(bitmapInfoMutex);
			InfoCache::const_iterator it = g_sBitmapInfoCache.find(CBitmapInfo(szFileName));
			if (it != g_sBitmapInfoCache.cend())
			{
				BitmapInfo = *it;
				bResult = true;
			}
		}
	}

	if (!bResult)
	{
		if (IsRAWPicture(szFileName, BitmapInfo))
			bResult = true;
		else if (IsTIFFPicture(szFileName, BitmapInfo))
			bResult = true;
		else if (IsFITSPicture(szFileName, BitmapInfo))
			bResult = true;
		else if (IsOtherPicture(szFileName, BitmapInfo))
			bResult = true;

		if (bResult)
		{
			TCHAR			szTime[200];
			TCHAR			szDate[200];

			if (!BitmapInfo.m_DateTime.wYear)
			{
				// use the file creation time instead of the EXIF info
				FILETIME		FileTime;
				SYSTEMTIME		SystemTime;

				GetFileCreationDateTime(szFileName, FileTime);
				FileTimeToSystemTime(&FileTime, &SystemTime);
				SystemTimeToTzSpecificLocalTime(nullptr, &SystemTime, &SystemTime);
				BitmapInfo.m_DateTime = SystemTime;

			};

			GetSystemTime(&BitmapInfo.m_InfoTime);

			GetDateFormat(LOCALE_USER_DEFAULT, 0, &BitmapInfo.m_DateTime, nullptr, szDate, sizeof(szDate)/sizeof(TCHAR));
			GetTimeFormat(LOCALE_USER_DEFAULT, 0, &BitmapInfo.m_DateTime, nullptr, szTime, sizeof(szTime)/sizeof(TCHAR));

			BitmapInfo.m_strDateTime.Format(_T("%s %s"), szDate, szTime);

			std::shared_lock<std::shared_mutex> readLock(bitmapInfoMutex);
			if (g_sBitmapInfoCache.empty())
				GetSystemTime(&g_BitmapInfoTime);
			g_sBitmapInfoCache.insert(BitmapInfo);
		};
	};
#endif

	return bResult;
};

/* ------------------------------------------------------------------- */

bool LoadPicture(LPCTSTR szFileName, CMemoryBitmap** ppBitmap, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	if (ppBitmap != nullptr)
	{
		CSmartPtr<CMemoryBitmap> pBitmap;
		*ppBitmap = nullptr;

#if DSSFILEDECODING==0
		if (IsPCLPicture(szFileName, BitmapInfo))
			bResult = LoadPCLPicture(szFileName, &pBitmap, pProgress);
#else
		do  // Once only 
		{
			CBitmapInfo BitmapInfo;
			int loadResult = 0;

			if (IsRAWPicture(szFileName, BitmapInfo))
				bResult = LoadRAWPicture(szFileName, &pBitmap, pProgress);
			if (bResult)
				break;		// All done - file has been loaded 
			
			//
			// Meanings of loadResult:
			//
			//		-1		Not a file of the appropriate type
			//		0		File successfully loaded
			//		1		File failed to load
			//
			// If the file loaded or failed to load, leave the loop with an appropriate
			// value of bResult set.
			//
			loadResult = LoadTIFFPicture(szFileName, BitmapInfo, &pBitmap, pProgress);
			if (0 == loadResult)
			{
				bResult = true;
				break;		// All done - file has been loaded 
			}
			else if (1 == loadResult)
				break;		// All done - file failed to load

			//
			// It wasn't a TIFF file, so try to load a FITS file
			//
			loadResult = LoadFITSPicture(szFileName, BitmapInfo, &pBitmap, pProgress);
			if (0 == loadResult)
			{
				bResult = true;
				break;		// All done - file has been loaded 
			}
			else if (1 == loadResult)
				break;		// All done - file failed to load

			//
			// It wasn't a FITS file, so try to load other stuff ...
			//
			bResult = LoadOtherPicture(szFileName, &pBitmap, pProgress);

		} while (false);

#endif

		if (bResult)
			pBitmap.CopyTo(ppBitmap);
	}

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CreateBitmap(const CBitmapCharacteristics & bc, CMemoryBitmap ** ppOutBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool						bResult = false;
	CSmartPtr<CMemoryBitmap>	pBitmap;


	if (bc.m_lNrChannels == 1)
	{
		if (bc.m_lBitsPerPixel == 8)
		{
			pBitmap.Attach(new C8BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap %p", pBitmap.m_p);
		}
		else if (bc.m_lBitsPerPixel == 16)
		{
			pBitmap.Attach(new C16BitGrayBitmap());
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap %p", pBitmap.m_p);
		}
		else if (bc.m_lBitsPerPixel == 32)
		{
			if (bc.m_bFloat)
			{
				pBitmap.Attach(new C32BitFloatGrayBitmap());
				ZTRACE_RUNTIME("Creating 32 float Gray bit memory bitmap %p", pBitmap.m_p);
			}
			else
			{
				pBitmap.Attach(new C32BitGrayBitmap());
				ZTRACE_RUNTIME("Creating 32 Gray bit memory bitmap %p", pBitmap.m_p);
			};
		};
	}
	else if (bc.m_lNrChannels==3)
	{
		if (bc.m_lBitsPerPixel == 8)
		{
			pBitmap.Attach(new C24BitColorBitmap());
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap %p", pBitmap.m_p);
		}
		else if (bc.m_lBitsPerPixel == 16)
		{
			pBitmap.Attach(new C48BitColorBitmap());
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap %p", pBitmap.m_p);
		}
		else if (bc.m_lBitsPerPixel == 32)
		{
			if (bc.m_bFloat)
			{
				pBitmap.Attach(new C96BitFloatColorBitmap());
				ZTRACE_RUNTIME("Creating 32 float RGB bit memory bitmap %p", pBitmap.m_p);
			}
			else
			{
				pBitmap.Attach(new C96BitColorBitmap());
				ZTRACE_RUNTIME("Creating 32 RGB bit memory bitmap %p", pBitmap.m_p);
			};
		};
	};

	if (pBitmap)
	{
		pBitmap.CopyTo(ppOutBitmap);
		bResult = true;
	};

	return bResult;
};


class CSubtractTask
{
private :
	CSmartPtr<CMemoryBitmap>	m_pTarget;
	CSmartPtr<CMemoryBitmap>	m_pSource;
	double						m_fRedFactor;
	double						m_fGreenFactor;
	double						m_fBlueFactor;
	double						m_fGrayFactor;
	double						m_fXShift;
	double						m_fYShift;
	bool						m_bMonochrome;
	CDSSProgress *				m_pProgress;
	bool						m_bAddMode;
	double						m_fMinimum;

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

	void Init(CMemoryBitmap* pTarget, CMemoryBitmap* pSource, CDSSProgress* pProgress, const double fRedFactor, const double fGreenFactor, const double fBlueFactor)
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
			pProgress->Start2(nullptr, pTarget->RealWidth());
	}

	CSubtractTask&	SetShift(const double fXShift, const double fYShift)
	{
		m_fXShift	= fXShift;
		m_fYShift	= fYShift;
		return *this;
	}

	CSubtractTask&	SetAddMode(const bool bSet)
	{
		m_bAddMode = bSet;
		return *this;
	}

	CSubtractTask&	SetMinimumValue(const double fValue)
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

template <class T> struct thread_init {
	T& target;
	T& source;
	PixelIterator PixelItTgt;
	PixelIterator PixelItSrc;

	explicit thread_init(T& t, T& s) : target{ t }, source{ s } {
		target->GetIterator(&PixelItTgt);
		source->GetIterator(&PixelItSrc);
	}
	thread_init(const thread_init& rhs) : target{ rhs.target }, source{ rhs.source } {
		target->GetIterator(&PixelItTgt);
		source->GetIterator(&PixelItSrc);
	}
};

void CSubtractTask::process()
{
	ZFUNCTRACE_RUNTIME();
	const int height = m_pTarget->RealHeight() - (m_fYShift == 0 ? 0 : static_cast<int>(std::fabs(m_fYShift) + 0.5));
	const int nrProcessors = CMultitask::GetNrProcessors();

	if (m_pProgress != nullptr)
	{
		m_pProgress->Start2(nullptr, height);
		m_pProgress->SetNrUsedProcessors(nrProcessors);
	}

	const int extraWidth = m_fXShift == 0 ? 0 : static_cast<int>(std::fabs(m_fXShift) + 0.5);
	const int width = m_pTarget->RealWidth() - extraWidth;

	thread_init threadVars(m_pTarget, m_pSource);

#pragma omp parallel for schedule(guided) default(none) firstprivate(threadVars) if(nrProcessors > 1)
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
			lSrcStartX += std::fabs(m_fXShift) + 0.5;
		}
		if (m_fYShift > 0)
		{
			// Target is moved
			lTgtStartY += m_fYShift + 0.5;
		}
		else
		{
			// Source is moved
			lSrcStartY += std::fabs(m_fYShift) + 0.5;
		}

		threadVars.PixelItTgt->Reset(lTgtStartX, lTgtStartY);
		threadVars.PixelItSrc->Reset(lSrcStartX, lSrcStartY);

		for (int col = 0; col < width; ++col)
		{
			if (m_bMonochrome)
			{
				double fSrcGray, fTgtGray;
				threadVars.PixelItTgt->GetPixel(fTgtGray);
				threadVars.PixelItSrc->GetPixel(fSrcGray);

				if (m_bAddMode)
					fTgtGray = std::min(std::max(0.0, fTgtGray + fSrcGray * m_fGrayFactor), 256.0);
				else
					fTgtGray = std::max(m_fMinimum, fTgtGray - fSrcGray * m_fGrayFactor);
				threadVars.PixelItTgt->SetPixel(fTgtGray);
			}
			else
			{
				double fSrcRed, fSrcGreen, fSrcBlue;
				double fTgtRed, fTgtGreen, fTgtBlue;
				threadVars.PixelItTgt->GetPixel(fTgtRed, fTgtGreen, fTgtBlue);
				threadVars.PixelItSrc->GetPixel(fSrcRed, fSrcGreen, fSrcBlue);

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
				threadVars.PixelItTgt->SetPixel(fTgtRed, fTgtGreen, fTgtBlue);
			}

			(*threadVars.PixelItTgt)++;
			(*threadVars.PixelItSrc)++;
		}
		(*threadVars.PixelItTgt) += extraWidth;
		(*threadVars.PixelItSrc) += extraWidth;

		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(nullptr, row);
	}

	if (m_pProgress != nullptr)
	{
		m_pProgress->SetNrUsedProcessors();
		m_pProgress->End2();
	}
}


bool Subtract(CMemoryBitmap* pTarget, CMemoryBitmap* pSource, CDSSProgress* pProgress, const double fRedFactor, const double fGreenFactor, const double fBlueFactor)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	// Check and remove super pixel settings
	CFATRANSFORMATION CFATransform = CFAT_NONE;
	CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pTarget);
	if (pCFABitmapInfo != nullptr)
	{
		CFATransform = pCFABitmapInfo->GetCFATransformation();
		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseBilinear(true);
	}

	// Check that it is the same sizes
	if (pTarget != nullptr && pSource != nullptr)
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

bool ShiftAndSubtract(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress, double fXShift, double fYShift)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	// Check that it is the same sizes
	if (pTarget && pSource)
	{
		if ((pTarget->RealWidth() == pSource->RealWidth()) &&
			(pTarget->RealHeight() == pSource->RealHeight()) &&
			(pTarget->IsMonochrome() == pSource->IsMonochrome()))
		{
			CSubtractTask			SubtractTask;

			SubtractTask.Init(pTarget, pSource, pProgress, 1.0, 1.0, 1.0);
			SubtractTask.SetShift(fXShift, fYShift);
			SubtractTask.SetMinimumValue(1.0);
//			SubtractTask.StartThreads();
//			SubtractTask.Process();
			SubtractTask.process();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

bool Add(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;
	// Check that it is the same sizes
	if (pTarget && pSource)
	{
		if ((pTarget->RealWidth() == pSource->RealWidth()) &&
			(pTarget->RealHeight() == pSource->RealHeight()) &&
			(pTarget->IsMonochrome() == pSource->IsMonochrome()))
		{
			CSubtractTask		AddTask;

			AddTask.SetAddMode(true);
			AddTask.Init(pTarget, pSource, pProgress, 1.0, 1.0, 1.0);
//			AddTask.StartThreads();
//			AddTask.Process();
			AddTask.process();
		};
	};

	return bResult;
};


CFATYPE	GetCFAType(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	CFATYPE Result = CFATYPE_NONE;

	if (pBitmap != nullptr && pBitmap->IsCFA()) {
		if (CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pBitmap)) {
			Result = pCFABitmapInfo->GetCFAType();
		}
	}
	return Result;
};


bool GetFilteredImage(CMemoryBitmap* pInBitmap, CMemoryBitmap** ppOutBitmap, int lFilterSize, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	*ppOutBitmap = nullptr;
	if (pInBitmap != nullptr)
	{
		CSmartPtr<CMedianFilterEngine> pMedianFilterEngine;
		pInBitmap->GetMedianFilterEngine(&pMedianFilterEngine);

		bResult = pMedianFilterEngine->GetFilteredImage(ppOutBitmap, lFilterSize, pProgress);
	};

	return bResult;
};
