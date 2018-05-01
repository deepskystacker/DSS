#include <stdafx.h>
#include <tiffio.h>
#include "BitmapExt.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "Registry.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "RAWUtils.h"
#include <set>
#include <list>
#include <float.h>
#include "Multitask.h"
#include "Workspace.h"

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

	fRed = max(0, min (255.0, fRed));
	fGreen = max(0, min (255.0, fGreen));
	fBlue = max(0, min (255.0, fBlue));
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
	fRed   = max(0, fMagenta+fYellow-fCyan)/2.0;
	fGreen = max(0, fYellow+fCyan-fMagenta)/2.0;
	fBlue  = max(0 ,fMagenta+fCyan-fYellow)/2.0;

/*	if (fGreen2)
	{
		fRed  *= fGreen2/fGreen;
		fBlue *= fGreen2/fGreen;
		fGreen = fGreen2;
	};
	fRed = min(fRed, 255);
	fBlue = min(fBlue, 255);
	fGreen = min(fGreen, 255);*/

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

	fRed = max(0, min (255.0, fRed));
	fGreen = max(0, min (255.0, fGreen));
	fBlue = max(0, min (255.0, fBlue));
};


/* ------------------------------------------------------------------- */

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	ZFUNCTRACE_RUNTIME();
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
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

	hScreenDC = GetDC(NULL);
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
	ReleaseDC(NULL, hScreenDC);

	if (hTgtBmp)
	{
		OpenClipboard(NULL);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hTgtBmp);
		CloseClipboard();
	};
};

/* ------------------------------------------------------------------- */
#if DSSFILEDECODING==1

BOOL	DebayerPicture(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
	CSmartPtr<CMemoryBitmap>	pOutBitmap;

	*ppOutBitmap = NULL;
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
			LONG							lWidth = pInBitmap->Width(),
											lHeight = pInBitmap->Height();
			PixelIterator					it;

			pColorBitmap.Create();
			pColorBitmap->Init(lWidth, lHeight);
			pColorBitmap->GetIterator(&it);

			for (LONG j = 0;j<lHeight;j++)
			{
				for (LONG i = 0;i<lWidth;i++)
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

BOOL	LoadPicture(LPCTSTR szFileName, CAllDepthBitmap & AllDepthBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
	AllDepthBitmap.Clear();

	if (LoadPicture(szFileName, &(AllDepthBitmap.m_pBitmap), pProgress))
	{
		C16BitGrayBitmap *			pGrayBitmap;
		CSmartPtr<CMemoryBitmap>	pBitmap = AllDepthBitmap.m_pBitmap;
		CCFABitmapInfo *			pCFABitmapInfo;

		pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(AllDepthBitmap.m_pBitmap.m_p);
		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pBitmap.m_p);
		if (pBitmap->IsCFA())
		{
			if (AllDepthBitmap.m_bDontUseAHD && 
				(pCFABitmapInfo->GetCFATransformation() == CFAT_AHD))
				pCFABitmapInfo->UseBilinear(TRUE);

			if (pCFABitmapInfo->GetCFATransformation() == CFAT_AHD)
			{
				// AHD Demosaicing of the image
				CSmartPtr<CMemoryBitmap>		pColorBitmap;

				AHDDemosaicing(pGrayBitmap, &pColorBitmap, NULL);

				AllDepthBitmap.m_pBitmap = pColorBitmap;
			}
			else 
			{
				// Transform the gray scale image to color image
				CSmartPtr<C48BitColorBitmap>	pColorBitmap;
				LONG							lWidth = pBitmap->Width(),
												lHeight = pBitmap->Height();
				PixelIterator					it;

				pColorBitmap.Create();
				pColorBitmap->Init(lWidth, lHeight);
				pColorBitmap->GetIterator(&it);

				for (LONG j = 0;j<lHeight;j++)
				{
					for (LONG i = 0;i<lWidth;i++)
					{
						double			fRed, fGreen, fBlue;

						pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
						it->SetPixel(fRed, fGreen, fBlue);
						(*it)++;
					};
				};

				AllDepthBitmap.m_pBitmap = pColorBitmap;
			};
		};

		AllDepthBitmap.m_pWndBitmap.Create();
		AllDepthBitmap.m_pWndBitmap->InitFrom(AllDepthBitmap.m_pBitmap);

		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	LoadOtherPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL							bResult = FALSE;
	CSmartPtr<C24BitColorBitmap>	pBitmap;
	Bitmap	*						pSrcBitmap;

	pSrcBitmap = new Bitmap(CComBSTR(szFileName));
	if (pSrcBitmap)
	{
		pBitmap.Attach(new C24BitColorBitmap());
		ZTRACE_RUNTIME("Creating 8 bit RGB memory bitmap %p (%s)", pBitmap.m_p, szFileName);
		if (pBitmap)
		{
			LONG					lWidth  = pSrcBitmap->GetWidth();
			LONG					lHeight = pSrcBitmap->GetHeight();
			Rect					rc(0, 0, lWidth-1, lHeight-1);
			BitmapData 				bitmapData;
			
			if (pProgress)
				pProgress->Start2(NULL, lHeight);
			pBitmap->Init(lWidth, lHeight);

			if (pSrcBitmap->LockBits(&rc, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData) == Ok)
			{
				BYTE *				pBasePixels = (BYTE*)bitmapData.Scan0;

				BYTE *				pRedPixel	= pBitmap->GetRedPixel(0, 0);
				BYTE *				pGreenPixel = pBitmap->GetGreenPixel(0, 0);
				BYTE *				pBluePixel	= pBitmap->GetBluePixel(0, 0);

				for (LONG j = 0;j<lHeight;j++)
				{
					BYTE *			pPixel = pBasePixels;

					for (LONG i = 0;i<lWidth;i++)
					{
						*pBluePixel = *pPixel;
						pPixel++;
						*pGreenPixel = *pPixel;
						pPixel++;
						*pRedPixel = *pPixel;
						pPixel++;

						pRedPixel++;
						pGreenPixel++;
						pBluePixel++;
					};
					if (pProgress)
						pProgress->Progress2(NULL, j+1);
					pBasePixels += labs(bitmapData.Stride);
				};

				if (pProgress)
					pProgress->End2();

				pSrcBitmap->UnlockBits(&bitmapData);

				C24BitColorBitmap *	p24Bitmap;

				pBitmap.CopyTo(&p24Bitmap);
				*ppBitmap = dynamic_cast<CMemoryBitmap *>(p24Bitmap);

				CBitmapInfo				bmpInfo;
				if (RetrieveEXIFInfo(szFileName, bmpInfo))
					pBitmap->m_DateTime = bmpInfo.m_DateTime;

				bResult = TRUE;
			};
		};

		delete pSrcBitmap;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	RetrieveEXIFInfo(Bitmap * pBitmap, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	if (pBitmap)
	{
		UINT			dwPropertySize;

		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifExposureTime);
		if (dwPropertySize)
		{
			// PropertyTagTypeRational
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);
			
			if (pBitmap->GetPropertyItem(PropertyTagExifExposureTime, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeRational)
				{
					DWORD *			pValues = (DWORD*)propertyItem->value;
					DWORD			dwNumerator,
									dwDenominator;

					dwNumerator = *pValues;
					pValues++;
					dwDenominator = *pValues;

					if (dwDenominator)
					{
						BitmapInfo.m_fExposure = (double)dwNumerator/(double)dwDenominator;
						bResult = TRUE;
					};
				};
			};

			free(propertyItem);
		};
		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagExifISOSpeed);
		if (dwPropertySize)
		{
			// PropertyTagTypeShort
			PropertyItem* propertyItem = (PropertyItem*)malloc(dwPropertySize);

			if (pBitmap->GetPropertyItem(PropertyTagExifISOSpeed, dwPropertySize, propertyItem) == Ok)
			{
				if(propertyItem->type == PropertyTagTypeShort)
				{
					BitmapInfo.m_lISOSpeed = *((WORD*)propertyItem->value);
					bResult = TRUE;
				};
			};

			free(propertyItem);
		};

		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagEquipModel);
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
					bResult = TRUE;
				};
			};

			free(propertyItem);
		};

		dwPropertySize = pBitmap->GetPropertyItemSize(PropertyTagDateTime);
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

					bResult = TRUE;
				};
			};

			free(propertyItem);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	RetrieveEXIFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;
	Bitmap *				pBitmap;

	pBitmap = new Bitmap(CComBSTR(szFileName));

	if (pBitmap)
	{
		bResult = RetrieveEXIFInfo(pBitmap, BitmapInfo);
		delete pBitmap;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	IsOtherPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;
	Bitmap *				pBitmap;

	pBitmap = new Bitmap(CComBSTR(szFileName));

	if (pBitmap)
	{
		GUID				rawformat;

		if ((pBitmap->GetType() == ImageTypeBitmap) &&
			(pBitmap->GetRawFormat(&rawformat) == Ok))
		{
			bResult = TRUE;
			if (rawformat == ImageFormatBMP)
				BitmapInfo.m_strFileType	= "Windows BMP";
			else if (rawformat == ImageFormatGIF)
				BitmapInfo.m_strFileType	= "GIF";
			else if (rawformat == ImageFormatJPEG)
				BitmapInfo.m_strFileType	= "JPEG";
			else if (rawformat == ImageFormatPNG)
				BitmapInfo.m_strFileType	= "PNG";
			else 
				bResult = FALSE;

			RetrieveEXIFInfo(pBitmap, BitmapInfo);

			if (bResult)
			{
				BitmapInfo.m_strFileName	= szFileName;
				BitmapInfo.m_CFAType		= CFATYPE_NONE;
				BitmapInfo.m_lWidth			= pBitmap->GetWidth();
				BitmapInfo.m_lHeight		= pBitmap->GetHeight();
				BitmapInfo.m_lBitPerChannel	= 8;
				BitmapInfo.m_lNrChannels	= 3;
				BitmapInfo.m_bCanLoad		= TRUE;
			};
		};

		delete pBitmap;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	C32BitsBitmap::CopyToClipboard()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	DWORD			dwSize = 16+sizeof(BITMAPINFO)+m_dwByteWidth*m_lHeight;

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
			bmpInfo.bmiHeader.biXPelsPerMeter = (LONG)(96*100.0/2.54);
			bmpInfo.bmiHeader.biYPelsPerMeter = (LONG)(96*100.0/2.54);
			bmpInfo.bmiHeader.biClrUsed = 0;
			bmpInfo.bmiHeader.biClrImportant = 0;

			pGlobal = (char*)GlobalLock(hGlobal);
			memcpy(pGlobal, &bmpInfo, sizeof(BITMAPINFO));
			pGlobal += sizeof(BITMAPINFO);

			memcpy(pGlobal, m_lpBits, m_dwByteWidth*m_lHeight);

			GlobalUnlock(hGlobal);

			if (OpenClipboard(NULL/*AfxGetMainWnd()->GetSafeHwnd()*/))
			{
				EmptyClipboard();
				if (!SetClipboardData(CF_DIB, hGlobal))
					GlobalFree(hGlobal);
				else
					bResult = TRUE;
				CloseClipboard();
			}
			else
				GlobalFree(hGlobal);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	C32BitsBitmap::InitFrom(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	LONG			i, j;

	Free();
	if (pBitmap)
	{
		HBITMAP		hBitmap;
		hBitmap = Create(pBitmap->Width(), pBitmap->Height());
		if (hBitmap)
		{
			if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
			{
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

						lpOutPixel->rgbRed		= min(max(0, fRed), 255.0);
						lpOutPixel->rgbGreen	= min(max(0, fGreen), 255.0);
						lpOutPixel->rgbBlue		= min(max(0, fBlue), 255.0);
						lpOutPixel->rgbReserved	= 0;

						lpOut += 4;
					};
				};
			}
			else
			{
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

						lpOutPixel->rgbRed		= min(max(0, fRed), 255.0);
						lpOutPixel->rgbGreen	= min(max(0, fGreen), 255.0);
						lpOutPixel->rgbBlue		= min(max(0, fBlue), 255.0);
						lpOutPixel->rgbReserved	= 0;

						lpOut += 4;
						(*it)++;
					};
				};
			};
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CGammaTransformation::InitTransformation(double fBlackPoint, double fGrayPoint, double fWhitePoint)
{
	ZFUNCTRACE_RUNTIME();
	LONG				lBlackPoint = 65535.0*fBlackPoint,
						lWhitePoint = 65535.0*fWhitePoint;


	m_vTransformation.resize(65537L);

	CRationalInterpolation		ri;

	ri.Initialize(fBlackPoint, fGrayPoint, fWhitePoint, 0, 0.5, 1.0);

	// Perform rational interpolation
	for (LONG i = 0;i<m_vTransformation.size();i++)
	{
		if (i<=lBlackPoint)
			m_vTransformation[i] = 0;
		else if (i>=lWhitePoint)
			m_vTransformation[i] = 255;
		else
		{
			double			fValue = ri.Interpolate(i/65535.0);
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

	fGrayPoint = pow(0.5, 1.0/fGamma);
	InitTransformation(fBlackPoint, fGrayPoint, fWhitePoint);
};

/* ------------------------------------------------------------------- */

template <class TType>
BOOL	ApplyGammaTransformation(C32BitsBitmap * pOutBitmap, CColorBitmapT<TType> * pInBitmap, CGammaTransformation & gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (pInBitmap && gammatrans.IsInitialized())
	{
		BOOL			bContinue;
		LONG			lWidth = pInBitmap->Width(),
						lHeight = pInBitmap->Height();
		double			fMultiplier;

		if (pOutBitmap->IsEmpty())
		{
			// Create the Bitmap
			pOutBitmap->Init(pInBitmap->Width(), pInBitmap->Height());
		};

		// Check that the output bitmap size is matching the input bitmap
		bContinue = (pOutBitmap->Width() == pInBitmap->Width()) &&
				    (pOutBitmap->Height() == pInBitmap->Height());

		if (bContinue)
		{
			// Init iterators
			TType *			pRed  = pInBitmap->GetRedPixel(0, 0);
			TType *			pGreen= pInBitmap->GetGreenPixel(0, 0);
			TType *			pBlue = pInBitmap->GetBluePixel(0, 0);
			LPBYTE			pOut;
			LPRGBQUAD &		pOutPixel = (LPRGBQUAD &)pOut;

			fMultiplier = pInBitmap->GetMultiplier()/256.0;
			
			for (LONG j =  0;j<lHeight;j++)
			{
				pOut = pOutBitmap->GetPixelBase(0, j);
				for (LONG i = 0;i<lWidth;i++)
				{

					pOutPixel->rgbRed   = gammatrans.m_vTransformation[*pRed/fMultiplier];
					pOutPixel->rgbGreen = gammatrans.m_vTransformation[*pGreen/fMultiplier];
					pOutPixel->rgbBlue  = gammatrans.m_vTransformation[*pBlue/fMultiplier];
					pOutPixel->rgbReserved = 0;
					pRed++;
					pGreen++;
					pBlue++;
					pOut += 4;
				};
			};
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

template <class TType>
BOOL	ApplyGammaTransformation(C32BitsBitmap * pOutBitmap, CGrayBitmapT<TType> * pInBitmap, CGammaTransformation & gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (pInBitmap && gammatrans.IsInitialized())
	{
		BOOL			bContinue;
		LONG			lWidth = pInBitmap->Width(),
						lHeight = pInBitmap->Height();
		double			fMultiplier;

		if (pOutBitmap->IsEmpty())
		{
			// Create the Bitmap
			pOutBitmap->Init(pInBitmap->Width(), pInBitmap->Height());
		};

		// Check that the output bitmap size is matching the input bitmap
		bContinue = (pOutBitmap->Width() == pInBitmap->Width()) &&
				    (pOutBitmap->Height() == pInBitmap->Height());

		if (bContinue)
		{
			// Init iterators
			TType *			pGray  = pInBitmap->GetGrayPixel(0, 0);
			LPBYTE			pOut;
			LPRGBQUAD &		pOutPixel = (LPRGBQUAD &)pOut;

			fMultiplier = pInBitmap->GetMultiplier()/256.0;

			for (LONG j =  0;j<lHeight;j++)
			{
				pOut = pOutBitmap->GetPixelBase(0, j);
				for (LONG i = 0;i<lWidth;i++)
				{

					pOutPixel->rgbRed   = gammatrans.m_vTransformation[*pGray/fMultiplier];
					pOutPixel->rgbBlue  = pOutPixel->rgbGreen = pOutPixel->rgbRed;
					pOutPixel->rgbReserved = 0;
					pGray++;
					pOut += 4;
				};
			};
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	ApplyGammaTransformation(C32BitsBitmap * pOutBitmap, CMemoryBitmap * pInBitmap, CGammaTransformation & gammatrans)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	C24BitColorBitmap * p24BitColorBitmap = dynamic_cast<C24BitColorBitmap *>(pInBitmap);
	C48BitColorBitmap * p48BitColorBitmap = dynamic_cast<C48BitColorBitmap *>(pInBitmap);
	C96BitColorBitmap * p96BitColorBitmap = dynamic_cast<C96BitColorBitmap *>(pInBitmap);
	C96BitFloatColorBitmap * p96BitFloatColorBitmap = dynamic_cast<C96BitFloatColorBitmap *>(pInBitmap);

	CGrayBitmap *		pGrayBitmap  = dynamic_cast<CGrayBitmap *>(pInBitmap);
	C8BitGrayBitmap *	p8BitGrayBitmap  = dynamic_cast<C8BitGrayBitmap *>(pInBitmap);
	C16BitGrayBitmap *	p16BitGrayBitmap  = dynamic_cast<C16BitGrayBitmap *>(pInBitmap);
	C32BitGrayBitmap *	p32BitGrayBitmap  = dynamic_cast<C32BitGrayBitmap *>(pInBitmap);
	C32BitFloatGrayBitmap * p32BitFloatGrayBitmap  = dynamic_cast<C32BitFloatGrayBitmap *>(pInBitmap);

	if (p24BitColorBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p24BitColorBitmap, gammatrans);
	else if (p48BitColorBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p48BitColorBitmap, gammatrans);
	else if (p96BitColorBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p96BitColorBitmap, gammatrans);
	else if (p96BitFloatColorBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p96BitFloatColorBitmap, gammatrans);
	else if (pGrayBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, pGrayBitmap, gammatrans);
	else if (p8BitGrayBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p8BitGrayBitmap, gammatrans);
	else if (p16BitGrayBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p16BitGrayBitmap, gammatrans);
	else if (p32BitGrayBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p32BitGrayBitmap, gammatrans);
	else if (p32BitFloatGrayBitmap)
		bResult = ApplyGammaTransformation(pOutBitmap, p32BitFloatGrayBitmap, gammatrans);

	return bResult;
};

/* ------------------------------------------------------------------- */

#endif // DSSFILEDECODING

/* ------------------------------------------------------------------- */

static		std::set<CBitmapInfo>			g_sBitmapInfoCache;
static		SYSTEMTIME						g_BitmapInfoTime;

/* ------------------------------------------------------------------- */

BOOL	GetPictureInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

#if DSSFILEDECODING==0
	if (IsPCLPicture(szFileName, BitmapInfo))
		bResult = TRUE;
#else
	// First try to find the info in the cache
	if (g_sBitmapInfoCache.size())
	{
		// Check that the cache is not old (more than 5 minutes)
		SYSTEMTIME			st;
		FILETIME			ft1, ft2;
		ULARGE_INTEGER		ulft1, ulft2;

		GetSystemTime(&st);

		SystemTimeToFileTime(&g_BitmapInfoTime, &ft1);
		SystemTimeToFileTime(&st, &ft2);

		__int64				diff;

		ulft1.LowPart = ft1.dwLowDateTime;
		ulft1.HighPart = ft1.dwHighDateTime;

		ulft2.LowPart = ft2.dwLowDateTime;
		ulft2.HighPart = ft2.dwHighDateTime;

		diff = ulft2.QuadPart-ulft1.QuadPart;

		// diff is in 100 of nanoseconds (1e6 millisecond = 1 nanosecond)
		if (diff > 10000.0*1000.0*60.0*5.0)
			g_sBitmapInfoCache.clear();

		std::set<CBitmapInfo>::iterator		it;

		it = g_sBitmapInfoCache.find(CBitmapInfo(szFileName));
		if (it != g_sBitmapInfoCache.end())
		{
			BitmapInfo = *it;
			bResult = true;
		};
	};

	if (!bResult)
	{
		if (IsRAWPicture(szFileName, BitmapInfo))
			bResult = TRUE;
		else if (IsTIFFPicture(szFileName, BitmapInfo))
			bResult = TRUE;
		else if (IsFITSPicture(szFileName, BitmapInfo))
			bResult = TRUE;
		else if (IsOtherPicture(szFileName, BitmapInfo))
			bResult = TRUE;

		if (bResult)
		{
			TCHAR			szTime[200];
			TCHAR			szDate[200];
			CString			strDateTime;

			if (!BitmapInfo.m_DateTime.wYear)
			{
				// use the file creation time instead of the EXIF info
				FILETIME		FileTime;
				SYSTEMTIME		SystemTime;

				GetFileCreationDateTime(szFileName, FileTime);
				FileTimeToSystemTime(&FileTime, &SystemTime);
				SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &SystemTime);
				BitmapInfo.m_DateTime = SystemTime;

			};

			GetSystemTime(&BitmapInfo.m_InfoTime);

			GetDateFormat(LOCALE_USER_DEFAULT, 0, &BitmapInfo.m_DateTime, NULL, szDate, sizeof(szDate));
			GetTimeFormat(LOCALE_USER_DEFAULT, 0, &BitmapInfo.m_DateTime, NULL, szTime, sizeof(szTime));

			BitmapInfo.m_strDateTime.Format(_T("%s %s"), szDate, szTime);

			if (!g_sBitmapInfoCache.size())
				GetSystemTime(&g_BitmapInfoTime);
			g_sBitmapInfoCache.insert(BitmapInfo);
		};
	};
#endif

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	LoadPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	
	if (ppBitmap)
	{
		CBitmapInfo					BitmapInfo;
		CSmartPtr<CMemoryBitmap>	pBitmap;
		*ppBitmap = NULL;

#if DSSFILEDECODING==0
		if (IsPCLPicture(szFileName, BitmapInfo))
			bResult = LoadPCLPicture(szFileName, &pBitmap, pProgress);
#else
		if (IsRAWPicture(szFileName, BitmapInfo))
			bResult = LoadRAWPicture(szFileName, &pBitmap, pProgress);
		else if (IsTIFFPicture(szFileName, BitmapInfo))
			bResult = LoadTIFFPicture(szFileName, &pBitmap, pProgress);
		else if (IsFITSPicture(szFileName, BitmapInfo))
			bResult = LoadFITSPicture(szFileName, &pBitmap, pProgress);
		else 
			bResult = LoadOtherPicture(szFileName, &pBitmap, pProgress);
#endif

		if (bResult)
			pBitmap.CopyTo(ppBitmap);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CreateBitmap(const CBitmapCharacteristics & bc, CMemoryBitmap ** ppOutBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
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
		bResult = TRUE;
	};

	return bResult;
};


/* ------------------------------------------------------------------- */

class CSubtractTask : public CMultitask
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
	BOOL						m_bMonochrome;
	CDSSProgress *				m_pProgress;
	BOOL						m_bAddMode;
	double						m_fMinimum;

public :
	CSubtractTask()
	{
		m_fXShift = 0;
		m_fYShift = 0;
		m_bAddMode = FALSE;
		m_fMinimum = 0;
	};

	virtual ~CSubtractTask()
	{
	};

	void	Init(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress, double fRedFactor, double fGreenFactor, double fBlueFactor)
	{
		m_pProgress		= pProgress;
		m_pTarget		= pTarget;
		m_pSource		= pSource;
		m_fRedFactor	= fRedFactor;
		m_fGreenFactor	= fGreenFactor;
		m_fBlueFactor	= fBlueFactor;

		m_bMonochrome = pTarget->IsMonochrome();
		m_fGrayFactor = 1.0;

		if (m_bMonochrome)
			m_fGrayFactor = max(fRedFactor, max(fGreenFactor, fBlueFactor));

		if (m_pProgress)
			m_pProgress->Start2(NULL, pTarget->RealWidth());
	};

	void	SetShift(double fXShift, double fYShift)
	{
		m_fXShift	= fXShift;
		m_fYShift	= fYShift;
	};

	void	SetAddMode(BOOL bSet)
	{
		m_bAddMode = bSet;
	};

	void	SetMinimumValue(double fValue)
	{
		m_fMinimum = fValue;
	};

	void	End()
	{
		if (m_pProgress)
			m_pProgress->End2();
	};

	virtual BOOL	Process();
	virtual BOOL	DoTask(HANDLE hEvent);
};

/* ------------------------------------------------------------------- */

BOOL	CSubtractTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	LONG			i, j;
	BOOL			bEnd = FALSE;
	MSG				msg;
	LONG			lWidth = m_pTarget->RealWidth();
	LONG			lExtraWidth = 0;

	PixelIterator	PixelItTgt;
	PixelIterator 	PixelItSrc;

	m_pTarget->GetIterator(&PixelItTgt);
	m_pSource->GetIterator(&PixelItSrc);

	if (m_fXShift)
	{
		lExtraWidth = fabs(m_fXShift)+0.5;
		lWidth -= lExtraWidth;
	};

	// Create a message queue and signal the event
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			LONG			lTgtStartX = 0,
							lTgtStartY = msg.wParam,
							lSrcStartX = 0,
							lSrcStartY = msg.wParam;

			if (m_fXShift>0)
			{
				// Target is moved
				lTgtStartX += m_fXShift+0.5;
			}
			else if (m_fXShift<0)
			{
				// Source is moved
				lSrcStartX += fabs(m_fXShift)+0.5;
			};
			if (m_fYShift>0)
			{
				// Target is moved
				lTgtStartY += m_fYShift+0.5;
			}
			else
			{
				// Source is moved
				lSrcStartY += fabs(m_fYShift)+0.5;
			};
			PixelItTgt->Reset(lTgtStartX, lTgtStartY);
			PixelItSrc->Reset(lSrcStartX, lSrcStartY);
			for (j = 0;j<msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					if (m_bMonochrome)
					{
						double			fSrcGray,
										fTgtGray;

						PixelItTgt->GetPixel(fTgtGray);
						PixelItSrc->GetPixel(fSrcGray);
						if (m_bAddMode)
							fTgtGray = min(max(0, fTgtGray+fSrcGray * m_fGrayFactor), 256.0);
						else
							fTgtGray = max(m_fMinimum, fTgtGray-fSrcGray * m_fGrayFactor);
						PixelItTgt->SetPixel(fTgtGray);
					}
					else
					{
						double			fSrcRed, fSrcGreen, fSrcBlue;
						double			fTgtRed, fTgtGreen, fTgtBlue;

						PixelItTgt->GetPixel(fTgtRed, fTgtGreen, fTgtBlue);
						PixelItSrc->GetPixel(fSrcRed, fSrcGreen, fSrcBlue);
						if (m_bAddMode)
						{
							fTgtRed		= min(max(0, fTgtRed + fSrcRed * m_fRedFactor), 256.0);
							fTgtGreen	= min(max(0, fTgtGreen + fSrcGreen * m_fGreenFactor), 256.0);
							fTgtBlue	= min(max(0, fTgtBlue + fSrcBlue * m_fBlueFactor), 256.0);
						}
						else
						{
							fTgtRed		= max(m_fMinimum, fTgtRed - fSrcRed * m_fRedFactor);
							fTgtGreen	= max(m_fMinimum, fTgtGreen - fSrcGreen * m_fGreenFactor);
							fTgtBlue	= max(m_fMinimum, fTgtBlue - fSrcBlue * m_fBlueFactor);
						};
						PixelItTgt->SetPixel(fTgtRed, fTgtGreen, fTgtBlue);
					};

					(*PixelItTgt)++;
					(*PixelItSrc)++;
				};
				(*PixelItTgt)+=lExtraWidth;
				(*PixelItSrc)+=lExtraWidth;
			};
			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CSubtractTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;
	LONG			lHeight = m_pTarget->RealHeight();
	LONG			lStep;
	LONG			lRemaining;
	LONG			i = 0;

	if (m_fYShift)
		lHeight -= fabs(m_fYShift)+0.5;

	if (m_pProgress)
	{
		m_pProgress->Start2(NULL, lHeight);
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	};

	bResult = TRUE;
	lStep = max(1, lHeight/50);
	lRemaining = lHeight;

	while (i<lHeight)
	{
		DWORD			dwThreadId;
		LONG			lAdd = min(lStep, lRemaining);
		
		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+= lAdd;
		lRemaining	-= lAdd;

		if (m_pProgress)
			m_pProgress->Progress2(NULL, i);
	};

	CloseAllThreads();
	if (m_pProgress)
	{
		m_pProgress->SetNrUsedProcessors();
		m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL Subtract(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress, double fRedFactor, double fGreenFactor, double fBlueFactor)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	// Check that it is the same sizes
	if (pTarget && pSource)
	{
		if ((pTarget->RealWidth() == pSource->RealWidth()) && 
			(pTarget->RealHeight() == pSource->RealHeight()) &&
			(pTarget->IsMonochrome() == pSource->IsMonochrome()))
		{
			CSubtractTask			SubtractTask;

			SubtractTask.Init(pTarget, pSource, pProgress, fRedFactor, fGreenFactor, fBlueFactor);
			SubtractTask.StartThreads();
			SubtractTask.Process();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL ShiftAndSubtract(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress, double fXShift, double fYShift)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
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
			SubtractTask.StartThreads();
			SubtractTask.Process();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL Add(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = FALSE;
	// Check that it is the same sizes
	if (pTarget && pSource)
	{
		if ((pTarget->RealWidth() == pSource->RealWidth()) && 
			(pTarget->RealHeight() == pSource->RealHeight()) &&
			(pTarget->IsMonochrome() == pSource->IsMonochrome()))
		{
			CSubtractTask		AddTask;

			AddTask.SetAddMode(TRUE);
			AddTask.Init(pTarget, pSource, pProgress, 1.0, 1.0, 1.0);
			AddTask.StartThreads();
			AddTask.Process();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CMultiplyTask : public CMultitask
{
private :
	CSmartPtr<CMemoryBitmap>	m_pTarget;
	double						m_fRedFactor;
	double						m_fGreenFactor;
	double						m_fBlueFactor;
	double						m_fGrayFactor;
	BOOL						m_bMonochrome;
	CDSSProgress *				m_pProgress;

public :
	CMultiplyTask()
	{
	};

	virtual ~CMultiplyTask()
	{
	};

	void	Init(CMemoryBitmap * pTarget, CDSSProgress * pProgress, double fRedFactor, double fGreenFactor, double fBlueFactor)
	{
		m_pProgress		= pProgress;
		m_pTarget		= pTarget;
		m_fRedFactor	= fRedFactor;
		m_fGreenFactor	= fGreenFactor;
		m_fBlueFactor	= fBlueFactor;

		m_bMonochrome = pTarget->IsMonochrome();
		m_fGrayFactor = 1.0;

		if (m_bMonochrome)
			m_fGrayFactor = max(fRedFactor, max(fGreenFactor, fBlueFactor));

		if (m_pProgress)
			m_pProgress->Start2(NULL, pTarget->RealWidth());
	};

	void	End()
	{
		if (m_pProgress)
			m_pProgress->End2();
	};

	virtual BOOL	Process();
	virtual BOOL	DoTask(HANDLE hEvent);
};

/* ------------------------------------------------------------------- */

BOOL	CMultiplyTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	LONG			i, j;
	BOOL			bEnd = FALSE;
	MSG				msg;
	LONG			lWidth = m_pTarget->RealWidth();

	PixelIterator	PixelItTgt;

	m_pTarget->GetIterator(&PixelItTgt);

	// Create a message queue and signal the event
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			PixelItTgt->Reset(0, msg.wParam);
			for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					if (m_bMonochrome)
					{
						double			fTgtGray;

						PixelItTgt->GetPixel(fTgtGray);
						fTgtGray = min(256.0, max(0, fTgtGray * m_fGrayFactor));
						PixelItTgt->SetPixel(fTgtGray);
					}
					else
					{
						double			fTgtRed, fTgtGreen, fTgtBlue;

						PixelItTgt->GetPixel(fTgtRed, fTgtGreen, fTgtBlue);
						fTgtRed		= min(256.0, max(0, fTgtRed * m_fRedFactor));
						fTgtGreen	= min(256.0, max(0, fTgtGreen * m_fGreenFactor));
						fTgtBlue	= min(256.0, max(0, fTgtBlue * m_fBlueFactor));
						PixelItTgt->SetPixel(fTgtRed, fTgtGreen, fTgtBlue);
					};

					(*PixelItTgt)++;
				};
			};
			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CMultiplyTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	BOOL			bResult = TRUE;
	LONG			lHeight = m_pTarget->RealHeight();
	LONG			lStep;
	LONG			lRemaining;
	LONG			i = 0;

	if (m_pProgress)
	{
		m_pProgress->Start2(NULL, lHeight);
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	};

	bResult = TRUE;
	lStep = max(1, lHeight/50);
	lRemaining = lHeight;

	while (i<lHeight)
	{
		DWORD			dwThreadId;
		LONG			lAdd = min(lStep, lRemaining);
		
		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+= lAdd;
		lRemaining	-= lAdd;

		if (m_pProgress)
			m_pProgress->Progress2(NULL, i);
	};

	CloseAllThreads();
	if (m_pProgress)
	{
		m_pProgress->SetNrUsedProcessors();
		m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL Multiply(CMemoryBitmap * pTarget, double fRedFactor, double fGreenFactor, double fBlueFactor, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	if (pTarget)
	{
		bResult = TRUE;
		CMultiplyTask			MultiplyTask;

		MultiplyTask.Init(pTarget, pProgress, fRedFactor, fGreenFactor, fBlueFactor);
		MultiplyTask.StartThreads();
		MultiplyTask.Process();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

CFATYPE	GetCFAType(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	CFATYPE					Result = CFATYPE_NONE;

	if (pBitmap)
	{
		if (pBitmap->IsCFA())
		{
			CCFABitmapInfo *			pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pBitmap);

			if (pCFABitmapInfo)
				Result = pCFABitmapInfo->GetCFAType();
		};
	}
	return Result;
};

/* ------------------------------------------------------------------- */

BOOL	GetFilteredImage(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, LONG lFilterSize, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = FALSE;

	*ppOutBitmap = NULL;
	if (pInBitmap)
	{
		CSmartPtr<CMedianFilterEngine>	pMedianFilterEngine;

		pInBitmap->GetMedianFilterEngine(&pMedianFilterEngine);

		bResult = pMedianFilterEngine->GetFilteredImage(ppOutBitmap, lFilterSize, pProgress);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
