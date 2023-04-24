#pragma once
#include "MultiBitmap.h"

template <typename TType, typename TTypeOutput = TType>
class CColorMultiBitmapT : public CMultiBitmap
{
protected:
	virtual std::shared_ptr<CMemoryBitmap> CreateNewMemoryBitmap() const override;
	virtual std::shared_ptr<CMemoryBitmap> CreateOutputMemoryBitmap() const override;
	virtual bool SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) override;

public:
	CColorMultiBitmapT()
	{
	};

	virtual ~CColorMultiBitmapT()
	{
	};

	virtual int GetNrChannels() const override
	{
		return 3;
	};

	virtual int GetNrBytesPerChannel() const override
	{
		return sizeof(TType);
	};
};
