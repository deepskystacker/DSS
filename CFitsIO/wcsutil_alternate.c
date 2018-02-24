#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "fitsio2.h"

/* ======================================================================
This file contains stubs for the AIPS WCS routines that are 
contained in the source file wcsutil.c.  The routines in wcsutil.c
should only be used by software that adheres to the terms of
the GNU General Public License.  Users who want to use CFITSIO but are
unwilling to release their code under the terms of the GNU General
Public License should replace the wcsutil.c file with this current
file before building the CFITSIO library.   This alternate version of
CFITSIO will behave the same as the standard version, except that it
will not support the ffwldp and ffxypx routines that calculate 
image coordinate transformation from pixel coordinates to world 
coordinates (e.g. Right Ascension and Declination) and vise versa.
======================================================================== */


/*--------------------------------------------------------------------------*/
int ffwldp(double xpix, double ypix, double xref, double yref,
      double xrefpix, double yrefpix, double xinc, double yinc, double rot,
      char *type, double *xpos, double *ypos, int *status)
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support");
    ffpmsg(" celestial coordinate transformations.");
    return(*status = 503);
}  
/*--------------------------------------------------------------------------*/
int ffxypx(double xpos, double ypos, double xref, double yref, 
      double xrefpix, double yrefpix, double xinc, double yinc, double rot,
      char *type, double *xpix, double *ypix, int *status)
{
    if (*status > 0)
        return(*status);

    ffpmsg("This non-GNU version of CFITSIO does not support");
    ffpmsg(" celestial coordinate transformations.");
    return(*status = 503);
} 
