#ifndef __PCLTOOLS_H__
#define __PCLTOOLS_H__

#ifdef PCL_PROJECT

bool	IsPCLPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
bool	LoadPCLPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, ProgressBase * pProgress);
bool	CreatePCLView(CMemoryBitmap * pBitmap);

#endif

#endif