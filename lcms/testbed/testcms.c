//
//  Little cms
//  Copyright (C) 1998-2006 Marti Maria
//
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Test Suite for Little cms

// #define ICM_COMPARATIVE      1
// #define CHECK_SPEED          1

#ifdef __BORLANDC__
#     include <condefs.h>
#endif

#include "lcms.h"



#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NON_WINDOWS
#include <icm.h>
#endif


#define PREC  20

#define TYPE_XYZA_16            (COLORSPACE_SH(PT_XYZ)|CHANNELS_SH(3)|BYTES_SH(2)|EXTRA_SH(1))
#define TYPE_LABA_16            (COLORSPACE_SH(PT_Lab)|CHANNELS_SH(3)|BYTES_SH(2)|EXTRA_SH(1))

typedef struct {BYTE r, g, b, a;}   Scanline_rgb1;
typedef struct {WORD r, g, b, a;}   Scanline_rgb2;
typedef struct {BYTE r, g, b;}      Scanline_rgb8;
typedef struct {WORD r, g, b;}      Scanline_rgb0;


// Print a dot for gauging

static 
void Dot(void)
{
    fprintf(stdout, "."); fflush(stdout);
}

// #ifndef LCMS_DLL

// Are we little or big endian?  From Harbison&Steele.  

static
int CheckEndianess(void)
{
   int BigEndian, IsOk;   
   union {

     long l;
     char c[sizeof (long)];
   
   } u;

   u.l = 1;
   BigEndian = (u.c[sizeof (long) - 1] == 1);

#ifdef USE_BIG_ENDIAN
        IsOk = BigEndian;
#else
        IsOk = !BigEndian;
#endif

        if (!IsOk) {

            printf("\nOOOPPSS! You have USE_BIG_ENDIAN toggle misconfigured!\n\n");
            printf("Please, edit lcms.h and %s the USE_BIG_ENDIAN toggle.\n", BigEndian? "uncomment" : "comment");
            return 0;
        }

        return 1;

}


static
int CheckSwab(void)
{
    unsigned char Test[] = { 1, 2, 3, 4, 5, 6};

#ifdef USE_CUSTOM_SWAB
        return 1;
#endif

#ifdef USE_BIG_ENDIAN
        return 1;
#endif

    swab((char*) Test, (char*) Test, 6);

    if (strncmp((char*) Test, "\x2\x1\x4\x3\x6\x5", 6) != 0)
    {
            printf("\nOOOPPSS! swab() does not work as expected in your machine!\n\n");
            printf("Please, edit lcms.h and uncomment the USE_CUSTOM_SWAB toggle.\n");
            return 0;

    }
    return 1;
}


static
int CheckQuickFloor(void)
{
        
    if ((_cmsQuickFloor(1.234) != 1) ||
        (_cmsQuickFloor(32767.234) != 32767) ||
        (_cmsQuickFloor(-1.234) != -2) ||
        (_cmsQuickFloor(-32767.1) != -32768)) {

                printf("\nOOOPPSS! _cmsFloor() does not work as expected in your machine!\n\n");
                printf("Please, edit lcms.h and uncomment the LCMS_DEFAULT_FLOOR_CONVERSION toggle.\n");
                return 0;

    }

    return 1;
}

typedef struct _Stats {
                      double n, x, y, x2, y2, xy;
                      double Peak;
                      } STATS, FAR* LPSTATS;

static void ClearStats(LPSTATS p)
{       
       p -> n = p -> x = p -> y = p -> x2 = p -> y2 = p -> xy 
       = p -> Peak = 0.0;
}

static double Std(LPSTATS p)
{
       return sqrt((p->n*p->x2 - p->x * p->x) / (p->n*(p->n-1)));
}



static
void PrintStatistics(clock_t atime, LPSTATS Stats)
{

       clock_t diff;
       double a;

       diff = clock() - atime;
       a = (double) diff / CLOCKS_PER_SEC;

       // These are statistics of 16 bit, so divide
       // by 257 to get dE relative to 8 bits

       printf("\n");

       if (Stats) 
          printf("dE: mean=%g, SD=%g, max=%g ",
                     (Stats->x / Stats -> n) / 257.,
                     (Std(Stats)) / 257.,
                     Stats -> Peak / 257.);       
           

       if (atime > 0)
            printf("[%d tics, %g sec.]", (int) diff, a);

}


// Simpler fixed-point math

static
void TestFixedPoint(void)
{
       Fixed32 a, b, c, d; 
       double f;

       a = DOUBLE_TO_FIXED(1.1234);
       b = DOUBLE_TO_FIXED(2.5678);
       
       c = FixedMul(a, b);
      
       d = FIXED_REST_TO_INT(c);
       f = ((double) d / 0xffff) * 1000000.0;

       printf("Testing fixed point:\t%f = %d.%d\n", (1.1234 * 2.5678), FIXED_TO_INT(c), (int) f);
      
}



static
int TestFixedScaling(void)
{
       int i, j, nfl, nfx;
       double FloatFactor;
       Fixed32 FixedFactor;



       printf("Testing fixed scaling...");

       for (j=5; j<100; j++)
       {
       FloatFactor = (double) j / 100.0 ;
       FloatFactor = FIXED_TO_DOUBLE(DOUBLE_TO_FIXED(FloatFactor));
       FixedFactor = DOUBLE_TO_FIXED(FloatFactor);

       for (i=0; i < 0x10000L; i++)
              {
                     nfl = (WORD) ((double) i * FloatFactor);
                     nfx = FixedScale((WORD) i, FixedFactor);

                     if (nfl != nfx) {
                            printf("Failed!\ni=%x (%d), float=%x, fixed=%x", i, i, nfl, nfx);
                            return 0;
                            }
              }

       }

       printf ("pass.\n");
       return 1;
}

// Curve joining test. Joining two high-gamma of 3.0 curves should
// give something like linear

static
int TestJointCurves(void)
{
    LPGAMMATABLE Forward, Reverse, Result;
    BOOL rc;

    printf("Testing curves join ...");

    Forward = cmsBuildGamma(256, 3.0);
    Reverse = cmsBuildGamma(256, 3.0);

    Result = cmsJoinGammaEx(Forward, Reverse, 256);

    cmsFreeGamma(Forward); cmsFreeGamma(Reverse); 

    rc = cmsIsLinear(Result->GammaTable, Result ->nEntries);
    cmsFreeGamma(Result); 

    if (!rc) {
        printf("failed!\n");
        return 0;
    }
    else {
        printf("pass.\n");
        return 1;
    }
    
}



// Check reversing of gamma curves

#define NPOINTS     1024

static
int TestReversingOfCurves(void)
{
    LPGAMMATABLE Gamma, Reverse, Computed;
    int i;
    double dE;
    STATS Stats;

    printf("Testing reversing of curves ...");
    ClearStats(&Stats);


    Gamma   = cmsBuildGamma(NPOINTS, 3.0);
    Reverse = cmsBuildGamma(NPOINTS, 1.0/3.0);

    Computed = cmsReverseGamma(NPOINTS, Gamma);

    for (i=0; i < NPOINTS; i++) {

            dE = fabs(Reverse->GammaTable[i] - Computed->GammaTable[i]);

            Stats.x += dE;                                   
            Stats.x2 += (dE * dE);
            Stats.n += 1.0;
            if (dE > Stats.Peak) {
                Stats.Peak = dE;                                
            }

            if (dE > 0x0010) {
                printf("Coarse error! %x on entry %d: %X/%X", (int) dE, i, Reverse->GammaTable[i],
                                                                           Computed->GammaTable[i]);
                return 0;
            }
                                   
    }
    
    PrintStatistics(0, &Stats); 
    printf(" pass.\n");
    cmsFreeGamma(Gamma);
    cmsFreeGamma(Reverse);
    cmsFreeGamma(Computed);
    return 1;
}

// Linear interpolation test. Here I check the cmsLinearInterpLUT16
// Tables are supposed to be monotonic, but the algorithm works on
// non-monotonic as well.

static
int TestLinearInterpolation(int lExhaustive)
{
       static WORD Tab[4098];
       int j, i, k = 0;
       L16PARAMS p;
       int n;
       clock_t time;

       printf("Testing linear interpolation ...");

       // First I will check exact values. Since prime factors of 65535 (FFFF) are,
       //
       //            0xFFFF = 1 * 3 * 5 * 17 * 257
       //
       // I test tables of 2, 4, 6, and 18 points, that will be exact.
       // Then, a table of 3 elements are tested. Error must be < 1
       // Since no floating point is involved, This will be a measure of speed.


       // Perform 10 times, so i can measure average times

       time = clock();
       for (j=0; j < 10; j++)
       {

       // 2 points - exact

       Tab[0] = 0;
       Tab[1] = 0xffffU;

       cmsCalcL16Params(2, &p);

       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (n != i)
                     {
                     printf("Error in Linear interpolation (2p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }

       }


       // 3 points - Here the error must be <= 1, since
       // 2 == (3 - 1)  is not a factor of 0xffff

       Tab[0] = 0;
       Tab[1] = 0x7FFF;
       Tab[2] = 0xffffU;

       cmsCalcL16Params(3, &p);

       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (abs(n - i) > 1)
                     {
                     printf("Error in Linear interpolation (3p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }

       }


       // 4 points - exact

       Tab[0] = 0;
       Tab[1] = 0x5555U;
       Tab[2] = 0xAAAAU;
       Tab[3] = 0xffffU;

       cmsCalcL16Params(4, &p);

       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (n != i) {
                     printf("Error in Linear interpolation (4p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }

       }


       // 6 - points

       Tab[0] = 0;
       Tab[1] = 0x3333U;
       Tab[2] = 0x6666U;
       Tab[3] = 0x9999U;
       Tab[4] = 0xCCCCU;
       Tab[5] = 0xFFFFU;

       cmsCalcL16Params(6, &p);

       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (n != i) {
                     printf("Error in Linear interpolation (6p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }

       }


       // 18 points

       for (i=0; i < 18; i++)
              Tab[i] = (WORD) (0x0f0fU*i);

       cmsCalcL16Params(18, &p);

       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (n != i) {
                     printf("Error in Linear interpolation (18p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }
       }
       }



       printf("pass. (%d tics)\n", (int) (clock() - time));

       // Now test descending tables
       printf("Testing descending tables (linear interpolation)...");

       // 2 points - exact

       Tab[1] = 0;
       Tab[0] = 0xffffU;

       cmsCalcL16Params(2, &p);

       for (i=0xffffL; i > 0; --i)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if ((0xffffL - n) != i) {

                     printf("Error in Linear interpolation (descending) (2p): Must be i=%x, But is n=%x\n", i, 0xffff - n);
                     return 0;
                     }
       }


       // 3 points - Here the error must be <= 1, since
       // 2 = (3 - 1)  is not a factor of 0xffff

       Tab[2] = 0;
       Tab[1] = 0x7FFF;
       Tab[0] = 0xffffU;

       cmsCalcL16Params(3, &p);

       for (i=0xffffL; i > 0; --i)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (abs((0xffffL - n) - i) > 1) {

                     printf("Error in Linear interpolation (descending) (3p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }
       }


       // 4 points - exact

       Tab[3] = 0;
       Tab[2] = 0x5555U;
       Tab[1] = 0xAAAAU;
       Tab[0] = 0xffffU;

       cmsCalcL16Params(4, &p);

       for (i=0xffffL; i > 0; --i)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if ((0xffffL - n) != i) {

                     printf("Error in Linear interpolation (descending) (4p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }
       }


       // 6 - points

       Tab[5] = 0;
       Tab[4] = 0x3333U;
       Tab[3] = 0x6666U;
       Tab[2] = 0x9999U;
       Tab[1] = 0xCCCCU;
       Tab[0] = 0xFFFFU;

       cmsCalcL16Params(6, &p);

       for (i=0xffffL; i > 0; --i)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if ((0xffffL - n) != i) {
                     printf("Error in Linear interpolation (descending) (6p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }

       }


       // 18 points

       for (i=0; i < 18; i++)
              Tab[17-i] = (WORD) (0x0f0fU*i);

       cmsCalcL16Params(18, &p);

       for (i=0xffffL; i > 0; --i)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if ((0xffffL - n) != i) {

                     printf("Error in Linear interpolation (descending) (18p): Must be i=%x, But is n=%x\n", i, n);
                     return 0;
                     }
       }

       printf("pass.\n");

       if (!lExhaustive) return 1;

       printf("Now, testing interpolation errors for tables of n elements ...\n");

       for (j=10; j < 4096; j ++)
       {
       if ((j % 10) == 0) printf("%d\r", j);

       for (i=0; i <= j; i++)
              {
              Tab[i] = (WORD) floor((((double) i / ((double) j-1)) * 65535.0) + .5);
              }

       k =0;
       cmsCalcL16Params(j, &p);
       for (i=0; i <= 0xffffL; i++)
       {
              n = cmsLinearInterpLUT16((WORD) i, Tab, &p);
              if (n != i) k++;

       }
       
       }
       printf("\n%d: %d errors\n\n", j, k);
       return 1;
}



static
int IsGood(const char *frm, WORD in, WORD out)
{

        // 1 for rounding
        if ((abs(in - out) > 1)) {

              printf("error %s %x - %x\n", frm, in, out);
              return 0;
              }

       return 1;
}

static
BOOL TestReverseLinearInterpolation(void)
{
        WORD Tab[20];
        L16PARAMS p;
        int i, n, v;

        printf("Testing reverse linear interpolation\n");


        cmsCalcL16Params(16, &p);

        for (i=0; i < 16; i++) Tab[i] = (WORD) i * 0x1111;

        printf("\ton normal monotonic curve...");
        for (i=0; i < 16; i++)
        {
              v = (i * 0x1111);
              n = cmsReverseLinearInterpLUT16((WORD) v, Tab, &p);
              if (!IsGood("unexpected result", (WORD) v, (WORD) n))
                        return FALSE;
       }
       printf("pass.\n");


        Tab[0] = 0;
        Tab[1] = 0;
        Tab[2] = 0;
        Tab[3] = 0;
        Tab[4] = 0;
        Tab[5] = 0x5555;
        Tab[6] = 0x6666;
        Tab[7] = 0x7777;
        Tab[8] = 0x8888;
        Tab[9] = 0x9999;
        Tab[10]= 0xffff;
        Tab[11]= 0xffff;
        Tab[12]= 0xffff;
        Tab[13]= 0xffff;
        Tab[14]= 0xffff;
        Tab[15]= 0xffff;


        printf("\ton degenerated curve ...");

        for (i=0; i < 16; i++)
        {
              v = (i * 0x1111);
              n = cmsReverseLinearInterpLUT16((WORD) v, Tab, &p);

              if (i > 5 && i <= 9) {

              if (!IsGood("unexpected result", (WORD)  v, (WORD) n))
                        return FALSE;
              }
       }

       printf("pass.\n");

     return TRUE;
}




// 3D LUT test

static
int Test3D(void)
{
   LPLUT MyLut;
   LPWORD Table;
   WORD In[3], Out[3];
   int r, g, b, i;
   double *SampleTablePtr, SampleTable[] = { //R     G    B

                                              0,    0,   0,     // B=0,G=0,R=0
                                              0,    0,  .25,    // B=1,G=0,R=0

                                              0,   .5,    0,    // B=0,G=1,R=0
                                              0,   .5,  .25,    // B=1,G=1,R=0

                                              1,    0,    0,    // B=0,G=0,R=1
                                              1,    0,  .25,     // B=1,G=0,R=1

                                              1,    .5,   0,    // B=0,G=1,R=1
                                              1,    .5,  .25    // B=1,G=1,R=1

                                              };

    
      printf("Testing 3D interpolation on LUT...");
      
      // 1.- Allocate an empty LUT

      MyLut = cmsAllocLUT();

      // 2.- In this LUT, allocate a 3D grid of 2 points, from 3 components (RGB)
      //     to 3 components. First 3 is input dimension, last 3 is output one.
      //         2 is number of grid points.

      MyLut = cmsAlloc3DGrid(MyLut, 2, 3, 3);

      // 3.- Fill the LUT table with values.

      Table = MyLut -> T;

      SampleTablePtr = SampleTable;

      for (i= 0; i < 3; i++)
       for (r = 0; r < 2; r++)
         for (g = 0; g < 2; g++)
          for (b = 0; b < 2; b++) {

            WORD a =  (WORD) floor(*SampleTablePtr++ * 65535. + .5);

            *Table++ = a;
        }


   // The sample table gives
   //
   //        r = input,
   //           g = input divided by 2
   //            b = input divided by 4
   //
   // So, I should obtain on output r, g/2, g/4


   for (i=0; i < 0xffff; i++) {

       In[0] = In[1] = In[2] = (WORD) i;

       cmsEvalLUT(MyLut, In, Out);


       
        // Check results, I will tolerate error <= 1  for rounding

       

       if (!IsGood("Channel 1", Out[0], In[0])) return 0;
       if (!IsGood("Channel 2", Out[1], (WORD) ((double) In[1] / 2))) return 0;
       if (!IsGood("Channel 3", Out[2], (WORD) ((double) In[2] / 4))) return 0;

      }

   
   
   // Last, remember free stuff

   cmsFreeLUT(MyLut);

   printf("pass.\n");
   return 1;
}



static
void PrintMatrix(LPMAT3 lpM)
{
       int i, j;

       for (i=0; i < 3; i++) {
              printf ("[ ");
              for (j=0; j < 3; j++)
                     {
                            printf("%1.6f  ", (*lpM).v[i].n[j]);
                     }
              printf("]\n");
       }

       printf("\n");

}


static
BOOL CmpMatrix(LPMAT3 lpM1, LPMAT3 lpM2, double tolerance)
{
       int i, j;

       for (i=0; i < 3; i++) {
              for (j=0; j < 3; j++) {
                        if (fabs(lpM1 -> v[i].n[j] - lpM2 -> v[i].n[j]) > tolerance)
                                        return FALSE;
                     }

       }

       return TRUE;

}


static
BOOL TestMatrixCreation(void)
{
       MAT3 Mat;
       int rc;

       cmsCIExyY WhitePt =  {0.3127, 0.3290, 1.0};
       cmsCIExyYTRIPLE Primaries = {
                                   {0.6400, 0.3300, 1.0},
                                   {0.3000, 0.6000, 1.0},
                                   {0.1500, 0.0600, 1.0}
                                   };
       MAT3 sRGB = {{
                   {{ 0.436066,  0.385147,  0.143066 }},
                   {{ 0.222488,  0.716873,  0.060608 }},
                   {{ 0.013916,  0.097076,  0.714096 }}
                   }};


       printf("Testing virtual profiles (Emulating sRGB)...");

       rc = cmsBuildRGB2XYZtransferMatrix(&Mat,
                                          &WhitePt,
                                          &Primaries);
       cmsAdaptMatrixToD50(&Mat, &WhitePt);

       if (rc < 0)
       {
       printf("TestMatrixCreation failed, rc = %d\n", rc);
       return FALSE;
       }
      

       if (!CmpMatrix(&Mat, &sRGB, 0.001)) {
                printf("FAILED!\n");
                printf("sRGB final matrix is:\n");
                PrintMatrix(&sRGB);
                printf("\nlcms calculated matrix is:\n");
                PrintMatrix(&Mat);
                return FALSE;
       }

       printf("pass.\n");
       return TRUE;


}


/*

       Used for debug purposes
*/

#if 0
static
void AdaptationMatrixTest(void)
{
       cmsCIExyY D65 = {0.3127, 0.329001, 1.0};   // D65
       MAT3 sRGB, TosRGB;


       VEC3init(&sRGB.v[0], 0.4124,  0.3576,  0.1805);
       VEC3init(&sRGB.v[1], 0.2126,  0.7152,  0.0722);
       VEC3init(&sRGB.v[2], 0.0193,  0.1192,  0.9505);

       cmsAdaptMatrixToD50(&sRGB, &D65);
       printf("Adaptation matrix D65 -> D50 (to PCS)\n");
       PrintMatrix(&sRGB);

       MAT3inverse(&sRGB, &TosRGB);
       printf("inverse\n");
       PrintMatrix(&TosRGB);

       cmsAdaptMatrixFromD50(&TosRGB, &D65);
       printf("adaptated to D65\n");
       PrintMatrix(&TosRGB);
}
#endif
// #endif



static
double VecDist(Scanline_rgb2 *bin, Scanline_rgb2 *bout)
{
       double rdist, gdist, bdist;

       rdist = fabs(bout -> r - bin -> r);
       gdist = fabs(bout -> g - bin -> g);
       bdist = fabs(bout -> b - bin -> b);

       return (sqrt((rdist*rdist + gdist*gdist + bdist*bdist)));
}






// Perform sampling in the full spectrum & acotate error.
// I choose red for the lowest incidence in eye.
// Green is most lightful, eye is most accurate on blue.

static
int TestFullSpectrum(cmsHTRANSFORM xform, int nRedInterv, int MaxErr)
{
       int r, g, b;
       double err;
       Scanline_rgb2 *bin, *bout;
       STATS Stats;
       clock_t t;


       bin  = (Scanline_rgb2 *) malloc(256*sizeof(Scanline_rgb2));
       bout = (Scanline_rgb2 *) malloc(256*sizeof(Scanline_rgb2));


       ClearStats(&Stats);

       Stats.x = 0.0; Stats.n = 0.0; // GCC BUG HERE!!!!

       t = clock();

       for (r=0; r < 256; r+= nRedInterv)
       {
              // printf("\r%02x:", r);

              Dot();
              for (g=0; g < 256; g++)
                     {

                            for (b=0; b < 256; b++)
                            {
                            bin[b].r = (WORD) r << 8;          // For L 0nly to 0xFF00
                            bin[b].g = RGB_8_TO_16(g);
                            bin[b].b = RGB_8_TO_16(b);
                            bin[b].a = 0;
                            }

                            cmsDoTransform(xform, bin, bout, 256);

                            // I'm using b as index

                            for (b=0; b < 256; b ++)
                            {
                                   // I measure the error using vector distance

                                   err = VecDist(bin+b, bout+b);
                                   Stats.x += (double) err;
                                   Stats.x2 += (double) err * err;
                                   Stats.n += 1.0;
                                   if (err > Stats.Peak)
                                          Stats.Peak = err;


                                   if (err > MaxErr)
                                   {
                                          printf("Coarse error! : In=(%x,%x,%x) Out=(%x,%x,%x)\n",
                                                        bin[b].r, bin[b].g, bin[b].b,
                                                        bout[b].r, bout[b].g, bout[b].b);
                                          free(bin);
                                          free(bout);
                                          return 0;
                                   }
                            }

                     }

       }


       PrintStatistics(t, &Stats);
       free(bin);
       free(bout);

       return 1;
}




static
int TestInducedError(DWORD Type)
{
       cmsHPROFILE In, Out;
       cmsHTRANSFORM xform;
       int nMaxError;

       In  = cmsCreateLabProfile(NULL);
       Out = cmsCreateLabProfile(NULL);

       printf("Error Induced by the CMM due to roundoff (dE) ");

       xform = cmsCreateTransform(In,  Type,
                                  Out, Type,
                                  INTENT_RELATIVE_COLORIMETRIC, 0);
              

       nMaxError = TestFullSpectrum(xform, 31, 0x800);

       printf("\n");

       cmsDeleteTransform(xform);
       cmsCloseProfile(In);
       cmsCloseProfile(Out);

       return nMaxError;
}


static
double ConvertL(WORD v)
{
       int fix32;

       fix32 = v;
       return (double)fix32/652.800;    /* 0xff00/100.0 */
}


static
double Convertab(WORD v)
{
       int fix32;


       fix32 = v;

       return ((double)fix32/256.0)-128.0;
}


#define BASE  255

static
int CompareTransforms(cmsHTRANSFORM xform1, cmsHTRANSFORM xform2, 
                      int nRedInterv, int lWithStats, BOOL lIsLab)
{
       int r, g, b;
       double err;
       Scanline_rgb2 *bin, *bout1, *bout2;
       STATS Stats;
       int OutOfGamut = 0;


       bin   = (Scanline_rgb2 *) malloc(256*sizeof(Scanline_rgb2));
       bout1 = (Scanline_rgb2 *) malloc(256*sizeof(Scanline_rgb2));
       bout2 = (Scanline_rgb2 *) malloc(256*sizeof(Scanline_rgb2));


       ClearStats(&Stats);
       Stats.x = 0.0; Stats.n = 0.0; // GCC BUG HERE!!!!


       for (r=0; r <= BASE; r+= nRedInterv)
       {
              // printf("\r%02x:", r);

              Dot();
              for (g=0; g <= BASE; g++)
                     {
                            // I will test random LSB

                            for (b=0; b <= BASE; b++)     // 256
                            {

                            bin[b].r = RGB_8_TO_16(r);
                            bin[b].g = RGB_8_TO_16(g);
                            bin[b].b = RGB_8_TO_16(b);
                            bin[b].a = 0;
                            }

                            cmsDoTransform(xform1, bin, bout1, 256);
                            cmsDoTransform(xform2, bin, bout2, 256);

                            // I'm using b as index

                            for (b=0; b <= BASE; b ++) {

                                   // I measure the error using vector distance
                                   // Only if encodable values

                              if (bout1[b].r != 0xffff && bout1[b].g != 0xffff && bout1[b].b != 0xffff)
                              {

                                   err = VecDist(bout1+b, bout2+b);


                                   if (err > 0x1000L)
                                   {

                                       if (lIsLab) {
                                          printf("Coarse error: In=(%x,%x,%x) Out1=(%g,%g,%g) Out2=(%g,%g,%g)\n",
                                                        bin[b].r, bin[b].g, bin[b].b,
                                                        ConvertL(bout1[b].r), Convertab(bout1[b].g), Convertab(bout1[b].b),
                                                        ConvertL(bout2[b].r), Convertab(bout2[b].g), Convertab(bout2[b].b));
                                       }
                                       else
                                       {
                                        printf("Coarse error: In=(%x,%x,%x) Out1=(%x,%x,%x) Out2=(%x,%x,%x)\n",
                                                        bin[b].r, bin[b].g, bin[b].b,
                                                        bout1[b].r, bout1[b].g, bout1[b].b,
                                                        bout2[b].r, bout2[b].g, bout2[b].b);
                                       }
                                       return 0;

                                   }

                                   else
                                   {
                                   Stats.x += (double) err;
                                   Stats.x2 += (double) err * err;
                                   Stats.n += 1.0;
                                   if (err > Stats.Peak)
                                          Stats.Peak = err;
                                   }
                              } else
                                   OutOfGamut++;
                            }

                     }

       }


       if (lWithStats) {

        PrintStatistics(0, &Stats);
        printf(" pass.\n");
       }

       if (OutOfGamut > 0)
                printf("Out of encodeable representation=%d\n\n", OutOfGamut);
     

       free(bin);
       free(bout1);
       free(bout2);

       return 1;
}



static
BOOL CheckXYZ(LPcmsCIEXYZ Check, double X, double Y, double Z)
{
    return ((fabs(Check->X - X) < 0.001) && 
            (fabs(Check->Y - Y) < 0.001) &&
            (fabs(Check->Z - Z) < 0.001));
}


static
LPGAMMATABLE Build_sRGBGamma(void)
{
    double Parameters[5];

    Parameters[0] = 2.4;
    Parameters[1] = 1. / 1.055;
    Parameters[2] = 0.055 / 1.055;
    Parameters[3] = 1. / 12.92;
    Parameters[4] = 0.04045;    // d

    return cmsBuildParametricGamma(1024, 4, Parameters);
}

static
BOOL Check_sRGBGamma(LPGAMMATABLE Shape)
{
    LPGAMMATABLE sRGB = Build_sRGBGamma();
    int i;

    if (Shape ->nEntries != 1024) { 
        printf("because wrong sizes (%d != 1024), ", Shape -> nEntries); 
        return 0;
    }


    for (i=0; i < Shape -> nEntries; i++) {
        double nErr = Shape ->GammaTable[i] - sRGB ->GammaTable[i];

        if (fabs(nErr) > 1.0) {
                    int j;
                    printf("because %x != %x on index %d\n", Shape ->GammaTable[i], sRGB ->GammaTable[i], i);
                    printf("table dump follows:\n");
                    for (j=0; j < 10; j++)
                            printf("%d) %X\n", j, Shape ->GammaTable[j]);
                    printf("\nso, ");

                    return 0;
        }
    }

    cmsFreeGamma(sRGB);
    return 1;
}


static
int GetInfoTest(void)
{
    cmsHPROFILE hProfile;
    cmsCIEXYZ WhitePoint;
    cmsCIEXYZTRIPLE Primaries;
    const char* Product;
    LPGAMMATABLE Shapes[3];


    printf("Testing profile decoding (sRGB)");
    hProfile = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "rb");
    cmsTakeMediaWhitePoint(&WhitePoint, hProfile);

    Dot();
    if (!CheckXYZ(&WhitePoint, 0.95045, 1.0, 1.08905)) {
        printf("White point read failed!\n");
        return 0;
    }

    Dot();
    cmsTakeColorants(&Primaries, hProfile);

    if (!CheckXYZ(&Primaries.Red, 0.43607, 0.22249, 0.01392)) {
        printf("Red colorant failed!\n");
        return 0;
    }

    if (!CheckXYZ(&Primaries.Green, 0.38515,0.71617, 0.09708)) {
        printf("Green colorant failed!\n");
        return 0;
    }

    if (!CheckXYZ(&Primaries.Blue, 0.14307, 0.06061, 0.71410)) {
        printf("Blue colorant failed!\n");
        return 0;
    }

    Dot();
    Product = cmsTakeProductName(hProfile);
    if (strcmp(Product, "IEC 61966-2.1 Default RGB colour space - sRGB") != 0) {        
    printf("Product name mismatch!\n");
    }

    Dot();
    Shapes[0] = cmsReadICCGamma(hProfile, icSigRedTRCTag);
    Shapes[1] = cmsReadICCGamma(hProfile, icSigGreenTRCTag);
    Shapes[2] = cmsReadICCGamma(hProfile, icSigBlueTRCTag);

    if (!Check_sRGBGamma(Shapes[0]) ||
        !Check_sRGBGamma(Shapes[1]) ||
        !Check_sRGBGamma(Shapes[2])) {
            printf("Gamma curves mismatch!\n");
            return 0;
    }


    cmsFreeGammaTriple(Shapes);
    

    Dot();
    cmsCloseProfile(hProfile);
    printf("pass.\n");
    return 1;

}
static
int Test_sRGB(void)
{
       cmsHPROFILE In1, In2, Out1, Out2;
       cmsHTRANSFORM xform1, xform2;
       int nMaxErr;

       printf("Testing sRGB built-in space");
       
       In1   = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "rb");
       Out1  = cmsCreateXYZProfile();

       In2   = cmsCreate_sRGBProfile();
       Out2  = cmsCreateXYZProfile();

       xform1 = cmsCreateTransform(In1, TYPE_RGBA_16, Out1, TYPE_XYZA_16, 0, cmsFLAGS_NOTPRECALC);
       xform2 = cmsCreateTransform(In2, TYPE_RGBA_16, Out2, TYPE_XYZA_16, 0, cmsFLAGS_NOTPRECALC);

       nMaxErr = CompareTransforms(xform1, xform2, 31, TRUE, FALSE);

       cmsDeleteTransform(xform1);
       cmsCloseProfile(In1);
       cmsCloseProfile(Out1);

       cmsDeleteTransform(xform2);
       cmsCloseProfile(In2);
       cmsCloseProfile(Out2);

       return nMaxErr;

}

static
int RealProfilesTest(void)
{
       cmsHPROFILE In1, In2, Out1, Out2;
       cmsHTRANSFORM xform1, xform2;
       int nMaxErr;

       printf("Using  two real profiles");

       // sRGB is a simpler, public domain XYZ PCS profile
       // sRGBSpac comes with Win95 Platform SDK, in the public domain.
       //            (not latest revisions)

       // Using LAB identity as output profile, I'm forcing an
       // implicit XYZ => L*a*b conversion in xform1.
       // xform2 is 8 bits - LUT based, and PCS is L*a*b

       In1   = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "rb");
       Out1  = cmsCreateXYZProfile();

       In2   = cmsOpenProfileFromFile("sRGBSpac.icm", "rb");
       Out2  = cmsCreateXYZProfile();

      
       // Since LUT is 8-bits width,
       xform1 = cmsCreateTransform(In1, TYPE_RGBA_16, Out1, TYPE_XYZA_16, 0, cmsFLAGS_NOTPRECALC|cmsFLAGS_MATRIXINPUT);
       xform2 = cmsCreateTransform(In2, TYPE_RGBA_16, Out2, TYPE_XYZA_16, 0, cmsFLAGS_NOTPRECALC);

       nMaxErr = CompareTransforms(xform1, xform2, 31, FALSE, FALSE);

       printf("pass\n");

       cmsDeleteTransform(xform1);
       cmsCloseProfile(In1);
       cmsCloseProfile(Out1);

       cmsDeleteTransform(xform2);
       cmsCloseProfile(In2);
       cmsCloseProfile(Out2);

       return nMaxErr;
}



// ---------------------------------------------------------


static
int TestPreview(void)
{
       cmsHPROFILE In, Out, Proof;
       cmsHTRANSFORM xform;
       int nMaxErr;

       printf("Testing preview");

       In    = cmsCreateLabProfile(NULL);
       Out   = cmsCreateLabProfile(NULL);
       Proof = cmsCreateLabProfile(NULL);

       xform = cmsCreateProofingTransform(In, TYPE_LABA_16, Out, TYPE_LABA_16, Proof, 0, 0, cmsFLAGS_SOFTPROOFING);

       nMaxErr = TestFullSpectrum(xform, 31, 0x1000L);

       cmsDeleteTransform(xform);
       cmsCloseProfile(In);
       cmsCloseProfile(Out);
       cmsCloseProfile(Proof);
       printf("\n");

       return nMaxErr;
}


// Check induced error on multiprofile transforms

static
int TestMultiprofile(void)
{

    cmsHPROFILE hsRGB, hXYZ, hLab;
    cmsHTRANSFORM hXForm;
    cmsHPROFILE Profiles[10];
    int nMaxErr;

    hsRGB = cmsCreate_sRGBProfile();
    hLab = cmsCreateLabProfile(NULL);
    hXYZ = cmsCreateXYZProfile();

    Profiles[0] = hsRGB;
    Profiles[1] = hLab;
    Profiles[2] = hsRGB;        
    Profiles[3] = hsRGB;        

    Profiles[4] = hLab;
    Profiles[5] = hXYZ;
    Profiles[6] = hsRGB;

    hXForm = cmsCreateMultiprofileTransform(Profiles, 7, TYPE_RGBA_16, TYPE_RGBA_16, INTENT_RELATIVE_COLORIMETRIC, cmsFLAGS_HIGHRESPRECALC);
    
    printf("Testing multiprofile transforms (6 profiles)");

    nMaxErr = TestFullSpectrum(hXForm, 31, 0x1000L);

    cmsDeleteTransform(hXForm);
    cmsCloseProfile(hsRGB);
    cmsCloseProfile(hXYZ);
    cmsCloseProfile(hLab);

    printf("\n");

    return nMaxErr;
}

// Check linearization and other goodies

static
int TestLinearizationDevicelink()
{
    LPGAMMATABLE Transfer[3];
    cmsHPROFILE hLin1, hLin2;
    cmsHTRANSFORM hXForm;
    cmsHPROFILE Profiles[10];
    int nMaxErr;

    printf("Testing linearization devicelink");

    Transfer[0] = cmsBuildGamma(256, 1./2.2);
    Transfer[1] = cmsBuildGamma(256, 1./2.2);
    Transfer[2] = cmsBuildGamma(256, 1./2.2);

    hLin1 = cmsCreateLinearizationDeviceLink(icSigRgbData, Transfer);

    cmsFreeGammaTriple(Transfer);
    


    Transfer[0] = cmsBuildGamma(256, 2.2);
    Transfer[1] = cmsBuildGamma(256, 2.2);
    Transfer[2] = cmsBuildGamma(256, 2.2);

    hLin2 = cmsCreateLinearizationDeviceLink(icSigRgbData, Transfer);

    cmsFreeGammaTriple(Transfer);

    Profiles[0] = hLin1;
    Profiles[1] = hLin2;

    hXForm = cmsCreateMultiprofileTransform(Profiles, 2, TYPE_RGBA_16, TYPE_RGBA_16, INTENT_RELATIVE_COLORIMETRIC, cmsFLAGS_HIGHRESPRECALC);
    if (!hXForm) {

        printf("Error!\n");
        return 1;
    }
    

    nMaxErr = TestFullSpectrum(hXForm, 31, 0x1000L);

    cmsDeleteTransform(hXForm);
    cmsCloseProfile(hLin1);
    cmsCloseProfile(hLin2);

    printf("\n");

    return nMaxErr;
}


static
int TestLinearizationDevicelink2()
{
    LPGAMMATABLE Transfer[3];
    cmsHPROFILE hLin1;
    cmsHTRANSFORM hXForm;    
    int nMaxErr;

    printf("Testing saved linearization devicelink");

    Transfer[0] = cmsBuildGamma(256, 1);
    Transfer[1] = cmsBuildGamma(256, 1);
    Transfer[2] = cmsBuildGamma(256, 1);
    
    hLin1 = cmsCreateLinearizationDeviceLink(icSigRgbData, Transfer);

    _cmsSaveProfile(hLin1, "lin1.icc");
    cmsFreeGammaTriple(Transfer);
    cmsCloseProfile(hLin1);

    hLin1 = cmsOpenProfileFromFile("lin1.icc", "r");

    hXForm = cmsCreateTransform(hLin1, TYPE_RGBA_16, NULL, TYPE_RGBA_16, INTENT_ABSOLUTE_COLORIMETRIC, 0);

    if (!hXForm) {

        printf("Error!\n");
        return 1;
    }
    
    nMaxErr = TestFullSpectrum(hXForm, 31, 1);

    cmsDeleteTransform(hXForm);
    cmsCloseProfile(hLin1);
    unlink("lin1.icc");

    printf("\n");

    return nMaxErr;
}


static
int TestDeviceLinkGeneration()
{
    cmsHTRANSFORM hXForm, hIdentity;
    cmsHPROFILE hDevLink, hsRGB;
    int nMaxErr;


    printf("Testing devicelink generation");

    hsRGB     = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "r");
    hIdentity = cmsCreateTransform(hsRGB, TYPE_RGBA_16, hsRGB, TYPE_RGBA_16, INTENT_RELATIVE_COLORIMETRIC, 0);
    hDevLink  = cmsTransform2DeviceLink(hIdentity, 0);
    _cmsSaveProfile(hDevLink, "devicelink.icm");

    cmsCloseProfile(hDevLink);
    cmsCloseProfile(hsRGB);
    cmsDeleteTransform(hIdentity);
	

    hDevLink = cmsOpenProfileFromFile("devicelink.icm", "r");
    hXForm   = cmsCreateTransform(hDevLink, TYPE_RGBA_16, NULL, TYPE_RGBA_16, INTENT_RELATIVE_COLORIMETRIC, 0); 
    nMaxErr  = TestFullSpectrum(hXForm, 31, 0x1000L);

    cmsDeleteTransform(hXForm); 
    cmsCloseProfile(hDevLink);
    
    printf("\n");
    unlink("devicelink.icm");

    return nMaxErr;
}

static
int TestInkLimiting()
{
    cmsHPROFILE hIL;
    cmsHTRANSFORM hXForm;
    BYTE In[4], Out[4];
    int i, j, k, l, res;
    


    printf("Testing ink limiting ");
    
    hIL = cmsCreateInkLimitingDeviceLink(icSigCmykData, 100);

    
    hXForm = cmsCreateTransform(hIL, TYPE_CMYK_8, NULL, TYPE_CMYK_8, INTENT_RELATIVE_COLORIMETRIC, 0);
    if (!hXForm) {

        printf("Error!\n");
        return 0;
    }

    for (l=0; l < 255; l += 8) {
        Dot();      
        for (k=0; k < 255; k += 8) 
            for (j=0; j < 255; j += 8) 
                for (i=0; i < 255; i += 8) {

                    In[0] = (BYTE) i; In[1] = (BYTE) j; In[2] = (BYTE) k; In[3] = (BYTE) l;

                    cmsDoTransform(hXForm, In, Out, 1);

                    res = Out[0] + Out[1] + Out[2] + Out[3];
                        
                    if (res > 0x100) {
            
                        printf("Failed! (%d) \n", res);
                        return 0;   
                    }
        }
    }

    cmsDeleteTransform(hXForm);
    cmsCloseProfile(hIL);
    printf(" pass.\n");

    return 1;
}



static
void CheckPlanar(void)
{
    cmsHTRANSFORM xform;
    cmsHPROFILE hsRGB;
    int i;
    BYTE Out[12];
    BYTE Bmp[] = { 0x00, 0x10, 0x20, 0x30,   // R Plane  
                   0x00, 0x10, 0x20, 0x30,   // G Plane
                   0x00, 0x10, 0x20, 0x30 }; // B Plane
    

    
            hsRGB = cmsCreate_sRGBProfile();
            xform = cmsCreateTransform(hsRGB, TYPE_RGB_8_PLANAR, 
                                       hsRGB, TYPE_RGB_8_PLANAR, 
                                       INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC);

            cmsDoTransform(xform, Bmp, Out, 4);

            for (i=0; i < 12; i += 3) {
                    printf("RGB=(%x, %x, %x)\n", Out[i+0], Out[i+1], Out[i+2]);
            }

        cmsDeleteTransform(xform);
        cmsCloseProfile(hsRGB);
}

#ifdef ICM_COMPARATIVE
#ifndef NON_WINDOWS

static
void CompareWithICM_16bit(void)
{

    HTRANSFORM hICMxform;
    HPROFILE   hICMProfileFrom, hICMProfileTo;
    LOGCOLORSPACE LogColorSpace;
    COLOR In, Out;
	COLOR *InBlk, *OutBlk, *InPtr;
	size_t size;
    int r, g, b;
    PROFILE Profile;
    clock_t atime;
    double seconds, diff;
    cmsHPROFILE hlcmsProfileIn, hlcmsProfileOut;
    cmsHTRANSFORM hlcmsxform;


    printf("\n\nComparative with MS-Windows ICM (16 bits per sample):\n");
    

    Profile.dwType = PROFILE_FILENAME;
    Profile.pProfileData = "sRGBSpac.icm";
    Profile.cbDataSize   = strlen("sRGBSpac.icm");

    hICMProfileFrom = OpenColorProfile(&Profile, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

    Profile.pProfileData = "sRGBSpac.icm";
    Profile.cbDataSize   = strlen("sRGBSpac.icm");
    hICMProfileTo   = OpenColorProfile(&Profile, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

    ZeroMemory(&LogColorSpace, sizeof(LOGCOLORSPACE));

    LogColorSpace.lcsSignature = LCS_SIGNATURE;
    LogColorSpace.lcsVersion   = 0x400;
    LogColorSpace.lcsCSType    = LCS_CALIBRATED_RGB;
    strcpy(LogColorSpace.lcsFilename, "sRGBSpac.icm");

    hICMxform = CreateColorTransform(&LogColorSpace, hICMProfileTo, NULL, BEST_MODE);



	size = 256 * 256 * 256;
	InBlk = malloc((size_t) size * sizeof(COLOR));
	OutBlk = malloc((size_t) size * sizeof(COLOR));

	if (InBlk == NULL || OutBlk == NULL) {
		printf("Out of memory\n"); exit(2);
	}
			
    printf("Windows ICM is transforming full spectrum...");

	InPtr = InBlk;
	for (r=0; r < 255; r++)
        for (g=0; g < 255; g++)
            for (b=0; b < 255; b++) {

        InPtr->rgb.red   = (r << 8) | r;
        InPtr->rgb.green = (g << 8) | g;
        InPtr->rgb.blue  = (b << 8) | b;

		InPtr++;
	}

    atime = clock();
    
    TranslateColors( hICMxform, InBlk, size, COLOR_RGB, OutBlk, COLOR_RGB);
    
    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;


    printf("done. [%d tics, %g sec.]\n", (int) diff, seconds);
  
    CloseColorProfile(hICMProfileFrom);
    CloseColorProfile(hICMProfileTo);
    DeleteColorTransform(hICMxform);

    hlcmsProfileIn  = cmsOpenProfileFromFile("sRGBSpac.icm", "r");
    hlcmsProfileOut = cmsOpenProfileFromFile("sRGBSpac.icm", "r");

    hlcmsxform  = cmsCreateTransform(hlcmsProfileIn, TYPE_RGB_16, hlcmsProfileOut, TYPE_RGB_16, INTENT_PERCEPTUAL, 0);

    printf("lcms is transforming full spectrum...");

    atime = clock();

    cmsDoTransform(hlcmsxform, InBlk, OutBlk, size);
   
    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;

    printf("done. [%d tics, %g sec.]\n", (int) diff, seconds);

    cmsDeleteTransform(hlcmsxform);
    cmsCloseProfile(hlcmsProfileIn);
    cmsCloseProfile(hlcmsProfileOut);

	free(InBlk);
	free(OutBlk);
}

static
void CompareWithICM_8bit(void)
{

    HTRANSFORM hICMxform;
    HPROFILE   hICMProfileFrom, hICMProfileTo;
    LOGCOLORSPACE LogColorSpace;
    RGBQUAD In, Out;
    int r, g, b;
    PROFILE Profile;
    clock_t atime;
    double seconds, diff;
    cmsHPROFILE hlcmsProfileIn, hlcmsProfileOut;
    cmsHTRANSFORM hlcmsxform;


    printf("\n\nComparative with MS-Windows ICM (8 bits per sample):\n");
    

    Profile.dwType = PROFILE_FILENAME;
    Profile.pProfileData = "sRGBSpac.icm";
    Profile.cbDataSize   = strlen("sRGBSpac.icm");

    hICMProfileFrom = OpenColorProfile(&Profile, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

    Profile.pProfileData = "sRGBSpac.icm";
    Profile.cbDataSize   = strlen("sRGBSpac.icm");
    hICMProfileTo   = OpenColorProfile(&Profile, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

    ZeroMemory(&LogColorSpace, sizeof(LOGCOLORSPACE));

    LogColorSpace.lcsSignature = LCS_SIGNATURE;
    LogColorSpace.lcsVersion   = 0x400;
    LogColorSpace.lcsCSType    = LCS_CALIBRATED_RGB;
    strcpy(LogColorSpace.lcsFilename, "sRGBSpac.icm");

    hICMxform = CreateColorTransform(&LogColorSpace, hICMProfileTo, NULL, BEST_MODE);

    printf("Windows ICM is transforming full spectrum...");

    atime = clock();

    for (r=0; r < 255; r++)
        for (g=0; g < 255; g++)
            for (b=0; b < 255; b++) {

        In.rgbRed   = r;
        In.rgbGreen = g;
        In.rgbBlue  = b;
        
        if (!TranslateBitmapBits(hICMxform, &In,  BM_RGBTRIPLETS, 1, 1, 0, &Out, BM_RGBTRIPLETS, 0, NULL, 0))
            exit(2);
       
    }

    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;


    printf("done. [%d tics, %g sec.]\n", (int) diff, seconds);
  
    CloseColorProfile(hICMProfileFrom);
    CloseColorProfile(hICMProfileTo);
    DeleteColorTransform(hICMxform);

    hlcmsProfileIn  = cmsOpenProfileFromFile("sRGBSpac.icm", "r");
    hlcmsProfileOut = cmsOpenProfileFromFile("sRGBSpac.icm", "r");

    hlcmsxform  = cmsCreateTransform(hlcmsProfileIn, TYPE_BGRA_8, hlcmsProfileOut, TYPE_BGRA_8, INTENT_PERCEPTUAL, 0);

    printf("lcms is transforming full spectrum...");

    atime = clock();

    for (r=0; r < 255; r++)
        for (g=0; g < 255; g++)
            for (b=0; b < 255; b++) {

                In.rgbRed   = r;
                In.rgbGreen = g;
                In.rgbBlue  = b;
                
        cmsDoTransform(hlcmsxform, &In, &Out, 1);
    }

    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;

    printf("done. [%d tics, %g sec.]\n", (int) diff, seconds);

    cmsDeleteTransform(hlcmsxform);
    cmsCloseProfile(hlcmsProfileIn);
    cmsCloseProfile(hlcmsProfileOut);

}


#endif
#endif


#ifdef CHECK_SPEED



static
void SpeedTest(void)
{

    int r, g, b, j;
    clock_t atime;
    double seconds, diff;
    cmsHPROFILE hlcmsProfileIn, hlcmsProfileOut;
    cmsHTRANSFORM hlcmsxform;
    Scanline_rgb0 *In;
	size_t Mb;
   	
    hlcmsProfileIn  = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "r");
    hlcmsProfileOut = cmsOpenProfileFromFile("sRGBSpac.icm", "r");

    hlcmsxform  = cmsCreateTransform(hlcmsProfileIn, TYPE_RGB_16, hlcmsProfileOut, TYPE_RGB_16, INTENT_PERCEPTUAL, cmsFLAGS_NOTCACHE);

	Mb = 256*256*256*sizeof(Scanline_rgb0);

	In = (Scanline_rgb0*) malloc(Mb);

	j = 0;
	for (r=0; r < 256; r++)
        for (g=0; g < 256; g++)
            for (b=0; b < 256; b++) {

        In[j].r = (WORD) ((r << 8) | r);
        In[j].g = (WORD) ((g << 8) | g);
        In[j].b = (WORD) ((b << 8) | b);

		j++;
	}


    printf("lcms is transforming full spectrum...");

    atime = clock();

    cmsDoTransform(hlcmsxform, In, In, 256*256*256);

    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;
	free(In);
	
    
    printf("done.\n[%d tics, %g sec, %g Mpixel/sec.]\n", (int) diff, seconds, Mb / (1024*1024*seconds*3*2) );

    cmsDeleteTransform(hlcmsxform);
    cmsCloseProfile(hlcmsProfileIn);
    cmsCloseProfile(hlcmsProfileOut);

}



static
void SpeedTest2(void)
{

    int r, g, b, j;
    clock_t atime;
    double seconds, diff;
    cmsHPROFILE hlcmsProfileIn, hlcmsProfileOut;
    cmsHTRANSFORM hlcmsxform;
    Scanline_rgb8 *In;
	size_t Mb;
   
   
    hlcmsProfileIn  = cmsOpenProfileFromFile("sRGB Color Space Profile.icm", "r");
    hlcmsProfileOut = cmsOpenProfileFromFile("sRGBSpac.icm", "r");

    hlcmsxform  = cmsCreateTransform(hlcmsProfileIn, TYPE_RGB_8, hlcmsProfileOut, TYPE_RGB_8, INTENT_PERCEPTUAL, cmsFLAGS_NOTCACHE);

	Mb = 256*256*256*sizeof(Scanline_rgb8);

	In = (Scanline_rgb8*) malloc(Mb);

	j = 0;
	for (r=0; r < 256; r++)
        for (g=0; g < 256; g++)
            for (b=0; b < 256; b++) {

        In[j].r = (BYTE) r;
        In[j].g = (BYTE) g;
        In[j].b = (BYTE) b;

		j++;
	}


    printf("lcms is transforming full spectrum...");

    atime = clock();

    cmsDoTransform(hlcmsxform, In, In, 256*256*256);

    diff = clock() - atime;
    seconds = (double) diff / CLOCKS_PER_SEC;
	free(In);
	
    
    printf("done.\n[%d tics, %g sec, %g Mpixels/sec.]\n", (int) diff, seconds, Mb / (1024*1024*seconds*3) );

    cmsDeleteTransform(hlcmsxform);
    cmsCloseProfile(hlcmsProfileIn);
    cmsCloseProfile(hlcmsProfileOut);

}

#endif


static
int TestSaveToMem(void)
{
      void    *memPtr=0;
      size_t  bytesNeeded=0;
      int rc = FALSE;
      cmsHPROFILE hProfile = cmsCreate_sRGBProfile();

      printf("Testing save to memory: ");

     // pass 1 - compute length
      if (!_cmsSaveProfileToMem(hProfile, memPtr, &bytesNeeded)) {
                printf("Failed!\n");
                return FALSE;
      }
    // pass 2 - generate profile
      if(!bytesNeeded) {
            printf("Failed!\n");
            return FALSE;
      }

    memPtr = malloc(bytesNeeded);
    if (_cmsSaveProfileToMem(hProfile, memPtr, &bytesNeeded)) {
            

        cmsHPROFILE newProfile = cmsOpenProfileFromMem(memPtr, (DWORD) bytesNeeded);        
        const char* s = cmsTakeProductName(newProfile);

        if (strncmp(s, "sRGB", 4) == 0) rc = TRUE;

        cmsCloseProfile(newProfile);
        free(memPtr);
        
    }

    cmsCloseProfile(hProfile);

    printf (rc ? "pass.\n" : "failed!\n");
    return rc;
}



static
int TestNamedColor(void)
{
    LPcmsNAMEDCOLORLIST nc2;    
    cmsHPROFILE hProfile, hDevicelink, hsRGB, hLab;
    cmsHTRANSFORM xform, rgb2lab;    
    int i;


    printf("Testing Named color profiles: ");


    hsRGB    = cmsCreate_sRGBProfile();
    hLab     = cmsCreateLabProfile(NULL);
    
    rgb2lab = cmsCreateTransform(hsRGB, TYPE_RGB_16, hLab, TYPE_Lab_16, INTENT_PERCEPTUAL, cmsFLAGS_NOTPRECALC);

    nc2 = cmsAllocNamedColorList(64);

    nc2 ->ColorantCount = 3;
    strcpy(nc2 ->Prefix, "prefix");
    strcpy(nc2 ->Suffix, "suffix");

    for (i=0; i < 64; i++) {

        WORD vv = RGB_8_TO_16((i*4));
        
        nc2 ->List[i].DeviceColorant[0] = vv;
        nc2 ->List[i].DeviceColorant[1] = vv;
        nc2 ->List[i].DeviceColorant[2] = vv;

        cmsDoTransform(rgb2lab, nc2 ->List[i].DeviceColorant, nc2 ->List[i].PCS, 1);        
        
        sprintf(nc2 ->List[i].Name, "Color #%d", i);

    }

    hProfile = cmsOpenProfileFromFile("named.icc", "w");

    cmsSetDeviceClass(hProfile, icSigNamedColorClass);
    cmsSetPCS(hProfile, icSigLabData);
    cmsSetColorSpace(hProfile, icSigRgbData);

    cmsAddTag(hProfile, icSigNamedColor2Tag, (void*) nc2);
    cmsAddTag(hProfile, icSigMediaWhitePointTag, cmsD50_XYZ());
    cmsCloseProfile(hProfile);

    cmsFreeNamedColorList(nc2);


    hProfile = cmsOpenProfileFromFile("named.icc", "r");
    
    xform = cmsCreateTransform(hProfile, TYPE_NAMED_COLOR_INDEX, NULL, TYPE_RGB_16, INTENT_PERCEPTUAL, 0);

    for (i=0; i < 64; i++) {

        WORD index;
        WORD Color[3];

        index = (WORD) i;

        cmsDoTransform(xform, &index, Color, 1);

        if (Color[0] != RGB_8_TO_16((i*4)) ||
            Color[1] != RGB_8_TO_16((i*4)) ||
            Color[2] != RGB_8_TO_16((i*4))) { 

                    printf(" fail on spot color #%d\n", i); 
                    return 0; 
            }
    }


    cmsDeleteTransform(xform);
    cmsCloseProfile(hProfile);
    cmsDeleteTransform(rgb2lab);
    cmsCloseProfile(hLab);

        
    hProfile = cmsOpenProfileFromFile("named.icc", "r");
    
    xform = cmsCreateTransform(hProfile, TYPE_NAMED_COLOR_INDEX, hsRGB, TYPE_RGB_16, INTENT_PERCEPTUAL, 0);

    hDevicelink = cmsTransform2DeviceLink(xform, 0);

    _cmsSaveProfile(hDevicelink, "named2.icc");
    cmsCloseProfile(hDevicelink);
    
    cmsDeleteTransform(xform);
    cmsCloseProfile(hProfile);
    
    cmsCloseProfile(hsRGB);
        
    unlink("named.icc");
    unlink("named2.icc");

    printf(" pass.\n");
    return 1;
}


static
int TestColorantTableTag()
{
	LPcmsNAMEDCOLORLIST	     nc2;
	cmsHPROFILE hProfile = cmsOpenProfileFromFile("colTable.icc", "w");

	nc2 = cmsAllocNamedColorList(3);

	strcpy(nc2 ->List[0].Name, "Red");
	strcpy(nc2 ->List[1].Name, "Green");
	strcpy(nc2 ->List[2].Name, "Blue");

	
    cmsSetDeviceClass(hProfile, icSigOutputClass);
    cmsSetPCS(hProfile, icSigLabData);
    cmsSetColorSpace(hProfile, icSigRgbData);

    cmsAddTag(hProfile, icSigColorantTableTag, (void*) nc2);
    cmsAddTag(hProfile, icSigMediaWhitePointTag, cmsD50_XYZ());
    cmsCloseProfile(hProfile);
    cmsFreeNamedColorList(nc2);


	hProfile = cmsOpenProfileFromFile("colTable.icc", "r");

	nc2 = cmsReadColorantTable(hProfile, icSigColorantTableTag);

	cmsFreeNamedColorList(nc2);
	cmsCloseProfile(hProfile);

	unlink("colTable.icc");

	return 1;

}

// New to 1.13 -- CGATS/IT8.7


#define NPOINTS_IT8 10  // (17*17*17*17)

static
int TestIT8(void)
{
    LCMSHANDLE it8;
    int i;

    printf("Testing CGATS parser: ");

    it8 = cmsIT8Alloc();

    cmsIT8SetSheetType(it8, "LCMS/TESTING");
    cmsIT8SetPropertyStr(it8, "ORIGINATOR",   "1 2 3 4");
    cmsIT8SetPropertyUncooked(it8, "DESCRIPTOR",   "1234");
    cmsIT8SetPropertyStr(it8, "MANUFACTURER", "3");
    cmsIT8SetPropertyDbl(it8, "CREATED",      4);
    cmsIT8SetPropertyDbl(it8, "SERIAL",       5);
    cmsIT8SetPropertyHex(it8, "MATERIAL",     0x123);

    cmsIT8SetPropertyDbl(it8, "NUMBER_OF_SETS", NPOINTS_IT8);
    cmsIT8SetPropertyDbl(it8, "NUMBER_OF_FIELDS", 4);

    cmsIT8SetDataFormat(it8, 0, "SAMPLE_ID");
    cmsIT8SetDataFormat(it8, 1, "RGB_R");
    cmsIT8SetDataFormat(it8, 2, "RGB_G");
    cmsIT8SetDataFormat(it8, 3, "RGB_B");

    for (i=0; i < NPOINTS_IT8; i++) {

          char Patch[20];

          sprintf(Patch, "P%d", i);

          cmsIT8SetDataRowCol(it8, i, 0, Patch);
          cmsIT8SetDataRowColDbl(it8, i, 1, i);
          cmsIT8SetDataRowColDbl(it8, i, 2, i);
          cmsIT8SetDataRowColDbl(it8, i, 3, i);
    }

    cmsIT8SaveToFile(it8, "TEST.IT8");
    cmsIT8Free(it8);


	it8 = cmsIT8LoadFromFile("TEST.IT8");
	cmsIT8SaveToFile(it8, "TEST.IT8");
	cmsIT8Free(it8);



    it8 = cmsIT8LoadFromFile("TEST.IT8");

    if (cmsIT8GetPropertyDbl(it8, "DESCRIPTOR") != 1234) {
    
        printf("fail!\n");
        return 0;
    }


	cmsIT8SetPropertyDbl(it8, "DESCRIPTOR", 5678);

	if (cmsIT8GetPropertyDbl(it8, "DESCRIPTOR") != 5678) {
    
        printf("fail!\n");
        return 0;
    }


    if (cmsIT8GetDataDbl(it8, "P3", "RGB_G") != 3) {
        printf("fail!\n");
        return 0;
    }


    cmsIT8Free(it8);

    unlink("TEST.IT8");
    printf("pass.\n");
    return 1;

}


// Create CSA/CRD

static
void GenerateCSA(const char* cInProf)
{
	cmsHPROFILE hProfile;
	
	
	DWORD n;
	char* Buffer;


	if (cInProf == NULL) 
		hProfile = cmsCreateLabProfile(NULL);
	else 
		hProfile = cmsOpenProfileFromFile(cInProf, "r");

	n = cmsGetPostScriptCSA(hProfile, 0, NULL, 0);
	if (n == 0) return;

	Buffer = (char*) malloc(n + 1);
	cmsGetPostScriptCSA(hProfile, 0, Buffer, n);
	Buffer[n] = 0;
	
	free(Buffer);
	cmsCloseProfile(hProfile);
}


static
void GenerateCRD(const char* cOutProf)
{
	cmsHPROFILE hProfile;
	DWORD n;
	char* Buffer;
    DWORD dwFlags = 0;
    

	if (cOutProf == NULL) 
		hProfile = cmsCreateLabProfile(NULL);
	else 
		hProfile = cmsOpenProfileFromFile(cOutProf, "r");

	n = cmsGetPostScriptCRDEx(hProfile, 0, dwFlags, NULL, 0);
	if (n == 0) return;

	Buffer = (char*) malloc(n + 1);
    cmsGetPostScriptCRDEx(hProfile, 0, dwFlags, Buffer, n);
	Buffer[n] = 0;
	
	free(Buffer);
	cmsCloseProfile(hProfile);
}

static 
int TestPostScript()
{
	GenerateCSA("sRGB Color Space Profile.icm");
	GenerateCRD("sRGB Color Space Profile.icm");
	GenerateCSA(NULL);
	GenerateCRD(NULL);

	return 1;
}


static
void TestLabFloat()
{
#define TYPE_LabA_DBL   (COLORSPACE_SH(PT_Lab)|CHANNELS_SH(3)|BYTES_SH(0)|EXTRA_SH(1)|DOSWAP_SH(1))

	struct {
		   double L, a, b;
		   double A;
	} a;
	cmsCIELab b;
	cmsHPROFILE hLab  = cmsCreateLabProfile(NULL);
    cmsHTRANSFORM xform = cmsCreateTransform(hLab, TYPE_LabA_DBL, hLab, TYPE_Lab_DBL, 0, 0);

	a.L = 100; a.a = 0; a.b= 0;
	cmsDoTransform(xform, &a, &b, 1);

	cmsDeleteTransform(xform);
	cmsCloseProfile(hLab);
}



int main(int argc, char *argv[])
{
       int lExhaustive = 0;   
	
	   // #include "crtdbg.h"
	   // _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	  	   
       printf("little cms testbed. Ver %1.2f [build %s %s]\n\n", LCMS_VERSION / 100., __DATE__, __TIME__);
       
#ifndef LCMS_DLL

       if (!CheckEndianess()) return 1;	   
       if (!CheckSwab()) return 1;
	   if (!CheckQuickFloor()) return 1;

       TestFixedPoint();
     
       if (!TestFixedScaling()) return 1;          
       if (!TestJointCurves()) return 1;
       if (!TestReversingOfCurves()) return 1;
       if (!TestLinearInterpolation(lExhaustive)) return 1;
       if (!TestReverseLinearInterpolation()) return 1;

	   
       

       if (!Test3D()) return 1;
       if (!TestMatrixCreation()) return 1;
       if (!GetInfoTest()) return 1;


#endif

       if (!Test_sRGB()) return 1;
	
       if (!RealProfilesTest()) return 1;
       if (!TestInducedError(TYPE_LABA_16)) return 1;	   
       if (!TestPreview()) return 1;
       if (!TestMultiprofile()) return 1; 	   	 
       if (!TestLinearizationDevicelink()) return 1;
	  	 
       if (!TestDeviceLinkGeneration()) return 1; 	  
	   

       if (!TestLinearizationDevicelink2()) return 1;
	  

       if (!TestInkLimiting()) return 1;
       if (!TestSaveToMem()) return 1;
       if (!TestNamedColor()) return 1;  
       if (!TestIT8()) return 1;
	   if (!TestPostScript()) return 1;
	   if (!TestColorantTableTag()) return 1;
	   
#ifdef ICM_COMPARATIVE
#ifndef NON_WINDOWS
       CompareWithICM_8bit();
       CompareWithICM_16bit();
#endif
#endif

#ifdef CHECK_SPEED    
       SpeedTest();
	   SpeedTest2();
#endif

       printf("\nSuccess.\n");

       return 0;

}







