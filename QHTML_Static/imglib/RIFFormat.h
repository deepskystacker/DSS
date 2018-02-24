/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	RIFFormat.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef RIFFORMAT_H
#define RIFFORMAT_H

#define RIF_SIGNATURE	"\\RIF\\"
#define RIF_VERSION	"001"

#pragma pack( push, 1 )
struct RIFHeader
{
	BYTE bSig[5];
	BYTE bVersion[3];
	short nImageWidth;
	short nImageHeight;
	short nFrameCount;
	long lDataBlockSize;
	long lCompressedLength;
};

struct RIFFrameHeader
{
	short nDisplayTimeMilliSeconds;
	unsigned int bHasPalette:  1;
	short nPaletteColors;
	unsigned long lPaletteBytes;
	unsigned long lFrameBytes;
};

#pragma pack( pop )

#endif //RIFFORMAT_H