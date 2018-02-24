/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	gif.h
Owner:	russf@gipsysoft.com
Purpose:	Gif stuff
----------------------------------------------------------------------*/
#ifndef GIF_H
#define GIF_H


// GIF Image File
#define GIF_MAXCOLORS 256
#define GIF_MAXCOLORMAPSIZE 256
#define GIF_CM_RED 0  // color map vectors
#define GIF_CM_GREEN 1
#define GIF_CM_BLUE 2
#define GIF_MAX_LZW_BITS 12
#define GIF_INTERLACE 0X40  // in image descriptor
#define GIF_LOCALCOLORMAP 0X80
#define GIF_TRANSPARENT 0X01
#define GIF_BitSet(byte, bit) (((byte) & (bit)) == (bit))
#define GIF_LM_to_unit(a, b) (((b) << 8) | (a))  // assemble little-endian


#pragma pack( push, 1 )

struct COLOR_MAP
{
	RGBTRIPLE	arrColour[ GIF_MAXCOLORMAPSIZE ];
};

typedef struct tagGIFSCREEN
{
	WORD Width;
	WORD Height;
	COLOR_MAP ColorMap;
	WORD BitPixel;	
	WORD ColorResolution;
	WORD Background;
	WORD AspectRatio;
} GIFSCREEN;

typedef struct tagGIF89
{
	bool bIsTransparent;
	signed short transparent;
	signed short delayTime;
	signed short inputFlag;
	signed short disposal;
	signed short nLeft;
	signed short nTop;
	signed short nWidth;
	signed short nHeight;
	signed short nLoops;
} GIF89;


typedef struct tagGIF89ImageDescriptor
{
	signed short nLeft;
	signed short nTop;
	signed short nWidth;
	signed short nHeight;
	unsigned char cPacked;
} GIFIMAGEDESCRIPTOR;

#pragma pack( pop )


#endif //GIF_H