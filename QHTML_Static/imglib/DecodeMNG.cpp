/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeMNG.cpp
Owner:	russf@gipsysoft.com
Purpose:	Decode an MNG file into a frame array.
					The only exported function is:
					bool DecodeMNG( CDataSourceABC &ds, CFrameData &fd )

----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include <libpng\png.h>
#include "ImgLib.h"
#include "ImgLibInt.h"
#include "DibLoader.h"
#include "Config.h"
#include "CompatibleDC.h"
#include "FrameData.h"

//#define TRACING

#ifdef TRACING
	#define ITRACE TRACE
	#define ITRACEA TRACEA
#else	//	TRACING
	#define ITRACE
	#define ITRACEA
#endif	//	TRACING

#ifdef IMGLIB_MNG

static inline long GetStreamLong( BYTE *pb )
{
	return ( pb[0] << 24)+(pb[1] << 16)+(pb[2] << 8 ) + pb[3];
}

static inline short GetStreamShort( BYTE *pb )
{
	return static_cast<short>( (pb[0] << 8 ) + pb[1] );
}

static inline BYTE GetStreamByte( BYTE *pb )
{
	return *pb;
}

class CChunkData
{
public:
	CChunkData( UINT uSize ) : m_pb( new BYTE[ uSize ] ) {}
	~CChunkData() { delete[] m_pb; }

	operator BYTE *() { return m_pb; }

private:
	BYTE *m_pb;
};



static void my_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  CDataSourceABC* pSourceInfo=(CDataSourceABC*)png_get_io_ptr(png_ptr);
	pSourceInfo->ReadBytes( data, static_cast<UINT>( length ) );
}



static void user_error_fn( png_structp /*png_ptr*/, png_const_charp /*error_msg*/ )
{
	//TRACE( _T("%s\n"), error_msg);
  throw 1;
}

static void user_warning_fn( png_structp /*png_ptr*/, png_const_charp /*warning_msg*/ )
{
  //TRACE( _T("%s\n"), warning_msg);
}


static GS::CDIB * ReadPNG( CDataSourceABC &ds, SIZE &size )
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

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
	GS::CDIB *pDib = new GS::CDIB( hdc, size.cx, size.cy );
	pDib->SetHasAlphaChannel( bAlphaChannel );
	ReleaseDC( NULL, hdc );

	const GS::CDIB::CLineArray &arrLines = pDib->GetLineArray();

  png_read_image(png_ptr, (unsigned char **)arrLines.GetData() );
  png_read_end(png_ptr, info_ptr);


	if( bTransparent )
	{
		pDib->SetColourTransparent( transparent );
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	return pDib;
}


bool DecodeMNG( CDataSourceABC &ds, CFrameData &fd )
{
  int delay=100;
  int ticks_per_second=1000;
//	bool bTransparent = false;
//	COLORREF crTransparent = 0;

	//
	//	According to the spec. each chunk has the following format:
	//	Length, name, data, CRC
	do
	{
		long lChunkLength;
		if( !ds.MSBReadLong( lChunkLength ) )
		{
			//ITRACE(_T("Failed to read chunk length\n"));
			return false;
		}

		BYTE ChunkType[ 4 ];
		if( !ds.ReadBytes( ChunkType, 4 ) )
		{
			//ITRACE(_T("Failed to read chunk type\n"));
			return false;
		}

    if( memcmp( ChunkType, "MEND", 4 ) == 0)
      break;

		CChunkData pChunkData( lChunkLength );
		if( !pChunkData )
		{
			//ITRACE(_T("Failed to allocate memory for the chunk\n"));
			return false;
		}

		if( lChunkLength && !ds.ReadBytes( pChunkData, lChunkLength ) )
		{
			//ITRACE(_T("Failed to read chunk data\n"));
			return false;
		}

		//
		//	We don't care about the CRC
		long lCRC;
		ds.MSBReadLong( lCRC );

		//
		//	Get here and we have our data chunks.
		if( memcmp( ChunkType, "MHDR", 4 ) == 0)
		{
			fd.m_size.cx = GetStreamLong( pChunkData );
			fd.m_size.cy = GetStreamLong( pChunkData + 4);
			ticks_per_second = GetStreamLong( pChunkData + 8 );
			//long layer_count = GetStreamLong( pChunkData + 12 );
			//long frame_count = GetStreamLong( pChunkData + 16 );
			//long play_time = GetStreamLong( pChunkData + 20 );
			long profile = GetStreamLong( pChunkData + 24 );
			if( profile >> 7 & 0x01 )
			{
				//TRACE( _T("Transparency\n") );
			}
			continue;

		}
		else if( memcmp( ChunkType, "FRAM", 4 ) == 0)
		{
			if( lChunkLength > 6 )
			{
				BYTE *p = pChunkData;
        p++; /* framing mode */
        while (*p && ((p-pChunkData) < lChunkLength) )
				{
          p++;
				}
        if ((p-pChunkData) < (lChunkLength-4))
        {
          p+=5;
          if (*(p-4))
					{
            delay = GetStreamLong( p ) * 1000 / ticks_per_second;
					}
        }
			}
			continue;
		}
		else if( memcmp( ChunkType, "TERM", 4 ) == 0)
		{
			//int nAction = GetStreamByte( pChunkData );
			//int nActionAfter = GetStreamByte( pChunkData + 1 );
			//int nDelay = GetStreamLong( pChunkData + 2 );
			const int nLoops = GetStreamLong( pChunkData + 6);
			fd.SetLoopCount( nLoops );
		}
		
		else if( memcmp( ChunkType, "BACK", 4 ) == 0)
		{
			//
			//	The spec. I used said that teh background colors are red, green, blue but the file written
			//	by PSP is the other way around as per below.
//			int nBlue = GetStreamShort( pChunkData );
//			int nGreen = GetStreamShort( pChunkData + 2);
//			int nRed = GetStreamShort( pChunkData + 4);
//			ITRACE(_T("Background color is RGB( %d, %d, %d )\n"), nRed, nGreen, nBlue );
//			crTransparent = RGB( nRed, nGreen, nBlue );
//			bTransparent = true;
		}
		else if( memcmp( ChunkType, "IHDR", 4 ) == 0)
		{
			//
			//	Read an actual PNG image from the stream. 

			//
			//	Move back so that the PNG library can read it's header.
			ds.SetRelativePos( -((int) lChunkLength + 12) );

			GS::CDIB *pDib = ReadPNG( ds, fd.m_size );
			if( pDib )
			{
				if( !delay )	delay = 1000;
				CFrame *pFrame = new CFrame( pDib, delay );
				fd.m_arrFrames.Add( pFrame );
			}
			else
				return false;
		}
		else
		{
			#ifdef _DEBUG
				TCHAR szChunkBuf[ 5 ];
				memcpy( szChunkBuf, ChunkType, 4 );
				szChunkBuf[ 4 ] = '\000';
				ITRACEA( "Unknown and unsupported MNG chunk \"%s\"\n", szChunkBuf );
			#endif	//	_DEBUG
		}

	} while( 1 );
	return true;
}

#endif	//	IMGLIB_MNG
