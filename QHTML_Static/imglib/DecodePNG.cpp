/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeMNG.cpp
Owner:	russf@gipsysoft.com
Purpose:	Decode an MNG file into a frame array.
					The only exported function is:
					bool DecodePNG( CDataSourceABC &ds, CFrameArray &arrFrames, int &nWidth, int &nHeight )

----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include <libpng\png.h>
#include "ImgLib.h"
#include "ImgLibInt.h"
#include "DibLoader.h"
#include "Config.h"
#include "CompatibleDC.h"

#ifdef TRACING
	#define ITRACE TRACE
#else	//	TRACING
	#define ITRACE 
#endif	//	TRACING

#ifdef IMGLIB_PNG

static void my_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  CDataSourceABC* pSourceInfo=(CDataSourceABC*)png_get_io_ptr(png_ptr);
	pSourceInfo->ReadBytes( data, static_cast<UINT>( length ) );
}


static int g_nError = 0;
static void user_error_fn( png_structp /*png_ptr*/, png_const_charp /*error_msg */)
{
	//TRACE( _T("%s\n"), error_msg);
  g_nError = 1;
}

static void user_warning_fn( png_structp /*png_ptr*/, png_const_charp /*warning_msg */)
{
  //TRACE( _T("%s\n"), warning_msg);
}


bool DecodePNG( CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	GS::CDIB * pDib = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	//try
	{
		png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,(void *) NULL, user_error_fn, user_warning_fn );
		if( !png_ptr )
		{
			//ITRACE(_T("Failed to allocate PNG struct\n"));
			return NULL;
		}

		//
		//	Prevents libpng from attempting to read the signature (fools it into thinking it has already).
		png_set_sig_bytes( png_ptr, 8 );

		info_ptr = png_create_info_struct( png_ptr );
		png_set_read_fn( png_ptr, (void*)&ds, my_read_data );
		png_read_info( png_ptr, info_ptr );

		bool bAlphaChannel = true;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
		size.cx = width;
		size.cy = height;

		if( color_type & PNG_COLOR_MASK_ALPHA )
		{
			bAlphaChannel = true;
		}
		png_set_bgr( png_ptr );
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			png_set_gray_to_rgb(png_ptr);
		}

		png_set_bgr (png_ptr);

    if (bit_depth < 8)
      png_set_packing(png_ptr);

    png_read_update_info( png_ptr, info_ptr );

		bool bTransparent = false;
		int transparent = 0;

		png_bytep trans;
    int real_num_trans = 0, idx_first_trans = -1;
    int min_trans = 256, idx_min_trans = -1;
		if( png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) )
		{
			int num_trans;
			png_get_tRNS( png_ptr, info_ptr, &trans, &num_trans, NULL);
			for( int i = 0;  i < num_trans;  ++i) {
					if (trans[i] < 255) {
							++real_num_trans;
							if (idx_first_trans < 0)
									idx_first_trans = i;
							if (trans[i] < min_trans) {
									min_trans = trans[i];
									idx_min_trans = i;
							}
					}
			}

			if (real_num_trans > 0)
			{
				bTransparent = true;
				if (real_num_trans > 1 || trans[idx_first_trans] != 0)
				{
						transparent = idx_min_trans;
				}
				else
				{
						transparent = idx_first_trans;
				}
			}
		}

    if (color_type == PNG_COLOR_TYPE_PALETTE )
    {
      png_color* ppng_color_tab=NULL;
      int   nbColor=0;

      png_get_PLTE(png_ptr,info_ptr,&ppng_color_tab,&nbColor);

			if( bTransparent )
			{
				transparent = RGB( ppng_color_tab[ transparent ].red, ppng_color_tab[ transparent ].green, ppng_color_tab[ transparent ].blue );
			}
    }



		HDC hdc = GetDC( NULL );
		pDib = new GS::CDIB( hdc, size.cx, size.cy );
		pDib->SetHasAlphaChannel( bAlphaChannel );
		ReleaseDC( NULL, hdc );

		const GS::CDIB::CLineArray &arrLines = pDib->GetLineArray();

    png_read_image(png_ptr, (unsigned char **)arrLines.GetData() );
    png_read_end(png_ptr, info_ptr);


		if( bTransparent )
		{
			pDib->SetColourTransparent( transparent );
		}

		CFrame *pFrame = new CFrame( pDib, 0 );
		arrFrames.Add( pFrame );
	}
//	catch( ... )
//	{
//		if( pDib )
//		{
//			delete pDib;
//			pDib = NULL;
//		}
//	}
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	if( pDib )
	{
		return true;
	}
	return false;
}

#endif	//	IMGLIB_PNG
