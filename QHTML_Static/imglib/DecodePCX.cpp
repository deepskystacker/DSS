/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DecodePCX.cpp
Owner:	rich@woodbridgeinternalmed.com
Purpose:	Decode an PCX file into a frame array.
			The only exported function is:
			bool DecodePCX( CDataSourceABC &ds, CFrameArray &arrFrames, int &nWidth, int &nHeight )
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <datasource/DataSourceABC.h>
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

#ifdef IMGLIB_PCX

// PCX Data structure
#pragma pack(push, 1)

struct PCXHEAD
{
	char manufacturer;
	char version;
	char encoding;
	char bits;
	short xmin, ymin;
	short xmax, ymax;
	short hres;
	short vres;
	char palette[48];
	char reserved;
	char color_planes;
	short bytes_per_line;
	short palette_type;
	char filler[58];
};

#pragma pack(pop)

// Here is a local class that handles some of the details, such as buffering
// the input.

class PCXFileReader
{
public:
	PCXFileReader();

	bool Decode(CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size );

protected:
	bool ReadLine(BYTE *p, int bytes);
	int NextChar();

	CDataSourceABC* m_ds;

	WORD m_width, m_depth, m_bits;
	BYTE m_palette[768];

	BYTE* m_readbuffer;
	BYTE* m_nextbyte;
	DWORD m_bytes_in_buffer;
	DWORD m_sourceSize;

};

#define RGB_RED		0
#define RGB_GREEN		1
#define RGB_BLUE     2
#define WRGB_RED     2
#define WRGB_GREEN   1
#define WRGB_BLUE     0
#define RGB_SIZE 		3



static const int masktable[8] =
   {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static const int bittable[8] =
   {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};


#define READ_BUFFER_SIZE  16384

PCXFileReader::PCXFileReader()
: m_readbuffer(0)
{
}

int PCXFileReader::NextChar()
{
	if (!m_bytes_in_buffer)
	{
		// Get bytes to read...
		m_bytes_in_buffer = min(READ_BUFFER_SIZE, m_sourceSize - m_ds->GetCurrentPos());
		// load more data...
		if (!m_ds->ReadBytes(m_readbuffer, m_bytes_in_buffer))
		{
			m_bytes_in_buffer = 0;
		}
		m_nextbyte = m_readbuffer;
   }
   if (!m_bytes_in_buffer)
      return -1;
   --m_bytes_in_buffer;
   return *m_nextbyte++;
}


bool PCXFileReader::ReadLine(BYTE* p, int bytes)
{
	short n = 0, i;
	int c;

	do
	{
		c = NextChar();
		if (c == -1) { return false; }
		if ((c & 0xc0) == 0xc0)
		{
			i = WORD(c & 0x3f);
			c = NextChar();
			if (c == -1) { return false; }
			while (i--) p[n++] = (BYTE)c;
		} else p[n++] = (BYTE)c;
	} while (n < bytes);
	if (n != bytes)
      return false;
   return true;
}

inline
DWORD LineBytes(DWORD width, WORD bpp)
{
  return (DWORD)((width*(DWORD)bpp+31)&(~31))/8L;
}

bool PCXFileReader::Decode(CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	PCXHEAD pcx;
	BYTE *ps, *pd, *linebuffer = 0, *extrabuffer = 0;
	int a, i, j, k, n, x, bytes;
	bool readLargePal = false;
	GS::CDIB * pDib = NULL;

	static const BYTE pcxpalette[48] = { 0x00, 0x00, 0x0e, 0x00, 0x52, 0x07, 0x2c, 0x00,
									  0x0e, 0x00, 0x00, 0x00, 0xf8, 0x01, 0x2c, 0x00,
									  0x85, 0x0f, 0x42, 0x00, 0x21, 0x00, 0x00, 0x00,
									  0x00, 0x00, 0x6a, 0x24, 0x9b, 0x49, 0xa1, 0x5e,
									  0x90, 0x5e, 0x18, 0x5e, 0x84, 0x14, 0xd9, 0x95,
									  0xa0, 0x14, 0x12, 0x00, 0x06, 0x00, 0x68, 0x1f
	};

	m_ds = &ds;
	m_sourceSize = m_ds->GetSize();
	
	if (!m_ds->ReadBytes((BYTE*)&pcx, sizeof(PCXHEAD)))
		return false;

	// Make sure it's a pcx file
	if (pcx.manufacturer != 0x0a)
	    return false;
	m_width = (WORD)(pcx.xmax - pcx.xmin + 1);
	m_depth = (WORD)(pcx.ymax - pcx.ymin + 1);

	if (pcx.bits == 8 && pcx.color_planes == 3) 
		m_bits = 24;
	else if (pcx.bits == 1) 
		m_bits = pcx.color_planes;
	else 
		m_bits = pcx.bits;

	if (m_bits == 1)
		memcpy(m_palette, "\000\000\000\377\377\377", 6);
	else
	{
		if (m_bits == 8 && pcx.version >= 5)
		{
			readLargePal = true;
		} 
		else if (pcx.version == 3) 
			memcpy(m_palette, pcxpalette, 48);
		else 
			memcpy(m_palette, pcx.palette, 48);
	}

	if (m_bits > 1 && m_bits <= 4)
	{
		if (pcx.bits == 4 && pcx.color_planes == 1) 
			bytes = pcx.bytes_per_line;
		else 
			bytes = (short)(pcx.bytes_per_line * m_bits);
	} 
	else if (m_bits == 24) 
		bytes = (short)(pcx.bytes_per_line * RGB_SIZE);
	else
		bytes = pcx.bytes_per_line;

	size.cx = m_width;
	size.cy = m_depth;

	CDibLoader *pDibLoader = new CDibLoader( size.cx, size.cy, m_bits );

	// Allocate work data
	long linewidth = LineBytes(m_width, m_bits);
	long l = m_width > linewidth ? m_width : linewidth;
	// Need to be able to read at least 'bytes' into memory 
	long l1 = l > (long)bytes ? l : bytes;
	linebuffer = new BYTE[l1];
	extrabuffer= new BYTE[l1];
	n = linewidth;

	// Setup a read buffer
	m_bytes_in_buffer = 0;
	m_readbuffer = new BYTE[READ_BUFFER_SIZE];   // big enough?

	// Get line array from GS::CDIB
	Container::CArray< BYTE * > arrLines;

	pDibLoader->GetLineArray( arrLines );

	for (i = 0; i < m_depth; ++i)
	{
		if (!ReadLine(linebuffer, bytes))
			throw 1;
		if (m_bits > 1 && m_bits <= 4)
		{
			if (pcx.bits != 4 || pcx.color_planes != 1)
			{
				memset(extrabuffer, 0, linewidth);

				pd = extrabuffer;
				ps = linebuffer;
				for (j = 0; j < m_bits; ++j)
				{
					memcpy(pd, ps, pcx.bytes_per_line);
					ps += pcx.bytes_per_line;
					pd += n;
				}
				memset(linebuffer, 0, linewidth);
				for (j = x = 0; j < m_width;)
				{
					a = 0;
					ps = extrabuffer;
					for (k = 0; k < m_bits; ++k)
					{
						if (ps[j >> 3] & masktable[j & 0x0007])
						a |= bittable[k];
						ps += n;
					}
					linebuffer[x] = (BYTE)((a & 0x0f) << 4);

					++j;

					if (j < m_width)
					{
						a = 0;
						ps = extrabuffer;
						for (k = 0; k < m_bits; ++k)
						{
							if (ps[j >> 3] & masktable[j & 0x0007])
							a |= bittable[k];
							ps += n;
						}
						linebuffer[x] |= (BYTE)(a & 0x0f);
					}
					++j;
					++x;
				}
			}
		}
		else if (m_bits == 24)
		{
			memcpy(extrabuffer, linebuffer, linewidth);
			pd = linebuffer;
			ps = extrabuffer;
			for (j = 0; j < m_width; ++j)
			{
				pd[j*RGB_SIZE + WRGB_RED] = ps[j];
				pd[j*RGB_SIZE + WRGB_GREEN] = ps[RGB_GREEN * pcx.bytes_per_line + j];
				pd[j*RGB_SIZE + WRGB_BLUE] = ps[RGB_BLUE * pcx.bytes_per_line + j];
			}
		}
		memcpy(arrLines[i], linebuffer, linewidth);
	}

	// Now, create the palette
	RGBQUAD *pct = pDibLoader->GetColorTable();

	if (pct)
	{
		// Now, a 256 color palette may need to be read...
		if (readLargePal)
		{
			if (NextChar() == 12)
			{
				for (int y = 0; y < 768; ++y)
				m_palette[y] = (BYTE)NextChar();
			}
			else 
				memcpy(m_palette, pcx.palette, 48);
		}
		   // and set the palette
		int num_colors = 1 << m_bits;
		for (short i = 0; i < num_colors; i++)
		{
				pct->rgbRed   = m_palette[(i*3)];
				pct->rgbGreen = m_palette[(i*3)+1];
				pct->rgbBlue  = m_palette[(i*3)+2];
				pct->rgbReserved = 0xFF;
				pct++;
		}
	}

	if( pDibLoader )
	{
		HDC hdc = GetDC( NULL );

		pDib = new GS::CDIB( hdc, size.cx, size.cy );
		{
			CCompatibleDC dc( hdc );
			HGDIOBJ hOld = SelectObject( dc, pDib->GetBitmap() );
			pDibLoader->Draw( dc, 0, 0 );
			SelectObject( dc, hOld );
		}
		ReleaseDC( NULL, hdc );
		delete pDibLoader;

		CFrame *pFrame = new CFrame( pDib, 0 );
		arrFrames.Add( pFrame );
	}
	delete [] m_readbuffer;
	delete [] extrabuffer;
	delete [] linebuffer;

	if( pDib )
		return true;
	return false;
}

bool DecodePCX(CDataSourceABC &ds, CFrameArray &arrFrames, SIZE &size )
{
	PCXFileReader fr;
	return fr.Decode(ds, arrFrames, size);
}

#endif	//	IMGLIB_PCX
