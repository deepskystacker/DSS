#include "apn.h"

// error codes

#define LoadRaw_NO_ERROR 0
#define LoadRaw_UNSUPPORTED_FORMAT 1
#define LoadRaw_UNSUPPORTED_COMPRESSION 2
#define LoadRaw_NOT_SUPPORTED_FILE 3
#define LoadRaw_CANNOT_USE_WHITE_BALANCE 4
#define LoadRaw_FILE_NOT_FOUND 5
#define LoadRaw_FILE_TYPE_UNK 6
#define LoadRaw_ALLOC_ERROR 7



/******************************************************/
/*             RAW DIGITAL PHOTO DECODING             */
/*----------------------------------------------------*/
/* http://www.cybercom.net/~dcoffin/dcraw/            */
/*----------------------------------------------------*/
/* Compiler: Visual C++ 6.0                           */
/*----------------------------------------------------*/
/*                                                    */ 
/* Modified by C.Buil / C.Cavadore - Sept 2003        */
/*                                                    */ 
/******************************************************/

//  Modification
typedef unsigned char  uchar;           // 8  bit unsigned
typedef unsigned short ushort;          // 16 bit signed



FILE      *ifp;
char      name[64];
short     order;
char      make[64], model[64], model2[64];
int       table,lowbits;
int       raw_height, raw_width;	/* Including black borders */
int       timestamp;
int       tiff_data_offset, tiff_data_compression;
int       kodak_data_compression;
int       nef_curve_offset;
int       height, width, colors, black, rgb_max;
int       is_canon, is_cmy, is_foveon, use_coeff, trim2, xmag, ymag;
unsigned int filters;

double gamma_val=0.8, bright=1.0, red_scale=1.0, blue_scale=1.0;
int four_color_rgb=0, use_camera_wb=0, document_mode=0, quick_interpolate=0;
double camera_red, camera_blue;
double pre_mul[4], coeff[3][4];


// Procedure variable
void (*load_raw)();

// pointer of table array , 4 elements
ushort (*image)[4];

//////////////


struct decode 
  {
  struct decode *branch[2];
  int leaf;
  } first_decode[32], second_decode[512];

/*
   In order to inline this calculation, I make the risky
   assumption that all filter patterns can be described
   by a repeating pattern of eight rows and two columns

   Return values are either 0/1/2/3 = G/M/C/Y or 0/1/2/3 = R/G1/B/G2
 */
#define FC(row,col) \
	(filters >> ((((row) << 1 & 14) + ((col) & 1)) << 1) & 3)
/*
   PowerShot 600 uses 0xe1e4e1e4:

	  0 1 2 3 4 5
	0 G M G M G M
	1 C Y C Y C Y
	2 M G M G M G
	3 C Y C Y C Y

   PowerShot A5 uses 0x1e4e1e4e:

	  0 1 2 3 4 5
	0 C Y C Y C Y
	1 G M G M G M
	2 C Y C Y C Y
	3 M G M G M G

   PowerShot A50 uses 0x1b4e4b1e:

	  0 1 2 3 4 5
	0 C Y C Y C Y
	1 M G M G M G
	2 Y C Y C Y C
	3 G M G M G M
	4 C Y C Y C Y
	5 G M G M G M
	6 Y C Y C Y C
	7 M G M G M G

   PowerShot Pro70 uses 0x1e4b4e1b:

	  0 1 2 3 4 5
	0 Y C Y C Y C
	1 M G M G M G
	2 C Y C Y C Y
	3 G M G M G M
	4 Y C Y C Y C
	5 G M G M G M
	6 C Y C Y C Y
	7 M G M G M G

   PowerShots Pro90 and G1 use 0xb4b4b4b4:

	  0 1 2 3 4 5
	0 G M G M G M
	1 Y C Y C Y C

   These are Bayer grids, used by most RGB cameras:

	0x94949494:	0x61616161:	0x16161616:

	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 R G R G R G	0 G R G R G R	0 B G B G B G
	1 G B G B G B	1 B G B G B G	1 G R G R G R
	2 R G R G R G	2 G R G R G R	2 B G B G B G
	3 G B G B G B	3 B G B G B G	3 G R G R G R

 */

/******************************* FGET2 ********************************/
/* Get a 2-byte integer, making no assumptions about CPU byte order.  */
/* Nor should we assume that the compiler evaluates left-to-right.    */
/**********************************************************************/
ushort fget2 (FILE *f)
{
uchar a,b;

a=fgetc(f);
b=fgetc(f);
if (order == 0x4949)		/* "II" means little-endian */
   return a + (b << 8);
else				/* "MM" means big-endian */
   return (a << 8) + b;
}

/************************ FGET4 ***********************/
/* Same for a 4-byte integer.                         */
/******************************************************/
int fget4 (FILE *f)
{
uchar a,b,c,d;

a=fgetc(f);
b=fgetc(f);
c=fgetc(f);
d=fgetc(f);
if (order == 0x4949)
   return a + (b << 8) + (c << 16) + (d << 24);
else
   return (a << 24) + (b << 16) + (c << 8) + d;
}

/************** PS600_LOAD_RAW ***************/
void ps600_load_raw()
{
uchar  data[1120], *dp;
ushort pixel[896], *pix;
int irow, orow, col;

/*
   Immediately after the 26-byte header come the data rows.  First
   the even rows 0..612, then the odd rows 1..611.  Each row is 896
   pixels, ten bits per pixel, packed into 1120 bytes (8960 bits).
 */
for (irow=orow=0; irow < height; irow++)
   {
   fread (data, 1120, 1, ifp);
   for (dp=data, pix=pixel; dp < data+1120; dp+=10, pix+=8)
      {
      pix[0] = (dp[0] << 2) + (dp[1] >> 6    );
      pix[1] = (dp[2] << 2) + (dp[1] >> 4 & 3);
      pix[2] = (dp[3] << 2) + (dp[1] >> 2 & 3);
      pix[3] = (dp[4] << 2) + (dp[1]      & 3);
      pix[4] = (dp[5] << 2) + (dp[9]      & 3);
      pix[5] = (dp[6] << 2) + (dp[9] >> 2 & 3);
      pix[6] = (dp[7] << 2) + (dp[9] >> 4 & 3);
      pix[7] = (dp[8] << 2) + (dp[9] >> 6    );
      }
/*
   Copy 854 pixels into the image[] array.  The other 42 pixels
   are black.  Left-shift by 4 for extra precision in upcoming
   calculations.
 */
   for (col=0; col < width; col++)
      image[orow*width+col][FC(orow,col)] = pixel[col] << 4;
   for (col=width; col < 896; col++)
      black += pixel[col];

   if ((orow+=2) > height)	/* Once we've read all the even rows, */
      orow = 1;			/* read the odd rows. */
   }
black = (int)(((INT64)black << 4) / ((896 - width) * height));
}

/**************** A5_LOAD_RAW **************/
void a5_load_raw()
{
uchar  data[1240], *dp;
ushort pixel[992], *pix;
int row, col;

/*
   Each data row is 992 ten-bit pixels, packed into 1240 bytes.
 */
for (row=0; row < height; row++)
   {
   fread (data, 1240, 1, ifp);
   for (dp=data, pix=pixel; dp < data+1240; dp+=10, pix+=8)
      {
      pix[0] = (dp[1] << 2) + (dp[0] >> 6);
      pix[1] = (dp[0] << 4) + (dp[3] >> 4);
      pix[2] = (dp[3] << 6) + (dp[2] >> 2);
      pix[3] = (dp[2] << 8) + (dp[5]     );
      pix[4] = (dp[4] << 2) + (dp[7] >> 6);
      pix[5] = (dp[7] << 4) + (dp[6] >> 4);
      pix[6] = (dp[6] << 6) + (dp[9] >> 2);
      pix[7] = (dp[9] << 8) + (dp[8]     );
      }
/*
   Copy 960 pixels into the image[] array.  The other 32 pixels
   are black.  Left-shift by 4 for extra precision in upcoming
   calculations.
 */
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (pixel[col] & 0x3ff) << 4;
   for (col=width; col < 992; col++)
      black += pixel[col] & 0x3ff;
   }
black = (int)(((INT64) black << 4) / ((992 - width) * height));
}

/************** A50_LOAD_RAW ***************/
void a50_load_raw()
{
uchar  data[1650], *dp;
ushort pixel[1320], *pix;
int row, col;

/*
  Each row is 1320 ten-bit pixels, packed into 1650 bytes.
 */
for (row=0; row < height; row++) 
   {
   fread (data, 1650, 1, ifp);
   for (dp=data, pix=pixel; dp < data+1650; dp+=10, pix+=8)
      {
      pix[0] = (dp[1] << 2) + (dp[0] >> 6);
      pix[1] = (dp[0] << 4) + (dp[3] >> 4);
      pix[2] = (dp[3] << 6) + (dp[2] >> 2);
      pix[3] = (dp[2] << 8) + (dp[5]     );
      pix[4] = (dp[4] << 2) + (dp[7] >> 6);
      pix[5] = (dp[7] << 4) + (dp[6] >> 4);
      pix[6] = (dp[6] << 6) + (dp[9] >> 2);
      pix[7] = (dp[9] << 8) + (dp[8]     );
      }
/*
   Copy 1290 pixels into the image[] array.  The other 30 pixels
   are black.  Left-shift by 4 for extra precision in upcoming
   calculations.
 */
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (pixel[col] & 0x3ff) << 4;
   for (col=width; col < 1320; col++)
      black += pixel[col] & 0x3ff;
   }
black = (int)(((INT64)black << 4) / ((1320 - width) * height));
}

/***************** PRO70_LOAD_RAW ****************/
void pro70_load_raw()
{
uchar  data[1940], *dp;
ushort pixel[1552], *pix;
int row, col;

/*
  Each row is 1552 ten-bit pixels, packed into 1940 bytes.
 */
for (row=0; row < height; row++) 
   {
   fread (data, 1940, 1, ifp);
   for (dp=data, pix=pixel; dp < data+1940; dp+=10, pix+=8)
      {
      pix[0] = (dp[1] << 2) + (dp[0] >> 6);	/* Same as PS A5 */
      pix[1] = (dp[0] << 4) + (dp[3] >> 4);
      pix[2] = (dp[3] << 6) + (dp[2] >> 2);
      pix[3] = (dp[2] << 8) + (dp[5]     );
      pix[4] = (dp[4] << 2) + (dp[7] >> 6);
      pix[5] = (dp[7] << 4) + (dp[6] >> 4);
      pix[6] = (dp[6] << 6) + (dp[9] >> 2);
      pix[7] = (dp[9] << 8) + (dp[8]     );
      }
/*
   Copy all pixels into the image[] array.  Left-shift by 4 for
   extra precision in upcoming calculations.  No black pixels?
 */
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (pixel[col] & 0x3ff) << 4;
   }
}

/******************** MAKE_DECODER *******************/
void make_decoder(struct decode *dest,const uchar *source,int level)
{
static struct decode *free;	/* Next unused node */
static int leaf;		/* number of leaves already added */
int i, next;

if (level==0) 
   {
   free = dest;
   leaf = 0;
   }
free++;

//   At what level should the next leaf appear?
for (i=next=0; i <= leaf && next < 16; )
   i += source[next++];

if (level < next) 
   {		/* Are we there yet? */
   dest->branch[0] = free;
   make_decoder(free,source,level+1);
   dest->branch[1] = free;
   make_decoder(free,source,level+1);
   } 
else
   dest->leaf = source[16 + leaf++];
}

/******************* INIT_TABLES ****************/
void init_tables(unsigned table)
{
  static const uchar first_tree[3][29] = {
    { 0,1,4,2,3,1,2,0,0,0,0,0,0,0,0,0,
      0x04,0x03,0x05,0x06,0x02,0x07,0x01,0x08,0x09,0x00,0x0a,0x0b,0xff  },

    { 0,2,2,3,1,1,1,1,2,0,0,0,0,0,0,0,
      0x03,0x02,0x04,0x01,0x05,0x00,0x06,0x07,0x09,0x08,0x0a,0x0b,0xff  },

    { 0,0,6,3,1,1,2,0,0,0,0,0,0,0,0,0,
      0x06,0x05,0x07,0x04,0x08,0x03,0x09,0x02,0x00,0x0a,0x01,0x0b,0xff  },
  };

  static const uchar second_tree[3][180] = {
    { 0,2,2,2,1,4,2,1,2,5,1,1,0,0,0,139,
      0x03,0x04,0x02,0x05,0x01,0x06,0x07,0x08,
      0x12,0x13,0x11,0x14,0x09,0x15,0x22,0x00,0x21,0x16,0x0a,0xf0,
      0x23,0x17,0x24,0x31,0x32,0x18,0x19,0x33,0x25,0x41,0x34,0x42,
      0x35,0x51,0x36,0x37,0x38,0x29,0x79,0x26,0x1a,0x39,0x56,0x57,
      0x28,0x27,0x52,0x55,0x58,0x43,0x76,0x59,0x77,0x54,0x61,0xf9,
      0x71,0x78,0x75,0x96,0x97,0x49,0xb7,0x53,0xd7,0x74,0xb6,0x98,
      0x47,0x48,0x95,0x69,0x99,0x91,0xfa,0xb8,0x68,0xb5,0xb9,0xd6,
      0xf7,0xd8,0x67,0x46,0x45,0x94,0x89,0xf8,0x81,0xd5,0xf6,0xb4,
      0x88,0xb1,0x2a,0x44,0x72,0xd9,0x87,0x66,0xd4,0xf5,0x3a,0xa7,
      0x73,0xa9,0xa8,0x86,0x62,0xc7,0x65,0xc8,0xc9,0xa1,0xf4,0xd1,
      0xe9,0x5a,0x92,0x85,0xa6,0xe7,0x93,0xe8,0xc1,0xc6,0x7a,0x64,
      0xe1,0x4a,0x6a,0xe6,0xb3,0xf1,0xd3,0xa5,0x8a,0xb2,0x9a,0xba,
      0x84,0xa4,0x63,0xe5,0xc5,0xf3,0xd2,0xc4,0x82,0xaa,0xda,0xe4,
      0xf2,0xca,0x83,0xa3,0xa2,0xc3,0xea,0xc2,0xe2,0xe3,0xff,0xff  },

    { 0,2,2,1,4,1,4,1,3,3,1,0,0,0,0,140,
      0x02,0x03,0x01,0x04,0x05,0x12,0x11,0x06,
      0x13,0x07,0x08,0x14,0x22,0x09,0x21,0x00,0x23,0x15,0x31,0x32,
      0x0a,0x16,0xf0,0x24,0x33,0x41,0x42,0x19,0x17,0x25,0x18,0x51,
      0x34,0x43,0x52,0x29,0x35,0x61,0x39,0x71,0x62,0x36,0x53,0x26,
      0x38,0x1a,0x37,0x81,0x27,0x91,0x79,0x55,0x45,0x28,0x72,0x59,
      0xa1,0xb1,0x44,0x69,0x54,0x58,0xd1,0xfa,0x57,0xe1,0xf1,0xb9,
      0x49,0x47,0x63,0x6a,0xf9,0x56,0x46,0xa8,0x2a,0x4a,0x78,0x99,
      0x3a,0x75,0x74,0x86,0x65,0xc1,0x76,0xb6,0x96,0xd6,0x89,0x85,
      0xc9,0xf5,0x95,0xb4,0xc7,0xf7,0x8a,0x97,0xb8,0x73,0xb7,0xd8,
      0xd9,0x87,0xa7,0x7a,0x48,0x82,0x84,0xea,0xf4,0xa6,0xc5,0x5a,
      0x94,0xa4,0xc6,0x92,0xc3,0x68,0xb5,0xc8,0xe4,0xe5,0xe6,0xe9,
      0xa2,0xa3,0xe3,0xc2,0x66,0x67,0x93,0xaa,0xd4,0xd5,0xe7,0xf8,
      0x88,0x9a,0xd7,0x77,0xc4,0x64,0xe2,0x98,0xa5,0xca,0xda,0xe8,
      0xf3,0xf6,0xa9,0xb2,0xb3,0xf2,0xd2,0x83,0xba,0xd3,0xff,0xff  },

    { 0,0,6,2,1,3,3,2,5,1,2,2,8,10,0,117,
      0x04,0x05,0x03,0x06,0x02,0x07,0x01,0x08,
      0x09,0x12,0x13,0x14,0x11,0x15,0x0a,0x16,0x17,0xf0,0x00,0x22,
      0x21,0x18,0x23,0x19,0x24,0x32,0x31,0x25,0x33,0x38,0x37,0x34,
      0x35,0x36,0x39,0x79,0x57,0x58,0x59,0x28,0x56,0x78,0x27,0x41,
      0x29,0x77,0x26,0x42,0x76,0x99,0x1a,0x55,0x98,0x97,0xf9,0x48,
      0x54,0x96,0x89,0x47,0xb7,0x49,0xfa,0x75,0x68,0xb6,0x67,0x69,
      0xb9,0xb8,0xd8,0x52,0xd7,0x88,0xb5,0x74,0x51,0x46,0xd9,0xf8,
      0x3a,0xd6,0x87,0x45,0x7a,0x95,0xd5,0xf6,0x86,0xb4,0xa9,0x94,
      0x53,0x2a,0xa8,0x43,0xf5,0xf7,0xd4,0x66,0xa7,0x5a,0x44,0x8a,
      0xc9,0xe8,0xc8,0xe7,0x9a,0x6a,0x73,0x4a,0x61,0xc7,0xf4,0xc6,
      0x65,0xe9,0x72,0xe6,0x71,0x91,0x93,0xa6,0xda,0x92,0x85,0x62,
      0xf3,0xc5,0xb2,0xa4,0x84,0xba,0x64,0xa5,0xb3,0xd2,0x81,0xe5,
      0xd3,0xaa,0xc4,0xca,0xf2,0xb1,0xe4,0xd1,0x83,0x63,0xea,0xc3,
      0xe2,0x82,0xf1,0xa3,0xc2,0xa1,0xc1,0xe3,0xa2,0xe1,0xff,0xff  }
  };

if (table > 2) table = 2;
memset( first_decode, 0, sizeof first_decode);
memset(second_decode, 0, sizeof second_decode);
make_decoder( first_decode,  first_tree[table], 0);
make_decoder(second_decode, second_tree[table], 0);
}

/************************* GETBITS *************************/
/* getbits(-1) initializes the buffer                      */
/* getbits(n) where 0 <= n <= 25 returns an n-bit integer  */
/***********************************************************/
unsigned long getbits(int nbits)
{
static unsigned long bitbuf=0, ret=0;
static int vbits=0;
unsigned char c;

if (nbits == 0) return 0;
if (nbits == -1)
    ret = bitbuf = vbits = 0;
else 
   {
   ret = bitbuf << (32 - vbits) >> (32 - nbits);
   vbits -= nbits;
   }
while (vbits < 25) 
   {
   c = fgetc(ifp);
   bitbuf = (bitbuf << 8) + c;
   if (c == 0xff && is_canon)	/* Canon puts an extra 0 after 0xff */
      fgetc(ifp);
   vbits += 8;
   }
return ret;
}

/************************* DECOMPRESS *************************/
/* Decompress "count" blocks of 64 samples each.              */
/*                                                            */
/*  Note that the width passed to this function is slightly   */
/*  larger than the global width, because it includes some    */
/* blank pixels that (*load_raw) will strip off.              */ 
/**************************************************************/
void decompress(ushort *outbuf, int count)
{
struct decode *decode, *dindex;
int i, leaf, len, sign, diff, diffbuf[64];
static int carry, pixel, base[2];

if (!outbuf) 
   {			/* Initialize */
   carry = pixel = 0;
   fseek (ifp, count, SEEK_SET);
   getbits(-1);
   return;
   }
while (count--) 
   {
   memset(diffbuf,0,sizeof diffbuf);
   decode = first_decode;
   for (i=0; i < 64; i++ ) 
      {
      for (dindex=decode; dindex->branch[0]; )
	   dindex = dindex->branch[getbits(1)];
      leaf = dindex->leaf;
      decode = second_decode;

      if (leaf == 0 && i) break;
      if (leaf == 0xff) continue;
      i  += leaf >> 4;
      len = leaf & 15;
      if (len == 0) continue;
      sign=(getbits(1));	/* 1 is positive, 0 is negative */
      diff=getbits(len-1);
      if (sign)
	      diff += 1 << (len-1);
      else
	      diff += (-1 << len) + 1;
      if (i < 64) diffbuf[i] = diff;
      }
   diffbuf[0] += carry;
   carry = diffbuf[0];
   for (i=0; i < 64; i++ ) 
      {
      if (pixel++ % raw_width == 0)
	      base[0] = base[1] = 512;
      outbuf[i] = ( base[i & 1] += diffbuf[i] );
      }
   outbuf += 64;
   }
}

/******************* CANON_HAS_LOWBITS ************************/ 
/* Return 0 if the image starts with compressed data,         */
/* 1 if it starts with uncompressed low-order bits.           */
/* In Canon compressed data, 0xff is always followed by 0x00. */
/**************************************************************/
int canon_has_lowbits()
{
uchar test[8192];
int ret=1, i;

fseek (ifp, 0, SEEK_SET);
fread (test, 1, 8192, ifp);
for (i=540; i < 8191; i++)
   {
   if (test[i] == 0xff) 
      {
      if (test[i+1]) return 1;
      ret=0;
      }
   }
return ret;
}

/****************** CANON_COMPRESSED_LOAD_RAW *********************/
void canon_compressed_load_raw()
{
ushort *pixel, *prow;
int lowbits, shift, i, row, r, col, save;
int top=0, left=0, irow, icol;
uchar c;

/* Set the width of the black borders */
switch (raw_width) 
   {
   case 2144:  top = 8;  left =  4;  break;	/* G1 */
   case 2224:  top = 6;  left = 48;  break;	/* EOS D30 */
   case 2376:  top = 6;  left = 12;  break;	/* G2 or G3 */
   case 2672:  top = 6;  left = 12;  break;	/* S50 */
   case 3152:  top =12;  left = 64;  break;	/* EOS D60 */
   }
pixel = (ushort *)calloc (raw_width*8, sizeof *pixel);
lowbits = canon_has_lowbits();
shift = 4 - lowbits*2;
decompress(0, 540 + lowbits*raw_height*raw_width/4);
for (row = 0; row < raw_height; row += 8) 
   {
   decompress(pixel, raw_width/8);		/* Get eight rows */
   if (lowbits) 
      {
      save = ftell(ifp);			/* Don't lose our place */
      fseek (ifp, 26 + row*raw_width/4, SEEK_SET);
      for (prow=pixel, i=0; i < raw_width*2; i++)
         {
	      c = fgetc(ifp);
	      for (r = 0; r < 8; r += 2)
	         *prow++ = (*prow << 2) + ((c >> r) & 3);
         }
      fseek (ifp, save, SEEK_SET);
      }
   for (r=0; r < 8; r++)
      for (col = 0; col < raw_width; col++) 
         {
	      irow = row+r-top;
	      icol = col-left;
	      if (irow >= height) continue;
         
         if (irow<0) continue; // correction bug
         //if (icol<0) continue;
	      //if (icol < width) // ancienne version
	      if (icol < width && icol>=0)
             {
             // Modification: convert to true 12-bits data
	          image[irow*width+icol][FC(irow,icol)]=(pixel[r*raw_width+col] << shift)/4;
	          }
         else
	         black += (pixel[r*raw_width+col])/4;
         }
   }
free(pixel);
black = (int)(((INT64)black << shift) / ((raw_width - width) * height));
}

/***************** NIKON_COMPRESSED_LOAD_RAW *******************/
void nikon_compressed_load_raw()
{
int waste=0;
static const uchar nikon_tree[] = 
   {
    0,1,5,1,1,1,1,1,1,2,0,0,0,0,0,0,
    5,4,3,6,2,7,1,0,8,9,11,10,12
   };
int vpred[4], hpred[2], csize, row, col, i, len, diff;
ushort *curve;
struct decode *dindex;

if (!strcmp(model,"D1X")) waste = 4;

memset( first_decode, 0, sizeof first_decode);
make_decoder( first_decode,  nikon_tree, 0);

fseek (ifp, nef_curve_offset, SEEK_SET);
for (i=0; i < 4; i++) vpred[i] = fget2(ifp);
csize = fget2(ifp);
curve = (ushort *)calloc (csize, sizeof *curve);
for (i=0; i < csize; i++) curve[i] = fget2(ifp);

fseek (ifp, tiff_data_offset, SEEK_SET);
getbits(-1);

for (row=0; row < height; row++)
    for (col=0; col < width+waste; col++)
       {
       for (dindex=first_decode; dindex->branch[0]; )
	    dindex = dindex->branch[getbits(1)];
       len = dindex->leaf;
       diff = getbits(len);
       if ((diff & (1 << (len-1))) == 0) diff -= (1 << len) - 1;
       if (col < 2)
          {
	       i = 2*(row & 1) + col;
	       vpred[i] += diff;
	       hpred[col] = vpred[i];
          } 
       else
	       hpred[col & 1] += diff;
       if (col >= width) continue;
       diff = hpred[col & 1];
       if (diff < 0) diff = 0;
       if (diff >= csize) diff = csize-1;
       // Modification: convert to true 12-bits data
       image[row*width+col][FC(row,col)] = (curve[diff] << 2)/4;
       }
free(curve);
}

/*********************** NIKON_IS_COMPRESSED **************************/
/* Figure out if a NEF file is compressed.  These fancy heuristics    */
/*   are only needed for the D100, thanks to a bug in some cameras    */
/*   that tags all images as "compressed".                            */
/**********************************************************************/
int nikon_is_compressed()
{
uchar test[256];
int i;

if (tiff_data_compression != 34713) return 0;
if (strcmp(model,"D100")) return 1;
fseek (ifp, tiff_data_offset, SEEK_SET);
fread (test, 1, 256, ifp);
for (i=15; i < 256; i+=16)
   if (test[i]) return 1;
return 0;
}

/**************************** NIKON_LOAD_RAW ************************/
void nikon_load_raw()
{
int waste=0, skip16=0;
int irow, row, col, i;

if (!strcmp(model,"D100")) width = 3034;
if (nikon_is_compressed()) 
   {
   nikon_compressed_load_raw();
   return;
   }
if (!strcmp(model,"D1X")) waste = 4;
if (!strcmp(model,"D100") && tiff_data_compression == 34713) 
   {
   waste = 3;
   skip16 = 1;
   width = 3037;
   }

fseek (ifp, tiff_data_offset, SEEK_SET);
getbits(-1);
for (irow=0; irow < height; irow++) 
   {
   row = irow;
   if (model[0] == 'E') 
      {
      row = irow * 2 % height + irow / (height/2);
      if (row == 1 && atoi(model+1) < 5000) 
         {
	      fseek (ifp, 0, SEEK_END);
	      fseek (ifp, ftell(ifp)/2, SEEK_SET);
	      getbits(-1);
         }
      }
   for (col=0; col < width+waste; col++) 
      {
      i = getbits(12);
      if (col < width)
       // Modification: convert to true 12-bits data
	      image[row*width+col][FC(row,col)] = (i << 2)/4;
      if (skip16 && (col % 10) == 9)
	      getbits(8);
      }
   }
}

/*************** NIKON_E950_LOAD_RAW *****************/
void nikon_e950_load_raw()
{
int irow, row, col;

fseek (ifp, 0, SEEK_SET);
getbits(-1);
for (irow=0; irow < height; irow++) 
   {
   row = irow * 2 % height;
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (ushort)getbits(10) << 4;
   for (col=28; col--; )
      getbits(8);
   }
}

/************************* FUJI_LOAD_RAW ***********************/
/* The Fuji Super CCD is just a Bayer grid rotated 45 degrees. */
/***************************************************************/
void fuji_load_raw()
{
unsigned short pixel[2944];
int row, col, r, c;

fseek (ifp, 100, SEEK_SET);
fseek (ifp, fget4(ifp) + (2944*24+32)*2, SEEK_SET);
for (row=0; row < 2144; row++) 
   {
   fread (pixel, 2, 2944, ifp);
   for (col=0; col < 2880; col++) 
      {
      r = row + ((col+1) >> 1);
      c = 2143 - row + (col >> 1);
      // Modification
      swab((char *)&pixel[col],(char *)&pixel[col],2);
      image[row*width+col][FC(row,col)] = pixel[col] << 2;
      }
   }
}

/********************** FUJI SUPER CCD *************************/
/* The Fuji Super CCD is just a Bayer grid rotated 45 degrees. */
/************************************************************* */
void fuji_s2_load_raw()
{
  unsigned short pixel[2944];
  int row, col, r, c;

  fseek (ifp, tiff_data_offset + (2944*24+32)*2, SEEK_SET);
  for (row=0; row < 2144; row++) {
    fread (pixel, 2, 2944, ifp);
    for (col=0; col < 2880; col++) {
      r = row + ((col+1) >> 1);
      c = 2143 - row + (col >> 1);
      // Modification
      swab((char *)&pixel[col],(char *)&pixel[col],2);
      image[r*width+c][FC(r,c)] = pixel[col] << 2;
    }
  }
}

/*************** FUJI S5000 ************/
void fuji_s5000_load_raw()
{
  unsigned short pixel[1472];
  int row, col, r, c;

  fseek (ifp, tiff_data_offset + (1472*4+24)*2, SEEK_SET);
  for (row=0; row < 2152; row++) {
    fread (pixel, 2, 1472, ifp);
      // if (ntohs(0xaa55) == 0xaa55)	/* data is little-endian */
      // Modification
      swab ((char *)pixel, (char *)pixel, 1472*2);
    for (col=0; col < 1424; col++) {
      r = 1423 - col + (row >> 1);
      c = col + ((row+1) >> 1);
      image[r*width+c][FC(r,c)] = pixel[col];
    }
  }
}

/************************* FUJI F700 ****************************/
/* The Fuji Super CCD SR has two photodiodes for each pixel.    */ 
/* The secondary has about 1/16 the sensitivity of the primary, */
/* but this ratio may vary.                                     */
/****************************************************************/
void fuji_f700_load_raw()
{
  unsigned short pixel[2944];
  int row, col, r, c, val;

  fseek (ifp, tiff_data_offset, SEEK_SET);
  for (row=0; row < 2168; row++) {
    fread (pixel, 2, 2944, ifp);
      // if (ntohs(0xaa55) == 0xaa55)	/* data is little-endian */
      // Modification 
      swab ((char *)pixel, (char *)pixel, 2944*2);
    for (col=0; col < 1440; col++) {
      r = 1439 - col + (row >> 1);
      c = col + ((row+1) >> 1);
      val = pixel[col+16];
      if (val == 0x3fff)		/* If the primary is maxed, */
	val = pixel[col+1488] << 4;	/* use the secondary.       */
      if (val > 0xffff)
	val = 0xffff;
      image[r*width+c][FC(r,c)] = val >> 1;
    }
  }
}

/******************** MINOLTA_LOAD_RAW *****************/
void minolta_load_raw()
{
unsigned short *pixel; // Modification
int row, col;

pixel = (unsigned short *)calloc (width, sizeof *pixel);
fseek (ifp, tiff_data_offset, SEEK_SET);
for (row=0; row < height; row++) 
   {
   fread (pixel, 2, width, ifp);
   for (col=0; col < width; col++)
      {
      // Modification
      swab((char *)&pixel[col],(char *)&pixel[col],2);
      image[row*width+col][FC(row,col)] = ((ushort)(pixel[col] << 2))/4;
      }
   }
free(pixel);
}

/********************** OLYMPUS_LOAD_RAW ******************/
void olympus_load_raw()
{
unsigned short *pixel;
int row, col;

pixel = (unsigned short *)calloc (width, sizeof *pixel);
fseek (ifp, tiff_data_offset, SEEK_SET);
for (row=0; row < height; row++) 
   {
   fread (pixel, 2, width, ifp);
   for (col=0; col < width; col++)
      {
      // Modification
      swab((char *)&pixel[col],(char *)&pixel[col],2);
      image[row*width+col][FC(row,col)] = ((ushort)(pixel[col] >> 2))/4;
      }
   }
free(pixel);
}

/********************* OLYMPUS2_LOAD_RAW *****************/
void olympus2_load_raw()
{
int irow, row, col;

for (irow=0; irow < height; irow++) 
   {
   row = irow * 2 % height + irow / (height/2);
   if (row < 2) 
      {
      fseek (ifp, 15360 + row*(width*height*3/4 + 184), SEEK_SET);
      getbits(-1);
      }
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = ((ushort)getbits(12) << 2)/4;
   }
}

/**************** KYOCERA_LOAD_RAW *****************/
void kyocera_load_raw()
{
int row, col;

fseek (ifp, tiff_data_offset, SEEK_SET);
getbits(-1);
for (row=0; row < height; row++)
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (ushort)getbits(12) << 2;
}

/************** CASIO_EASY_LOAD_RAW ****************/
void casio_easy_load_raw()
{
uchar *pixel;
int row, col;

pixel = (uchar *)calloc (raw_width, sizeof *pixel);
fseek (ifp, tiff_data_offset, SEEK_SET);
for (row=0; row < height; row++)
   {
   fread (pixel, 1, raw_width, ifp);
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = pixel[col] << 6;
   }
free (pixel);
}

/**************** CASIO_QV5700_LOAD_RAW ****************/
void casio_qv5700_load_raw()
{
uchar  data[3232],  *dp;
ushort pixel[2576], *pix;
int row, col;

fseek (ifp, 0, SEEK_SET);
for (row=0; row < height; row++) 
   {
   fread (data, 1, 3232, ifp);
   for (dp=data, pix=pixel; dp < data+3220; dp+=5, pix+=4) 
      {
      pix[0] = (dp[0] << 2) + (dp[1] >> 6);
      pix[1] = (dp[1] << 4) + (dp[2] >> 4);
      pix[2] = (dp[2] << 6) + (dp[3] >> 2);
      pix[3] = (dp[3] << 8) + (dp[4]     );
      }
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (pixel[col] & 0x3ff) << 4;
   }
}

/***************** NUCORE_LOAD_RAW *******************/
void nucore_load_raw()
{
uchar *data, *dp;
int irow, row, col;

data = (uchar *)calloc (width, 2);
fseek (ifp, tiff_data_offset, SEEK_SET);
for (irow=0; irow < height; irow++) 
   {
   fread (data, 2, width, ifp);
   if (model[0] == 'B' && width == 2598)
      row = height - 1 - irow/2 - height/2 * (irow & 1);
   else
      row = irow;
   for (dp=data, col=0; col < width; col++, dp+=2)
      image[row*width+col][FC(row,col)] = (dp[0] << 2) + (dp[1] << 10);
  }
free(data);
}

/*************** KODAK_EASY_LOAD_RAW ***************/
void kodak_easy_load_raw()
{
uchar *pixel;
int row, col, margin;

if ((margin = (raw_width - width)/2)) black = 0;
pixel = (uchar *)calloc (raw_width, sizeof *pixel);
fseek (ifp, tiff_data_offset, SEEK_SET);
for (row=0; row < height; row++) 
   {
   fread (pixel, 1, raw_width, ifp);
   for (col=0; col < width; col++)
      image[row*width+col][FC(row,col)] = (ushort) pixel[col+margin] << 6;
   if (margin == 2)
      black += pixel[0] + pixel[1] + pixel[raw_width-2] + pixel[raw_width-1];
   }
if (margin)
   black = (int)(((INT64)black << 6) / (4 * height));
free(pixel);
}

/****************** KODAK_COMPRESSED_LOAD_RAW *****************/
void kodak_compressed_load_raw()
{
uchar c, blen[256];
int row, col, len, i, bits=0, pred[2];
INT64 bitbuf=0;
int diff;

fseek (ifp, tiff_data_offset, SEEK_SET);

for (row=0; row < height; row++)
   for (col=0; col < width; col++)
      {
      if ((col & 255) == 0) 
         {		/* Get the bit-lengths of the */
	      len = width - col;		/* next 256 pixel values      */
	      if (len > 256) len = 256;
	      for (i=0; i < len; ) 
            {
	         c = fgetc(ifp);
	         blen[i++] = c & 15;
	         blen[i++] = c >> 4;
	         }
	      bitbuf = bits = pred[0] = pred[1] = 0;
	      if (len % 8 == 4) 
            {
	         bitbuf  = fgetc(ifp) << 8;
	         bitbuf += fgetc(ifp);
	         bits = 16;
	         }
      }
   len = blen[col & 255];		/* Number of bits for this pixel */
   if (bits < len) 
      {			/* Got enough bits in the buffer? */
	   for (i=0; i < 32; i+=8)
	   bitbuf += (INT64) fgetc(ifp) << (bits+(i^8));
	   bits += 32;
      }
   diff = (int)(bitbuf & (0xffff >> (16-len)));  /* Pull bits from buffer */
   bitbuf >>= len;
   bits -= len;
   if ((diff & (1 << (len-1))) == 0) diff -= (1 << len) - 1;
   pred[col & 1] += diff;
   diff = pred[col & 1];
   image[row*width+col][FC(row,col)] = diff << 2;
   }
}

/************** KODAK_YUV_LOAD_RAW ****************/
void kodak_yuv_load_raw()
{
uchar c, blen[384];
int row, col, len, bits=0;
INT64 bitbuf=0;
int i, li=0, si, diff, six[6], y[4], cb=0, cr=0, rgb[3];
ushort *ip;

fseek (ifp, tiff_data_offset, SEEK_SET);

for (row=0; row < height; row+=2)
   for (col=0; col < width; col+=2) 
      {
      if ((col & 127) == 0) 
         {
	      len = (width - col) * 3;
	      if (len > 384) len = 384;
	      for (i=0; i < len; ) 
            {
	         c = fgetc(ifp);
	         blen[i++] = c & 15;
	         blen[i++] = c >> 4;
	         }
	      li = bits = y[1] = y[3] = cb = cr = 0;
         bitbuf=0;
         }
      for (si=0; si < 6; si++) 
         {
	      len = blen[li++];
	      if (bits < len) 
            {
	         for (i=0; i < 32; i+=8)
	         bitbuf += (INT64) fgetc(ifp) << (bits+(i^8));
	         bits += 32;
	         }
	      diff = (int)(bitbuf & (0xffff >> (16-len)));
	      bitbuf >>= len;
	      bits -= len;
	      if ((diff & (1 << (len-1))) == 0)
	         diff -= (1 << len) - 1;
	      six[si] = diff << 2;
         }
      y[0] = six[0] + y[1];
      y[1] = six[1] + y[0];
      y[2] = six[2] + y[3];
      y[3] = six[3] + y[2];
      cb  += six[4];
      cr  += six[5];
      for (i=0; i < 4; i++) 
         {
	      ip = image[(row+(i >> 1))*width + col+(i & 1)];
	      rgb[0] = y[i] + (int)1.40200/2 * cr;
	      rgb[1] = y[i] - (int)0.34414/2 * cb - (int)0.71414/2 * cr;
	      rgb[2] = y[i] + (int)1.77200/2 * cb;
	      for (c=0; c < 3; c++)
	          if (rgb[c] > 0) ip[c] = rgb[c];
         }
      }
}

/***************************** PARSE_CIFF ***************************/
/* Parse the CIFF structure looking for two pieces of information:  */
/* The camera model, and the decode table number.                   */
/********************************************************************/
void parse_ciff(int offset,int length)
{
int tboff,nrecs,i,type,len,roff,aoff,save;
int wbi=0;

fseek(ifp,offset+length-4,SEEK_SET);
tboff=fget4(ifp)+offset;
fseek(ifp,tboff,SEEK_SET);
nrecs=fget2(ifp);
for (i=0;i<nrecs;i++) 
   {
   type = fget2(ifp);
   len  = fget4(ifp);
   roff = fget4(ifp);
   aoff = offset + roff;
   save = ftell(ifp);
   if (type == 0x080a) 
      {		/* Get the camera make and model */
      fseek (ifp, aoff, SEEK_SET);
      fread (make, 64, 1, ifp);
      fseek (ifp, aoff+strlen(make)+1, SEEK_SET);
      fread (model, 64, 1, ifp);
      }
   if (type==0x102a) 
      {		/* Find the White Balance index */
      fseek (ifp, aoff+14, SEEK_SET);	/* 0=auto, 1=daylight, 2=cloudy ... */
      wbi = fget2(ifp);
      }
   if (type==0x102c) 
      {		/* Get white balance (G2) */
      fseek (ifp, aoff+100, SEEK_SET);	/* could use 100, 108 or 116 */
      camera_red = fget2(ifp);
      camera_red = fget2(ifp) / camera_red;
      camera_blue  = fget2(ifp);
      camera_blue /= fget2(ifp);
      }
   if (type==0x0032 && !strcmp(model,"Canon EOS D30")) 
      {
      fseek (ifp, aoff+72, SEEK_SET);	/* Get white balance (D30) */
      camera_red   = fget2(ifp);
      camera_red   = fget2(ifp) / camera_red;
      camera_blue  = fget2(ifp);
      camera_blue /= fget2(ifp);
      if (wbi==0)			/* AWB doesn't work here */
	      camera_red = camera_blue = 0;
      }
   if (type==0x10a9) 
      {		/* Get white balance (D60) */
      fseek (ifp, aoff+2 + wbi*8, SEEK_SET);
      camera_red  = fget2(ifp);
      camera_red /= fget2(ifp);
      camera_blue = fget2(ifp);
      camera_blue = fget2(ifp) / camera_blue;
      }
   if (type == 0x1031) 
      {		/* Get the raw width and height */
      fseek (ifp, aoff+2, SEEK_SET);
      raw_width  = fget2(ifp);
      raw_height = fget2(ifp);
      }
   if (type == 0x180e) 
      {		/* Get the timestamp */
      fseek (ifp, aoff, SEEK_SET);
      timestamp = fget4(ifp);
      }
   if (type == 0x1835) 
      {		/* Get the decoder table */
      fseek (ifp, aoff, SEEK_SET);
      init_tables (fget4(ifp));
      }
   if (type >> 8 == 0x28 || type >> 8 == 0x30)	/* Get sub-tables */
      parse_ciff(aoff, len);
   fseek (ifp, save, SEEK_SET);
   }
}


/*********** PARSE_FOVEON *************/
void parse_foveon()
{
char *buf,*bp,*np;
int off1,off2,len,i;

order = 0x4949;			/* Little-endian */
fseek (ifp, -4, SEEK_END);
off2 = fget4(ifp);
fseek (ifp, off2, SEEK_SET);
while (fget4(ifp) != 0x464d4143)	/* Search for "CAMF" */
  if (feof(ifp)) return;
off1 = fget4(ifp);
fseek (ifp, off1+8, SEEK_SET);
off1 += (fget4(ifp)+3) * 8;
len = (off2 - off1)/2;
fseek (ifp, off1, SEEK_SET);
buf=(char *)malloc(len);
for (i=0; i < len; i++)		/* Convert Unicode to ASCII */
   buf[i] = (char)fget2(ifp);
for (bp=buf; bp < buf+len; bp=np) 
   {
   np = bp + strlen(bp) + 1;
   if (!strcmp(bp,"CAMMANUF"))
      strcpy (make, np);
   if (!strcmp(bp,"CAMMODEL"))
      strcpy (model, np);
   }
fseek (ifp, 248, SEEK_SET);
raw_width  = fget4(ifp);
raw_height = fget4(ifp);
free(buf);
}

/************** FOVEON_COEF *****************/
void foveon_coeff()
{
static const double foveon[3][3] = 
   {
      {  2.0343955, -0.727533, -0.3067457 },
      { -0.2287194,  1.231793, -0.0028293 },
      { -0.0086152, -0.153336,  1.1617814 }   
   }, mul[3] = { 1.179, 1.0, 0.713 };
int i, j;

for (i=0; i < 3; i++)
   for (j=0; j < 3; j++)
      coeff[i][j] = foveon[i][j] * mul[i];
use_coeff = 1;
}


/**************** NIKON_E950_COEFF ****************/
void nikon_e950_coeff()
{
int r,g;
double my_coeff[3][4] =
  { { -1.936280,  1.800443, -1.448486,  2.584324 },
    {  1.405365, -0.524955, -0.289090,  0.408680 },
    { -1.204965,  1.082304,  2.941367, -1.818705 } };

for (r=0; r < 3; r++) 
   {
   for (g=0; g < 4; g++)
      coeff[r][g] = my_coeff[r][g];
   }
use_coeff = 1;
}


/***************** TIFF_PARSE_SUBIFD ******************/
void tiff_parse_subifd(int base)
{
int entries,tag,type,len,val,save;

entries = fget2(ifp);
while (entries--) 
   {
   tag  = fget2(ifp);
   type = fget2(ifp);
   len  = fget4(ifp);
   if (type == 3) 
      {		/* short int */
      val = fget2(ifp);  fget2(ifp);
      } 
   else
      val = fget4(ifp);
   switch (tag) 
      {
      case 0x100:		/* ImageWidth */
	      raw_width = val;
	      break;
      case 0x101:		/* ImageHeight */
	      raw_height = val;
	      break;
      case 0x102:		/* Bits per sample */
	      break;
      case 0x103:		/* Compression */
	      tiff_data_compression = val;
	      break;
      case 0x106:		/* Kodak color format */
	      kodak_data_compression = val;
	      break;
      case 0x111:		/* StripOffset */
	      if (len == 1)
	      tiff_data_offset = val;
	      else 
            {
	         save = ftell(ifp);
	         fseek (ifp, val+base, SEEK_SET);
	         tiff_data_offset = fget4(ifp);
	         fseek (ifp, save, SEEK_SET);
	         }
	      break;
      case 0x115:		/* SamplesPerRow */
	      break;
      case 0x116:		/* RowsPerStrip */
	      break;
      case 0x117:		/* StripByteCounts */
	      break;
      case 0x828d:		/* Unknown */
      case 0x828e:		/* Unknown */
      case 0x9217:		/* Unknown */
	      break;
      }
   }
}

/***************** NEF_PARSE_MAKERNOTE **************/
void nef_parse_makernote()
{
int base=0, offset=0, entries, tag, type, len, val, save;
short sorder;
char buf[10];

//  The MakerNote might have its own TIFF header (possibly with
//  its own byte-order!), or it might just be a table.
sorder = order;
fread (buf, 1, 10, ifp);
if (!strcmp (buf,"Nikon")) 
   {	/* starts with "Nikon\0\2\0\0\0" ? */
   base = ftell(ifp);
   order = fget2(ifp);		/* might differ from file-wide byteorder */
   val = fget2(ifp);		/* should be 42 decimal */
   offset = fget4(ifp);
   fseek (ifp, offset-8, SEEK_CUR);
   } 
else
   fseek (ifp, -10, SEEK_CUR);

entries = fget2(ifp);
while (entries--) 
   {
   tag  = fget2(ifp);
   type = fget2(ifp);
   len  = fget4(ifp);
   val  = fget4(ifp);
   if (tag == 0xc) 
      {
      save = ftell(ifp);
      fseek (ifp, base + val, SEEK_SET);
      camera_red  = fget4(ifp);
      camera_red /= fget4(ifp);
      camera_blue = fget4(ifp);
      camera_blue/= fget4(ifp);
      fseek (ifp, save, SEEK_SET);
      }
   if (tag == 0x8c)
      nef_curve_offset = base + val + 2112;
   if (tag == 0x96)
      nef_curve_offset = base + val + 2;
   }
order = sorder;
}

/****************** NEF_PARSE_EXIF ******************/
void nef_parse_exif()
{
int entries, tag, type, len, val, save;

entries = fget2(ifp);
while (entries--)
   {
   tag  = fget2(ifp);
   type = fget2(ifp);
   len  = fget4(ifp);
   val  = fget4(ifp);
   save = ftell(ifp);
   if (tag == 0x927c && !strncmp(make,"NIKON",5)) 
      {
      fseek (ifp, val, SEEK_SET);
      nef_parse_makernote();
      fseek (ifp, save, SEEK_SET);
      }
   }
}

/*************************** PARSE_TIFF *******************************/
/* Parse a TIFF file looking for camera model and decompress offsets. */
/**********************************************************************/
void parse_tiff(int base)
{
int doff,entries,tag,type,len,val,save;
char software[64];

tiff_data_offset = 0;
tiff_data_compression = 0;
nef_curve_offset = 0;
fseek (ifp, base, SEEK_SET);
order = fget2(ifp);
val = fget2(ifp);		/* Should be 42 for standard TIFF */
while ((doff = fget4(ifp))) 
   {
   fseek (ifp, doff+base, SEEK_SET);
   entries = fget2(ifp);
   while (entries--) 
      {
      tag  = fget2(ifp);
      type = fget2(ifp);
      len  = fget4(ifp);
      val  = fget4(ifp);
      save = ftell(ifp);
      fseek (ifp, val+base, SEEK_SET);
      switch (tag) 
         {
	      case 271:			/* Make tag */
	         fgets (make, 64, ifp);
	         break;
	      case 272:			/* Model tag */
	         fgets (model, 64, ifp);
	         break;
	      case 33405:			/* Model2 tag */
	         fgets (model2, 64, ifp);
	         break;
	      case 305:			/* Software tag */
	         fgets (software, 64, ifp);
	         if (!strncmp(software,"Adobe",5))
	            model[0] = 0;
	         break;
	      case 330:			/* SubIFD tag */
	         if (len > 2) len=2;
	         if (len > 1)
	            while (len--)
                  {
	               fseek (ifp, val+base, SEEK_SET);
	               fseek (ifp, fget4(ifp)+base, SEEK_SET);
	               tiff_parse_subifd(base);
	               val += 4;
	               }
	         else
	            tiff_parse_subifd(base);
	         break;
	      case 0x8769:			/* Nikon EXIF tag */
	         nef_parse_exif();
	         break;
         }
      fseek (ifp, save, SEEK_SET);
      }
   }
}

/******************************* GMCY_COEFF *****************************/
/*  Given a matrix that converts RGB to GMCY, create a matrix to do     */
/*   the opposite.  Only square matrices can be inverted, so I create   */
/*   four 3x3 matrices by omitting a different GMCY color in each one.  */
/*   The final coeff[][] matrix is the sum of these four.               */
/************************************************************************/
void gmcy_coeff()
{
static const double gmcy[4][3] = {
/*    red  green  blue			   */
    { 0.11, 0.86, 0.08 },	/* green   */
    { 0.50, 0.29, 0.51 },	/* magenta */
    { 0.11, 0.92, 0.75 },	/* cyan    */
    { 0.81, 0.98, 0.08 }	/* yellow  */
  };
double invert[3][6], num;
int ignore, i, j, k, r, g;

memset (coeff, 0, sizeof coeff);
for (ignore=0; ignore < 4; ignore++) 
   {
   for (j=0; j < 3; j++) 
      {
      g = (j < ignore) ? j : j+1;
      for (r=0; r < 3; r++) 
         {
	      invert[j][r] = gmcy[g][r];	/* 3x3 matrix to invert */
	      invert[j][r+3] = (r == j);	/* Identity matrix	*/
         }
      }
   for (j=0; j < 3; j++) 
      {
      num = invert[j][j];		/* Normalize this row	*/
      for (i=0; i < 6; i++)
	      invert[j][i] /= num;
      for (k=0; k < 3; k++) 
         {		/* Subtract it from the other rows */
	      if (k==j) continue;
	      num = invert[k][j];
	      for (i=0; i < 6; i++)
	      invert[k][i] -= invert[j][i] * num;
         }
      }
   for (j=0; j < 3; j++) 
      {		/* Add the result to coeff[][] */
      g = (j < ignore) ? j : j+1;
      for (r=0; r < 3; r++)
	      coeff[r][g] += invert[r][j+3];
      }
   }
for (r=0; r < 3; r++) 
   {		/* Normalize such that:		*/
   for (num=g=0; g < 4; g++)		/* (1,1,1,1) x coeff = (1,1,1) */
      num += coeff[r][g];
   for (g=0; g < 4; g++)
      coeff[r][g] /= num;
   }
use_coeff = 1;
}




/**************************** IDENTIFY ********************************/
/* Identify which camera created this file, and set global variables  */
/* accordingly. Return nonzero if the file cannot be decoded.         */
/**********************************************************************/
int identify(char *fname)
{
char head[26],*c;
int hlen,fsize,g;
unsigned int magic;
// char text[256];

pre_mul[0]=pre_mul[1]=pre_mul[2]=pre_mul[3]=1;
camera_red=camera_blue=black=timestamp=0;
rgb_max=0x4000;
colors=3;
is_cmy=is_foveon=use_coeff=0;
xmag=ymag=1;

strcpy (make,"NIKON");		/* wild guess */
model[0]=model2[0]=0;
tiff_data_offset=0;
order=fget2(ifp);
hlen=fget4(ifp);
fread(head,1,26,ifp);
fseek(ifp,0,SEEK_END);
fsize=ftell(ifp);
fseek(ifp,0,SEEK_SET);
magic=fget4(ifp);

if (order==0x4949 || order==0x4d4d) 
   {
   if (!memcmp(head,"HEAPCCDR",8))
      {
      parse_ciff(hlen,fsize - hlen);
      fseek (ifp,hlen,SEEK_SET);
      }
   else
      parse_tiff(0);
   } 
else if (magic==0x4d524d) 
   {	/* "\0MRM" (Minolta) */
   parse_tiff(48);
   fseek(ifp,4,SEEK_SET);
   tiff_data_offset=fget4(ifp)+8;
   fseek(ifp,24,SEEK_SET);
   raw_height=fget2(ifp);
   raw_width=fget2(ifp);
   } 
else if (magic >> 16 == 0x424d) 
   {  /* "BM" */
   tiff_data_offset=0x1000;
   order=0x4949;
   fseek(ifp,38,SEEK_SET);
   if (fget4(ifp)==2834 && fget4(ifp)==2834)
      {
      strcpy(model,"BMQ");
      goto nucore;
      }
   } 
else if (magic >> 16 == 0x4252)
   {	/* "BR" */
   strcpy (model,"RAW");
   nucore:
   strcpy (make,"Nucore");
   order = 0x4949;
   fseek (ifp,10,SEEK_SET);
   tiff_data_offset+=fget4(ifp);
   fget4(ifp);
   raw_width=fget4(ifp);
   raw_height=fget4(ifp);
   if (model[0]=='B' && raw_width==2597) 
      {
      raw_width++;
      tiff_data_offset -= 0x1000;
      }
   }
else if (!memcmp(head+19,"ARECOYK",7)) 
   {
   strcpy (make,"CONTAX");
   strcpy (model,"N DIGITAL");
   } 
else if (magic == 0x46554a49) /* "FUJI" */
   {	
   fseek (ifp, 84, SEEK_SET);
   parse_tiff (fget4(ifp)+12);
   order = 0x4d4d;
   fseek (ifp, 100, SEEK_SET);
   tiff_data_offset = fget4(ifp);
   }
else if (magic == 0x464f5662)		/* "FOVb" */
   parse_foveon();
else if (fsize == 2465792)		/* Nikon "DIAG RAW" formats */
   strcpy (model,"E950");
else if (fsize == 2940928)
   strcpy (model,"E2500");
else if (fsize == 4771840)
   strcpy (model,"E990/995");
else if (fsize == 5865472)
   strcpy (model,"E4500");
else if (fsize == 5869568)
   strcpy (model,"E4300");
else 
   {
   strcpy (make, "Casio");		/* Casio has similar formats */
   if (fsize == 1976352)
      strcpy (model,"QV-2000UX");
   else if (fsize==3217760)
      strcpy (model,"QV-3*00EX");
   else if (fsize==7684000)
      strcpy (model, "QV-4000");
   else if (fsize == 6218368)
      strcpy (model, "QV-5700");
   }



/* Remove excess wordage */
if (!strncmp(make,"NIKON",5) || !strncmp(make,"Canon",5))
   make[5]=0;
if (!strncmp(make,"OLYMPUS",7) || !strncmp(make,"Minolta",7))
   make[7] = 0;
if (!strncmp(make,"KODAK",5))
   make[16]=model[16]=0;
if (!strncmp(model,"Canon",5) || !strncmp(model,"NIKON",5) ||
    !strncmp(model,"SIGMA",5))
   memmove (model,model+6,64-6);



/* Remove trailing spaces */
c=make+strlen(make);
while (*--c == ' ') *c=0;
c=model+strlen(model);
while (*--c == ' ') *c=0;

   if (model[0]==0) 
   {
   return LoadRaw_NOT_SUPPORTED_FILE;
   }


is_canon=!strcmp(make,"Canon");
if (!strcmp(model,"PowerShot 600")) 
   {
   height=613;
   width= 854;
   colors=4;
   filters=0xe1e4e1e4;
   load_raw=ps600_load_raw;
   pre_mul[0]=1.137;
   pre_mul[1]=1.257;
   }
else if (!strcmp(model,"PowerShot A5")) 
   {
   height=776;
   width= 960;
   colors=4;
   filters=0x1e4e1e4e;
   load_raw=a5_load_raw;
   pre_mul[0] = 1.5842;
   pre_mul[1] = 1.2966;
   pre_mul[2] = 1.0419;
   } 
else if (!strcmp(model,"PowerShot A50")) 
   {
   height=968;
   width= 1290;
   colors=4;
   filters = 0x1b4e4b1e;
   load_raw=a50_load_raw;
   pre_mul[0]=1.750;
   pre_mul[1]=1.381;
   pre_mul[3]=1.182;
   } 
else if (!strcmp(model,"PowerShot Pro70"))
   {
   height = 1024;
   width  = 1552;
   colors = 4;
   filters = 0x1e4b4e1b;
   load_raw = pro70_load_raw;
   pre_mul[0] = 1.389;
   pre_mul[1] = 1.343;
   pre_mul[3] = 1.034;
   } 
else if (!strcmp(model,"PowerShot Pro90 IS")) 
   {
   height = 1416;
   width  = 1896;
   colors = 4;
   filters = 0xb4b4b4b4;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.496;
   pre_mul[1] = 1.509;
   pre_mul[3] = 1.009;
   } 
else if (!strcmp(model,"PowerShot G1")) 
   {
   height = 1550;
   width  = 2088;
   colors = 4;
   filters = 0xb4b4b4b4;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.446;
   pre_mul[1] = 1.405;
   pre_mul[2] = 1.016;
   } 
else if (!strcmp(model,"PowerShot S30"))
   {
   height = 1550;
   width  = 2088;
   filters = 0x94949494;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.785;
   pre_mul[2] = 1.266;
   } 
else if (!strcmp(model,"PowerShot G2")  ||
	      !strcmp(model,"PowerShot G3")  ||
	      !strcmp(model,"PowerShot S40") ||
	      !strcmp(model,"PowerShot S45")) 
   {
   height = 1720;
   width  = 2312;
   filters = 0x94949494;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.965;
   pre_mul[2] = 1.208;
   } 
else if (!strcmp(model,"PowerShot G5")  ||
  	      !strcmp(model,"PowerShot S50")) 
   {
   height = 1960;
   width  = 2616;
   filters = 0x94949494;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.895;
   pre_mul[2] = 1.403;
   } 
else if (!strcmp(model,"EOS D30")) 
   {
   height = 1448;
   width  = 2176;
   filters = 0x94949494;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 1.592;
   pre_mul[2] = 1.261;
   }
else if (!strcmp(model,"EOS D60") ||
	      !strcmp(model,"EOS 10D") ||
	      !strcmp(model,"EOS 300D DIGITAL"))
   {
   height = 2056;
   width  = 3088;
   filters = 0x94949494;
   load_raw = canon_compressed_load_raw;
   pre_mul[0] = 2.242;
   pre_mul[2] = 1.245;
   rgb_max = 4095; // Modification
   } 
else if (!strcmp(model,"EOS-1D")) 
   {
   height = 1662;
   width  = 2496;
   filters = 0x61616161;
//   load_raw = lossless_jpeg_load_raw;
   tiff_data_offset = 288912;
   pre_mul[0] = 1.976;
   pre_mul[2] = 1.282;
   }
else if (!strcmp(model,"EOS-1DS")) 
   {
   height = 2718;
   width  = 4082;
   filters = 0x61616161;
   // load_raw = lossless_jpeg_load_raw;
   tiff_data_offset = 289168;
   pre_mul[0] = 1.66;
   pre_mul[2] = 1.13;
   rgb_max = 14464;
   } 
else if (!strcmp(model,"EOS D2000C"))
   {
   height = raw_height;
   width  = raw_width;
   filters = 0x61616161;
   // load_raw = lossless_jpeg_load_raw;
   black = 800;
   pre_mul[2] = 1.25;
   } 
else if (!strcmp(model,"D1")) 
   {
   height = 1324;
   width  = 2012;
   filters = 0x16161616;
   load_raw = nikon_load_raw;
   pre_mul[0] = 0.838;
   pre_mul[2] = 1.095;
   } 
else if (!strcmp(model,"D1H")) 
   {
   height = 1324;
   width  = 2012;
   filters = 0x16161616;
   load_raw = nikon_load_raw;
   pre_mul[0] = 1.347;
   pre_mul[2] = 3.279;
   } 
else if (!strcmp(model,"D1X")) 
   {
   height = 1324;
   width  = 4024;
   filters = 0x16161616;
   ymag = 2;
   load_raw = nikon_load_raw;
   pre_mul[0] = 1.910;
   pre_mul[2] = 1.220;
   } 
else if (!strcmp(model,"D100")) 
   {
   height = 2024;
   width  = 3037;
   filters = 0x61616161;
   load_raw = nikon_load_raw;
   pre_mul[0] = 2.374;
   pre_mul[2] = 1.677;
   pre_mul[1] = 1;
   rgb_max = 4095;  // Modification
   } 
else if (!strcmp(model,"E950")) 
   {
   height = 1203;
   width  = 1616;
   filters = 0x4b4b4b4b;
   colors = 4;
   load_raw = nikon_e950_load_raw;
   nikon_e950_coeff();
   pre_mul[0] = 1.18193;
   pre_mul[2] = 1.16452;
   pre_mul[3] = 1.17250;
   }
else if (!strcmp(model,"E990/995")) 
   {
   height = 1540;
   width  = 2064;
   filters = 0xb4b4b4b4;
   colors = 4;
   load_raw = nikon_load_raw;
   // nikon_e950_coeff();  // bug ?
   pre_mul[0] = 1.196;
   pre_mul[1] = 1.246;
   pre_mul[2] = 1.018;
   is_cmy=1; // Modification
   } 
else if  (!strcmp(model,"E2500"))
   {
   height = 1204;
   width  = 1616;
   filters = 0x4b4b4b4b;
   goto coolpix;
   } 
else if (!strcmp(model,"E4300")) 
   {
   height = 1710;
   width  = 2288;
   filters = 0x16161616;
   load_raw = nikon_load_raw;
   } 
else if (!strcmp(model,"E4500")) 
   {
   height = 1708;
   width  = 2288;
   filters = 0xb4b4b4b4;
   goto coolpix;
   } 
else if (!strcmp(model,"E5000") || !strcmp(model,"E5700")) 
   {
   height = 1924;
   width  = 2576;
   filters = 0xb4b4b4b4;
   coolpix:
   colors = 4;
   load_raw = nikon_load_raw;
   pre_mul[0] = 1.300;
   pre_mul[1] = 1.300;
   pre_mul[3] = 1.148;
   } 
else if (!strcmp(model,"FinePixS2Pro")) 
   {
   height = 3584;
   width  = 3583;
   filters = 0x61616161;
   load_raw = fuji_s2_load_raw;
   pre_mul[0] = 1.424;
   pre_mul[2] = 1.718;
   } 
else if (!strcmp(model,"FinePix S5000")) 
   {
   height = 2499;
   width  = 2500;
   filters = 0x49494949;
   load_raw = fuji_s5000_load_raw;
   pre_mul[0] = 1.639;
   pre_mul[2] = 1.438;
   } 
else if (!strcmp(model,"FinePix F700")) 
   {
   height = 2523;
   width  = 2524;
   filters = 0x49494949;
   load_raw = fuji_f700_load_raw;
   pre_mul[0] = 1.639;
   pre_mul[2] = 1.438;
   rgb_max = 0x7fff;
   }
else if (!strcmp(make,"Minolta")) 
   {
   height = raw_height;
   width  = raw_width;
   filters = 0x94949494;
   load_raw = minolta_load_raw;
   pre_mul[0] = 1.339;
   pre_mul[2] = 1.703;
   } 
else if (!strcmp(model,"E-10")) 
   {
   height = 1684;
   width  = 2256;
   filters = 0x94949494;
   tiff_data_offset = 0x4000;
   load_raw = olympus_load_raw;
   pre_mul[0] = 1.43;
   pre_mul[2] = 1.77;
   } 
else if (!strncmp(model,"E-20",4)) 
   {
   height = 1924;
   width  = 2576;
   filters = 0x94949494;
   tiff_data_offset = 0x4000;
   load_raw = olympus_load_raw;
   pre_mul[0] = 1.43;
   pre_mul[2] = 1.77;
   } 
else if (!strcmp(model,"C5050Z")) 
   {
   height = 1926;
   width  = 2576;
   filters = 0x16161616;
   load_raw = olympus2_load_raw;
   pre_mul[0] = 1.533;
   pre_mul[2] = 1.880;
   } 
else if (!strcmp(model,"N DIGITAL")) 
   {
   height = 2047;
   width  = 3072;
   filters = 0x61616161;
   tiff_data_offset = 0x1a00;
   load_raw = kyocera_load_raw;
   pre_mul[0] = 1.366;
   pre_mul[2] = 1.251;
   } 
else if (!strcmp(make,"KODAK") || !strcmp(make,"kodak")) 
   {
   height = raw_height;
   width  = raw_width;
   filters = 0x61616161;
   black = 400;
   if (!strcmp(model,"DCS315C")) 
      {
      pre_mul[0] = 0.973;
      pre_mul[2] = 0.987;
      black = 0;
      } 
   else if (!strcmp(model,"DCS330C")) 
      {
      pre_mul[0] = 0.996;
      pre_mul[2] = 1.279;
      black = 0;
      } 
   else if (!strcmp(model,"DCS420")) 
      {
      pre_mul[0] = 1.21;
      pre_mul[2] = 1.63;
      width -= 4;
      } 
   else if (!strcmp(model,"DCS460")) 
      {
      pre_mul[0] = 1.46;
      pre_mul[2] = 1.84;
      width -= 4;
      } 
   else if (!strcmp(model,"DCS460A"))
      {
      colors = 1;
      filters = 0;
      width -= 4;
      } 
   else if (!strcmp(model,"EOSDCS3B")) 
      {
      pre_mul[0] = 1.43;
      pre_mul[2] = 2.16;
      width -= 4;
      } 
   else if (!strcmp(model,"EOSDCS1")) 
      {
      pre_mul[0] = 1.28;
      pre_mul[2] = 2.00;
      width -= 4;
      } 
   else if (!strcmp(model,"DCS520C")) 
      {
      pre_mul[0] = 1.00;
      pre_mul[2] = 1.20;
      } 
   else if (!strcmp(model,"DCS560C")) 
      {
      pre_mul[0] = 0.985;
      pre_mul[2] = 1.15;
      } 
   else if (!strcmp(model,"DCS620C")) 
      {
      pre_mul[0] = 1.00;
      pre_mul[2] = 1.20;
      } 
   else if (!strcmp(model,"DCS620X")) 
      {
      pre_mul[0] = 1.12;
      pre_mul[2] = 1.07;
      is_cmy = 1;
      } 
   else if (!strcmp(model,"DCS660C")) 
      {
      pre_mul[0] = 1.05;
      pre_mul[2] = 1.17;
      } 
   else if (!strcmp(model,"DCS660M")) 
      {
      colors = 1;
      filters = 0;
      } 
   else if (!strcmp(model,"DCS720X")) 
      {
      pre_mul[0] = 1.35;
      pre_mul[2] = 1.18;
      is_cmy = 1;
      } 
   else if (!strcmp(model,"DCS760C")) 
      {
      pre_mul[0] = 1.06;
      pre_mul[2] = 1.72;
      } 
   else if (!strcmp(model,"DCS760M")) 
      {
      colors = 1;
      filters = 0;
      } 
   else if (!strcmp(model,"ProBack")) 
      {
      pre_mul[0] = 1.06;
      pre_mul[2] = 1.385;
      }
   else if (!strncmp(model2,"PB645C",6)) 
      {
      pre_mul[0] = 1.0497;
      pre_mul[2] = 1.3306;
      } 
   else if (!strncmp(model2,"PB645H",6)) 
      {
      pre_mul[0] = 1.2010;
      pre_mul[2] = 1.5061;
      } 
   else if (!strncmp(model2,"PB645M",6)) 
      {
      pre_mul[0] = 1.01755;
      pre_mul[2] = 1.5424;
      } 
   else if (!strcmp(model,"DCS Pro 14n")) 
      {
      pre_mul[1] = 1.0191;
      pre_mul[2] = 1.1567;
      }
   switch (tiff_data_compression) 
      {
      case 0:				/* No compression */

      case 1:
	      rgb_max = 0x3fc0;
	      load_raw = kodak_easy_load_raw;  break;

      case 7:				/* Lossless JPEG */
//	      load_raw = lossless_jpeg_load_raw;  break;

      case 65000:			/* Kodak DCR compression */
	      black = 0;
	      if (kodak_data_compression == 32803)
	         load_raw = kodak_compressed_load_raw;
	      else 
            {
	         load_raw = kodak_yuv_load_raw;
	         filters = 0;
       	   }
	      break;

      default:
	      return LoadRaw_UNSUPPORTED_COMPRESSION;
      }
   }
else if (!strcmp(model,"QV-2000UX")) 
   {
   height = 1208;
   raw_width = width = 1632;
   filters = 0x94949494;
   tiff_data_offset = width * 2;
   load_raw = casio_easy_load_raw;
   } 
else if (!strcmp(model,"QV-3*00EX")) 
   {
   height = 1546;
   width  = 2070;
   raw_width = 2080;
   filters = 0x94949494;
   load_raw = casio_easy_load_raw;
   } 
else if (!strcmp(model,"QV-4000")) 
   {
   height = 1700;
   width  = 2260;
   filters = 0x94949494;
   load_raw = olympus_load_raw;
   } 
else if  (!strcmp(model,"QV-5700")) 
   {
   height = 1924;
   width  = 2576;
   filters = 0x94949494;
   load_raw = casio_qv5700_load_raw;
   } 
else if (!strcmp(make,"Nucore")) 
   {
   height = raw_height;
   width = raw_width;
   filters = 0x61616161;
   load_raw = nucore_load_raw;
   } 
else 
   {
   return LoadRaw_UNSUPPORTED_FORMAT;
   }
if (use_camera_wb) 
   {
   if (camera_red && camera_blue && colors == 3) 
      {
      pre_mul[0] = camera_red;
      pre_mul[2] = camera_blue;
      } 
   else
      {
	  return LoadRaw_CANNOT_USE_WHITE_BALANCE;    
      }
   }
if (colors == 4 && !use_coeff) gmcy_coeff();
if (use_coeff)
   {
   for (g=0; g < colors; g++) 
      {
      coeff[0][g] *= red_scale;
      coeff[2][g] *= blue_scale;
      }
   }
else if (colors == 3) 
   {
   pre_mul[0] *= red_scale;	/* Apply user-selected color balance */
   pre_mul[2] *= blue_scale;
   if (four_color_rgb) 
      {		/* Use two types of green */
      magic = filters;
      for (g=0; g < 32; g+=4) 
         {
         if ((filters >> g & 15) == 9)
 	         filters |= 2 << g;
         if ((filters >> g & 15) == 6)
	         filters |= 8 << g;
         }
      if (filters != magic) colors++;
      }
   }  

// No error, fine !
return LoadRaw_NO_ERROR;

}




/******************************* LOAD_CFA **********************************/
/* READ the RAW format from a digital camera                               */
/* The result is a BAYER image (Color Filter Array)                        */
/* output : is error
/* __stdcall to be used by Delphi and VB
/***************************************************************************/


__declspec(dllexport) 
int __stdcall load_cfa(char *name_in,int *imagepointer,int *sizex,int *sizey) 
{
	
  int error;
  
  // Open file - read only
  ifp = fopen(name_in,"rb");

  if (!ifp) 
   {
   // No handle ! Exit..
   return(LoadRaw_FILE_NOT_FOUND);
   }


  // Is a supported file ?
  // MANDATORY This procedure fills global DLL variables
  // as well as  (*load_raw)(); function

  error=identify(name_in);

  if (error!=LoadRaw_NO_ERROR)
   {
   fclose(ifp);
   return(error);
   }


  *sizex=width;
  *sizey=height;


  // Alloc data
  image=(ushort (*)[4])calloc(height * width, sizeof(*image));

  if (image==NULL) 
  {
  fclose(ifp);
  return(LoadRaw_ALLOC_ERROR);
  }


  // Load image 
  (*load_raw)();

  // Close file
  fclose(ifp);


  // Create a raw buffer, to put the Data
  ushort *p;
  p=(ushort(*))calloc(height*width,sizeof(ushort));

  *imagepointer=(int)p;

  // colors can be equal to 4

  int i,j,k,c;
  

  for (c=0;c<colors;c++)
   {
   k=0;
   for (j=0;j<*sizey;j++)
      { 
      for (i=0;i<*sizex;i++,k++)
         {
         p[k]+=(short)image[k][c];
         }
      }
   }

   // Get rid of image
   free(image);

   return(NO_ERROR);
}