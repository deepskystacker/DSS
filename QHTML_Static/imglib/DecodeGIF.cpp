/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodeMNG.cpp
Owner:	russf@gipsysoft.com
Purpose:	Decode an GIF file into a frame array.
					The only exported function is:
					bool DecodeGIF( CDataSourceABC &ds, CFrameData &fd );

----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
#include "ImgLib.h"
#include "ImgLibInt.h"
#include "Config.h"
#include "Gif.h"
#include "FrameData.h"

#ifdef IMGLIB_GIF

#ifdef TRACING
	#define ITRACE TRACE
#else	//	TRACING
	#define ITRACE 
#endif	//	TRACING

typedef LONG RGBAPIXEL;

static short GIF_GetDataBlock( CDataSourceABC &ds, LPBYTE buf, short &bZeroDataBlock );
static bool GIF_ReadImage( const GS::CDIB *pDibPrevious, CDataSourceABC &ds, short len, short height, COLOR_MAP &colormap, short interlace, GS::CDIB *& pDib, short bitPixel, short bZeroDataBlock );
static short GIF_GetCode(CDataSourceABC &ds, short code_size, short flag, short &bZeroDataBlock );
static short GIF_LZWReadByte( CDataSourceABC &ds, short flag, short input_code_size, short bZeroDataBlock );

#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3


inline void SetRGBAPixel
    ( RGBAPIXEL * pPixel,
      BYTE r,
      BYTE g,
      BYTE b,
      BYTE a
    )
{
  ((BYTE *)pPixel)[RGBA_RED] = r;
  ((BYTE *)pPixel)[RGBA_GREEN] = g;
  ((BYTE *)pPixel)[RGBA_BLUE] = b;
  ((BYTE *)pPixel)[RGBA_ALPHA] = a;
}


// Read color map into buffer with number entries
static BOOL GIF_ReadColorMap( CDataSourceABC &ds , short number, COLOR_MAP &colormap)
{
	for (int i = 0; i < number; i++)
	{
		colormap.arrColour[i].rgbtRed = ds.ReadByte();
		colormap.arrColour[i].rgbtGreen = ds.ReadByte();
		colormap.arrColour[i].rgbtBlue = ds.ReadByte();
	}

	return TRUE;
}

static BOOL GIF_DoExtension(CDataSourceABC &ds, BYTE& label, GIF89& Gif89, short &bZeroDataBlock )
{
	BYTE buf[256];
	switch (label)
	{
	case 0X01:  // Plain Text Extension
		break;

	case 0XFF:  // Application Extension
		{
			GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock );
			if( strncmp( (char *)buf, "NETSCAPE2.0", 11 ) == 0 )
			{
				//TRACE( _T("Found extension block\n"));
				GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock );
				Gif89.nLoops = (short)buf[1];

			}
			
		}
		break;

	case 0XFE:  // Comment Extension
		while ( GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock ) != 0)
		{
			//ITRACE( _T("GIF Comment: %s\n"), buf);
		}
		return TRUE;

	case 0XF9:  // Graphic Control Extension
		GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock );
		Gif89.bIsTransparent = false;
		Gif89.disposal = (WORD)((buf[0] >> 2) & 0X07);
		Gif89.inputFlag = (WORD)((buf[0] >> 1) & 0X01);
		Gif89.delayTime = (WORD)(GIF_LM_to_unit(buf[1], buf[2]));
		if ((buf[0] & 0X01) != 0)
		{
			Gif89.bIsTransparent = true;
			Gif89.transparent = buf[3];
		}
		while( GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock ) != 0)
			;
		return TRUE;

	default:
		//TRACE( _T("Unknown (0x%02x)\n"), label );
		break;
	}

	while( GIF_GetDataBlock( ds, &buf[0], bZeroDataBlock ) != 0 )
		;
	return TRUE;
}



#pragma warning( disable: 4702 )
bool DecodeGIF( CDataSourceABC &ds, CFrameData &fd )
{
	GIF89 Gif89 = { false, -1, -1, -1, 0,0,0,0,0 };
	BYTE buf[16], c;
	short useGlobalColorMap;
	short bitPixel;
	short imageCount = 0;
	short i;

	short bZeroDataBlock = FALSE;

	COLOR_MAP localColorMap;
	GIFSCREEN GifScreen = { 0 };

	//ITRACE(_T("Decoding GIF.\n"));

	for (i = 0; i < 6; i++)
		buf[i] = ds.ReadByte();

	if( strncmp((const char*)buf, "GIF87a", 6) != 0 && strncmp((const char*)buf, "GIF89a", 6) != 0 )
	{
		return false;
	}

	for (i = 0; i < 7; i++)
		buf[i] = ds.ReadByte();
	fd.m_size.cx = GifScreen.Width = (WORD)GIF_LM_to_unit(buf[0], buf[1]);
	fd.m_size.cy = GifScreen.Height = (WORD)GIF_LM_to_unit(buf[2], buf[3]);
	GifScreen.BitPixel = (WORD)(2 << (buf[4] & 0X07));
	GifScreen.ColorResolution = (WORD)(((buf[4] & 0X70) >> 3) + 1);
	GifScreen.Background = buf[5];
	GifScreen.AspectRatio = buf[6];

	// Read the color map data
	if (GIF_BitSet(buf[4], GIF_LOCALCOLORMAP))
		GIF_ReadColorMap(ds, GifScreen.BitPixel, GifScreen.ColorMap);

	if (GifScreen.AspectRatio != 0 && GifScreen.AspectRatio != 49)
	{
		double r;
		r = ((double)GifScreen.AspectRatio + 15.0) / 64.0;
		//ITRACE(_T("Warning - non-square pixels.\n"));
	}

	GS::CDIB * pDib = NULL, *pDibPrevious = NULL ;
	for(; ;)
	{
		c = ds.ReadByte();
		if (c == ';')  // GIF terminator
		{
			//ITRACE(_T("Decoding finished.\n"));
			fd.SetLoopCount( Gif89.nLoops );
			return true;
		}

		if (c == '!')  // Extension
		{
			c = ds.ReadByte();
			GIF_DoExtension( ds, c, Gif89, bZeroDataBlock );
			continue;
		}

		if (c != ',')
		{
			//ITRACE(_T("Bogus character 0X%02x, ignoring\n"), (int)c);
			continue;
		}

		imageCount++;

		GIFIMAGEDESCRIPTOR gi;
		ds.ReadBytes( (LPBYTE)&gi, sizeof( gi ) );
		Gif89.nLeft = gi.nLeft;
		Gif89.nTop = gi.nTop;
		Gif89.nWidth = gi.nWidth;
		Gif89.nHeight = gi.nHeight;

		useGlobalColorMap = !GIF_BitSet(gi.cPacked, GIF_LOCALCOLORMAP);
		bitPixel = (short)(1 << ((gi.cPacked & 0X07) + 1));
		COLORREF crTransparent = 0;

		bool bGotImage = false;
		if (!useGlobalColorMap)
		{
			if (GIF_ReadColorMap(ds, bitPixel, localColorMap))
			{
				return false;
			}

			bGotImage = GIF_ReadImage(pDibPrevious, ds, Gif89.nWidth, Gif89.nHeight, localColorMap, (short)GIF_BitSet(gi.cPacked, GIF_INTERLACE), pDib, GifScreen.BitPixel, bZeroDataBlock );
			if( Gif89.bIsTransparent )
			{
				crTransparent = RGB( localColorMap.arrColour[ Gif89.transparent ].rgbtRed, localColorMap.arrColour[ Gif89.transparent ].rgbtGreen, localColorMap.arrColour[ Gif89.transparent ].rgbtBlue );
			}
		}
		else
		{
			bGotImage = GIF_ReadImage(pDibPrevious, ds, Gif89.nWidth, Gif89.nHeight, GifScreen.ColorMap, (short)GIF_BitSet(gi.cPacked, GIF_INTERLACE), pDib, GifScreen.BitPixel, bZeroDataBlock );
			if( Gif89.bIsTransparent )
			{
				crTransparent = RGB( GifScreen.ColorMap.arrColour[ Gif89.transparent ].rgbtRed, GifScreen.ColorMap.arrColour[ Gif89.transparent ].rgbtGreen, GifScreen.ColorMap.arrColour[ Gif89.transparent ].rgbtBlue );
			}
		}


		if( bGotImage )
		{
			//
			//	If the images are different sizes then we need to create the next image.
			if( Gif89.nWidth != GifScreen.Width || Gif89.nHeight != GifScreen.Height )
			{
				if( Gif89.bIsTransparent )
				{
					//ITRACE( _T("Transparent color=%d,%d,%d\n"), GetRValue( crTransparent ), GetGValue( crTransparent ), GetBValue( crTransparent ) );
					pDib->SetColourTransparent( crTransparent );
				}

				if( pDibPrevious )
				{

					GS::CDIB *pdibNew = pDibPrevious->CreateCopy( pDibPrevious->GetWidth(), pDibPrevious->GetHeight() );
					pdibNew->AddImage( Gif89.nLeft, pDibPrevious->GetHeight() - ( Gif89.nTop + pDib->GetHeight() ), pDib );
					delete pDib;
					pDib = pdibNew;
				}

			}
			else
			{
				if( Gif89.bIsTransparent )
				{
					//ITRACE( _T("Transparent color=%d,%d,%d\n"), GetRValue( crTransparent ), GetGValue( crTransparent ), GetBValue( crTransparent ) );
					pDib->SetColourTransparent( crTransparent );
				}
			}

			int nDelay = 0;
			if( Gif89.delayTime != -1 )
			{
				nDelay = Gif89.delayTime * 10;
			}

			CFrame *pFrame = new CFrame( pDib, nDelay );
			fd.m_arrFrames.Add( pFrame );
			pDibPrevious = pDib;
		}
	}
 	return false;
}
#pragma warning( default: 4702 )


static short GIF_GetDataBlock( CDataSourceABC &ds, LPBYTE buf, short &bZeroDataBlock )
{
	BYTE count, i;
	count = ds.ReadByte();
	bZeroDataBlock = (count == 0);
	if (count != 0)
	{
		for (i = 0; i < count; i++)
			buf[i] = ds.ReadByte();
	}
	return count;
}



static bool GIF_ReadImage( const GS::CDIB *pDibPrevious, CDataSourceABC &ds, short len, short height, COLOR_MAP &cmap, short interlace, GS::CDIB *& pDib, short bitPixel, short bZeroDataBlock )
{
	UNREFERENCED_PARAMETER( bitPixel );
	UNREFERENCED_PARAMETER( pDibPrevious );	
	RGBTRIPLE	*arrColour = cmap.arrColour;

	BYTE c;
	short v;
	short xpos = 0, ypos = 0, pass = 0;

	c = ds.ReadByte();
	if( GIF_LZWReadByte( ds, TRUE, c, bZeroDataBlock ) < 0 )
	{
		return false;
	}
	pDib = new GS::CDIB( NULL, len, height );

	const GS::CDIB::CLineArray &arr = pDib->GetLineArray();

	while( (v = GIF_LZWReadByte( ds, FALSE, c, bZeroDataBlock ) ) >= 0 )
	{
		// Look up pixel in color map and store it away
		RGBAPIXEL * pPixel = (RGBAPIXEL *)arr[ ypos ];

		SetRGBAPixel( pPixel + xpos, arrColour[v].rgbtRed, arrColour[v].rgbtGreen, arrColour[v].rgbtBlue, 0xFF );

		xpos++;
		if (xpos == len)
		{
			xpos = 0;
			if (interlace)
			{
				// handle interlaced row order
				switch (pass)
				{
				case 0:
				case 1:
					ypos += 8;
					break;
				case 2:
					ypos += 4;
					break;
				case 3:
					ypos += 2;
					break;
				}
				if (ypos >= height)
				{
					pass++;
					switch (pass)
					{
					case 1:
						ypos = 4;
						break;
					case 2:
						ypos = 2;
						break;
					case 3:
						ypos = 1;
						break;
					default:
						goto fini;
					}
				}
			}
			else  // no interlace, read in order
			{
				ypos++;
			}
		}
		if (ypos >= height)
			break;
	}

fini:
	if (GIF_LZWReadByte(ds, FALSE, c, bZeroDataBlock) >= 0)
	{
		//ITRACE(_T("Too much input data, ignoring extra...\n"));
	}
	return TRUE;
}

static short GIF_GetCode(CDataSourceABC &ds, short code_size, short flag, short &bZeroDataBlock)
{
	static BYTE buf[280];
	static short curbit;  // current bit number in buf[]
	static short lastbit;  // number of bits in buf[]
	static BOOL done;  // hit file EOF
	static short last_byte;  // last valid byte # in buffer
	short i, j, ret;
	BYTE count;

	if (flag)  // prepare to start reading
	{
		curbit = 0;
		lastbit = 0;
		done = FALSE;
		return 0;
	}
	if ((curbit + code_size) >= lastbit)  // need more bits?
	{
		if (done)
		{
			if ((curbit) >= lastbit)
			{
				//ITRACE(_T("Run off the end of my bits\n"));
				return -99;
			}
			return -1;
		}
		buf[0] = buf[last_byte - 2];
		buf[1] = buf[last_byte - 1];
		if ((count = (BYTE)GIF_GetDataBlock(ds, &buf[2], bZeroDataBlock)) == 0 )
			done = TRUE;
		last_byte = (short)(2 + count);  // update count of valid bytes
		curbit = (short)((curbit - lastbit) + 16);  // // account for slide
		lastbit = (short)((2 + count) * 8);
	}
	ret = 0;  // pick out bits one at a time
	for (i = curbit, j = 0; j < code_size; i++, j++)
		ret |= ((buf[i / 8] & (1 << (i % 8))) != 0) << j;
	curbit = (short)(curbit + code_size);  // update bit pointer
	return ret;
}

static short GIF_LZWReadByte(CDataSourceABC &ds, short flag, short input_code_size, short bZeroDataBlock )
{
	static short fresh = FALSE;
	short code, incode;
	static short code_size, set_code_size;
	static short max_code, max_code_size;
	static short firstcode, oldcode;
	static short clear_code, end_code;
	static WORD next[1 << GIF_MAX_LZW_BITS];
	static BYTE vals[1 << GIF_MAX_LZW_BITS];
	static BYTE stack[1 << (GIF_MAX_LZW_BITS + 1)], *sp;
	register short i;

	if (flag)  // initialization call
	{
		set_code_size = input_code_size;
		code_size = (short)(set_code_size + 1);
		clear_code = (short)(1 << set_code_size);
		end_code = (short)(clear_code + 1);
		max_code = (short)(clear_code + 2);
		max_code_size = (short)(2 * clear_code);
		GIF_GetCode(ds, 0, TRUE, bZeroDataBlock);
		fresh = TRUE;  // note next is first decode call
		// single pixel strings
		for (i = 0; i < clear_code; i++)
		{
			next[i] = 0;
			vals[i] = (BYTE)i;
		}  // clear the rest of the dictionary, just in case
		for (; i < (1 << GIF_MAX_LZW_BITS); i++)
			next[i] = vals[i] = 0;
		sp = stack;
		return 0;
	}
	else if (fresh)  // first decode call
	{
		fresh = FALSE;
		do
		{
			firstcode = oldcode = GIF_GetCode(ds, code_size, FALSE, bZeroDataBlock);
		} while (firstcode == clear_code);
		return firstcode;
	}
	if (sp > stack)  // still working through a previously decoded string
		return *--sp;
	while ((code = GIF_GetCode(ds, code_size, FALSE, bZeroDataBlock)) >= 0)
	{
		if (code == clear_code)  // clear out the dictionary
		{
			for (i = 0; i < clear_code; i++)
			{
				next[i] = 0;
				vals[i] = (BYTE)i;
			}
			for (; i < (1 << GIF_MAX_LZW_BITS); i++)
				next[i] = vals[i] = 0;
			code_size = (short)(set_code_size + 1);
			max_code_size = (short)(2 * clear_code);
			max_code = (short)(clear_code + 2);
			sp = stack;
			firstcode = oldcode = GIF_GetCode(ds, code_size, FALSE, bZeroDataBlock);
			return firstcode;
		}
		else if (code == end_code)  // end of image
		{
			short count;
			BYTE buf[260];
			if (bZeroDataBlock)
				return -2;

			while ((count = GIF_GetDataBlock(ds, &buf[0], bZeroDataBlock)) > 0)
				;  // discard input data
			//if (count != 0)
			//	ITRACE(_T("Missing EOD in data stream (common occurence)\n"));
			return -2;
		}
		incode = code;
		if (code >= max_code)
		{
			// defensive special case for preview of about to be inserted code
			*sp++ = (BYTE)firstcode;
			code = oldcode;
		}
		while (code >= clear_code)
		{
			// Run through dict pushing
			*sp++ = vals[code];  // entire string on the stack
			ASSERT(code != next[code]);
			code = next[code];
		}
		firstcode = vals[code];  // first pixel in dict string
		*sp++ = (BYTE)firstcode;
		if ((code = max_code) < (1 << GIF_MAX_LZW_BITS))  // add to dictionary
		{
			next[code] = oldcode;
			vals[code] = (BYTE)firstcode;
			max_code++;
			if ((max_code >= max_code_size) &&  // increase code size
				(max_code_size < (1 << GIF_MAX_LZW_BITS)))
			{
				max_code_size *= 2;
				code_size++;
			}
		}
		oldcode = incode;
		if (sp > stack)
			return *--sp;
	}  // while GIF_GetCode
	return code;
}


#endif	//	IMGLIB_GIF
