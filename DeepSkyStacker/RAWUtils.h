#ifndef _RAWUTILS_H__
#define _RAWUTILS_H__

#if DSSFILEDECODING==1
bool IsSuperPixels();
bool IsRawBayer();
bool IsRawAHD();
bool IsRawBilinear();

void PushRAWSettings(bool bSuperPixel, bool bRawBayer);
void PopRAWSettings();

bool IsRAWPicture(LPCTSTR szFileName, CString& strModel);
bool IsRAWPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo);
bool LoadRAWPicture(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* pProgress);

#else
inline bool	IsSuperPixels()		{ return false; };
inline bool	IsRawBayer()		{ return false; };
inline bool	IsRawAHD()			{ return false; };
inline bool	IsRawBilinear()		{ return false; };

inline void	PushRAWSettings(bool bSuperPixel, bool bRawBayer) {};
inline void	PopRAWSettings() {};

#endif // !DSSFILEDECODING


#endif // _RAWUTILS_H__