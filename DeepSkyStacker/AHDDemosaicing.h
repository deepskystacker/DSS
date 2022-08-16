#ifndef __AHDDEMOSAICING_H__
#define __AHDDEMOSAICING_H__


template <typename T>
bool AHDDemosaicing(const CGrayBitmapT<T>& grayInputBitmap, std::shared_ptr<CMemoryBitmap>& rpColorBitmap, CDSSProgress* pProgress);


#endif __AHDDEMOSAICING_H__
