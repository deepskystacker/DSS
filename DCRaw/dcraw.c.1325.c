/*
   dcraw.c -- Dave Coffin's raw photo decoder
   Copyright 1997-2006 by Dave Coffin, dcoffin a cybercom o net

   This is a command-line ANSI C program to convert raw photos from
   any digital camera on any computer running any operating system.

   Attention!  Some parts of this program are restricted under the
   terms of the GNU General Public License.  Such code is enclosed
   in "BEGIN GPL BLOCK" and "END GPL BLOCK" declarations.
   Any code not declared GPL is free for all uses.

   Starting in Revision 1.237, the code to support Foveon cameras
   is under GPL.

   To lawfully redistribute dcraw.c, you must either (a) include
   full source code for all executable files containing restricted
   functions, (b) remove these functions, re-implement them, or
   copy them from an earlier, non-GPL Revision of dcraw.c, or (c)
   purchase a license from the author.

   $Revision: 1.325 $
   $Date: 2006/05/14 05:08:10 $
 */

#define _GNU_SOURCE
#define _USE_MATH_DEFINES
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*
   NO_JPEG disables decoding of compressed Kodak DC120 files.
   NO_LCMS disables the "-p" option.
 */
#ifndef NO_JPEG
#include <jpeglib.h>
#endif
#ifndef NO_LCMS
#include <lcms.h>
#endif

#ifdef __CYGWIN__
#include <io.h>
#endif
#ifdef WIN32
#include <sys/utime.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define strcasecmp stricmp
typedef __int64 INT64;
typedef unsigned __int64 UINT64;
#else
#include <unistd.h>
#include <utime.h>
#include <netinet/in.h>
typedef long long INT64;
typedef unsigned long long UINT64;
#endif

#ifdef LJPEG_DECODE
#error Please compile dcraw.c by itself.
#error Do not link it with ljpeg_decode.
#endif

#ifndef LONG_BIT
#define LONG_BIT (8 * sizeof (long))
#endif

#define ushort UshORt
typedef unsigned char uchar;
typedef unsigned short ushort;

/*
   All global variables are defined here, and all functions that
   access them are prefixed with "CLASS".  Note that a thread-safe
   C++ class cannot have non-const static local variables.
 */
FILE *ifp;
short order;
char *ifname, make[64], model[72], model2[64], *meta_data, cdesc[5];
float flash_used, canon_ev, iso_speed, shutter, aperture, focal_len;
time_t timestamp;
unsigned shot_order, kodak_cbpp, filters, unique_id;
int profile_offset, profile_length;
int thumb_offset, thumb_length, thumb_width, thumb_height, thumb_misc;
int data_offset, strip_offset, curve_offset, meta_offset, meta_length;
int tiff_nifds, tiff_flip, tiff_bps, tiff_compress;
int raw_height, raw_width, top_margin, left_margin;
int height, width, fuji_width, colors, tiff_samples;
int black, maximum, raw_color, use_gamma;
int iheight, iwidth, shrink, flip, xmag, ymag;
int zero_after_ff, is_raw, dng_version, is_foveon;
ushort (*image)[4], white[8][8], curve[0x1000], cr2_slice[3];
float bright=1, user_mul[4]={0,0,0,0}, sigma_d=0, sigma_r=0;
int four_color_rgb=0, document_mode=0, highlight=0;
int verbose=0, use_auto_wb=0, use_camera_wb=0, output_color=1;
int fuji_layout, fuji_secondary, use_secondary=0;
float cam_mul[4], pre_mul[4], rgb_cam[3][4];	/* RGB from camera color */
const double xyz_rgb[3][3] = {			/* XYZ from RGB */
  { 0.412453, 0.357580, 0.180423 },
  { 0.212671, 0.715160, 0.072169 },
  { 0.019334, 0.119193, 0.950227 } };
const float d65_white[3] = { 0.950456, 1, 1.088754 };
int histogram[4][0x2000];
void write_ppm(FILE *);
void (*write_thumb)(FILE *), (*write_fun)(FILE *);
void (*load_raw)(), (*thumb_load_raw)();
jmp_buf failure;

struct decode {
  struct decode *branch[2];
  int leaf;
} first_decode[2048], *second_decode, *free_decode;

struct {
  int width, height, bps, comp, phint, offset, flip, samples, bytes;
} tiff_ifd[10];

struct {
  int format, key_off, black, black_off, split_col, tag_21a;
  float tag_210;
} ph1;

#define CLASS

#define FORC3 for (c=0; c < 3; c++)
#define FORC4 for (c=0; c < 4; c++)
#define FORCC for (c=0; c < colors; c++)

#define SQR(x) ((x)*(x))
#define ABS(x) (((int)(x) ^ ((int)(x) >> 31)) - ((int)(x) >> 31))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIM(x,min,max) MAX(min,MIN(x,max))
#define ULIM(x,y,z) ((y) < (z) ? LIM(x,y,z) : LIM(x,z,y))
#define CLIP(x) LIM(x,0,65535)
#define SWAP(a,b) { a ^= b; a ^= (b ^= a); }

/*
   In order to inline this calculation, I make the risky
   assumption that all filter patterns can be described
   by a repeating pattern of eight rows and two columns

   Return values are either 0/1/2/3 = G/M/C/Y or 0/1/2/3 = R/G1/B/G2
 */
#define FC(row,col) \
	(filters >> ((((row) << 1 & 14) + ((col) & 1)) << 1) & 3)

#define BAYER(row,col) \
	image[((row) >> shrink)*iwidth + ((col) >> shrink)][FC(row,col)]

/*
	PowerShot 600	PowerShot A50	PowerShot Pro70	Pro90 & G1
	0xe1e4e1e4:	0x1b4e4b1e:	0x1e4b4e1b:	0xb4b4b4b4:

	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 G M G M G M	0 C Y C Y C Y	0 Y C Y C Y C	0 G M G M G M
	1 C Y C Y C Y	1 M G M G M G	1 M G M G M G	1 Y C Y C Y C
	2 M G M G M G	2 Y C Y C Y C	2 C Y C Y C Y
	3 C Y C Y C Y	3 G M G M G M	3 G M G M G M
			4 C Y C Y C Y	4 Y C Y C Y C
	PowerShot A5	5 G M G M G M	5 G M G M G M
	0x1e4e1e4e:	6 Y C Y C Y C	6 C Y C Y C Y
			7 M G M G M G	7 M G M G M G
	  0 1 2 3 4 5
	0 C Y C Y C Y
	1 G M G M G M
	2 C Y C Y C Y
	3 M G M G M G

   All RGB cameras use one of these Bayer grids:

	0x16161616:	0x61616161:	0x49494949:	0x94949494:

	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5	  0 1 2 3 4 5
	0 B G B G B G	0 G R G R G R	0 G B G B G B	0 R G R G R G
	1 G R G R G R	1 B G B G B G	1 R G R G R G	1 G B G B G B
	2 B G B G B G	2 G R G R G R	2 G B G B G B	2 R G R G R G
	3 G R G R G R	3 B G B G B G	3 R G R G R G	3 G B G B G B
 */

#ifndef __GLIBC__
char *my_memmem (char *haystack, size_t haystacklen,
	      char *needle, size_t needlelen)
{
  char *c;
  for (c = haystack; c <= haystack + haystacklen - needlelen; c++)
    if (!memcmp (c, needle, needlelen))
      return c;
  return NULL;
}
#define memmem my_memmem
#endif

void CLASS merror (void *ptr, char *where)
{
  if (ptr) return;
  fprintf (stderr, "%s: Out of memory in %s\n", ifname, where);
  longjmp (failure, 1);
}

ushort CLASS sget2 (uchar *s)
{
  if (order == 0x4949)		/* "II" means little-endian */
    return s[0] | s[1] << 8;
  else				/* "MM" means big-endian */
    return s[0] << 8 | s[1];
}

ushort CLASS get2()
{
  uchar str[2] = { 0xff,0xff };
  fread (str, 1, 2, ifp);
  return sget2(str);
}

int CLASS sget4 (uchar *s)
{
  if (order == 0x4949)
    return s[0] | s[1] << 8 | s[2] << 16 | s[3] << 24;
  else
    return s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
}
#define sget4(s) sget4((uchar *)s)

int CLASS get4()
{
  uchar str[4] = { 0xff,0xff,0xff,0xff };
  fread (str, 1, 4, ifp);
  return sget4(str);
}

int CLASS getint (int type)
{
  return type == 3 ? get2() : get4();
}

float CLASS int_to_float (int i)
{
  union { int i; float f; } u;
  u.i = i;
  return u.f;
}

double CLASS getreal (int type)
{
  union { char c[8]; double d; } u;
  int i, rev;

  switch (type) {
    case 3: return (unsigned short) get2();
    case 4: return (unsigned int) get4();
    case 5:  u.d = (unsigned int) get4();
      return u.d / (unsigned int) get4();
    case 8: return (signed short) get2();
    case 9: return (signed int) get4();
    case 10: u.d = (signed int) get4();
      return u.d / (signed int) get4();
    case 11: return int_to_float (get4());
    case 12:
      rev = 7 * ((order == 0x4949) == (ntohs(0x1234) == 0x1234));
      for (i=0; i < 8; i++)
	u.c[i ^ rev] = fgetc(ifp);
      return u.d;
    default: return fgetc(ifp);
  }
}
#define getrat() getreal(10)

void CLASS read_shorts (ushort *pixel, int count)
{
  fread (pixel, 2, count, ifp);
  if ((order == 0x4949) == (ntohs(0x1234) == 0x1234))
    swab (pixel, pixel, count*2);
}

void CLASS canon_600_fixed_wb (int temp)
{
  static const short mul[4][5] = {
    {  667, 358,397,565,452 },
    {  731, 390,367,499,517 },
    { 1119, 396,348,448,537 },
    { 1399, 485,431,508,688 } };
  int lo, hi, i;
  float frac=0;

  for (lo=4; --lo; )
    if (*mul[lo] <= temp) break;
  for (hi=0; hi < 3; hi++)
    if (*mul[hi] >= temp) break;
  if (lo != hi)
    frac = (float) (temp - *mul[lo]) / (*mul[hi] - *mul[lo]);
  for (i=1; i < 5; i++)
    pre_mul[i-1] = 1 / (frac * mul[hi][i] + (1-frac) * mul[lo][i]);
}

/* Return values:  0 = white  1 = near white  2 = not white */
int CLASS canon_600_color (int ratio[2], int mar)
{
  int clipped=0, target, miss;

  if (flash_used) {
    if (ratio[1] < -104)
      { ratio[1] = -104; clipped = 1; }
    if (ratio[1] >   12)
      { ratio[1] =   12; clipped = 1; }
  } else {
    if (ratio[1] < -264 || ratio[1] > 461) return 2;
    if (ratio[1] < -50)
      { ratio[1] = -50; clipped = 1; }
    if (ratio[1] > 307)
      { ratio[1] = 307; clipped = 1; }
  }
  target = flash_used || ratio[1] < 197
	? -38 - (398 * ratio[1] >> 10)
	: -123 + (48 * ratio[1] >> 10);
  if (target - mar <= ratio[0] &&
      target + 20  >= ratio[0] && !clipped) return 0;
  miss = target - ratio[0];
  if (abs(miss) >= mar*4) return 2;
  if (miss < -20) miss = -20;
  if (miss > mar) miss = mar;
  ratio[0] = target - miss;
  return 1;
}

void CLASS canon_600_auto_wb()
{
  int mar, row, col, i, j, st, count[] = { 0,0 };
  int test[8], total[2][8], ratio[2][2], stat[2];

  memset (&total, 0, sizeof total);
  i = canon_ev + 0.5;
  if      (i < 10) mar = 150;
  else if (i > 12) mar = 20;
  else mar = 280 - 20 * i;
  if (flash_used) mar = 80;
  for (row=14; row < height-14; row+=4)
    for (col=10; col < width; col+=2) {
      for (i=0; i < 8; i++)
	test[(i & 4) + FC(row+(i >> 1),col+(i & 1))] =
		    BAYER(row+(i >> 1),col+(i & 1));
      for (i=0; i < 8; i++)
	if (test[i] < 150 || test[i] > 1500) goto next;
      for (i=0; i < 4; i++)
	if (abs(test[i] - test[i+4]) > 50) goto next;
      for (i=0; i < 2; i++) {
	for (j=0; j < 4; j+=2)
	  ratio[i][j >> 1] = ((test[i*4+j+1]-test[i*4+j]) << 10) / test[i*4+j];
	stat[i] = canon_600_color (ratio[i], mar);
      }
      if ((st = stat[0] | stat[1]) > 1) goto next;
      for (i=0; i < 2; i++)
	if (stat[i])
	  for (j=0; j < 2; j++)
	    test[i*4+j*2+1] = test[i*4+j*2] * (0x400 + ratio[i][j]) >> 10;
      for (i=0; i < 8; i++)
	total[st][i] += test[i];
      count[st]++;
next: continue;
    }
  if (count[0] | count[1]) {
    st = count[0]*200 < count[1];
    for (i=0; i < 4; i++)
      pre_mul[i] = 1.0 / (total[st][i] + total[st][i+4]);
  }
}

void CLASS canon_600_coeff()
{
  static const short table[6][12] = {
    { -190,702,-1878,2390,   1861,-1349,905,-393, -432,944,2617,-2105  },
    { -1203,1715,-1136,1648, 1388,-876,267,245,  -1641,2153,3921,-3409 },
    { -615,1127,-1563,2075,  1437,-925,509,3,     -756,1268,2519,-2007 },
    { -190,702,-1886,2398,   2153,-1641,763,-251, -452,964,3040,-2528  },
    { -190,702,-1878,2390,   1861,-1349,905,-393, -432,944,2617,-2105  },
    { -807,1319,-1785,2297,  1388,-876,769,-257,  -230,742,2067,-1555  } };
  int t=0, i, c;
  float mc, yc;

  mc = pre_mul[1] / pre_mul[2];
  yc = pre_mul[3] / pre_mul[2];
  if (mc > 1 && mc <= 1.28 && yc < 0.8789) t=1;
  if (mc > 1.28 && mc <= 2) {
    if  (yc < 0.8789) t=3;
    else if (yc <= 2) t=4;
  }
  if (flash_used) t=5;
  for (raw_color = i=0; i < 3; i++)
    FORCC rgb_cam[i][c] = table[t][i*4 + c] / 1024.0;
}

void CLASS canon_600_load_raw()
{
  uchar  data[1120], *dp;
  ushort pixel[896], *pix;
  int irow, row, col, val;
  static const short mul[4][2] =
  { { 1141,1145 }, { 1128,1109 }, { 1178,1149 }, { 1128,1109 } };

  for (irow=row=0; irow < height; irow++)
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
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col];
    for (col=width; col < 896; col++)
      black += pixel[col];
    if ((row+=2) > height) row = 1;
  }
  black = black / ((896 - width) * height) - 4;
  for (row=0; row < height; row++)
    for (col=0; col < width; col++) {
      val = (BAYER(row,col) - black) * mul[row & 3][col & 1] >> 9;
      if (val < 0) val = 0;
      BAYER(row,col) = val;
    }
  canon_600_fixed_wb(1311);
  canon_600_auto_wb();
  canon_600_coeff();
  maximum = (0x3ff - black) * 1109 >> 9;
  black = 0;
}

void CLASS canon_a5_load_raw()
{
  uchar  data[1940], *dp;
  ushort pixel[1552], *pix;
  int row, col;

  for (row=0; row < height; row++) {
    fread (data, raw_width * 10 / 8, 1, ifp);
    for (dp=data, pix=pixel; pix < pixel+raw_width; dp+=10, pix+=8)
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
    for (col=0; col < width; col++)
      BAYER(row,col) = (pixel[col] & 0x3ff);
    for (col=width; col < raw_width; col++)
      black += pixel[col] & 0x3ff;
  }
  if (raw_width > width)
    black /= (raw_width - width) * height;
  maximum = 0x3ff;
}

/*
   getbits(-1) initializes the buffer
   getbits(n) where 0 <= n <= 25 returns an n-bit integer
 */
unsigned CLASS getbits (int nbits)
{
  static unsigned bitbuf=0;
  static int vbits=0, reset=0;
  unsigned c;

  if (nbits == -1)
    return bitbuf = vbits = reset = 0;
  if (nbits == 0 || reset) return 0;
  while (vbits < nbits) {
    c = fgetc(ifp);
    if ((reset = zero_after_ff && c == 0xff && fgetc(ifp))) return 0;
    bitbuf = (bitbuf << 8) + c;
    vbits += 8;
  }
  vbits -= nbits;
  return bitbuf << (32-nbits-vbits) >> (32-nbits);
}

void CLASS init_decoder()
{
  memset (first_decode, 0, sizeof first_decode);
  free_decode = first_decode;
}

/*
   Construct a decode tree according the specification in *source.
   The first 16 bytes specify how many codes should be 1-bit, 2-bit
   3-bit, etc.  Bytes after that are the leaf values.

   For example, if the source is

    { 0,1,4,2,3,1,2,0,0,0,0,0,0,0,0,0,
      0x04,0x03,0x05,0x06,0x02,0x07,0x01,0x08,0x09,0x00,0x0a,0x0b,0xff  },

   then the code is

	00		0x04
	010		0x03
	011		0x05
	100		0x06
	101		0x02
	1100		0x07
	1101		0x01
	11100		0x08
	11101		0x09
	11110		0x00
	111110		0x0a
	1111110		0x0b
	1111111		0xff
 */
uchar * CLASS make_decoder (const uchar *source, int level)
{
  struct decode *cur;
  static int leaf;
  int i, next;

  if (level==0) leaf=0;
  cur = free_decode++;
  if (free_decode > first_decode+2048) {
    fprintf (stderr, "%s: decoder table overflow\n", ifname);
    longjmp (failure, 2);
  }
  for (i=next=0; i <= leaf && next < 16; )
    i += source[next++];
  if (i > leaf) {
    if (level < next) {
      cur->branch[0] = free_decode;
      make_decoder (source, level+1);
      cur->branch[1] = free_decode;
      make_decoder (source, level+1);
    } else
      cur->leaf = source[16 + leaf++];
  }
  return (uchar *) source + 16 + leaf;
}

void CLASS crw_init_tables (unsigned table)
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
  init_decoder();
  make_decoder ( first_tree[table], 0);
  second_decode = free_decode;
  make_decoder (second_tree[table], 0);
}

/*
   Return 0 if the image starts with compressed data,
   1 if it starts with uncompressed low-order bits.

   In Canon compressed data, 0xff is always followed by 0x00.
 */
int CLASS canon_has_lowbits()
{
  uchar test[0x4000];
  int ret=1, i;

  fseek (ifp, 0, SEEK_SET);
  fread (test, 1, sizeof test, ifp);
  for (i=540; i < sizeof test - 1; i++)
    if (test[i] == 0xff) {
      if (test[i+1]) return 1;
      ret=0;
    }
  return ret;
}

void CLASS canon_compressed_load_raw()
{
  ushort *pixel, *prow;
  int lowbits, i, row, r, col, save, val;
  unsigned irow, icol;
  struct decode *decode, *dindex;
  int block, diffbuf[64], leaf, len, diff, carry=0, pnum=0, base[2];
  uchar c;

  crw_init_tables (tiff_compress);
  pixel = calloc (raw_width*8, sizeof *pixel);
  merror (pixel, "canon_compressed_load_raw()");
  lowbits = canon_has_lowbits();
  if (!lowbits) maximum = 0x3ff;
  fseek (ifp, 540 + lowbits*raw_height*raw_width/4, SEEK_SET);
  zero_after_ff = 1;
  getbits(-1);
  for (row = 0; row < raw_height; row += 8) {
    for (block=0; block < raw_width >> 3; block++) {
      memset (diffbuf, 0, sizeof diffbuf);
      decode = first_decode;
      for (i=0; i < 64; i++ ) {
	for (dindex=decode; dindex->branch[0]; )
	  dindex = dindex->branch[getbits(1)];
	leaf = dindex->leaf;
	decode = second_decode;
	if (leaf == 0 && i) break;
	if (leaf == 0xff) continue;
	i  += leaf >> 4;
	len = leaf & 15;
	if (len == 0) continue;
	diff = getbits(len);
	if ((diff & (1 << (len-1))) == 0)
	  diff -= (1 << len) - 1;
	if (i < 64) diffbuf[i] = diff;
      }
      diffbuf[0] += carry;
      carry = diffbuf[0];
      for (i=0; i < 64; i++ ) {
	if (pnum++ % raw_width == 0)
	  base[0] = base[1] = 512;
	pixel[(block << 6) + i] = ( base[i & 1] += diffbuf[i] );
      }
    }
    if (lowbits) {
      save = ftell(ifp);
      fseek (ifp, 26 + row*raw_width/4, SEEK_SET);
      for (prow=pixel, i=0; i < raw_width*2; i++) {
	c = fgetc(ifp);
	for (r=0; r < 8; r+=2, prow++) {
	  val = (*prow << 2) + ((c >> r) & 3);
	  if (raw_width == 2672 && val < 512) val += 2;
	  *prow = val;
	}
      }
      fseek (ifp, save, SEEK_SET);
    }
    for (r=0; r < 8; r++) {
      irow = row - top_margin + r;
      if (irow >= height) continue;
      for (col = 0; col < raw_width; col++) {
	icol = col - left_margin;
	if (icol < width)
	  BAYER(irow,icol) = pixel[r*raw_width+col];
	else
	  black += pixel[r*raw_width+col];
      }
    }
  }
  free (pixel);
  if (raw_width > width)
    black /= (raw_width - width) * height;
}

/*
   Not a full implementation of Lossless JPEG, just
   enough to decode Canon, Kodak and Adobe DNG images.
 */
struct jhead {
  int bits, high, wide, clrs, restart, vpred[4];
  struct decode *huff[4];
  ushort *row;
};

int CLASS ljpeg_start (struct jhead *jh, int info_only)
{
  int i, tag, len;
  uchar data[0x10000], *dp;

  init_decoder();
  for (i=0; i < 4; i++)
    jh->huff[i] = free_decode;
  jh->restart = INT_MAX;
  fread (data, 2, 1, ifp);
  if (data[1] != 0xd8) return 0;
  do {
    fread (data, 2, 2, ifp);
    tag =  data[0] << 8 | data[1];
    len = (data[2] << 8 | data[3]) - 2;
    if (tag <= 0xff00) return 0;
    fread (data, 1, len, ifp);
    switch (tag) {
      case 0xffc0:
      case 0xffc3:
	jh->bits = data[0];
	jh->high = data[1] << 8 | data[2];
	jh->wide = data[3] << 8 | data[4];
	jh->clrs = data[5];
	break;
      case 0xffc4:
	if (info_only) break;
	for (dp = data; dp < data+len && *dp < 4; ) {
	  jh->huff[*dp] = free_decode;
	  dp = make_decoder (++dp, 0);
	}
	break;
      case 0xffdd:
	jh->restart = data[0] << 8 | data[1];
    }
  } while (tag != 0xffda);
  if (info_only) return 1;
  jh->row = calloc (jh->wide*jh->clrs, 2);
  merror (jh->row, " jpeg_start()");
  return zero_after_ff = 1;
}

int CLASS ljpeg_diff (struct decode *dindex)
{
  int len, diff;

  while (dindex->branch[0])
    dindex = dindex->branch[getbits(1)];
  len = dindex->leaf;
  if (len == 16 && (!dng_version || dng_version >= 0x1010000))
    return -32768;
  diff = getbits(len);
  if ((diff & (1 << (len-1))) == 0)
    diff -= (1 << len) - 1;
  return diff;
}

void CLASS ljpeg_row (int jrow, struct jhead *jh)
{
  int col, c, diff;
  ushort *outp=jh->row;

  if (jrow * jh->wide % jh->restart == 0) {
    FORC4 jh->vpred[c] = 1 << (jh->bits-1);
    if (jrow) get2();			/* Eat the FF Dx marker */
    getbits(-1);
  }
  for (col=0; col < jh->wide; col++)
    for (c=0; c < jh->clrs; c++) {
      diff = ljpeg_diff (jh->huff[c]);
      *outp = col ? outp[-jh->clrs]+diff : (jh->vpred[c] += diff);
      outp++;
    }
}

void CLASS lossless_jpeg_load_raw()
{
  int jwide, jrow, jcol, val, jidx, i, j, row=0, col=0;
  struct jhead jh;
  int min=INT_MAX;

  if (!ljpeg_start (&jh, 0)) return;
  jwide = jh.wide * jh.clrs;

  for (jrow=0; jrow < jh.high; jrow++) {
    ljpeg_row (jrow, &jh);
    for (jcol=0; jcol < jwide; jcol++) {
      val = jh.row[jcol];
      if (jh.bits <= 12)
	val = curve[val];
      if (cr2_slice[0]) {
	jidx = jrow*jwide + jcol;
	i = jidx / (cr2_slice[1]*jh.high);
	if ((j = i >= cr2_slice[0]))
		 i  = cr2_slice[0];
	jidx -= i * (cr2_slice[1]*jh.high);
	row = jidx / cr2_slice[1+j];
	col = jidx % cr2_slice[1+j] + i*cr2_slice[1];
      }
      if ((unsigned) (row-top_margin) < height) {
	if ((unsigned) (col-left_margin) < width) {
	  BAYER(row-top_margin,col-left_margin) = val;
	  if (min > val) min = val;
	} else black += val;
      }
      if (++col >= raw_width)
	col = (row++,0);
    }
  }
  free (jh.row);
  if (raw_width > width)
    black /= (raw_width - width) * height;
  if (!strcasecmp(make,"KODAK"))
    black = min;
}

void CLASS adobe_copy_pixel (int row, int col, ushort **rp)
{
  unsigned r, c;

  r = row -= top_margin;
  c = col -= left_margin;
  if (fuji_secondary && use_secondary) (*rp)++;
  if (filters) {
    if (fuji_width) {
      r = row + fuji_width - 1 - (col >> 1);
      c = row + ((col+1) >> 1);
    }
    if (r < height && c < width)
      BAYER(r,c) = **rp < 0x1000 ? curve[**rp] : **rp;
    *rp += 1 + fuji_secondary;
  } else {
    if (r < height && c < width)
      for (c=0; c < tiff_samples; c++)
	image[row*width+col][c] = (*rp)[c] < 0x1000 ? curve[(*rp)[c]]:(*rp)[c];
    *rp += tiff_samples;
  }
  if (fuji_secondary && use_secondary) (*rp)--;
}

void CLASS adobe_dng_load_raw_lj()
{
  int save, twide, trow=0, tcol=0, jrow, jcol;
  struct jhead jh;
  ushort *rp;

  while (1) {
    save = ftell(ifp);
    fseek (ifp, get4(), SEEK_SET);
    if (!ljpeg_start (&jh, 0)) break;
    if (trow >= raw_height) break;
    if (jh.high > raw_height-trow)
	jh.high = raw_height-trow;
    twide = jh.wide;
    if (filters) twide *= jh.clrs;
    else         colors = jh.clrs;
    if (fuji_secondary) twide /= 2;
    if (twide > raw_width-tcol)
	twide = raw_width-tcol;

    for (jrow=0; jrow < jh.high; jrow++) {
      ljpeg_row (jrow, &jh);
      for (rp=jh.row, jcol=0; jcol < twide; jcol++)
	adobe_copy_pixel (trow+jrow, tcol+jcol, &rp);
    }
    fseek (ifp, save+4, SEEK_SET);
    if ((tcol += twide) >= raw_width) {
      tcol = 0;
      trow += jh.high;
    }
    free (jh.row);
  }
}

void CLASS adobe_dng_load_raw_nc()
{
  ushort *pixel, *rp;
  int row, col;

  pixel = calloc (raw_width * tiff_samples, sizeof *pixel);
  merror (pixel, "adobe_dng_load_raw_nc()");
  for (row=0; row < raw_height; row++) {
    if (tiff_bps == 16)
      read_shorts (pixel, raw_width * tiff_samples);
    else {
      getbits(-1);
      for (col=0; col < raw_width * tiff_samples; col++)
	pixel[col] = getbits(tiff_bps);
    }
    for (rp=pixel, col=0; col < raw_width; col++)
      adobe_copy_pixel (row, col, &rp);
  }
  free (pixel);
}

void CLASS nikon_compressed_load_raw()
{
  static const uchar nikon_tree[] = {
    0,1,5,1,1,1,1,1,1,2,0,0,0,0,0,0,
    5,4,3,6,2,7,1,0,8,9,11,10,12
  };
  int csize, row, col, i, diff;
  ushort vpred[4], hpred[2], *curve;

  init_decoder();
  make_decoder (nikon_tree, 0);

  fseek (ifp, curve_offset, SEEK_SET);
  read_shorts (vpred, 4);
  csize = get2();
  curve = calloc (csize, sizeof *curve);
  merror (curve, "nikon_compressed_load_raw()");
  read_shorts (curve, csize);

  fseek (ifp, data_offset, SEEK_SET);
  getbits(-1);

  for (row=0; row < height; row++)
    for (col=0; col < raw_width; col++)
    {
      diff = ljpeg_diff (first_decode);
      if (col < 2) {
	i = 2*(row & 1) + (col & 1);
	vpred[i] += diff;
	hpred[col] = vpred[i];
      } else
	hpred[col & 1] += diff;
      if ((unsigned) (col-left_margin) >= width) continue;
      diff = hpred[col & 1];
      if (diff >= csize) diff = csize-1;
      BAYER(row,col-left_margin) = curve[diff];
    }
  free (curve);
}

void CLASS nikon_load_raw()
{
  int irow, row, col, i;

  getbits(-1);
  for (irow=0; irow < height; irow++) {
    row = irow;
    if (make[0] == 'O' || model[0] == 'E') {
      row = irow * 2 % height + irow / (height/2);
      if (row == 1 && data_offset == 0) {
	fseek (ifp, 0, SEEK_END);
	fseek (ifp, ftell(ifp)/2, SEEK_SET);
	getbits(-1);
      }
    }
    for (col=0; col < raw_width; col++) {
      i = getbits(12);
      if ((unsigned) (col-left_margin) < width)
	BAYER(row,col-left_margin) = i;
      if (tiff_compress == 34713 && (col % 10) == 9)
	getbits(8);
    }
  }
}

/*
   Figure out if a NEF file is compressed.  These fancy heuristics
   are only needed for the D100, thanks to a bug in some cameras
   that tags all images as "compressed".
 */
int CLASS nikon_is_compressed()
{
  uchar test[256];
  int i;

  if (tiff_compress != 34713)
    return 0;
  if (strcmp(model,"D100"))
    return 1;
  fseek (ifp, data_offset, SEEK_SET);
  fread (test, 1, 256, ifp);
  for (i=15; i < 256; i+=16)
    if (test[i]) return 1;
  return 0;
}

/*
   Returns 1 for a Coolpix 995, 0 for anything else.
 */
int CLASS nikon_e995()
{
  int i, histo[256];
  const uchar often[] = { 0x00, 0x55, 0xaa, 0xff };

  memset (histo, 0, sizeof histo);
  fseek (ifp, -2000, SEEK_END);
  for (i=0; i < 2000; i++)
    histo[fgetc(ifp)]++;
  for (i=0; i < 4; i++)
    if (histo[often[i]] < 200)
      return 0;
  return 1;
}

/*
   Returns 1 for a Coolpix 2100, 0 for anything else.
 */
int CLASS nikon_e2100()
{
  uchar t[12];
  int i;

  fseek (ifp, 0, SEEK_SET);
  for (i=0; i < 1024; i++) {
    fread (t, 1, 12, ifp);
    if (((t[2] & t[4] & t[7] & t[9]) >> 4
	& t[1] & t[6] & t[8] & t[11] & 3) != 3)
      return 0;
  }
  return 1;
}

/*
   Returns 0 for a Pentax Optio 33WR,
	   1 for a Nikon E3700,
	   2 for an Olympus C740UZ.
 */
int CLASS nikon_3700()
{
  int i, sum[] = { 0, 0 };
  uchar tail[952];

  fseek (ifp, -sizeof tail, SEEK_END);
  fread (tail, 1, sizeof tail, ifp);
  for (i=0; i < sizeof tail; i++)
    sum[(i>>2) & 1] += tail[i];
  if (sum[1] > 4*sum[0]) return 2;
  return sum[0] > 4*sum[1];
}

/*
   Separates a Minolta DiMAGE Z2 from a Nikon E4300.
 */
int CLASS minolta_z2()
{
  int i;
  char tail[424];

  fseek (ifp, -sizeof tail, SEEK_END);
  fread (tail, 1, sizeof tail, ifp);
  for (i=0; i < sizeof tail; i++)
    if (tail[i]) return 1;
  return 0;
}

/* Here raw_width is in bytes, not pixels. */
void CLASS nikon_e900_load_raw()
{
  int offset=0, irow, row, col;

  for (irow=0; irow < height; irow++) {
    row = irow * 2 % height;
    if (row == 1)
      offset = - (-offset & -4096);
    fseek (ifp, offset, SEEK_SET);
    offset += raw_width;
    getbits(-1);
    for (col=0; col < width; col++)
      BAYER(row,col) = getbits(10);
  }
}

void CLASS nikon_e2100_load_raw()
{
  uchar   data[3456], *dp;
  ushort pixel[2304], *pix;
  int row, col;

  for (row=0; row <= height; row+=2) {
    if (row == height) {
      fseek (ifp, ((width==1616) << 13) - (-ftell(ifp) & -2048), SEEK_SET);
      row = 1;
    }
    fread (data, 1, width*3/2, ifp);
    for (dp=data, pix=pixel; pix < pixel+width; dp+=12, pix+=8) {
      pix[0] = (dp[2] >> 4) + (dp[ 3] << 4);
      pix[1] = (dp[2] << 8) +  dp[ 1];
      pix[2] = (dp[7] >> 4) + (dp[ 0] << 4);
      pix[3] = (dp[7] << 8) +  dp[ 6];
      pix[4] = (dp[4] >> 4) + (dp[ 5] << 4);
      pix[5] = (dp[4] << 8) +  dp[11];
      pix[6] = (dp[9] >> 4) + (dp[10] << 4);
      pix[7] = (dp[9] << 8) +  dp[ 8];
    }
    for (col=0; col < width; col++)
      BAYER(row,col) = (pixel[col] & 0xfff);
  }
}

/*
   The Fuji Super CCD is just a Bayer grid rotated 45 degrees.
 */
void CLASS fuji_load_raw()
{
  ushort *pixel;
  int row, col, r, c;

  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "fuji_load_raw()");
  for (row=0; row < raw_height; row++) {
    read_shorts (pixel, raw_width);
    for (col=0; col < fuji_width << !fuji_layout; col++) {
      if (fuji_layout) {
	r = fuji_width - 1 - col + (row >> 1);
	c = col + ((row+1) >> 1);
      } else {
	r = fuji_width - 1 + row - (col >> 1);
	c = row + ((col+1) >> 1);
      }
      BAYER(r,c) = pixel[col];
    }
  }
  free (pixel);
}

void CLASS jpeg_thumb (FILE *tfp)
{
  char *thumb = malloc (thumb_length);
  merror (thumb, "jpeg_thumb()");
  fread  (thumb, 1, thumb_length, ifp);
  thumb[0] = 0xff;
  fwrite (thumb, 1, thumb_length, tfp);
  free (thumb);
}

void CLASS ppm_thumb (FILE *tfp)
{
  char *thumb = malloc (thumb_length);
  merror (thumb, "ppm_thumb()");
  fprintf (tfp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  fread  (thumb, 1, thumb_length, ifp);
  fwrite (thumb, 1, thumb_length, tfp);
  free (thumb);
}

void CLASS layer_thumb (FILE *tfp)
{
  int i, c;
  char *thumb;
  colors = thumb_misc >> 5;
  thumb = malloc (thumb_length*colors);
  merror (thumb, "layer_thumb()");
  fprintf (tfp, "P%d\n%d %d\n255\n",
	5 + (thumb_misc >> 6), thumb_width, thumb_height);
  fread (thumb, thumb_length, colors, ifp);
  for (i=0; i < thumb_length; i++)
    FORCC putc (thumb[i+thumb_length*c], tfp);
  free (thumb);
}

void CLASS rollei_thumb (FILE *tfp)
{
  int i, size = thumb_width * thumb_height;
  ushort *thumb = calloc (size, 2);
  merror (thumb, "rollei_thumb()");
  fprintf (tfp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  read_shorts (thumb, size);
  for (i=0; i < size; i++) {
    putc (thumb[i] << 3, tfp);
    putc (thumb[i] >> 5  << 2, tfp);
    putc (thumb[i] >> 11 << 3, tfp);
  }
  free (thumb);
}

void CLASS rollei_load_raw()
{
  uchar pixel[10];
  unsigned iten=0, isix, i, buffer=0, row, col, todo[16];

  isix = raw_width * raw_height * 5 / 8;
  while (fread (pixel, 1, 10, ifp) == 10) {
    for (i=0; i < 10; i+=2) {
      todo[i]   = iten++;
      todo[i+1] = pixel[i] << 8 | pixel[i+1];
      buffer    = pixel[i] >> 2 | buffer << 6;
    }
    for (   ; i < 16; i+=2) {
      todo[i]   = isix++;
      todo[i+1] = buffer >> (14-i)*5;
    }
    for (i=0; i < 16; i+=2) {
      row = todo[i] / raw_width - top_margin;
      col = todo[i] % raw_width - left_margin;
      if (row < height && col < width)
	BAYER(row,col) = (todo[i+1] & 0x3ff);
    }
  }
  maximum = 0x3ff;
}

int CLASS bayer (unsigned row, unsigned col)
{
  return (row < height && col < width) ? BAYER(row,col) : 0;
}

void CLASS phase_one_flat_field (int is_float, int nc)
{
  ushort head[8];
  unsigned wide, y, x, c, rend, cend, row, col;
  float *mrow, num, mult[4];

  read_shorts (head, 8);
  wide = head[2] / head[4];
  mrow = calloc (nc*wide, sizeof *mrow);
  merror (mrow, "phase_one_flat_field()");
  for (y=0; y < head[3] / head[5]; y++) {
    for (x=0; x < wide; x++)
      for (c=0; c < nc; c+=2) {
	num = is_float ? getreal(11) : get2()/32768.0;
	if (y==0) mrow[c*wide+x] = num;
	else mrow[(c+1)*wide+x] = (num - mrow[c*wide+x]) / head[5];
      }
    if (y==0) continue;
    rend = head[1]-top_margin + y*head[5];
    for (row = rend-head[5]; row < height && row < rend; row++) {
      for (x=1; x < wide; x++) {
	for (c=0; c < nc; c+=2) {
	  mult[c] = mrow[c*wide+x-1];
	  mult[c+1] = (mrow[c*wide+x] - mult[c]) / head[4];
	}
	cend = head[0]-left_margin + x*head[4];
	for (col = cend-head[4]; col < width && col < cend; col++) {
	  c = nc > 2 ? FC(row,col) : 0;
	  if (!(c & 1)) {
	    c = BAYER(row,col) * mult[c];
	    BAYER(row,col) = LIM(c,0,65535);
	  }
	  for (c=0; c < nc; c+=2)
	    mult[c] += mult[c+1];
	}
      }
      for (x=0; x < wide; x++)
	for (c=0; c < nc; c+=2)
	  mrow[c*wide+x] += mrow[(c+1)*wide+x];
    }
  }
  free (mrow);
}

void CLASS phase_one_correct()
{
  unsigned entries, tag, data, save, col, row, type;
  int len, i, j, k, cip, val[4], dev[4], sum, max;
  int head[9], diff, mindiff=INT_MAX, off_412=0;
  static const signed char dir[12][2] =
    { {-1,-1}, {-1,1}, {1,-1}, {1,1}, {-2,0}, {0,-2}, {0,2}, {2,0},
      {-2,-2}, {-2,2}, {2,-2}, {2,2} };
  float poly[8], num, cfrac, frac, mult[2], *yval[2];
  ushort curve[0x10000], *xval[2];

  if (shrink || !meta_length) return;
  if (verbose) fprintf (stderr, "Phase One correction...\n");
  fseek (ifp, meta_offset, SEEK_SET);
  order = get2();
  fseek (ifp, 6, SEEK_CUR);
  fseek (ifp, meta_offset+get4(), SEEK_SET);
  entries = get4();  get4();
  while (entries--) {
    tag  = get4();
    len  = get4();
    data = get4();
    save = ftell(ifp);
    fseek (ifp, meta_offset+data, SEEK_SET);
    if (tag == 0x419) {				/* Polynomial curve */
      for (get4(), i=0; i < 8; i++)
	poly[i] = getreal(11);
      poly[3] += (ph1.tag_210 - poly[7]) * poly[6] + 1;
      for (i=0; i < 0x10000; i++) {
	num = (poly[5]*i + poly[3])*i + poly[1];
	curve[i] = LIM(num,0,65535);
      } goto apply;				/* apply to right half */
    } else if (tag == 0x41a) {			/* Polynomial curve */
      for (i=0; i < 4; i++)
	poly[i] = getreal(11);
      for (i=0; i < 0x10000; i++) {
	for (num=0, j=4; j--; )
	  num = num * i + poly[j];
	curve[i] = LIM(num+i,0,65535);
      } apply:					/* apply to whole image */
      for (row=0; row < height; row++)
	for (col = (tag & 1)*ph1.split_col; col < width; col++)
	  BAYER(row,col) = curve[BAYER(row,col)];
    } else if (tag == 0x400) {			/* Sensor defects */
      while ((len -= 8) >= 0) {
	col  = get2() - left_margin;
	row  = get2() - top_margin;
	type = get2(); get2();
	if (col >= width) continue;
	if (type == 131)			/* Bad column */
	  for (row=0; row < height; row++)
	    if (FC(row,col) == 1) {
	      for (sum=i=0; i < 4; i++)
		sum += val[i] = bayer (row+dir[i][0], col+dir[i][1]);
	      for (max=i=0; i < 4; i++) {
		dev[i] = abs((val[i] << 2) - sum);
		if (dev[max] < dev[i]) max = i;
	      }
	      BAYER(row,col) = (sum - val[max])/3.0 + 0.5;
	    } else {
	      for (sum=0, i=8; i < 12; i++)
		sum += bayer (row+dir[i][0], col+dir[i][1]);
	      BAYER(row,col) = 0.5 + sum * 0.0732233 +
		(bayer(row,col-2) + bayer(row,col+2)) * 0.3535534;
	    }
	else if (type == 129) {			/* Bad pixel */
	  if (row >= height) continue;
	  j = (FC(row,col) != 1) * 4;
	  for (sum=0, i=j; i < j+8; i++)
	    sum += bayer (row+dir[i][0], col+dir[i][1]);
	  BAYER(row,col) = (sum + 4) >> 3;
	}
      }
    } else if (tag == 0x401) {			/* All-color flat fields */
      phase_one_flat_field (1, 2);
    } else if (tag == 0x416 || tag == 0x410) {
      phase_one_flat_field (0, 2);
    } else if (tag == 0x40b) {			/* Red+blue flat field */
      phase_one_flat_field (0, 4);
    } else if (tag == 0x412) {
      fseek (ifp, 36, SEEK_CUR);
      diff = abs (get2() - ph1.tag_21a);
      if (mindiff > diff) {
	mindiff = diff;
	off_412 = ftell(ifp) - 38;
      }
    }
    fseek (ifp, save, SEEK_SET);
  }
  if (off_412) {
    fseek (ifp, off_412, SEEK_SET);
    for (i=0; i < 9; i++) head[i] = get4();
    yval[0] = calloc (head[1]*head[3] + head[2]*head[4], 6);
    merror (yval[0], "phase_one_correct()");
    yval[1] = (float  *) (yval[0] + head[1]*head[3]);
    xval[0] = (ushort *) (yval[1] + head[2]*head[4]);
    xval[1] = (ushort *) (xval[0] + head[1]*head[3]);
    get2();
    for (i=0; i < 2; i++)
      for (j=0; j < head[i+1]*head[i+3]; j++)
	yval[i][j] = getreal(11);
    for (i=0; i < 2; i++)
      for (j=0; j < head[i+1]*head[i+3]; j++)
	xval[i][j] = get2();
    for (row=0; row < height; row++)
      for (col=0; col < width; col++) {
	cfrac = (float) col * head[3] / raw_width;
	cfrac -= cip = cfrac;
	num = BAYER(row,col) * 0.5;
	for (i=cip; i < cip+2; i++) {
	  for (k=j=0; j < head[1]; j++)
	    if (num < xval[0][k = head[1]*i+j]) break;
	  frac = (j == 0 || j == head[1]) ? 0 :
		(xval[0][k] - num) / (xval[0][k] - xval[0][k-1]);
	  mult[i-cip] = yval[0][k-1] * frac + yval[0][k] * (1-frac);
	}
	i = ((mult[0] * (1-cfrac) + mult[1] * cfrac)
		* (row + top_margin) + num) * 2;
	BAYER(row,col) = LIM(i,0,65535);
      }
    free (yval[0]);
  }
}

void CLASS phase_one_load_raw()
{
  int row, col, a, b;
  ushort *pixel, akey, bkey, mask;

  fseek (ifp, ph1.key_off, SEEK_SET);
  akey = get2();
  bkey = get2();
  mask = ph1.format == 1 ? 0x5555:0x1354;
  fseek (ifp, data_offset + top_margin*raw_width*2, SEEK_SET);
  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "phase_one_load_raw()");
  for (row=0; row < height; row++) {
    read_shorts (pixel, raw_width);
    for (col=0; col < raw_width; col+=2) {
      a = pixel[col+0] ^ akey;
      b = pixel[col+1] ^ bkey;
      pixel[col+0] = (a & mask) | (b & ~mask);
      pixel[col+1] = (b & mask) | (a & ~mask);
    }
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col+left_margin];
  }
  free (pixel);
  phase_one_correct();
}

unsigned CLASS ph1_bits (int nbits)
{
  static UINT64 bitbuf=0;
  static int vbits=0;

  if (nbits == 0)
    return bitbuf = vbits = 0;
  if (vbits < nbits) {
    bitbuf = bitbuf << 32 | (unsigned) get4();
    vbits += 32;
  }
  vbits -= nbits;
  return bitbuf << (64 - nbits - vbits) >> (64 - nbits);
}

void CLASS phase_one_load_raw_c()
{
  static const int length[] = { 8,7,6,9,11,10,5,12,14,13 };
  int *offset, len[2], pred[2], row, col, i, j;
  ushort *pixel;
  short (*black)[2];

  pixel = calloc (raw_width + raw_height*4, 2);
  merror (pixel, "phase_one_load_raw_c()");
  offset = (int *) (pixel + raw_width);
  fseek (ifp, strip_offset, SEEK_SET);
  for (row=0; row < raw_height; row++)
    offset[row] = get4();
  black = (short (*)[2]) offset + raw_height;
  fseek (ifp, ph1.black_off, SEEK_SET);
  if (ph1.black_off)
    read_shorts ((ushort *) black[0], raw_height*2);
  for (row=0; row < raw_height; row++) {
    fseek (ifp, data_offset + offset[row], SEEK_SET);
    ph1_bits(0);
    pred[0] = pred[1] = 0;
    for (col=0; col < raw_width; col++) {
      if (col >= (raw_width & -8))
	len[0] = len[1] = 14;
      else if ((col & 7) == 0)
	for (i=0; i < 2; i++) {
	  for (j=0; j < 5 && !ph1_bits(1); j++);
	  if (j--) len[i] = length[j*2 + ph1_bits(1)];
	}
      if ((i = len[col & 1]) == 14)
	pixel[col] = pred[col & 1] = ph1_bits(16);
      else
	pixel[col] = pred[col & 1] += ph1_bits(i) + 1 - (1 << (i - 1));
    }
    if ((unsigned) (row-top_margin) < height)
      for (col=0; col < width; col++) {
	i = (pixel[col+left_margin] << 2)
		- ph1.black + black[row][col >= ph1.split_col];
	if (i > 0) BAYER(row-top_margin,col) = i;
      }
  }
  free (pixel);
  phase_one_correct();
}

void CLASS leaf_full_load_raw()
{
  ushort *pixel;
  int r, c, row, col;

  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "leaf_full_load_raw()");
  for (r=0; r < height-32; r+=32)
    FORC3 for (row=r; row < r+32; row++) {
      read_shorts (pixel, raw_width);
      for (col=0; col < width; col++)
	image[row*width+col][c] = pixel[col];
    }
  free (pixel);
}

void CLASS imacon_full_load_raw()
{
  int row, col;

  for (row=0; row < height; row++)
    for (col=0; col < width; col++)
      read_shorts (image[row*width+col], 3);
}

/* Here raw_width is in bytes, not pixels. */
void CLASS packed_12_load_raw()
{
  int row, col;

  getbits(-1);
  for (row=0; row < height; row++) {
    for (col=0; col < width; col++)
      BAYER(row,col) = getbits(12);
    for (col = width*3/2; col < raw_width; col++)
      getbits(8);
  }
}

void CLASS unpacked_load_raw()
{
  ushort *pixel;
  int row, col;

  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "unpacked_load_raw()");
  for (row=0; row < height; row++) {
    read_shorts (pixel, raw_width);
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col];
  }
  free (pixel);
}

void CLASS olympus_e300_load_raw()
{
  uchar  *data,  *dp;
  ushort *pixel, *pix;
  int dwide, row, col, bls=width, ble=raw_width;

  if (raw_width == 3360) bls += 4;
  if (raw_width == 3280) ble = bls + 8;
  dwide = raw_width * 16 / 10;
  data = malloc (dwide + raw_width*2);
  merror (data, "olympus_e300_load_raw()");
  pixel = (ushort *) (data + dwide);
  for (row=0; row < height; row++) {
    fread (data, 1, dwide, ifp);
    for (dp=data, pix=pixel; pix < pixel+raw_width; dp+=3, pix+=2) {
      if (((dp-data) & 15) == 15) dp++;
      pix[0] = dp[1] << 8 | dp[0];
      pix[1] = dp[2] << 4 | dp[1] >> 4;
    }
    for (col=0; col < width; col++)
      BAYER(row,col) = (pixel[col] & 0xfff);
    for (col=bls; col < ble; col++)
      black += pixel[col] & 0xfff;
  }
  if (ble > bls) black /= (ble - bls) * height;
  free (data);
}

void CLASS olympus_cseries_load_raw()
{
  int irow, row, col;

  for (irow=0; irow < height; irow++) {
    row = irow * 2 % height + irow / (height/2);
    if (row < 2) {
      fseek (ifp, data_offset - row*(-width*height*3/4 & -2048), SEEK_SET);
      getbits(-1);
    }
    for (col=0; col < width; col++)
      BAYER(row,col) = getbits(12);
  }
}

void CLASS minolta_rd175_load_raw()
{
  uchar pixel[768];
  unsigned irow, box, row, col;

  for (irow=0; irow < 1481; irow++) {
    fread (pixel, 1, 768, ifp);
    box = irow / 82;
    row = irow % 82 * 12 + ((box < 12) ? box | 1 : (box-12)*2);
    switch (irow) {
      case 1477: case 1479: continue;
      case 1476: row = 984; break;
      case 1480: row = 985; break;
      case 1478: row = 985; box = 1;
    }
    if ((box < 12) && (box & 1)) {
      for (col=0; col < 1533; col++, row ^= 1)
	if (col != 1) BAYER(row,col) = (col+1) & 2 ?
		   pixel[col/2-1] + pixel[col/2+1] : pixel[col/2] << 1;
      BAYER(row,1)    = pixel[1]   << 1;
      BAYER(row,1533) = pixel[765] << 1;
    } else
      for (col=row & 1; col < 1534; col+=2)
	BAYER(row,col) = pixel[col/2] << 1;
  }
  maximum = 0xff << 1;
}

void CLASS eight_bit_load_raw()
{
  uchar *pixel;
  int row, col;

  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "eight_bit_load_raw()");
  for (row=0; row < height; row++) {
    fread (pixel, 1, raw_width, ifp);
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col];
  }
  free (pixel);
  maximum = 0xff;
}

void CLASS casio_qv5700_load_raw()
{
  uchar  data[3232],  *dp;
  ushort pixel[2576], *pix;
  int row, col;

  for (row=0; row < height; row++) {
    fread (data, 1, 3232, ifp);
    for (dp=data, pix=pixel; dp < data+3220; dp+=5, pix+=4) {
      pix[0] = (dp[0] << 2) + (dp[1] >> 6);
      pix[1] = (dp[1] << 4) + (dp[2] >> 4);
      pix[2] = (dp[2] << 6) + (dp[3] >> 2);
      pix[3] = (dp[3] << 8) + (dp[4]     );
    }
    for (col=0; col < width; col++)
      BAYER(row,col) = (pixel[col] & 0x3ff);
  }
  maximum = 0x3fc;
}

void CLASS nucore_load_raw()
{
  ushort *pixel;
  int irow, row, col;

  pixel = calloc (width, 2);
  merror (pixel, "nucore_load_raw()");
  for (irow=0; irow < height; irow++) {
    read_shorts (pixel, width);
    row = irow/2 + height/2 * (irow & 1);
    for (col=0; col < width; col++)
      BAYER(row,col) = pixel[col];
  }
  free (pixel);
}

const int * CLASS make_decoder_int (const int *source, int level)
{
  struct decode *cur;

  cur = free_decode++;
  if (level < source[0]) {
    cur->branch[0] = free_decode;
    source = make_decoder_int (source, level+1);
    cur->branch[1] = free_decode;
    source = make_decoder_int (source, level+1);
  } else {
    cur->leaf = source[1];
    source += 2;
  }
  return source;
}

int CLASS radc_token (int tree)
{
  int t;
  static struct decode *dstart[18], *dindex;
  static const int *s, source[] = {
    1,1, 2,3, 3,4, 4,2, 5,7, 6,5, 7,6, 7,8,
    1,0, 2,1, 3,3, 4,4, 5,2, 6,7, 7,6, 8,5, 8,8,
    2,1, 2,3, 3,0, 3,2, 3,4, 4,6, 5,5, 6,7, 6,8,
    2,0, 2,1, 2,3, 3,2, 4,4, 5,6, 6,7, 7,5, 7,8,
    2,1, 2,4, 3,0, 3,2, 3,3, 4,7, 5,5, 6,6, 6,8,
    2,3, 3,1, 3,2, 3,4, 3,5, 3,6, 4,7, 5,0, 5,8,
    2,3, 2,6, 3,0, 3,1, 4,4, 4,5, 4,7, 5,2, 5,8,
    2,4, 2,7, 3,3, 3,6, 4,1, 4,2, 4,5, 5,0, 5,8,
    2,6, 3,1, 3,3, 3,5, 3,7, 3,8, 4,0, 5,2, 5,4,
    2,0, 2,1, 3,2, 3,3, 4,4, 4,5, 5,6, 5,7, 4,8,
    1,0, 2,2, 2,-2,
    1,-3, 1,3,
    2,-17, 2,-5, 2,5, 2,17,
    2,-7, 2,2, 2,9, 2,18,
    2,-18, 2,-9, 2,-2, 2,7,
    2,-28, 2,28, 3,-49, 3,-9, 3,9, 4,49, 5,-79, 5,79,
    2,-1, 2,13, 2,26, 3,39, 4,-16, 5,55, 6,-37, 6,76,
    2,-26, 2,-13, 2,1, 3,-39, 4,16, 5,-55, 6,-76, 6,37
  };

  if (free_decode == first_decode)
    for (s=source, t=0; t < 18; t++) {
      dstart[t] = free_decode;
      s = make_decoder_int (s, 0);
    }
  if (tree == 18) {
    if (kodak_cbpp == 243)
      return (getbits(6) << 2) + 2;	/* most DC50 photos */
    else
      return (getbits(5) << 3) + 4;	/* DC40, Fotoman Pixtura */
  }
  for (dindex = dstart[tree]; dindex->branch[0]; )
    dindex = dindex->branch[getbits(1)];
  return dindex->leaf;
}

#define FORYX for (y=1; y < 3; y++) for (x=col+1; x >= col; x--)

#define PREDICTOR (c ? (buf[c][y-1][x] + buf[c][y][x+1]) / 2 \
: (buf[c][y-1][x+1] + 2*buf[c][y-1][x] + buf[c][y][x+1]) / 4)

void CLASS kodak_radc_load_raw()
{
  int row, col, tree, nreps, rep, step, i, c, s, r, x, y, val;
  short last[3] = { 16,16,16 }, mul[3], buf[3][3][386];

  init_decoder();
  getbits(-1);
  for (i=0; i < sizeof(buf)/sizeof(short); i++)
    buf[0][0][i] = 2048;
  for (row=0; row < height; row+=4) {
    FORC3 mul[c] = getbits(6);
    FORC3 {
      val = ((0x1000000/last[c] + 0x7ff) >> 12) * mul[c];
      s = val > 65564 ? 10:12;
      x = ~(-1 << (s-1));
      val <<= 12-s;
      for (i=0; i < sizeof(buf[0])/sizeof(short); i++)
	buf[c][0][i] = (buf[c][0][i] * val + x) >> s;
      last[c] = mul[c];
      for (r=0; r <= !c; r++) {
	buf[c][1][width/2] = buf[c][2][width/2] = mul[c] << 7;
	for (tree=1, col=width/2; col > 0; ) {
	  if ((tree = radc_token(tree))) {
	    col -= 2;
	    if (tree == 8)
	      FORYX buf[c][y][x] = radc_token(tree+10) * mul[c];
	    else
	      FORYX buf[c][y][x] = radc_token(tree+10) * 16 + PREDICTOR;
	  } else
	    do {
	      nreps = (col > 2) ? radc_token(9) + 1 : 1;
	      for (rep=0; rep < 8 && rep < nreps && col > 0; rep++) {
		col -= 2;
		FORYX buf[c][y][x] = PREDICTOR;
		if (rep & 1) {
		  step = radc_token(10) << 4;
		  FORYX buf[c][y][x] += step;
		}
	      }
	    } while (nreps == 9);
	}
	for (y=0; y < 2; y++)
	  for (x=0; x < width/2; x++) {
	    val = (buf[c][y+1][x] << 4) / mul[c];
	    if (val < 0) val = 0;
	    if (c)
	      BAYER(row+y*2+c-1,x*2+2-c) = val;
	    else
	      BAYER(row+r*2+y,x*2+y) = val;
	  }
	memcpy (buf[c][0]+!c, buf[c][2], sizeof buf[c][0]-2*!c);
      }
    }
    for (y=row; y < row+4; y++)
      for (x=0; x < width; x++)
	if ((x+y) & 1) {
	  val = (BAYER(y,x)-2048)*2 + (BAYER(y,x-1)+BAYER(y,x+1))/2;
	  if (val < 0) val = 0;
	  BAYER(y,x) = val;
	}
  }
  maximum = 10000;
}

#undef FORYX
#undef PREDICTOR

#ifdef NO_JPEG
void CLASS kodak_jpeg_load_raw() {}
#else

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  static uchar jpeg_buffer[4096];
  size_t nbytes;

  nbytes = fread (jpeg_buffer, 1, 4096, ifp);
  swab (jpeg_buffer, jpeg_buffer, nbytes);
  cinfo->src->next_input_byte = jpeg_buffer;
  cinfo->src->bytes_in_buffer = nbytes;
  return TRUE;
}

void CLASS kodak_jpeg_load_raw()
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPARRAY buf;
  JSAMPLE (*pixel)[3];
  int row, col;

  cinfo.err = jpeg_std_error (&jerr);
  jpeg_create_decompress (&cinfo);
  jpeg_stdio_src (&cinfo, ifp);
  cinfo.src->fill_input_buffer = fill_input_buffer;
  jpeg_read_header (&cinfo, TRUE);
  jpeg_start_decompress (&cinfo);
  if ((cinfo.output_width      != width  ) ||
      (cinfo.output_height*2   != height ) ||
      (cinfo.output_components != 3      )) {
    fprintf (stderr, "%s: incorrect JPEG dimensions\n", ifname);
    jpeg_destroy_decompress (&cinfo);
    longjmp (failure, 3);
  }
  buf = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, width*3, 1);

  while (cinfo.output_scanline < cinfo.output_height) {
    row = cinfo.output_scanline * 2;
    jpeg_read_scanlines (&cinfo, buf, 1);
    pixel = (void *) buf[0];
    for (col=0; col < width; col+=2) {
      BAYER(row+0,col+0) = pixel[col+0][1] << 1;
      BAYER(row+1,col+1) = pixel[col+1][1] << 1;
      BAYER(row+0,col+1) = pixel[col][0] + pixel[col+1][0];
      BAYER(row+1,col+0) = pixel[col][2] + pixel[col+1][2];
    }
  }
  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);
  maximum = 0xff << 1;
}
#endif

void CLASS kodak_dc120_load_raw()
{
  static const int mul[4] = { 162, 192, 187,  92 };
  static const int add[4] = {   0, 636, 424, 212 };
  uchar pixel[848];
  int row, shift, col;

  for (row=0; row < height; row++) {
    fread (pixel, 848, 1, ifp);
    shift = row * mul[row & 3] + add[row & 3];
    for (col=0; col < width; col++)
      BAYER(row,col) = (ushort) pixel[(col + shift) % 848];
  }
  maximum = 0xff;
}

void CLASS kodak_easy_load_raw()
{
  uchar *pixel;
  unsigned row, col, icol;

  if (raw_width > width)
    black = 0;
  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "kodak_easy_load_raw()");
  for (row=0; row < height; row++) {
    fread (pixel, 1, raw_width, ifp);
    for (col=0; col < raw_width; col++) {
      icol = col - left_margin;
      if (icol < width)
	BAYER(row,icol) = (ushort) curve[pixel[col]];
      else
	black += curve[pixel[col]];
    }
  }
  free (pixel);
  if (raw_width > width)
    black /= (raw_width - width) * height;
  if (!strncmp(model,"DC2",3))
    black = 0;
  maximum = curve[0xff];
}

int CLASS kodak_65000_decode (short *out, int bsize)
{
  uchar c, blen[768];
  ushort raw[6];
  INT64 bitbuf=0;
  int save, bits=0, i, j, len, diff;

  save = ftell(ifp);
  bsize = (bsize + 3) & -4;
  for (i=0; i < bsize; i+=2) {
    c = fgetc(ifp);
    if ((blen[i  ] = c & 15) > 12 ||
	(blen[i+1] = c >> 4) > 12 ) {
      fseek (ifp, save, SEEK_SET);
      for (i=0; i < bsize; i+=8) {
	read_shorts (raw, 6);
	out[i  ] = raw[0] >> 12 << 8 | raw[2] >> 12 << 4 | raw[4] >> 12;
	out[i+1] = raw[1] >> 12 << 8 | raw[3] >> 12 << 4 | raw[5] >> 12;
	for (j=0; j < 6; j++)
	  out[i+2+j] = raw[j] & 0xfff;
      }
      return 1;
    }
  }
  if ((bsize & 7) == 4) {
    bitbuf  = fgetc(ifp) << 8;
    bitbuf += fgetc(ifp);
    bits = 16;
  }
  for (i=0; i < bsize; i++) {
    len = blen[i];
    if (bits < len) {
      for (j=0; j < 32; j+=8)
	bitbuf += (INT64) fgetc(ifp) << (bits+(j^8));
      bits += 32;
    }
    diff = bitbuf & (0xffff >> (16-len));
    bitbuf >>= len;
    bits -= len;
    if ((diff & (1 << (len-1))) == 0)
      diff -= (1 << len) - 1;
    out[i] = diff;
  }
  return 0;
}

void CLASS kodak_65000_load_raw()
{
  short buf[256];
  int row, col, len, pred[2], ret, i;

  for (row=0; row < height; row++)
    for (col=0; col < width; col+=256) {
      pred[0] = pred[1] = 0;
      len = MIN (256, width-col);
      ret = kodak_65000_decode (buf, len);
      for (i=0; i < len; i++)
	BAYER(row,col+i) = curve[ret ? buf[i] : (pred[i & 1] += buf[i])];
    }
}

void CLASS kodak_ycbcr_load_raw()
{
  short buf[384], *bp;
  int row, col, len, c, i, j, k, y[2][2], cb, cr, rgb[3];
  ushort *ip;

  for (row=0; row < height; row+=2)
    for (col=0; col < width; col+=128) {
      len = MIN (128, width-col);
      kodak_65000_decode (buf, len*3);
      y[0][1] = y[1][1] = cb = cr = 0;
      for (bp=buf, i=0; i < len; i+=2, bp+=2) {
	cb += bp[4];
	cr += bp[5];
	rgb[1] = -((cb + cr + 2) >> 2);
	rgb[2] = rgb[1] + cb;
	rgb[0] = rgb[1] + cr;
	for (j=0; j < 2; j++)
	  for (k=0; k < 2; k++) {
	    y[j][k] = y[j][k^1] + *bp++;
	    ip = image[(row+j)*width + col+i+k];
	    FORC3 ip[c] = curve[LIM(y[j][k]+rgb[c], 0, 0xfff)];
	  }
      }
    }
}

void CLASS kodak_rgb_load_raw()
{
  short buf[768], *bp;
  int row, col, len, c, i, rgb[3];
  ushort *ip=image[0];

  for (row=0; row < height; row++)
    for (col=0; col < width; col+=256) {
      len = MIN (256, width-col);
      kodak_65000_decode (buf, len*3);
      memset (rgb, 0, sizeof rgb);
      for (bp=buf, i=0; i < len; i++, ip+=4)
	FORC3 ip[c] = (rgb[c] += *bp++) & 0xfff;
    }
}

void CLASS kodak_thumb_load_raw()
{
  int row, col;
  colors = thumb_misc >> 5;
  for (row=0; row < height; row++)
    for (col=0; col < width; col++)
      read_shorts (image[row*width+col], colors);
  maximum = (1 << (thumb_misc & 31)) - 1;
}

void CLASS sony_decrypt (unsigned *data, int len, int start, int key)
{
  static unsigned pad[128], p;

  if (start) {
    for (p=0; p < 4; p++)
      pad[p] = key = key * 48828125 + 1;
    pad[3] = pad[3] << 1 | (pad[0]^pad[2]) >> 31;
    for (p=4; p < 127; p++)
      pad[p] = (pad[p-4]^pad[p-2]) << 1 | (pad[p-3]^pad[p-1]) >> 31;
    for (p=0; p < 127; p++)
      pad[p] = htonl(pad[p]);
  }
  while (len--)
    *data++ ^= pad[p++ & 127] = pad[(p+1) & 127] ^ pad[(p+65) & 127];
}

void CLASS sony_load_raw()
{
  uchar head[40];
  ushort *pixel;
  unsigned i, key, row, col;

  fseek (ifp, 200896, SEEK_SET);
  fseek (ifp, (unsigned) fgetc(ifp)*4 - 1, SEEK_CUR);
  order = 0x4d4d;
  key = get4();
  fseek (ifp, 164600, SEEK_SET);
  fread (head, 1, 40, ifp);
  sony_decrypt ((void *) head, 10, 1, key);
  for (i=26; i-- > 22; )
    key = key << 8 | head[i];
  fseek (ifp, data_offset, SEEK_SET);
  pixel = calloc (raw_width, sizeof *pixel);
  merror (pixel, "sony_load_raw()");
  for (row=0; row < height; row++) {
    fread (pixel, 2, raw_width, ifp);
    sony_decrypt ((void *) pixel, raw_width/2, !row, key);
    for (col=9; col < left_margin; col++)
      black += ntohs(pixel[col]);
    for (col=0; col < width; col++)
      BAYER(row,col) = ntohs(pixel[col+left_margin]);
  }
  free (pixel);
  if (left_margin > 9)
    black /= (left_margin-9) * height;
  maximum = 0x3ff0;
}

#define HOLE(row) ((holes >> (((row) - raw_height) & 7)) & 1)

/* Kudos to Rich Taylor for figuring out SMaL's compression algorithm. */
void CLASS smal_decode_segment (unsigned seg[2][2], int holes)
{
  uchar hist[3][13] = {
    { 7, 7, 0, 0, 63, 55, 47, 39, 31, 23, 15, 7, 0 },
    { 7, 7, 0, 0, 63, 55, 47, 39, 31, 23, 15, 7, 0 },
    { 3, 3, 0, 0, 63,     47,     31,     15,    0 } };
  int low, high=0xff, carry=0, nbits=8;
  int s, count, bin, next, i, sym[3];
  uchar diff, pred[]={0,0};
  ushort data=0, range=0;
  unsigned pix, row, col;

  fseek (ifp, seg[0][1]+1, SEEK_SET);
  getbits(-1);
  for (pix=seg[0][0]; pix < seg[1][0]; pix++) {
    for (s=0; s < 3; s++) {
      data = data << nbits | getbits(nbits);
      if (carry < 0)
	carry = (nbits += carry+1) < 1 ? nbits-1 : 0;
      while (--nbits >= 0)
	if ((data >> nbits & 0xff) == 0xff) break;
      if (nbits > 0)
	  data = ((data & ((1 << (nbits-1)) - 1)) << 1) |
	((data + (((data & (1 << (nbits-1)))) << 1)) & (-1 << nbits));
      if (nbits >= 0) {
	data += getbits(1);
	carry = nbits - 8;
      }
      count = ((((data-range+1) & 0xffff) << 2) - 1) / (high >> 4);
      for (bin=0; hist[s][bin+5] > count; bin++);
		low = hist[s][bin+5] * (high >> 4) >> 2;
      if (bin) high = hist[s][bin+4] * (high >> 4) >> 2;
      high -= low;
      for (nbits=0; high << nbits < 128; nbits++);
      range = (range+low) << nbits;
      high <<= nbits;
      next = hist[s][1];
      if (++hist[s][2] > hist[s][3]) {
	next = (next+1) & hist[s][0];
	hist[s][3] = (hist[s][next+4] - hist[s][next+5]) >> 2;
	hist[s][2] = 1;
      }
      if (hist[s][hist[s][1]+4] - hist[s][hist[s][1]+5] > 1) {
	if (bin < hist[s][1])
	  for (i=bin; i < hist[s][1]; i++) hist[s][i+5]--;
	else if (next <= bin)
	  for (i=hist[s][1]; i < bin; i++) hist[s][i+5]++;
      }
      hist[s][1] = next;
      sym[s] = bin;
    }
    diff = sym[2] << 5 | sym[1] << 2 | (sym[0] & 3);
    if (sym[0] & 4)
      diff = diff ? -diff : 0x80;
    if (ftell(ifp) + 12 >= seg[1][1])
      diff = 0;
    pred[pix & 1] += diff;
    row = pix / raw_width - top_margin;
    col = pix % raw_width - left_margin;
    if (row < height && col < width)
      BAYER(row,col) = pred[pix & 1];
    if (!(pix & 1) && HOLE(row)) pix += 2;
  }
  maximum = 0xff;
}

void CLASS smal_v6_load_raw()
{
  unsigned seg[2][2];

  fseek (ifp, 16, SEEK_SET);
  seg[0][0] = 0;
  seg[0][1] = get2();
  seg[1][0] = raw_width * raw_height;
  seg[1][1] = INT_MAX;
  smal_decode_segment (seg, 0);
  use_gamma = 0;
}

int CLASS median4 (int *p)
{
  int min, max, sum, i;

  min = max = sum = p[0];
  for (i=1; i < 4; i++) {
    sum += p[i];
    if (min > p[i]) min = p[i];
    if (max < p[i]) max = p[i];
  }
  return (sum - min - max) >> 1;
}

void CLASS fill_holes (int holes)
{
  int row, col, val[4];

  for (row=2; row < height-2; row++) {
    if (!HOLE(row)) continue;
    for (col=1; col < width-1; col+=4) {
      val[0] = BAYER(row-1,col-1);
      val[1] = BAYER(row-1,col+1);
      val[2] = BAYER(row+1,col-1);
      val[3] = BAYER(row+1,col+1);
      BAYER(row,col) = median4(val);
    }
    for (col=2; col < width-2; col+=4)
      if (HOLE(row-2) || HOLE(row+2))
	BAYER(row,col) = (BAYER(row,col-2) + BAYER(row,col+2)) >> 1;
      else {
	val[0] = BAYER(row,col-2);
	val[1] = BAYER(row,col+2);
	val[2] = BAYER(row-2,col);
	val[3] = BAYER(row+2,col);
	BAYER(row,col) = median4(val);
      }
  }
}

void CLASS smal_v9_load_raw()
{
  unsigned seg[256][2], offset, nseg, holes, i;

  fseek (ifp, 67, SEEK_SET);
  offset = get4();
  nseg = fgetc(ifp);
  fseek (ifp, offset, SEEK_SET);
  for (i=0; i < nseg*2; i++)
    seg[0][i] = get4() + data_offset*(i & 1);
  fseek (ifp, 78, SEEK_SET);
  holes = fgetc(ifp);
  fseek (ifp, 88, SEEK_SET);
  seg[nseg][0] = raw_height * raw_width;
  seg[nseg][1] = get4() + data_offset;
  for (i=0; i < nseg; i++)
    smal_decode_segment (seg+i, holes);
  if (holes) fill_holes (holes);
}

/* BEGIN GPL BLOCK */

void CLASS foveon_decoder (unsigned size, unsigned code)
{
  static unsigned huff[1024];
  struct decode *cur;
  int i, len;

  if (!code) {
    for (i=0; i < size; i++)
      huff[i] = get4();
    init_decoder();
  }
  cur = free_decode++;
  if (free_decode > first_decode+2048) {
    fprintf (stderr, "%s: decoder table overflow\n", ifname);
    longjmp (failure, 2);
  }
  if (code)
    for (i=0; i < size; i++)
      if (huff[i] == code) {
	cur->leaf = i;
	return;
      }
  if ((len = code >> 27) > 26) return;
  code = (len+1) << 27 | (code & 0x3ffffff) << 1;

  cur->branch[0] = free_decode;
  foveon_decoder (size, code);
  cur->branch[1] = free_decode;
  foveon_decoder (size, code+1);
}

void CLASS foveon_thumb (FILE *tfp)
{
  int bwide, row, col, bit=-1, c, i;
  char *buf;
  struct decode *dindex;
  short pred[3];
  unsigned bitbuf=0;

  bwide = get4();
  fprintf (tfp, "P6\n%d %d\n255\n", thumb_width, thumb_height);
  if (bwide > 0) {
    buf = malloc (bwide);
    merror (buf, "foveon_thumb()");
    for (row=0; row < thumb_height; row++) {
      fread  (buf, 1, bwide, ifp);
      fwrite (buf, 3, thumb_width, tfp);
    }
    free (buf);
    return;
  }
  foveon_decoder (256, 0);

  for (row=0; row < thumb_height; row++) {
    memset (pred, 0, sizeof pred);
    if (!bit) get4();
    for (col=bit=0; col < thumb_width; col++)
      FORC3 {
	for (dindex=first_decode; dindex->branch[0]; ) {
	  if ((bit = (bit-1) & 31) == 31)
	    for (i=0; i < 4; i++)
	      bitbuf = (bitbuf << 8) + fgetc(ifp);
	  dindex = dindex->branch[bitbuf >> bit & 1];
	}
	pred[c] += dindex->leaf;
	fputc (pred[c], tfp);
      }
  }
}

void CLASS foveon_load_camf()
{
  unsigned key, i, val;

  fseek (ifp, meta_offset, SEEK_SET);
  key = get4();
  fread (meta_data, 1, meta_length, ifp);
  for (i=0; i < meta_length; i++) {
    key = (key * 1597 + 51749) % 244944;
    val = key * (INT64) 301593171 >> 24;
    meta_data[i] ^= ((((key << 8) - val) >> 1) + val) >> 17;
  }
}

void CLASS foveon_load_raw()
{
  struct decode *dindex;
  short diff[1024], pred[3];
  unsigned bitbuf=0;
  int fixed, row, col, bit=-1, c, i;

  fixed = get4();
  read_shorts ((ushort *) diff, 1024);
  if (!fixed) foveon_decoder (1024, 0);

  for (row=0; row < height; row++) {
    memset (pred, 0, sizeof pred);
    if (!bit && !fixed) get4();
    for (col=bit=0; col < width; col++) {
      if (fixed) {
	bitbuf = get4();
	FORC3 pred[2-c] += diff[bitbuf >> c*10 & 0x3ff];
      }
      else FORC3 {
	for (dindex=first_decode; dindex->branch[0]; ) {
	  if ((bit = (bit-1) & 31) == 31)
	    for (i=0; i < 4; i++)
	      bitbuf = (bitbuf << 8) + fgetc(ifp);
	  dindex = dindex->branch[bitbuf >> bit & 1];
	}
	pred[c] += diff[dindex->leaf];
      }
      FORC3 image[row*width+col][c] = pred[c];
    }
  }
  if (document_mode)
    for (i=0; i < height*width*4; i++)
      if ((short) image[0][i] < 0) image[0][i] = 0;
  foveon_load_camf();
}

char * CLASS foveon_camf_param (char *block, char *param)
{
  unsigned idx, num;
  char *pos, *cp, *dp;

  for (idx=0; idx < meta_length; idx += sget4(pos+8)) {
    pos = meta_data + idx;
    if (strncmp (pos, "CMb", 3)) break;
    if (pos[3] != 'P') continue;
    if (strcmp (block, pos+sget4(pos+12))) continue;
    cp = pos + sget4(pos+16);
    num = sget4(cp);
    dp = pos + sget4(cp+4);
    while (num--) {
      cp += 8;
      if (!strcmp (param, dp+sget4(cp)))
	return dp+sget4(cp+4);
    }
  }
  return NULL;
}

void * CLASS foveon_camf_matrix (int dim[3], char *name)
{
  unsigned i, idx, type, ndim, size, *mat;
  char *pos, *cp, *dp;

  for (idx=0; idx < meta_length; idx += sget4(pos+8)) {
    pos = meta_data + idx;
    if (strncmp (pos, "CMb", 3)) break;
    if (pos[3] != 'M') continue;
    if (strcmp (name, pos+sget4(pos+12))) continue;
    dim[0] = dim[1] = dim[2] = 1;
    cp = pos + sget4(pos+16);
    type = sget4(cp);
    if ((ndim = sget4(cp+4)) > 3) break;
    dp = pos + sget4(cp+8);
    for (i=ndim; i--; ) {
      cp += 12;
      dim[i] = sget4(cp);
    }
    if ((size = dim[0]*dim[1]*dim[2]) > meta_length/4) break;
    mat = malloc (size * 4);
    merror (mat, "foveon_camf_matrix()");
    for (i=0; i < size; i++)
      if (type && type != 6)
	mat[i] = sget4(dp + i*4);
      else
	mat[i] = sget4(dp + i*2) & 0xffff;
    return mat;
  }
  fprintf (stderr, "%s: \"%s\" matrix not found!\n", ifname, name);
  return NULL;
}

int CLASS foveon_fixed (void *ptr, int size, char *name)
{
  void *dp;
  int dim[3];

  dp = foveon_camf_matrix (dim, name);
  if (!dp) return 0;
  memcpy (ptr, dp, size*4);
  free (dp);
  return 1;
}

float CLASS foveon_avg (short *pix, int range[2], float cfilt)
{
  int i;
  float val, min=FLT_MAX, max=-FLT_MAX, sum=0;

  for (i=range[0]; i <= range[1]; i++) {
    sum += val = pix[i*4] + (pix[i*4]-pix[(i-1)*4]) * cfilt;
    if (min > val) min = val;
    if (max < val) max = val;
  }
  return (sum - min - max) / (range[1] - range[0] - 1);
}

short * CLASS foveon_make_curve (double max, double mul, double filt)
{
  short *curve;
  int i, size;
  double x;

  if (!filt) filt = 0.8;
  size = 4*M_PI*max / filt;
  curve = calloc (size+1, sizeof *curve);
  merror (curve, "foveon_make_curve()");
  curve[0] = size;
  for (i=0; i < size; i++) {
    x = i*filt/max/4;
    curve[i+1] = (cos(x)+1)/2 * tanh(i*filt/mul) * mul + 0.5;
  }
  return curve;
}

void CLASS foveon_make_curves
	(short **curvep, float dq[3], float div[3], float filt)
{
  double mul[3], max=0;
  int c;

  FORC3 mul[c] = dq[c]/div[c];
  FORC3 if (max < mul[c]) max = mul[c];
  FORC3 curvep[c] = foveon_make_curve (max, mul[c], filt);
}

int CLASS foveon_apply_curve (short *curve, int i)
{
  if (abs(i) >= curve[0]) return 0;
  return i < 0 ? -curve[1-i] : curve[1+i];
}

#define image ((short (*)[4]) image)

void CLASS foveon_interpolate()
{
  static const short hood[] = { -1,-1, -1,0, -1,1, 0,-1, 0,1, 1,-1, 1,0, 1,1 };
  short *pix, prev[3], *curve[8], (*shrink)[3];
  float cfilt=0, ddft[3][3][2], ppm[3][3][3];
  float cam_xyz[3][3], correct[3][3], last[3][3], trans[3][3];
  float chroma_dq[3], color_dq[3], diag[3][3], div[3];
  float (*black)[3], (*sgain)[3], (*sgrow)[3];
  float fsum[3], val, frow, num;
  int row, col, c, i, j, diff, sgx, irow, sum, min, max, limit;
  int dim[3], dscr[2][2], dstb[4], (*smrow[7])[3], total[4], ipix[3];
  int work[3][3], smlast, smred, smred_p=0, dev[3];
  int satlev[3], keep[4], active[4];
  unsigned *badpix;
  double dsum=0, trsum[3];
  char str[128], *cp;

  if (verbose)
    fprintf (stderr, "Foveon interpolation...\n");

  foveon_fixed (dscr, 4, "DarkShieldColRange");
  foveon_fixed (ppm[0][0], 27, "PostPolyMatrix");
  foveon_fixed (satlev, 3, "SaturationLevel");
  foveon_fixed (keep, 4, "KeepImageArea");
  foveon_fixed (active, 4, "ActiveImageArea");
  foveon_fixed (chroma_dq, 3, "ChromaDQ");
  foveon_fixed (color_dq, 3,
	foveon_camf_param ("IncludeBlocks", "ColorDQ") ?
		"ColorDQ" : "ColorDQCamRGB");
  if (foveon_camf_param ("IncludeBlocks", "ColumnFilter"))
  		 foveon_fixed (&cfilt, 1, "ColumnFilter");

  memset (ddft, 0, sizeof ddft);
  if (!foveon_camf_param ("IncludeBlocks", "DarkDrift")
	 || !foveon_fixed (ddft[1][0], 12, "DarkDrift"))
    for (i=0; i < 2; i++) {
      foveon_fixed (dstb, 4, i ? "DarkShieldBottom":"DarkShieldTop");
      for (row = dstb[1]; row <= dstb[3]; row++)
	for (col = dstb[0]; col <= dstb[2]; col++)
	  FORC3 ddft[i+1][c][1] += (short) image[row*width+col][c];
      FORC3 ddft[i+1][c][1] /= (dstb[3]-dstb[1]+1) * (dstb[2]-dstb[0]+1);
    }

  if (!(cp = foveon_camf_param ("WhiteBalanceIlluminants", model2)))
  { fprintf (stderr, "%s: Invalid white balance \"%s\"\n", ifname, model2);
    return; }
  foveon_fixed (cam_xyz, 9, cp);
  foveon_fixed (correct, 9,
	foveon_camf_param ("WhiteBalanceCorrections", model2));
  memset (last, 0, sizeof last);
  for (i=0; i < 3; i++)
    for (j=0; j < 3; j++)
      FORC3 last[i][j] += correct[i][c] * cam_xyz[c][j];

  sprintf (str, "%sRGBNeutral", model2);
  if (foveon_camf_param ("IncludeBlocks", str))
    foveon_fixed (div, 3, str);
  else {
    #define LAST(x,y) last[(i+x)%3][(c+y)%3]
    for (i=0; i < 3; i++)
      FORC3 diag[c][i] = LAST(1,1)*LAST(2,2) - LAST(1,2)*LAST(2,1);
    #undef LAST
    FORC3 div[c] = diag[c][0]*0.3127 + diag[c][1]*0.329 + diag[c][2]*0.3583;
  }
  num = 0;
  FORC3 if (num < div[c]) num = div[c];
  FORC3 div[c] /= num;

  memset (trans, 0, sizeof trans);
  for (i=0; i < 3; i++)
    for (j=0; j < 3; j++)
      FORC3 trans[i][j] += rgb_cam[i][c] * last[c][j] * div[j];
  FORC3 trsum[c] = trans[c][0] + trans[c][1] + trans[c][2];
  dsum = (6*trsum[0] + 11*trsum[1] + 3*trsum[2]) / 20;
  for (i=0; i < 3; i++)
    FORC3 last[i][c] = trans[i][c] * dsum / trsum[i];
  memset (trans, 0, sizeof trans);
  for (i=0; i < 3; i++)
    for (j=0; j < 3; j++)
      FORC3 trans[i][j] += (i==c ? 32 : -1) * last[c][j] / 30;

  foveon_make_curves (curve, color_dq, div, cfilt);
  FORC3 chroma_dq[c] /= 3;
  foveon_make_curves (curve+3, chroma_dq, div, cfilt);
  FORC3 dsum += chroma_dq[c] / div[c];
  curve[6] = foveon_make_curve (dsum, dsum, cfilt);
  curve[7] = foveon_make_curve (dsum*2, dsum*2, cfilt);

  sgain = foveon_camf_matrix (dim, "SpatialGain");
  if (!sgain) return;
  sgrow = calloc (dim[1], sizeof *sgrow);
  sgx = (width + dim[1]-2) / (dim[1]-1);

  black = calloc (height, sizeof *black);
  for (row=0; row < height; row++) {
    for (i=0; i < 6; i++)
      ddft[0][0][i] = ddft[1][0][i] +
	row / (height-1.0) * (ddft[2][0][i] - ddft[1][0][i]);
    FORC3 black[row][c] =
 	( foveon_avg (image[row*width]+c, dscr[0], cfilt) +
	  foveon_avg (image[row*width]+c, dscr[1], cfilt) * 3
	  - ddft[0][c][0] ) / 4 - ddft[0][c][1];
  }
  memcpy (black, black+8, sizeof *black*8);
  memcpy (black+height-11, black+height-22, 11*sizeof *black);
  memcpy (last, black, sizeof last);

  for (row=1; row < height-1; row++) {
    FORC3 if (last[1][c] > last[0][c]) {
	if (last[1][c] > last[2][c])
	  black[row][c] = (last[0][c] > last[2][c]) ? last[0][c]:last[2][c];
      } else
	if (last[1][c] < last[2][c])
	  black[row][c] = (last[0][c] < last[2][c]) ? last[0][c]:last[2][c];
    memmove (last, last+1, 2*sizeof last[0]);
    memcpy (last[2], black[row+1], sizeof last[2]);
  }
  FORC3 black[row][c] = (last[0][c] + last[1][c])/2;
  FORC3 black[0][c] = (black[1][c] + black[3][c])/2;

  val = 1 - exp(-1/24.0);
  memcpy (fsum, black, sizeof fsum);
  for (row=1; row < height; row++)
    FORC3 fsum[c] += black[row][c] =
	(black[row][c] - black[row-1][c])*val + black[row-1][c];
  memcpy (last[0], black[height-1], sizeof last[0]);
  FORC3 fsum[c] /= height;
  for (row = height; row--; )
    FORC3 last[0][c] = black[row][c] =
	(black[row][c] - fsum[c] - last[0][c])*val + last[0][c];

  memset (total, 0, sizeof total);
  for (row=2; row < height; row+=4)
    for (col=2; col < width; col+=4) {
      FORC3 total[c] += (short) image[row*width+col][c];
      total[3]++;
    }
  for (row=0; row < height; row++)
    FORC3 black[row][c] += fsum[c]/2 + total[c]/(total[3]*100.0);

  for (row=0; row < height; row++) {
    for (i=0; i < 6; i++)
      ddft[0][0][i] = ddft[1][0][i] +
	row / (height-1.0) * (ddft[2][0][i] - ddft[1][0][i]);
    pix = image[row*width];
    memcpy (prev, pix, sizeof prev);
    frow = row / (height-1.0) * (dim[2]-1);
    if ((irow = frow) == dim[2]-1) irow--;
    frow -= irow;
    for (i=0; i < dim[1]; i++)
      FORC3 sgrow[i][c] = sgain[ irow   *dim[1]+i][c] * (1-frow) +
			  sgain[(irow+1)*dim[1]+i][c] *    frow;
    for (col=0; col < width; col++) {
      FORC3 {
	diff = pix[c] - prev[c];
	prev[c] = pix[c];
	ipix[c] = pix[c] + floor ((diff + (diff*diff >> 14)) * cfilt
		- ddft[0][c][1] - ddft[0][c][0] * ((float) col/width - 0.5)
		- black[row][c] );
      }
      FORC3 {
	work[0][c] = ipix[c] * ipix[c] >> 14;
	work[2][c] = ipix[c] * work[0][c] >> 14;
	work[1][2-c] = ipix[(c+1) % 3] * ipix[(c+2) % 3] >> 14;
      }
      FORC3 {
	for (val=i=0; i < 3; i++)
	  for (  j=0; j < 3; j++)
	    val += ppm[c][i][j] * work[i][j];
	ipix[c] = floor ((ipix[c] + floor(val)) *
		( sgrow[col/sgx  ][c] * (sgx - col%sgx) +
		  sgrow[col/sgx+1][c] * (col%sgx) ) / sgx / div[c]);
	if (ipix[c] > 32000) ipix[c] = 32000;
	pix[c] = ipix[c];
      }
      pix += 4;
    }
  }
  free (black);
  free (sgrow);
  free (sgain);

  if ((badpix = foveon_camf_matrix (dim, "BadPixels"))) {
    for (i=0; i < dim[0]; i++) {
      col = (badpix[i] >> 8 & 0xfff) - keep[0];
      row = (badpix[i] >> 20       ) - keep[1];
      if ((unsigned)(row-1) > height-3 || (unsigned)(col-1) > width-3)
	continue;
      memset (fsum, 0, sizeof fsum);
      for (sum=j=0; j < 8; j++)
	if (badpix[i] & (1 << j)) {
	  FORC3 fsum[c] += (short)
		image[(row+hood[j*2])*width+col+hood[j*2+1]][c];
	  sum++;
	}
      if (sum) FORC3 image[row*width+col][c] = fsum[c]/sum;
    }
    free (badpix);
  }

  /* Array for 5x5 Gaussian averaging of red values */
  smrow[6] = calloc (width*5, sizeof **smrow);
  merror (smrow[6], "foveon_interpolate()");
  for (i=0; i < 5; i++)
    smrow[i] = smrow[6] + i*width;

  /* Sharpen the reds against these Gaussian averages */
  for (smlast=-1, row=2; row < height-2; row++) {
    while (smlast < row+2) {
      for (i=0; i < 6; i++)
	smrow[(i+5) % 6] = smrow[i];
      pix = image[++smlast*width+2];
      for (col=2; col < width-2; col++) {
	smrow[4][col][0] =
	  (pix[0]*6 + (pix[-4]+pix[4])*4 + pix[-8]+pix[8] + 8) >> 4;
	pix += 4;
      }
    }
    pix = image[row*width+2];
    for (col=2; col < width-2; col++) {
      smred = ( 6 *  smrow[2][col][0]
	      + 4 * (smrow[1][col][0] + smrow[3][col][0])
	      +      smrow[0][col][0] + smrow[4][col][0] + 8 ) >> 4;
      if (col == 2)
	smred_p = smred;
      i = pix[0] + ((pix[0] - ((smred*7 + smred_p) >> 3)) >> 3);
      if (i > 32000) i = 32000;
      pix[0] = i;
      smred_p = smred;
      pix += 4;
    }
  }

  /* Adjust the brighter pixels for better linearity */
  min = 0xffff;
  FORC3 {
    i = satlev[c] / div[c];
    if (min > i) min = i;
  }
  limit = min * 9 >> 4;
  for (pix=image[0]; pix < image[height*width]; pix+=4) {
    if (pix[0] <= limit || pix[1] <= limit || pix[2] <= limit)
      continue;
    min = max = pix[0];
    for (c=1; c < 3; c++) {
      if (min > pix[c]) min = pix[c];
      if (max < pix[c]) max = pix[c];
    }
    if (min >= limit*2) {
      pix[0] = pix[1] = pix[2] = max;
    } else {
      i = 0x4000 - ((min - limit) << 14) / limit;
      i = 0x4000 - (i*i >> 14);
      i = i*i >> 14;
      FORC3 pix[c] += (max - pix[c]) * i >> 14;
    }
  }
/*
   Because photons that miss one detector often hit another,
   the sum R+G+B is much less noisy than the individual colors.
   So smooth the hues without smoothing the total.
 */
  for (smlast=-1, row=2; row < height-2; row++) {
    while (smlast < row+2) {
      for (i=0; i < 6; i++)
	smrow[(i+5) % 6] = smrow[i];
      pix = image[++smlast*width+2];
      for (col=2; col < width-2; col++) {
	FORC3 smrow[4][col][c] = (pix[c-4]+2*pix[c]+pix[c+4]+2) >> 2;
	pix += 4;
      }
    }
    pix = image[row*width+2];
    for (col=2; col < width-2; col++) {
      FORC3 dev[c] = -foveon_apply_curve (curve[7], pix[c] -
	((smrow[1][col][c] + 2*smrow[2][col][c] + smrow[3][col][c]) >> 2));
      sum = (dev[0] + dev[1] + dev[2]) >> 3;
      FORC3 pix[c] += dev[c] - sum;
      pix += 4;
    }
  }
  for (smlast=-1, row=2; row < height-2; row++) {
    while (smlast < row+2) {
      for (i=0; i < 6; i++)
	smrow[(i+5) % 6] = smrow[i];
      pix = image[++smlast*width+2];
      for (col=2; col < width-2; col++) {
	FORC3 smrow[4][col][c] =
		(pix[c-8]+pix[c-4]+pix[c]+pix[c+4]+pix[c+8]+2) >> 2;
	pix += 4;
      }
    }
    pix = image[row*width+2];
    for (col=2; col < width-2; col++) {
      for (total[3]=375, sum=60, c=0; c < 3; c++) {
	for (total[c]=i=0; i < 5; i++)
	  total[c] += smrow[i][col][c];
	total[3] += total[c];
	sum += pix[c];
      }
      if (sum < 0) sum = 0;
      j = total[3] > 375 ? (sum << 16) / total[3] : sum * 174;
      FORC3 pix[c] += foveon_apply_curve (curve[6],
		((j*total[c] + 0x8000) >> 16) - pix[c]);
      pix += 4;
    }
  }

  /* Transform the image to a different colorspace */
  for (pix=image[0]; pix < image[height*width]; pix+=4) {
    FORC3 pix[c] -= foveon_apply_curve (curve[c], pix[c]);
    sum = (pix[0]+pix[1]+pix[1]+pix[2]) >> 2;
    FORC3 pix[c] -= foveon_apply_curve (curve[c], pix[c]-sum);
    FORC3 {
      for (dsum=i=0; i < 3; i++)
	dsum += trans[c][i] * pix[i];
      if (dsum < 0)  dsum = 0;
      if (dsum > 24000) dsum = 24000;
      ipix[c] = dsum + 0.5;
    }
    FORC3 pix[c] = ipix[c];
  }

  /* Smooth the image bottom-to-top and save at 1/4 scale */
  shrink = calloc ((width/4) * (height/4), sizeof *shrink);
  merror (shrink, "foveon_interpolate()");
  for (row = height/4; row--; )
    for (col=0; col < width/4; col++) {
      ipix[0] = ipix[1] = ipix[2] = 0;
      for (i=0; i < 4; i++)
	for (j=0; j < 4; j++)
	  FORC3 ipix[c] += image[(row*4+i)*width+col*4+j][c];
      FORC3
	if (row+2 > height/4)
	  shrink[row*(width/4)+col][c] = ipix[c] >> 4;
	else
	  shrink[row*(width/4)+col][c] =
	    (shrink[(row+1)*(width/4)+col][c]*1840 + ipix[c]*141 + 2048) >> 12;
    }
  /* From the 1/4-scale image, smooth right-to-left */
  for (row=0; row < (height & ~3); row++) {
    ipix[0] = ipix[1] = ipix[2] = 0;
    if ((row & 3) == 0)
      for (col = width & ~3 ; col--; )
	FORC3 smrow[0][col][c] = ipix[c] =
	  (shrink[(row/4)*(width/4)+col/4][c]*1485 + ipix[c]*6707 + 4096) >> 13;

  /* Then smooth left-to-right */
    ipix[0] = ipix[1] = ipix[2] = 0;
    for (col=0; col < (width & ~3); col++)
      FORC3 smrow[1][col][c] = ipix[c] =
	(smrow[0][col][c]*1485 + ipix[c]*6707 + 4096) >> 13;

  /* Smooth top-to-bottom */
    if (row == 0)
      memcpy (smrow[2], smrow[1], sizeof **smrow * width);
    else
      for (col=0; col < (width & ~3); col++)
	FORC3 smrow[2][col][c] =
	  (smrow[2][col][c]*6707 + smrow[1][col][c]*1485 + 4096) >> 13;

  /* Adjust the chroma toward the smooth values */
    for (col=0; col < (width & ~3); col++) {
      for (i=j=30, c=0; c < 3; c++) {
	i += smrow[2][col][c];
	j += image[row*width+col][c];
      }
      j = (j << 16) / i;
      for (sum=c=0; c < 3; c++) {
	ipix[c] = foveon_apply_curve (curve[c+3],
	  ((smrow[2][col][c] * j + 0x8000) >> 16) - image[row*width+col][c]);
	sum += ipix[c];
      }
      sum >>= 3;
      FORC3 {
	i = image[row*width+col][c] + ipix[c] - sum;
	if (i < 0) i = 0;
	image[row*width+col][c] = i;
      }
    }
  }
  free (shrink);
  free (smrow[6]);
  for (i=0; i < 8; i++)
    free (curve[i]);

  /* Trim off the black border */
  active[1] -= keep[1];
  active[3] -= 2;
  i = active[2] - active[0];
  for (row = 0; row < active[3]-active[1]; row++)
    memcpy (image[row*i], image[(row+active[1])*width+active[0]],
	 i * sizeof *image);
  width = i;
  height = row;
}
#undef image

/* END GPL BLOCK */

/*
   Seach from the current directory up to the root looking for
   a ".badpixels" file, and fix those pixels now.
 */
void CLASS bad_pixels()
{
  FILE *fp=NULL;
  char *fname, *cp, line[128];
  int len, time, row, col, r, c, rad, tot, n, fixed=0;

  if (!filters) return;
  for (len=32 ; ; len *= 2) {
    fname = malloc (len);
    if (!fname) return;
    if (getcwd (fname, len-16)) break;
    free (fname);
    if (errno != ERANGE) return;
  }
#if defined(WIN32) || defined(DJGPP)
  if (fname[1] == ':')
    memmove (fname, fname+2, len-2);
  for (cp=fname; *cp; cp++)
    if (*cp == '\\') *cp = '/';
#endif
  cp = fname + strlen(fname);
  if (cp[-1] == '/') cp--;
  while (*fname == '/') {
    strcpy (cp, "/.badpixels");
    if ((fp = fopen (fname, "r"))) break;
    if (cp == fname) break;
    while (*--cp != '/');
  }
  free (fname);
  if (!fp) return;
  while (fgets (line, 128, fp)) {
    cp = strchr (line, '#');
    if (cp) *cp = 0;
    if (sscanf (line, "%d %d %d", &col, &row, &time) != 3) continue;
    if ((unsigned) col >= width || (unsigned) row >= height) continue;
    if (time > timestamp) continue;
    for (tot=n=0, rad=1; rad < 3 && n==0; rad++)
      for (r = row-rad; r <= row+rad; r++)
	for (c = col-rad; c <= col+rad; c++)
	  if ((unsigned) r < height && (unsigned) c < width &&
		(r != row || c != col) && FC(r,c) == FC(row,col)) {
	    tot += BAYER(r,c);
	    n++;
	  }
    BAYER(row,col) = tot/n;
    if (verbose) {
      if (!fixed++)
	fprintf (stderr, "Fixed bad pixels at:");
      fprintf (stderr, " %d,%d", col, row);
    }
  }
  if (fixed) fputc ('\n', stderr);
  fclose (fp);
}

void CLASS pseudoinverse (double (*in)[3], double (*out)[3], int size)
{
  double work[3][6], num;
  int i, j, k;

  for (i=0; i < 3; i++) {
    for (j=0; j < 6; j++)
      work[i][j] = j == i+3;
    for (j=0; j < 3; j++)
      for (k=0; k < size; k++)
	work[i][j] += in[k][i] * in[k][j];
  }
  for (i=0; i < 3; i++) {
    num = work[i][i];
    for (j=0; j < 6; j++)
      work[i][j] /= num;
    for (k=0; k < 3; k++) {
      if (k==i) continue;
      num = work[k][i];
      for (j=0; j < 6; j++)
	work[k][j] -= work[i][j] * num;
    }
  }
  for (i=0; i < size; i++)
    for (j=0; j < 3; j++)
      for (out[i][j]=k=0; k < 3; k++)
	out[i][j] += work[j][k+3] * in[i][k];
}

void CLASS cam_xyz_coeff (double cam_xyz[4][3])
{
  double cam_rgb[4][3], inverse[4][3], num;
  int i, j, k;

  for (i=0; i < colors; i++)		/* Multiply out XYZ colorspace */
    for (j=0; j < 3; j++)
      for (cam_rgb[i][j] = k=0; k < 3; k++)
	cam_rgb[i][j] += cam_xyz[i][k] * xyz_rgb[k][j];

  for (i=0; i < colors; i++) {		/* Normalize cam_rgb so that */
    for (num=j=0; j < 3; j++)		/* cam_rgb * (1,1,1) is (1,1,1,1) */
      num += cam_rgb[i][j];
    for (j=0; j < 3; j++)
      cam_rgb[i][j] /= num;
    pre_mul[i] = 1 / num;
  }
  pseudoinverse (cam_rgb, inverse, colors);
  for (raw_color = i=0; i < 3; i++)
    for (j=0; j < colors; j++)
      rgb_cam[i][j] = inverse[j][i];
}

#ifdef COLORCHECK
void CLASS colorcheck()
{
#define NSQ 24
// Coordinates of the GretagMacbeth ColorChecker squares
// width, height, 1st_column, 1st_row
  static const int cut[NSQ][4] = {
    { 241, 231, 234, 274 },
    { 251, 235, 534, 274 },
    { 255, 239, 838, 272 },
    { 255, 240, 1146, 274 },
    { 251, 237, 1452, 278 },
    { 243, 238, 1758, 288 },
    { 253, 253, 218, 558 },
    { 255, 249, 524, 562 },
    { 261, 253, 830, 562 },
    { 260, 255, 1144, 564 },
    { 261, 255, 1450, 566 },
    { 247, 247, 1764, 576 },
    { 255, 251, 212, 862 },
    { 259, 259, 518, 862 },
    { 263, 261, 826, 864 },
    { 265, 263, 1138, 866 },
    { 265, 257, 1450, 872 },
    { 257, 255, 1762, 874 },
    { 257, 253, 212, 1164 },
    { 262, 251, 516, 1172 },
    { 263, 257, 826, 1172 },
    { 263, 255, 1136, 1176 },
    { 255, 252, 1452, 1182 },
    { 257, 253, 1760, 1180 } };
// ColorChecker Chart under 6500-kelvin illumination
  static const double gmb_xyY[NSQ][3] = {
    { 0.400, 0.350, 10.1 },		// Dark Skin
    { 0.377, 0.345, 35.8 },		// Light Skin
    { 0.247, 0.251, 19.3 },		// Blue Sky
    { 0.337, 0.422, 13.3 },		// Foliage
    { 0.265, 0.240, 24.3 },		// Blue Flower
    { 0.261, 0.343, 43.1 },		// Bluish Green
    { 0.506, 0.407, 30.1 },		// Orange
    { 0.211, 0.175, 12.0 },		// Purplish Blue
    { 0.453, 0.306, 19.8 },		// Moderate Red
    { 0.285, 0.202, 6.6 },		// Purple
    { 0.380, 0.489, 44.3 },		// Yellow Green
    { 0.473, 0.438, 43.1 },		// Orange Yellow
    { 0.187, 0.129, 6.1 },		// Blue
    { 0.305, 0.478, 23.4 },		// Green
    { 0.539, 0.313, 12.0 },		// Red
    { 0.448, 0.470, 59.1 },		// Yellow
    { 0.364, 0.233, 19.8 },		// Magenta
    { 0.196, 0.252, 19.8 },		// Cyan
    { 0.310, 0.316, 90.0 },		// White
    { 0.310, 0.316, 59.1 },		// Neutral 8
    { 0.310, 0.316, 36.2 },		// Neutral 6.5
    { 0.310, 0.316, 19.8 },		// Neutral 5
    { 0.310, 0.316, 9.0 },		// Neutral 3.5
    { 0.310, 0.316, 3.1 } };		// Black
  double gmb_cam[NSQ][4], gmb_xyz[NSQ][3];
  double inverse[NSQ][3], cam_xyz[4][3], num;
  int c, i, j, k, sq, row, col, count[4];

  memset (gmb_cam, 0, sizeof gmb_cam);
  for (sq=0; sq < NSQ; sq++) {
    FORCC count[c] = 0;
    for   (row=cut[sq][3]; row < cut[sq][3]+cut[sq][1]; row++)
      for (col=cut[sq][2]; col < cut[sq][2]+cut[sq][0]; col++) {
	c = FC(row,col);
	if (c >= colors) c -= 2;
	gmb_cam[sq][c] += BAYER(row,col);
	count[c]++;
      }
    FORCC gmb_cam[sq][c] = gmb_cam[sq][c]/count[c] - black;
    gmb_xyz[sq][0] = gmb_xyY[sq][2] * gmb_xyY[sq][0] / gmb_xyY[sq][1];
    gmb_xyz[sq][1] = gmb_xyY[sq][2];
    gmb_xyz[sq][2] = gmb_xyY[sq][2] *
		(1 - gmb_xyY[sq][0] - gmb_xyY[sq][1]) / gmb_xyY[sq][1];
  }
  pseudoinverse (gmb_xyz, inverse, NSQ);
  for (i=0; i < colors; i++)
    for (j=0; j < 3; j++)
      for (cam_xyz[i][j] = k=0; k < NSQ; k++)
	cam_xyz[i][j] += gmb_cam[k][i] * inverse[k][j];
  cam_xyz_coeff (cam_xyz);
  if (verbose) {
    printf ("    { \"%s %s\", %d,\n\t{", make, model, black);
    num = 10000 / (cam_xyz[1][0] + cam_xyz[1][1] + cam_xyz[1][2]);
    FORCC for (j=0; j < 3; j++)
      printf ("%c%d", (c | j) ? ',':' ', (int) (cam_xyz[c][j] * num + 0.5));
    puts (" } },");
  }
#undef NSQ
}
#endif

void CLASS scale_colors()
{
  int row, col, x, y, c, val;
  int min[4], max[4], sum[8];
  double dsum[8], dmin, dmax;
  float scale_mul[4];

  maximum -= black;
  if (use_auto_wb || (use_camera_wb && cam_mul[0] == -1)) {
    FORC4 min[c] = INT_MAX;
    FORC4 max[c] = 0;
    memset (dsum, 0, sizeof dsum);
    for (row=0; row < height-7; row += 8)
      for (col=0; col < width-7; col += 8) {
	memset (sum, 0, sizeof sum);
	for (y=row; y < row+8; y++)
	  for (x=col; x < col+8; x++)
	    FORC4 {
	      val = image[y*width+x][c];
	      if (!val) continue;
	      if (min[c] > val) min[c] = val;
	      if (max[c] < val) max[c] = val;
	      val -= black;
	      if (val > maximum-25) goto skip_block;
	      if (val < 0) val = 0;
	      sum[c] += val;
	      sum[c+4]++;
	    }
	for (c=0; c < 8; c++) dsum[c] += sum[c];
skip_block:
	continue;
      }
    FORC4 if (dsum[c]) pre_mul[c] = dsum[c+4] / dsum[c];
  }
  if (use_camera_wb && cam_mul[0] != -1) {
    memset (sum, 0, sizeof sum);
    for (row=0; row < 8; row++)
      for (col=0; col < 8; col++) {
	c = FC(row,col);
	if ((val = white[row][col] - black) > 0)
	  sum[c] += val;
	sum[c+4]++;
      }
    if (sum[0] && sum[1] && sum[2] && sum[3])
      FORC4 pre_mul[c] = (float) sum[c+4] / sum[c];
    else if (cam_mul[0] && cam_mul[2])
      memcpy (pre_mul, cam_mul, sizeof pre_mul);
    else
      fprintf (stderr, "%s: Cannot use camera white balance.\n", ifname);
  }
  if (user_mul[0])
    memcpy (pre_mul, user_mul, sizeof pre_mul);
  if (pre_mul[3] == 0) pre_mul[3] = colors < 4 ? pre_mul[1] : 1;
  for (dmin=DBL_MAX, dmax=c=0; c < 4; c++) {
    if (dmin > pre_mul[c])
	dmin = pre_mul[c];
    if (dmax < pre_mul[c])
	dmax = pre_mul[c];
  }
  if (!highlight) dmax = dmin;
  FORC4 scale_mul[c] = (pre_mul[c] /= dmax) * 65535.0 / maximum;
  if (verbose) {
    fprintf (stderr, "Scaling with black=%d, pre_mul[] =", black);
    FORC4 fprintf (stderr, " %f", pre_mul[c]);
    fputc ('\n', stderr);
  }
  for (row=0; row < height; row++)
    for (col=0; col < width; col++)
      FORC4 {
	val = image[row*width+col][c];
	if (!val) continue;
	val -= black;
	val *= scale_mul[c];
	image[row*width+col][c] = CLIP(val);
      }
  if (filters && colors == 3) {
    if (four_color_rgb) {
      colors++;
      FORC3 rgb_cam[c][3] = rgb_cam[c][1] /= 2;
    } else {
      for (row = FC(1,0) >> 1; row < height; row+=2)
	for (col = FC(row,1) & 1; col < width; col+=2)
	  image[row*width+col][1] = image[row*width+col][3];
      filters &= ~((filters & 0x55555555) << 1);
    }
  }
}

void CLASS border_interpolate (int border)
{
  unsigned row, col, y, x, c, sum[8];

  for (row=0; row < height; row++)
    for (col=0; col < width; col++) {
      if (col==border && row >= border && row < height-border)
	col = width-border;
      memset (sum, 0, sizeof sum);
      for (y=row-1; y != row+2; y++)
	for (x=col-1; x != col+2; x++)
	  if (y < height && x < width) {
	    sum[FC(y,x)] += BAYER(y,x);
	    sum[FC(y,x)+4]++;
	  }
      FORCC if (c != FC(row,col))
	image[row*width+col][c] = sum[c] / sum[c+4];
    }
}

void CLASS lin_interpolate()
{
  int code[8][2][32], *ip, sum[4];
  int c, i, x, y, row, col, shift, color;
  ushort *pix;

  if (verbose) fprintf (stderr, "Bilinear interpolation...\n");

  border_interpolate(1);
  for (row=0; row < 8; row++)
    for (col=0; col < 2; col++) {
      ip = code[row][col];
      memset (sum, 0, sizeof sum);
      for (y=-1; y <= 1; y++)
	for (x=-1; x <= 1; x++) {
	  shift = (y==0) + (x==0);
	  if (shift == 2) continue;
	  color = FC(row+y,col+x);
	  *ip++ = (width*y + x)*4 + color;
	  *ip++ = shift;
	  *ip++ = color;
	  sum[color] += 1 << shift;
	}
      FORCC
	if (c != FC(row,col)) {
	  *ip++ = c;
	  *ip++ = sum[c];
	}
    }
  for (row=1; row < height-1; row++)
    for (col=1; col < width-1; col++) {
      pix = image[row*width+col];
      ip = code[row & 7][col & 1];
      memset (sum, 0, sizeof sum);
      for (i=8; i--; ip+=3)
	sum[ip[2]] += pix[ip[0]] << ip[1];
      for (i=colors; --i; ip+=2)
	pix[ip[0]] = sum[ip[0]] / ip[1];
    }
}

/*
   This algorithm is officially called:

   "Interpolation using a Threshold-based variable number of gradients"

   described in http://www-ise.stanford.edu/~tingchen/algodep/vargra.html

   I've extended the basic idea to work with non-Bayer filter arrays.
   Gradients are numbered clockwise from NW=0 to W=7.
 */
void CLASS vng_interpolate()
{
  static const signed char *cp, terms[] = {
    -2,-2,+0,-1,0,0x01, -2,-2,+0,+0,1,0x01, -2,-1,-1,+0,0,0x01,
    -2,-1,+0,-1,0,0x02, -2,-1,+0,+0,0,0x03, -2,-1,+0,+1,1,0x01,
    -2,+0,+0,-1,0,0x06, -2,+0,+0,+0,1,0x02, -2,+0,+0,+1,0,0x03,
    -2,+1,-1,+0,0,0x04, -2,+1,+0,-1,1,0x04, -2,+1,+0,+0,0,0x06,
    -2,+1,+0,+1,0,0x02, -2,+2,+0,+0,1,0x04, -2,+2,+0,+1,0,0x04,
    -1,-2,-1,+0,0,0x80, -1,-2,+0,-1,0,0x01, -1,-2,+1,-1,0,0x01,
    -1,-2,+1,+0,1,0x01, -1,-1,-1,+1,0,0x88, -1,-1,+1,-2,0,0x40,
    -1,-1,+1,-1,0,0x22, -1,-1,+1,+0,0,0x33, -1,-1,+1,+1,1,0x11,
    -1,+0,-1,+2,0,0x08, -1,+0,+0,-1,0,0x44, -1,+0,+0,+1,0,0x11,
    -1,+0,+1,-2,1,0x40, -1,+0,+1,-1,0,0x66, -1,+0,+1,+0,1,0x22,
    -1,+0,+1,+1,0,0x33, -1,+0,+1,+2,1,0x10, -1,+1,+1,-1,1,0x44,
    -1,+1,+1,+0,0,0x66, -1,+1,+1,+1,0,0x22, -1,+1,+1,+2,0,0x10,
    -1,+2,+0,+1,0,0x04, -1,+2,+1,+0,1,0x04, -1,+2,+1,+1,0,0x04,
    +0,-2,+0,+0,1,0x80, +0,-1,+0,+1,1,0x88, +0,-1,+1,-2,0,0x40,
    +0,-1,+1,+0,0,0x11, +0,-1,+2,-2,0,0x40, +0,-1,+2,-1,0,0x20,
    +0,-1,+2,+0,0,0x30, +0,-1,+2,+1,1,0x10, +0,+0,+0,+2,1,0x08,
    +0,+0,+2,-2,1,0x40, +0,+0,+2,-1,0,0x60, +0,+0,+2,+0,1,0x20,
    +0,+0,+2,+1,0,0x30, +0,+0,+2,+2,1,0x10, +0,+1,+1,+0,0,0x44,
    +0,+1,+1,+2,0,0x10, +0,+1,+2,-1,1,0x40, +0,+1,+2,+0,0,0x60,
    +0,+1,+2,+1,0,0x20, +0,+1,+2,+2,0,0x10, +1,-2,+1,+0,0,0x80,
    +1,-1,+1,+1,0,0x88, +1,+0,+1,+2,0,0x08, +1,+0,+2,-1,0,0x40,
    +1,+0,+2,+1,0,0x10
  }, chood[] = { -1,-1, -1,0, -1,+1, 0,+1, +1,+1, +1,0, +1,-1, 0,-1 };
  ushort (*brow[5])[4], *pix;
  int code[8][2][320], *ip, gval[8], gmin, gmax, sum[4];
  int row, col, x, y, x1, x2, y1, y2, t, weight, grads, color, diag;
  int g, diff, thold, num, c;

  lin_interpolate();
  if (verbose) fprintf (stderr, "VNG interpolation...\n");

  for (row=0; row < 8; row++) {		/* Precalculate for VNG */
    for (col=0; col < 2; col++) {
      ip = code[row][col];
      for (cp=terms, t=0; t < 64; t++) {
	y1 = *cp++;  x1 = *cp++;
	y2 = *cp++;  x2 = *cp++;
	weight = *cp++;
	grads = *cp++;
	color = FC(row+y1,col+x1);
	if (FC(row+y2,col+x2) != color) continue;
	diag = (FC(row,col+1) == color && FC(row+1,col) == color) ? 2:1;
	if (abs(y1-y2) == diag && abs(x1-x2) == diag) continue;
	*ip++ = (y1*width + x1)*4 + color;
	*ip++ = (y2*width + x2)*4 + color;
	*ip++ = weight;
	for (g=0; g < 8; g++)
	  if (grads & 1<<g) *ip++ = g;
	*ip++ = -1;
      }
      *ip++ = INT_MAX;
      for (cp=chood, g=0; g < 8; g++) {
	y = *cp++;  x = *cp++;
	*ip++ = (y*width + x) * 4;
	color = FC(row,col);
	if (FC(row+y,col+x) != color && FC(row+y*2,col+x*2) == color)
	  *ip++ = (y*width + x) * 8 + color;
	else
	  *ip++ = 0;
      }
    }
  }
  brow[4] = calloc (width*3, sizeof **brow);
  merror (brow[4], "vng_interpolate()");
  for (row=0; row < 3; row++)
    brow[row] = brow[4] + row*width;
  for (row=2; row < height-2; row++) {		/* Do VNG interpolation */
    for (col=2; col < width-2; col++) {
      pix = image[row*width+col];
      ip = code[row & 7][col & 1];
      memset (gval, 0, sizeof gval);
      while ((g = ip[0]) != INT_MAX) {		/* Calculate gradients */
	diff = ABS(pix[g] - pix[ip[1]]) << ip[2];
	gval[ip[3]] += diff;
	ip += 5;
	if ((g = ip[-1]) == -1) continue;
	gval[g] += diff;
	while ((g = *ip++) != -1)
	  gval[g] += diff;
      }
      ip++;
      gmin = gmax = gval[0];			/* Choose a threshold */
      for (g=1; g < 8; g++) {
	if (gmin > gval[g]) gmin = gval[g];
	if (gmax < gval[g]) gmax = gval[g];
      }
      if (gmax == 0) {
	memcpy (brow[2][col], pix, sizeof *image);
	continue;
      }
      thold = gmin + (gmax >> 1);
      memset (sum, 0, sizeof sum);
      color = FC(row,col);
      for (num=g=0; g < 8; g++,ip+=2) {		/* Average the neighbors */
	if (gval[g] <= thold) {
	  FORCC
	    if (c == color && ip[1])
	      sum[c] += (pix[c] + pix[ip[1]]) >> 1;
	    else
	      sum[c] += pix[ip[0] + c];
	  num++;
	}
      }
      FORCC {					/* Save to buffer */
	t = pix[color];
	if (c != color)
	  t += (sum[c] - sum[color]) / num;
	brow[2][col][c] = CLIP(t);
      }
    }
    if (row > 3)				/* Write buffer to image */
      memcpy (image[(row-2)*width+2], brow[0]+2, (width-4)*sizeof *image);
    for (g=0; g < 4; g++)
      brow[(g-1) & 3] = brow[g];
  }
  memcpy (image[(row-2)*width+2], brow[0]+2, (width-4)*sizeof *image);
  memcpy (image[(row-1)*width+2], brow[1]+2, (width-4)*sizeof *image);
  free (brow[4]);
}

void CLASS cam_to_cielab (ushort cam[4], float lab[3])
{
  int c, i, j, k;
  float r, xyz[3];
  static float cbrt[0x10000], xyz_cam[3][4];

  if (cam == NULL) {
    for (i=0; i < 0x10000; i++) {
      r = (float) i / maximum;
      cbrt[i] = r > 0.008856 ? pow(r,1/3.0) : 7.787*r + 16/116.0;
    }
    for (i=0; i < 3; i++)
      for (j=0; j < colors; j++)
        for (xyz_cam[i][j] = k=0; k < 3; k++)
	  xyz_cam[i][j] += xyz_rgb[i][k] * rgb_cam[k][j] / d65_white[i];
  } else {
    for (i=0; i < 3; i++) {
      for (xyz[i]=0.5, c=0; c < colors; c++)
	xyz[i] += xyz_cam[i][c] * cam[c];
      xyz[i] = cbrt[CLIP((int) xyz[i])];
    }
    lab[0] = 116 * xyz[1] - 16;
    lab[1] = 500 * (xyz[0] - xyz[1]);
    lab[2] = 200 * (xyz[1] - xyz[2]);
  }
}

/*
   Adaptive Homogeneity-Directed interpolation is based on
   the work of Keigo Hirakawa, Thomas Parks, and Paul Lee.
 */
#define TS 256		/* Tile Size */

void CLASS ahd_interpolate()
{
  int i, j, top, left, row, col, tr, tc, fc, c, d, val, hm[2];
  ushort (*pix)[4], (*rix)[3];
  static const int dir[4] = { -1, 1, -TS, TS };
  unsigned ldiff[2][4], abdiff[2][4], leps, abeps;
  float flab[3];
  ushort (*rgb)[TS][TS][3];
   short (*lab)[TS][TS][3];
   char (*homo)[TS][TS], *buffer;

  if (verbose) fprintf (stderr, "AHD interpolation...\n");

  border_interpolate(3);
  buffer = malloc (26*TS*TS);			/* 1664 kB */
  merror (buffer, "ahd_interpolate()");
  rgb  = (void *) buffer;
  lab  = (void *) (buffer + 12*TS*TS);
  homo = (void *) (buffer + 24*TS*TS);

  for (top=0; top < height; top += TS-6)
    for (left=0; left < width; left += TS-6) {
      memset (rgb, 0, 12*TS*TS);

/*  Interpolate green horizontally and vertically:		*/
      for (row = top < 2 ? 2:top; row < top+TS && row < height-2; row++) {
	col = left + (FC(row,left) == 1);
	if (col < 2) col += 2;
	for (fc = FC(row,col); col < left+TS && col < width-2; col+=2) {
	  pix = image + row*width+col;
	  val = ((pix[-1][1] + pix[0][fc] + pix[1][1]) * 2
		- pix[-2][fc] - pix[2][fc]) >> 2;
	  rgb[0][row-top][col-left][1] = ULIM(val,pix[-1][1],pix[1][1]);
	  val = ((pix[-width][1] + pix[0][fc] + pix[width][1]) * 2
		- pix[-2*width][fc] - pix[2*width][fc]) >> 2;
	  rgb[1][row-top][col-left][1] = ULIM(val,pix[-width][1],pix[width][1]);
	}
      }
/*  Interpolate red and blue, and convert to CIELab:		*/
      for (d=0; d < 2; d++)
	for (row=top+1; row < top+TS-1 && row < height-1; row++)
	  for (col=left+1; col < left+TS-1 && col < width-1; col++) {
	    pix = image + row*width+col;
	    rix = &rgb[d][row-top][col-left];
	    if ((c = 2 - FC(row,col)) == 1) {
	      c = FC(row+1,col);
	      val = pix[0][1] + (( pix[-1][2-c] + pix[1][2-c]
				 - rix[-1][1] - rix[1][1] ) >> 1);
	      rix[0][2-c] = CLIP(val);
	      val = pix[0][1] + (( pix[-width][c] + pix[width][c]
				 - rix[-TS][1] - rix[TS][1] ) >> 1);
	    } else
	      val = rix[0][1] + (( pix[-width-1][c] + pix[-width+1][c]
				 + pix[+width-1][c] + pix[+width+1][c]
				 - rix[-TS-1][1] - rix[-TS+1][1]
				 - rix[+TS-1][1] - rix[+TS+1][1] + 1) >> 2);
	    rix[0][c] = CLIP(val);
	    c = FC(row,col);
	    rix[0][c] = pix[0][c];
	    cam_to_cielab (rix[0], flab);
	    FORC3 lab[d][row-top][col-left][c] = 64*flab[c];
	  }
/*  Build homogeneity maps from the CIELab images:		*/
      memset (homo, 0, 2*TS*TS);
      for (row=top+2; row < top+TS-2 && row < height; row++) {
	tr = row-top;
	for (col=left+2; col < left+TS-2 && col < width; col++) {
	  tc = col-left;
	  for (d=0; d < 2; d++)
	    for (i=0; i < 4; i++)
	      ldiff[d][i] = ABS(lab[d][tr][tc][0]-lab[d][tr][tc+dir[i]][0]);
	  leps = MIN(MAX(ldiff[0][0],ldiff[0][1]),
		     MAX(ldiff[1][2],ldiff[1][3]));
	  for (d=0; d < 2; d++)
	    for (i=0; i < 4; i++)
	      if (i >> 1 == d || ldiff[d][i] <= leps)
		abdiff[d][i] = SQR(lab[d][tr][tc][1]-lab[d][tr][tc+dir[i]][1])
			     + SQR(lab[d][tr][tc][2]-lab[d][tr][tc+dir[i]][2]);
	  abeps = MIN(MAX(abdiff[0][0],abdiff[0][1]),
		      MAX(abdiff[1][2],abdiff[1][3]));
	  for (d=0; d < 2; d++)
	    for (i=0; i < 4; i++)
	      if (ldiff[d][i] <= leps && abdiff[d][i] <= abeps)
		homo[d][tr][tc]++;
	}
      }
/*  Combine the most homogenous pixels for the final result:	*/
      for (row=top+3; row < top+TS-3 && row < height-3; row++) {
	tr = row-top;
	for (col=left+3; col < left+TS-3 && col < width-3; col++) {
	  tc = col-left;
	  for (d=0; d < 2; d++)
	    for (hm[d]=0, i=tr-1; i <= tr+1; i++)
	      for (j=tc-1; j <= tc+1; j++)
		hm[d] += homo[d][i][j];
	  if (hm[0] != hm[1])
	    FORC3 image[row*width+col][c] = rgb[hm[1] > hm[0]][tr][tc][c];
	  else
	    FORC3 image[row*width+col][c] =
		(rgb[0][tr][tc][c] + rgb[1][tr][tc][c]) >> 1;
	}
      }
    }
  free (buffer);
}
#undef TS

/*
   Bilateral Filtering was developed by C. Tomasi and R. Manduchi.
 */
void CLASS bilateral_filter()
{
  float (**window)[7], *kernel, scale_r, elut[1024], sum[5];
  int c, i, wr, ws, wlast, row, col, y, x;
  unsigned sep;

  if (verbose) fprintf (stderr, "Bilateral filtering...\n");

  wr = ceil(sigma_d*2);		/* window radius */
  ws = 2*wr + 1;		/* window size */
  window = calloc ((ws+1)*sizeof  *window +
		 ws*width*sizeof **window + ws*sizeof *kernel, 1);
  merror (window, "bilateral_filter()");
  for (i=0; i <= ws; i++)
    window[i] = (float(*)[7]) (window+ws+1) + i*width;
  kernel = (float *) window[ws] + wr;
  for (i=-wr; i <= wr; i++)
    kernel[i] = 256 / (2*SQR(sigma_d)) * i*i + 0.25;
  scale_r     = 256 / (2*SQR(sigma_r));
  for (i=0; i < 1024; i++)
    elut[i] = exp (-i/256.0);

  for (wlast=-1, row=0; row < height; row++) {
    while (wlast < row+wr) {
      wlast++;
      for (i=0; i <= ws; i++)	/* rotate window rows */
	window[(ws+i) % (ws+1)] = window[i];
      if (wlast < height)
	for (col=0; col < width; col++) {
	  FORCC window[ws-1][col][c] = image[wlast*width+col][c];
	  cam_to_cielab (image[wlast*width+col], window[ws-1][col]+4);
	}
    }
    for (col=0; col < width; col++) {
      memset (sum, 0, sizeof sum);
      for (y=-wr; y <= wr; y++)
	if ((unsigned)(row+y) < height)
	  for (x=-wr; x <= wr; x++)
	    if ((unsigned)(col+x) < width) {
	      sep = ( SQR(window[wr+y][col+x][4] - window[wr][col][4])
		    + SQR(window[wr+y][col+x][5] - window[wr][col][5])
		    + SQR(window[wr+y][col+x][6] - window[wr][col][6]) )
			* scale_r + kernel[y] + kernel[x];
	      if (sep < 1024) {
		FORCC sum[c] += elut[sep] * window[wr+y][col+x][c];
		sum[4] += elut[sep];
	      }
	    }
      FORCC image[row*width+col][c] = sum[c]/sum[4];
    }
  }
  free (window);
}

#define SIGMA 3
#define FS 13
void CLASS recover_highlights()
{
  float base[FS][FS], (*filt)[FS];
  int c, y, x, s, ns, row, col, val, sat[4];
  double sum, wgt;
  ushort *cent, *test;

  if (verbose) fprintf (stderr, "Highlight recovery...\n");

  filt = (float (*)[FS]) &base[FS/2][FS/2];
  for (y=-FS/2; y <= FS/2; y++)
    for (x=-FS/2; x <= FS/2; x++)
      filt[y][x] = exp(-(x*x+y*y)/(2*SIGMA*SIGMA));
  for (ns=0, c=1; c < colors; c++)
    if (pre_mul[ns] < pre_mul[c]) ns = c;
  FORCC sat[c] = 62000 * pre_mul[c];
  for (row=0; row < height; row++)
    for (col=0; col < width; col++) {
      cent = image[row*width+col];
      for (s=0; s < colors; s++) {
	if (s == ns || cent[s] < sat[s]) continue;
	sum = wgt = 4;
	for (y = MAX(-FS/2,-row); y <= MIN(FS/2,height-row-1); y++)
	  for (x = MAX(-FS/2,-col); x <= MIN(FS/2,width-col-1); x++) {
	    test = image[(row+y)*width+col+x];
	    if (2*test[s]/sat[s] == 1 && test[ns]/32700 == 1) {
	      sum += filt[y][x] * test[s] / test[ns];
	      wgt += filt[y][x];
	    }
	  }
	if (cent[s] < (val = sum*cent[ns]/wgt))
	  cent[s] = CLIP(val);
      }
    }
}
#undef SIGMA
#undef FS

void CLASS tiff_get (unsigned base,
	unsigned *tag, unsigned *type, unsigned *len, unsigned *save)
{
  *tag  = get2();
  *type = get2();
  *len  = get4();
  *save = ftell(ifp) + 4;
  if (*len * ("1112481124848"[*type < 13 ? *type:0]-'0') > 4)
    fseek (ifp, get4()+base, SEEK_SET);
}

void CLASS parse_olympus_note (int base)
{
  unsigned entries, tag, type, len, save;

  entries = get2();
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    if (tag == 257) thumb_offset = get4();
    if (tag == 258) thumb_length = get4();
    fseek (ifp, save, SEEK_SET);
  }
}

void CLASS parse_makernote (int base)
{
  static const uchar xlat[2][256] = {
  { 0xc1,0xbf,0x6d,0x0d,0x59,0xc5,0x13,0x9d,0x83,0x61,0x6b,0x4f,0xc7,0x7f,0x3d,0x3d,
    0x53,0x59,0xe3,0xc7,0xe9,0x2f,0x95,0xa7,0x95,0x1f,0xdf,0x7f,0x2b,0x29,0xc7,0x0d,
    0xdf,0x07,0xef,0x71,0x89,0x3d,0x13,0x3d,0x3b,0x13,0xfb,0x0d,0x89,0xc1,0x65,0x1f,
    0xb3,0x0d,0x6b,0x29,0xe3,0xfb,0xef,0xa3,0x6b,0x47,0x7f,0x95,0x35,0xa7,0x47,0x4f,
    0xc7,0xf1,0x59,0x95,0x35,0x11,0x29,0x61,0xf1,0x3d,0xb3,0x2b,0x0d,0x43,0x89,0xc1,
    0x9d,0x9d,0x89,0x65,0xf1,0xe9,0xdf,0xbf,0x3d,0x7f,0x53,0x97,0xe5,0xe9,0x95,0x17,
    0x1d,0x3d,0x8b,0xfb,0xc7,0xe3,0x67,0xa7,0x07,0xf1,0x71,0xa7,0x53,0xb5,0x29,0x89,
    0xe5,0x2b,0xa7,0x17,0x29,0xe9,0x4f,0xc5,0x65,0x6d,0x6b,0xef,0x0d,0x89,0x49,0x2f,
    0xb3,0x43,0x53,0x65,0x1d,0x49,0xa3,0x13,0x89,0x59,0xef,0x6b,0xef,0x65,0x1d,0x0b,
    0x59,0x13,0xe3,0x4f,0x9d,0xb3,0x29,0x43,0x2b,0x07,0x1d,0x95,0x59,0x59,0x47,0xfb,
    0xe5,0xe9,0x61,0x47,0x2f,0x35,0x7f,0x17,0x7f,0xef,0x7f,0x95,0x95,0x71,0xd3,0xa3,
    0x0b,0x71,0xa3,0xad,0x0b,0x3b,0xb5,0xfb,0xa3,0xbf,0x4f,0x83,0x1d,0xad,0xe9,0x2f,
    0x71,0x65,0xa3,0xe5,0x07,0x35,0x3d,0x0d,0xb5,0xe9,0xe5,0x47,0x3b,0x9d,0xef,0x35,
    0xa3,0xbf,0xb3,0xdf,0x53,0xd3,0x97,0x53,0x49,0x71,0x07,0x35,0x61,0x71,0x2f,0x43,
    0x2f,0x11,0xdf,0x17,0x97,0xfb,0x95,0x3b,0x7f,0x6b,0xd3,0x25,0xbf,0xad,0xc7,0xc5,
    0xc5,0xb5,0x8b,0xef,0x2f,0xd3,0x07,0x6b,0x25,0x49,0x95,0x25,0x49,0x6d,0x71,0xc7 },
  { 0xa7,0xbc,0xc9,0xad,0x91,0xdf,0x85,0xe5,0xd4,0x78,0xd5,0x17,0x46,0x7c,0x29,0x4c,
    0x4d,0x03,0xe9,0x25,0x68,0x11,0x86,0xb3,0xbd,0xf7,0x6f,0x61,0x22,0xa2,0x26,0x34,
    0x2a,0xbe,0x1e,0x46,0x14,0x68,0x9d,0x44,0x18,0xc2,0x40,0xf4,0x7e,0x5f,0x1b,0xad,
    0x0b,0x94,0xb6,0x67,0xb4,0x0b,0xe1,0xea,0x95,0x9c,0x66,0xdc,0xe7,0x5d,0x6c,0x05,
    0xda,0xd5,0xdf,0x7a,0xef,0xf6,0xdb,0x1f,0x82,0x4c,0xc0,0x68,0x47,0xa1,0xbd,0xee,
    0x39,0x50,0x56,0x4a,0xdd,0xdf,0xa5,0xf8,0xc6,0xda,0xca,0x90,0xca,0x01,0x42,0x9d,
    0x8b,0x0c,0x73,0x43,0x75,0x05,0x94,0xde,0x24,0xb3,0x80,0x34,0xe5,0x2c,0xdc,0x9b,
    0x3f,0xca,0x33,0x45,0xd0,0xdb,0x5f,0xf5,0x52,0xc3,0x21,0xda,0xe2,0x22,0x72,0x6b,
    0x3e,0xd0,0x5b,0xa8,0x87,0x8c,0x06,0x5d,0x0f,0xdd,0x09,0x19,0x93,0xd0,0xb9,0xfc,
    0x8b,0x0f,0x84,0x60,0x33,0x1c,0x9b,0x45,0xf1,0xf0,0xa3,0x94,0x3a,0x12,0x77,0x33,
    0x4d,0x44,0x78,0x28,0x3c,0x9e,0xfd,0x65,0x57,0x16,0x94,0x6b,0xfb,0x59,0xd0,0xc8,
    0x22,0x36,0xdb,0xd2,0x63,0x98,0x43,0xa1,0x04,0x87,0x86,0xf7,0xa6,0x26,0xbb,0xd6,
    0x59,0x4d,0xbf,0x6a,0x2e,0xaa,0x2b,0xef,0xe6,0x78,0xb6,0x4e,0xe0,0x2f,0xdc,0x7c,
    0xbe,0x57,0x19,0x32,0x7e,0x2a,0xd0,0xb8,0xba,0x29,0x00,0x3c,0x52,0x7d,0xa8,0x49,
    0x3b,0x2d,0xeb,0x25,0x49,0xfa,0xa3,0xaa,0x39,0xa7,0xc5,0xa7,0x50,0x11,0x36,0xfb,
    0xc6,0x67,0x4a,0xf5,0xa5,0x12,0x65,0x7e,0xb0,0xdf,0xaf,0x4e,0xb3,0x61,0x7f,0x2f } };
  unsigned offset=0, entries, tag, type, len, save, c;
  unsigned ver97=0, serial=0, i;
  uchar buf97[324], ci, cj, ck;
  short sorder;
  char buf[10];
/*
   The MakerNote might have its own TIFF header (possibly with
   its own byte-order!), or it might just be a table.
 */
  sorder = order;
  fread (buf, 1, 10, ifp);
  if (!strncmp (buf,"KC"  ,2) ||	/* these aren't TIFF format */
      !strncmp (buf,"KDK" ,3) ||
      !strncmp (buf,"MLY" ,3) ||
      !strncmp (buf,"VER" ,3) ||
      !strncmp (buf,"IIII",4) ||
      !strncmp (buf,"MMMM",4)) return;
  if (!strcmp (buf,"Nikon")) {
    base = ftell(ifp);
    order = get2();
    if (get2() != 42) goto quit;
    offset = get4();
    fseek (ifp, offset-8, SEEK_CUR);
  } else if (!strncmp (buf,"FUJIFILM",8) ||
	     !strncmp (buf,"SONY",4) ||
	     !strcmp  (buf,"Panasonic")) {
    order = 0x4949;
    fseek (ifp,  2, SEEK_CUR);
  } else if (!strcmp (buf,"OLYMP") ||
	     !strcmp (buf,"LEICA") ||
	     !strcmp (buf,"Ricoh") ||
	     !strcmp (buf,"EPSON"))
    fseek (ifp, -2, SEEK_CUR);
  else if (!strcmp (buf,"AOC") ||
	   !strcmp (buf,"QVC"))
    fseek (ifp, -4, SEEK_CUR);
  else fseek (ifp, -10, SEEK_CUR);

  entries = get2();
  if (entries > 1000) return;
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    if (tag == 2 && strstr(make,"NIKON"))
      iso_speed = (get2(),get2());
    if (tag == 4 && len == 27) {
      iso_speed = 50 * pow (2, (get4(),get2())/32.0 - 4);
      aperture = (get2(), pow (2, get2()/64.0));
      shutter = pow (2, ((short) get2())/-32.0);
    }
    if (tag == 8 && type == 4)
      shot_order = get4();
    if (tag == 0xc && len == 4) {
      cam_mul[0] = getrat();
      cam_mul[2] = getrat();
    }
    if (tag == 0x10 && type == 4)
      unique_id = get4();
    if (tag == 0x14 && len == 2560 && type == 7) {
      fseek (ifp, 1248, SEEK_CUR);
      goto get2_256;
    }
    if (strstr(make,"PENTAX")) {
      if (tag == 0x1b) tag = 0x1018;
      if (tag == 0x1c) tag = 0x1017;
    }
    if (tag == 0x1d)
      while ((c = fgetc(ifp)))
	serial = serial*10 + (isdigit(c) ? c - '0' : c % 10);
    if (tag == 0x81 && type == 4) {
      data_offset = get4();
      fseek (ifp, data_offset + 41, SEEK_SET);
      raw_height = get2() * 2;
      raw_width  = get2();
      filters = 0x61616161;
    }
    if ((tag == 0x81  && type == 7) ||
	(tag == 0x100 && type == 7) ||
	(tag == 0x280 && type == 1)) {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (tag == 0x88 && type == 4 && (thumb_offset = get4()))
      thumb_offset += base;
    if (tag == 0x89 && type == 4)
      thumb_length = get4();
    if (tag == 0x8c)
      curve_offset = ftell(ifp) + 2112;
    if (tag == 0x96)
      curve_offset = ftell(ifp) + 2;
    if (tag == 0x97) {
      for (i=0; i < 4; i++)
	ver97 = (ver97 << 4) + fgetc(ifp)-'0';
      switch (ver97) {
	case 0x100:
	  fseek (ifp, 68, SEEK_CUR);
	  FORC4 cam_mul[(c >> 1) | ((c & 1) << 1)] = get2();
	  break;
	case 0x102:
	  fseek (ifp, 6, SEEK_CUR);
	  goto get2_rggb;
	case 0x103:
	  fseek (ifp, 16, SEEK_CUR);
	  FORC4 cam_mul[c] = get2();
      }
      if (ver97 >> 8 == 2) {
	if (ver97 != 0x205) fseek (ifp, 280, SEEK_CUR);
	fread (buf97, 324, 1, ifp);
      }
    }
    if (tag == 0xa7 && ver97 >> 8 == 2) {
      ci = xlat[0][serial & 0xff];
      cj = xlat[1][fgetc(ifp)^fgetc(ifp)^fgetc(ifp)^fgetc(ifp)];
      ck = 0x60;
      for (i=0; i < 324; i++)
	buf97[i] ^= (cj += ci * ck++);
      FORC4 cam_mul[c ^ (c >> 1)] =
	sget2 (buf97 + (ver97 == 0x205 ? 14:6) + c*2);
    }
    if (tag == 0x200 && len == 4)
      black = (get2()+get2()+get2()+get2())/4;
    if (tag == 0x201 && len == 4)
      goto get2_rggb;
    if (tag == 0x401 && len == 4) {
      black = (get4()+get4()+get4()+get4())/4;
    }
    if (tag == 0xe01) {		/* Nikon Capture Note */
      type = order;
      order = 0x4949;
      fseek (ifp, 22, SEEK_CUR);
      for (offset=22; offset+22 < len; offset += 22+i) {
	tag = get4();
	fseek (ifp, 14, SEEK_CUR);
	i = get4()-4;
	if (tag == 0x76a43207) flip = get2();
	else fseek (ifp, i, SEEK_CUR);
      }
      order = type;
    }
    if (tag == 0xe80 && len == 256 && type == 7) {
      fseek (ifp, 48, SEEK_CUR);
      cam_mul[0] = get2() * 508 * 1.078 / 0x10000;
      cam_mul[2] = get2() * 382 * 1.173 / 0x10000;
    }
    if (tag == 0xf00 && len == 614 && type == 7) {
      fseek (ifp, 176, SEEK_CUR);
      goto get2_256;
    }
    if (tag == 0x1011 && len == 9 && use_camera_wb) {
      for (i=0; i < 3; i++)
	FORC3 rgb_cam[i][c] = ((short) get2()) / 256.0;
      raw_color = rgb_cam[0][0] < 1;
    }
    if (tag == 0x1017)
      cam_mul[0] = get2() / 256.0;
    if (tag == 0x1018)
      cam_mul[2] = get2() / 256.0;
    if (tag == 0x2011 && len == 2) {
get2_256:
      order = 0x4d4d;
      cam_mul[0] = get2() / 256.0;
      cam_mul[2] = get2() / 256.0;
    }
    if (tag == 0x2020)
      parse_olympus_note (base);
    if (tag == 0x4001) {
      i = len == 582 ? 50 : len == 653 ? 68 : len == 796 ? 126 : 0;
      fseek (ifp, i ,SEEK_CUR);
get2_rggb:
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
    }
    fseek (ifp, save, SEEK_SET);
  }
quit:
  order = sorder;
}

/*
   Since the TIFF DateTime string has no timezone information,
   assume that the camera's clock was set to Universal Time.
 */
void CLASS get_timestamp (int reversed)
{
  struct tm t;
  char str[20];
  int i;

  if (timestamp) return;
  str[19] = 0;
  if (reversed)
    for (i=19; i--; ) str[i] = fgetc(ifp);
  else
    fread (str, 19, 1, ifp);
  if (sscanf (str, "%d:%d:%d %d:%d:%d", &t.tm_year, &t.tm_mon,
	&t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) != 6)
    return;
  t.tm_year -= 1900;
  t.tm_mon -= 1;
  if (mktime(&t) > 0)
    timestamp = mktime(&t);
}

void CLASS parse_exif (int base)
{
  unsigned kodak, entries, tag, type, len, save;
  double expo;

  kodak = !strncmp(make,"EASTMAN",7);
  entries = get2();
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    switch (tag) {
      case 33434:  shutter = getrat();			break;
      case 33437:  aperture = getrat();			break;
      case 34855:  iso_speed = get2();			break;
      case 36867:
      case 36868:  get_timestamp(0);			break;
      case 37377:  if ((expo = -getrat()) < 128)
		     shutter = pow (2, expo);		break;
      case 37378:  aperture = pow (2, getrat()/2);	break;
      case 37386:  focal_len = getrat();		break;
      case 37500:  parse_makernote (base);		break;
      case 40962:  if (kodak) raw_width  = get4();	break;
      case 40963:  if (kodak) raw_height = get4();	break;
    }
    fseek (ifp, save, SEEK_SET);
  }
}

void CLASS romm_coeff (float romm_cam[3][3])
{
  static const float rgb_romm[3][3] =	/* ROMM == Kodak ProPhoto */
  { {  2.0564, -0.7932, -0.2632 },
    { -0.2118,  1.2490, -0.0372 },
    { -0.0152, -0.1405,  1.1556 } };
  int i, j, k;

  for (raw_color = i=0; i < 3; i++)
    for (j=0; j < 3; j++)
      for (rgb_cam[i][j] = k=0; k < 3; k++)
	rgb_cam[i][j] += rgb_romm[i][k] * romm_cam[k][j];
}

void CLASS parse_mos (int offset)
{
  char data[40];
  int skip, from, i, c, neut[4], planes=0, frot=0;
  static const char *mod[] = { "Aptus","Valeo","Volare" };
  float romm_cam[3][3];

  fseek (ifp, offset, SEEK_SET);
  while (1) {
    fread (data, 1, 8, ifp);
    if (strcmp(data,"PKTS")) break;
    if (!make[0]) strcpy(make,"Leaf");
    fread (data, 1, 40, ifp);
    skip = get4();
    from = ftell(ifp);
    if (!strcmp(data,"JPEG_preview_data")) {
      thumb_offset = from;
      thumb_length = skip;
    }
    if (!strcmp(data,"icc_camera_profile")) {
      profile_offset = from;
      profile_length = skip;
    }
    if (!strcmp(data,"CaptProf_serial_number")) {
      fread (data, 1, 40, ifp);
      for (i=0; i < sizeof mod / sizeof *mod; i++)
	if (data[0] == mod[i][0] && data[1] == toupper(mod[i][1]))
	  sprintf (model, "%s %d", mod[i], atoi(data+2));
    }
    if (!strcmp(data,"CaptProf_color_matrix") && use_camera_wb) {
      for (i=0; i < 9; i++)
	fscanf (ifp, "%f", &romm_cam[0][i]);
      romm_coeff (romm_cam);
    }
    if (!strcmp(data,"CaptProf_number_of_planes"))
      fscanf (ifp, "%d", &planes);
    if (!strcmp(data,"CaptProf_raw_data_rotation"))
      fscanf (ifp, "%d", &flip);
    if (!strcmp(data,"CaptProf_mosaic_pattern"))
      FORC4 {
	fscanf (ifp, "%d", &i);
	if (i == 1) frot = c ^ (c >> 1);
      }
    if (!strcmp(data,"ImgProf_rotation_angle")) {
      fscanf (ifp, "%d", &i);
      flip = i - flip;
    }
    if (!strcmp(data,"NeutObj_neutrals")) {
      FORC4 fscanf (ifp, "%d", neut+c);
      FORC3 cam_mul[c] = (float) neut[0] / neut[c+1];
    }
    parse_mos (from);
    fseek (ifp, skip+from, SEEK_SET);
  }
  if (planes)
    filters = (planes == 1) * 0x01010101 *
	(uchar) "\x94\x61\x16\x49"[(flip/90 + frot) & 3];
}

int CLASS parse_tiff_ifd (int base, int level)
{
  unsigned entries, tag, type, len, plen=16, save;
  int ifd, use_cm=0, cfa, i, j, c, ima_len=0;
  char software[64], *cbuf, *cp;
  uchar cfa_pat[16], cfa_pc[] = { 0,1,2,3 }, tab[256];
  double dblack, cc[4][4], cm[4][3], cam_xyz[4][3];
  double ab[]={ 1,1,1,1 }, asn[] = { 0,0,0,0 }, xyz[] = { 1,1,1 };
  unsigned *buf, sony_offset=0, sony_length=0, sony_key=0;
  struct jhead jh;
  FILE *sfp;

  if (tiff_nifds >= sizeof tiff_ifd / sizeof tiff_ifd[0])
    return 1;
  ifd = tiff_nifds++;
  for (j=0; j < 4; j++)
    for (i=0; i < 4; i++)
      cc[j][i] = i == j;
  entries = get2();
  if (entries > 512) return 1;
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    switch (tag) {
      case 17: case 18:
	if (type == 3 && len == 1)
	  cam_mul[(tag-17)*2] = get2() / 256.0;
	break;
      case 36: case 37: case 38:
	cam_mul[tag-0x24] = get2();
	break;
      case 39:
	if (len < 50 || cam_mul[0]) break;
	fseek (ifp, 12, SEEK_CUR);
	FORC3 cam_mul[c] = get2();
	break;
      case 2: case 256:			/* ImageWidth */
	tiff_ifd[ifd].width = getint(type);
	break;
      case 3: case 257:			/* ImageHeight */
	tiff_ifd[ifd].height = getint(type);
	break;
      case 258:				/* BitsPerSample */
	tiff_ifd[ifd].samples = len;
	tiff_ifd[ifd].bps = get2();
	break;
      case 259:				/* Compression */
	tiff_ifd[ifd].comp = get2();
	break;
      case 262:				/* PhotometricInterpretation */
	tiff_ifd[ifd].phint = get2();
	break;
      case 271:				/* Make */
	fgets (make, 64, ifp);
	break;
      case 272:				/* Model */
	fgets (model, 64, ifp);
	break;
      case 273:				/* StripOffset */
      case 513:
	tiff_ifd[ifd].offset = get4()+base;
	if (!tiff_ifd[ifd].width) {
	  fseek (ifp, tiff_ifd[ifd].offset, SEEK_SET);
	  if (ljpeg_start (&jh, 1)) {
	    tiff_ifd[ifd].comp    = 6;
	    tiff_ifd[ifd].width   = jh.wide << (jh.clrs == 2);
	    tiff_ifd[ifd].height  = jh.high;
	    tiff_ifd[ifd].bps     = jh.bits;
	    tiff_ifd[ifd].samples = jh.clrs;
	  }
	}
	break;
      case 274:				/* Orientation */
	tiff_ifd[ifd].flip = "50132467"[get2() & 7]-'0';
	break;
      case 277:				/* SamplesPerPixel */
	tiff_ifd[ifd].samples = getint(type);
	break;
      case 279:				/* StripByteCounts */
      case 514:
	tiff_ifd[ifd].bytes = get4();
	break;
      case 305:				/* Software */
	fgets (software, 64, ifp);
	if (!strncmp(software,"Adobe",5) ||
	    !strncmp(software,"Bibble",6) ||
	    !strcmp (software,"Digital Photo Professional"))
	  is_raw = 0;
	break;
      case 306:				/* DateTime */
	get_timestamp(0);
	break;
      case 324:				/* TileOffsets */
	tiff_ifd[ifd].offset = level ? ftell(ifp) : get4();
	break;
      case 330:				/* SubIFDs */
	while (len--) {
	  i = ftell(ifp);
	  fseek (ifp, get4()+base, SEEK_SET);
	  if (parse_tiff_ifd (base, level+1)) break;
	  fseek (ifp, i+4, SEEK_SET);
	}
	break;
      case 400:
	strcpy (make, "Sarnoff");
	maximum = 0xfff;
	break;
      case 29184: sony_offset = get4();  break;
      case 29185: sony_length = get4();  break;
      case 29217: sony_key    = get4();  break;
      case 29443:
	FORC4 cam_mul[c ^ (c < 2)] = get2();
	break;
      case 33405:			/* Model2 */
	fgets (model2, 64, ifp);
	break;
      case 33422:			/* CFAPattern */
      case 64777:			/* Kodak P-series */
	if ((plen=len) > 16) plen = 16;
	fread (cfa_pat, 1, plen, ifp);
	for (colors=cfa=i=0; i < plen; i++) {
	  colors += !(cfa & (1 << cfa_pat[i]));
	  cfa |= 1 << cfa_pat[i];
	}
	if (cfa == 070) memcpy (cfa_pc,"\003\004\005",3);	/* CMY */
	if (cfa == 072) memcpy (cfa_pc,"\005\003\004\001",4);	/* GMCY */
	goto guess_cfa_pc;
      case 33434:			/* ExposureTime */
	shutter = getrat();
	break;
      case 33437:			/* FNumber */
	aperture = getrat();
	break;
      case 34310:			/* Leaf metadata */
	parse_mos (ftell(ifp));
	break;
      case 34665:			/* EXIF tag */
	fseek (ifp, get4()+base, SEEK_SET);
	parse_exif (base);
	break;
      case 34675:			/* InterColorProfile */
      case 50831:			/* AsShotICCProfile */
	profile_offset = ftell(ifp);
	profile_length = len;
	break;
      case 37122:			/* CompressedBitsPerPixel */
	kodak_cbpp = get4();
	break;
      case 37386:			/* FocalLength */
	focal_len = getrat();
	break;
      case 37393:			/* ImageNumber */
	shot_order = getint(type);
	break;
      case 37400:			/* old Kodak KDC tag */
	for (raw_color = i=0; i < 3; i++) {
	  getrat();
	  FORC3 rgb_cam[i][c] = getrat();
	}
	break;
      case 46275:			/* Imacon tags */
	strcpy (make, "Imacon");
	data_offset = ftell(ifp);
	ima_len = len;
	break;
      case 46279:
	fseek (ifp, 78, SEEK_CUR);
	raw_width  = get4();
	raw_height = get4();
	left_margin = get4() & 7;
	width = raw_width - left_margin - (get4() & 7);
	top_margin = get4() & 7;
	height = raw_height - top_margin - (get4() & 7);
	fseek (ifp, 178, SEEK_CUR);
	flip = (get2() >> 7) * 90;
	if (width * height * 6 == ima_len) {
	  if (flip % 180 == 90) SWAP(width,height);
	  filters = flip = 0;
	}
	break;
      case 50454:			/* Sinar tag */
      case 50455:
	if (!(cbuf = malloc(len))) break;
	fread (cbuf, 1, len, ifp);
	for (cp = cbuf-1; cp && cp < cbuf+len; cp = strchr(cp,'\n'))
	  if (!strncmp (++cp,"Neutral ",8))
	    sscanf (cp+8, "%f %f %f", cam_mul, cam_mul+1, cam_mul+2);
	free (cbuf);
	break;
      case 50706:			/* DNGVersion */
	FORC4 dng_version = (dng_version << 8) + fgetc(ifp);
	break;
      case 50710:			/* CFAPlaneColor */
	if (len > 4) len = 4;
	colors = len;
	fread (cfa_pc, 1, colors, ifp);
guess_cfa_pc:
	FORCC tab[cfa_pc[c]] = c;
	cdesc[c] = 0;
	for (i=16; i--; )
	  filters = filters << 2 | tab[cfa_pat[i % plen]];
	break;
      case 50711:			/* CFALayout */
	if (get2() == 2) {
	  fuji_width = 1;
	  filters = 0x49494949;
	}
	break;
      case 291:
      case 2317:
      case 50712:			/* LinearizationTable */
	if (len > 0x1000)
	    len = 0x1000;
	read_shorts (curve, len);
	for (i=len; i < 0x1000; i++)
	  curve[i] = curve[i-1];
	maximum = curve[0xfff];
	break;
      case 50714:			/* BlackLevel */
      case 50715:			/* BlackLevelDeltaH */
      case 50716:			/* BlackLevelDeltaV */
	for (dblack=i=0; i < len; i++)
	  dblack += getreal(type);
	black += dblack/len + 0.5;
	break;
      case 50717:			/* WhiteLevel */
	maximum = getint(type);
	break;
      case 50718:			/* DefaultScale */
	i  = get4();
	j  = get4() * get4();
	i *= get4();
	if (i > j) xmag = i / j;
	else	   ymag = j / i;
	break;
      case 50721:			/* ColorMatrix1 */
      case 50722:			/* ColorMatrix2 */
	FORCC for (j=0; j < 3; j++)
	  cm[c][j] = getrat();
	use_cm = 1;
	break;
      case 50723:			/* CameraCalibration1 */
      case 50724:			/* CameraCalibration2 */
	for (i=0; i < colors; i++)
	  FORCC cc[i][c] = getrat();
      case 50727:			/* AnalogBalance */
	FORCC ab[c] = getrat();
	break;
      case 50728:			/* AsShotNeutral */
	FORCC asn[c] = getreal(type);
	break;
      case 50729:			/* AsShotWhiteXY */
	xyz[0] = getrat();
	xyz[1] = getrat();
	xyz[2] = 1 - xyz[0] - xyz[1];
	FORC3 xyz[c] /= d65_white[c];
	break;
      case 50740:			/* DNGPrivateData */
	if (dng_version) break;
	fseek (ifp, get4()+base, SEEK_SET);
	parse_tiff_ifd (base, level+1);
	break;
      case 50752:
	read_shorts (cr2_slice, 3);
	break;
      case 50829:			/* ActiveArea */
	top_margin = getint(type);
	left_margin = getint(type);
	height = getint(type) - top_margin;
	width = getint(type) - left_margin;
	break;
      case 64772:			/* Kodak P-series */
	fseek (ifp, 16, SEEK_CUR);
	data_offset = get4();
	fseek (ifp, 28, SEEK_CUR);
	data_offset += get4();
	load_raw = packed_12_load_raw;
    }
    fseek (ifp, save, SEEK_SET);
  }
  if (sony_length && (buf = malloc(sony_length))) {
    fseek (ifp, sony_offset, SEEK_SET);
    fread (buf, sony_length, 1, ifp);
    sony_decrypt (buf, sony_length/4, 1, sony_key);
    sfp = ifp;
    if ((ifp = tmpfile())) {
      fwrite (buf, sony_length, 1, ifp);
      fseek (ifp, 0, SEEK_SET);
      parse_tiff_ifd (-sony_offset, level);
      fclose (ifp);
    }
    ifp = sfp;
    free (buf);
  }
  for (i=0; i < colors; i++)
    FORCC cc[i][c] *= ab[i];
  if (use_cm) {
    FORCC for (i=0; i < 3; i++)
      for (cam_xyz[c][i]=j=0; j < colors; j++)
	cam_xyz[c][i] += cc[c][j] * cm[j][i] * xyz[i];
    cam_xyz_coeff (cam_xyz);
  }
  if (asn[0])
    FORCC pre_mul[c] = 1 / asn[c];
  if (!use_cm)
    FORCC pre_mul[c] /= cc[c][c];
  return 0;
}

void CLASS parse_tiff (int base)
{
  int doff, max_samp=0, raw=-1, thm=-1, i;
  struct jhead jh;

  fseek (ifp, base, SEEK_SET);
  order = get2();
  if (order != 0x4949 && order != 0x4d4d) return;
  get2();
  memset (tiff_ifd, 0, sizeof tiff_ifd);
  tiff_nifds = 0;
  while ((doff = get4())) {
    fseek (ifp, doff+base, SEEK_SET);
    if (parse_tiff_ifd (base, 0)) break;
  }
  if (!dng_version && !strncmp(make,"Kodak",5)) {
    fseek (ifp, 12+base, SEEK_SET);
    parse_tiff_ifd (base, 2);
  }
  thumb_misc = 16;
  if (thumb_offset) {
    fseek (ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1)) {
      thumb_misc   = jh.bits;
      thumb_width  = jh.wide;
      thumb_height = jh.high;
    }
  }
  for (i=0; i < tiff_nifds; i++) {
    if (max_samp < tiff_ifd[i].samples)
	max_samp = tiff_ifd[i].samples;
    if ((tiff_ifd[i].comp != 6 || tiff_ifd[i].samples != 3) &&
	tiff_ifd[i].width*tiff_ifd[i].height > raw_width*raw_height) {
      raw_width     = tiff_ifd[i].width;
      raw_height    = tiff_ifd[i].height;
      tiff_bps      = tiff_ifd[i].bps;
      tiff_compress = tiff_ifd[i].comp;
      data_offset   = tiff_ifd[i].offset;
      tiff_flip     = tiff_ifd[i].flip;
      tiff_samples  = tiff_ifd[i].samples;
      fuji_secondary = tiff_samples == 2;
      raw = i;
    }
  }
  fuji_width *= (raw_width+1)/2;
  if (tiff_ifd[0].flip) tiff_flip = tiff_ifd[0].flip;
  if (raw >= 0) {
    if (tiff_compress < 2)
      load_raw = tiff_bps > 8 ? unpacked_load_raw : eight_bit_load_raw;
    if (tiff_compress/2 == 3)
      load_raw = lossless_jpeg_load_raw;
    if (tiff_compress == 32773)
      load_raw = packed_12_load_raw;
    if (tiff_compress == 65000)
      switch (tiff_ifd[raw].phint) {
	case     2: load_raw = kodak_rgb_load_raw;   filters = 0;  break;
	case     6: load_raw = kodak_ycbcr_load_raw; filters = 0;  break;
	case 32803: load_raw = kodak_65000_load_raw;
      }
  }
  if (tiff_samples == 3 && tiff_bps == 8)
    if (!dng_version) is_raw = 0;
  for (i=0; i < tiff_nifds; i++)
    if (i != raw && tiff_ifd[i].samples == max_samp &&
	tiff_ifd[i].width * tiff_ifd[i].height / SQR(tiff_ifd[i].bps+1) >
	      thumb_width *       thumb_height / SQR(thumb_misc+1)) {
      thumb_width  = tiff_ifd[i].width;
      thumb_height = tiff_ifd[i].height;
      thumb_offset = tiff_ifd[i].offset;
      thumb_length = tiff_ifd[i].bytes;
      thumb_misc   = tiff_ifd[i].bps;
      thm = i;
    }
  if (thm >= 0) {
    thumb_misc |= tiff_ifd[thm].samples << 5;
    switch (tiff_ifd[thm].comp) {
      case 0:
	write_thumb = layer_thumb;
	break;
      case 1:
	if (tiff_ifd[thm].bps > 8)
	  thumb_load_raw = kodak_thumb_load_raw;
	else
	  write_thumb = ppm_thumb;
	break;
      case 65000:
	thumb_load_raw = tiff_ifd[thm].phint == 6 ?
		kodak_ycbcr_load_raw : kodak_rgb_load_raw;
    }
  }
}

void CLASS parse_minolta()
{
  int save, tag, len, offset, high=0, wide=0, i, c;

  fseek (ifp, 4, SEEK_SET);
  offset = get4() + 8;
  while ((save=ftell(ifp)) < offset) {
    tag = get4();
    len = get4();
    switch (tag) {
      case 0x505244:				/* PRD */
	fseek (ifp, 8, SEEK_CUR);
	high = get2();
	wide = get2();
	break;
      case 0x574247:				/* WBG */
	get4();
	i = strstr(model,"A200") ? 3:0;
	FORC4 cam_mul[c ^ (c >> 1) ^ i] = get2();
	break;
      case 0x545457:				/* TTW */
	parse_tiff (ftell(ifp));
    }
    fseek (ifp, save+len+8, SEEK_SET);
  }
  raw_height = high;
  raw_width  = wide;
  data_offset = offset;
}

/*
   Many cameras have a "debug mode" that writes JPEG and raw
   at the same time.  The raw file has no header, so try to
   to open the matching JPEG file and read its metadata.
 */
void CLASS parse_external_jpeg()
{
  char *file, *ext, *jname, *jfile, *jext;
  FILE *save=ifp;

  ext  = strrchr (ifname, '.');
  file = strrchr (ifname, '/');
  if (!file) file = strrchr (ifname, '\\');
  if (!file) file = ifname-1;
  file++;
  if (!ext || strlen(ext) != 4 || ext-file != 8) return;
  jname = malloc (strlen(ifname) + 1);
  merror (jname, "parse_external()");
  strcpy (jname, ifname);
  jfile = file - ifname + jname;
  jext  = ext  - ifname + jname;
  if (strcasecmp (ext, ".jpg")) {
    strcpy (jext, isupper(ext[1]) ? ".JPG":".jpg");
    memcpy (jfile, file+4, 4);
    memcpy (jfile+4, file, 4);
  } else
    while (isdigit(*--jext)) {
      if (*jext != '9') {
        (*jext)++;
	break;
      }
      *jext = '0';
    }
  if (strcmp (jname, ifname)) {
    if ((ifp = fopen (jname, "rb"))) {
      if (verbose)
	fprintf (stderr, "Reading metadata from %s...\n", jname);
      parse_tiff (12);
      thumb_offset = 0;
      is_raw = 1;
      fclose (ifp);
    }
  }
  if (!timestamp)
    fprintf (stderr, "Failed to read metadata from %s\n", jname);
  free (jname);
  ifp = save;
}

/*
   CIFF block 0x1030 contains an 8x8 white sample.
   Load this into white[][] for use in scale_colors().
 */
void CLASS ciff_block_1030()
{
  static const ushort key[] = { 0x410, 0x45f3 };
  int i, bpp, row, col, vbits=0;
  unsigned long bitbuf=0;

  if ((get2(),get4()) != 0x80008 || !get4()) return;
  bpp = get2();
  if (bpp != 10 && bpp != 12) return;
  for (i=row=0; row < 8; row++)
    for (col=0; col < 8; col++) {
      if (vbits < bpp) {
	bitbuf = bitbuf << 16 | (get2() ^ key[i++ & 1]);
	vbits += 16;
      }
      white[row][col] =
	bitbuf << (LONG_BIT - vbits) >> (LONG_BIT - bpp);
      vbits -= bpp;
    }
}

/*
   Parse a CIFF file, better known as Canon CRW format.
 */
void CLASS parse_ciff (int offset, int length)
{
  int tboff, nrecs, i, c, type, len, save, wbi=-1;
  ushort key[] = { 0x410, 0x45f3 };

  fseek (ifp, offset+length-4, SEEK_SET);
  tboff = get4() + offset;
  fseek (ifp, tboff, SEEK_SET);
  nrecs = get2();
  if (nrecs > 100) return;
  for (i = 0; i < nrecs; i++) {
    type = get2();
    len  = get4();
    save = ftell(ifp) + 4;
    fseek (ifp, offset+get4(), SEEK_SET);
    if ((((type >> 8) + 8) | 8) == 0x38)
      parse_ciff (ftell(ifp), len);	/* Parse a sub-table */

    if (type == 0x080a) {
      fread (make, 64, 1, ifp);
      fseek (ifp, strlen(make) - 63, SEEK_CUR);
      fread (model, 64, 1, ifp);
    }
    if (type == 0x1810) {
      fseek (ifp, 12, SEEK_CUR);
      flip = get4();
    }
    if (type == 0x1835)			/* Get the decoder table */
      tiff_compress = get4();
    if (type == 0x2007) {
      thumb_offset = ftell(ifp);
      thumb_length = len;
    }
    if (type == 0x1818) {
      shutter = pow (2, -int_to_float((get4(),get4())));
      aperture = pow (2, int_to_float(get4())/2);
    }
    if (type == 0x102a) {
      iso_speed = pow (2, (get4(),get2())/32.0 - 4) * 50;
      aperture  = pow (2, (get2(),get2())/64.0);
      shutter   = pow (2,-((short)get2())/32.0);
      wbi = (get2(),get2());
      if (wbi > 17) wbi = 0;
    }
    if (type == 0x102c) {
      if (get2() > 512) {		/* Pro90, G1 */
	fseek (ifp, 118, SEEK_CUR);
	FORC4 cam_mul[c ^ 2] = get2();
      } else {				/* G2, S30, S40 */
	fseek (ifp, 98, SEEK_CUR);
	FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get2();
      }
    }
    if (type == 0x0032) {
      if (len == 768) {			/* EOS D30 */
	fseek (ifp, 72, SEEK_CUR);
	FORC4 cam_mul[c ^ (c >> 1)] = 1024.0 / get2();
	if (!wbi) cam_mul[0] = -1;	/* use my auto white balance */
      } else if (!cam_mul[0]) {
	if (get2() == key[0])		/* Pro1, G6, S60, S70 */
	  c = (strstr(model,"Pro1") ?
	      "012346000000000000":"01345:000000006008")[wbi]-'0'+ 2;
	else {				/* G3, G5, S45, S50 */
	  c = "023457000000006000"[wbi]-'0';
	  key[0] = key[1] = 0;
	}
	fseek (ifp, 78 + c*8, SEEK_CUR);
	FORC4 cam_mul[c ^ (c >> 1) ^ 1] = get2() ^ key[c & 1];
	if (!wbi) cam_mul[0] = -1;
      }
    }
    if (type == 0x10a9) {		/* D60, 10D, 300D, and clones */
      if (len > 66) wbi = "0134567028"[wbi]-'0';
      fseek (ifp, 2 + wbi*8, SEEK_CUR);
      FORC4 cam_mul[c ^ (c >> 1)] = get2();
    }
    if (type == 0x1030 && (0x18040 >> wbi & 1))
      ciff_block_1030();		/* all that don't have 0x10a9 */
    if (type == 0x1031) {
      raw_width = (get2(),get2());
      raw_height = get2();
    }
    if (type == 0x5813) flash_used = int_to_float(len);
    if (type == 0x5814) canon_ev   = int_to_float(len);
    if (type == 0x5817) shot_order = len;
    if (type == 0x5834) unique_id  = len;
    if (type == 0x580e) timestamp  = len;
    if (type == 0x180e) timestamp  = get4();
#ifdef LOCALTIME
    if ((type | 0x4000) == 0x580e)
      timestamp = mktime (gmtime (&timestamp));
#endif
    fseek (ifp, save, SEEK_SET);
  }
}

void CLASS parse_rollei()
{
  char line[128], *val;
  struct tm t;

  fseek (ifp, 0, SEEK_SET);
  do {
    fgets (line, 128, ifp);
    if ((val = strchr(line,'=')))
      *val++ = 0;
    else
      val = line + strlen(line);
    if (!strcmp(line,"DAT"))
      sscanf (val, "%d.%d.%d", &t.tm_mday, &t.tm_mon, &t.tm_year);
    if (!strcmp(line,"TIM"))
      sscanf (val, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
    if (!strcmp(line,"HDR"))
      thumb_offset = atoi(val);
    if (!strcmp(line,"X  "))
      raw_width = atoi(val);
    if (!strcmp(line,"Y  "))
      raw_height = atoi(val);
    if (!strcmp(line,"TX "))
      thumb_width = atoi(val);
    if (!strcmp(line,"TY "))
      thumb_height = atoi(val);
  } while (strncmp(line,"EOHD",4));
  data_offset = thumb_offset + thumb_width * thumb_height * 2;
  t.tm_year -= 1900;
  t.tm_mon -= 1;
  if (mktime(&t) > 0)
    timestamp = mktime(&t);
  strcpy (make, "Rollei");
  strcpy (model,"d530flex");
  write_thumb = rollei_thumb;
}

void CLASS parse_phase_one (int base)
{
  unsigned entries, tag, type, len, data, save, i, c;
  float romm_cam[3][3];
  char *cp;

  memset (&ph1, 0, sizeof ph1);
  fseek (ifp, base, SEEK_SET);
  order = get4() & 0xffff;
  if (get4() >> 8 != 0x526177) return;		/* "Raw" */
  fseek (ifp, base+get4(), SEEK_SET);
  entries = get4();
  get4();
  while (entries--) {
    tag  = get4();
    type = get4();
    len  = get4();
    data = get4();
    save = ftell(ifp);
    fseek (ifp, base+data, SEEK_SET);
    switch (tag) {
      case 0x100:  flip = "0653"[data & 3]-'0';  break;
      case 0x106:
	for (i=0; i < 9; i++)
	  romm_cam[0][i] = getreal(11);
	romm_coeff (romm_cam);
	break;
      case 0x107:
	FORC3 cam_mul[c] = pre_mul[c] = getreal(11);
	break;
      case 0x108:  raw_width     = data;	break;
      case 0x109:  raw_height    = data;	break;
      case 0x10a:  left_margin   = data;	break;
      case 0x10b:  top_margin    = data;	break;
      case 0x10c:  width         = data;	break;
      case 0x10d:  height        = data;	break;
      case 0x10e:  ph1.format    = data;	break;
      case 0x10f:  data_offset   = data+base;	break;
      case 0x110:  meta_offset   = data+base;
		   meta_length   = len;			break;
      case 0x112:  ph1.key_off   = save - 4;		break;
      case 0x210:  ph1.tag_210   = int_to_float(data);	break;
      case 0x21a:  ph1.tag_21a   = data;		break;
      case 0x21c:  strip_offset  = data+base;		break;
      case 0x21d:  ph1.black     = data;		break;
      case 0x222:  ph1.split_col = data - left_margin;	break;
      case 0x223:  ph1.black_off = data+base;		break;
      case 0x301:
	model[63] = 0;
	fread (model, 1, 63, ifp);
	if ((cp = strstr(model," camera"))) *cp = 0;
    }
    fseek (ifp, save, SEEK_SET);
  }
  load_raw = ph1.format < 3 ?
	phase_one_load_raw : phase_one_load_raw_c;
  maximum = 0xffff;
  strcpy (make, "Phase One");
  if (model[0]) return;
  switch (raw_height) {
    case 2060: strcpy (model,"LightPhase");	break;
    case 2682: strcpy (model,"H 10");		break;
    case 4128: strcpy (model,"H 20");		break;
    case 5488: strcpy (model,"H 25");		break;
  }
}

void CLASS parse_fuji (int offset)
{
  unsigned entries, tag, len, save, c;

  fseek (ifp, offset, SEEK_SET);
  entries = get4();
  if (entries > 255) return;
  while (entries--) {
    tag = get2();
    len = get2();
    save = ftell(ifp);
    if (tag == 0x100) {
      raw_height = get2();
      raw_width  = get2();
    } else if (tag == 0x121) {
      height = get2();
      if ((width = get2()) == 4284) width += 3;
    } else if (tag == 0x130)
      fuji_layout = fgetc(ifp) >> 7;
    if (tag == 0x2ff0)
      FORC4 cam_mul[c ^ 1] = get2();
    fseek (ifp, save+len, SEEK_SET);
  }
  if (fuji_layout) {
    height *= 2;
    width  /= 2;
  }
}

int CLASS parse_jpeg (int offset)
{
  int len, save, hlen, mark;

  fseek (ifp, offset, SEEK_SET);
  if (fgetc(ifp) != 0xff || fgetc(ifp) != 0xd8) return 0;

  while (fgetc(ifp) == 0xff && (mark = fgetc(ifp)) != 0xda) {
    order = 0x4d4d;
    len   = get2() - 2;
    save  = ftell(ifp);
    if (mark == 0xc0 || mark == 0xc3) {
      fgetc(ifp);
      raw_height = get2();
      raw_width  = get2();
    }
    order = get2();
    hlen  = get4();
    if (get4() == 0x48454150)		/* "HEAP" */
      parse_ciff (save+hlen, len-hlen);
    parse_tiff (save+6);
    fseek (ifp, save+len, SEEK_SET);
  }
  return 1;
}

void CLASS parse_riff()
{
  unsigned i, size, end;
  char tag[4], date[64], month[64];
  static const char mon[12][4] =
  { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
  struct tm t;

  order = 0x4949;
  fread (tag, 4, 1, ifp);
  size = get4();
  if (!memcmp(tag,"RIFF",4) || !memcmp(tag,"LIST",4)) {
    end = ftell(ifp) + size;
    get4();
    while (ftell(ifp) < end)
      parse_riff();
  } else if (!memcmp(tag,"IDIT",4) && size < 64) {
    fread (date, 64, 1, ifp);
    date[size] = 0;
    if (sscanf (date, "%*s %s %d %d:%d:%d %d", month, &t.tm_mday,
	&t.tm_hour, &t.tm_min, &t.tm_sec, &t.tm_year) == 6) {
      for (i=0; i < 12 && strcmp(mon[i],month); i++);
      t.tm_mon = i;
      t.tm_year -= 1900;
      if (mktime(&t) > 0)
	timestamp = mktime(&t);
    }
  } else
    fseek (ifp, size, SEEK_CUR);
}

void CLASS parse_smal (int offset, int fsize)
{
  int ver;

  fseek (ifp, offset+2, SEEK_SET);
  order = 0x4949;
  ver = fgetc(ifp);
  if (ver == 6)
    fseek (ifp, 5, SEEK_CUR);
  if (get4() != fsize) return;
  if (ver > 6) data_offset = get4();
  raw_height = height = get2();
  raw_width  = width  = get2();
  strcpy (make, "SMaL");
  sprintf (model, "v%d %dx%d", ver, width, height);
  if (ver == 6) load_raw = smal_v6_load_raw;
  if (ver == 9) load_raw = smal_v9_load_raw;
}

char * CLASS foveon_gets (int offset, char *str, int len)
{
  int i;
  fseek (ifp, offset, SEEK_SET);
  for (i=0; i < len-1; i++)
    if ((str[i] = get2()) == 0) break;
  str[i] = 0;
  return str;
}

void CLASS parse_foveon()
{
  int entries, img=0, off, len, tag, save, i, wide, high, pent, poff[256][2];
  char name[64], value[64];

  order = 0x4949;			/* Little-endian */
  fseek (ifp, 36, SEEK_SET);
  flip = get4();
  fseek (ifp, -4, SEEK_END);
  fseek (ifp, get4(), SEEK_SET);
  if (get4() != 0x64434553) return;	/* SECd */
  entries = (get4(),get4());
  while (entries--) {
    off = get4();
    len = get4();
    tag = get4();
    save = ftell(ifp);
    fseek (ifp, off, SEEK_SET);
    if (get4() != (0x20434553 | (tag << 24))) return;
    switch (tag) {
      case 0x47414d49:			/* IMAG */
      case 0x32414d49:			/* IMA2 */
	fseek (ifp, 12, SEEK_CUR);
	wide = get4();
	high = get4();
	if (wide > raw_width && high > raw_height) {
	  raw_width  = wide;
	  raw_height = high;
	  data_offset = off+24;
	}
	fseek (ifp, off+28, SEEK_SET);
	if (fgetc(ifp) == 0xff && fgetc(ifp) == 0xd8) {
	  thumb_offset = off+28;
	  thumb_length = len-28;
	}
	if (++img == 2 && !thumb_length) {
	  thumb_offset = off+24;
	  thumb_width = wide;
	  thumb_height = high;
	  write_thumb = foveon_thumb;
	}
	break;
      case 0x464d4143:			/* CAMF */
	meta_offset = off+24;
	meta_length = len-28;
	if (meta_length > 0x20000)
	    meta_length = 0x20000;
	break;
      case 0x504f5250:			/* PROP */
	pent = (get4(),get4());
	fseek (ifp, 12, SEEK_CUR);
	off += pent*8 + 24;
	if (pent > 256) pent=256;
	for (i=0; i < pent*2; i++)
	  poff[0][i] = off + get4()*2;
	for (i=0; i < pent; i++) {
	  foveon_gets (poff[i][0], name, 64);
	  foveon_gets (poff[i][1], value, 64);
	  if (!strcmp (name, "ISO"))
	    iso_speed = atoi(value);
	  if (!strcmp (name, "CAMMANUF"))
	    strcpy (make, value);
	  if (!strcmp (name, "CAMMODEL"))
	    strcpy (model, value);
	  if (!strcmp (name, "WB_DESC"))
	    strcpy (model2, value);
	  if (!strcmp (name, "TIME"))
	    timestamp = atoi(value);
	  if (!strcmp (name, "EXPTIME"))
	    shutter = atoi(value) / 1000000.0;
	  if (!strcmp (name, "APERTURE"))
	    aperture = atof(value);
	  if (!strcmp (name, "FLENGTH"))
	    focal_len = atof(value);
	}
#ifdef LOCALTIME
	timestamp = mktime (gmtime (&timestamp));
#endif
    }
    fseek (ifp, save, SEEK_SET);
  }
  is_foveon = 1;
}

/*
   Thanks to Adobe for providing these excellent CAM -> XYZ matrices!
 */
void CLASS adobe_coeff (char *make, char *model)
{
  static const struct {
    const char *prefix;
    short black, trans[12];
  } table[] = {
    { "Canon EOS D2000", 0,
	{ 24542,-10860,-3401,-1490,11370,-297,2858,-605,3225 } },
    { "Canon EOS D6000", 0,
	{ 20482,-7172,-3125,-1033,10410,-285,2542,226,3136 } },
    { "Canon EOS D30", 0,
	{ 9805,-2689,-1312,-5803,13064,3068,-2438,3075,8775 } },
    { "Canon EOS D60", 0,
	{ 6188,-1341,-890,-7168,14489,2937,-2640,3228,8483 } },
    { "Canon EOS 5D", 0,
	{ 6347,-479,-972,-8297,15954,2480,-1968,2131,7649 } },
    { "Canon EOS 20Da", 0,
	{ 14155,-5065,-1382,-6550,14633,2039,-1623,1824,6561 } },
    { "Canon EOS 20D", 0,
	{ 6599,-537,-891,-8071,15783,2424,-1983,2234,7462 } },
    { "Canon EOS 350D", 0,
	{ 6018,-617,-965,-8645,15881,2975,-1530,1719,7642 } },
    { "Canon EOS-1Ds Mark II", 0,
	{ 6517,-602,-867,-8180,15926,2378,-1618,1771,7633 } },
    { "Canon EOS-1D Mark II N", 0,
	{ 6240,-466,-822,-8180,15825,2500,-1801,1938,8042 } },
    { "Canon EOS-1D Mark II", 0,
	{ 6264,-582,-724,-8312,15948,2504,-1744,1919,8664 } },
    { "Canon EOS-1DS", 0,
	{ 4374,3631,-1743,-7520,15212,2472,-2892,3632,8161 } },
    { "Canon EOS-1D", 0,
	{ 6806,-179,-1020,-8097,16415,1687,-3267,4236,7690 } },
    { "Canon EOS", 0,
	{ 8197,-2000,-1118,-6714,14335,2592,-2536,3178,8266 } },
    { "Canon PowerShot A50", 0,
	{ -5300,9846,1776,3436,684,3939,-5540,9879,6200,-1404,11175,217 } },
    { "Canon PowerShot A5", 0,
	{ -4801,9475,1952,2926,1611,4094,-5259,10164,5947,-1554,10883,547 } },
    { "Canon PowerShot G1", 0,
	{ -4778,9467,2172,4743,-1141,4344,-5146,9908,6077,-1566,11051,557 } },
    { "Canon PowerShot G2", 0,
	{ 9087,-2693,-1049,-6715,14382,2537,-2291,2819,7790 } },
    { "Canon PowerShot G3", 0,
	{ 9212,-2781,-1073,-6573,14189,2605,-2300,2844,7664 } },
    { "Canon PowerShot G5", 0,
	{ 9757,-2872,-933,-5972,13861,2301,-1622,2328,7212 } },
    { "Canon PowerShot G6", 0,
	{ 9877,-3775,-871,-7613,14807,3072,-1448,1305,7485 } },
    { "Canon PowerShot Pro1", 0,
	{ 10062,-3522,-999,-7643,15117,2730,-765,817,7323 } },
    { "Canon PowerShot Pro70", 34,
	{ -4155,9818,1529,3939,-25,4522,-5521,9870,6610,-2238,10873,1342 } },
    { "Canon PowerShot Pro90", 0,
	{ -4963,9896,2235,4642,-987,4294,-5162,10011,5859,-1770,11230,577 } },
    { "Canon PowerShot S30", 0,
	{ 10566,-3652,-1129,-6552,14662,2006,-2197,2581,7670 } },
    { "Canon PowerShot S40", 0,
	{ 8510,-2487,-940,-6869,14231,2900,-2318,2829,9013 } },
    { "Canon PowerShot S45", 0,
	{ 8163,-2333,-955,-6682,14174,2751,-2077,2597,8041 } },
    { "Canon PowerShot S50", 0,
	{ 8882,-2571,-863,-6348,14234,2288,-1516,2172,6569 } },
    { "Canon PowerShot S60", 0,
	{ 8795,-2482,-797,-7804,15403,2573,-1422,1996,7082 } },
    { "Canon PowerShot S70", 0,
	{ 9976,-3810,-832,-7115,14463,2906,-901,989,7889 } },
    { "Contax N Digital", 0,
	{ 7777,1285,-1053,-9280,16543,2916,-3677,5679,7060 } },
    { "EPSON R-D1", 0,
	{ 6827,-1878,-732,-8429,16012,2564,-704,592,7145 } },
    { "FUJIFILM FinePix E550", 0,
	{ 11044,-3888,-1120,-7248,15168,2208,-1531,2277,8069 } },
    { "FUJIFILM FinePix E900", 0,
	{ 9183,-2526,-1078,-7461,15071,2574,-2022,2440,8639 } },
    { "FUJIFILM FinePix F8", 0,
	{ 11044,-3888,-1120,-7248,15168,2208,-1531,2277,8069 } },
    { "FUJIFILM FinePix F7", 0,
	{ 10004,-3219,-1201,-7036,15047,2107,-1863,2565,7736 } },
    { "FUJIFILM FinePix S20Pro", 0,
	{ 10004,-3219,-1201,-7036,15047,2107,-1863,2565,7736 } },
    { "FUJIFILM FinePix S2Pro", 128,
	{ 12492,-4690,-1402,-7033,15423,1647,-1507,2111,7697 } },
    { "FUJIFILM FinePix S3Pro", 0,
	{ 11807,-4612,-1294,-8927,16968,1988,-2120,2741,8006 } },
    { "FUJIFILM FinePix S5000", 0,
	{ 8754,-2732,-1019,-7204,15069,2276,-1702,2334,6982 } },
    { "FUJIFILM FinePix S5100", 0,
	{ 11940,-4431,-1255,-6766,14428,2542,-993,1165,7421 } },
    { "FUJIFILM FinePix S5500", 0,
	{ 11940,-4431,-1255,-6766,14428,2542,-993,1165,7421 } },
    { "FUJIFILM FinePix S5200", 0,
	{ 9636,-2804,-988,-7442,15040,2589,-1803,2311,8621 } },
    { "FUJIFILM FinePix S5600", 0,
	{ 9636,-2804,-988,-7442,15040,2589,-1803,2311,8621 } },
    { "FUJIFILM FinePix S7000", 0,
	{ 10190,-3506,-1312,-7153,15051,2238,-2003,2399,7505 } },
    { "FUJIFILM FinePix S9", 0,
	{ 10491,-3423,-1145,-7385,15027,2538,-1809,2275,8692 } },
    { "Imacon Ixpress", 0,	/* DJC */
	{ 7025,-1415,-704,-5188,13765,1424,-1248,2742,6038 } },
    { "KODAK NC2000F", 0,	/* DJC */
	{ 16475,-6903,-1218,-851,10375,477,2505,-7,1020 } },
    { "Kodak DCS315C", 8,
	{ 17523,-4827,-2510,756,8546,-137,6113,1649,2250 } },
    { "Kodak DCS330C", 8,
	{ 20620,-7572,-2801,-103,10073,-396,3551,-233,2220 } },
    { "KODAK DCS420", 0,
	{ 10868,-1852,-644,-1537,11083,484,2343,628,2216 } },
    { "KODAK DCS460", 0,
	{ 10592,-2206,-967,-1944,11685,230,2206,670,1273 } },
    { "KODAK EOSDCS1", 0,
	{ 10592,-2206,-967,-1944,11685,230,2206,670,1273 } },
    { "KODAK EOSDCS3B", 0,
	{ 9898,-2700,-940,-2478,12219,206,1985,634,1031 } },
    { "Kodak DCS520C", 180,
	{ 24542,-10860,-3401,-1490,11370,-297,2858,-605,3225 } },
    { "Kodak DCS560C", 188,
	{ 20482,-7172,-3125,-1033,10410,-285,2542,226,3136 } },
    { "Kodak DCS620C", 180,
	{ 23617,-10175,-3149,-2054,11749,-272,2586,-489,3453 } },
    { "Kodak DCS620X", 185,
	{ 13095,-6231,154,12221,-21,-2137,895,4602,2258 } },
    { "Kodak DCS660C", 214,
	{ 18244,-6351,-2739,-791,11193,-521,3711,-129,2802 } },
    { "Kodak DCS720X", 0,
	{ 11775,-5884,950,9556,1846,-1286,-1019,6221,2728 } },
    { "Kodak DCS760C", 0,
	{ 16623,-6309,-1411,-4344,13923,323,2285,274,2926 } },
    { "Kodak DCS Pro SLR", 0,
	{ 5494,2393,-232,-6427,13850,2846,-1876,3997,5445 } },
    { "Kodak DCS Pro 14nx", 0,
	{ 5494,2393,-232,-6427,13850,2846,-1876,3997,5445 } },
    { "Kodak DCS Pro 14", 0,
	{ 7791,3128,-776,-8588,16458,2039,-2455,4006,6198 } },
    { "Kodak ProBack645", 0,
	{ 16414,-6060,-1470,-3555,13037,473,2545,122,4948 } },
    { "Kodak ProBack", 0,
	{ 21179,-8316,-2918,-915,11019,-165,3477,-180,4210 } },
    { "KODAK P850", 0,
	{ 10511,-3836,-1102,-6946,14587,2558,-1481,1792,6246 } },
    { "KODAK P880", 0,
	{ 12805,-4662,-1376,-7480,15267,2360,-1626,2194,7904 } },
    { "LEICA DIGILUX 2", 0,
	{ 11340,-4069,-1275,-7555,15266,2448,-2960,3426,7685 } },
    { "LEICA D-LUX2", 0,
	{ 10704,-4187,-1230,-8314,15952,2501,-920,945,8927 } },
    { "Leaf Valeo 6", 0,
	{ 3952,2189,449,-6701,14585,2275,-4536,7349,6536 } },
    { "Leaf", 0,
	{ 8236,1746,-1314,-8251,15953,2428,-3673,5786,5771 } },
    { "Minolta DiMAGE 5", 0,
	{ 8983,-2942,-963,-6556,14476,2237,-2426,2887,8014 } },
    { "Minolta DiMAGE 7Hi", 0,
	{ 11368,-3894,-1242,-6521,14358,2339,-2475,3056,7285 } },
    { "Minolta DiMAGE 7", 0,
	{ 9144,-2777,-998,-6676,14556,2281,-2470,3019,7744 } },
    { "Minolta DiMAGE A1", 0,
	{ 9274,-2547,-1167,-8220,16323,1943,-2273,2720,8340 } },
    { "MINOLTA DiMAGE A200", 0,
	{ 8560,-2487,-986,-8112,15535,2771,-1209,1324,7743 } },
    { "Minolta DiMAGE A2", 0,
	{ 9097,-2726,-1053,-8073,15506,2762,-966,981,7763 } },
    { "Minolta DiMAGE Z2", 0,	/* DJC */
	{ 11280,-3564,-1370,-4655,12374,2282,-1423,2168,5396 } },
    { "MINOLTA DYNAX 5", 0,
	{ 10284,-3283,-1086,-7957,15762,2316,-829,882,6644 } },
    { "MINOLTA DYNAX 7", 0,
	{ 10239,-3104,-1099,-8037,15727,2451,-927,925,6871 } },
    { "NIKON D100", 0,
	{ 5902,-933,-782,-8983,16719,2354,-1402,1455,6464 } },
    { "NIKON D1H", 0,
	{ 7577,-2166,-926,-7454,15592,1934,-2377,2808,8606 } },
    { "NIKON D1X", 0,
	{ 7702,-2245,-975,-9114,17242,1875,-2679,3055,8521 } },
    { "NIKON D1", 0,	/* multiplied by 2.218750, 1.0, 1.148438 */
	{ 16772,-4726,-2141,-7611,15713,1972,-2846,3494,9521 } },
    { "NIKON D2H", 0,
	{ 5710,-901,-615,-8594,16617,2024,-2975,4120,6830 } },
    { "NIKON D2X", 0,
	{ 10231,-2769,-1255,-8301,15900,2552,-797,680,7148 } },
    { "NIKON D50", 0,
	{ 7732,-2422,-789,-8238,15884,2498,-859,783,7330 } },
    { "NIKON D70", 0,
	{ 7732,-2422,-789,-8238,15884,2498,-859,783,7330 } },
    { "NIKON D200", 0,
	{ 8367,-2248,-763,-8758,16447,2422,-1527,1550,8053 } },
    { "NIKON E950", 0,		/* DJC */
	{ -3746,10611,1665,9621,-1734,2114,-2389,7082,3064,3406,6116,-244 } },
    { "NIKON E995", 0,	/* copied from E5000 */
	{ -5547,11762,2189,5814,-558,3342,-4924,9840,5949,688,9083,96 } },
    { "NIKON E2500", 0,
	{ -5547,11762,2189,5814,-558,3342,-4924,9840,5949,688,9083,96 } },
    { "NIKON E4300", 0, /* copied from Minolta DiMAGE Z2 */
	{ 11280,-3564,-1370,-4655,12374,2282,-1423,2168,5396 } },
    { "NIKON E4500", 0,
	{ -5547,11762,2189,5814,-558,3342,-4924,9840,5949,688,9083,96 } },
    { "NIKON E5000", 0,
	{ -5547,11762,2189,5814,-558,3342,-4924,9840,5949,688,9083,96 } },
    { "NIKON E5400", 0,
	{ 9349,-2987,-1001,-7919,15766,2266,-2098,2680,6839 } },
    { "NIKON E5700", 0,
	{ -5368,11478,2368,5537,-113,3148,-4969,10021,5782,778,9028,211 } },
    { "NIKON E8400", 0,
	{ 7842,-2320,-992,-8154,15718,2599,-1098,1342,7560 } },
    { "NIKON E8700", 0,
	{ 8489,-2583,-1036,-8051,15583,2643,-1307,1407,7354 } },
    { "NIKON E8800", 0,
	{ 7971,-2314,-913,-8451,15762,2894,-1442,1520,7610 } },
    { "OLYMPUS C5050", 0,
	{ 10508,-3124,-1273,-6079,14294,1901,-1653,2306,6237 } },
    { "OLYMPUS C5060", 0,
	{ 10445,-3362,-1307,-7662,15690,2058,-1135,1176,7602 } },
    { "OLYMPUS C7070", 0,
	{ 10252,-3531,-1095,-7114,14850,2436,-1451,1723,6365 } },
    { "OLYMPUS C70", 0,
	{ 10793,-3791,-1146,-7498,15177,2488,-1390,1577,7321 } },
    { "OLYMPUS C80", 0,
	{ 8606,-2509,-1014,-8238,15714,2703,-942,979,7760 } },
    { "OLYMPUS E-10", 0,
	{ 12745,-4500,-1416,-6062,14542,1580,-1934,2256,6603 } },
    { "OLYMPUS E-1", 0,
	{ 11846,-4767,-945,-7027,15878,1089,-2699,4122,8311 } },
    { "OLYMPUS E-20", 0,
	{ 13173,-4732,-1499,-5807,14036,1895,-2045,2452,7142 } },
    { "OLYMPUS E-300", 0,
	{ 7828,-1761,-348,-5788,14071,1830,-2853,4518,6557 } },
    { "OLYMPUS E-500", 0,
	{ 8136,-1968,-299,-5481,13742,1871,-2556,4205,6630 } },
    { "OLYMPUS SP350", 0,
	{ 12078,-4836,-1069,-6671,14306,2578,-786,939,7418 } },
    { "OLYMPUS SP500UZ", 0,
	{ 9493,-3415,-666,-5211,12334,3260,-1548,2262,6482 } },
    { "PENTAX *ist DL", 0,
	{ 10829,-2838,-1115,-8339,15817,2696,-837,680,11939 } },
    { "PENTAX *ist DS2", 0,
	{ 10504,-2438,-1189,-8603,16207,2531,-1022,863,12242 } },
    { "PENTAX *ist DS", 0,
	{ 10371,-2333,-1206,-8688,16231,2602,-1230,1116,11282 } },
    { "PENTAX *ist D", 0,
	{ 9651,-2059,-1189,-8881,16512,2487,-1460,1345,10687 } },
    { "Panasonic DMC-FZ30", 0,
	{ 10976,-4029,-1141,-7918,15491,2600,-1670,2071,8246 } },
    { "Panasonic DMC-LC1", 0,
	{ 11340,-4069,-1275,-7555,15266,2448,-2960,3426,7685 } },
    { "Panasonic DMC-LX1", 0,
	{ 10704,-4187,-1230,-8314,15952,2501,-920,945,8927 } },
    { "Sinar", 0,		/* DJC */
	{ 16442,-2956,-2422,-2877,12128,750,-1136,6066,4559 } },
    { "SONY DSC-F828", 491,
	{ 7924,-1910,-777,-8226,15459,2998,-1517,2199,6818,-7242,11401,3481 } },
    { "SONY DSC-R1", 512,
	{ 8512,-2641,-694,-8042,15670,2526,-1821,2117,7414 } },
    { "SONY DSC-V3", 0,
	{ 7511,-2571,-692,-7894,15088,3060,-948,1111,8128 } }
  };
  double cam_xyz[4][3];
  char name[130];
  int i, j;

  sprintf (name, "%s %s", make, model);
  for (i=0; i < sizeof table / sizeof *table; i++)
    if (!strncmp (name, table[i].prefix, strlen(table[i].prefix))) {
      if (table[i].black)
	black = table[i].black;
      for (j=0; j < 12; j++)
	cam_xyz[0][j] = table[i].trans[j] / 10000.0;
      cam_xyz_coeff (cam_xyz);
      break;
    }
}

void CLASS simple_coeff (int index)
{
  static const float table[][12] = {
  /* index 0 -- all Foveon cameras */
  { 1.4032,-0.2231,-0.1016,-0.5263,1.4816,0.017,-0.0112,0.0183,0.9113 },
  /* index 1 -- Kodak DC20 and DC25 */
  { 2.25,0.75,-1.75,-0.25,-0.25,0.75,0.75,-0.25,-0.25,-1.75,0.75,2.25 },
  /* index 2 -- Logitech Fotoman Pixtura */
  { 1.893,-0.418,-0.476,-0.495,1.773,-0.278,-1.017,-0.655,2.672 },
  /* index 3 -- Nikon E880, E900, and E990 */
  { -1.936280,  1.800443, -1.448486,  2.584324,
     1.405365, -0.524955, -0.289090,  0.408680,
    -1.204965,  1.082304,  2.941367, -1.818705 }
  };
  int i, c;

  for (raw_color = i=0; i < 3; i++)
    FORCC rgb_cam[i][c] = table[index][i*colors+c];
}

short CLASS guess_byte_order (int words)
{
  uchar test[4][2];
  int t=2, msb;
  double diff, sum[2] = {0,0};

  fread (test[0], 2, 2, ifp);
  for (words-=2; words--; ) {
    fread (test[t], 2, 1, ifp);
    for (msb=0; msb < 2; msb++) {
      diff = (test[t^2][msb] << 8 | test[t^2][!msb])
	   - (test[t  ][msb] << 8 | test[t  ][!msb]);
      sum[msb] += diff*diff;
    }
    t = (t+1) & 3;
  }
  return sum[0] < sum[1] ? 0x4d4d : 0x4949;
}

/*
   Identify which camera created this file, and set global variables
   accordingly.
 */
void CLASS identify()
{
  char head[32], *cp;
  unsigned hlen, fsize, i, c, is_canon;
  struct jhead jh;
  static const struct {
    int fsize;
    char make[12], model[15], withjpeg;
  } table[] = {
    {    62464, "Kodak",    "DC20"       ,0 },
    {   124928, "Kodak",    "DC20"       ,0 },
    {   311696, "ST Micro", "STV680 VGA" ,0 },  /* SPYz */
    {   614400, "Kodak",    "KAI-0340"   ,0 },
    {   787456, "Creative", "PC-CAM 600" ,0 },
    {  1138688, "Minolta",  "RD175"      ,0 },
    {  3840000, "Foculus",  "531C"       ,0 },
    {  1447680, "AVT",      "F-145C"     ,0 },
    {  1920000, "AVT",      "F-201C"     ,0 },
    {  5067304, "AVT",      "F-510C"     ,0 },
    { 10134608, "AVT",      "F-510C"     ,0 },
    { 16157136, "AVT",      "F-810C"     ,0 },
    {  6624000, "Pixelink", "A782"       ,0 },
    { 13248000, "Pixelink", "A782"       ,0 },
    {  6291456, "RoverShot","3320AF"     ,0 },
    {  5939200, "OLYMPUS",  "C770UZ"     ,0 },
    {  1581060, "NIKON",    "E900"       ,1 },  /* or E900s,E910 */
    {  2465792, "NIKON",    "E950"       ,1 },  /* or E800,E700 */
    {  2940928, "NIKON",    "E2100"      ,1 },  /* or E2500 */
    {  4771840, "NIKON",    "E990"       ,1 },  /* or E995, Oly C3030Z */
    {  4775936, "NIKON",    "E3700"      ,1 },  /* or Optio 33WR */
    {  5869568, "NIKON",    "E4300"      ,1 },  /* or DiMAGE Z2 */
    {  5865472, "NIKON",    "E4500"      ,1 },
    {  7438336, "NIKON",    "E5000"      ,1 },  /* or E5700 */
    {  1976352, "CASIO",    "QV-2000UX"  ,1 },
    {  3217760, "CASIO",    "QV-3*00EX"  ,1 },
    {  6218368, "CASIO",    "QV-5700"    ,1 },
    {  7530816, "CASIO",    "QV-R51"     ,1 },
    {  7684000, "CASIO",    "QV-4000"    ,1 },
    {  4948608, "CASIO",    "EX-S100"    ,1 },
    {  7542528, "CASIO",    "EX-Z50"     ,1 },
    {  7753344, "CASIO",    "EX-Z55"     ,1 },
    {  7426656, "CASIO",    "EX-P505"    ,1 },
    {  9313536, "CASIO",    "EX-P600"    ,1 },
    { 10979200, "CASIO",    "EX-P700"    ,1 },
    {  3178560, "PENTAX",   "Optio S"    ,1 },  /*  8-bit */
    {  4841984, "PENTAX",   "Optio S"    ,1 },  /* 12-bit */
    {  6114240, "PENTAX",   "Optio S4"   ,1 },  /* or S4i */
    { 12582980, "Sinar",    ""           ,0 } };
  static const char *corp[] =
    { "Canon", "NIKON", "EPSON", "KODAK", "Kodak", "OLYMPUS", "PENTAX",
      "MINOLTA", "Minolta", "Konica", "CASIO", "Sinar", "Phase One" };

  tiff_flip = flip = filters = -1;	/* 0 is valid, so -1 is unknown */
  raw_height = raw_width = fuji_width = cr2_slice[0] = 0;
  maximum = height = width = top_margin = left_margin = 0;
  make[0] = model[0] = model2[0] = cdesc[0] = 0;
  iso_speed = shutter = aperture = focal_len = unique_id = 0;
  memset (white, 0, sizeof white);
  thumb_offset = thumb_length = thumb_width = thumb_height = 0;
  load_raw = thumb_load_raw = NULL;
  write_thumb = jpeg_thumb;
  data_offset = meta_length = tiff_bps = tiff_compress = 0;
  kodak_cbpp = zero_after_ff = dng_version = fuji_secondary = 0;
  timestamp = shot_order = tiff_samples = black = is_foveon = 0;
  is_raw = raw_color = use_gamma = xmag = ymag = 1;
  for (i=0; i < 4; i++) {
    cam_mul[i] = i == 1;
    pre_mul[i] = i < 3;
    FORC3 rgb_cam[c][i] = c == i;
  }
  colors = 3;
  tiff_bps = 12;
  for (i=0; i < 0x1000; i++) curve[i] = i;
  profile_length = 0;

  order = get2();
  hlen = get4();
  fseek (ifp, 0, SEEK_SET);
  fread (head, 1, 32, ifp);
  fseek (ifp, 0, SEEK_END);
  fsize = ftell(ifp);
  if ((cp = (char *) memmem (head, 32, "MMMM", 4)) ||
      (cp = (char *) memmem (head, 32, "IIII", 4))) {
    parse_phase_one (cp-head);
    if (cp-head) parse_tiff(0);
  } else if (order == 0x4949 || order == 0x4d4d) {
    if (!memcmp (head+6,"HEAPCCDR",8)) {
      data_offset = hlen;
      parse_ciff (hlen, fsize - hlen);
    } else {
      parse_tiff(0);
    }
  } else if (!memcmp (head,"\xff\xd8\xff\xe1",4) &&
	     !memcmp (head+6,"Exif",4)) {
    fseek (ifp, 4, SEEK_SET);
    fseek (ifp, 4 + get2(), SEEK_SET);
    if (fgetc(ifp) != 0xff)
      parse_tiff(12);
    thumb_offset = 0;
  } else if (!memcmp (head,"BM",2) &&
	head[26] == 1 && head[28] == 16 && head[30] == 0) {
    data_offset = 0x1000;
    order = 0x4949;
    fseek (ifp, 38, SEEK_SET);
    if (get4() == 2834 && get4() == 2834 && get4() == 0 && get4() == 4096) {
      strcpy (model, "BMQ");
      flip = 3;
      goto nucore;
    }
  } else if (!memcmp (head,"BR",2)) {
    strcpy (model, "RAW");
nucore:
    strcpy (make, "Nucore");
    order = 0x4949;
    fseek (ifp, 10, SEEK_SET);
    data_offset += get4();
    raw_width = (get4(),get4());
    raw_height = get4();
    if (model[0] == 'B' && raw_width == 2597) {
      raw_width++;
      data_offset -= 0x1000;
    }
  } else if (!memcmp (head+25,"ARECOYK",7)) {
    strcpy (make, "Contax");
    strcpy (model,"N Digital");
    fseek (ifp, 33, SEEK_SET);
    get_timestamp(1);
    fseek (ifp, 60, SEEK_SET);
    FORC4 cam_mul[c ^ (c >> 1)] = get4();
  } else if (!strcmp (head, "PXN")) {
    strcpy (make, "Logitech");
    strcpy (model,"Fotoman Pixtura");
  } else if (!memcmp (head,"FUJIFILM",8)) {
    fseek (ifp, 84, SEEK_SET);
    thumb_offset = get4();
    thumb_length = get4();
    fseek (ifp, 92, SEEK_SET);
    parse_fuji (get4());
    if (thumb_offset > 120) {
      fseek (ifp, 120, SEEK_SET);
      fuji_secondary = (i = get4()) && 1;
      if (fuji_secondary && use_secondary)
	parse_fuji (i);
    }
    fseek (ifp, 100, SEEK_SET);
    data_offset = get4();
    parse_tiff (thumb_offset+12);
  } else if (!memcmp (head,"RIFF",4)) {
    fseek (ifp, 0, SEEK_SET);
    parse_riff();
  } else if (!memcmp (head,"DSC-Image",9))
    parse_rollei();
  else if (!memcmp (head,"\0MRM",4))
    parse_minolta();
  else if (!memcmp (head,"FOVb",4))
    parse_foveon();
  else
    for (i=0; i < sizeof table / sizeof *table; i++)
      if (fsize == table[i].fsize) {
	strcpy (make,  table[i].make );
	strcpy (model, table[i].model);
	if (table[i].withjpeg)
	  parse_external_jpeg();
      }
  parse_mos(8);
  parse_mos(3472);
  if (make[0] == 0) parse_smal (0, fsize);
  if (make[0] == 0) parse_jpeg (is_raw = 0);

  for (i=0; i < sizeof corp / sizeof *corp; i++)
    if (strstr (make, corp[i]))		/* Simplify company names */
	strcpy (make, corp[i]);
  if (!strncmp (make,"KODAK",5))
    make[16] = model[16] = 0;
  cp = make + strlen(make);		/* Remove trailing spaces */
  while (*--cp == ' ') *cp = 0;
  cp = model + strlen(model);
  while (*--cp == ' ') *cp = 0;
  i = strlen(make);			/* Remove make from model */
  if (!strncmp (model, make, i) && model[i++] == ' ')
    memmove (model, model+i, 64-i);
  make[63] = model[63] = model2[63] = 0;
  if (!is_raw) return;

  if ((raw_height | raw_width) < 0)
       raw_height = raw_width  = 0;
  if (!maximum) maximum = (1 << tiff_bps) - 1;
  if (!height) height = raw_height;
  if (!width)  width  = raw_width;
  if (fuji_width) {
    width = height + fuji_width;
    height = width - 1;
    xmag = ymag = 1;
  }
  if (dng_version) {
    strcat (model," DNG");
    if (filters == UINT_MAX) filters = 0;
    if (!filters)
      colors = tiff_samples;
    if (tiff_compress == 1)
      load_raw = adobe_dng_load_raw_nc;
    if (tiff_compress == 7)
      load_raw = adobe_dng_load_raw_lj;
    FORC4 cam_mul[c] = pre_mul[c];
    goto dng_skip;
  }

/*  We'll try to decode anything from Canon or Nikon. */

  if ((is_canon = !strcmp(make,"Canon"))) {
    load_raw = memcmp (head+6,"HEAPCCDR",8) ?
	lossless_jpeg_load_raw : canon_compressed_load_raw;
    maximum = 0xfff;
  }
  if (!strcmp(make,"NIKON"))
    load_raw = nikon_is_compressed() ?
	nikon_compressed_load_raw : nikon_load_raw;
  if (!strncmp (make,"OLYMPUS",7))
    height += height & 1;

/* Set parameters based on camera name (for non-DNG files). */

  if (is_foveon) {
    if (height*2 < width) ymag = 2;
    if (height   > width) xmag = 2;
    filters = 0;
    load_raw = foveon_load_raw;
    simple_coeff(0);
  } else if (!strcmp(model,"PowerShot 600")) {
    height = 613;
    width  = 854;
    colors = 4;
    filters = 0xe1e4e1e4;
    load_raw = canon_600_load_raw;
  } else if (!strcmp(model,"PowerShot A5") ||
	     !strcmp(model,"PowerShot A5 Zoom")) {
    height = 773;
    width  = 960;
    raw_width = 992;
    colors = 4;
    filters = 0x1e4e1e4e;
    load_raw = canon_a5_load_raw;
  } else if (!strcmp(model,"PowerShot A50")) {
    height =  968;
    width  = 1290;
    raw_width = 1320;
    colors = 4;
    filters = 0x1b4e4b1e;
    load_raw = canon_a5_load_raw;
  } else if (!strcmp(model,"PowerShot Pro70")) {
    height = 1024;
    width  = 1552;
    colors = 4;
    filters = 0x1e4b4e1b;
    load_raw = canon_a5_load_raw;
  } else if (!strcmp(model,"PowerShot Pro90 IS")) {
    width  = 1896;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (is_canon && raw_width == 2144) {
    height = 1550;
    width  = 2088;
    top_margin  = 8;
    left_margin = 4;
    if (!strcmp(model,"PowerShot G1")) {
      colors = 4;
      filters = 0xb4b4b4b4;
    }
  } else if (is_canon && raw_width == 2224) {
    height = 1448;
    width  = 2176;
    top_margin  = 6;
    left_margin = 48;
  } else if (is_canon && raw_width == 2376) {
    height = 1720;
    width  = 2312;
    top_margin  = 6;
    left_margin = 12;
  } else if (is_canon && raw_width == 2672) {
    height = 1960;
    width  = 2616;
    top_margin  = 6;
    left_margin = 12;
  } else if (is_canon && raw_width == 3152) {
    height = 2056;
    width  = 3088;
    top_margin  = 12;
    left_margin = 64;
    maximum = 0xfa0;
  } else if (is_canon && raw_width == 3160) {
    height = 2328;
    width  = 3112;
    top_margin  = 12;
    left_margin = 44;
  } else if (is_canon && raw_width == 3344) {
    height = 2472;
    width  = 3288;
    top_margin  = 6;
    left_margin = 4;
  } else if (!strcmp(model,"EOS D2000C")) {
    filters = 0x61616161;
    black = curve[200];
  } else if (is_canon && raw_width == 3516) {
    top_margin  = 14;
    left_margin = 42;
    if (unique_id == 0x80000189)
      adobe_coeff ("Canon","EOS 350D");
    goto canon_cr2;
  } else if (is_canon && raw_width == 3596) {
    top_margin  = 12;
    left_margin = 74;
    goto canon_cr2;
  } else if (is_canon && raw_width == 4476) {
    top_margin  = 34;
    left_margin = 90;
    goto canon_cr2;
  } else if (is_canon && raw_width == 5108) {
    top_margin  = 13;
    left_margin = 98;
    maximum = 0xe80;
canon_cr2:
    height = raw_height - top_margin;
    width  = raw_width - left_margin;
  } else if (!strcmp(model,"D1")) {
    cam_mul[0] *= 256/527.0;
    cam_mul[2] *= 256/317.0;
  } else if (!strcmp(model,"D1X")) {
    width -= 4;
    ymag = 2;
  } else if (!strncmp(model,"D50",3) || !strncmp(model,"D70",3)) {
    width--;
    maximum = 0xf53;
  } else if (!strcmp(model,"D100")) {
    if (tiff_compress == 34713 && load_raw == nikon_load_raw)
      raw_width = (width += 3) + 3;
    maximum = 0xf44;
  } else if (!strncmp(model,"D2H",3)) {
    left_margin = 6;
    width -= 14;
  } else if (!strcmp(model,"D2X")) {
    width -= 8;
  } else if (fsize == 1581060) {
    height = 963;
    width = 1287;
    raw_width = 1632;
    load_raw = nikon_e900_load_raw;
    maximum = 0x3f4;
    colors = 4;
    filters = 0x1e1e1e1e;
    simple_coeff(3);
    pre_mul[0] = 1.2085;
    pre_mul[1] = 1.0943;
    pre_mul[3] = 1.1103;
  } else if (fsize == 2465792) {
    height = 1203;
    width  = 1616;
    raw_width = 2048;
    load_raw = nikon_e900_load_raw;
    maximum = 0x3dd;
    colors = 4;
    filters = 0x4b4b4b4b;
    adobe_coeff ("NIKON","E950");
  } else if (fsize == 4771840) {
    height = 1540;
    width  = 2064;
    colors = 4;
    filters = 0xe1e1e1e1;
    load_raw = nikon_load_raw;
    if (!timestamp && nikon_e995())
      strcpy (model, "E995");
    if (strcmp(model,"E995")) {
      filters = 0xb4b4b4b4;
      simple_coeff(3);
      pre_mul[0] = 1.196;
      pre_mul[1] = 1.246;
      pre_mul[2] = 1.018;
    }
  } else if (!strcmp(model,"E2100")) {
    if (!timestamp && !nikon_e2100()) goto cp_e2500;
    height = 1206;
    width  = 1616;
    load_raw = nikon_e2100_load_raw;
    pre_mul[0] = 1.945;
    pre_mul[2] = 1.040;
  } else if (!strcmp(model,"E2500")) {
cp_e2500:
    strcpy (model, "E2500");
    height = 1204;
    width  = 1616;
    colors = 4;
    filters = 0x4b4b4b4b;
  } else if (fsize == 4775936) {
    height = 1542;
    width  = 2064;
    load_raw = nikon_e2100_load_raw;
    pre_mul[0] = 1.818;
    pre_mul[2] = 1.618;
    if ((i = nikon_3700()) == 2) {
      strcpy (make, "OLYMPUS");
      strcpy (model, "C740UZ");
    } else if (i == 0) {
      strcpy (make, "PENTAX");
      strcpy (model,"Optio 33WR");
      flip = 1;
      filters = 0x16161616;
      pre_mul[0] = 1.331;
      pre_mul[2] = 1.820;
    }
  } else if (fsize == 5869568) {
    height = 1710;
    width  = 2288;
    filters = 0x16161616;
    if (!timestamp && minolta_z2()) {
      strcpy (make, "Minolta");
      strcpy (model,"DiMAGE Z2");
    }
    if (make[0] == 'M')
      load_raw = nikon_e2100_load_raw;
  } else if (!strcmp(model,"E4500")) {
    height = 1708;
    width  = 2288;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (fsize == 7438336) {
    height = 1924;
    width  = 2576;
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (!strncmp(model,"R-D1",4)) {
    tiff_compress = 34713;
    load_raw = nikon_load_raw;
  } else if (!strcmp(model,"FinePix S5100") ||
	     !strcmp(model,"FinePix S5500")) {
    load_raw = unpacked_load_raw;
    maximum = 0x3e00;
  } else if (!strncmp(model,"FinePix",7)) {
    if (!strcmp(model+7,"S2Pro")) {
      strcpy (model+7," S2Pro");
      height = 2144;
      width  = 2880;
      flip = 6;
    } else
      maximum = 0x3e00;
    top_margin = (raw_height - height)/2;
    left_margin = (raw_width - width )/2;
    data_offset += (top_margin*raw_width + left_margin) * 2;
    if (fuji_secondary)
      data_offset += use_secondary * ( strcmp(model+7," S3Pro")
		? (raw_width *= 2) : raw_height*raw_width*2 );
    fuji_width = width >> !fuji_layout;
    width = (height >> fuji_layout) + fuji_width;
    raw_height = height;
    height = width - 1;
    load_raw = fuji_load_raw;
    if (!(fuji_width & 1)) filters = 0x49494949;
  } else if (!strcmp(model,"RD175")) {
    height = 986;
    width = 1534;
    data_offset = 513;
    filters = 0x61616161;
    load_raw = minolta_rd175_load_raw;
  } else if (!strcmp(model,"Digital Camera KD-400Z")) {
    height = 1712;
    width  = 2312;
    raw_width = 2336;
    data_offset = 4034;
    fseek (ifp, 2032, SEEK_SET);
    goto konica_400z;
  } else if (!strcmp(model,"Digital Camera KD-510Z")) {
    data_offset = 4032;
    pre_mul[0] = 1.297;
    pre_mul[2] = 1.438;
    fseek (ifp, 2032, SEEK_SET);
    goto konica_510z;
  } else if (!strcasecmp(make,"MINOLTA")) {
    load_raw = unpacked_load_raw;
    maximum = 0xf7d;
    if (!strncmp(model,"DiMAGE A",8)) {
      if (!strcmp(model,"DiMAGE A200"))
	filters = 0x49494949;
      load_raw = packed_12_load_raw;
      maximum = model[8] == '1' ? 0xf8b : 0xfff;
    } else if (!strncmp(model,"ALPHA",5) ||
	       !strncmp(model,"DYNAX",5) ||
	       !strncmp(model,"MAXXUM",6)) {
      sprintf (model, "DYNAX %s", model+6 + (model[0]=='M'));
      load_raw = packed_12_load_raw;
      maximum = 0xffb;
    } else if (!strncmp(model,"DiMAGE G",8)) {
      if (model[8] == '4') {
	data_offset = 5056;
	pre_mul[0] = 1.602;
	pre_mul[2] = 1.441;
	fseek (ifp, 2078, SEEK_SET);
	height = 1716;
	width  = 2304;
      } else if (model[8] == '5') {
	data_offset = 4016;
	fseek (ifp, 1936, SEEK_SET);
konica_510z:
	height = 1956;
	width  = 2607;
	raw_width = 2624;
      } else if (model[8] == '6') {
	data_offset = 4032;
	fseek (ifp, 2030, SEEK_SET);
	height = 2136;
	width  = 2848;
      }
      filters = 0x61616161;
konica_400z:
      load_raw = unpacked_load_raw;
      maximum = 0x3df;
      order = 0x4d4d;
      FORC4 cam_mul[(c >> 1) | ((c & 1) << 1)] = get2();
    }
    if (pre_mul[0] == 1 && pre_mul[2] == 1) {
      pre_mul[0] = 1.42;
      pre_mul[2] = 1.25;
    }
  } else if (!strcmp(model,"*ist DS")) {
    height -= 2;
  } else if (!strcmp(model,"Optio S")) {
    if (fsize == 3178560) {
      height = 1540;
      width  = 2064;
      load_raw = eight_bit_load_raw;
      cam_mul[0] *= 4;
      cam_mul[2] *= 4;
      pre_mul[0] = 1.391;
      pre_mul[2] = 1.188;
    } else {
      height = 1544;
      width  = 2068;
      raw_width = 3136;
      load_raw = packed_12_load_raw;
      maximum = 0xf7c;
      pre_mul[0] = 1.137;
      pre_mul[2] = 1.453;
    }
  } else if (!strncmp(model,"Optio S4",8)) {
    height = 1737;
    width  = 2324;
    raw_width = 3520;
    load_raw = packed_12_load_raw;
    maximum = 0xf7a;
    pre_mul[0] = 1.980;
    pre_mul[2] = 1.570;
  } else if (!strcmp(model,"STV680 VGA")) {
    height = 484;
    width  = 644;
    load_raw = eight_bit_load_raw;
    flip = 2;
    filters = 0x16161616;
    black = 16;
    pre_mul[0] = 1.097;
    pre_mul[2] = 1.128;
  } else if (!strcmp(model,"KAI-0340")) {
    height = 477;
    width  = 640;
    order = 0x4949;
    data_offset = 3840;
    load_raw = unpacked_load_raw;
    pre_mul[0] = 1.561;
    pre_mul[2] = 2.454;
  } else if (!strcmp(model,"531C")) {
    height = 1200;
    width  = 1600;
    load_raw = unpacked_load_raw;
    filters = 0x49494949;
    pre_mul[1] = 1.218;
  } else if (!strcmp(model,"F-145C")) {
    height = 1040;
    width  = 1392;
    load_raw = eight_bit_load_raw;
  } else if (!strcmp(model,"F-201C")) {
    height = 1200;
    width  = 1600;
    load_raw = eight_bit_load_raw;
  } else if (!strcmp(model,"F-510C")) {
    height = 1958;
    width  = 2588;
    load_raw = (fsize < 7500000) ? eight_bit_load_raw : unpacked_load_raw;
    maximum = 0xfff0;
  } else if (!strcmp(model,"F-810C")) {
    height = 2469;
    width  = 3272;
    load_raw = unpacked_load_raw;
    maximum = 0xfff0;
  } else if (!strcmp(model,"A782")) {
    height = 3000;
    width  = 2208;
    filters = 0x61616161;
    load_raw = (fsize < 10000000) ? eight_bit_load_raw : unpacked_load_raw;
    maximum = 0xffc0;
  } else if (!strcmp(model,"3320AF")) {
    height = 1536;
    raw_width = width = 2048;
    filters = 0x61616161;
    load_raw = unpacked_load_raw;
    maximum = 0x3ff;
    pre_mul[0] = 1.717;
    pre_mul[2] = 1.138;
    fseek (ifp, 0x300000, SEEK_SET);
    if ((order = guess_byte_order(0x10000)) == 0x4d4d) {
      data_offset = (2048 * 16 + 28) * 2;
      height -= 16;
      width  -= 28;
      maximum = 0xf5c0;
      strcpy (make, "ISG");
      model[0] = 0;
    }
  } else if (!strcmp(make,"Imacon")) {
    sprintf (model, "Ixpress %d-Mp", height*width/1000000);
    load_raw = imacon_full_load_raw;
    if (filters) {
      data_offset += (top_margin*raw_width + left_margin) * 2;
      if (left_margin & 1) filters = 0x61616161;
      load_raw = unpacked_load_raw;
    }
    maximum = 0xffff;
  } else if (!strcmp(make,"Sinar")) {
    if (!memcmp(head,"8BPS",4)) {
      fseek (ifp, 14, SEEK_SET);
      height = get4();
      width  = get4();
      filters = 0x61616161;
      data_offset = 68;
    }
    load_raw = unpacked_load_raw;
    maximum = 0x3fff;
  } else if (!strcmp(make,"Leaf")) {
    if (tiff_compress == 99)
      load_raw = lossless_jpeg_load_raw;
    maximum = 0x3fff;
    if (filters == 0) {
      strcpy (model, "Volare");
      load_raw = leaf_full_load_raw;
      maximum = 0xffff;
      raw_color = 0;
    }
  } else if (!strcmp(make,"LEICA") || !strcmp(make,"Panasonic")) {
    if (width == 3880) {
      data_offset += 12;
      maximum = 0xf7f0;
      width -= 22;
    } else if (width == 3304) {
      maximum = 0xf94c;
      width -= 16;
    } else maximum = 0xfff0;
    load_raw = unpacked_load_raw;
  } else if (!strcmp(model,"E-1")) {
    filters = 0x61616161;
    maximum = 0xfff0;
    black = 1024;
  } else if (!strcmp(model,"E-10")) {
    maximum = 0xfff0;
    black = 2048;
  } else if (!strncmp(model,"E-20",4)) {
    maximum = 0xffc0;
    black = 2560;
  } else if (!strcmp(model,"E-300") ||
	     !strcmp(model,"E-500")) {
    width -= 20;
    maximum = 0xfc30;
    if (fsize <= 15728640) {
      maximum = 0xfff;
      load_raw = olympus_e300_load_raw;
    }
  } else if (!strcmp(model,"E-330")) {
    width -= 30;
    load_raw = olympus_e300_load_raw;
  } else if (!strcmp(model,"C770UZ")) {
    height = 1718;
    width  = 2304;
    filters = 0x16161616;
    load_raw = nikon_e2100_load_raw;
  } else if (!strcmp(make,"OLYMPUS")) {
    load_raw = olympus_cseries_load_raw;
    if (!strcmp(model,"C5050Z") ||
	!strcmp(model,"C8080WZ"))
      filters = 0x16161616;
    if (!strcmp(model,"SP500UZ"))
      filters = 0x49494949;
  } else if (!strcmp(model,"N Digital")) {
    height = 2047;
    width  = 3072;
    filters = 0x61616161;
    data_offset = 0x1a00;
    load_raw = packed_12_load_raw;
    maximum = 0xf1e;
  } else if (!strcmp(model,"DSC-F828")) {
    width = 3288;
    left_margin = 5;
    data_offset = 862144;
    load_raw = sony_load_raw;
    filters = 0x9c9c9c9c;
    colors = 4;
    strcpy (cdesc, "RGBE");
  } else if (!strcmp(model,"DSC-V3")) {
    width = 3109;
    left_margin = 59;
    data_offset = 787392;
    load_raw = sony_load_raw;
  } else if (!strcmp(make,"SONY") && raw_width == 3984) {
    adobe_coeff ("SONY","DSC-R1");
    width = 3925;
    order = 0x4d4d;
  } else if (!strncmp(model,"P850",4)) {
    maximum = 0xf7c;
  } else if (!strcasecmp(make,"KODAK")) {
    if (filters == UINT_MAX) filters = 0x61616161;
    if (!strcmp(model,"NC2000F")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS3B")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS1")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS420")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS460")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS460A")) {
      width -= 4;
      left_margin = 2;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS660M")) {
      black = 214;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS760M")) {
      colors = 1;
      filters = 0;
    }
    if (load_raw == eight_bit_load_raw)
	load_raw = kodak_easy_load_raw;
    if (strstr(model,"DC25")) {
      strcpy (model, "DC25");
      data_offset = 15424;
    }
    if (!strncmp(model,"DC2",3)) {
      height = 242;
      if (fsize < 100000) {
	raw_width = 256; width = 249;
      } else {
	raw_width = 512; width = 501;
      }
      data_offset += raw_width + 1;
      colors = 4;
      filters = 0x8d8d8d8d;
      simple_coeff(1);
      pre_mul[1] = 1.179;
      pre_mul[2] = 1.209;
      pre_mul[3] = 1.036;
      load_raw = kodak_easy_load_raw;
    } else if (!strcmp(model,"Digital Camera 40")) {
      strcpy (model, "DC40");
      height = 512;
      width  = 768;
      data_offset = 1152;
      load_raw = kodak_radc_load_raw;
    } else if (strstr(model,"DC50")) {
      strcpy (model, "DC50");
      height = 512;
      width  = 768;
      data_offset = 19712;
      load_raw = kodak_radc_load_raw;
    } else if (strstr(model,"DC120")) {
      strcpy (model, "DC120");
      height = 976;
      width  = 848;
      load_raw = (tiff_compress == 7)
		? kodak_jpeg_load_raw : kodak_dc120_load_raw;
    }
  } else if (!strcmp(model,"Fotoman Pixtura")) {
    height = 512;
    width  = 768;
    data_offset = 3632;
    load_raw = kodak_radc_load_raw;
    filters = 0x61616161;
    simple_coeff(2);
  } else if (!strcmp(make,"Rollei")) {
    switch (raw_width) {
      case 1316:
	height = 1030;
	width  = 1300;
	top_margin  = 1;
	left_margin = 6;
	break;
      case 2568:
	height = 1960;
	width  = 2560;
	top_margin  = 2;
	left_margin = 8;
    }
    filters = 0x16161616;
    load_raw = rollei_load_raw;
    pre_mul[0] = 1.8;
    pre_mul[2] = 1.3;
  } else if (!strcmp(model,"PC-CAM 600")) {
    height = 768;
    data_offset = width = 1024;
    filters = 0x49494949;
    load_raw = eight_bit_load_raw;
    pre_mul[0] = 1.14;
    pre_mul[2] = 2.73;
  } else if (!strcmp(model,"QV-2000UX")) {
    height = 1208;
    width  = 1632;
    data_offset = width * 2;
    load_raw = eight_bit_load_raw;
  } else if (fsize == 3217760) {
    height = 1546;
    width  = 2070;
    raw_width = 2080;
    load_raw = eight_bit_load_raw;
  } else if (!strcmp(model,"QV-4000")) {
    height = 1700;
    width  = 2260;
    load_raw = unpacked_load_raw;
    maximum = 0xffff;
  } else if (!strcmp(model,"QV-5700")) {
    height = 1924;
    width  = 2576;
    load_raw = casio_qv5700_load_raw;
  } else if (!strcmp(model,"QV-R51")) {
    height = 1926;
    width  = 2576;
    raw_width = 3904;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 1.340;
    pre_mul[2] = 1.672;
  } else if (!strcmp(model,"EX-S100")) {
    height = 1544;
    width  = 2058;
    raw_width = 3136;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 1.631;
    pre_mul[2] = 1.106;
  } else if (!strcmp(model,"EX-Z50")) {
    height = 1931;
    width  = 2570;
    raw_width = 3904;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 2.529;
    pre_mul[2] = 1.185;
  } else if (!strcmp(model,"EX-Z55")) {
    height = 1960;
    width  = 2570;
    raw_width = 3904;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 1.520;
    pre_mul[2] = 1.316;
  } else if (!strcmp(model,"EX-P505")) {
    height = 1928;
    width  = 2568;
    raw_width = 3852;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 2.07;
    pre_mul[2] = 1.88;
  } else if (fsize == 9313536) {	/* EX-P600 or QV-R61 */
    height = 2142;
    width  = 2844;
    raw_width = 4288;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 1.797;
    pre_mul[2] = 1.219;
  } else if (!strcmp(model,"EX-P700")) {
    height = 2318;
    width  = 3082;
    raw_width = 4672;
    load_raw = packed_12_load_raw;
    pre_mul[0] = 1.758;
    pre_mul[2] = 1.504;
  } else if (!strcmp(make,"Nucore")) {
    filters = 0x61616161;
    load_raw = unpacked_load_raw;
    if (width == 2598) {
      filters = 0x16161616;
      load_raw = nucore_load_raw;
      flip = 2;
    }
  }
  if (!model[0])
    sprintf (model, "%dx%d", width, height);
  if (filters == UINT_MAX) filters = 0x94949494;
  if (raw_color) adobe_coeff (make, model);
  if (thumb_offset && !thumb_height) {
    fseek (ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1)) {
      thumb_width  = jh.wide;
      thumb_height = jh.high;
    }
  }
dng_skip:
  if (!load_raw || !height) is_raw = 0;
#ifdef NO_JPEG
  if (load_raw == kodak_jpeg_load_raw) {
    fprintf (stderr, "%s: You must link dcraw.c with libjpeg!!\n", ifname);
    is_raw = 0;
  }
#endif
  if (flip == -1) flip = tiff_flip;
  if (flip == -1) flip = 0;
  if (!cdesc[0])
    strcpy (cdesc, colors == 3 ? "RGB":"GMCY");
  if (!raw_height) raw_height = height;
  if (!raw_width ) raw_width  = width;
  if (filters && colors == 3)
    for (i=0; i < 32; i+=4) {
      if ((filters >> i & 15) == 9)
	filters |= 2 << i;
      if ((filters >> i & 15) == 6)
	filters |= 8 << i;
    }
}

#ifndef NO_LCMS
void CLASS apply_profile (char *input, char *output)
{
  char *prof;
  cmsHPROFILE hInProfile=NULL, hOutProfile;
  cmsHTRANSFORM hTransform;

  cmsErrorAction (LCMS_ERROR_SHOW);
  if (strcmp (input, "embed"))
    hInProfile = cmsOpenProfileFromFile (input, "r");
  else if (profile_length) {
    prof = malloc (profile_length);
    merror (prof, "apply_profile()");
    fseek (ifp, profile_offset, SEEK_SET);
    fread (prof, 1, profile_length, ifp);
    hInProfile = cmsOpenProfileFromMem (prof, profile_length);
    free (prof);
  } else
    fprintf (stderr, "%s has no embedded profile.\n", ifname);
  if (!hInProfile) return;
  hOutProfile = output ? cmsOpenProfileFromFile (output, "r")
		       : cmsCreate_sRGBProfile();
  if (!hOutProfile) goto quit;
  if (verbose)
    fprintf (stderr, "Applying color profile...\n");
  hTransform = cmsCreateTransform (hInProfile, TYPE_RGBA_16,
	hOutProfile, TYPE_RGBA_16, INTENT_PERCEPTUAL, 0);
  cmsDoTransform (hTransform, image, image, width*height);
  maximum = 0xffff;
  raw_color = 1;		/* Don't use rgb_cam with a profile */
  cmsDeleteTransform (hTransform);
  cmsCloseProfile (hOutProfile);
quit:
  cmsCloseProfile (hInProfile);
}
#endif

void CLASS convert_to_rgb()
{
  int mix_green, row, col, c, i, j, k;
  ushort *img;
  float out[3], out_cam[3][4];
  static const double adobe_rgb[3][3] =
  { { 0.715146, 0.284856, 0.000000 },
    { 0.000000, 1.000000, 0.000000 },
    { 0.000000, 0.041166, 0.958839 } };
  static const double wgd65_rgb[3][3] =
  { { 0.593087, 0.404710, 0.002206 },
    { 0.095413, 0.843149, 0.061439 },
    { 0.011621, 0.069091, 0.919288 } };
  static const double prophoto_rgb[3][3] =
  { { 0.522966, 0.346773, 0.130274 },
    { 0.089210, 0.862705, 0.048090 },
    { 0.017725, 0.109450, 0.872912 } };
  static const double (*out_rgb[])[3] =
  { adobe_rgb, wgd65_rgb, prophoto_rgb, xyz_rgb };
  static const char *name[] =
  { "sRGB", "Adobe 1998 RGB", "Wide Gamut D65", "ProPhoto D65", "XYZ" };

  memcpy (out_cam, rgb_cam, sizeof out_cam);
  raw_color |= colors == 1 || output_color < 1 || output_color > 5;
  if (!raw_color)
    if (output_color > 1)
      for (i=0; i < 3; i++)
	for (j=0; j < colors; j++)
	  for (out_cam[i][j] = k=0; k < 3; k++)
	    out_cam[i][j] += out_rgb[output_color-2][i][k] * rgb_cam[k][j];
  if (verbose)
    fprintf (stderr, raw_color ? "Building histograms...\n" :
	"Converting to %s colorspace...\n", name[output_color-1]);

  mix_green = raw_color && rgb_cam[1][1] == rgb_cam[1][3];
  memset (histogram, 0, sizeof histogram);
  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++) {
      img = image[row*width+col];
      if (document_mode && filters)
	img[0] = img[FC(row,col)];
      else if (mix_green)
	img[1] = (img[1] + img[3]) >> 1;
      else if (!raw_color) {
	FORC3 for (out[c]=i=0; i < colors; i++)
	  out[c] += img[i] * out_cam[c][i];
	FORC3 img[c] = CLIP((int) out[c]);
      }
      FORCC histogram[c][img[c] >> 3]++;
    }
  if (colors == 4 && (output_color || mix_green)) colors = 3;
  if (document_mode && filters) colors = 1;
}

void CLASS fuji_rotate()
{
  int i, wide, high, row, col;
  double step;
  float r, c, fr, fc;
  unsigned ur, uc;
  ushort (*img)[4], (*pix)[4];

  if (!fuji_width) return;
  if (verbose)
    fprintf (stderr, "Rotating image 45 degrees...\n");
  fuji_width = (fuji_width - 1 + shrink) >> shrink;
  step = sqrt(0.5);
  wide = fuji_width / step;
  high = (height - fuji_width) / step;
  img = calloc (wide*high, sizeof *img);
  merror (img, "fuji_rotate()");

  for (row=0; row < high; row++)
    for (col=0; col < wide; col++) {
      ur = r = fuji_width + (row-col)*step;
      uc = c = (row+col)*step;
      if (ur > height-2 || uc > width-2) continue;
      fr = r - ur;
      fc = c - uc;
      pix = image + ur*width + uc;
      for (i=0; i < colors; i++)
	img[row*wide+col][i] =
	  (pix[    0][i]*(1-fc) + pix[      1][i]*fc) * (1-fr) +
	  (pix[width][i]*(1-fc) + pix[width+1][i]*fc) * fr;
    }
  free (image);
  width  = wide;
  height = high;
  image  = img;
  fuji_width = 0;
}

int CLASS flip_index (int row, int col)
{
  if (flip & 1) col = width - 1 - col;
  if (flip & 2) row = height - 1 - row;
  return (flip & 4) ? col * height + row
		    : row * width + col;
}

void CLASS flip_image()
{
  int row, col, soff=0, doff, rstep, cstep;
  INT64 *src, *dest;

  if (verbose)
    fprintf (stderr, "Flipping image %c:%c:%c...\n",
	flip & 1 ? 'H':'0', flip & 2 ? 'V':'0', flip & 4 ? 'T':'0');
  src = (INT64 *) image;
  dest = calloc (height * width, sizeof *dest);
  merror (dest, "flip_image()");
  doff  = flip_index (0, 0);
  cstep = flip_index (0, 1) - doff;
  rstep = flip_index (1, 0) - flip_index (0, width);
  for (row=0; row < height; row++, doff += rstep)
    for (col=0; col < width; col++, doff += cstep)
      dest[doff] = src[soff++];
  image = (ushort (*)[4]) dest;
  free (src);
  if (flip & 4) {
    SWAP (height, width);
    SWAP (ymag, xmag);
  }
}

void CLASS write_ppm (FILE *ofp)
{
  uchar *ppm, lut[0x10000];
  int perc, c, val, total, i, row, col;
  float white=0, r;

  ppm = calloc (width, colors*xmag);
  merror (ppm, "write_ppm()");

  if (colors > 3)
    fprintf (ofp,
      "P7\nWIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL 255\nTUPLTYPE %s\nENDHDR\n",
	xmag*width, ymag*height, colors, cdesc);
  else
    fprintf (ofp, "P%d\n%d %d\n255\n", colors/2+5, xmag*width, ymag*height);

  perc = width * height * 0.01;		/* 99th percentile white point */
  if (fuji_width) perc /= 2;
  FORCC {
    for (val=0x2000, total=0; --val > 32; )
      if ((total += histogram[c][val]) > perc) break;
    if (white < val) white = val;
  }
  white *= 8 / bright;
  for (i=0; i < 0x10000; i++) {
    r = i / white;
    val = 256 * ( !use_gamma ? r :
#ifdef SRGB_GAMMA
	r <= 0.00304 ? r*12.92 : pow(r,2.5/6)*1.055-0.055 );
#else
	r <= 0.018 ? r*4.5 : pow(r,0.45)*1.099-0.099 );
#endif
    if (val > 255) val = 255;
    lut[i] = val;
  }
  for (row=0; row < height; row++) {
    for (col=0; col < width; col++)
      FORCC for (i=0; i < xmag; i++)
	ppm[(col*xmag+i)*colors+c] = lut[image[row*width+col][c]];
    for (i=0; i < ymag; i++)
      fwrite (ppm, colors*xmag, width, ofp);
  }
  free (ppm);
}

void CLASS write_ppm16 (FILE *ofp)
{
  int row, col, c;
  ushort *ppm;

  ppm = calloc (width, 2*colors);
  merror (ppm, "write_ppm16()");
  if (colors > 3)
    fprintf (ofp,
      "P7\nWIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL %d\nTUPLTYPE %s\nENDHDR\n",
	width, height, colors, 65535, cdesc);
  else
    fprintf (ofp, "P%d\n%d %d\n%d\n",
	colors/2+5, width, height, 65535);

  for (row = 0; row < height; row++) {
    for (col = 0; col < width; col++)
      FORCC ppm[col*colors+c] = htons(image[row*width+col][c]);
    fwrite (ppm, 2*colors, width, ofp);
  }
  free (ppm);
}

void CLASS write_psd (FILE *ofp)
{
  char head[] = {
    '8','B','P','S',		/* signature */
    0,1,0,0,0,0,0,0,		/* version and reserved */
    0,3,			/* number of channels */
    0,0,0,0,			/* height, big-endian */
    0,0,0,0,			/* width, big-endian */
    0,16,			/* 16-bit color */
    0,3,			/* mode (1=grey, 3=rgb) */
    0,0,0,0,			/* color mode data */
    0,0,0,0,			/* image resources */
    0,0,0,0,			/* layer/mask info */
    0,0				/* no compression */
  };
  int hw[2], psize, row, col, c;
  ushort *buffer, *pred;

  hw[0] = htonl(height);	/* write the header */
  hw[1] = htonl(width);
  memcpy (head+14, hw, sizeof hw);
  head[13] = head[25] = colors;
  fwrite (head, 40, 1, ofp);

  psize = height*width;
  buffer = calloc (colors, psize*2);
  merror (buffer, "write_psd()");
  pred = buffer;

  for (row = 0; row < height; row++)
    for (col = 0; col < width; col++) {
      FORCC pred[c*psize] = htons(image[row*width+col][c]);
      pred++;
    }
  fwrite(buffer, psize*2, colors, ofp);
  free (buffer);
}

int CLASS main (int argc, char **argv)
{
  int arg, status=0, user_flip=-1, user_black=-1, user_qual=-1;
  int timestamp_only=0, thumbnail_only=0, identify_only=0, write_to_stdout=0;
  int half_size=0, use_fuji_rotate=1, quality, i, c;
  char opt, *write_ext, *ofname, *sp, *cp;
  struct utimbuf ut;
  FILE *ofp = stdout;
  void (*write_image)(FILE *) = write_ppm;
#ifndef NO_LCMS
  char *cam_profile = NULL, *out_profile = NULL;
#endif

#ifndef LOCALTIME
  putenv ("TZ=UTC");
#endif
  if (argc == 1)
  {
    fprintf (stderr,
    "\nRaw Photo Decoder \"dcraw\" v8.17"
    "\nby Dave Coffin, dcoffin a cybercom o net"
    "\n\nUsage:  %s [options] file1 file2 ...\n"
    "\nValid options:"
    "\n-v        Print verbose messages"
    "\n-c        Write image data to standard output"
    "\n-e        Extract embedded thumbnail image"
    "\n-i        Identify files without decoding them"
    "\n-z        Change file dates to camera timestamp"
    "\n-a        Use automatic white balance"
    "\n-w        Use camera white balance, if possible"
    "\n-r <nums> Set raw white balance (four values required)"
    "\n-b <num>  Adjust brightness (default = 1.0)"
    "\n-k <num>  Set black point"
    "\n-H [0-2]  Highlight mode (0=clip, 1=no clip, 2=recover)"
    "\n-t [0-7]  Flip image (0=none, 3=180, 5=90CCW, 6=90CW)"
    "\n-o [0-5]  Output colorspace (raw,sRGB,Adobe,Wide,ProPhoto,XYZ)"
#ifndef NO_LCMS
    "\n-o <file> Apply output ICC profile from file"
    "\n-p <file> Apply camera ICC profile from file or \"embed\""
#endif
    "\n-d        Document Mode (no color, no interpolation)"
    "\n-D        Document Mode without scaling (totally raw)"
    "\n-q [0-3]  Set the interpolation quality"
    "\n-h        Half-size color image (twice as fast as \"-q 0\")"
    "\n-f        Interpolate RGGB as four colors"
    "\n-B <domain> <range>  Apply bilateral filter to reduce noise"
    "\n-j        Show Fuji Super CCD images tilted 45 degrees"
    "\n-s        Use secondary pixels (Fuji Super CCD SR only)"
    "\n-2        Write 8-bit non-linear PPM (default)"
    "\n-4        Write 16-bit linear PPM"
    "\n-3        Write 16-bit linear PSD (Adobe Photoshop)"
    "\n\n", argv[0]);
    return 1;
  }

  argv[argc] = "";
  for (arg=1; argv[arg][0] == '-'; ) {
    opt = argv[arg++][1];
    if ((cp = strchr (sp="BbrktqH", opt)))
      for (i=0; i < "2141111"[cp-sp]-'0'; i++)
	if (!isdigit(argv[arg+i][0])) {
	  fprintf (stderr, "Non-numeric argument to \"-%c\"\n", opt);
	  return 1;
	}
    switch (opt)
    {
      case 'B':  sigma_d     = atof(argv[arg++]);
		 sigma_r     = atof(argv[arg++]);  break;
      case 'b':  bright      = atof(argv[arg++]);  break;
      case 'r':
	   FORC4 user_mul[c] = atof(argv[arg++]);  break;
      case 'k':  user_black  = atoi(argv[arg++]);  break;
      case 't':  user_flip   = atoi(argv[arg++]);  break;
      case 'q':  user_qual   = atoi(argv[arg++]);  break;
      case 'H':  highlight   = atoi(argv[arg++]);  break;
      case 'o':
	if (isdigit(argv[arg][0]) && !argv[arg][1])
	  output_color = atoi(argv[arg++]);
#ifndef NO_LCMS
	else     out_profile = argv[arg++];
	break;
      case 'p':  cam_profile = argv[arg++];
#endif
	break;
      case 'z':  timestamp_only    = 1;  break;
      case 'e':  thumbnail_only    = 1;  break;
      case 'i':  identify_only     = 1;  break;
      case 'c':  write_to_stdout   = 1;  break;
      case 'v':  verbose           = 1;  break;
      case 'h':  half_size         = 1;		/* "-h" implies "-f" */
      case 'f':  four_color_rgb    = 1;  break;
      case 'd':  document_mode     = 1;  break;
      case 'D':  document_mode     = 2;  break;
      case 'a':  use_auto_wb       = 1;  break;
      case 'w':  use_camera_wb     = 1;  break;
      case 'j':  use_fuji_rotate   = 0;  break;
      case 's':  use_secondary     = 1;  break;
      case 'n':  highlight         = 1;  break;
      case 'm':  output_color      = 0;  break;

      case '2':  write_image = write_ppm;   break;
      case '4':  write_image = write_ppm16; break;
      case '3':  write_image = write_psd;   break;

      default:
	fprintf (stderr, "Unknown option \"-%c\".\n", opt);
	return 1;
    }
  }
  if (arg == argc) {
    fprintf (stderr, "No files to process.\n");
    return 1;
  }
  if (write_to_stdout) {
    if (isatty(1)) {
      fprintf (stderr, "Will not write an image to the terminal!\n");
      return 1;
    }
#if defined(WIN32) || defined(DJGPP) || defined(__CYGWIN__)
    if (setmode(1,O_BINARY) < 0) {
      perror("setmode()");
      return 1;
    }
#endif
  }
  for ( ; arg < argc; arg++) {
    status = 1;
    image = NULL;
    if (setjmp (failure)) {
      if (fileno(ifp) > 2) fclose(ifp);
      if (fileno(ofp) > 2) fclose(ofp);
      if (image) free (image);
      status = 1;
      continue;
    }
    ifname = argv[arg];
    if (!(ifp = fopen (ifname, "rb"))) {
      perror (ifname);
      continue;
    }
    status = (identify(),!is_raw);
    if (timestamp_only) {
      if ((status = !timestamp))
	fprintf (stderr, "%s has no timestamp.\n", ifname);
      else if (identify_only)
	printf ("%10ld%10d %s\n", (long) timestamp, shot_order, ifname);
      else {
	if (verbose)
	  fprintf (stderr, "%s time set to %d.\n", ifname, (int) timestamp);
	ut.actime = ut.modtime = timestamp;
	utime (ifname, &ut);
      }
      goto next;
    }
    write_fun = write_image;
    if (thumbnail_only) {
      if ((status = !thumb_offset)) {
	fprintf (stderr, "%s has no thumbnail.\n", ifname);
	goto next;
      } else if (thumb_load_raw) {
	load_raw = thumb_load_raw;
	data_offset = thumb_offset;
	height = thumb_height;
	width  = thumb_width;
	filters = 0;
      } else {
	fseek (ifp, thumb_offset, SEEK_SET);
	write_fun = write_thumb;
	goto thumbnail;
      }
    }
    if (load_raw == kodak_ycbcr_load_raw) {
      height += height & 1;
      width  += width  & 1;
    }
    if (identify_only && verbose && make[0]) {
      printf ("\nFilename: %s\n", ifname);
      printf ("Timestamp: %s", ctime(&timestamp));
      printf ("Camera: %s %s\n", make, model);
      printf ("ISO speed: %d\n", (int) iso_speed);
      printf ("Shutter: ");
      if (shutter > 0 && shutter < 1)
	shutter = (printf ("1/"), 1 / shutter);
      printf ("%0.1f sec\n", shutter);
      printf ("Aperture: f/%0.1f\n", aperture);
      printf ("Focal Length: %d mm\n", (int) focal_len);
      printf ("Embedded ICC profile: %s\n", profile_length ? "yes":"no");
      printf ("Decodable with dcraw: %s\n", is_raw ? "yes":"no");
      if (thumb_offset)
	printf ("Thumb size:  %4d x %d\n", thumb_width, thumb_height);
      printf ("Full size:   %4d x %d\n", raw_width, raw_height);
    } else if (!is_raw)
      fprintf (stderr, "Cannot decode %s\n", ifname);
    if (!is_raw) goto next;
    if (user_flip >= 0)
      flip = user_flip;
    switch ((flip+3600) % 360) {
      case 270:  flip = 5;  break;
      case 180:  flip = 3;  break;
      case  90:  flip = 6;
    }
    shrink = half_size && filters;
    iheight = (height + shrink) >> shrink;
    iwidth  = (width  + shrink) >> shrink;
    if (identify_only) {
      if (verbose) {
	if (fuji_width && use_fuji_rotate) {
	  fuji_width = (fuji_width - 1 + shrink) >> shrink;
	  iwidth = fuji_width / sqrt(0.5);
	  iheight = (iheight - fuji_width) / sqrt(0.5);
	}
	if (write_fun == write_ppm) {
	  iheight *= ymag;
	  iwidth  *= xmag;
	}
	if (flip & 4)
	  SWAP (iheight, iwidth);
	printf ("Image size:  %4d x %d\n", width, height);
	printf ("Output size: %4d x %d\n", iwidth, iheight);
	printf ("Raw colors: %d", colors);
	if (filters) {
	  printf ("\nFilter pattern: ");
	  if (!cdesc[3]) cdesc[3] = 'G';
	  for (i=0; i < 32; i+=2)
	    putchar (cdesc[filters >> i & 3]);
	}
	printf ("\nDaylight multipliers:");
	FORCC printf (" %f", pre_mul[c]);
	if (cam_mul[0] > 0) {
	  printf ("\nCamera multipliers:");
	  FORC4 printf (" %f", cam_mul[c]);
	}
	putchar ('\n');
      } else
	printf ("%s is a %s %s image.\n", ifname, make, model);
next:
      fclose(ifp);
      continue;
    }
    image = calloc (iheight*iwidth*sizeof *image + meta_length, 1);
    merror (image, "main()");
    meta_data = (char *) (image + iheight*iwidth);
    if (verbose)
      fprintf (stderr,
	"Loading %s %s image from %s...\n", make, model, ifname);
    fseek (ifp, data_offset, SEEK_SET);
    (*load_raw)();
    bad_pixels();
    height = iheight;
    width  = iwidth;
    quality = 2 + !fuji_width;
    if (user_qual >= 0) quality = user_qual;
    if (user_black >= 0) black = user_black;
#ifdef COLORCHECK
    colorcheck();
#endif
    if (is_foveon && !document_mode) foveon_interpolate();
    if (!is_foveon && document_mode < 2) scale_colors();
    if (shrink) filters = 0;
    cam_to_cielab (NULL,NULL);
    if (filters && !document_mode) {
      if (quality == 0)
	lin_interpolate();
      else if (quality < 3 || colors > 3)
	   vng_interpolate();
      else ahd_interpolate();
    }
    if (sigma_d > 0 && sigma_r > 0) bilateral_filter();
    if (!is_foveon && highlight > 1) recover_highlights();
    if (use_fuji_rotate) fuji_rotate();
#ifndef NO_LCMS
    if (cam_profile) apply_profile (cam_profile, out_profile);
#endif
    convert_to_rgb();
    if (flip) flip_image();
thumbnail:
    if (write_fun == jpeg_thumb)
      write_ext = ".jpg";
    else if (write_fun == write_psd)
      write_ext = ".psd";
    else
      write_ext = ".pgm\0.ppm\0.ppm\0.pam" + colors*5-5;
    ofname = malloc (strlen(ifname) + 16);
    merror (ofname, "main()");
    if (write_to_stdout)
      strcpy (ofname, "standard output");
    else {
      strcpy (ofname, ifname);
      if ((cp = strrchr (ofname, '.'))) *cp = 0;
      if (thumbnail_only)
	strcat (ofname, ".thumb");
      strcat (ofname, write_ext);
      ofp = fopen (ofname, "wb");
      if (!ofp) {
	status = 1;
	perror(ofname);
	goto cleanup;
      }
    }
    if (verbose)
      fprintf (stderr, "Writing data to %s ...\n", ofname);
    (*write_fun)(ofp);
    fclose(ifp);
    if (ofp != stdout) fclose(ofp);
cleanup:
    free (ofname);
    free (image);
  }
  return status;
}
