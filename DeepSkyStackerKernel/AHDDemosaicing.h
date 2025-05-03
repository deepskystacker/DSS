#pragma once
#include "GrayBitmap.h"

namespace DSS { class OldProgressBase; }
using namespace DSS;

template <typename T>
bool AHDDemosaicing(CGrayBitmapT<T> * pGrayInputBitmap, std::shared_ptr<CMemoryBitmap>& rpColorBitmap, OldProgressBase* pProgress);
