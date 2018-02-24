/*
  The following code was written by Richard White at STScI and made
  available for use in CFITSIO in July 1999. 
*/

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

#include "fitsio2.h"

/* nearest integer function */
# define NINT(x)  ((x >= 0.) ? (int) (x + 0.5) : (int) (x - 0.5))
# define SORT_CUTOFF 100	/* used by xMedian */
# define NELEM         5	/* used by xMedian */

#define NULL_VALUE -2147483647 /* value used to represent undefined pixels */
#define N_RESERVED_VALUES 1   /* number of reserved values, starting with */
                               /* and including NULL_VALUE.  These values */
                               /* may not be used to represent the quantized */
                               /* and scaled floating point pixel values */

/* factor to convert from median deviation to rms */
# define MEDIAN_TO_RMS  1.4826

/* more than this many standard deviations from the mean is an outlier */
# define SIGMA_CLIP     5.

# define NITER          3	/* number of sigma-clipping iterations */

static float xMedian (float [], int);
static void InsertionSort (float x[], int);
static int FqCompare (const void *, const void *);
static void FqMean (float [], int, double *, double *);


/*---------------------------------------------------------------------------*/
/* this routine used to be called 'quantize'  (WDP)  */

int fits_quantize_float (float fdata[], int nx, float in_null_value,
                  int noise_bits, int idata[], double *bscale,
                  double *bzero, int *iminval, int *imaxval) {

/* arguments:
float fdata[]       i: array of image pixels to be compressed
int nx              i: length of fdata array
float in_null_value i: value used to represent undefined pixels in fdata
int noise_bits      i: quantization level (number of bits)
int idata[]         o: values of fdata after applying bzero and bscale
double bscale       o: scale factor
double bzero        o: zero offset
int iminval         o: minimum quantized value that is returned
int imaxval         o: maximum quantized value that is returned

The function value will be one if the input fdata were copied to idata;
in this case the parameters bscale and bzero can be used to convert back to
nearly the original floating point values:  fdata ~= idata * bscale + bzero.
If the function value is zero, the data were not copied to idata.
*/

	float *diff;		/* difference array */
	int ndiff;		/* size of diff array */
	int intflag;		/* true if data are really integer */
	int i, j, iter;		/* loop indices */
        int anynulls = 0;       /* set if fdata contains any null values */
        int nshift, itemp;
        int first_nonnull = 0;
	double mean, stdev;	/* mean and RMS of differences */
	double minval = 0., maxval = 0.;  /* min & max of fdata */
	double delta;		/* bscale, 1 in idata = delta in fdata */
	double zeropt;	        /* bzero */
	double median;		/* median of diff array */
	double temp;

	if (nx <= 1) {
	    *bscale = 1.;
	    *bzero  = 0.;
	    return (0);
	}

        *iminval = INT32_MAX;
        *imaxval = INT32_MIN;

	/* Check to see if data are "floating point integer." */
        /* This also catches the case where all the pixels are null */

        /* Since idata and fdata may point to the same memory location, */
	/* we cannot write to idata unless we are sure we don't need   */
	/* the corresponding float value any more */
	
	intflag = 1;		/* initial value */
	for (i = 0;  i < nx;  i++) {
            if (fdata[i] == in_null_value) {
                anynulls = 1;
            }
	    else if (fdata[i] > INT32_MAX || 
                     fdata[i] < NULL_VALUE + N_RESERVED_VALUES) {
		intflag = 0;	/* not integer */
		break;
	    }
            else {
  	        itemp = (int)(fdata[i] + 0.5);

	        if (itemp != fdata[i]) {
		    intflag = 0;	/* not integer */
		    break;
                }
	    }
	}

        if (intflag) { /* data are "floating point integer" */
	  for (i = 0;  i < nx;  i++) {
            if (fdata[i] == in_null_value) {
                idata[i] = NULL_VALUE;
                anynulls = 1;
            }
            else {
  	        idata[i] = (int)(fdata[i] + 0.5);
                *iminval = minvalue(idata[i], *iminval);
                *imaxval = maxvalue(idata[i], *imaxval);
	    }
	  }
	}

	if (intflag) {  /* data are "floating point integer" */
            if (anynulls) {
                /* Shift the range of values so they lie close to NULL_VALUE. */
                /* This will make the compression more efficient.             */
                /* Maximum allowed shift is 2^31 - 1 = 2147483646 */
                /* Can't use 2147483647 because OSF says this is not a legal number */

                if (*iminval >= 0) {
		   nshift = -(NULL_VALUE + 1) - N_RESERVED_VALUES;
		} else {
                  nshift = *iminval - NULL_VALUE - N_RESERVED_VALUES;
                }

                for (i = 0;  i < nx;  i++) {
                    if (idata[i] != NULL_VALUE) {
                        idata[i] -= nshift;
                    }
                }
                *iminval = *iminval - nshift;
                *imaxval = *imaxval - nshift;
  	        *bscale = 1.;
	        *bzero = (double) nshift;
            }
            else {
                /* there were no null values, so no need to shift the range */
  	        *bscale = 1.;
	        *bzero = 0.;
            }
	    return (1);
	}

        /* data are not "floating point integer"; need to quantize them */

        /* find first non-null pixel, and initialize min and max values */
	for (i = 0;  i < nx;  i++) {
	    if (fdata[i] != in_null_value) {
               minval = fdata[i];
               maxval = fdata[i];
               first_nonnull = i;
               break;
            }
        }

        /* allocate temporary buffer for differences */
	ndiff = nx - first_nonnull - 1;
	if ((diff = (float *) malloc (ndiff * sizeof (float))) == NULL) {
            ffpmsg("Out of memory in 'fits_quantize_float'.");  
	    return (0);
	}

        /* calc ABS difference between successive non-null pixels */
        j = first_nonnull;
        ndiff = 0;
	for (i = j + 1 ;  i < nx;  i++) {
            if (fdata[i] != in_null_value) {
 	        diff[ndiff] = (float) (fabs (fdata[i] - fdata[j]));
                j = i;
                ndiff++;
                minval = minvalue(minval, fdata[i]);
                maxval = maxvalue(maxval, fdata[i]);
            }
        }

        /* check if there were any null values */
        if (ndiff + 1 == nx)
            anynulls = 0;
        else
            anynulls = 1;

	/* use median of absolute deviations */

	median = xMedian (diff, ndiff);
	stdev = median * MEDIAN_TO_RMS;
	/* substitute sigma-clipping if median is zero */
	if (stdev == 0.0) {

            /* calculate differences between non-null pixels */
            j = first_nonnull;
            ndiff = 0;
	    for (i = j + 1 ;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
 	            diff[ndiff] = fdata[i] - fdata[j];
                    j = i;
                    ndiff++;
                }
            }

	    FqMean (diff, ndiff, &mean, &stdev);

	    for (iter = 0;  iter < NITER;  iter++) {
		j = 0;
		for (i = 0;  i < ndiff;  i++) {
		    if (fabs (diff[i] - mean) < SIGMA_CLIP * stdev) {
			if (j < i)
			    diff[j] = diff[i];
			j++;
		    }
		}
		if (j == ndiff)
		    break;
		ndiff = j;
		FqMean (diff, ndiff, &mean, &stdev);
	    }
	}
	free (diff);

	delta = stdev / pow (2., (double)noise_bits);
	if (delta == 0. && ndiff > 0)
	    return (0);	/* Zero variance in differences!  Don't quantize. */

        /* check that the range of quantized levels is not > range of int */
	if ((maxval - minval) / delta > 2. * 2147483647. - N_RESERVED_VALUES )
	    return (0);			/* don't quantize */

        if (!anynulls) {   /* don't have to check for nulls */
            /* return all positive values, if possible since some */
            /* compression algorithms either only work for positive integers, */
            /* or are more efficient.  */
            if ((maxval - minval) / delta < 2147483647. - N_RESERVED_VALUES )
            {
                zeropt = minval;
            }
            else
            {
                /* center the quantized levels around zero */
                zeropt = (minval + maxval) / 2.;
            }

       	    for (i = 0;  i < nx;  i++) {
	        temp = (fdata[i] - zeropt) / delta;
	        idata[i] = NINT (temp);
            }
        }
        else {
            /* data contains null values; shift the range to be */
            /* close to the value used to represent null values */
            zeropt = minval - delta * (NULL_VALUE + N_RESERVED_VALUES);

	    for (i = 0;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
	            temp = (fdata[i] - zeropt) / delta;
	            idata[i] = NINT (temp);
                }
                else
                    idata[i] = NULL_VALUE;
            }
	}

        /* calc min and max values */
        temp = (minval - zeropt) / delta;
        *iminval =  NINT (temp);
        temp = (maxval - zeropt) / delta;
        *imaxval =  NINT (temp);

	*bscale = delta;
	*bzero = zeropt;

	return (1);			/* yes, data have been quantized */
}

/*---------------------------------------------------------------------------*/
int fits_quantize_double (double fdata[], int nx, double in_null_value,
                  int noise_bits, int idata[], double *bscale,
                  double *bzero, int *iminval, int *imaxval) {

/* arguments:
double fdata[]       i: array of image pixels to be compressed
int nx               i: length of fdata array
double in_null_value i: value used to represent undefined pixels in fdata
int noise_bits       i: quantization level (number of bits)
int idata[]          o: values of fdata after applying bzero and bscale
double bscale        o: scale factor
double bzero         o: zero offset
int imaxval        o: maximum quantized value that is returned
int iminval        o: minimum quantized value that is returned

The function value will be one if the input fdata were copied to idata;
in this case the parameters bscale and bzero can be used to convert back to
nearly the original floating point values:  fdata ~= idata * bscale + bzero.
If the function value is zero, the data were not copied to idata.
*/

	float *diff;		/* difference array */
	int ndiff;		/* size of diff array */
	int intflag;		/* true if data are really integer */
	int i, j, iter;		/* loop indices */
        int anynulls = 0;       /* set if fdata contains any null values */
        int nshift, itemp;
        int first_nonnull = 0;
	double mean, stdev;	/* mean and RMS of differences */
	double minval = 0., maxval = 0.;   /* min & max of fdata */
	double delta;		/* bscale, 1 in idata = delta in fdata */
	double zeropt;	        /* bzero */
	double median;		/* median of diff array */
	double temp;

	if (nx <= 1) {
	    *bscale = 1.;
	    *bzero  = 0.;
	    return (0);
	}

        *iminval = INT32_MAX;
        *imaxval = INT32_MIN;

	/* Check to see if data are "floating point integer." */
        /* This also catches the case where all the pixels are null */

        /* Since idata and fdata may point to the same memory location, */
	/* we cannot write to idata unless we are sure we don't need   */
	/* the corresponding float value any more */
	
	intflag = 1;		/* initial value */
	for (i = 0;  i < nx;  i++) {
            if (fdata[i] == in_null_value) {
                anynulls = 1;
            }
	    else if (fdata[i] > INT32_MAX || 
                     fdata[i] < NULL_VALUE + N_RESERVED_VALUES) {
		intflag = 0;	/* not integer */
		break;
	    }
            else {
  	        itemp = (int)(fdata[i] + 0.5);

	        if (itemp != fdata[i]) {
		    intflag = 0;	/* not integer */
		    break;
                }
	    }
	}

        if (intflag) { /* data are "floating point integer" */
	  for (i = 0;  i < nx;  i++) {
            if (fdata[i] == in_null_value) {
                idata[i] = NULL_VALUE;
                anynulls = 1;
            }
            else {
  	        idata[i] = (int)(fdata[i] + 0.5);
                *iminval = minvalue(idata[i], *iminval);
                *imaxval = maxvalue(idata[i], *imaxval);
	    }
	  }
	}

	if (intflag) {  /* data are "floating point integer" */
            if (anynulls) {
                /* Shift the range of values so they lie close to NULL_VALUE. */
                /* This will make the compression more efficient.             */
                /* Maximum allowed shift is 2^31 - 1 = 2147483646 */
                /* Can't use 2147483647 because OSF says this is not a legal number */

                if (*iminval >= 0) {
		   nshift = -(NULL_VALUE +1) - N_RESERVED_VALUES;
		} else {
                  nshift = *iminval - NULL_VALUE - N_RESERVED_VALUES;
                }

                for (i = 0;  i < nx;  i++) {
                    if (idata[i] != NULL_VALUE) {
                        idata[i] -= nshift;
                    }
                }
                *iminval = *iminval - nshift;
                *imaxval = *imaxval - nshift;
  	        *bscale = 1.;
	        *bzero = (double) nshift;
            }
            else {
                /* there were no null values, so no need to shift the range */
  	        *bscale = 1.;
	        *bzero = 0.;
            }
	    return (1);
	}

        /* data are not "floating point integer"; need to quantize them */

        /* find first non-null pixel, and initialize min and max values */
	for (i = 0;  i < nx;  i++) {
	    if (fdata[i] != in_null_value) {
                minval = fdata[i];
                maxval = fdata[i];
                first_nonnull = i;
                break;
            }
        }

        /* allocate temporary buffer for differences */
	ndiff = nx - first_nonnull - 1;
	if ((diff = (float *) malloc (ndiff * sizeof (float))) == NULL) {
            ffpmsg("Out of memory in 'fits_quantize_double'.");  
	    return (0);
	}

        /* calc ABS difference between successive non-null pixels */
        j = first_nonnull;
        ndiff = 0;
	for (i = j + 1 ;  i < nx;  i++) {
            if (fdata[i] != in_null_value) {
 	        diff[ndiff] = (float) (fabs (fdata[i] - fdata[j]));
                j = i;
                ndiff++;
                minval = minvalue(minval, fdata[i]);
                maxval = maxvalue(maxval, fdata[i]);
            }
        }

        /* check if there were any null values */
        if (ndiff + 1 == nx)
            anynulls = 0;
        else
            anynulls = 1;

	/* use median of absolute deviations */

	median = xMedian (diff, ndiff);
	stdev = median * MEDIAN_TO_RMS;
	/* substitute sigma-clipping if median is zero */
	if (stdev == 0.0) {

            /* calculate differences between non-null pixels */
            j = first_nonnull;
            ndiff = 0;
	    for (i = j + 1 ;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
 	            diff[ndiff] = (float) (fdata[i] - fdata[j]);
                    j = i;
                    ndiff++;
                }
            }

	    FqMean (diff, ndiff, &mean, &stdev);

	    for (iter = 0;  iter < NITER;  iter++) {
		j = 0;
		for (i = 0;  i < ndiff;  i++) {
		    if (fabs (diff[i] - mean) < SIGMA_CLIP * stdev) {
			if (j < i)
			    diff[j] = diff[i];
			j++;
		    }
		}
		if (j == ndiff)
		    break;
		ndiff = j;
		FqMean (diff, ndiff, &mean, &stdev);
	    }
	}
	free (diff);

	delta = stdev / pow (2., (double)noise_bits);
	if (delta == 0. && ndiff > 0)
	    return (0);	/* Zero variance in differences!  Don't quantize. */

        /* check that the range of quantized levels is not > range of int */
	if ((maxval - minval) / delta > 2. * 2147483647 - N_RESERVED_VALUES )
	    return (0);			/* don't quantize */
        if (!anynulls) {   /* don't have to check for nulls */
            /* center the quantized levels around zero */
            zeropt = (minval + maxval) / 2.;
       	    for (i = 0;  i < nx;  i++) {
	       temp = (fdata[i] - zeropt) / delta;
	       idata[i] = NINT (temp);
            }
        }
        else {
            /* data contains null values; shift the range to be */
            /* close to the value used to represent null values */
            zeropt = minval - delta * (NULL_VALUE + N_RESERVED_VALUES);
	    for (i = 0;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
	            temp = (fdata[i] - zeropt) / delta;
	            idata[i] = NINT (temp);
                }
                else
                    idata[i] = NULL_VALUE;
            }
	}

        /* calc min and max values */
        temp = (minval - zeropt) / delta;
        *iminval =  NINT (temp);
        temp = (maxval - zeropt) / delta;
        *imaxval =  NINT (temp);

	*bscale = delta;
	*bzero = zeropt;

	return (1);			/* yes, data have been quantized */
}
/*---------------------------------------------------------------------------*/
int fits_rms_float (float fdata[], int nx, float in_null_value,
                   double *rms, int *status) {

/*
Compute the background RMS (noise) in an array of image pixels.

arguments:
float fdata[]       i: array of image pixels
int nx              i: length of fdata array
float in_null_value i: value used to represent undefined pixels in fdata
double rms          o: computed RMS value
*/
	float *diff;		/* difference array */
	int ndiff;		/* size of diff array */
	int i, j, iter;		/* loop indices */
        int first_nonnull = 0;
	double mean, stdev;	/* mean and RMS of differences */
	double median;		/* median of diff array */

	if (*status) return (*status);
	
	if (nx <= 1) {
	    *rms = 0;
	    return (0);
	}

        /* find first non-null pixel, and initialize min and max values */
	for (i = 0;  i < nx;  i++) {
	    if (fdata[i] != in_null_value) {
               first_nonnull = i;
               break;
            }
        }

        /* allocate temporary buffer for differences */
	ndiff = nx - first_nonnull - 1;
	if ((diff = (float *) malloc (ndiff * sizeof (float))) == NULL) {
            ffpmsg("Out of memory in 'fits_float_rms'."); 
	    *status = 113;  /* memory allocation error */ 
	    return (0);
	}

        /* calc ABS difference between successive non-null pixels */
        j = first_nonnull;
        ndiff = 0;
	for (i = j + 1 ;  i < nx;  i++) {
            if (fdata[i] != in_null_value) {
 	        diff[ndiff] = (float) (fabs (fdata[i] - fdata[j]));
                j = i;
                ndiff++;
            }
        }

	/* use median of absolute deviations */

	median = xMedian (diff, ndiff);
	stdev = median * MEDIAN_TO_RMS;
	
	/* substitute sigma-clipping if median is zero */
	if (stdev == 0.0) {

            /* calculate differences between non-null pixels */
            j = first_nonnull;
            ndiff = 0;
	    for (i = j + 1 ;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
 	            diff[ndiff] = fdata[i] - fdata[j];
                    j = i;
                    ndiff++;
                }
            }

	    FqMean (diff, ndiff, &mean, &stdev);

	    for (iter = 0;  iter < NITER;  iter++) {
		j = 0;
		for (i = 0;  i < ndiff;  i++) {
		    if (fabs (diff[i] - mean) < SIGMA_CLIP * stdev) {
			if (j < i)
			    diff[j] = diff[i];
			j++;
		    }
		}
		if (j == ndiff)
		    break;
		ndiff = j;
		FqMean (diff, ndiff, &mean, &stdev);
	    }
	}
	free (diff);
	
        *rms = stdev;

	return (0);
}
/*---------------------------------------------------------------------------*/
int fits_rms_short (short fdata[], int nx, short in_null_value,
                   double *rms, int *status) {

/*
Compute the background RMS (noise) in an array of image pixels.

arguments:
short fdata[]       i: array of image pixels
int nx              i: length of fdata array
short in_null_value i: value used to represent undefined pixels in fdata
double rms          o: computed RMS value
*/
	float *diff;		/* difference array */
	int ndiff;		/* size of diff array */
	int i, j, iter;		/* loop indices */
        int first_nonnull = 0;
	double mean, stdev;	/* mean and RMS of differences */
	double median;		/* median of diff array */

	if (*status) return (*status);
	
	if (nx <= 1) {
	    *rms = 0;
	    return (0);
	}

        /* find first non-null pixel, and initialize min and max values */
	for (i = 0;  i < nx;  i++) {
	    if (fdata[i] != in_null_value) {
               first_nonnull = i;
               break;
            }
        }

        /* allocate temporary buffer for differences */
	ndiff = nx - first_nonnull - 1;
	if ((diff = (float *) malloc (ndiff * sizeof (float))) == NULL) {
            ffpmsg("Out of memory in 'fits_float_rms'."); 
	    *status = 113;  /* memory allocation error */ 
	    return (0);
	}

        /* calc ABS difference between successive non-null pixels */
        j = first_nonnull;
        ndiff = 0;
	for (i = j + 1 ;  i < nx;  i++) {
            if (fdata[i] != in_null_value) {
 	        diff[ndiff] = (float) (abs (fdata[i] - fdata[j]));
                j = i;
                ndiff++;
            }
        }

	/* use median of absolute deviations */

	median = xMedian (diff, ndiff);
	stdev = median * MEDIAN_TO_RMS;
	
	/* substitute sigma-clipping if median is zero */
	if (stdev == 0.0) {

            /* calculate differences between non-null pixels */
            j = first_nonnull;
            ndiff = 0;
	    for (i = j + 1 ;  i < nx;  i++) {
                if (fdata[i] != in_null_value) {
 	            diff[ndiff] = (float) (fdata[i] - fdata[j]);
                    j = i;
                    ndiff++;
                }
            }

	    FqMean (diff, ndiff, &mean, &stdev);

	    for (iter = 0;  iter < NITER;  iter++) {
		j = 0;
		for (i = 0;  i < ndiff;  i++) {
		    if (fabs (diff[i] - mean) < SIGMA_CLIP * stdev) {
			if (j < i)
			    diff[j] = diff[i];
			j++;
		    }
		}
		if (j == ndiff)
		    break;
		ndiff = j;
		FqMean (diff, ndiff, &mean, &stdev);
	    }
	}
	free (diff);
	
        *rms = stdev;

	return (0);
}

/*---------------------------------------------------------------------------*/
/* This computes the mean and standard deviation. */

static void FqMean (float diff[], int ndiff, double *mean, double *stdev) {

	int i;
	double sum, sumsq;
	double m;		/* mean */
	double xn;		/* = ndiff */
	double temp;

	if (ndiff < 2) {
	    if (ndiff < 1)
		*mean = 0.;
	    else
		*mean = diff[0];
	    *stdev = 0.;
	    return;
	}

	xn = (double)ndiff;

	sum = 0.;
	sumsq = 0.;
	for (i = 0;  i < ndiff;  i++) {
	    sum += diff[i];
	    sumsq += (diff[i] * diff[i]);
	}

	m = sum / xn;
	*mean = m;
	temp = (sumsq / xn - m*m) * xn;
	if (temp <= 0)
	    *stdev = 0.;
	else
	    *stdev = sqrt (temp / (xn-1.));
}

/*---------------------------------------------------------------------------*/
/* This returns an approximation to the median.
   The input array will be clobbered.
*/

static float xMedian (float x[], int n) {

/* arguments:
float x[]     io: the array (will be scrambled and possibly modified)
int n         i: number of elements in x (modified locally)
*/

	int i, j;
	int next_n;
	int npix;
	int done;
	float median = 0.;

	if (n < 1) {
            ffpmsg("xMedian: no data");
	    return (0.);
	}
	if (n == 1)
	    return (x[0]);
	if (n == 2)
	    return ((float) ((x[0] + x[1]) / 2.));

	done = 0;
	while (!done) {

	    if (n < SORT_CUTOFF) {
		qsort (x, n, sizeof (float), FqCompare);
		if (n / 2 * 2 == n)
		    median = (float) ((x[n/2-1] + x[n/2]) / 2.);
		else
		    median = x[n/2];
		return (median);
	    }

	    /* ignore trailing groups of less than three elements */
	    next_n = (n + NELEM-3) / NELEM;

	    for (j = 0;  j < next_n;  j++) {

		i = j * NELEM;
		npix = minvalue (NELEM, n - j*NELEM);

		InsertionSort (&x[i], npix);

		switch (npix) {
		case 1:
		    median = x[i];
		    break;
		case 2:
		    median = (float) ((x[i] + x[i+1]) / 2.);
		    break;
		case 3:
		    median = x[i+1];
		    break;
		case 4:
		    median = (float) ((x[i+1] + x[i+2]) / 2.);
		    break;
		case 5:				/* NELEM = 5 */
		    median = x[i+2];
		    break;
		default:
                    ffpmsg("npix should be 1..5");
		}

		x[j] = median;
	    }

	    if (next_n <= 1)
		done = 1;
	    else
		n = next_n;
	}

	return (x[0]);
}
/*---------------------------------------------------------------------------*/
static void InsertionSort (float x[], int n) {

	float a;
	int i, j;

	for (j = 1;  j < n;  j++) {

	    a = x[j];
	    i = j - 1;
	    while (i >= 0 && x[i] > a) {
		x[i+1] = x[i];
		i--;
	    }
	    x[i+1] = a;
	}
}
/*---------------------------------------------------------------------------*/
static int FqCompare (const void *vp, const void *vq) {

	const float *p = vp;
	const float *q = vq;

	if (*p > *q)
	    return (1);
	else if (*p < *q)
	    return (-1);
	else
	    return (0);
}
