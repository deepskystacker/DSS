# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>
# include "fitsio2.h"

/*--------------------------------------------------------------------------*/
int fits_set_compression_type(fitsfile *fptr,  /* I - FITS file pointer     */
       int ctype,    /* image compression type code;                        */
                     /* allowed values: RICE_1, GZIP_1, PLIO_1, HCOMPRESS_1 */
       int *status)  /* IO - error status                                   */
{
/*
   This routine specifies the image compression algorithm that should be
   used when writing a FITS image.  The image is divided into tiles, and
   each tile is compressed and stored in a row of at variable length binary
   table column.
*/
    (fptr->Fptr)->request_compress_type = ctype;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_set_tile_dim(fitsfile *fptr,  /* I - FITS file pointer             */
           int ndim,   /* number of dimensions in the compressed image      */
           long *dims, /* size of image compression tile in each dimension  */
                      /* default tile size = (NAXIS1, 1, 1, ...)            */
           int *status)         /* IO - error status                        */
{
/*
   This routine specifies the size (dimension) of the image
   compression  tiles that should be used when writing a FITS
   image.  The image is divided into tiles, and each tile is compressed
   and stored in a row of at variable length binary table column.
*/
    int ii;

    if (ndim < 0 || ndim > MAX_COMPRESS_DIM)
    {
        *status = BAD_DIMEN;
        return(*status);
    }

    for (ii = 0; ii < ndim; ii++)
    {
        (fptr->Fptr)->request_tilesize[ii] = dims[ii];
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_set_noise_bits(fitsfile *fptr,  /* I - FITS file pointer   */
           int noisebits,       /* noise_bits parameter value       */
                                /* (default = 4)                    */
           int *status)         /* IO - error status                */
{
/*
   This routine specifies the value of the noice_bits parameter that
   should be used when compressing floating point images.  The image is
   divided into tiles, and each tile is compressed and stored in a row
   of at variable length binary table column.
*/
    if (noisebits < 1 || noisebits > 16)
    {
        *status = DATA_COMPRESSION_ERR;
        return(*status);
    }

    (fptr->Fptr)->request_noise_nbits = noisebits;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_set_hcomp_scale(fitsfile *fptr,  /* I - FITS file pointer   */
           int scale,       /* hcompress scale parameter value       */
                                /* (default = 4)                    */
           int *status)         /* IO - error status                */
{
/*
   This routine specifies the value of the hcompress scale parameter that
  The image is
   divided into tiles, and each tile is compressed and stored in a row
   of at variable length binary table column.
*/

    (fptr->Fptr)->request_hcomp_scale = scale;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_set_hcomp_smooth(fitsfile *fptr,  /* I - FITS file pointer   */
           int smooth,       /* hcompress smooth parameter value       */
                                /* if scale > 1 and smooth != 0, then */
				/*  the image will be smoothed when it is */
				/* decompressed to remove some of the */
				/* 'blockiness' in the image produced */
				/* by the lossy compression    */
           int *status)         /* IO - error status                */
{
/*
   This routine specifies the value of the hcompress scale parameter that
  The image is
   divided into tiles, and each tile is compressed and stored in a row
   of at variable length binary table column.
*/

    (fptr->Fptr)->request_hcomp_smooth = smooth;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_compression_type(fitsfile *fptr,  /* I - FITS file pointer     */
       int *ctype,   /* image compression type code;                        */
                     /* allowed values: RICE_1, GZIP_1, PLIO_1, HCOMPRESS_1 */
       int *status)  /* IO - error status                                   */
{
/*
   This routine returns the image compression algorithm that should be
   used when writing a FITS image.  The image is divided into tiles, and
   each tile is compressed and stored in a row of at variable length binary
   table column.
*/
    *ctype = (fptr->Fptr)->request_compress_type;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_tile_dim(fitsfile *fptr,  /* I - FITS file pointer             */
           int ndim,   /* number of dimensions in the compressed image      */
           long *dims, /* size of image compression tile in each dimension  */
                       /* default tile size = (NAXIS1, 1, 1, ...)           */
           int *status)         /* IO - error status                        */
{
/*
   This routine returns the size (dimension) of the image
   compression  tiles that should be used when writing a FITS
   image.  The image is divided into tiles, and each tile is compressed
   and stored in a row of at variable length binary table column.
*/
    int ii;

    if (ndim < 0 || ndim > MAX_COMPRESS_DIM)
    {
        *status = BAD_DIMEN;
        return(*status);
    }

    for (ii = 0; ii < ndim; ii++)
    {
        dims[ii] = (fptr->Fptr)->request_tilesize[ii];
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_noise_bits(fitsfile *fptr,  /* I - FITS file pointer   */
           int *noisebits,       /* noise_bits parameter value       */
                                /* (default = 4)                    */
           int *status)         /* IO - error status                */

{
/*
   This routine returns the value of the noice_bits parameter that
   should be used when compressing floating point images.  The image is
   divided into tiles, and each tile is compressed and stored in a row
   of at variable length binary table column.
*/

    *noisebits = (fptr->Fptr)->request_noise_nbits;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_hcomp_scale(fitsfile *fptr,  /* I - FITS file pointer   */
           int *scale,          /* Hcompress scale parameter value       */
           int *status)         /* IO - error status                */

{
/*
   This routine returns the value of the noice_bits parameter that
   should be used when compressing floating point images.  The image is
   divided into tiles, and each tile is compressed and stored in a row
   of at variable length binary table column.
*/

    *scale = (fptr->Fptr)->request_hcomp_scale;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_get_hcomp_smooth(fitsfile *fptr,  /* I - FITS file pointer   */
           int *smooth,          /* Hcompress smooth parameter value       */
           int *status)         /* IO - error status                */

{
    *smooth = (fptr->Fptr)->request_hcomp_smooth;
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_img_compress(fitsfile *infptr, /* pointer to image to be compressed */
                 fitsfile *outfptr, /* empty HDU for output compressed image */
                 int *status)       /* IO - error status               */

/*
   This routine initializes the output table, copies all the keywords,
   and  loops through the input image, compressing the data and
   writing the compressed tiles to the output table.
   
   This is a high level routine that is called by the fpack and funpack
   FITS compression utilities.
*/
{
    int bitpix, naxis;
    long naxes[MAX_COMPRESS_DIM];

    if (*status > 0)
        return(*status);

    /* get datatype and size of input image */
    if (fits_get_img_param(infptr, MAX_COMPRESS_DIM, &bitpix, 
                       &naxis, naxes, status) > 0)
        return(*status);

    if (naxis < 1 || naxis > MAX_COMPRESS_DIM)
    {
        ffpmsg("Image cannot be compressed: NAXIS out of range");
        return(*status = BAD_NAXIS);
    }

    /* initialize output table */
    if (imcomp_init_table(outfptr, bitpix, naxis, naxes, 0, status) > 0)
        return (*status);

    /* Copy the image header keywords to the table header. */
    if (imcomp_copy_img2comp(infptr, outfptr, status) > 0)
	    return (*status);

    /* turn off any intensity scaling (defined by BSCALE and BZERO */
    /* keywords) so that unscaled values will be read by CFITSIO */
    ffpscl(infptr, 1.0, 0.0, status);

    /* force a rescan of the output file keywords, so that */
    /* the compression parameters will be copied to the internal */
    /* fitsfile structure used by CFITSIO */
    ffrdef(outfptr, status);

    /* turn off any intensity scaling (defined by BSCALE and BZERO */
    /* keywords) so that unscaled values will be written by CFITSIO */
    ffpscl(outfptr, 1.0, 0.0, status);

    /* Read each image tile, compress, and write to a table row. */
    imcomp_compress_image (infptr, outfptr, status);

    /* force another rescan of the output file keywords, to */
    /* update PCOUNT and TFORMn = '1PB(iii)' keyword values. */
    ffrdef(outfptr, status);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int fits_compress_img(fitsfile *infptr, /* pointer to image to be compressed */
                 fitsfile *outfptr, /* empty HDU for output compressed image */
                 int compress_type, /* compression type code               */
                                    /*  RICE_1, HCOMPRESS_1, etc.          */
                 long *intilesize,    /* size in each dimension of the tiles */
                                    /* NULL pointer means tile by rows */
		 int blocksize,     /* compression parameter: blocksize  */
                 int nbits,         /* compression parameter: nbits  */
                 int *status)       /* IO - error status               */

/*
  !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!
   This routine is obsolete and should not be used.  Currently the 
   ftools 'fimgzip' task calls this; it should be modified to call fits_img_compress
   instead.
  !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!
  
   This routine initializes the output table, copies all the keywords,
   and  loops through the input image, compressing the data and
   writing the compressed tiles to the output table.
*/
{
    int bitpix, naxis;
    long naxes[MAX_COMPRESS_DIM];

    if (*status > 0)
        return(*status);

    /* get datatype and size of input image */
    if (fits_get_img_param(infptr, MAX_COMPRESS_DIM, &bitpix, 
                       &naxis, naxes, status) > 0)
        return(*status);

    if (naxis < 1 || naxis > MAX_COMPRESS_DIM)
    {
        ffpmsg("Image cannot be compressed: NAXIS out of range");
        return(*status = BAD_NAXIS);
    }

    /* initialize output table */
    if (imcomp_init_table(outfptr, bitpix, naxis, naxes, 0, status) > 0)
        return (*status);

    /* Copy the image header keywords to the table header. */
    if (imcomp_copy_imheader(infptr, outfptr, status) > 0)
	    return (*status);

    /* turn off any intensity scaling (defined by BSCALE and BZERO */
    /* keywords) so that unscaled values will be read by CFITSIO */
    ffpscl(infptr, 1.0, 0.0, status);

    /* force a rescan of the output file keywords, so that */
    /* the compression parameters will be copied to the internal */
    /* fitsfile structure used by CFITSIO */
    ffrdef(outfptr, status);

    /* Read each image tile, compress, and write to a table row. */
    imcomp_compress_image (infptr, outfptr, status);

    /* force another rescan of the output file keywords, to */
    /* update PCOUNT and TFORMn = '1PB(iii)' keyword values. */
    ffrdef(outfptr, status);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_init_table(fitsfile *outfptr,
        int inbitpix,
        int naxis,
        long *naxes,
	int writebitpix,    /* write the ZBITPIX, ZNAXIS, and ZNAXES keyword? */
        int *status)
/* 
  create a BINTABLE extension for the output compressed image.
*/
{
    char keyname[FLEN_KEYWORD], zcmptype[12];
    int ii, jj, minspace, tempsize, remain, leftspace, ncols, bitpix;
    long nrows;
    char *ttype[] = {"COMPRESSED_DATA", "UNCOMPRESSED_DATA", "ZSCALE", "ZZERO"};
    char *tform[4];
    char tf0[4], tf1[4], tf2[4], tf3[4];
    char *tunit[] = {"\0",            "\0",            "\0",      "\0"    };
    char comm[FLEN_COMMENT];

    if (*status > 0)
        return(*status);

    /* test for the 2 special cases that represent unsigned integers */
    if (inbitpix == USHORT_IMG)
        bitpix = SHORT_IMG;
    else if (inbitpix == ULONG_IMG)
        bitpix = LONG_IMG;
    else if (inbitpix == SBYTE_IMG)
        bitpix = BYTE_IMG;
    else 
        bitpix = inbitpix;

    /* reset default tile dimensions too if required */

    if ((outfptr->Fptr)->request_compress_type == HCOMPRESS_1) {
         if (((outfptr->Fptr)->request_tilesize[0] == 0) &&
             ((outfptr->Fptr)->request_tilesize[1] == 1) ){
	     

 	     for (ii = 0; ii < 2 && ii < naxis; ii++) {  /* only reset the 1st 2 dimensions */
	         if (naxes[ii] <= 600) {

	             /* use the full dimension of the image as the tile dimension */
	             (outfptr->Fptr)->request_tilesize[ii] = naxes[ii];

	         } else {

	             /* look for an even tile size in the range 200 - 600 */
		     /* This is a brute force algorithm; probably more efficient ways to do this */

                     minspace = naxes[ii];
		     tempsize = naxes[ii];
		     
                     for (jj = 600; jj >= 200; jj -= 2) {
		   
		        remain = naxes[ii] % jj;
		       
		        if (!remain) {
		          /* found an even multiple tile size */
                          tempsize = jj;
			  break;
			  
		        } else {

                           leftspace = jj - remain;
			   if (leftspace < minspace) {
			       /* save the best case found so far */
			       minspace = leftspace;
			       tempsize = jj;
			   }
		        }
		    } /* end of for jj loop */

 	            (outfptr->Fptr)->request_tilesize[ii] = tempsize;
		    
	        }
	    }	  /* end of for ii loop */
	}
    } /* end, if HCOMPRESS_1 */
    

    for (ii = 0; ii < naxis; ii++) {
        if ((outfptr->Fptr)->request_tilesize[ii] <= 0) {
	    /* tile size of 0 means use the image size of that dimension */
	    (outfptr->Fptr)->request_tilesize[ii] = naxes[ii];
	}
    }

    /*  (only used to quantize floating point images)  */
    if ((outfptr->Fptr)->request_noise_nbits < 1)  /* use default value if input is not legal */
        (outfptr->Fptr)->request_noise_nbits = 4;

    /* ---- set up array of TFORM strings -------------------------------*/
    strcpy(tf0, "1PB");
    strcpy(tf2, "1D");
    strcpy(tf3, "1D");

    tform[0] = tf0;
    tform[1] = tf1;
    tform[2] = tf2;
    tform[3] = tf3;

    /* calculate number of rows in output table */
    nrows = 1;
    for (ii = 0; ii < naxis; ii++)
    {
        nrows = nrows * ((naxes[ii] - 1)/ ((outfptr->Fptr)->request_tilesize[ii]) + 1);
    }

    if (bitpix < 0 )  /* floating point image */
        ncols = 4;
    else
        ncols = 1; /* default table has just one 'COMPRESSED_DATA' column */

    if ((outfptr->Fptr)->request_compress_type == RICE_1)
    {
        strcpy(zcmptype, "RICE_1");
    }
    else if ((outfptr->Fptr)->request_compress_type == GZIP_1)
    {
        strcpy(zcmptype, "GZIP_1");
    }
    else if ((outfptr->Fptr)->request_compress_type == PLIO_1)
    {
        strcpy(zcmptype, "PLIO_1");
       /* the PLIO compression algorithm outputs short integers, not bytes */
       strcpy(tform[0], "1PI");
    }
    else if ((outfptr->Fptr)->request_compress_type == HCOMPRESS_1)
    {
        strcpy(zcmptype, "HCOMPRESS_1");
    }
    else
    {
        ffpmsg("unknown compression type (imcomp_init_table)");
        return(*status = DATA_COMPRESSION_ERR);
    }

    /* set correct datatype for any tiles that cannot be compressed */
    if (bitpix == SHORT_IMG)
       strcpy(tform[1], "1PI");
    else if (bitpix == LONG_IMG)
       strcpy(tform[1], "1PJ");
    else if (bitpix == FLOAT_IMG)
       strcpy(tform[1], "1PE");
    else if (bitpix == DOUBLE_IMG)
       strcpy(tform[1], "1PD");

    /* create the bintable extension to contain the compressed image */
    ffcrtb(outfptr, BINARY_TBL, nrows, ncols, ttype, 
                tform, tunit, 0, status);

    /* Add standard header keywords. */
    ffpkyl (outfptr, "ZIMAGE", 1, 
           "extension contains compressed image", status);  

    if (writebitpix) {
        /*  write the keywords defining the datatype and dimensions of */
	/*  the uncompressed image.  These keywords may get created */
        /*  later, copied from the input uncompressed image  */
	   
        ffpkyj (outfptr, "ZBITPIX", bitpix,
			"data type of original image", status);
        ffpkyj (outfptr, "ZNAXIS", naxis,
			"dimension of original image", status);

        for (ii = 0;  ii < naxis;  ii++)
        {
            sprintf (keyname, "ZNAXIS%d", ii+1);
            ffpkyj (outfptr, keyname, naxes[ii],
			"length of original image axis", status);
        }
    }
                      
    for (ii = 0;  ii < naxis;  ii++)
    {
        sprintf (keyname, "ZTILE%d", ii+1);
        ffpkyj (outfptr, keyname, (outfptr->Fptr)->request_tilesize[ii],
			"size of tiles to be compressed", status);
    }

    ffpkys (outfptr, "ZCMPTYPE", zcmptype,
	          "compression algorithm", status);

    /* write any algorithm-specific keywords */
    if ((outfptr->Fptr)->request_compress_type == RICE_1)
    {
        ffpkys (outfptr, "ZNAME1", "BLOCKSIZE",
            "compression block size", status);

        /* for now at least, the block size is always 32 */
        ffpkyj (outfptr, "ZVAL1", 32,
			"pixels per block", status);

        if (bitpix < 0 )  /* floating point image */
        {
            ffpkys (outfptr, "ZNAME2", "NOISEBIT",
                "floating point quantization level", status);

            ffpkyj (outfptr, "ZVAL2", (long) (outfptr->Fptr)->request_noise_nbits,
                "floating point quantization level", status);
        }
    }
    else if ((outfptr->Fptr)->request_compress_type == HCOMPRESS_1)
    {
        ffpkys (outfptr, "ZNAME1", "SCALE",
            "HCOMPRESS scale factor", status);
        ffpkyj (outfptr, "ZVAL1", (long) (outfptr->Fptr)->request_hcomp_scale,
			"HCOMPRESS scale factor", status);

        ffpkys (outfptr, "ZNAME2", "SMOOTH",
            "HCOMPRESS smooth option", status);
        ffpkyj (outfptr, "ZVAL2", (long) (outfptr->Fptr)->request_hcomp_smooth,
			"HCOMPRESS smooth option", status);

        if (bitpix < 0 )  /* floating point image */
        {
            ffpkys (outfptr, "ZNAME3", "NOISEBIT",
                "floating point quantization level", status);

            ffpkyj (outfptr, "ZVAL3", (long) (outfptr->Fptr)->request_noise_nbits,
                "floating point quantization level", status);
        }
    }
    else
    {
        if (bitpix < 0 )  /* floating point image */
        {
            ffpkys (outfptr, "ZNAME1", "NOISEBIT",
                "floating point quantization level", status);

            ffpkyj (outfptr, "ZVAL1", (long) (outfptr->Fptr)->request_noise_nbits,
                "floating point quantization level", status);
        }
    }

    /* Write the BSCALE and BZERO keywords, if an unsigned integer image */
    if (inbitpix == USHORT_IMG)
    {
        strcpy(comm, "offset data range to that of unsigned short");
        ffpkyg(outfptr, "BZERO", 32768., 0, comm, status);
        strcpy(comm, "default scaling factor");
        ffpkyg(outfptr, "BSCALE", 1.0, 0, comm, status);
    }
    else if (inbitpix == SBYTE_IMG)
    {
        strcpy(comm, "offset data range to that of signed byte");
        ffpkyg(outfptr, "BZERO", -128., 0, comm, status);
        strcpy(comm, "default scaling factor");
        ffpkyg(outfptr, "BSCALE", 1.0, 0, comm, status);
    }
    else if (inbitpix == ULONG_IMG)
    {
        strcpy(comm, "offset data range to that of unsigned long");
        ffpkyg(outfptr, "BZERO", 2147483648., 0, comm, status);
        strcpy(comm, "default scaling factor");
        ffpkyg(outfptr, "BSCALE", 1.0, 0, comm, status);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_calc_max_elem (int comptype, int nx, int zbitpix, int blocksize)

/* This function returns the maximum number of bytes in a compressed
   image line.

    nx = maximum number of pixels in a tile
    blocksize is only relevant for RICE compression
*/
{    
    if (comptype == RICE_1)
    {
        return (sizeof(float) * nx + nx / blocksize + 2 + 4);
    }
    else if (comptype == GZIP_1)
    {
        /* gzip usually compressed by at least a factor of 2 */
        /* If this size turns out to be too small, then the gzip */
        /* compression routine will allocate more space as required */

        return(nx * sizeof(int) / 2);
    }
    else if (comptype == HCOMPRESS_1)
    {
        /* Imperical evidence suggests in the worst case, 
	   the compressed stream could be up to 10% larger than the original
	   image.  Add 26 byte overhead, only significant for very small tiles
	   
         Possible improvement: may need to allow a larger size for 32-bit images */

        if (zbitpix == 16 || zbitpix == 8)
	
            return( (int) (nx * 2.2 + 26));   /* will be compressing 16-bit int array */
        else
            return( (int) (nx * 4.4 + 26));   /* will be compressing 32-bit int array */
    }
    else
        return(nx * sizeof(int));
}
/*--------------------------------------------------------------------------*/
int imcomp_compress_image (fitsfile *infptr, fitsfile *outfptr, int *status)

/* This routine does the following:
        - reads an image one tile at a time
        - if it is a float or double image, then it quantizes the pixels
        - compresses the integer pixel values
        - writes the compressed byte stream to the FITS file.

   If the tile cannot be quantized than the raw float or double values
   are written to the output table.

*/
{
    double *tiledata = 0;
    int anynul, gotnulls = 0, datatype, tstatus, colnum;
    long ii, row, nelem, offset;
    int naxis;
    double dummy = 0.;
    long maxtilelen, tilelen, incre[] = {1, 1, 1, 1, 1, 1};
    long naxes[MAX_COMPRESS_DIM], fpixel[MAX_COMPRESS_DIM];
    long lpixel[MAX_COMPRESS_DIM], tile[MAX_COMPRESS_DIM];
    long tilesize[MAX_COMPRESS_DIM];
    long i0, i1, i2, i3, i4, i5;
    char card[FLEN_CARD];

    if (*status > 0)
        return(*status);

    maxtilelen = (outfptr->Fptr)->maxtilelen;

    /* allocate buffer to hold 1 tile of data */
    /* if the hcompress algorithm is being used on BITPIX = 32, -32, or -64  */
    /* image, then we need to allocate 8-bytes per pixel buffer */
    
    if ((outfptr->Fptr)->zbitpix == FLOAT_IMG)
    {
        datatype = TFLOAT;

        if ( (outfptr->Fptr)->compress_type == HCOMPRESS_1) {
	    /* need twice as much scratch space (8 bytes per pixel) */
            tiledata = (double*) calloc (maxtilelen * 2, sizeof (float));	
	} else {
            tiledata = (double*) calloc (maxtilelen, sizeof (float));
	}
    }
    else if ((outfptr->Fptr)->zbitpix == DOUBLE_IMG)
    {
        datatype = TDOUBLE;
        tiledata = (double*) calloc (maxtilelen, sizeof (double));
    }
    else
    {
        datatype = TINT;

        if ( (outfptr->Fptr)->compress_type == HCOMPRESS_1 &&
             (outfptr->Fptr)->zbitpix == LONG_IMG) {
	    /* need twice as much scratch space (8 bytes per pixel) */

            tiledata = (double*) calloc (maxtilelen * 2, sizeof (int));	
	} else {
            tiledata = (double*) calloc (maxtilelen, sizeof (int));
        }
    }

    if (tiledata == NULL)
    {
	ffpmsg("Out of memory. (imcomp_compress_image)");
	return (*status = MEMORY_ALLOCATION);
    }

    /*  calculate size of tile in each dimension */
    naxis = (outfptr->Fptr)->zndim;
    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        if (ii < naxis)
        {
             naxes[ii] = (outfptr->Fptr)->znaxis[ii];
             tilesize[ii] = (outfptr->Fptr)->tilesize[ii];
        }
        else
        {
            naxes[ii] = 1;
            tilesize[ii] = 1;
        }
    }
    row = 1;

    /* set up big loop over up to 6 dimensions */
    for (i5 = 1; i5 <= naxes[5]; i5 += tilesize[5])
    {
     fpixel[5] = i5;
     lpixel[5] = minvalue(i5 + tilesize[5] - 1, naxes[5]);
     tile[5] = lpixel[5] - fpixel[5] + 1;
     for (i4 = 1; i4 <= naxes[4]; i4 += tilesize[4])
     {
      fpixel[4] = i4;
      lpixel[4] = minvalue(i4 + tilesize[4] - 1, naxes[4]);
      tile[4] = lpixel[4] - fpixel[4] + 1;
      for (i3 = 1; i3 <= naxes[3]; i3 += tilesize[3])
      {
       fpixel[3] = i3;
       lpixel[3] = minvalue(i3 + tilesize[3] - 1, naxes[3]);
       tile[3] = lpixel[3] - fpixel[3] + 1;
       for (i2 = 1; i2 <= naxes[2]; i2 += tilesize[2])
       {
        fpixel[2] = i2;
        lpixel[2] = minvalue(i2 + tilesize[2] - 1, naxes[2]);
        tile[2] = lpixel[2] - fpixel[2] + 1;
        for (i1 = 1; i1 <= naxes[1]; i1 += tilesize[1])
        {
         fpixel[1] = i1;
         lpixel[1] = minvalue(i1 + tilesize[1] - 1, naxes[1]);
         tile[1] = lpixel[1] - fpixel[1] + 1;
         for (i0 = 1; i0 <= naxes[0]; i0 += tilesize[0])
         {
          fpixel[0] = i0;
          lpixel[0] = minvalue(i0 + tilesize[0] - 1, naxes[0]);
          tile[0] = lpixel[0] - fpixel[0] + 1;

          /* number of pixels in this tile */
          tilelen = tile[0];
          for (ii = 1; ii < naxis; ii++)
          {
             tilelen *= tile[ii];
          }

          /* read next tile of data from image */
          if (datatype == TFLOAT)
          {
              ffgsve(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  FLOATNULLVALUE, (float *) tiledata,  &anynul, status);
          }
          else if (datatype == TDOUBLE)
          {
              ffgsvd(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  DOUBLENULLVALUE, tiledata, &anynul, status);
          }
          else  /* read all integer data types as int */
          {

              ffgsvk(infptr, 1, naxis, naxes, fpixel, lpixel, incre, 
                  0, (int *) tiledata,  &anynul, status);
          }

          /* now compress the tile, and write to row of binary table */
          imcomp_compress_tile(outfptr, row, datatype, tiledata, tilelen,
                               tile[0], tile[1], 0, &dummy, status);

          /* set flag if we found any null values */
          if (anynul)
              gotnulls = 1;

          /* check for any error in the previous operations */
          if (*status > 0)
          {
              ffpmsg("Error writing compressed image to table");
              free(tiledata);
              return (*status);
          }

	  row++;
         }
        }
       }
      }
     }
    }

    free (tiledata);  /* finished with this buffer */

    /* insert ZBLANK keyword if necessary; only for TFLOAT or TDOUBLE images */
    if (gotnulls)
    {
          ffgcrd(outfptr, "ZCMPTYPE", card, status);
          ffikyj(outfptr, "ZBLANK", COMPRESS_NULL_VALUE, 
             "null value in the compressed integer array", status);
    }

    if (datatype >= TFLOAT )
    {
       /* check if any data were written to the UNCOMPRESSED_DATA column */
       /* If not, then delete that column from the table */
       for (ii = 1; ii < row; ii++)
       {
          ffgdes (outfptr, (outfptr->Fptr)->cn_uncompressed, ii, 
                  &nelem, &offset, status);
          if (nelem)
              break;   
       }

       if (!nelem)
       {
           tstatus = 0;
           ffgcno(outfptr, CASEINSEN, "UNCOMPRESSED_DATA", &colnum, &tstatus);
           if (tstatus == 0)
           {
              /* make sure table is properly terminated before deleting col */
               /* (in particular, make sure the '1PB(nnn)' keyword is updated */
               ffrdef(outfptr, status);
               ffdcol(outfptr, colnum, status);
           }
       }
    }

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_compress_tile (fitsfile *outfptr,
    long row, 
    int datatype, 
    void *tiledata, 
    long tilelen,
    long tilenx,
    long tileny,
    int nullcheck,
    void *nullflagval,
    int *status)

/*
   This is the main compression routine.

   This routine does the following to the input tile of pixels:
        - if it is a float or double image, then it quantizes the pixels
        - compresses the integer pixel values
        - writes the compressed byte stream to the FITS file.

   If the tile cannot be quantized than the raw float or double values
   are written to the output table.


   The input tiledata array must only be of type TINT, TFLOAT, or TDOUBLE.
   Shorter integer types will be type converted to TINT, under the assumption
   that the input array was allocated with enough scratch space to do this
   in place in memory.
   
   This array may be modified by this routine.  If the array is of type TINT
   or TFLOAT, and the compression type is HCOMPRESS, then it must have been 
   allocated to be twice as large (8 bytes per pixel) to provide scratch space.
*/
{
    int *idata = 0;		/* quantized integer data */
    short *cbuf;	/* compressed data */
    short *sbuff;
    unsigned short *usbuff;
    int *intbuff;
    unsigned int *uintbuff;
    int clen;		/* size of cbuf */
    int flag = 1; /* true by default; only = 0 if float data couldn't be quantized */
    int iminval = 0, imaxval = 0;  /* min and max quantized integers */
    double bscale[1] = {1.}, bzero[1] = {0.};	/* scaling parameters */
    double scale, zero;
    int  nelem = 0;		/* number of bytes */
    size_t gzip_nelem = 0;
    long ii, hcomp_len;
    LONGLONG *lldata;
    signed char *sbbuff;
    unsigned char *usbbuff;
    long *lbuff;
    unsigned long *ulbuff;
    int hcompscale, cn_zblank, zbitpix, nullval, flagval = 0;
    float floatnull;
    double doublenull;

    if (*status > 0)
        return(*status);

    idata = (int *) tiledata;
    hcompscale = (outfptr->Fptr)->hcomp_scale;
    zbitpix = (outfptr->Fptr)->zbitpix;

    /* if the tile/image has an integer datatype, see if a null value has */
    /* been defined (with the BLANK keyword in a normal FITS image).  */
    /* If so, and if the input tile array also contains null pixels, */
    /* (represented by pixels that have a value = nullflagval) then  */
    /* any pixels whose value = nullflagval, must be set to the value = nullval */
    /* before the pixel array is compressed.  These null pixel values must */
    /* not be inverse scaled by the BSCALE/BZERO values, if present. */

    cn_zblank = (outfptr->Fptr)->cn_zblank;
    nullval = (outfptr->Fptr)->zblank;

    if (zbitpix > 0 && cn_zblank != -1)  /* If the integer image has no defined null */
        nullcheck = 0;    /* value, then don't bother checking input array for nulls. */

    /* if the BSCALE and BZERO keywords exist, then the input values must */
    /* be inverse scaled by this factor, before the values are compressed. */
    /* (The program may have turned off scaling, which over rides the keywords) */
    
    scale = (outfptr->Fptr)->cn_bscale;
    zero  = (outfptr->Fptr)->cn_bzero;
	   

    /*  convert input tile array in place to 4-byte ints for compression */
    /*  Note that the calling routine must have allocated the array big enough */
    /* to be able to do this.  */

    if (datatype == TSHORT)
    {
       sbuff = (short *) tiledata;
       if (nullcheck == 1)
           flagval = *(short *) (nullflagval);
       
       for (ii = tilelen; ii >= 0; ii--)
	  idata[ii] = (int) (sbuff[ii]);
    }
    else if (datatype == TUSHORT)
    {
       usbuff = (unsigned short *) tiledata;
       if (nullcheck == 1)
           flagval = *(unsigned short *) (nullflagval);

       for (ii = tilelen; ii >= 0; ii--)
            idata[ii] = (int) (usbuff[ii]);
    }
    else if (datatype == TINT)
    {
       if (nullcheck == 1)
           flagval = *(int *) (nullflagval);
         /* no conversion necessary */
    }
    else if (datatype == TUINT)
    {
       if (nullcheck == 1)
           flagval = *(unsigned int *) (nullflagval);
    }
    else if (datatype == TBYTE)
    {
       usbbuff = (unsigned char *) tiledata;
       if (nullcheck == 1)
           flagval = *(unsigned char *) (nullflagval);
       for (ii = tilelen; ii >= 0; ii--)
            idata[ii] = (int) (usbbuff[ii]);
    }
    else if (datatype == TSBYTE)
    {
       sbbuff = (signed char *) tiledata;
       if (nullcheck == 1)
           flagval = *(signed char *) (nullflagval);
       for (ii = tilelen; ii >= 0; ii--)
            idata[ii] = (int) (sbbuff[ii]);
    }
    else if (datatype == TLONG && sizeof(long) == 8)
    {
       /* warning: potential for data overflow here */
       if (nullcheck == 1)
           flagval = *(long *) (nullflagval);
       lbuff = (long *) tiledata;
       for (ii = 0; ii < tilelen; ii++)
             idata[ii] = (int) (lbuff[ii]);
    }
    else if (datatype == TLONG)
    {
       if (nullcheck == 1)
           flagval = *(long *) (nullflagval);
    }
    else if (datatype == TULONG && sizeof(long) == 8)
    {
       /* warning: potential for data overflow here */
       if (nullcheck == 1)
           flagval = *(unsigned long *) (nullflagval);
       ulbuff = (unsigned long *) tiledata;
       for (ii = 0; ii < tilelen; ii++)
             idata[ii] = (int) (ulbuff[ii]);
    }
    else if (datatype == TULONG)
    {
       if (nullcheck == 1)
           flagval = *(unsigned long *) (nullflagval);
    }
    
    else if (datatype == TFLOAT)
    {
          /* if the tile-compressed table contains zscale and zzero columns */
          /* then scale and quantize the input floating point data.    */
          /* Otherwise, just truncate the floats to (scaled) integers.     */
          if ((outfptr->Fptr)->cn_zscale > 0)
          {
            if (nullcheck == 1)
	      floatnull = *(float *) (nullflagval);
	    else
	      floatnull = FLOATNULLVALUE;
	      
            /* quantize the float values into integers */
            flag = fits_quantize_float ((float *) tiledata, tilelen,
               floatnull, (outfptr->Fptr)->noise_nbits, idata,
               bscale, bzero, &iminval, &imaxval);
	       
            /* adjust the hcompress scale by the same scaling factor */
            if (hcompscale > 1) hcompscale = (int) (hcompscale / bscale[0]);

          }
          else  /* input float data is implicitly converted to integer image */
          {
            if ((scale != 1. || zero != 0.))  /* must scale the values */
	       imcomp_nullscalefloats((float *) tiledata, tilelen, idata, scale, zero,
	           nullcheck, *(float *) (nullflagval), nullval, status);
             else
	       imcomp_nullfloats((float *) tiledata, tilelen, idata,
	           nullcheck, *(float *) (nullflagval), nullval,  status);
          }
    }
    else if (datatype == TDOUBLE)
    {
          /* if the tile-compressed table contains zscale and zzero columns */
          /* then scale and quantize the input floating point data.    */
          /* Otherwise, just truncate the floats to integers.          */

          if ((outfptr->Fptr)->cn_zscale > 0)
          {
            if (nullcheck == 1)
	      doublenull = *(double *) (nullflagval);
	    else
	      doublenull = DOUBLENULLVALUE;
	      
            /* quantize the double values into integers */
            flag = fits_quantize_double ((double *) tiledata, tilelen,
               doublenull, (outfptr->Fptr)->noise_nbits, idata,
               bscale, bzero, &iminval, &imaxval);

            /* adjust the hcompress scale by the same scaling factor */
            if (hcompscale > 1) hcompscale = (int) (hcompscale / bscale[0]);

          }
          else
          {
            if ((scale != 1. || zero != 0.))  /* must scale the values */
	       imcomp_nullscaledoubles((double *) tiledata, tilelen, idata, scale, zero,
	           nullcheck, *(double *) (nullflagval), nullval, status);
             else
	       imcomp_nulldoubles((double *) tiledata, tilelen, idata,
	           nullcheck, *(double *) (nullflagval), nullval,  status);
          }
    }
    else 
    {
          ffpmsg("unsupported datatype (imcomp_compress_tile)");
          return(*status = BAD_DATATYPE);
    }

    if (datatype < TFLOAT) {  /* see if we need to test for null values, or */
                              /* scale the data values before compression */
    
        if (zbitpix < 0 )  /* If writing an integer input array to a float or double */
             nullval = -2147483647;  /* choose an arbitrary value for null pixels.  */

        if (nullval == flagval)  /* If the flag value is the same as the */
            nullcheck = 0;          /*  null value, then no need to test nulls.  */

        if ((nullcheck ==1)  && (scale != 1. || zero != 0.))  /* do both */
	   imcomp_nullscale(idata, tilelen, flagval, nullval,
	       scale, zero, status);

        else if (nullcheck == 1)   /* just do null value substitution */
	   imcomp_nullvalues(idata, tilelen, flagval, nullval, status);

        else if (scale != 1. || zero != 0.)   /* just do scaling */
	   imcomp_scalevalues(idata, tilelen, scale, zero, status);
    }      

    if (flag)   /* we can now compress the int array */
    {
        /* allocate buffer for the compressed tile bytes */
        clen = (outfptr->Fptr)->maxelem;
        cbuf = (short *) calloc (clen, sizeof (unsigned char));
        if (cbuf == NULL)
        {
            ffpmsg("Out of memory. (imcomp_compress_tile)");
	    return (*status = MEMORY_ALLOCATION);
        }

	/* Compress the integer data, then write the compressed bytes */
        if ( (outfptr->Fptr)->compress_type == RICE_1)
        {
  	        nelem = fits_rcomp (idata, tilelen, (unsigned char *) cbuf,
                       clen, (outfptr->Fptr)->rice_blocksize);

	        /* Write the compressed byte stream. */
                ffpclb(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     nelem, (unsigned char *) cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == PLIO_1)
        {
                if (iminval < 0 || imaxval > 16777215)
                {
                   /* plio algorithn only supports positive 24 bit ints */
                   ffpmsg("data out of range for PLIO compression (0 - 2**24)");
                   return(*status = DATA_DECOMPRESSION_ERR);
                }

  	        nelem = pl_p2li (idata, 1, cbuf, tilelen);

	        /* Write the compressed byte stream. */
                ffpcli(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     nelem, cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == GZIP_1)
        {

#if BYTESWAPPED
               ffswap4(idata, tilelen); /* reverse order of bytes */
#endif
                compress2mem_from_mem((char *) idata, tilelen * sizeof(int),
                 (char **) &cbuf, (size_t *) &clen, realloc, 
                 &gzip_nelem, status);

	        /* Write the compressed byte stream. */
                ffpclb(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     gzip_nelem, (unsigned char *) cbuf, status);
        }
        else if ( (outfptr->Fptr)->compress_type == HCOMPRESS_1)
        {
            hcomp_len = clen;  /* allocated size of the buffer */
	    
            if ((outfptr->Fptr)->zbitpix == BYTE_IMG ||
	        (outfptr->Fptr)->zbitpix == SHORT_IMG) {

                fits_hcompress(idata, tilenx, tileny, 
		  hcompscale, (char *) cbuf, &hcomp_len, status);

            } else {
                 /* have to convert idata to an I*8 array, in place */
                 /* idata must have been allocated large enough to do this */
                lldata = (LONGLONG *) idata;
		
                for (ii = tilelen; ii >= 0; ii--) {
		    lldata[ii] = idata[ii];;
		}

                fits_hcompress64(lldata, tilenx, tileny, 
		  hcompscale, (char *) cbuf, &hcomp_len, status);
            }

	    /* Write the compressed byte stream. */
            ffpclb(outfptr, (outfptr->Fptr)->cn_compressed, row, 1,
                     hcomp_len, (unsigned char *) cbuf, status);
        }

	if (nelem < 0)  /* error condition */
        {
	    free (cbuf);
            ffpmsg
                ("error compressing row of the image (imcomp_compress_tile)");
            return (*status = DATA_COMPRESSION_ERR);
        }

        if ((outfptr->Fptr)->cn_zscale > 0)
        {
              /* write the linear scaling parameters */
	      ffpcld (outfptr, (outfptr->Fptr)->cn_zscale, row, 1, 1,
                      bscale, status);
	      ffpcld (outfptr, (outfptr->Fptr)->cn_zzero,  row, 1, 1,
                      bzero,  status);
         }

         free(cbuf);  /* finished with this buffer */
    } 
    else     /* floating point data couldn't be quantized */
    {
	 /* Write the original floating point data. */
         if (datatype == TFLOAT)
         {
              ffpcle (outfptr, (outfptr->Fptr)->cn_uncompressed, row, 1,
                      tilelen, (float *)tiledata, status);
         }
         else if (datatype == TDOUBLE)
         {
              ffpcld (outfptr, (outfptr->Fptr)->cn_uncompressed, row, 1,
                      tilelen, (double *)tiledata, status);
         }
    }

    return (*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nullscale(
     int *idata, 
     long tilelen,
     int nullflagval,
     int nullval,
     double scale,
     double zero,
     int *status)
/*
   do null value substitution AND scaling of the integer array.
   If array value = nullflagval, then set the value to nullval.
   Otherwise, inverse scale the integer value.
*/
{
    long ii;
    double dvalue;
    
    for (ii=0; ii < tilelen; ii++)
    {
        if (idata[ii] == nullflagval)
	    idata[ii] = nullval;
	else 
	{
            dvalue = (idata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
        }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nullvalues(
     int *idata, 
     long tilelen,
     int nullflagval,
     int nullval,
     int *status)
/*
   do null value substitution.
   If array value = nullflagval, then set the value to nullval.
*/
{
    long ii;
    
    for (ii=0; ii < tilelen; ii++)
    {
        if (idata[ii] == nullflagval)
	    idata[ii] = nullval;
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_scalevalues(
     int *idata, 
     long tilelen,
     double scale,
     double zero,
     int *status)
/*
   do inverse scaling the integer values.
*/
{
    long ii;
    double dvalue;
    
    for (ii=0; ii < tilelen; ii++)
    {
            dvalue = (idata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nullfloats(
     float *fdata,
     long tilelen,
     int *idata, 
     int nullcheck,
     float nullflagval,
     int nullval,
     int *status)
/*
   do null value substitution  of the float array.
   If array value = nullflagval, then set the output value to FLOATNULLVALUE.
   Otherwise, inverse scale the integer value.
*/
{
    long ii;
    double dvalue;
    
    if (nullcheck == 1) /* must check for null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
        if (fdata[ii] == nullflagval)
	    idata[ii] = nullval;
	else 
	{
            dvalue = fdata[ii];

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
        }
      }
    }
    else  /* don't have to worry about null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
            dvalue = fdata[ii];

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
      }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nullscalefloats(
     float *fdata,
     long tilelen,
     int *idata, 
     double scale,
     double zero,
     int nullcheck,
     float nullflagval,
     int nullval,
     int *status)
/*
   do null value substitution  of the float array.
   If array value = nullflagval, then set the output value to FLOATNULLVALUE.
   Otherwise, inverse scale the integer value.
*/
{
    long ii;
    double dvalue;
    
    if (nullcheck == 1) /* must check for null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
        if (fdata[ii] == nullflagval)
	    idata[ii] = nullval;
	else 
	{
            dvalue = (fdata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
        }
      }
    }
    else  /* don't have to worry about null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
            dvalue = (fdata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
      }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nulldoubles(
     double *fdata,
     long tilelen,
     int *idata, 
     int nullcheck,
     double nullflagval,
     int nullval,
     int *status)
/*
   do null value substitution  of the float array.
   If array value = nullflagval, then set the output value to FLOATNULLVALUE.
   Otherwise, inverse scale the integer value.
*/
{
    long ii;
    double dvalue;
    
    if (nullcheck == 1) /* must check for null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
        if (fdata[ii] == nullflagval)
	    idata[ii] = nullval;
	else 
	{
            dvalue = fdata[ii];

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
        }
      }
    }
    else  /* don't have to worry about null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
            dvalue = fdata[ii];

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
      }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int imcomp_nullscaledoubles(
     double *fdata,
     long tilelen,
     int *idata, 
     double scale,
     double zero,
     int nullcheck,
     double nullflagval,
     int nullval,
     int *status)
/*
   do null value substitution  of the float array.
   If array value = nullflagval, then set the output value to FLOATNULLVALUE.
   Otherwise, inverse scale the integer value.
*/
{
    long ii;
    double dvalue;
    
    if (nullcheck == 1) /* must check for null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
        if (fdata[ii] == nullflagval)
	    idata[ii] = nullval;
	else 
	{
            dvalue = (fdata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
        }
      }
    }
    else  /* don't have to worry about null values */
    {
      for (ii=0; ii < tilelen; ii++)
      {
            dvalue = (fdata[ii] - zero) / scale;

            if (dvalue < DINT_MIN)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MIN;
            }
            else if (dvalue > DINT_MAX)
            {
                *status = OVERFLOW_ERR;
                idata[ii] = INT32_MAX;
            }
            else
            {
                if (dvalue >= 0)
                    idata[ii] = (int) (dvalue + .5);
                else
                    idata[ii] = (int) (dvalue - .5);
            }
      }
    }
    return(*status);
}
/*---------------------------------------------------------------------------*/
int fits_write_compressed_img(fitsfile *fptr,   /* I - FITS file pointer     */
            int  datatype,   /* I - datatype of the array to be written      */
            long  *infpixel, /* I - 'bottom left corner' of the subsection   */
            long  *inlpixel, /* I - 'top right corner' of the subsection     */
            int  nullcheck,  /* I - 0 for no null checking                   */
                             /*     1: pixels that are = nullval will be     */
                             /*     written with the FITS null pixel value   */
                             /*     (floating point arrays only)             */
            void *array,     /* I - array of values to be written            */
            void *nullval,   /* I - undefined pixel value                    */
            int  *status)    /* IO - error status                            */
/*
   Write a section of a compressed image.
*/
{
    int naxis[MAX_COMPRESS_DIM], tiledim[MAX_COMPRESS_DIM];
    long tilesize[MAX_COMPRESS_DIM], thistilesize[MAX_COMPRESS_DIM];
    long ftile[MAX_COMPRESS_DIM], ltile[MAX_COMPRESS_DIM];
    long tfpixel[MAX_COMPRESS_DIM], tlpixel[MAX_COMPRESS_DIM];
    long rowdim[MAX_COMPRESS_DIM], offset[MAX_COMPRESS_DIM],ntemp;
    long fpixel[MAX_COMPRESS_DIM], lpixel[MAX_COMPRESS_DIM];
    int ii, i5, i4, i3, i2, i1, i0, ndim, irow, pixlen, tilenul;
    int anynull, tstatus, buffpixsiz;
    long totpix;
    void *buffer;
    char *bnullarray = 0, card[FLEN_CARD];
    float floatnull = 0.;
    double doublenull = 0.;

    if (*status > 0) 
        return(*status);

    if (!fits_is_compressed_image(fptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_write_compressed_img)");
        return(*status = DATA_COMPRESSION_ERR);
    }

    /* reset position to the correct HDU if necessary */
    if (fptr->HDUposition != (fptr->Fptr)->curhdu)
        ffmahd(fptr, (fptr->HDUposition) + 1, NULL, status);

    /* rescan header if data structure is undefined */
    else if ((fptr->Fptr)->datastart == DATA_UNDEFINED)
        if ( ffrdef(fptr, status) > 0)               
            return(*status);

    /* get temporary space for uncompressing one image tile */
    /*  Need at least 4-byte per pixel, and in some cases 8-bytes per pixel, */
    
    buffpixsiz = 4;
    if ( (fptr->Fptr)->compress_type == HCOMPRESS_1 &&
                ((fptr->Fptr)->zbitpix != BYTE_IMG &&
	         (fptr->Fptr)->zbitpix != SHORT_IMG) ){

              buffpixsiz = 8;
    }
    if (datatype == TDOUBLE)
	 buffpixsiz = 8;

    /* cast to double to force alignment on 8-byte addresses */
    buffer = (double *) calloc ((fptr->Fptr)->maxtilelen, buffpixsiz);

    if (datatype == TSHORT || datatype == TUSHORT)
    {
       pixlen = sizeof(short);
    }
    else if (datatype == TINT || datatype == TUINT)
    {
       pixlen = sizeof(int);
    }
    else if (datatype == TBYTE || datatype == TSBYTE)
    {
       pixlen = 1;
    }
    else if (datatype == TLONG || datatype == TULONG)
    {
       pixlen = sizeof(long);
    }
    else if (datatype == TFLOAT)
    {
       pixlen = sizeof(float);
    }
    else if (datatype == TDOUBLE)
    {
       pixlen = sizeof(double);
    }
    else
    {
        ffpmsg("unsupported datatype for compressing image");
        return(*status = BAD_DATATYPE);
    }

    if (buffer == NULL)
    {
	    ffpmsg("Out of memory (fits_write_compress_img)");
	    return (*status = MEMORY_ALLOCATION);
    }

    /* initialize all the arrays */
    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxis[ii] = 1;
        tiledim[ii] = 1;
        tilesize[ii] = 1;
        ftile[ii] = 1;
        ltile[ii] = 1;
        rowdim[ii] = 1;
    }

    ndim = (fptr->Fptr)->zndim;
    ntemp = 1;
    for (ii = 0; ii < ndim; ii++)
    {
        fpixel[ii] = infpixel[ii];
        lpixel[ii] = inlpixel[ii];

        /* calc number of tiles in each dimension, and tile containing */
        /* the first and last pixel we want to read in each dimension  */
        naxis[ii] = (fptr->Fptr)->znaxis[ii];
        if (fpixel[ii] < 1)
        {
            free(buffer);
            return(*status = BAD_PIX_NUM);
        }

        tilesize[ii] = (fptr->Fptr)->tilesize[ii];
        tiledim[ii] = (naxis[ii] - 1) / tilesize[ii] + 1;
        ftile[ii]   = (fpixel[ii] - 1)   / tilesize[ii] + 1;
        ltile[ii]   = minvalue((lpixel[ii] - 1) / tilesize[ii] + 1, 
                                tiledim[ii]);
        rowdim[ii]  = ntemp;  /* total tiles in each dimension */
        ntemp *= tiledim[ii];
    }

    /* support up to 6 dimensions for now */
    /* tfpixel and tlpixel are the first and last image pixels */
    /* along each dimension of the compression tile */
    for (i5 = ftile[5]; i5 <= ltile[5]; i5++)
    {
     tfpixel[5] = (i5 - 1) * tilesize[5] + 1;
     tlpixel[5] = minvalue(tfpixel[5] + tilesize[5] - 1, 
                            naxis[5]);
     thistilesize[5] = tlpixel[5] - tfpixel[5] + 1;
     offset[5] = (i5 - 1) * rowdim[5];
     for (i4 = ftile[4]; i4 <= ltile[4]; i4++)
     {
      tfpixel[4] = (i4 - 1) * tilesize[4] + 1;
      tlpixel[4] = minvalue(tfpixel[4] + tilesize[4] - 1, 
                            naxis[4]);
      thistilesize[4] = thistilesize[5] * (tlpixel[4] - tfpixel[4] + 1);
      offset[4] = (i4 - 1) * rowdim[4] + offset[5];
      for (i3 = ftile[3]; i3 <= ltile[3]; i3++)
      {
        tfpixel[3] = (i3 - 1) * tilesize[3] + 1;
        tlpixel[3] = minvalue(tfpixel[3] + tilesize[3] - 1, 
                              naxis[3]);
        thistilesize[3] = thistilesize[4] * (tlpixel[3] - tfpixel[3] + 1);
        offset[3] = (i3 - 1) * rowdim[3] + offset[4];
        for (i2 = ftile[2]; i2 <= ltile[2]; i2++)
        {
          tfpixel[2] = (i2 - 1) * tilesize[2] + 1;
          tlpixel[2] = minvalue(tfpixel[2] + tilesize[2] - 1, 
                                naxis[2]);
          thistilesize[2] = thistilesize[3] * (tlpixel[2] - tfpixel[2] + 1);
          offset[2] = (i2 - 1) * rowdim[2] + offset[3];
          for (i1 = ftile[1]; i1 <= ltile[1]; i1++)
          {
            tfpixel[1] = (i1 - 1) * tilesize[1] + 1;
            tlpixel[1] = minvalue(tfpixel[1] + tilesize[1] - 1, 
                                  naxis[1]);
            thistilesize[1] = thistilesize[2] * (tlpixel[1] - tfpixel[1] + 1);
            offset[1] = (i1 - 1) * rowdim[1] + offset[2];
            for (i0 = ftile[0]; i0 <= ltile[0]; i0++)
            {
              tfpixel[0] = (i0 - 1) * tilesize[0] + 1;
              tlpixel[0] = minvalue(tfpixel[0] + tilesize[0] - 1, 
                                    naxis[0]);
              thistilesize[0] = thistilesize[1] * (tlpixel[0] - tfpixel[0] + 1);
              /* calculate row of table containing this tile */
              irow = i0 + offset[1];

              /* read and uncompress this row (tile) of the table */
              /* also do type conversion and undefined pixel substitution */
              /* at this point */
              imcomp_decompress_tile(fptr, irow, thistilesize[0],
                    datatype, nullcheck, nullval, buffer, bnullarray, &tilenul,
                     status);

              if (*status == NO_COMPRESSED_TILE)
              {
                   /* tile doesn't exist, so initialize to zero */
                   memset(buffer, 0, pixlen * thistilesize[0]);
                   *status = 0;
              }

              /* copy the intersecting pixels to this tile from the input */
              imcomp_merge_overlap(buffer, pixlen, ndim, tfpixel, tlpixel, 
                     bnullarray, array, fpixel, lpixel, nullcheck, status);

              /* compress the tile again, and write it back to the FITS file */
              imcomp_compress_tile (fptr, irow, datatype, buffer, 
                                    thistilesize[0],
				    tlpixel[0] - tfpixel[0] + 1,
				    tlpixel[1] - tfpixel[1] + 1,
				    nullcheck, nullval, 
				    status);
            }
          }
        }
      }
     }
    }
    free(buffer);
    

    if ((fptr->Fptr)->zbitpix < 0 && nullcheck != 0) { 
/*
     This is a floating point FITS image with possible null values.
     It is too messy to test if any null values are actually written, so 
     just assume so.  We need to make sure that the
     ZBLANK keyword is present in the compressed image header.  If it is not
     there then we need to insert the keyword. 
*/   
        tstatus = 0;
        ffgcrd(fptr, "ZBLANK", card, &tstatus);

	if (tstatus) {   /* have to insert the ZBLANK keyword */
           ffgcrd(fptr, "ZCMPTYPE", card, status);
           ffikyj(fptr, "ZBLANK", COMPRESS_NULL_VALUE, 
                "null value in the compressed integer array", status);
	
           /* set this value into the internal structure; it is used if */
	   /* the program reads back the values from the array */
	 
          (fptr->Fptr)->zblank = COMPRESS_NULL_VALUE;
          (fptr->Fptr)->cn_zblank = -1;  /* flag for a constant ZBLANK */
        }  
    }  
    
    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_write_compressed_pixels(fitsfile *fptr, /* I - FITS file pointer   */
            int  datatype,  /* I - datatype of the array to be written      */
            LONGLONG   fpixel,  /* I - 'first pixel to write          */
            LONGLONG   npixel,  /* I - number of pixels to write      */
            int  nullcheck,  /* I - 0 for no null checking                   */
                             /*     1: pixels that are = nullval will be     */
                             /*     written with the FITS null pixel value   */
                             /*     (floating point arrays only)             */
            void *array,      /* I - array of values to write                */
            void *nullval,    /* I - value used to represent undefined pixels*/
            int  *status)     /* IO - error status                           */
/*
   Write a consecutive set of pixels to a compressed image.  This routine
   interpretes the n-dimensional image as a long one-dimensional array. 
   This is actually a rather inconvenient way to write compressed images in
   general, and could be rather inefficient if the requested pixels to be
   written are located in many different image compression tiles.    

   The general strategy used here is to write the requested pixels in blocks
   that correspond to rectangular image sections.  
*/
{
    int naxis, ii, bytesperpixel;
    long naxes[MAX_COMPRESS_DIM], nread;
    LONGLONG tfirst, tlast, last0, last1, dimsize[MAX_COMPRESS_DIM];
    long nplane, firstcoord[MAX_COMPRESS_DIM], lastcoord[MAX_COMPRESS_DIM];
    char *arrayptr;

    if (*status > 0)
        return(*status);

    arrayptr = (char *) array;

    /* get size of array pixels, in bytes */
    bytesperpixel = ffpxsz(datatype);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxes[ii] = 1;
        firstcoord[ii] = 0;
        lastcoord[ii] = 0;
    }

    /*  determine the dimensions of the image to be read */
    ffgidm(fptr, &naxis, status);
    ffgisz(fptr, MAX_COMPRESS_DIM, naxes, status);

    /* calc the cumulative number of pixels in each successive dimension */
    dimsize[0] = 1;
    for (ii = 1; ii < MAX_COMPRESS_DIM; ii++)
         dimsize[ii] = dimsize[ii - 1] * naxes[ii - 1];

    /*  determine the coordinate of the first and last pixel in the image */
    /*  Use zero based indexes here */
    tfirst = fpixel - 1;
    tlast = tfirst + npixel - 1;
    for (ii = naxis - 1; ii >= 0; ii--)
    {
        firstcoord[ii] = (long) (tfirst / dimsize[ii]);
        lastcoord[ii]  = (long) (tlast / dimsize[ii]);
        tfirst = tfirst - firstcoord[ii] * dimsize[ii];
        tlast = tlast - lastcoord[ii] * dimsize[ii];
    }

    /* to simplify things, treat 1-D, 2-D, and 3-D images as separate cases */

    if (naxis == 1)
    {
        /* Simple: just write the requested range of pixels */

        firstcoord[0] = firstcoord[0] + 1;
        lastcoord[0] = lastcoord[0] + 1;
        fits_write_compressed_img(fptr, datatype, firstcoord, lastcoord,
            nullcheck, array, nullval, status);
        return(*status);
    }
    else if (naxis == 2)
    {
        nplane = 0;  /* write 1st (and only) plane of the image */
        fits_write_compressed_img_plane(fptr, datatype, bytesperpixel,
          nplane, firstcoord, lastcoord, naxes, nullcheck,
          array, nullval, &nread, status);
    }
    else if (naxis == 3)
    {
        /* test for special case: writing an integral number of planes */
        if (firstcoord[0] == 0 && firstcoord[1] == 0 &&
            lastcoord[0] == naxes[0] - 1 && lastcoord[1] == naxes[1] - 1)
        {
            for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
            {
                /* convert from zero base to 1 base */
                (firstcoord[ii])++;
                (lastcoord[ii])++;
            }

            /* we can write the contiguous block of pixels in one go */
            fits_write_compressed_img(fptr, datatype, firstcoord, lastcoord,
                nullcheck, array, nullval, status);
            return(*status);
        }

        /* save last coordinate in temporary variables */
        last0 = lastcoord[0];
        last1 = lastcoord[1];

        if (firstcoord[2] < lastcoord[2])
        {
            /* we will write up to the last pixel in all but the last plane */
            lastcoord[0] = naxes[0] - 1;
            lastcoord[1] = naxes[1] - 1;
        }

        /* write one plane of the cube at a time, for simplicity */
        for (nplane = firstcoord[2]; nplane <= lastcoord[2]; nplane++)
        {
            if (nplane == lastcoord[2])
            {
                lastcoord[0] = (long) last0;
                lastcoord[1] = (long) last1;
            }

            fits_write_compressed_img_plane(fptr, datatype, bytesperpixel,
              nplane, firstcoord, lastcoord, naxes, nullcheck,
              arrayptr, nullval, &nread, status);

            /* for all subsequent planes, we start with the first pixel */
            firstcoord[0] = 0;
            firstcoord[1] = 0;

            /* increment pointers to next elements to be written */
            arrayptr = arrayptr + nread * bytesperpixel;
        }
    }
    else
    {
        ffpmsg("only 1D, 2D, or 3D images are currently supported");
        return(*status = DATA_COMPRESSION_ERR);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_write_compressed_img_plane(fitsfile *fptr, /* I - FITS file    */
            int  datatype,  /* I - datatype of the array to be written    */
            int  bytesperpixel, /* I - number of bytes per pixel in array */
            long   nplane,  /* I - which plane of the cube to write      */
            long *firstcoord, /* I coordinate of first pixel to write */
            long *lastcoord,  /* I coordinate of last pixel to write */
            long *naxes,     /* I size of each image dimension */
            int  nullcheck,  /* I - 0 for no null checking                   */
                             /*     1: pixels that are = nullval will be     */
                             /*     written with the FITS null pixel value   */
                             /*     (floating point arrays only)             */
            void *array,      /* I - array of values that are written        */
            void *nullval,    /* I - value for undefined pixels              */
            long *nread,      /* O - total number of pixels written          */
            int  *status)     /* IO - error status                           */

   /*
           in general we have to write the first partial row of the image,
           followed by the middle complete rows, followed by the last
           partial row of the image.  If the first or last rows are complete,
           then write them at the same time as all the middle rows.
    */
{
    /* bottom left coord. and top right coord. */
    long blc[MAX_COMPRESS_DIM], trc[MAX_COMPRESS_DIM]; 
    char *arrayptr;

    *nread = 0;

    arrayptr = (char *) array;

    blc[2] = nplane + 1;
    trc[2] = nplane + 1;

    if (firstcoord[0] != 0)
    { 
            /* have to read a partial first row */
            blc[0] = firstcoord[0] + 1;
            blc[1] = firstcoord[1] + 1;
            trc[1] = blc[1];  
            if (lastcoord[1] == firstcoord[1])
               trc[0] = lastcoord[0] + 1; /* 1st and last pixels in same row */
            else
               trc[0] = naxes[0];  /* read entire rest of the row */

            fits_write_compressed_img(fptr, datatype, blc, trc,
                nullcheck, arrayptr, nullval, status);

            *nread = *nread + trc[0] - blc[0] + 1;

            if (lastcoord[1] == firstcoord[1])
            {
               return(*status);  /* finished */
            }

            /* set starting coord to beginning of next line */
            firstcoord[0] = 0;
            firstcoord[1] += 1;
            arrayptr = arrayptr + (trc[0] - blc[0] + 1) * bytesperpixel;
    }

    /* write contiguous complete rows of the image, if any */
    blc[0] = 1;
    blc[1] = firstcoord[1] + 1;
    trc[0] = naxes[0];

    if (lastcoord[0] + 1 == naxes[0])
    {
            /* can write the last complete row, too */
            trc[1] = lastcoord[1] + 1;
    }
    else
    {
            /* last row is incomplete; have to read it separately */
            trc[1] = lastcoord[1];
    }

    if (trc[1] >= blc[1])  /* must have at least one whole line to read */
    {
        fits_write_compressed_img(fptr, datatype, blc, trc,
                nullcheck, arrayptr, nullval, status);

        *nread = *nread + (trc[1] - blc[1] + 1) * naxes[0];

        if (lastcoord[1] + 1 == trc[1])
               return(*status);  /* finished */

        /* increment pointers for the last partial row */
        arrayptr = arrayptr + (trc[1] - blc[1] + 1) * naxes[0] * bytesperpixel;

     }

    if (trc[1] == lastcoord[1] + 1)
        return(*status);           /* all done */

    /* set starting and ending coord to last line */

    trc[0] = lastcoord[0] + 1;
    trc[1] = lastcoord[1] + 1;
    blc[1] = trc[1];

    fits_write_compressed_img(fptr, datatype, blc, trc,
                nullcheck, arrayptr, nullval, status);

    *nread = *nread + trc[0] - blc[0] + 1;

    return(*status);
}

/* ######################################################################## */
/* ###                 Image Decompression Routines                     ### */
/* ######################################################################## */

/*--------------------------------------------------------------------------*/
int fits_img_decompress (fitsfile *infptr, /* image (bintable) to uncompress */
              fitsfile *outfptr,   /* empty HDU for output uncompressed image */
              int *status)         /* IO - error status               */

/* 
  This routine decompresses the whole image and writes it to the output file.
*/

{
    double *data;
    int ii, datatype = 0, byte_per_pix = 0, naxis, bitpix, numkeys;
    int nullcheck, anynul, tstatus, nullprime = 0, hdupos, norec = 0;
    LONGLONG fpixel[MAX_COMPRESS_DIM], lpixel[MAX_COMPRESS_DIM];
    long inc[MAX_COMPRESS_DIM], naxes[MAX_COMPRESS_DIM];
    long imgsize, memsize;
    float *nulladdr, fnulval;
    double dnulval;
    char card[FLEN_CARD];

    if (*status > 0)
        return(*status);

    if (!fits_is_compressed_image(infptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_img_decompress)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    /* get information about the state of the output file; does it already */
    /* contain any keywords and HDUs?  */
    fits_get_hdu_num(outfptr, &hdupos);  /* Get the current output HDU position */
    fits_get_hdrspace(outfptr, &numkeys, 0, status);

    /* Was the input compressed HDU originally the primary array image? */
    tstatus = 0;
    if (!fits_read_card(infptr, "ZSIMPLE", card, &tstatus)) { 
      /* yes, input HDU was a primary array (not an IMAGE extension) */
      /* Now determine if we can uncompress it into the primary array of */
      /* the output file.  This is only possible if the output file */
      /* currently only contains a null primary array, with no addition */
      /* header keywords and with no following extension in the FITS file. */
      
      if (hdupos == 1) {  /* are we positioned at the primary array? */
	if (numkeys <= 10)  { /* is the header practically empty? */
            if (numkeys == 0) { /* primary HDU is completely empty */

	        nullprime = 1;

            } else {
                fits_get_img_param(outfptr, MAX_COMPRESS_DIM, &bitpix, &naxis, naxes, status);
	
	        if (naxis == 0) /* is this a null image? */
                   nullprime = 1;
		  
           }
	}
      }
    } 

    if (nullprime) {  
       /* We will delete the existing keywords in the null primary array
          and uncompress the input image into the primary array of the output */

       for (ii = numkeys; ii > 0; ii--)
          fits_delete_record(outfptr, ii, status);

    } else  {

       /* if the ZTENSION keyword doesn't exist, then we have to 
          write the required keywords manually */
       tstatus = 0;
       if (fits_read_card(infptr, "ZTENSION", card, &tstatus)) {

          /* create an empty output image with the correct dimensions */
          if (ffcrim(outfptr, (infptr->Fptr)->zbitpix, (infptr->Fptr)->zndim, 
             (infptr->Fptr)->znaxis, status) > 0)
          {
             ffpmsg("error creating output decompressed image HDU");
    	     return (*status);
          }

	  norec = 1;  /* the required keywords have already been written */

       } else {
       
          if (numkeys == 0) {  /* the output file is currently completely empty */
	  
	     /* In this case, the input is a compressed IMAGE extension. */
	     /* Since the uncompressed output file is currently completely empty, */
	     /* we need to write a null primary array before uncompressing the */
             /* image extension */
	     
             ffcrim(outfptr, 8, 0, naxes, status); /* naxes is not used */
	     
	     /* now create the empty extension to uncompress into */
             if (fits_create_hdu(outfptr, status) > 0)
             {
                  ffpmsg("error creating output decompressed image HDU");
    	          return (*status);
             }
	  
	  } else {
              /* just create a new empty extension, then copy all the required */
	      /* keywords into it.  */
             fits_create_hdu(outfptr, status);
	  }
       }

    }

    if (*status > 0)  {
        ffpmsg("error creating output decompressed image HDU");
    	return (*status);
    }

    /* Copy the table header to the image header. */

    if (imcomp_copy_comp2img(infptr, outfptr, norec, status) > 0)
    {
        ffpmsg("error copying header keywords from compressed image");
    	return (*status);
    }

    /* force a rescan of the output header keywords, then reset the scaling */
    /* in case the BSCALE and BZERO keywords are present, so that the       */
    /* decompressed values won't be scaled when written to the output image */
    ffrdef(outfptr, status);
    ffpscl(outfptr, 1.0, 0.0, status);
    ffpscl(infptr, 1.0, 0.0, status);

    /* initialize; no null checking is needed for integer images */
    nullcheck = 0;
    nulladdr =  &fnulval;

    /* determine datatype for image */
    if ((infptr->Fptr)->zbitpix == BYTE_IMG)
    {
        datatype = TBYTE;
        byte_per_pix = 1;
    }
    else if ((infptr->Fptr)->zbitpix == SHORT_IMG)
    {
        datatype = TSHORT;
        byte_per_pix = sizeof(short);
    }
    else if ((infptr->Fptr)->zbitpix == LONG_IMG)
    {
        datatype = TINT;
        byte_per_pix = sizeof(int);
    }
    else if ((infptr->Fptr)->zbitpix == FLOAT_IMG)
    {
        /* In the case of float images we must check for NaNs  */
        nullcheck = 1;
        fnulval = FLOATNULLVALUE;
        nulladdr =  &fnulval;
        datatype = TFLOAT;
        byte_per_pix = sizeof(float);
    }
    else if ((infptr->Fptr)->zbitpix == DOUBLE_IMG)
    {
        /* In the case of double images we must check for NaNs  */
        nullcheck = 1;
        dnulval = DOUBLENULLVALUE;
        nulladdr = (float *) &dnulval;
        datatype = TDOUBLE;
        byte_per_pix = sizeof(double);
    }

    /* calculate size of the image (in pixels) */
    imgsize = 1;
    for (ii = 0; ii < (infptr->Fptr)->zndim; ii++)
    {
        imgsize *= (infptr->Fptr)->znaxis[ii];
        fpixel[ii] = 1;              /* Set first and last pixel to */
        lpixel[ii] = (infptr->Fptr)->znaxis[ii]; /* include the entire image. */
        inc[ii] = 1;
    }
    /* Calc equivalent number of double pixels same size as whole the image. */
    /* We use double datatype to force the memory to be aligned properly */
    memsize = ((imgsize * byte_per_pix) - 1) / sizeof(double) + 1;

    /* allocate memory for the image */
    data = (double*) calloc (memsize, sizeof(double));
    if (!data)
    { 
        ffpmsg("Couldn't allocate memory for the uncompressed image");
        return(*status = MEMORY_ALLOCATION);
    }

    /* uncompress the entire image into memory */
    /* This routine should be enhanced sometime to only need enough */
    /* memory to uncompress one tile at a time.  */
    fits_read_compressed_img(infptr, datatype, fpixel, lpixel, inc,  
            nullcheck, nulladdr, data, NULL, &anynul, status);

    /* write the image to the output file */
    if (anynul)
        fits_write_imgnull(outfptr, datatype, 1, imgsize, data, nulladdr, 
                          status);
    else
        fits_write_img(outfptr, datatype, 1, imgsize, data, status);

    free(data);
    return (*status);
}
/*--------------------------------------------------------------------------*/
int fits_decompress_img (fitsfile *infptr, /* image (bintable) to uncompress */
              fitsfile *outfptr,   /* empty HDU for output uncompressed image */
              int *status)         /* IO - error status               */

/* 
  THIS IS AN OBSOLETE ROUTINE.  USE fits_img_decompress instead!!!
  
  This routine decompresses the whole image and writes it to the output file.
*/

{
    double *data;
    int ii, datatype = 0, byte_per_pix = 0;
    int nullcheck, anynul;
    LONGLONG fpixel[MAX_COMPRESS_DIM], lpixel[MAX_COMPRESS_DIM];
    long inc[MAX_COMPRESS_DIM];
    long imgsize, memsize;
    float *nulladdr, fnulval;
    double dnulval;

    if (*status > 0)
        return(*status);

    if (!fits_is_compressed_image(infptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_decompress_img)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    /* create an empty output image with the correct dimensions */
    if (ffcrim(outfptr, (infptr->Fptr)->zbitpix, (infptr->Fptr)->zndim, 
       (infptr->Fptr)->znaxis, status) > 0)
    {
        ffpmsg("error creating output decompressed image HDU");
    	return (*status);
    }
    /* Copy the table header to the image header. */
    if (imcomp_copy_imheader(infptr, outfptr, status) > 0)
    {
        ffpmsg("error copying header of compressed image");
    	return (*status);
    }

    /* force a rescan of the output header keywords, then reset the scaling */
    /* in case the BSCALE and BZERO keywords are present, so that the       */
    /* decompressed values won't be scaled when written to the output image */
    ffrdef(outfptr, status);
    ffpscl(outfptr, 1.0, 0.0, status);
    ffpscl(infptr, 1.0, 0.0, status);

    /* initialize; no null checking is needed for integer images */
    nullcheck = 0;
    nulladdr =  &fnulval;

    /* determine datatype for image */
    if ((infptr->Fptr)->zbitpix == BYTE_IMG)
    {
        datatype = TBYTE;
        byte_per_pix = 1;
    }
    else if ((infptr->Fptr)->zbitpix == SHORT_IMG)
    {
        datatype = TSHORT;
        byte_per_pix = sizeof(short);
    }
    else if ((infptr->Fptr)->zbitpix == LONG_IMG)
    {
        datatype = TINT;
        byte_per_pix = sizeof(int);
    }
    else if ((infptr->Fptr)->zbitpix == FLOAT_IMG)
    {
        /* In the case of float images we must check for NaNs  */
        nullcheck = 1;
        fnulval = FLOATNULLVALUE;
        nulladdr =  &fnulval;
        datatype = TFLOAT;
        byte_per_pix = sizeof(float);
    }
    else if ((infptr->Fptr)->zbitpix == DOUBLE_IMG)
    {
        /* In the case of double images we must check for NaNs  */
        nullcheck = 1;
        dnulval = DOUBLENULLVALUE;
        nulladdr = (float *) &dnulval;
        datatype = TDOUBLE;
        byte_per_pix = sizeof(double);
    }

    /* calculate size of the image (in pixels) */
    imgsize = 1;
    for (ii = 0; ii < (infptr->Fptr)->zndim; ii++)
    {
        imgsize *= (infptr->Fptr)->znaxis[ii];
        fpixel[ii] = 1;              /* Set first and last pixel to */
        lpixel[ii] = (infptr->Fptr)->znaxis[ii]; /* include the entire image. */
        inc[ii] = 1;
    }
    /* Calc equivalent number of double pixels same size as whole the image. */
    /* We use double datatype to force the memory to be aligned properly */
    memsize = ((imgsize * byte_per_pix) - 1) / sizeof(double) + 1;

    /* allocate memory for the image */
    data = (double*) calloc (memsize, sizeof(double));
    if (!data)
    { 
        ffpmsg("Couldn't allocate memory for the uncompressed image");
        return(*status = MEMORY_ALLOCATION);
    }

    /* uncompress the entire image into memory */
    /* This routine should be enhanced sometime to only need enough */
    /* memory to uncompress one tile at a time.  */
    fits_read_compressed_img(infptr, datatype, fpixel, lpixel, inc,  
            nullcheck, nulladdr, data, NULL, &anynul, status);

    /* write the image to the output file */
    if (anynul)
        fits_write_imgnull(outfptr, datatype, 1, imgsize, data, nulladdr, 
                          status);
    else
        fits_write_img(outfptr, datatype, 1, imgsize, data, status);

    free(data);
    return (*status);
}
/*---------------------------------------------------------------------------*/
int fits_read_compressed_img(fitsfile *fptr,   /* I - FITS file pointer      */
            int  datatype,  /* I - datatype of the array to be returned      */
            LONGLONG  *infpixel, /* I - 'bottom left corner' of the subsection    */
            LONGLONG  *inlpixel, /* I - 'top right corner' of the subsection      */
            long  *ininc,    /* I - increment to be applied in each dimension */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
   Read a section of a compressed image;  Note: lpixel may be larger than the 
   size of the uncompressed image.  Only the pixels within the image will be
   returned.
*/
{
    int naxis[MAX_COMPRESS_DIM], tiledim[MAX_COMPRESS_DIM];
    long tilesize[MAX_COMPRESS_DIM], thistilesize[MAX_COMPRESS_DIM];
    long ftile[MAX_COMPRESS_DIM], ltile[MAX_COMPRESS_DIM];
    long tfpixel[MAX_COMPRESS_DIM], tlpixel[MAX_COMPRESS_DIM];
    long rowdim[MAX_COMPRESS_DIM], offset[MAX_COMPRESS_DIM],ntemp;
    long fpixel[MAX_COMPRESS_DIM], lpixel[MAX_COMPRESS_DIM];
    long inc[MAX_COMPRESS_DIM];
    int ii, i5, i4, i3, i2, i1, i0, ndim, irow, pixlen, tilenul;
    void *buffer;
    char *bnullarray = 0;

    if (*status > 0) 
        return(*status);

    if (!fits_is_compressed_image(fptr, status) )
    {
        ffpmsg("CHDU is not a compressed image (fits_read_compressed_img)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    /* get temporary space for uncompressing one image tile */
    if (datatype == TSHORT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (short));
       pixlen = sizeof(short);
    }
    else if (datatype == TINT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (int));
       pixlen = sizeof(int);
    }
    else if (datatype == TLONG)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (long));
       pixlen = sizeof(long);
    }
    else if (datatype == TFLOAT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (float));
       pixlen = sizeof(float);
    }
    else if (datatype == TDOUBLE)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (double));
       pixlen = sizeof(double);
    }
    else if (datatype == TUSHORT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned short));
       pixlen = sizeof(short);
    }
    else if (datatype == TUINT)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned int));
       pixlen = sizeof(int);
    }
    else if (datatype == TULONG)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (unsigned long));
       pixlen = sizeof(long);
    }
    else if (datatype == TBYTE || datatype == TSBYTE)
    {
       buffer =  calloc ((fptr->Fptr)->maxtilelen, sizeof (char));
       pixlen = 1;
    }
    else
    {
        ffpmsg("unsupported datatype for uncompressing image");
        return(*status = BAD_DATATYPE);
    }

    if (buffer == NULL)
    {
	    ffpmsg("Out of memory (fits_read_compress_img)");
	    return (*status = MEMORY_ALLOCATION);
    }
    /* allocate memory for a null flag array, if needed */
    if (nullcheck == 2)
    {
        bnullarray = calloc ((fptr->Fptr)->maxtilelen, sizeof (char));

        if (bnullarray == NULL)
        {
	    ffpmsg("Out of memory (fits_read_compress_img)");
            free(buffer);
	    return (*status = MEMORY_ALLOCATION);
        }
    }

    /* initialize all the arrays */
    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxis[ii] = 1;
        tiledim[ii] = 1;
        tilesize[ii] = 1;
        ftile[ii] = 1;
        ltile[ii] = 1;
        rowdim[ii] = 1;
    }

    ndim = (fptr->Fptr)->zndim;
    ntemp = 1;
    for (ii = 0; ii < ndim; ii++)
    {
        /* support for mirror-reversed image sections */
        if (infpixel[ii] <= inlpixel[ii])
        {
           fpixel[ii] = (long) infpixel[ii];
           lpixel[ii] = (long) inlpixel[ii];
           inc[ii]    = ininc[ii];
        }
        else
        {
           fpixel[ii] = (long) inlpixel[ii];
           lpixel[ii] = (long) infpixel[ii];
           inc[ii]    = -ininc[ii];
        }

        /* calc number of tiles in each dimension, and tile containing */
        /* the first and last pixel we want to read in each dimension  */
        naxis[ii] = (fptr->Fptr)->znaxis[ii];
        if (fpixel[ii] < 1)
        {
            if (nullcheck == 2)
            {
                free(bnullarray);
            }
            free(buffer);
            return(*status = BAD_PIX_NUM);
        }

        tilesize[ii] = (fptr->Fptr)->tilesize[ii];
        tiledim[ii] = (naxis[ii] - 1) / tilesize[ii] + 1;
        ftile[ii]   = (fpixel[ii] - 1)   / tilesize[ii] + 1;
        ltile[ii]   = minvalue((lpixel[ii] - 1) / tilesize[ii] + 1, 
                                tiledim[ii]);
        rowdim[ii]  = ntemp;  /* total tiles in each dimension */
        ntemp *= tiledim[ii];
    }

    if (anynul)
       *anynul = 0;  /* initialize */

    /* support up to 6 dimensions for now */
    /* tfpixel and tlpixel are the first and last image pixels */
    /* along each dimension of the compression tile */
    for (i5 = ftile[5]; i5 <= ltile[5]; i5++)
    {
     tfpixel[5] = (i5 - 1) * tilesize[5] + 1;
     tlpixel[5] = minvalue(tfpixel[5] + tilesize[5] - 1, 
                            naxis[5]);
     thistilesize[5] = tlpixel[5] - tfpixel[5] + 1;
     offset[5] = (i5 - 1) * rowdim[5];
     for (i4 = ftile[4]; i4 <= ltile[4]; i4++)
     {
      tfpixel[4] = (i4 - 1) * tilesize[4] + 1;
      tlpixel[4] = minvalue(tfpixel[4] + tilesize[4] - 1, 
                            naxis[4]);
      thistilesize[4] = thistilesize[5] * (tlpixel[4] - tfpixel[4] + 1);
      offset[4] = (i4 - 1) * rowdim[4] + offset[5];
      for (i3 = ftile[3]; i3 <= ltile[3]; i3++)
      {
        tfpixel[3] = (i3 - 1) * tilesize[3] + 1;
        tlpixel[3] = minvalue(tfpixel[3] + tilesize[3] - 1, 
                              naxis[3]);
        thistilesize[3] = thistilesize[4] * (tlpixel[3] - tfpixel[3] + 1);
        offset[3] = (i3 - 1) * rowdim[3] + offset[4];
        for (i2 = ftile[2]; i2 <= ltile[2]; i2++)
        {
          tfpixel[2] = (i2 - 1) * tilesize[2] + 1;
          tlpixel[2] = minvalue(tfpixel[2] + tilesize[2] - 1, 
                                naxis[2]);
          thistilesize[2] = thistilesize[3] * (tlpixel[2] - tfpixel[2] + 1);
          offset[2] = (i2 - 1) * rowdim[2] + offset[3];
          for (i1 = ftile[1]; i1 <= ltile[1]; i1++)
          {
            tfpixel[1] = (i1 - 1) * tilesize[1] + 1;
            tlpixel[1] = minvalue(tfpixel[1] + tilesize[1] - 1, 
                                  naxis[1]);
            thistilesize[1] = thistilesize[2] * (tlpixel[1] - tfpixel[1] + 1);
            offset[1] = (i1 - 1) * rowdim[1] + offset[2];
            for (i0 = ftile[0]; i0 <= ltile[0]; i0++)
            {
              tfpixel[0] = (i0 - 1) * tilesize[0] + 1;
              tlpixel[0] = minvalue(tfpixel[0] + tilesize[0] - 1, 
                                    naxis[0]);
              thistilesize[0] = thistilesize[1] * (tlpixel[0] - tfpixel[0] + 1);
              /* calculate row of table containing this tile */
              irow = i0 + offset[1];

/*
printf("row %d, %d %d, %d %d, %d %d; %d\n",
              irow, tfpixel[0],tlpixel[0],tfpixel[1],tlpixel[1],tfpixel[2],tlpixel[2],
	      thistilesize[0]);
*/      
              /* read and uncompress this row (tile) of the table */
              /* also do type conversion and undefined pixel substitution */
              /* at this point */
              imcomp_decompress_tile(fptr, irow, thistilesize[0],
                    datatype, nullcheck, nullval, buffer, bnullarray, &tilenul,
                     status);

              if (tilenul && anynul)
                  *anynul = 1;  /* there are null pixels */
/*
printf(" pixlen=%d, ndim=%d, %d %d %d, %d %d %d, %d %d %d\n",
     pixlen, ndim, fpixel[0],lpixel[0],inc[0],fpixel[1],lpixel[1],inc[1],
     fpixel[2],lpixel[2],inc[2]);
*/
              /* copy the intersecting pixels from this tile to the output */
              imcomp_copy_overlap(buffer, pixlen, ndim, tfpixel, tlpixel, 
                     bnullarray, array, fpixel, lpixel, inc, nullcheck, 
                     nullarray, status);
            }
          }
        }
      }
     }
    }
    if (nullcheck == 2)
    {
        free(bnullarray);
    }
    free(buffer);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_read_compressed_pixels(fitsfile *fptr, /* I - FITS file pointer    */
            int  datatype,  /* I - datatype of the array to be returned     */
            LONGLONG   fpixel, /* I - 'first pixel to read          */
            LONGLONG   npixel,  /* I - number of pixels to read      */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            int  *status)     /* IO - error status                           */
/*
   Read a consecutive set of pixels from a compressed image.  This routine
   interpretes the n-dimensional image as a long one-dimensional array. 
   This is actually a rather inconvenient way to read compressed images in
   general, and could be rather inefficient if the requested pixels to be
   read are located in many different image compression tiles.    

   The general strategy used here is to read the requested pixels in blocks
   that correspond to rectangular image sections.  
*/
{
    int naxis, ii, bytesperpixel, planenul;
    long naxes[MAX_COMPRESS_DIM], nread;
    long nplane, inc[MAX_COMPRESS_DIM];
    LONGLONG tfirst, tlast, last0, last1, dimsize[MAX_COMPRESS_DIM];
    LONGLONG firstcoord[MAX_COMPRESS_DIM], lastcoord[MAX_COMPRESS_DIM];
    char *arrayptr, *nullarrayptr;

    if (*status > 0)
        return(*status);

    arrayptr = (char *) array;
    nullarrayptr = nullarray;

    /* get size of array pixels, in bytes */
    bytesperpixel = ffpxsz(datatype);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        naxes[ii] = 1;
        firstcoord[ii] = 0;
        lastcoord[ii] = 0;
        inc[ii] = 1;
    }

    /*  determine the dimensions of the image to be read */
    ffgidm(fptr, &naxis, status);
    ffgisz(fptr, MAX_COMPRESS_DIM, naxes, status);

    /* calc the cumulative number of pixels in each successive dimension */
    dimsize[0] = 1;
    for (ii = 1; ii < MAX_COMPRESS_DIM; ii++)
         dimsize[ii] = dimsize[ii - 1] * naxes[ii - 1];

    /*  determine the coordinate of the first and last pixel in the image */
    /*  Use zero based indexes here */
    tfirst = fpixel - 1;
    tlast = tfirst + npixel - 1;
    for (ii = naxis - 1; ii >= 0; ii--)
    {
        firstcoord[ii] = tfirst / dimsize[ii];
        lastcoord[ii] =  tlast / dimsize[ii];
        tfirst = tfirst - firstcoord[ii] * dimsize[ii];
        tlast = tlast - lastcoord[ii] * dimsize[ii];
    }

    /* to simplify things, treat 1-D, 2-D, and 3-D images as separate cases */

    if (naxis == 1)
    {
        /* Simple: just read the requested range of pixels */

        firstcoord[0] = firstcoord[0] + 1;
        lastcoord[0] = lastcoord[0] + 1;
        fits_read_compressed_img(fptr, datatype, firstcoord, lastcoord, inc,
            nullcheck, nullval, array, nullarray, anynul, status);
        return(*status);
    }
    else if (naxis == 2)
    {
        nplane = 0;  /* read 1st (and only) plane of the image */

        fits_read_compressed_img_plane(fptr, datatype, bytesperpixel,
          nplane, firstcoord, lastcoord, inc, naxes, nullcheck, nullval,
          array, nullarray, anynul, &nread, status);
    }
    else if (naxis == 3)
    {
        /* test for special case: reading an integral number of planes */
        if (firstcoord[0] == 0 && firstcoord[1] == 0 &&
            lastcoord[0] == naxes[0] - 1 && lastcoord[1] == naxes[1] - 1)
        {
            for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
            {
                /* convert from zero base to 1 base */
                (firstcoord[ii])++;
                (lastcoord[ii])++;
            }

            /* we can read the contiguous block of pixels in one go */
            fits_read_compressed_img(fptr, datatype, firstcoord, lastcoord, inc,
                nullcheck, nullval, array, nullarray, anynul, status);

            return(*status);
        }

        if (anynul)
            *anynul = 0;  /* initialize */

        /* save last coordinate in temporary variables */
        last0 = lastcoord[0];
        last1 = lastcoord[1];

        if (firstcoord[2] < lastcoord[2])
        {
            /* we will read up to the last pixel in all but the last plane */
            lastcoord[0] = naxes[0] - 1;
            lastcoord[1] = naxes[1] - 1;
        }

        /* read one plane of the cube at a time, for simplicity */
        for (nplane = (long) firstcoord[2]; nplane <= lastcoord[2]; nplane++)
        {
            if (nplane == lastcoord[2])
            {
                lastcoord[0] = last0;
                lastcoord[1] = last1;
            }

            fits_read_compressed_img_plane(fptr, datatype, bytesperpixel,
              nplane, firstcoord, lastcoord, inc, naxes, nullcheck, nullval,
              arrayptr, nullarrayptr, &planenul, &nread, status);

            if (planenul && anynul)
               *anynul = 1;  /* there are null pixels */

            /* for all subsequent planes, we start with the first pixel */
            firstcoord[0] = 0;
            firstcoord[1] = 0;

            /* increment pointers to next elements to be read */
            arrayptr = arrayptr + nread * bytesperpixel;
            if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + nread;
        }
    }
    else
    {
        ffpmsg("only 1D, 2D, or 3D images are currently supported");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    return(*status);
}
/*--------------------------------------------------------------------------*/
int fits_read_compressed_img_plane(fitsfile *fptr, /* I - FITS file   */
            int  datatype,  /* I - datatype of the array to be returned      */
            int  bytesperpixel, /* I - number of bytes per pixel in array */
            long   nplane,  /* I - which plane of the cube to read      */
            LONGLONG *firstcoord,  /* coordinate of first pixel to read */
            LONGLONG *lastcoord,   /* coordinate of last pixel to read */
            long *inc,         /* increment of pixels to read */
            long *naxes,      /* size of each image dimension */
            int  nullcheck,  /* I - 0 for no null checking                   */
                              /*     1: set undefined pixels = nullval       */
                              /*     2: set nullarray=1 for undefined pixels */
            void *nullval,    /* I - value for undefined pixels              */
            void *array,      /* O - array of values that are returned       */
            char *nullarray,  /* O - array of flags = 1 if nullcheck = 2     */
            int  *anynul,     /* O - set to 1 if any values are null; else 0 */
            long *nread,      /* O - total number of pixels read and returned*/
            int  *status)     /* IO - error status                           */

   /*
           in general we have to read the first partial row of the image,
           followed by the middle complete rows, followed by the last
           partial row of the image.  If the first or last rows are complete,
           then read them at the same time as all the middle rows.
    */
{
     /* bottom left coord. and top right coord. */
    LONGLONG blc[MAX_COMPRESS_DIM], trc[MAX_COMPRESS_DIM]; 
    char *arrayptr, *nullarrayptr;
    int tnull;

    if (anynul)
        *anynul = 0;

    *nread = 0;

    arrayptr = (char *) array;
    nullarrayptr = nullarray;

    blc[2] = nplane + 1;
    trc[2] = nplane + 1;

    if (firstcoord[0] != 0)
    { 
            /* have to read a partial first row */
            blc[0] = firstcoord[0] + 1;
            blc[1] = firstcoord[1] + 1;
            trc[1] = blc[1];  
            if (lastcoord[1] == firstcoord[1])
               trc[0] = lastcoord[0] + 1; /* 1st and last pixels in same row */
            else
               trc[0] = naxes[0];  /* read entire rest of the row */

            fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

            *nread = *nread + (long) (trc[0] - blc[0] + 1);

            if (tnull && anynul)
               *anynul = 1;  /* there are null pixels */

            if (lastcoord[1] == firstcoord[1])
            {
               return(*status);  /* finished */
            }

            /* set starting coord to beginning of next line */
            firstcoord[0] = 0;
            firstcoord[1] += 1;
            arrayptr = arrayptr + (trc[0] - blc[0] + 1) * bytesperpixel;
            if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + (trc[0] - blc[0] + 1);

    }

    /* read contiguous complete rows of the image, if any */
    blc[0] = 1;
    blc[1] = firstcoord[1] + 1;
    trc[0] = naxes[0];

    if (lastcoord[0] + 1 == naxes[0])
    {
            /* can read the last complete row, too */
            trc[1] = lastcoord[1] + 1;
    }
    else
    {
            /* last row is incomplete; have to read it separately */
            trc[1] = lastcoord[1];
    }

    if (trc[1] >= blc[1])  /* must have at least one whole line to read */
    {
        fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

        *nread = *nread + (long) ((trc[1] - blc[1] + 1) * naxes[0]);

        if (tnull && anynul)
           *anynul = 1;

        if (lastcoord[1] + 1 == trc[1])
               return(*status);  /* finished */

        /* increment pointers for the last partial row */
        arrayptr = arrayptr + (trc[1] - blc[1] + 1) * naxes[0] * bytesperpixel;
        if (nullarrayptr && (nullcheck == 2) )
                nullarrayptr = nullarrayptr + (trc[1] - blc[1] + 1) * naxes[0];
     }

    if (trc[1] == lastcoord[1] + 1)
        return(*status);           /* all done */

    /* set starting and ending coord to last line */

    trc[0] = lastcoord[0] + 1;
    trc[1] = lastcoord[1] + 1;
    blc[1] = trc[1];

    fits_read_compressed_img(fptr, datatype, blc, trc, inc,
                nullcheck, nullval, arrayptr, nullarrayptr, &tnull, status);

    if (tnull && anynul)
       *anynul = 1;

    *nread = *nread + (long) (trc[0] - blc[0] + 1);

    return(*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_get_compressed_image_par(fitsfile *infptr, int *status)
 
/* 
    This routine reads keywords from a BINTABLE extension containing a
    compressed image.
*/
{
    char keyword[FLEN_KEYWORD];
    char value[FLEN_VALUE];
    int ii, tstatus;
    long expect_nrows, maxtilelen;

    if (*status > 0)
        return(*status);

    /* Copy relevant header keyword values to structure */
    if (ffgky (infptr, TSTRING, "ZCMPTYPE", value, NULL, status) > 0)
    {
        ffpmsg("required ZCMPTYPE compression keyword not found in");
        ffpmsg(" imcomp_get_compressed_image_par");
        return(*status);
    }

    (infptr->Fptr)->zcmptype[0] = '\0';
    strncat((infptr->Fptr)->zcmptype, value, 11);

    if (!FSTRCMP(value, "RICE_1") )
        (infptr->Fptr)->compress_type = RICE_1;
    else if (!FSTRCMP(value, "HCOMPRESS_1") )
        (infptr->Fptr)->compress_type = HCOMPRESS_1;
    else if (!FSTRCMP(value, "GZIP_1") )
        (infptr->Fptr)->compress_type = GZIP_1;
    else if (!FSTRCMP(value, "PLIO_1") )
        (infptr->Fptr)->compress_type = PLIO_1;
    else
    {
        ffpmsg("Unknown image compression type:");
        ffpmsg(value);
	return (*status = DATA_DECOMPRESSION_ERR);
    }

    if (ffgky (infptr, TINT,  "ZBITPIX",  &(infptr->Fptr)->zbitpix,  
               NULL, status) > 0)
    {
        ffpmsg("required ZBITPIX compression keyword not found");
        return(*status);
    }

    if (ffgky (infptr,TINT, "ZNAXIS", &(infptr->Fptr)->zndim, NULL, status) > 0)
    {
        ffpmsg("required ZNAXIS compression keyword not found");
        return(*status);
    }

    if ((infptr->Fptr)->zndim < 1)
    {
        ffpmsg("Compressed image has no data (ZNAXIS < 1)");
	return (*status = BAD_NAXIS);
    }

    if ((infptr->Fptr)->zndim > MAX_COMPRESS_DIM)
    {
        ffpmsg("Compressed image has too many dimensions");
        return(*status = BAD_NAXIS);
    }

    expect_nrows = 1;
    maxtilelen = 1;
    for (ii = 0;  ii < (infptr->Fptr)->zndim;  ii++)
    {
        /* get image size */
        sprintf (keyword, "ZNAXIS%d", ii+1);
	ffgky (infptr, TLONG,keyword, &(infptr->Fptr)->znaxis[ii],NULL,status);

        if (*status > 0)
        {
            ffpmsg("required ZNAXISn compression keyword not found");
            return(*status);
        }

        /* get compression tile size */
	sprintf (keyword, "ZTILE%d", ii+1);

        /* set default tile size in case keywords are not present */
        if (ii == 0)
            (infptr->Fptr)->tilesize[0] = (infptr->Fptr)->znaxis[0];
        else
            (infptr->Fptr)->tilesize[ii] = 1;

        tstatus = 0;
	ffgky (infptr, TLONG, keyword, &(infptr->Fptr)->tilesize[ii], NULL, 
               &tstatus);

        expect_nrows *= (((infptr->Fptr)->znaxis[ii] - 1) / 
                  (infptr->Fptr)->tilesize[ii]+ 1);
        maxtilelen *= (infptr->Fptr)->tilesize[ii];
    }

    /* check number of rows */
    if (expect_nrows != (infptr->Fptr)->numrows)
    {
        ffpmsg(
        "number of table rows != the number of tiles in compressed image");
        return (*status = DATA_DECOMPRESSION_ERR);
    }

    /* read any algorithm specific parameters */
    if ((infptr->Fptr)->compress_type == RICE_1 )
    {
        if (ffgky(infptr, TINT,"ZVAL1", &(infptr->Fptr)->rice_blocksize,
                  NULL, status) > 0)
        {
            ffpmsg("required ZVAL1 compression keyword not found");
            return(*status);
        }

        if ((infptr->Fptr)->zbitpix < 0)
        {
           /* try to read the floating point quantization parameter */
            tstatus = 0;
            ffgky(infptr, TINT,"ZVAL2", &(infptr->Fptr)->noise_nbits,
                  NULL, &tstatus);
        }
    }
    else if ((infptr->Fptr)->compress_type == HCOMPRESS_1 )
    {
        if (ffgky(infptr, TINT,"ZVAL1", &(infptr->Fptr)->hcomp_scale,
                  NULL, status) > 0)
        {
            ffpmsg("required ZVAL1 compression keyword not found");
            return(*status);
        }

        tstatus = 0;
        ffgky(infptr, TINT,"ZVAL2", &(infptr->Fptr)->hcomp_smooth,
                  NULL, &tstatus);

        if ((infptr->Fptr)->zbitpix < 0)
        {
           /* try to read the floating point quantization parameter */
            tstatus = 0;
            ffgky(infptr, TINT,"ZVAL3", &(infptr->Fptr)->noise_nbits,
                  NULL, &tstatus);
        }
    }    
    else
    {
        if ((infptr->Fptr)->zbitpix < 0)
        {
           /* try to read the floating point quantization parameter */
            tstatus = 0;
            ffgky(infptr, TINT,"ZVAL1", &(infptr->Fptr)->noise_nbits,
                  NULL, &tstatus);
        }
    }

    /* store number of pixels in each compression tile, */
    /* and max size of the compressed tile buffer */
    (infptr->Fptr)->maxtilelen = maxtilelen;

    (infptr->Fptr)->maxelem = 
           imcomp_calc_max_elem ((infptr->Fptr)->compress_type, maxtilelen, 
               (infptr->Fptr)->zbitpix, (infptr->Fptr)->rice_blocksize);

    /* Get Column numbers. */
    if (ffgcno(infptr, CASEINSEN, "COMPRESSED_DATA",
         &(infptr->Fptr)->cn_compressed, status) > 0)
    {
        ffpmsg("couldn't find COMPRESSED_DATA column (fits_get_compressed_img_par)");
        return(*status = DATA_DECOMPRESSION_ERR);
    }

    ffpmrk(); /* put mark on message stack; erase any messages after this */

    tstatus = 0;
    ffgcno(infptr,CASEINSEN, "UNCOMPRESSED_DATA",
          &(infptr->Fptr)->cn_uncompressed, &tstatus);

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZSCALE", &(infptr->Fptr)->cn_zscale,
              &tstatus) > 0)
    {
        /* CMPSCALE column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TDOUBLE, "ZSCALE", &(infptr->Fptr)->zscale, NULL, 
                 &tstatus) <= 0)
            (infptr->Fptr)->cn_zscale = -1;  /* flag for a constant ZSCALE */
    }

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZZERO", &(infptr->Fptr)->cn_zzero,
               &tstatus) > 0)
    {
        /* CMPZERO column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TDOUBLE, "ZZERO", &(infptr->Fptr)->zzero, NULL, 
                  &tstatus) <= 0)
            (infptr->Fptr)->cn_zzero = -1;  /* flag for a constant ZZERO */
    }

    tstatus = 0;
    if (ffgcno(infptr, CASEINSEN, "ZBLANK", &(infptr->Fptr)->cn_zblank,
               &tstatus) > 0)
    {
        /* ZBLANK column doesn't exist; see if there is a keyword */
        tstatus = 0;
        if (ffgky(infptr, TINT, "ZBLANK", &(infptr->Fptr)->zblank, NULL,
                  &tstatus) <= 0)  {
            (infptr->Fptr)->cn_zblank = -1;  /* flag for a constant ZBLANK */

        } else {
           /* ZBLANK keyword doesn't exist; see if there is a BLANK keyword */
           tstatus = 0;
           if (ffgky(infptr, TINT, "BLANK", &(infptr->Fptr)->zblank, NULL,
                  &tstatus) <= 0)  
              (infptr->Fptr)->cn_zblank = -1;  /* flag for a constant ZBLANK */
        }
    }

    /* read the conventional BSCALE and BZERO scaling keywords, if present */
    tstatus = 0;
    if (ffgky (infptr, TDOUBLE, "BSCALE", &(infptr->Fptr)->cn_bscale, 
        NULL, &tstatus) > 0)
    {
        (infptr->Fptr)->cn_bscale = 1.0;
    }

    tstatus = 0;
    if (ffgky (infptr, TDOUBLE, "BZERO", &(infptr->Fptr)->cn_bzero, 
        NULL, &tstatus) > 0)
    {
        (infptr->Fptr)->cn_bzero = 0.0;
    }

    ffcmrk();  /* clear any spurious error messages, back to the mark */
    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_imheader(fitsfile *infptr, fitsfile *outfptr, int *status)
/*
    This routine reads the header keywords from the input image and
    copies them to the output image;  the manditory structural keywords
    and the checksum keywords are not copied. If the DATE keyword is copied,
    then it is updated with the current date and time.
*/
{
    int nkeys, ii, keyclass;
    char card[FLEN_CARD];	/* a header record */

    if (*status > 0)
        return(*status);

    ffghsp(infptr, &nkeys, NULL, status); /* get number of keywords in image */

    for (ii = 5; ii <= nkeys; ii++)  /* skip the first 4 keywords */
    {
        ffgrec(infptr, ii, card, status);

	keyclass = ffgkcl(card);  /* Get the type/class of keyword */

        /* don't copy structural keywords or checksum keywords */
        if ((keyclass <= TYP_CMPRS_KEY) || (keyclass == TYP_CKSUM_KEY))
	    continue;

        if (FSTRNCMP(card, "DATE ", 5) == 0) /* write current date */
        {
            ffpdat(outfptr, status);
        }
        else if (FSTRNCMP(card, "EXTNAME ", 8) == 0) 
        {
            /* don't copy default EXTNAME keyword from a compressed image */
            if (FSTRNCMP(card, "EXTNAME = 'COMPRESSED_IMAGE'", 28))
            {
                /* if EXTNAME keyword already exists, overwrite it */
                /* otherwise append a new EXTNAME keyword */
                ffucrd(outfptr, "EXTNAME", card, status);
            }
        }
        else
        {
            /* just copy the keyword to the output header */
	    ffprec (outfptr, card, status);
        }

        if (*status > 0)
           return (*status);
    }
    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_img2comp(fitsfile *infptr, fitsfile *outfptr, int *status)
/*
    This routine copies the header keywords from the uncompressed input image 
    and to the compressed image (in a binary table) 
*/
{
    char card[FLEN_CARD];	/* a header record */

    /* tile compressed image keyword translation table  */
    /*                        INPUT      OUTPUT  */
    /*                       01234567   01234567 */
    char *patterns[][2] = {{"SIMPLE",  "ZSIMPLE" },  
			   {"XTENSION", "ZTENSION" },
			   {"BITPIX",  "ZBITPIX" },
			   {"NAXIS",   "ZNAXIS"  },
			   {"NAXISm",  "ZNAXISm" },
			   {"EXTEND",  "ZEXTEND" },
			   {"BLOCKED", "ZBLOCKED"},
			   {"PCOUNT",  "ZPCOUNT" },  
			   {"GCOUNT",  "ZGCOUNT" },

			   {"CHECKSUM","ZHECKSUM"},  /* save original checksums */
			   {"DATASUM", "ZDATASUM"},
			   
			   {"*",       "+"       }}; /* copy all other keywords */
    int npat;

    if (*status > 0)
        return(*status);

    /* write a default EXTNAME keyword if it doesn't exist in input file*/
    fits_read_card(infptr, "EXTNAME", card, status);
    
    if (*status) {
       *status = 0;
       strcpy(card, "EXTNAME = 'COMPRESSED_IMAGE'");
       fits_write_record(outfptr, card, status);
    }

    /* copy all the keywords from the input file to the output */
    npat = sizeof(patterns)/sizeof(patterns[0][0])/2;
    fits_translate_keywords(infptr, outfptr, 1, patterns, npat,
			    0, 0, 0, status);

    if (*status > 0)
       return (*status);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_comp2img(fitsfile *infptr, fitsfile *outfptr, 
                          int norec, int *status)
/*
    This routine copies the header keywords from the compressed input image 
    and to the uncompressed image (in a binary table) 
*/
{
    char card[FLEN_CARD];	/* a header record */
    char *patterns[40][2];
    char negative[] = "-";
    int ii, npat, nreq, nsp;
    
    /* tile compressed image keyword translation table  */
    /*                        INPUT      OUTPUT  */
    /*                       01234567   01234567 */

    /*  only translate these if required keywords not already written */
    char *reqkeys[][2] = {  
			   {"ZSIMPLE",   "SIMPLE" },  
			   {"ZTENSION", "XTENSION"},
			   {"ZBITPIX",   "BITPIX" },
			   {"ZNAXIS",    "NAXIS"  },
			   {"ZNAXISm",   "NAXISm" },
			   {"ZEXTEND",   "EXTEND" },
			   {"ZBLOCKED",  "BLOCKED"},
			   {"ZPCOUNT",   "PCOUNT" },  
			   {"ZGCOUNT",   "GCOUNT" },
			   {"ZHECKSUM",  "CHECKSUM"},  /* restore original checksums */
			   {"ZDATASUM",  "DATASUM"}}; 

    /* other special keywords */
    char *spkeys[][2] = {
			   {"XTENSION", "-"      },
			   {"BITPIX",  "-"       },
			   {"NAXIS",   "-"       },
			   {"NAXISm",  "-"       },
			   {"PCOUNT",  "-"       },
			   {"GCOUNT",  "-"       },
			   {"TFIELDS", "-"       },
			   {"TTYPEm",  "-"       },
			   {"TFORMm",  "-"       },
			   {"ZIMAGE",  "-"       },
			   {"ZTILEm",  "-"       },
			   {"ZCMPTYPE", "-"      },
			   {"ZNAMEm",  "-"       },
			   {"ZVALm",   "-"       },

			   {"CHECKSUM","-"       },  /* delete checksums */
			   {"DATASUM", "-"       },
			   {"EXTNAME", "+"       },  /* we may change this, below */
			   {"*",       "+"      }};  


    if (*status > 0)
        return(*status);
	
    nreq = sizeof(reqkeys)/sizeof(reqkeys[0][0])/2;
    nsp = sizeof(spkeys)/sizeof(spkeys[0][0])/2;

    /* construct translation patterns */

    for (ii = 0; ii < nreq; ii++) {
        patterns[ii][0] = reqkeys[ii][0];
	
        if (norec) 
            patterns[ii][1] = negative;
        else
            patterns[ii][1] = reqkeys[ii][1];
    }
    
    for (ii = 0; ii < nsp; ii++) {
        patterns[ii+nreq][0] = spkeys[ii][0];
        patterns[ii+nreq][1] = spkeys[ii][1];
    }

    npat = nreq + nsp;
    
    /* see if the EXTNAME keyword should be copied or not */
    fits_read_card(infptr, "EXTNAME", card, status);

    if (!strncmp(card, "EXTNAME = 'COMPRESSED_IMAGE'", 28)) 
        patterns[npat-2][1] = negative;
    
    /* translate and copy the keywords from the input file to the output */
    fits_translate_keywords(infptr, outfptr, 1, patterns, npat,
			    0, 0, 0, status);

    if (*status > 0)
       return (*status);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_decompress_tile (fitsfile *infptr,
          int nrow,            /* I - row of table to read and uncompress */
          int tilelen,         /* I - number of pixels in the tile        */
          int datatype,        /* I - datatype to be returned in 'buffer' */
          int nullcheck,       /* I - 0 for no null checking */
          void *nulval,        /* I - value to be used for undefined pixels */
          void *buffer,        /* O - buffer for returned decompressed values */
          char *bnullarray,    /* O - buffer for returned null flags */
          int *anynul,         /* O - any null values returned?  */
          int *status)

/* This routine decompresses one tile of the image */
{
    int *idata = 0;          /* uncompressed integer data */
    LONGLONG *lldata;
    size_t idatalen, tilebytesize;
    int ii, tnull;        /* value in the data which represents nulls */
    unsigned char *cbuf = 0; /* compressed data */
    unsigned char charnull = 0;
    short *sbuf = 0;
    short snull = 0;
    int blocksize;
    double bscale, bzero, dummy = 0;    /* scaling parameters */
    long nelem = 0, offset = 0;      /* number of bytes */
    int smooth, nx, ny, scale;  /* hcompress parameters */

    if (*status > 0)
       return(*status);

    /* get length of the compressed byte stream */
    ffgdes (infptr, (infptr->Fptr)->cn_compressed, nrow, &nelem, &offset, 
            status);

    /* EOF error here indicates that this tile has not yet been written */
    if (*status == END_OF_FILE)
       return(*status = NO_COMPRESSED_TILE);

    /* **************************************************************** */
    if (nelem == 0)  /* tile was not compressed; read uncompressed data */
    {
        if ((infptr->Fptr)->cn_uncompressed < 1 )
        {
	    return (*status = NO_COMPRESSED_TILE);
        }

        /* no compressed data, so simply read the uncompressed data */
        /* directly from the UNCOMPRESSED_DATA column, then return */   
        ffgdes (infptr, (infptr->Fptr)->cn_uncompressed, nrow, &nelem,
               &offset, status);

        if (nelem == 0 && offset == 0)
	    return (*status = NO_COMPRESSED_TILE);

        if (nullcheck <= 1)  
            fits_read_col(infptr, datatype, (infptr->Fptr)->cn_uncompressed,
               nrow, 1, nelem, nulval, buffer, anynul, status);
        else
            fits_read_colnull(infptr, datatype, (infptr->Fptr)->cn_uncompressed,
               nrow, 1, nelem, buffer, bnullarray, anynul, status);

        return(*status);
    }
    
    /* **************************************************************** */

    if (nullcheck == 2)
    {
        for (ii = 0; ii < tilelen; ii++)  /* initialize the null array */
            bnullarray[ii] = 0;
    }

    if (anynul)
       *anynul = 0;
    
    /* get linear scaling and offset values, if they exist */
    if ((infptr->Fptr)->cn_zscale == 0)
    {
         /* set default scaling, if scaling is not defined */
         bscale = 1.;
         bzero = 0.;
    }
    else if ((infptr->Fptr)->cn_zscale == -1)
    {
        bscale = (infptr->Fptr)->zscale;
        bzero  = (infptr->Fptr)->zzero;
    }
    else
    {
        /* read the linear scale and offset values for this row */
	ffgcvd (infptr, (infptr->Fptr)->cn_zscale, nrow, 1, 1, 0.,
				&bscale, NULL, status);
	ffgcvd (infptr, (infptr->Fptr)->cn_zzero, nrow, 1, 1, 0.,
				&bzero, NULL, status);
        if (*status > 0)
        {
          ffpmsg("error reading scaling factor and offset for compressed tile");
          free(idata);
          free (cbuf);
          return (*status);
        }
    }

    if (bscale == 1.0 && bzero == 0.0 ) 
    {
      /* if no other scaling has been specified, try using the values
         given by the BSCALE and BZERO keywords, if any */

        bscale = (infptr->Fptr)->cn_bscale;
        bzero  = (infptr->Fptr)->cn_bzero;
    }

    /* ************************************************************* */
    /* get the value used to represent nulls in the int array */
    if ((infptr->Fptr)->cn_zblank == 0)
    {
        nullcheck = 0;  /* no null value; don't check for nulls */
    }
    else if ((infptr->Fptr)->cn_zblank == -1)
    {
        tnull = (infptr->Fptr)->zblank;  /* use the the ZBLANK keyword */
    }
    else
    {
        /* read the null value for this row */
	ffgcvk (infptr, (infptr->Fptr)->cn_zblank, nrow, 1, 1, 0,
				&tnull, NULL, status);
        if (*status > 0)
        {
            ffpmsg("error reading null value for compressed tile");
            free(idata);
	    free (cbuf);
            return (*status);
        }
    }

    /* ************************************************************* */

    /* allocate memory for uncompressed integers */
    
    if ((infptr->Fptr)->compress_type == HCOMPRESS_1 &&
          ((infptr->Fptr)->zbitpix != BYTE_IMG &&
	   (infptr->Fptr)->zbitpix != SHORT_IMG) ) {

        /*  must allocate 8 bytes per pixel of scratch space */
        lldata = (LONGLONG*) calloc (tilelen, sizeof (LONGLONG));
	idata = (int *) lldata;

    } else {

        idata = (int*) calloc (tilelen, sizeof (int));
    }

    if (idata == NULL)
    {
	    ffpmsg("Out of memory for idata. (imcomp_decompress_tile)");
	    return (*status = MEMORY_ALLOCATION);
    }

    /* ************************************************************* */

    if ((infptr->Fptr)->compress_type == RICE_1)
    {
        cbuf = (unsigned char *) calloc (nelem, sizeof (unsigned char));
        if (cbuf == NULL)
        {
	    ffpmsg("Out of memory for cbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TBYTE, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &charnull, cbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
	    free (cbuf);
            free(idata);
            return (*status);
        }

        /* uncompress the data */
        blocksize = (infptr->Fptr)->rice_blocksize;
        if ((*status = fits_rdecomp (cbuf, nelem, (unsigned int *)idata,
                tilelen, blocksize)))
        {
            free (cbuf);
            free(idata);
            return (*status);
        }

        free(cbuf);
    }

    /* ************************************************************* */

    else if ((infptr->Fptr)->compress_type == HCOMPRESS_1)
    {
        cbuf = (unsigned char *) calloc (nelem, sizeof (unsigned char));
        if (cbuf == NULL)
        {
	    ffpmsg("Out of memory for cbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TBYTE, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &charnull, cbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
	    free (cbuf);
            free(idata);
            return (*status);
        }

        /* uncompress the data */
        smooth = (infptr->Fptr)->hcomp_smooth;

        if ( ((infptr->Fptr)->zbitpix == BYTE_IMG ||
	       (infptr->Fptr)->zbitpix == SHORT_IMG) )  {

            if ((*status = fits_hdecompress(cbuf, smooth, idata, &nx, &ny,
	        &scale, status)))
            {
                free (cbuf);
                free(idata);
                return (*status);
            }

        } else {
	
            /* idata must have been allocated twice as large for this to work */
            if ((*status = fits_hdecompress64(cbuf, smooth, lldata, &nx, &ny,
	        &scale, status)))
            {
                free (cbuf);
                free(idata);
                return (*status);
            }
        }       

        free(cbuf);
    }

    /* ************************************************************* */

    else if ((infptr->Fptr)->compress_type == PLIO_1)
    {
        sbuf = (short *) calloc (nelem, sizeof (short));
        if (sbuf == NULL)
        {
	    ffpmsg("Out of memory for sbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TSHORT, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &snull, sbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
            free(idata);
	    free (sbuf);
            return (*status);
        }

        pl_l2pi (sbuf, 1, idata, tilelen);  /* uncompress the data */
        free(sbuf);
    }

    /* ************************************************************* */

    else if ((infptr->Fptr)->compress_type == GZIP_1)
    {
        cbuf = (unsigned char *) calloc (nelem, sizeof (unsigned char));
        if (cbuf == NULL)
        {
	    ffpmsg("Out of memory for cbuf. (imcomp_decompress_tile)");
            free(idata);
	    return (*status = MEMORY_ALLOCATION);
        }

        /* read array of compressed bytes */
        if (fits_read_col(infptr, TBYTE, (infptr->Fptr)->cn_compressed, nrow,
             1, nelem, &charnull, cbuf, NULL, status) > 0)
        {
            ffpmsg("error reading compressed byte stream from binary table");
            free(idata);
	    free (cbuf);
            return (*status);
        }

        /* uncompress the data */
        idatalen = tilelen * sizeof(int);
        if (uncompress2mem_from_mem ((char *)cbuf, nelem,
             (char **) &idata, &idatalen, realloc, &tilebytesize, status))
        {
            ffpmsg("uncompress2mem_from_mem returned with an error");
            free(idata);
            free (cbuf);
            return (*status);
        }

#if BYTESWAPPED
         ffswap4(idata, tilelen); /* reverse order of bytes */
#endif

        if (idatalen != tilebytesize)
        {
            ffpmsg("error: uncompressed tile has wrong size");
            free(idata);
            free (cbuf);
            return (*status = DATA_DECOMPRESSION_ERR);
        }

        free(cbuf);
    }

    /* ************************************************************* */
    else
    {
        ffpmsg("unknown compression algorithm");
        free(idata);
        return (*status = DATA_DECOMPRESSION_ERR);
    }

    /* ************************************************************* */
    /* copy the uncompressed tile data to the output buffer, doing */
    /* null checking, datatype conversion and linear scaling, if necessary */

    if (nulval == 0)
         nulval = &dummy;  /* set address to dummy value */

    if (datatype == TSHORT)
    {
            fffi4i2(idata, tilelen, bscale, bzero, nullcheck, tnull,
             *(short *) nulval, bnullarray, anynul,
             (short *) buffer, status);
    }
    else if (datatype == TINT)
    {
        fffi4int(idata, (long) tilelen, bscale, bzero, nullcheck, tnull,
         *(int *) nulval, bnullarray, anynul,
        (int *) buffer, status);
    }
    else if (datatype == TLONG)
    {
        fffi4i4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(long *) nulval, bnullarray, anynul,
          (long *) buffer, status);
    }
    else if (datatype == TFLOAT)
    {
        fffi4r4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(float *) nulval, bnullarray, anynul,
          (float *) buffer, status);
    }
    else if (datatype == TDOUBLE)
    {
        fffi4r8(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(double *) nulval, bnullarray, anynul,
          (double *) buffer, status);
    }
    else if (datatype == TBYTE)
    {
        fffi4i1(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned char *) nulval, bnullarray, anynul,
          (unsigned char *) buffer, status);
    }
    else if (datatype == TSBYTE)
    {
        fffi4s1(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(signed char *) nulval, bnullarray, anynul,
          (signed char *) buffer, status);
    }
    else if (datatype == TUSHORT)
    {
        fffi4u2(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned short *) nulval, bnullarray, anynul,
          (unsigned short *) buffer, status);
    }
    else if (datatype == TUINT)
    {
        fffi4uint(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned int *) nulval, bnullarray, anynul,
          (unsigned int *) buffer, status);
    }
    else if (datatype == TULONG)
    {
        fffi4u4(idata, tilelen, bscale, bzero, nullcheck, tnull,
         *(unsigned long *) nulval, bnullarray, anynul, 
          (unsigned long *) buffer, status);
    }
    else
         *status = BAD_DATATYPE;

    free(idata);

    return (*status);
}
/*--------------------------------------------------------------------------*/
int imcomp_copy_overlap (
    char *tile,         /* I - multi dimensional array of tile pixels */
    int pixlen,         /* I - number of bytes in each tile or image pixel */
    int ndim,           /* I - number of dimension in the tile and image */
    long *tfpixel,      /* I - first pixel number in each dim. of the tile */
    long *tlpixel,      /* I - last pixel number in each dim. of the tile */
    char *bnullarray,   /* I - array of null flags; used if nullcheck = 2 */
    char *image,        /* O - multi dimensional output image */
    long *fpixel,       /* I - first pixel number in each dim. of the image */
    long *lpixel,       /* I - last pixel number in each dim. of the image */
    long *ininc,        /* I - increment to be applied in each image dimen. */
    int nullcheck,      /* I - 0, 1: do nothing; 2: set nullarray for nulls */
    char *nullarray, 
    int *status)

/* 
  copy the intersecting pixels from a decompressed tile to the output image. 
  Both the tile and the image must have the same number of dimensions. 
*/
{
    long imgdim[MAX_COMPRESS_DIM]; /* product of preceding dimensions in the */
                                   /* output image, allowing for inc factor */
    long tiledim[MAX_COMPRESS_DIM]; /* product of preceding dimensions in the */
                                 /* tile, array;  inc factor is not relevant */
    long imgfpix[MAX_COMPRESS_DIM]; /* 1st img pix overlapping tile: 0 base, */
                                    /*  allowing for inc factor */
    long imglpix[MAX_COMPRESS_DIM]; /* last img pix overlapping tile 0 base, */
                                    /*  allowing for inc factor */
    long tilefpix[MAX_COMPRESS_DIM]; /* 1st tile pix overlapping img 0 base, */
                                    /*  allowing for inc factor */
    long inc[MAX_COMPRESS_DIM]; /* local copy of input ininc */
    long i1, i2, i3, i4;   /* offset along each axis of the image */
    long it1, it2, it3, it4;
    long im1, im2, im3, im4;  /* offset to image pixel, allowing for inc */
    long ipos, tf, tl;
    long t2, t3, t4;   /* offset along each axis of the tile */
    long tilepix, imgpix, tilepixbyte, imgpixbyte;
    int ii, overlap_bytes, overlap_flags;

    if (*status > 0)
        return(*status);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        /* set default values for higher dimensions */
        inc[ii] = 1;
        imgdim[ii] = 1;
        tiledim[ii] = 1;
        imgfpix[ii] = 0;
        imglpix[ii] = 0;
        tilefpix[ii] = 0;
    }

    /* ------------------------------------------------------------ */
    /* calc amount of overlap in each dimension; if there is zero   */
    /* overlap in any dimension then just return  */
    /* ------------------------------------------------------------ */
    
    for (ii = 0; ii < ndim; ii++)
    {
        if (tlpixel[ii] < fpixel[ii] || tfpixel[ii] > lpixel[ii])
            return(*status);  /* there are no overlapping pixels */

        inc[ii] = ininc[ii];

        /* calc dimensions of the output image section */
        imgdim[ii] = (lpixel[ii] - fpixel[ii]) / labs(inc[ii]) + 1;
        if (imgdim[ii] < 1)
            return(*status = NEG_AXIS);

        /* calc dimensions of the tile */
        tiledim[ii] = tlpixel[ii] - tfpixel[ii] + 1;
        if (tiledim[ii] < 1)
            return(*status = NEG_AXIS);

        if (ii > 0)
           tiledim[ii] *= tiledim[ii - 1];  /* product of dimensions */

        /* first and last pixels in image that overlap with the tile, 0 base */
        tf = tfpixel[ii] - 1;
        tl = tlpixel[ii] - 1;

        /* skip this plane if it falls in the cracks of the subsampled image */
        while ((tf-(fpixel[ii] - 1)) % labs(inc[ii]))
        {
           tf++;
           if (tf > tl)
             return(*status);  /* no overlapping pixels */
        }

        while ((tl-(fpixel[ii] - 1)) % labs(inc[ii]))
        {
           tl--;
           if (tf > tl)
             return(*status);  /* no overlapping pixels */
        }
        imgfpix[ii] = maxvalue((tf - fpixel[ii] +1) / labs(inc[ii]) , 0);
        imglpix[ii] = minvalue((tl - fpixel[ii] +1) / labs(inc[ii]) ,
                               imgdim[ii] - 1);

        /* first pixel in the tile that overlaps with the image (0 base) */
        tilefpix[ii] = maxvalue(fpixel[ii] - tfpixel[ii], 0);

        while ((tfpixel[ii] + tilefpix[ii] - fpixel[ii]) % labs(inc[ii]))
        {
           (tilefpix[ii])++;
           if (tilefpix[ii] >= tiledim[ii])
              return(*status);  /* no overlapping pixels */
        }
/*
printf("ii tfpixel, tlpixel %d %d %d \n",ii, tfpixel[ii], tlpixel[ii]);
printf("ii, tf, tl, imgfpix,imglpix, tilefpix %d %d %d %d %d %d\n",ii,
 tf,tl,imgfpix[ii], imglpix[ii],tilefpix[ii]);
*/
        if (ii > 0)
           imgdim[ii] *= imgdim[ii - 1];  /* product of dimensions */
    }

    /* ---------------------------------------------------------------- */
    /* calc number of pixels in each row (first dimension) that overlap */
    /* multiply by pixlen to get number of bytes to copy in each loop   */
    /* ---------------------------------------------------------------- */

    if (inc[0] != 1)
       overlap_flags = 1;  /* can only copy 1 pixel at a time */
    else
       overlap_flags = imglpix[0] - imgfpix[0] + 1;  /* can copy whole row */

    overlap_bytes = overlap_flags * pixlen;

    /* support up to 5 dimensions for now */
    for (i4 = 0, it4=0; i4 <= imglpix[4] - imgfpix[4]; i4++, it4++)
    {
     /* increment plane if it falls in the cracks of the subsampled image */
     while (ndim > 4 &&  (tfpixel[4] + tilefpix[4] - fpixel[4] + it4)
                          % labs(inc[4]) != 0)
        it4++;

       /* offset to start of hypercube */
       if (inc[4] > 0)
          im4 = (i4 + imgfpix[4]) * imgdim[3];
       else
          im4 = imgdim[4] - (i4 + 1 + imgfpix[4]) * imgdim[3];

      t4 = (tilefpix[4] + it4) * tiledim[3];
      for (i3 = 0, it3=0; i3 <= imglpix[3] - imgfpix[3]; i3++, it3++)
      {
       /* increment plane if it falls in the cracks of the subsampled image */
       while (ndim > 3 &&  (tfpixel[3] + tilefpix[3] - fpixel[3] + it3)
                            % labs(inc[3]) != 0)
          it3++;

       /* offset to start of cube */
       if (inc[3] > 0)
          im3 = (i3 + imgfpix[3]) * imgdim[2] + im4;
       else
          im3 = imgdim[3] - (i3 + 1 + imgfpix[3]) * imgdim[2] + im4;

       t3 = (tilefpix[3] + it3) * tiledim[2] + t4;

       /* loop through planes of the image */
       for (i2 = 0, it2=0; i2 <= imglpix[2] - imgfpix[2]; i2++, it2++)
       {
          /* incre plane if it falls in the cracks of the subsampled image */
          while (ndim > 2 &&  (tfpixel[2] + tilefpix[2] - fpixel[2] + it2)
                               % labs(inc[2]) != 0)
             it2++;

          /* offset to start of plane */
          if (inc[2] > 0)
             im2 = (i2 + imgfpix[2]) * imgdim[1] + im3;
          else
             im2 = imgdim[2] - (i2 + 1 + imgfpix[2]) * imgdim[1] + im3;

          t2 = (tilefpix[2] + it2) * tiledim[1] + t3;

          /* loop through rows of the image */
          for (i1 = 0, it1=0; i1 <= imglpix[1] - imgfpix[1]; i1++, it1++)
          {
             /* incre row if it falls in the cracks of the subsampled image */
             while (ndim > 1 &&  (tfpixel[1] + tilefpix[1] - fpixel[1] + it1)
                                  % labs(inc[1]) != 0)
                it1++;

             /* calc position of first pixel in tile to be copied */
             tilepix = tilefpix[0] + (tilefpix[1] + it1) * tiledim[0] + t2;

             /* offset to start of row */
             if (inc[1] > 0)
                im1 = (i1 + imgfpix[1]) * imgdim[0] + im2;
             else
                im1 = imgdim[1] - (i1 + 1 + imgfpix[1]) * imgdim[0] + im2;
/*
printf("inc = %d %d %d %d\n",inc[0],inc[1],inc[2],inc[3]);
printf("im1,im2,im3,im4 = %d %d %d %d\n",im1,im2,im3,im4);
*/
             /* offset to byte within the row */
             if (inc[0] > 0)
                imgpix = imgfpix[0] + im1;
             else
                imgpix = imgdim[0] - 1 - imgfpix[0] + im1;
/*
printf("tilefpix0,1, imgfpix1, it1, inc1, t2= %d %d %d %d %d %d\n",
       tilefpix[0],tilefpix[1],imgfpix[1],it1,inc[1], t2);
printf("i1, it1, tilepix, imgpix %d %d %d %d \n", i1, it1, tilepix, imgpix);
*/
             /* loop over pixels along one row of the image */
             for (ipos = imgfpix[0]; ipos <= imglpix[0]; ipos += overlap_flags)
             {
               if (nullcheck == 2)
               {
                   /* copy overlapping null flags from tile to image */
                   memcpy(nullarray + imgpix, bnullarray + tilepix,
                          overlap_flags);
               }

               /* convert from image pixel to byte offset */
               tilepixbyte = tilepix * pixlen;
               imgpixbyte  = imgpix  * pixlen;
/*
printf("  tilepix, tilepixbyte, imgpix, imgpixbyte= %d %d %d %d\n",
          tilepix, tilepixbyte, imgpix, imgpixbyte);
*/
               /* copy overlapping row of pixels from tile to image */
               memcpy(image + imgpixbyte, tile + tilepixbyte, overlap_bytes);

               tilepix += (overlap_flags * labs(inc[0]));
               if (inc[0] > 0)
                 imgpix += overlap_flags;
               else
                 imgpix -= overlap_flags;
            }
          }
        }
      }
    }
    return(*status);
}

/*--------------------------------------------------------------------------*/
int imcomp_merge_overlap (
    char *tile,         /* O - multi dimensional array of tile pixels */
    int pixlen,         /* I - number of bytes in each tile or image pixel */
    int ndim,           /* I - number of dimension in the tile and image */
    long *tfpixel,      /* I - first pixel number in each dim. of the tile */
    long *tlpixel,      /* I - last pixel number in each dim. of the tile */
    char *bnullarray,   /* I - array of null flags; used if nullcheck = 2 */
    char *image,        /* I - multi dimensional output image */
    long *fpixel,       /* I - first pixel number in each dim. of the image */
    long *lpixel,       /* I - last pixel number in each dim. of the image */
    int nullcheck,      /* I - 0, 1: do nothing; 2: set nullarray for nulls */
    int *status)

/* 
  Similar to imcomp_copy_overlap, except it copies the overlapping pixels from
  the 'image' to the 'tile'.
*/
{
    long imgdim[MAX_COMPRESS_DIM]; /* product of preceding dimensions in the */
                                   /* output image, allowing for inc factor */
    long tiledim[MAX_COMPRESS_DIM]; /* product of preceding dimensions in the */
                                 /* tile, array;  inc factor is not relevant */
    long imgfpix[MAX_COMPRESS_DIM]; /* 1st img pix overlapping tile: 0 base, */
                                    /*  allowing for inc factor */
    long imglpix[MAX_COMPRESS_DIM]; /* last img pix overlapping tile 0 base, */
                                    /*  allowing for inc factor */
    long tilefpix[MAX_COMPRESS_DIM]; /* 1st tile pix overlapping img 0 base, */
                                    /*  allowing for inc factor */
    long inc[MAX_COMPRESS_DIM]; /* local copy of input ininc */
    long i1, i2, i3, i4;   /* offset along each axis of the image */
    long it1, it2, it3, it4;
    long im1, im2, im3, im4;  /* offset to image pixel, allowing for inc */
    long ipos, tf, tl;
    long t2, t3, t4;   /* offset along each axis of the tile */
    long tilepix, imgpix, tilepixbyte, imgpixbyte;
    int ii, overlap_bytes, overlap_flags;

    if (*status > 0)
        return(*status);

    for (ii = 0; ii < MAX_COMPRESS_DIM; ii++)
    {
        /* set default values for higher dimensions */
        inc[ii] = 1;
        imgdim[ii] = 1;
        tiledim[ii] = 1;
        imgfpix[ii] = 0;
        imglpix[ii] = 0;
        tilefpix[ii] = 0;
    }

    /* ------------------------------------------------------------ */
    /* calc amount of overlap in each dimension; if there is zero   */
    /* overlap in any dimension then just return  */
    /* ------------------------------------------------------------ */
    
    for (ii = 0; ii < ndim; ii++)
    {
        if (tlpixel[ii] < fpixel[ii] || tfpixel[ii] > lpixel[ii])
            return(*status);  /* there are no overlapping pixels */

        /* calc dimensions of the output image section */
        imgdim[ii] = (lpixel[ii] - fpixel[ii]) / labs(inc[ii]) + 1;
        if (imgdim[ii] < 1)
            return(*status = NEG_AXIS);

        /* calc dimensions of the tile */
        tiledim[ii] = tlpixel[ii] - tfpixel[ii] + 1;
        if (tiledim[ii] < 1)
            return(*status = NEG_AXIS);

        if (ii > 0)
           tiledim[ii] *= tiledim[ii - 1];  /* product of dimensions */

        /* first and last pixels in image that overlap with the tile, 0 base */
        tf = tfpixel[ii] - 1;
        tl = tlpixel[ii] - 1;

        /* skip this plane if it falls in the cracks of the subsampled image */
        while ((tf-(fpixel[ii] - 1)) % labs(inc[ii]))
        {
           tf++;
           if (tf > tl)
             return(*status);  /* no overlapping pixels */
        }

        while ((tl-(fpixel[ii] - 1)) % labs(inc[ii]))
        {
           tl--;
           if (tf > tl)
             return(*status);  /* no overlapping pixels */
        }
        imgfpix[ii] = maxvalue((tf - fpixel[ii] +1) / labs(inc[ii]) , 0);
        imglpix[ii] = minvalue((tl - fpixel[ii] +1) / labs(inc[ii]) ,
                               imgdim[ii] - 1);

        /* first pixel in the tile that overlaps with the image (0 base) */
        tilefpix[ii] = maxvalue(fpixel[ii] - tfpixel[ii], 0);

        while ((tfpixel[ii] + tilefpix[ii] - fpixel[ii]) % labs(inc[ii]))
        {
           (tilefpix[ii])++;
           if (tilefpix[ii] >= tiledim[ii])
              return(*status);  /* no overlapping pixels */
        }
/*
printf("ii tfpixel, tlpixel %d %d %d \n",ii, tfpixel[ii], tlpixel[ii]);
printf("ii, tf, tl, imgfpix,imglpix, tilefpix %d %d %d %d %d %d\n",ii,
 tf,tl,imgfpix[ii], imglpix[ii],tilefpix[ii]);
*/
        if (ii > 0)
           imgdim[ii] *= imgdim[ii - 1];  /* product of dimensions */
    }

    /* ---------------------------------------------------------------- */
    /* calc number of pixels in each row (first dimension) that overlap */
    /* multiply by pixlen to get number of bytes to copy in each loop   */
    /* ---------------------------------------------------------------- */

    if (inc[0] != 1)
       overlap_flags = 1;  /* can only copy 1 pixel at a time */
    else
       overlap_flags = imglpix[0] - imgfpix[0] + 1;  /* can copy whole row */

    overlap_bytes = overlap_flags * pixlen;

    /* support up to 5 dimensions for now */
    for (i4 = 0, it4=0; i4 <= imglpix[4] - imgfpix[4]; i4++, it4++)
    {
     /* increment plane if it falls in the cracks of the subsampled image */
     while (ndim > 4 &&  (tfpixel[4] + tilefpix[4] - fpixel[4] + it4)
                          % labs(inc[4]) != 0)
        it4++;

       /* offset to start of hypercube */
       if (inc[4] > 0)
          im4 = (i4 + imgfpix[4]) * imgdim[3];
       else
          im4 = imgdim[4] - (i4 + 1 + imgfpix[4]) * imgdim[3];

      t4 = (tilefpix[4] + it4) * tiledim[3];
      for (i3 = 0, it3=0; i3 <= imglpix[3] - imgfpix[3]; i3++, it3++)
      {
       /* increment plane if it falls in the cracks of the subsampled image */
       while (ndim > 3 &&  (tfpixel[3] + tilefpix[3] - fpixel[3] + it3)
                            % labs(inc[3]) != 0)
          it3++;

       /* offset to start of cube */
       if (inc[3] > 0)
          im3 = (i3 + imgfpix[3]) * imgdim[2] + im4;
       else
          im3 = imgdim[3] - (i3 + 1 + imgfpix[3]) * imgdim[2] + im4;

       t3 = (tilefpix[3] + it3) * tiledim[2] + t4;

       /* loop through planes of the image */
       for (i2 = 0, it2=0; i2 <= imglpix[2] - imgfpix[2]; i2++, it2++)
       {
          /* incre plane if it falls in the cracks of the subsampled image */
          while (ndim > 2 &&  (tfpixel[2] + tilefpix[2] - fpixel[2] + it2)
                               % labs(inc[2]) != 0)
             it2++;

          /* offset to start of plane */
          if (inc[2] > 0)
             im2 = (i2 + imgfpix[2]) * imgdim[1] + im3;
          else
             im2 = imgdim[2] - (i2 + 1 + imgfpix[2]) * imgdim[1] + im3;

          t2 = (tilefpix[2] + it2) * tiledim[1] + t3;

          /* loop through rows of the image */
          for (i1 = 0, it1=0; i1 <= imglpix[1] - imgfpix[1]; i1++, it1++)
          {
             /* incre row if it falls in the cracks of the subsampled image */
             while (ndim > 1 &&  (tfpixel[1] + tilefpix[1] - fpixel[1] + it1)
                                  % labs(inc[1]) != 0)
                it1++;

             /* calc position of first pixel in tile to be copied */
             tilepix = tilefpix[0] + (tilefpix[1] + it1) * tiledim[0] + t2;

             /* offset to start of row */
             if (inc[1] > 0)
                im1 = (i1 + imgfpix[1]) * imgdim[0] + im2;
             else
                im1 = imgdim[1] - (i1 + 1 + imgfpix[1]) * imgdim[0] + im2;
/*
printf("inc = %d %d %d %d\n",inc[0],inc[1],inc[2],inc[3]);
printf("im1,im2,im3,im4 = %d %d %d %d\n",im1,im2,im3,im4);
*/
             /* offset to byte within the row */
             if (inc[0] > 0)
                imgpix = imgfpix[0] + im1;
             else
                imgpix = imgdim[0] - 1 - imgfpix[0] + im1;
/*
printf("tilefpix0,1, imgfpix1, it1, inc1, t2= %d %d %d %d %d %d\n",
       tilefpix[0],tilefpix[1],imgfpix[1],it1,inc[1], t2);
printf("i1, it1, tilepix, imgpix %d %d %d %d \n", i1, it1, tilepix, imgpix);
*/
             /* loop over pixels along one row of the image */
             for (ipos = imgfpix[0]; ipos <= imglpix[0]; ipos += overlap_flags)
             {
               /* convert from image pixel to byte offset */
               tilepixbyte = tilepix * pixlen;
               imgpixbyte  = imgpix  * pixlen;
/*
printf("  tilepix, tilepixbyte, imgpix, imgpixbyte= %d %d %d %d\n",
          tilepix, tilepixbyte, imgpix, imgpixbyte);
*/
               /* copy overlapping row of pixels from image to tile */
               memcpy(tile + tilepixbyte, image + imgpixbyte,  overlap_bytes);

               tilepix += (overlap_flags * labs(inc[0]));
               if (inc[0] > 0)
                 imgpix += overlap_flags;
               else
                 imgpix -= overlap_flags;
            }
          }
        }
      }
    }
    return(*status);
}
