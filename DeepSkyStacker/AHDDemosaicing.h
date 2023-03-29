#ifndef __AHDDEMOSAICING_H__
#define __AHDDEMOSAICING_H__
#include "GrayBitmap.h"

namespace DSS { class ProgressBase; }
using namespace DSS;

template <typename T>
bool AHDDemosaicing(CGrayBitmapT<T> * pGrayInputBitmap, std::shared_ptr<CMemoryBitmap>& rpColorBitmap, ProgressBase* pProgress);


#endif __AHDDEMOSAICING_H__
