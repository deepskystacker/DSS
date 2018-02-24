#include <windows.h>  
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <dos.h>
#include <conio.h>
#include <math.h>

// __stdcall to be used by Delphi and VB

__declspec(dllexport) int __stdcall load_cfa(char *name_in,int *imagepointer,int *sizex,int *sizey); 

