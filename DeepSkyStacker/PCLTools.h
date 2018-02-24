#ifndef __PCLTOOLS_H__
#define __PCLTOOLS_H__

#ifdef PCL_PROJECT

BOOL	IsPCLPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	LoadPCLPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);
BOOL	CreatePCLView(CMemoryBitmap * pBitmap);

#endif

#endif