#pragma once
#include "GrayBitmap.h"

template <typename T>
bool AHDDemosaicing(CGrayBitmapT<T> * pGrayInputBitmap, std::shared_ptr<CMemoryBitmap>& rpColorBitmap, DSS::OldProgressBase* pProgress);

extern template bool AHDDemosaicing<std::uint16_t>(CGrayBitmapT<std::uint16_t>*, std::shared_ptr<CMemoryBitmap>&, DSS::OldProgressBase*);
