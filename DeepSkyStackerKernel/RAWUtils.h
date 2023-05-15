#pragma once

class CBitmapInfo;
class CMemoryBitmap;
namespace DSS { class ProgressBase; }

bool IsSuperPixels();
bool IsRawBayer();
bool IsRawAHD();
bool IsRawBilinear();

void PushRAWSettings(bool bSuperPixel, bool bRawBayer);
void PopRAWSettings();

bool IsRAWPicture(const fs::path& szFileName, QString& strModel);
bool IsRAWPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo);
bool LoadRAWPicture(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, DSS::ProgressBase* pProgress);
