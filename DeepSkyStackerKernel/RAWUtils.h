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

bool IsRAWPicture(const fs::path& path, QString& strModel);
bool IsRAWPicture(const fs::path& path, CBitmapInfo& BitmapInfo);
bool LoadRAWPicture(const fs::path& path, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, DSS::ProgressBase* pProgress);
