#include <stdafx.h>

#ifdef PCL_PROJECT

#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "PCLTools.h"
#include <pcl/ImageWindow.h>

/* ------------------------------------------------------------------- */

BOOL	IsPCLPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	BOOL			bResult = TRUE;

	BitmapInfo.m_bCanLoad		= TRUE;
	BitmapInfo.m_strFileName	= szFileName;
	BitmapInfo.m_lNrChannels	= 3;

	return bResult;
};

/* ------------------------------------------------------------------- */

template <class PCLImage>
BOOL	LoadPCLImageInMemoryBitmap(CMemoryBitmap * pBitmap, PCLImage * pImage, int NrChannels, double fMultiplier)
{
	BOOL					bResult = TRUE;
	BOOL					bGray = (NrChannels == 1);

	for (LONG i = 0;i<pBitmap->Width();i++)
	{
		for (LONG j = 0;j<pBitmap->Height();j++)
		{
			if (bGray)
			{
				double			fGray;

				fGray = fMultiplier * pImage->Pixel(i, j, 0);
				pBitmap->SetPixel(i, j, fGray);
			}
			else
			{
				double			fRed,
								fGreen,
								fBlue;

				fRed	= fMultiplier * pImage->Pixel(i, j, 0);
				fGreen	= fMultiplier * pImage->Pixel(i, j, 1);
				fBlue	= fMultiplier * pImage->Pixel(i, j, 2);

				pBitmap->SetPixel(i, j, fRed, fGreen, fBlue);
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	LoadPCLPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	BOOL					bResult = FALSE;
	CString					strFileName = szFileName;
	pcl::String				filename;
	filename = szFileName;

	pcl::ImageWindow		PCLWindow( filename);
 
	if ( PCLWindow.IsNull() )
	{
		CString		strError;

		strError.Format("Unable to load image file: %s", szFileName); 
		throw pcl::Error( (LPCTSTR)strError);
	}
	else
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;
		pcl::View 					PCLView = PCLWindow.MainView();
		pcl::ImageVariant 			PCLImage = PCLView.GetImage();

		int							width = PCLImage.AnyImage()->Width();
		int							height = PCLImage.AnyImage()->Height();
		int							numberOfChannels = PCLImage.AnyImage()->IsColor() ? 3 : 1;
		int							nBitPerChannel;
		bool						bFloat; 

		PCLWindow.GetSampleFormat(nBitPerChannel, bFloat);

		// From here created the appropriate MemoryBitmap
		if (numberOfChannels == 1)
		{
			switch (nBitPerChannel)
			{
			case 8 :
				pBitmap.Attach(new C8BitGrayBitmap);
				break;
			case 16 :
				pBitmap.Attach(new C16BitGrayBitmap);
				break;
			case 32 :
			case 64 :
				if (bFloat)
					pBitmap.Attach(new C32BitFloatGrayBitmap);
				else
					pBitmap.Attach(new C32BitGrayBitmap);
				break;
			};
		}
		else
		{
			switch (nBitPerChannel)
			{
			case 8 :
				pBitmap.Attach(new C24BitMemoryBitmap);
				break;
			case 16 :
				pBitmap.Attach(new C48BitMemoryBitmap);
				break;
			case 32 :
			case 64 :
				if (bFloat)
					pBitmap.Attach(new C96BitMemoryBitmap);
				else
					pBitmap.Attach(new C96BitFloatMemoryBitmap);
				break;
			};
		};

		if (pBitmap)
		{
			// Then fill the memory bitmap with the values
			if (pBitmap->Init(width, height))
			{
				switch (nBitPerChannel)
				{
				case 8 :
					bResult = LoadPCLImageInMemoryBitmap(pBitmap, reinterpret_cast<pcl::UInt8Image*>( PCLImage.AnyImage() ), numberOfChannels, 1.0);
					break;
				case 16 :
					bResult = LoadPCLImageInMemoryBitmap(pBitmap, reinterpret_cast<pcl::UInt16Image*>( PCLImage.AnyImage() ), numberOfChannels, 1.0/256.0);
					break;
				case 32 :
					if (bFloat)
						bResult = LoadPCLImageInMemoryBitmap(pBitmap, reinterpret_cast<pcl::Image*>( PCLImage.AnyImage() ), numberOfChannels, 256.0);
					else
						bResult = LoadPCLImageInMemoryBitmap(pBitmap, reinterpret_cast<pcl::UInt32Image*>( PCLImage.AnyImage() ), numberOfChannels, 1/256.0/65536.0);
					break;
				case 64 :
					bResult = LoadPCLImageInMemoryBitmap(pBitmap, reinterpret_cast<pcl::DImage*>( PCLImage.AnyImage() ), numberOfChannels, 256.0);
					break;
				};

				if (bResult)
					pBitmap.CopyTo(ppBitmap);
			};
		};

		PCLWindow.Close();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

template <class P>
void CreatePCLImageWindow( const pcl::Generic2DImage<P> & image, const pcl::String& id )
{
   // Create an image window with a 1x1x1 dummy image.
   // Tricky but efficient: the core application understands this and prepares
   // to build an actual image in subsequent steps.
	pcl::ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
 
   if ( w.IsNull() )
	   throw pcl::Error( "Unable to create image window: " + id );
 
   image.ResetSelections(); // in case we have made partial selections before
   reinterpret_cast<pcl::Generic2DImage<P>*>( w.MainView().GetImage().AnyImage() )->Assign( image );
 
   w.Show();
   w.ZoomToFit( false ); // leave'em laughing :-)
}

/* ------------------------------------------------------------------- */

template <class P>
BOOL CreatePCLImage(CMemoryBitmap * pBitmap, pcl::Generic2DImage<P> & image, double fMultiplier)
{
	BOOL				bResult = FALSE;
	pcl::String			id;
	LONG				i, j;

	try
	{
		bResult = TRUE;
		if (pBitmap->IsMonochrome() && !pBitmap->IsCFA())
		{
			image.AllocateData(pBitmap->Width(), pBitmap->Height(), 1, pcl::ColorSpace::Gray);
			for (i = 0;i<pBitmap->Width();i++)
			{
				for (j = 0;j<pBitmap->Height();j++)
				{
					double			fGray;
					P::sample *		fValue;

					pBitmap->GetPixel(i, j, fGray);
					fValue = image.PixelAddress(i, j, 0);
					*fValue = fGray*fMultiplier;
				};
			};
		}
		else
		{
			image.AllocateData(pBitmap->Width(), pBitmap->Height(), 3, pcl::ColorSpace::RGB);
			for (i = 0;i<pBitmap->Width();i++)
			{
				for (j = 0;j<pBitmap->Height();j++)
				{
					double			fRed, fGreen, fBlue;
					P::sample *		fValue;

					pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
					fValue = image.PixelAddress(i, j, 0);
					*fValue = fRed*fMultiplier;
					fValue = image.PixelAddress(i, j, 1);
					*fValue = fGreen*fMultiplier;
					fValue = image.PixelAddress(i, j, 2);
					*fValue = fBlue*fMultiplier;
				};
			};
		};

		if (bResult)
			CreatePCLImageWindow(image, id);
	}
	catch(...)
	{
		bResult = FALSE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CreatePCLView(CMemoryBitmap * pBitmap)
{
	BOOL			bResult = FALSE;

	if (pBitmap)
	{
		if (pBitmap->IsFloat())
		{
			pcl::Image		PCLImage;

			CreatePCLImage(pBitmap, PCLImage, 1/256.0);
		}
		else
		{
			switch (pBitmap->BitPerSample())
			{
			case 8 :
				{
					pcl::UInt8Image		PCLImage;

					CreatePCLImage(pBitmap, PCLImage, 1.0);
				}
				break;
			case 16 :
				{
					pcl::UInt16Image	PCLImage;

					CreatePCLImage(pBitmap, PCLImage, 256.0);
				}
				break;
			case 32 :
				{
					pcl::UInt32Image	PCLImage;

					CreatePCLImage(pBitmap, PCLImage, 256*0*65536.0);
				}
				break;
			};
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

#endif