#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fitsio2.h"
#include "region.h"

static int Pt_in_Poly( double x, double y, int nPts, double *Pts );

/*---------------------------------------------------------------------------*/
int ffrrgn( const char *filename,
            WCSdata    *wcs,
            SAORegion  **Rgn,
            int        *status )
/*  Read regions from a SAO-style region file and return the information     */
/*  in the "SAORegion" structure.  If it is nonNULL, use wcs to convert the  */
/*  region coordinates to pixels.  Return an error if region is in degrees   */
/*  but no WCS data is provided.                                             */
/*---------------------------------------------------------------------------*/
{
   char     *currLine;
   char     *namePtr, *paramPtr, *currLoc;
   char     *pX, *pY, *endp;
   long     allocLen, lineLen, hh, mm, dd;
   double   *coords = 0, X, Y, R, x, y, ss, xsave= 0., ysave= 0.;
   int      nParams, nCoords, negdec;
   int      i, done;
   FILE     *rgnFile;
   coordFmt cFmt;
   SAORegion *aRgn;
   RgnShape *newShape, *tmpShape;

   if( *status ) return( *status );

   aRgn = (SAORegion *)malloc( sizeof(SAORegion) );
   if( ! aRgn ) {
      ffpmsg("Couldn't allocate memory to hold Region file contents.");
      return(*status = MEMORY_ALLOCATION );
   }
   aRgn->nShapes    =    0;
   aRgn->Shapes     = NULL;
   if( wcs && wcs->exists )
      aRgn->wcs = *wcs;
   else
      aRgn->wcs.exists = 0;

   cFmt = pixel_fmt; /* set default format */

   /*  Allocate Line Buffer  */

   allocLen = 512;
   currLine = (char *)malloc( allocLen * sizeof(char) );
   if( !currLine ) {
      free( aRgn );
      ffpmsg("Couldn't allocate memory to hold Region file contents.");
      return(*status = MEMORY_ALLOCATION );
   }

   /*  Open Region File  */

   if( (rgnFile = fopen( filename, "r" ))==NULL ) {
      sprintf(currLine,"Could not open Region file %s.",filename);
      ffpmsg( currLine );
      free( currLine );
      free( aRgn );
      return( *status = FILE_NOT_OPENED );
   }
   
   /*  Read in file, line by line  */

   while( fgets(currLine,allocLen,rgnFile) != NULL ) {

      /*  Make sure we have a full line of text  */

      lineLen = strlen(currLine);
      while( lineLen==allocLen-1 && currLine[lineLen-1]!='\n' ) {
         currLoc = (char *)realloc( currLine, 2 * allocLen * sizeof(char) );
         if( !currLoc ) {
            ffpmsg("Couldn't allocate memory to hold Region file contents.");
            *status = MEMORY_ALLOCATION;
            goto error;
         } else {
            currLine = currLoc;
         }
         fgets( currLine+lineLen, allocLen+1, rgnFile );
         allocLen += allocLen;
         lineLen  += strlen(currLine+lineLen);
      }

      currLoc = currLine;
      if( *currLoc == '#' ) {

         /*  Look to see if it is followed by a format statement...  */
         /*  if not skip line                                        */

         currLoc++;
         while( *currLoc==' ' ) currLoc++;
         if( !strncasecmp( currLoc, "format:", 7 ) ) {
            if( aRgn->nShapes ) {
               ffpmsg("Format code encountered after reading 1 or more shapes.");
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }
            currLoc += 7;
            while( *currLoc==' ' ) currLoc++;
            if( !strncasecmp( currLoc, "pixel", 5 ) ) {
               cFmt = pixel_fmt;
            } else if( !strncasecmp( currLoc, "degree", 6 ) ) {
               cFmt = degree_fmt;
            } else if( !strncasecmp( currLoc, "hhmmss", 6 ) ) {
               cFmt = hhmmss_fmt;
            } else if( !strncasecmp( currLoc, "hms", 3 ) ) {
               cFmt = hhmmss_fmt;
            } else {
               ffpmsg("Unknown format code encountered in region file.");
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }
         }

      } else if( !strncasecmp( currLoc, "glob", 4 ) ) {
		  /* skip lines that begin with the word 'global' */

      } else {

         while( *currLoc != '\0' ) {

            namePtr  = currLoc;
            paramPtr = NULL;
            nParams  = 1;

            /*  Search for closing parenthesis  */

            done = 0;
            while( !done && !*status && *currLoc ) {
               switch (*currLoc) {
               case '(':
                  *currLoc = '\0';
                  currLoc++;
                  if( paramPtr )   /* Can't have two '(' in a region! */
                     *status = 1;
                  else
                     paramPtr = currLoc;
                  break;
               case ')':
                  *currLoc = '\0';
                  currLoc++;
                  if( !paramPtr )  /* Can't have a ')' without a '(' first */
                     *status = 1;
                  else
                     done = 1;
                  break;
               case '#':
               case '\n':
                  *currLoc = '\0';
                  if( !paramPtr )  /* Allow for a blank line */
                     done = 1;
                  break;
               case ':':  
                  currLoc++;
                  cFmt = hhmmss_fmt;
                  break;
               case 'd':
                  currLoc++;
                  cFmt = degree_fmt;
                  break;
               case ',':
                  nParams++;  /* Fall through to default */
               default:
                  currLoc++;
                  break;
               }
            }
            if( *status || !done ) {
               ffpmsg( "Error reading Region file" );
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }

            /*  Skip white space in region name  */

            while( *namePtr==' ' ) namePtr++;

            /*  Was this a blank line? Or the end of the current one  */

            if( ! *namePtr && ! paramPtr ) continue;


            /*  Check for format code at beginning of the line */

            if( !strncasecmp( namePtr, "image;", 6 ) ) {
				namePtr += 6;
				cFmt = pixel_fmt;
            } else if( !strncasecmp( namePtr, "physical;", 9 ) ) {
                                namePtr += 9;
                                cFmt = pixel_fmt;
            } else if( !strncasecmp( namePtr, "linear;", 7 ) ) {
                                namePtr += 7;
                                cFmt = pixel_fmt;
            } else if( !strncasecmp( namePtr, "fk4;", 4 ) ) {
				namePtr += 4;
				cFmt = degree_fmt;
            } else if( !strncasecmp( namePtr, "fk5;", 4 ) ) {
				namePtr += 4;
				cFmt = degree_fmt;
            } else if( !strncasecmp( namePtr, "icrs;", 5 ) ) {
				namePtr += 5;
				cFmt = degree_fmt;

            /* the following 5 cases support region files created by POW 
	       (or ds9 Version 4.x) which
               may have lines containing  only a format code, not followed
               by a ';' (and with no region specifier on the line).  We use
               the 'continue' statement to jump to the end of the loop and
               then continue reading the next line of the region file. */

            } else if( !strncasecmp( namePtr, "fk5", 3 ) ) {
				cFmt = degree_fmt;
                                continue;  /* supports POW region file format */
            } else if( !strncasecmp( namePtr, "fk4", 3 ) ) {
				cFmt = degree_fmt;
                                continue;  /* supports POW region file format */
            } else if( !strncasecmp( namePtr, "icrs", 4 ) ) {
				cFmt = degree_fmt;
                                continue;  /* supports POW region file format */
            } else if( !strncasecmp( namePtr, "image", 5 ) ) {
				cFmt = pixel_fmt;
                                continue;  /* supports POW region file format */
            } else if( !strncasecmp( namePtr, "physical", 8 ) ) {
				cFmt = pixel_fmt;
                                continue;  /* supports POW region file format */


            } else if( !strncasecmp( namePtr, "galactic;", 9 ) ) {
               ffpmsg( "Galactic region coordinates not supported" );
               ffpmsg( namePtr );
               *status = PARSE_SYNTAX_ERR;
               goto error;
            } else if( !strncasecmp( namePtr, "ecliptic;", 9 ) ) {
               ffpmsg( "ecliptic region coordinates not supported" );
               ffpmsg( namePtr );
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }

            /**************************************************/
            /*  We've apparently found a region... Set it up  */
            /**************************************************/

            if( !(aRgn->nShapes % 10) ) {
               if( aRgn->Shapes )
                  tmpShape = (RgnShape *)realloc( aRgn->Shapes,
                                                  (10+aRgn->nShapes)
                                                  * sizeof(RgnShape) );
               else
                  tmpShape = (RgnShape *) malloc( 10 * sizeof(RgnShape) );
               if( tmpShape ) {
                  aRgn->Shapes = tmpShape;
               } else {
                  ffpmsg( "Failed to allocate memory for Region data");
                  *status = MEMORY_ALLOCATION;
                  goto error;
               }

            }
            newShape        = &aRgn->Shapes[aRgn->nShapes++];
            newShape->sign  = 1;
            newShape->shape = point_rgn;

            while( *namePtr==' ' ) namePtr++;
            
			/*  Check for the shape's sign  */

            if( *namePtr=='+' ) {
               namePtr++;
            } else if( *namePtr=='-' ) {
               namePtr++;
               newShape->sign = 0;
            }

            /* Skip white space in region name */

            while( *namePtr==' ' ) namePtr++;
            if( *namePtr=='\0' ) {
               ffpmsg( "Error reading Region file" );
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }
            lineLen = strlen( namePtr ) - 1;
            while( namePtr[lineLen]==' ' ) namePtr[lineLen--] = '\0';

            /*  Now identify the region  */

            if(        !strcasecmp( namePtr, "circle"  ) ) {
               newShape->shape = circle_rgn;
               if( nParams != 3 )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "annulus" ) ) {
               newShape->shape = annulus_rgn;
               if( nParams != 4 )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "ellipse" ) ) {
               newShape->shape = ellipse_rgn;
               if( nParams < 4 || nParams > 5 )
                  *status = PARSE_SYNTAX_ERR;
               newShape->param.gen.p[4] = 0.0;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "elliptannulus" ) ) {
               newShape->shape = elliptannulus_rgn;
               if( !( nParams==8 || nParams==6 ) )
                  *status = PARSE_SYNTAX_ERR;
               newShape->param.gen.p[6] = 0.0;
               newShape->param.gen.p[7] = 0.0;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "box"    ) 
                    || !strcasecmp( namePtr, "rotbox" ) ) {
               newShape->shape = box_rgn;
               if( nParams < 4 || nParams > 5 )
                  *status = PARSE_SYNTAX_ERR;
               newShape->param.gen.p[4] = 0.0;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "rectangle"    )
                    || !strcasecmp( namePtr, "rotrectangle" ) ) {
               newShape->shape = rectangle_rgn;
               if( nParams < 4 || nParams > 5 )
                  *status = PARSE_SYNTAX_ERR;
               newShape->param.gen.p[4] = 0.0;
               nCoords = 4;
            } else if( !strcasecmp( namePtr, "diamond"    )
                    || !strcasecmp( namePtr, "rotdiamond" )
                    || !strcasecmp( namePtr, "rhombus"    )
                    || !strcasecmp( namePtr, "rotrhombus" ) ) {
               newShape->shape = diamond_rgn;
               if( nParams < 4 || nParams > 5 )
                  *status = PARSE_SYNTAX_ERR;
               newShape->param.gen.p[4] = 0.0;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "sector"  )
                    || !strcasecmp( namePtr, "pie"     ) ) {
               newShape->shape = sector_rgn;
               if( nParams != 4 )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "point"   ) ) {
               newShape->shape = point_rgn;
               if( nParams != 2 )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = 2;
            } else if( !strcasecmp( namePtr, "line"    ) ) {
               newShape->shape = line_rgn;
               if( nParams != 4 )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = 4;
            } else if( !strcasecmp( namePtr, "polygon" ) ) {
               newShape->shape = poly_rgn;
               if( nParams < 6 || (nParams&1) )
                  *status = PARSE_SYNTAX_ERR;
               nCoords = nParams;
            } else {
               ffpmsg( "Unrecognized region found in region file:" );
               ffpmsg( namePtr );
               *status = PARSE_SYNTAX_ERR;
               goto error;
            }
            if( *status ) {
               ffpmsg( "Wrong number of parameters found for region" );
               ffpmsg( namePtr );
               goto error;
            }

            /*  Parse Parameter string... convert to pixels if necessary  */

            if( newShape->shape==poly_rgn ) {
               newShape->param.poly.Pts = (double *)malloc( nParams
                                                            * sizeof(double) );
               if( !newShape->param.poly.Pts ) {
                  ffpmsg(
                      "Could not allocate memory to hold polygon parameters" );
                  *status = MEMORY_ALLOCATION;
                  goto error;
               }
               newShape->param.poly.nPts = nParams;
               coords = newShape->param.poly.Pts;
            } else
               coords = newShape->param.gen.p;

            /*  Parse the initial "WCS?" coordinates  */
            for( i=0; i<nCoords; i+=2 ) {
               pX = paramPtr;
               while( *paramPtr!=',' ) paramPtr++;
               *(paramPtr++) = '\0';

               pY = paramPtr;
               while( *paramPtr!=',' && *paramPtr != '\0' ) paramPtr++;
               *(paramPtr++) = '\0';

               if( strchr(pX, ':' ) ) {
                  /*  Read in special format & convert to decimal degrees  */
                  cFmt = hhmmss_fmt;
                  mm = 0;
                  ss = 0.;
                  hh = strtol(pX, &endp, 10);
                  if (endp && *endp==':') {
                      pX = endp + 1;
                      mm = strtol(pX, &endp, 10);
                      if (endp && *endp==':') {
                          pX = endp + 1;
                          ss = atof( pX );
                      }
                  }
                  X = 15. * (hh + mm/60. + ss/3600.); /* convert to degrees */

                  mm = 0;
                  ss = 0.;
                  negdec = 0;

                  while( *pY==' ' ) pY++;
                  if (*pY=='-') {
                      negdec = 1;
                      pY++;
                  }
                  dd = strtol(pY, &endp, 10);
                  if (endp && *endp==':') {
                      pY = endp + 1;
                      mm = strtol(pY, &endp, 10);
                      if (endp && *endp==':') {
                          pY = endp + 1;
                          ss = atof( pY );
                      }
                  }
                  if (negdec)
                     Y = -dd - mm/60. - ss/3600.; /* convert to degrees */
                  else
                     Y = dd + mm/60. + ss/3600.;

               } else {
                  X = atof( pX );
                  Y = atof( pY );
               }
               if (i==0) {   /* save 1st coord. in case needed later */
                   xsave = X;
                   ysave = Y;
               }

               if( cFmt!=pixel_fmt ) {
                  /*  Convert to pixels  */
                  if( wcs==NULL || ! wcs->exists ) {
                     ffpmsg("WCS information needed to convert region coordinates.");
                     *status = NO_WCS_KEY;
                     goto error;
                  }
                  
                  if( ffxypx(  X,  Y, wcs->xrefval, wcs->yrefval,
                                      wcs->xrefpix, wcs->yrefpix,
                                      wcs->xinc,    wcs->yinc,
                                      wcs->rot,     wcs->type,
                              &x, &y, status ) ) {
                     ffpmsg("Error converting region to pixel coordinates.");
                     goto error;
                  }
                  X = x; Y = y;
               }
               coords[i]   = X;
               coords[i+1] = Y;
            }

            /*  Read in remaining parameters...  */

            for( ; i<nParams; i++ ) {
               pX = paramPtr;
               while( *paramPtr!=',' && *paramPtr != '\0' ) paramPtr++;
               *(paramPtr++) = '\0';
               coords[i] = strtod( pX, &endp );

               if (endp && *endp=='"') {
                   /* parameter given in arcsec so convert to pixels. */
                   /* Increment first Y coordinate by this amount then calc */
                   /* the distance in pixels from the original coordinate. */
                   /* NOTE: This assumes the pixels are square!! */
                   if (ysave < 0.)
                       Y = ysave + coords[i]/3600.;  /* don't exceed -90 */
                   else
                       Y = ysave - coords[i]/3600.;  /* don't exceed +90 */

                   X = xsave;
                   if( ffxypx(  X,  Y, wcs->xrefval, wcs->yrefval,
                                      wcs->xrefpix, wcs->yrefpix,
                                      wcs->xinc,    wcs->yinc,
                                      wcs->rot,     wcs->type,
                              &x, &y, status ) ) {
                     ffpmsg("Error converting region to pixel coordinates.");
                     goto error;
                   }

                   coords[i] = sqrt( pow(x-coords[0],2) + pow(y-coords[1],2) );

               } else if (endp && *endp=='\'') {
                   /* parameter given in arcmin so convert to pixels. */
                   /* Increment first Y coordinate by this amount, then calc */
                   /* the distance in pixels from the original coordinate. */
                   /* NOTE: This assumes the pixels are square!! */
                   if (ysave < 0.)
                       Y = ysave + coords[i]/60.;  /* don't exceed -90 */
                   else
                       Y = ysave - coords[i]/60.;  /* don't exceed +90 */

                   X = xsave;
                   if( ffxypx(  X,  Y, wcs->xrefval, wcs->yrefval,
                                      wcs->xrefpix, wcs->yrefpix,
                                      wcs->xinc,    wcs->yinc,
                                      wcs->rot,     wcs->type,
                              &x, &y, status ) ) {
                     ffpmsg("Error converting region to pixel coordinates.");
                     goto error;
                   }

                   coords[i] = sqrt( pow(x-coords[0],2) + pow(y-coords[1],2) );

               } else if (endp && *endp=='d') {
                   /* parameter given in degrees so convert to pixels. */
                   /* Increment first Y coordinate by this amount, then calc */
                   /* the distance in pixels from the original coordinate. */
                   /* NOTE: This assumes the pixels are square!! */
                   if (ysave < 0.)
                       Y = ysave + coords[i];  /* don't exceed -90 */
                   else
                       Y = ysave - coords[i];  /* don't exceed +90 */

                   X = xsave;
                   if( ffxypx(  X,  Y, wcs->xrefval, wcs->yrefval,
                                      wcs->xrefpix, wcs->yrefpix,
                                      wcs->xinc,    wcs->yinc,
                                      wcs->rot,     wcs->type,
                              &x, &y, status ) ) {
                     ffpmsg("Error converting region to pixel coordinates.");
                     goto error;
                   }

                   coords[i] = sqrt( pow(x-coords[0],2) + pow(y-coords[1],2) );
               }
            }

            /* Perform some useful calculations now to speed up filter later */
            /* Also, correct the position angle for any WCS rotation:  */
            /*    If regions are specified in WCS coordintes, then the angles */
            /*    are relative to the WCS system, not the pixel X,Y system */

            switch( newShape->shape ) {
            case circle_rgn:
               newShape->param.gen.a = coords[2] * coords[2];
               break;
            case annulus_rgn:
               newShape->param.gen.a = coords[2] * coords[2];
               newShape->param.gen.b = coords[3] * coords[3];
               break;
            case sector_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[2] += (wcs->rot);
                 coords[3] += (wcs->rot);
               }
               while( coords[2]> 180.0 ) coords[2] -= 360.0;
               while( coords[2]<=-180.0 ) coords[2] += 360.0;
               while( coords[3]> 180.0 ) coords[3] -= 360.0;
               while( coords[3]<=-180.0 ) coords[3] += 360.0;
               break;
            case ellipse_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[4] += (wcs->rot);
               }
               newShape->param.gen.sinT = sin( myPI * (coords[4] / 180.0) );
               newShape->param.gen.cosT = cos( myPI * (coords[4] / 180.0) );
               break;
            case elliptannulus_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[6] += (wcs->rot);
                 coords[7] += (wcs->rot);
               }
               newShape->param.gen.a    = sin( myPI * (coords[6] / 180.0) );
               newShape->param.gen.b    = cos( myPI * (coords[6] / 180.0) );
               newShape->param.gen.sinT = sin( myPI * (coords[7] / 180.0) );
               newShape->param.gen.cosT = cos( myPI * (coords[7] / 180.0) );
               break;
            case box_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[4] += (wcs->rot);
               }
               newShape->param.gen.sinT = sin( myPI * (coords[4] / 180.0) );
               newShape->param.gen.cosT = cos( myPI * (coords[4] / 180.0) );
               break;
            case rectangle_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[4] += (wcs->rot);
               }
               newShape->param.gen.sinT = sin( myPI * (coords[4] / 180.0) );
               newShape->param.gen.cosT = cos( myPI * (coords[4] / 180.0) );
               X = 0.5 * ( coords[2]-coords[0] );
               Y = 0.5 * ( coords[3]-coords[1] );
               newShape->param.gen.a = fabs( X * newShape->param.gen.cosT
                                             + Y * newShape->param.gen.sinT );
               newShape->param.gen.b = fabs( Y * newShape->param.gen.cosT
                                             - X * newShape->param.gen.sinT );
               newShape->param.gen.p[5] = 0.5 * ( coords[2]+coords[0] );
               newShape->param.gen.p[6] = 0.5 * ( coords[3]+coords[1] );
               break;
            case diamond_rgn:
               if( cFmt!=pixel_fmt ) {
                 coords[4] += (wcs->rot);
               }
               newShape->param.gen.sinT = sin( myPI * (coords[4] / 180.0) );
               newShape->param.gen.cosT = cos( myPI * (coords[4] / 180.0) );
               break;
            case line_rgn:
               X = coords[2] - coords[0];
               Y = coords[3] - coords[1];
               R = sqrt( X*X + Y*Y );
               newShape->param.gen.sinT = ( R ? Y/R : 0.0 );
               newShape->param.gen.cosT = ( R ? X/R : 1.0 );
               newShape->param.gen.a    = R + 0.5;
               break;
            case point_rgn:
               break;
            case poly_rgn:
               /*  Find bounding box  */
               newShape->param.poly.xmin = coords[0];
               newShape->param.poly.xmax = coords[0];
               newShape->param.poly.ymin = coords[1];
               newShape->param.poly.ymax = coords[1];
               for( i=2; i<nParams; ) {
                  if( newShape->param.poly.xmin > coords[i] ) /* Min X */
                      newShape->param.poly.xmin = coords[i];
                  if( newShape->param.poly.xmax < coords[i] ) /* Max X */
                      newShape->param.poly.xmax = coords[i];
                  i++;
                  if( newShape->param.poly.ymin > coords[i] ) /* Min Y */
                      newShape->param.poly.ymin = coords[i];
                  if( newShape->param.poly.ymax < coords[i] ) /* Max Y */
                      newShape->param.poly.ymax = coords[i];
                  i++;
               }
               break;
            }

         }  /* End of while( *currLoc ) */
/*
  if (coords)printf("%.8f %.8f %.8f %.8f %.8f\n",
   coords[0],coords[1],coords[2],coords[3],coords[4]); 
*/
      }  /* End of if...else parse line */
   }   /* End of while( fgets(rgnFile) ) */


error:

   if( *status )
      fits_free_region( aRgn );
   else
      *Rgn = aRgn;

   fclose( rgnFile );
   free( currLine );

   return( *status );
}

/*---------------------------------------------------------------------------*/
int fftrgn( double    X,
            double    Y,
            SAORegion *Rgn )
/*  Test if the given point is within the region described by Rgn.  X and    */
/*  Y are in pixel coordinates.                                              */
/*---------------------------------------------------------------------------*/
{
   double x, y, dx, dy, xprime, yprime, r;
   RgnShape *Shapes;
   int i;
   int result = 0;

   Shapes = Rgn->Shapes;

   /* if an excluded region is given first, then implicitly   */
   /* assume a previous shape that includes the entire image. */
   if (!Shapes->sign)
      result = 1;

   for( i=0; i<Rgn->nShapes; i++, Shapes++ ) {

    /* only need to test if  */
    /*   the point is not already included and this is an include region, */
    /* or the point is included and this is an excluded region */

    if ( (!result && Shapes->sign) || (result && !Shapes->sign) ) { 

      result = 1;

      switch( Shapes->shape ) {

      case box_rgn:
         /*  Shift origin to center of region  */
         xprime = X - Shapes->param.gen.p[0];
         yprime = Y - Shapes->param.gen.p[1];

         /*  Rotate point to region's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         dx = 0.5 * Shapes->param.gen.p[2];
         dy = 0.5 * Shapes->param.gen.p[3];
         if( (x < -dx) || (x > dx) || (y < -dy) || (y > dy) )
            result = 0;
         break;

      case rectangle_rgn:
         /*  Shift origin to center of region  */
         xprime = X - Shapes->param.gen.p[5];
         yprime = Y - Shapes->param.gen.p[6];

         /*  Rotate point to region's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         dx = Shapes->param.gen.a;
         dy = Shapes->param.gen.b;
         if( (x < -dx) || (x > dx) || (y < -dy) || (y > dy) )
            result = 0;
         break;

      case diamond_rgn:
         /*  Shift origin to center of region  */
         xprime = X - Shapes->param.gen.p[0];
         yprime = Y - Shapes->param.gen.p[1];

         /*  Rotate point to region's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         dx = 0.5 * Shapes->param.gen.p[2];
         dy = 0.5 * Shapes->param.gen.p[3];
         r  = fabs(x/dx) + fabs(y/dy);
         if( r > 1 )
            result = 0;
         break;

      case circle_rgn:
         /*  Shift origin to center of region  */
         x = X - Shapes->param.gen.p[0];
         y = Y - Shapes->param.gen.p[1];

         r  = x*x + y*y;
         if ( r > Shapes->param.gen.a )
            result = 0;
         break;

      case annulus_rgn:
         /*  Shift origin to center of region  */
         x = X - Shapes->param.gen.p[0];
         y = Y - Shapes->param.gen.p[1];

         r = x*x + y*y;
         if ( r < Shapes->param.gen.a || r > Shapes->param.gen.b )
            result = 0;
         break;

      case sector_rgn:
         /*  Shift origin to center of region  */
         x = X - Shapes->param.gen.p[0];
         y = Y - Shapes->param.gen.p[1];

         if( x || y ) {
            r = atan2( y, x ) * 180.0 / myPI;
            if( Shapes->param.gen.p[2] <= Shapes->param.gen.p[3] ) {
               if( r < Shapes->param.gen.p[2] || r > Shapes->param.gen.p[3] )
                  result = 0;
            } else {
               if( r < Shapes->param.gen.p[2] && r > Shapes->param.gen.p[3] )
                  result = 0;
            }
         }
         break;

      case ellipse_rgn:
         /*  Shift origin to center of region  */
         xprime = X - Shapes->param.gen.p[0];
         yprime = Y - Shapes->param.gen.p[1];

         /*  Rotate point to region's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         x /= Shapes->param.gen.p[2];
         y /= Shapes->param.gen.p[3];
         r = x*x + y*y;
         if( r>1.0 )
            result = 0;
         break;

      case elliptannulus_rgn:
         /*  Shift origin to center of region  */
         xprime = X - Shapes->param.gen.p[0];
         yprime = Y - Shapes->param.gen.p[1];

         /*  Rotate point to outer ellipse's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         x /= Shapes->param.gen.p[4];
         y /= Shapes->param.gen.p[5];
         r = x*x + y*y;
         if( r>1.0 )
            result = 0;
         else {
            /*  Repeat test for inner ellipse  */
            x =  xprime * Shapes->param.gen.b + yprime * Shapes->param.gen.a;
            y = -xprime * Shapes->param.gen.a + yprime * Shapes->param.gen.b;

            x /= Shapes->param.gen.p[2];
            y /= Shapes->param.gen.p[3];
            r = x*x + y*y;
            if( r<1.0 )
               result = 0;
         }
         break;

      case line_rgn:
         /*  Shift origin to first point of line  */
         xprime = X - Shapes->param.gen.p[0];
         yprime = Y - Shapes->param.gen.p[1];

         /*  Rotate point to line's orientation  */
         x =  xprime * Shapes->param.gen.cosT + yprime * Shapes->param.gen.sinT;
         y = -xprime * Shapes->param.gen.sinT + yprime * Shapes->param.gen.cosT;

         if( (y < -0.5) || (y >= 0.5) || (x < -0.5)
             || (x >= Shapes->param.gen.a) )
            result = 0;
         break;

      case point_rgn:
         /*  Shift origin to center of region  */
         x = X - Shapes->param.gen.p[0];
         y = Y - Shapes->param.gen.p[1];

         if ( (x<-0.5) || (x>=0.5) || (y<-0.5) || (y>=0.5) )
            result = 0;
         break;

      case poly_rgn:
         if( X<Shapes->param.poly.xmin || X>Shapes->param.poly.xmax
             || Y<Shapes->param.poly.ymin || Y>Shapes->param.poly.ymax )
            result = 0;
         else
            result = Pt_in_Poly( X, Y, Shapes->param.poly.nPts,
                                       Shapes->param.poly.Pts );
         break;
      }

      if( !Shapes->sign ) result = !result;

     } 
   }

   return( result );
}

/*---------------------------------------------------------------------------*/
void fffrgn( SAORegion *Rgn )
/*   Free up memory allocated to hold the region data.                       */
/*---------------------------------------------------------------------------*/
{
   int i;

   for( i=0; i<Rgn->nShapes; i++ )
      if( Rgn->Shapes[i].shape == poly_rgn )
         free( Rgn->Shapes[i].param.poly.Pts );
   if( Rgn->Shapes )
      free( Rgn->Shapes );
   free( Rgn );
}

/*---------------------------------------------------------------------------*/
static int Pt_in_Poly( double x,
                       double y,
                       int nPts,
                       double *Pts )
/*  Internal routine for testing whether the coordinate x,y is within the    */
/*  polygon region traced out by the array Pts.                              */
/*---------------------------------------------------------------------------*/
{
   int i, j, flag=0;
   double prevX, prevY;
   double nextX, nextY;
   double dx, dy, Dy;

   nextX = Pts[nPts-2];
   nextY = Pts[nPts-1];

   for( i=0; i<nPts; i+=2 ) {
      prevX = nextX;
      prevY = nextY;

      nextX = Pts[i];
      nextY = Pts[i+1];

      if( (y>prevY && y>=nextY) || (y<prevY && y<=nextY)
          || (x>prevX && x>=nextX) )
         continue;
      
      /* Check to see if x,y lies right on the segment */

      if( x>=prevX || x>nextX ) {
         dy = y - prevY;
         Dy = nextY - prevY;

         if( fabs(Dy)<1e-10 ) {
            if( fabs(dy)<1e-10 )
               return( 1 );
            else
               continue;
         }

         dx = prevX + ( (nextX-prevX)/(Dy) ) * dy - x;
         if( dx < -1e-10 )
            continue;
         if( dx <  1e-10 )
            return( 1 );
      }

      /* There is an intersection! Make sure it isn't a V point.  */

      if( y != prevY ) {
         flag = 1 - flag;
      } else {
         j = i+1;  /* Point to Y component */
         do {
            if( j>1 )
               j -= 2;
            else
               j = nPts-1;
         } while( y == Pts[j] );

         if( (nextY-y)*(y-Pts[j]) > 0 )
            flag = 1-flag;
      }

   }
   return( flag );
}

