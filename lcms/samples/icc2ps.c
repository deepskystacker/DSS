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
// THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//
// IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
// OF THIS SOFTWARE.
//
// Version 1.12

#include "lcms.h"
#include <stdarg.h>

// xgetopt() interface -----------------------------------------------------

extern int   xoptind;
extern char *xoptarg;
extern int   xopterr;
extern char  SW;
int    cdecl xgetopt(int argc, char *argv[], char *optionS);

// ------------------------------------------------------------------------

static char *cInProf = NULL;
static char *cOutProf = NULL;
static int Intent = INTENT_PERCEPTUAL;
static FILE* OutFile;
static int BlackPointCompensation = FALSE;
static int Undecorated = FALSE;

static
void FatalError(const char *frm, ...)
{
       va_list args;

       va_start(args, frm);
       vfprintf(stderr, frm, args);
       va_end(args);

       exit(1);
}



// The toggles stuff

static
void HandleSwitches(int argc, char *argv[])
{
       int s;
      
       while ((s = xgetopt(argc,argv,"uUbBI:i:O:o:T:t:")) != EOF) {

       switch (s){

	 
       case 'i':
       case 'I':
            cInProf = xoptarg;
            break;

       case 'o':
       case 'O':
           cOutProf = xoptarg;
            break;

       case 'b':
       case 'B': BlackPointCompensation =TRUE;
            break;


       case 't':
       case 'T':
            Intent = atoi(xoptarg);
            if (Intent > 3) Intent = 3;
            if (Intent < 0) Intent = 0;
            break;
     
       case 'U':
       case 'u':
            Undecorated = TRUE;
            break;

  default:

       FatalError("Unknown option - run without args to see valid ones.\n");
    }       
    }
}

static
void Help(void)
{
    
         
     fprintf(stderr, "usage: icc2ps [flags]\n\n");

     fprintf(stderr, "flags:\n\n");
     
     fprintf(stderr, "%ci<profile> - Input profile: Generates Color Space Array (CSA)\n", SW);
     fprintf(stderr, "%co<profile> - Output profile: Generates Color Rendering Dictionary(CRD)\n", SW);   
     
     fprintf(stderr, "%ct<0,1,2,3> - Intent (0=Perceptual, 1=Colorimetric, 2=Saturation, 3=Absolute)\n", SW);    
          
     fprintf(stderr, "%cb - Black point compensation (CRD only)\n", SW);    
     fprintf(stderr, "%cu - Do NOT generate resource name on CRD\n", SW);    

	 fprintf(stderr, "\n");
     fprintf(stderr, "This program is intended to be a demo of the little cms\n"
                     "engine. Both lcms and this program are freeware. You can\n"
                     "obtain both in source code at http://www.littlecms.com\n"
                     "For suggestions, comments, bug reports etc. send mail to\n"
                     "info@littlecms.com\n\n");
     exit(0);
}


static
void GenerateCSA(void)
{
	cmsHPROFILE hProfile = cmsOpenProfileFromFile(cInProf, "r");
	size_t n;
	char* Buffer;

	n = cmsGetPostScriptCSA(hProfile, Intent, NULL, 0);
	if (n == 0) return;

	Buffer = (char*) malloc(n + 1);
	cmsGetPostScriptCSA(hProfile, Intent, Buffer, n);
	Buffer[n] = 0;

	fprintf(OutFile, "%s", Buffer);	
	
	free(Buffer);
	cmsCloseProfile(hProfile);
}


static
void GenerateCRD(void)
{
	cmsHPROFILE hProfile = cmsOpenProfileFromFile(cOutProf, "r");
	size_t n;
	char* Buffer;
    DWORD dwFlags = 0;
    
    if (BlackPointCompensation) dwFlags |= cmsFLAGS_BLACKPOINTCOMPENSATION;
    if (Undecorated)            dwFlags |= cmsFLAGS_NODEFAULTRESOURCEDEF;

	n = cmsGetPostScriptCRDEx(hProfile, Intent, dwFlags, NULL, 0);
	if (n == 0) return;

	Buffer = (char*) malloc(n + 1);
    cmsGetPostScriptCRDEx(hProfile, Intent, dwFlags, Buffer, n);
	Buffer[n] = 0;

	fprintf(OutFile, "%s", Buffer);			
	free(Buffer);
	cmsCloseProfile(hProfile);
}


static
int MyErrorHandler(int ErrorCode, const char *ErrorText)
{
    FatalError("icc2ps: %s", ErrorText);
    return 0;
}



int main(int argc, char *argv[])
{
	int nargs;

     fprintf(stderr, "little cms PostScript converter - v1.4\n\n");

	 HandleSwitches(argc, argv);

     cmsSetErrorHandler(MyErrorHandler);

     nargs = (argc - xoptind);
	 if (nargs != 0 && nargs != 1)
				Help();            
	
	 if (nargs == 0) 
			OutFile = stdout;
	 else
			OutFile = fopen(argv[xoptind], "wt");
	   		

	 if (cInProf == NULL && cOutProf == NULL)
				Help();

    
	  if (cInProf != NULL)
			GenerateCSA();
		  
	  if (cOutProf != NULL)
			GenerateCRD();
		
	  if (nargs == 1) {
		  fclose(OutFile);
	  }

      return 0;     
}


