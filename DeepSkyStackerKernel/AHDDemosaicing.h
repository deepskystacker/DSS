#pragma once
#include "GrayBitmap.h"

template <typename T>
bool AHDDemosaicing(CGrayBitmapT<T> * pGrayInputBitmap, std::shared_ptr<CMemoryBitmap>& rpColorBitmap, DSS::OldProgressBase* pProgress);
