/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeBMP.cpp
Owner:	russf@gipsysoft.com
Purpose:	Decode an BMP file into a frame array.
			The only exported function is:
			bool DecodeBMP( CDataSourceABC &ds, CFrameArray &arrFrames, int &nWidth, int &nHeight )
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include <ImgLib/ImgLib.h>
#include "ImgLibInt.h"
#include "Config.h"
#include "RIFFormat.h"
#include <ZLib\ZLib.h>

#ifdef IMGLIB_RIF

bool DecodeRIF( CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	RIFHeader header;
	if( ds.ReadBytes( (BYTE*)&header, sizeof( header ) )
		&& memcmp( header.bSig, RIF_SIGNATURE, sizeof( RIF_SIGNATURE ) - 1 ) == 0
		&& memcmp( header.bVersion, RIF_VERSION, sizeof( RIF_VERSION ) - 1 ) == 0
		)
	{
		size.cx = header.nImageWidth;
		size.cy = header.nImageHeight;

		Container::CArray<unsigned char> arrBytesSource( header.lCompressedLength );

		if( ds.ReadBytes( (BYTE*)arrBytesSource.GetData(), arrBytesSource.GetSize() ) )
		{
			Container::CArray<unsigned char> arrBytesDest( header.lDataBlockSize );
			uLongf lDataLen = arrBytesDest.GetSize();
			uncompress( arrBytesDest.GetData(), &lDataLen, arrBytesSource.GetData(), arrBytesSource.GetSize() );
			CBufferDataSource ds2;
			ds2.Open( arrBytesDest.GetData(), arrBytesDest.GetSize() );

			CDib *pDibPrevious = NULL;
			for( UINT uFrame = 0; uFrame < (UINT)header.nFrameCount; uFrame++ )
			{
				RIFFrameHeader frameheader;
				if( ds2.ReadBytes( (BYTE*)&frameheader, sizeof( frameheader ) ) )
				{
					CDib *pDib = new CDib( size.cx, size.cy, 32 );
					if( frameheader.nPaletteColors )
					{
						
					}
					else
					{
						ds2.ReadBytes( pDib->GetBits(), frameheader.lFrameBytes );
					}
					if( pDibPrevious )
					{
						CDib *pDibNext = pDibPrevious->CreatFromDelta( pDib );
						delete pDib;
						pDib = pDibNext;
						pDibPrevious = pDib;
					}
					else
					{
						pDibPrevious = pDib;
					}

					CFrame *pFrame = new CFrame( pDib, frameheader.nDisplayTimeMilliSeconds );
					arrFrames.Add( pFrame );
				}
			}
		}
		return true;
	}
	return false;
}

#endif	//	IMGLIB_RIF