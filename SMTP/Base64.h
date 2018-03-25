/*
Module : Base64.h
Purpose: Defines the interface for a simple base64 decoding class
Created: PJN / 22-04-1999

Copyright (c) 1999 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)
 
All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#ifndef __BASE64_H__
#define __BASE64_H__

#ifndef SOCKMFC_EXT_CLASS
#define SOCKMFC_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

class SOCKMFC_EXT_CLASS CBase64
{
public:
//Defines
  #define BASE64_FLAG_NONE	  0
  #define BASE64_FLAG_NOPAD	  1
  #define BASE64_FLAG_NOCRLF  2

//Methods
  int  DecodeGetRequiredLength(int nSrcLen);
  int  EncodeGetRequiredLength(int nSrcLen, DWORD dwFlags = BASE64_FLAG_NONE);
  BOOL Encode(const BYTE* pbSrcData, int nSrcLen,	LPSTR szDest,	int* pnDestLen,	DWORD dwFlags = BASE64_FLAG_NONE);
  BOOL Decode(LPCSTR szSrc, int nSrcLen, BYTE* pbDest, int* pnDestLen);

protected:
  int DecodeChar(unsigned int ch);
};

#endif //__BASE64_H__
