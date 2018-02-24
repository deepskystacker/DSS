#include <string.h>
#include <stdio.h>
#include "fitsio.h"

int main(int argc, char *argv[])
{
    fitsfile *fptr;         /* FITS file pointer, defined in fitsio.h */
    char card[FLEN_CARD];   /* Standard string lengths defined in fitsio.h */
    int status = 0, single = 0, hdupos, nkeys, ii;

    if (argc != 2) {
      printf("Usage:  listhead filename[ext] \n");
      printf("\n");
      printf("List the FITS header keywords in a single extension, or, if \n");
      printf("ext is not given, list the keywords in all the extensions. \n");
      printf("\n");
      printf("Examples: \n");
      printf("   listhead file.fits      - list every header in the file \n");
      printf("   listhead file.fits[0]   - list primary array header \n");
      printf("   listhead file.fits[2]   - list header of 2nd extension \n");
      printf("   listhead file.fits+2    - same as above \n");
      printf("   listhead file.fits[GTI] - list header of GTI extension\n");
      printf("\n");
      printf("Note that it may be necessary to enclose the input file\n");
      printf("name in single quote characters on the Unix command line.\n");
      return(0);
    }

    if (!fits_open_file(&fptr, argv[1], READONLY, &status))
    {
      fits_get_hdu_num(fptr, &hdupos);  /* Get the current HDU position */

      /* List only a single header if a specific extension was given */ 
      if (hdupos != 1 || strchr(argv[1], '[')) single = 1;

      for (; !status; hdupos++)  /* Main loop through each extension */
      {
        fits_get_hdrspace(fptr, &nkeys, NULL, &status); /* get # of keywords */

        printf("Header listing for HDU #%d:\n", hdupos);

        for (ii = 1; ii <= nkeys; ii++) { /* Read and print each keywords */

           if (fits_read_record(fptr, ii, card, &status))break;
           printf("%s\n", card);
        }
        printf("END\n\n");  /* terminate listing with END */

        if (single) break;  /* quit if only listing a single header */

        fits_movrel_hdu(fptr, 1, NULL, &status);  /* try to move to next HDU */
      }

      if (status == END_OF_FILE)  status = 0; /* Reset after normal error */

      fits_close_file(fptr, &status);
    }

    if (status) fits_report_error(stderr, status); /* print any error message */
    return(status);
}

