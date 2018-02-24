#ifndef _RAWUTILS_H__
#define _RAWUTILS_H__

#if DSSFILEDECODING==1
BOOL	IsSuperPixels();
BOOL	IsRawBayer();
BOOL	IsRawAHD();
BOOL	IsRawBilinear();

void	PushRAWSettings(BOOL bSuperPixel, BOOL bRawBayer);
void	PopRAWSettings();

BOOL	IsRAWPicture(LPCTSTR szFileName, CString & strModel);
BOOL	IsRAWPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	LoadRAWPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

#else
inline BOOL	IsSuperPixels()		{ return FALSE; };
inline BOOL	IsRawBayer()		{ return FALSE; };
inline BOOL	IsRawAHD()			{ return FALSE; };
inline BOOL	IsRawBilinear()		{ return FALSE; };

inline void	PushRAWSettings(BOOL bSuperPixel, BOOL bRawBayer) {};
inline void	PopRAWSettings() {};

#endif // !DSSFILEDECODING


#endif // _RAWUTILS_H__