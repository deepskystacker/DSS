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
// Version 1.16


/* File lcms.i */
%module lcms
%{
#include "lcms.h"
%}

#define register
%varargs(char* args) cmsSignalError;

%ignore USE_FLOAT;
%ignore USE_C;
%ignore USE_ASSEMBLER;
%ignore USE_INLINE;
%ignore LCMS_DLL;
%ignore LCMS_DLL_BUILD;
%ignore USE_BIG_ENDIAN;
%ignore USE_INT64;
%ignore USE_CUSTOM_SWAB;
%ignore M_PI;
%ignore LOGE;
%ignore MAX_PATH;
%ignore TRUE;
%ignore FALSE;
%ignore _cmsSAMPLER;
%ignore SAMPLEDCURVE;
%ignore LPSAMPLEDCURVE;

%rename (cmsSaveProfile) _cmsSaveProfile;


%{
static PyObject *lcmsError;
static volatile int InErrorFlag;

static 
int MyErrorHandler(int Severity, const char* Txt)
{
	
	if (Severity == LCMS_ERRC_WARNING)
		PyErr_Warn(lcmsError, (char*) Txt);
	else
		PyErr_SetString(lcmsError, Txt);
	
	InErrorFlag = 1;
	return 1;
}
%}


%init %{

lcmsError = PyErr_NewException("lcms.error", NULL, NULL);
PyDict_SetItemString(d, "error", lcmsError);
cmsSetErrorHandler(MyErrorHandler);
 
%}

%exception %{
 
	InErrorFlag = 0;	
	$function
	if (InErrorFlag) SWIG_fail; 		  
%}


/*
* Some typemaps
*/

/*
*	Gamma triple. Must be specified as tuple.
*/

%typemap(in) LPGAMMATABLE[3](LPGAMMATABLE gamma[3]) {  
  
  PyObject* tmp[3];

  if (PyTuple_Check($input)) {
    if (!PyArg_ParseTuple($input,"OOO",&tmp[0],&tmp[1],&tmp[2])) {
      PyErr_SetString(PyExc_TypeError,"gamma must have 3 elements");
      return NULL;
    }
	
	if ((SWIG_ConvertPtr(tmp[0], (void **) &gamma[0], SWIGTYPE_p_GAMMATABLE,1)) == -1) return NULL;
	if ((SWIG_ConvertPtr(tmp[1], (void **) &gamma[1], SWIGTYPE_p_GAMMATABLE,1)) == -1) return NULL;
	if ((SWIG_ConvertPtr(tmp[2], (void **) &gamma[2], SWIGTYPE_p_GAMMATABLE,1)) == -1) return NULL;

    $1 = gamma;

  } else {
    PyErr_SetString(PyExc_TypeError,"expected a tuple.");
    return NULL;
  }
}


/*
*	1-Color containers
*/

%inline %{
typedef struct {

	WORD w[MAXCHANNELS];
		
	} COLORW;

typedef COLORW* LPCOLORW;

typedef struct {

	BYTE b[MAXCHANNELS];
	
	} COLORB;

typedef COLORB* LPCOLORB;

typedef struct {

    LCMSHANDLE hIT8;
    
    } IT8;

typedef IT8* LPIT8;
%}


%include "lcms.h"


/*
** Extend our basic types
*/


%extend COLORB {
		COLORB() {		                
				LPCOLORB v;
                v = (LPCOLORB) malloc(sizeof(COLORB));
				ZeroMemory(v, sizeof(COLORB));
                return v;
        }
        ~COLORB() {
                free(self);
        }
		const char* __repr__() {                
                return "Color 8 bps";
        }

		WORD __getitem__(int key) {
                return self -> b[key];
        }
        void __setitem__(int key, int val) {
                self -> b[key] = (BYTE) val;
        }
};


%extend COLORW {
		COLORW() {		                
				LPCOLORW v;
                v = (LPCOLORW) malloc(sizeof(COLORW));
				ZeroMemory(v, sizeof(COLORW));
                return v;
        }
        ~COLORB() {
                free(self);
        }
		const char* __repr__() {                
                return "Color 16 bps";
        }

		WORD __getitem__(int key) {
                return self -> w[key];
        }
        void __setitem__(int key, int val) {
                self -> w[key] = (WORD) val;
        }
};



%extend cmsCIEXYZ {
        cmsCIEXYZ(double x=0, double y=0, double z=0) {
                LPcmsCIEXYZ v;
                v = (LPcmsCIEXYZ) malloc(sizeof(cmsCIEXYZ));
                v->X = x;
                v->Y = y;
                v->Z = z;
                return v;
        }
        ~cmsCIEXYZ() {
                free(self);
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "XYZ [%g, %g, %g]", self->X,self->Y,self->Z);
                return Buffer;
        }
};

%extend cmsCIExyY {
        cmsCIExyY(double x=0, double y=0, double Y=0) {
                LPcmsCIExyY v;
                v = (LPcmsCIExyY) malloc(sizeof(cmsCIExyY));
                v->x = x;
                v->y = y;
                v->Y = Y;
                return v;
        }
        ~cmsCIExyY() {
                free(self);
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "xyY [%g, %g, %g]", self->x,self->y,self->Y);
                return Buffer;
        }
};

%extend cmsCIELab {
        cmsCIELab(double L=0, double a=0, double b=0) {
                LPcmsCIELab v;
                v = (LPcmsCIELab) malloc(sizeof(cmsCIELab));
                v->L = L;
                v->a = a;
                v->b = b;
                return v;
        }
        ~cmsCIELab() {
                free(self);
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "Lab [%g, %g, %g]", self->L,self->a,self->b);
                return Buffer;
        }
        int __cmp__(cmsCIELab* a) {
                return cmsDeltaE(self, a) > 0.0;
        }

};

%extend cmsCIELCh {
        cmsCIELCh(double L=0, double C=0, double h=0) {
                LPcmsCIELCh v;
                v = (LPcmsCIELCh) malloc(sizeof(cmsCIELCh));
                v->L = L;
                v->C = C;
                v->h = h;
                return v;
        }
        ~cmsCIELCh() {
                free(self);
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "LCh [%g, %g, %g]", self->L,self->C,self->h);
                return Buffer;
        }
};


%extend cmsJCh {
        cmsJCh(double J=0, double C=0, double h=0) {
                LPcmsJCh v;
                v = (LPcmsJCh) malloc(sizeof(cmsJCh));
                v->J = J;
                v->C = C;
                v->h = h;
                return v;
        }
        ~cmsJCh() {
                free(self);
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "CIECAM JCh [%g, %g, %g]", self->J,self->C,self->h);
                return Buffer;
        }
};

%extend GAMMATABLE {
        GAMMATABLE(double Gamma, int nEntries=256) {
                return Gamma <= 0 ? cmsAllocGamma(nEntries) : cmsBuildGamma(nEntries, Gamma);
        }
        ~GAMMATABLE() {
                cmsFreeGamma(self);
        }

        const char* __repr__() {

				static char Buffer[256];

                sprintf(Buffer, "Gamma Table of %d entries [estimated gamma %g]", self ->nEntries, cmsEstimateGamma(self));
				return Buffer;
        }
        WORD __getitem__(int key) {
                return self -> GammaTable[key];
        }
        void __setitem__(int key, WORD val) {
                self -> GammaTable[key] = val;
        }

};


%extend cmsCIExyYTRIPLE {
        cmsCIExyYTRIPLE(cmsCIExyY* Red, cmsCIExyY* Green, cmsCIExyY* Blue) {
                LPcmsCIExyYTRIPLE v;
                v = (LPcmsCIExyYTRIPLE) malloc(sizeof(cmsCIExyYTRIPLE));
                CopyMemory(&v->Red,   Red,   sizeof(cmsCIExyY));
				CopyMemory(&v->Green, Green, sizeof(cmsCIExyY));
				CopyMemory(&v->Blue,  Blue,  sizeof(cmsCIExyY));
                return v;
        }
        ~cmsCIExyYTRIPLE() {
                free(self);
        }
        const char* __repr__() {
                
                return "xyY Triple";                
        }
};


%extend cmsCIEXYZTRIPLE {
        cmsCIEXYZTRIPLE(cmsCIEXYZ* Red, cmsCIEXYZ* Green, cmsCIEXYZ* Blue) {
                LPcmsCIEXYZTRIPLE v;
                v = (LPcmsCIEXYZTRIPLE) malloc(sizeof(cmsCIEXYZTRIPLE));
                CopyMemory(&v->Red,   Red,   sizeof(cmsCIExyY));
				CopyMemory(&v->Green, Green, sizeof(cmsCIExyY));
				CopyMemory(&v->Blue,  Blue,  sizeof(cmsCIExyY));
                return v;
        }
        ~cmsCIEXYZTRIPLE() {
                free(self);
        }
        const char* __repr__() {
                
                return "xyY Triple";                
        }
};




%extend cmsViewingConditions {
        cmsViewingConditions(cmsCIEXYZ* WhitePoint, double Yb, double La, int surround, double D_value) {
                LPcmsViewingConditions v;
                v = (LPcmsViewingConditions) malloc(sizeof(cmsViewingConditions));
				CopyMemory(&v -> whitePoint, WhitePoint, sizeof(cmsCIEXYZ));
				v ->Yb = Yb;
				v ->La = La;
				v -> surround = surround;
				v -> D_value = D_value;
                return v;
        }
        ~cmsViewingConditions() {
                free(self);
        }
        const char* __repr__() {
                
                return "CIECAM97s viewing conditions";                
        }
};



%extend VEC3 {
        VEC3(double vx=0, double vy=0, double vz=0) {
                LPVEC3 v;
                v = (LPVEC3) malloc(sizeof(VEC3));
                v->n[VX] =vx;
                v->n[VY] =vy;
                v->n[VZ] =vz;
                return v;

        }
        ~VEC3() {
                free(self);
        }
        double __getitem__(int key) {
                return self -> n[key];
        }
        void __setitem__(int key, double val) {
                self -> n[key] = val;
        }
        const char* __repr__() {
                static char Buffer[256];

                sprintf(Buffer, "VEC3 [%g, %g, %g]", self->n[VX],self->n[VY],self->n[VZ]);
                return Buffer;
        }

};


class icTagSignature {
};

%extend icTagSignature {
	icTagSignature(unsigned int n) {
		icTagSignature* v =  (icTagSignature*) malloc(sizeof(icTagSignature));
		*v = (icTagSignature) n;
		return v;
	}

	~icTagSignature() {
                free(self);
    }
};


%extend IT8 {

        IT8(const char* FileName) {
                LPIT8 it8;
                it8 = (LPIT8) malloc(sizeof(IT8));
                InErrorFlag = 0;	
                it8 -> hIT8 = cmsIT8LoadFromFile(FileName); 				              
                if (InErrorFlag) {
                    free(it8);
                    return NULL;
                }
                return it8;
        }

        IT8() {
                LPIT8 it8;
                it8 = (LPIT8) malloc(sizeof(IT8));
                it8 -> hIT8 = cmsIT8Alloc();                
                return it8;
        }

        ~IT8() {
                cmsIT8Free(self -> hIT8);
                free(self);
        }
        
        const char* __repr__() {
                return "CGATS.13/IT8 parser";
        }


        int saveToFile(const char* Filename) {

        return cmsIT8SaveToFile(self -> hIT8, Filename);
        }

        int tableCount()        { return cmsIT8TableCount(self -> hIT8); }
        int setTable(int n)     { return cmsIT8SetTable(self -> hIT8, n); }

        const char* getSheetType()                         { return cmsIT8GetSheetType(self -> hIT8); }
        int         setSheetType(const char* Type)         { return cmsIT8SetSheetType(self -> hIT8, Type); }

        int         addComment(const char* txt)            { return cmsIT8SetComment(self -> hIT8, txt); }

        int setProperty(const char* Prop, const char* Str) { return cmsIT8SetPropertyStr(self -> hIT8, Prop, Str); }
        int setProperty(const char* Prop, double dbl)      { return cmsIT8SetPropertyDbl(self -> hIT8, Prop, dbl); }
        int setPropertyAsHex(const char* Prop, int Val)    { return cmsIT8SetPropertyHex(self -> hIT8, Prop, Val); }
        int setPropertyUncooked(const char* Prop, 
                                const char* Str)           { return cmsIT8SetPropertyUncooked(self -> hIT8, Prop, Str); }


        const char* getProperty(const char* Prop)          { return cmsIT8GetProperty(self -> hIT8, Prop); }
        double      getPropertyAsDbl(const char* Prop)     { return cmsIT8GetPropertyDbl(self -> hIT8, Prop); }

        const char* getData(int row, int col)              { return cmsIT8GetDataRowCol(self -> hIT8, row, col); }

        const char* getData(const char* Patch, 
                               const char* Sample)         { return cmsIT8GetData(self -> hIT8, Patch, Sample); }

        double      getDataAsDbl(int row, int col)         { return cmsIT8GetDataRowColDbl(self -> hIT8, row, col); }
               
        double      getDataAsDbl(const char* Patch, 
                                    const char* Sample)    { return cmsIT8GetDataDbl(self -> hIT8, Patch, Sample); }


        int setData(int row, int col, const char* Val)     { return cmsIT8SetDataRowCol(self ->hIT8, row, col, Val); }
        int setData(int row, int col, double dbl)          { return cmsIT8SetDataRowColDbl(self ->hIT8, row, col, dbl); }

        int setData(const char* Patch, 
                    const char* Sample,
                    const char* Val)                       { return cmsIT8SetData(self ->hIT8, Patch, Sample, Val); }

        int setData(const char* Patch, 
                    const char* Sample,
                    double dbl)                            { return cmsIT8SetDataDbl(self ->hIT8, Patch, Sample, dbl); }



       int setDataFormat(int nSample, const char* Sample)  { return cmsIT8SetDataFormat(self -> hIT8, nSample, Sample); }

       const char* getPatchName(int nPatch)                { return  cmsIT8GetPatchName(self -> hIT8, nPatch, NULL); }
	   int         getDataFormat(const char* Patch)		   { return  cmsIT8GetDataFormat(self -> hIT8, Patch); }	

       PyObject* enumDataFormat() {

            char** DataFormat;
            PyObject* TheList;
            PyObject* OneDataFormat;
            int i, n;
  
    
            n = cmsIT8EnumDataFormat(self -> hIT8, &DataFormat);

            TheList = PyList_New(n);
            if (!TheList) return NULL;

            for (i = 0; i < n; i++) {
                OneDataFormat = PyString_FromString(DataFormat[i]);
                PyList_SET_ITEM(TheList, i, OneDataFormat);
            }
    
            return TheList; 
        }


        PyObject* enumProperties() {

            char** Props;
            PyObject* TheList;
            PyObject* OneProp;
            int i, n;
  
    
            n = cmsIT8EnumProperties(self -> hIT8, &Props);

            TheList = PyList_New(n);
            if (!TheList) return NULL;

            for (i = 0; i < n; i++) {
                OneProp = PyString_FromString(Props[i]);
                PyList_SET_ITEM(TheList, i, OneProp);
            }
    
            return TheList; 
        }

        
        int setTableByLabel(const char* cSet, const char *cFld = NULL, const char* ExpectedType = NULL) {

            return cmsIT8SetTableByLabel(self -> hIT8, cSet, cFld, ExpectedType);
        }
	
       PyObject* getRow(int n) {
			
			PyObject* TheList;
            PyObject* OneProp;
            int i;
            int nf = (int) cmsIT8GetPropertyDbl(self -> hIT8, "NUMBER_OF_FIELDS");
            if (nf <= 0) return NULL;

			TheList = PyList_New(nf);
			if (!TheList) return NULL;

            for (i = 0; i < nf; i++) {

                OneProp = PyString_FromString(cmsIT8GetDataRowCol(self ->hIT8, n, i));
                PyList_SET_ITEM(TheList, i, OneProp);
            }
    
           return TheList;
	   }

       PyObject* getCol(const char *Patch) {
			
	   	    PyObject* TheList;
            PyObject* OneProp;
            int i;
			
			int n  = cmsIT8GetDataFormat(self -> hIT8, Patch);
			if (n < 0) return NULL;

            int ns = (int) cmsIT8GetPropertyDbl(self -> hIT8, "NUMBER_OF_SETS");
            if (ns <= 0) return NULL;

			TheList = PyList_New(ns);
			if (!TheList) return NULL;

            for (i = 0; i < ns; i++) {

                OneProp = PyString_FromString(cmsIT8GetDataRowCol(self ->hIT8, i, n));
                PyList_SET_ITEM(TheList, i, OneProp);
            }
    
           return TheList;
	   }
	

};





// ----------------------------------------------------------------------- TODO


%inline %{
class StrPointer {
};
%}

%extend StrPointer {
	StrPointer(char* s) {
		return (StrPointer*) s;
	}
};
