#pragma once
#include "MultiBitmap.h"
#include "BitmapCharacteristics.h"

template <typename TType, typename TTypeOutput = TType>
class CGrayMultiBitmapT : public CMultiBitmap
{
protected:
	virtual std::shared_ptr<CMemoryBitmap> CreateNewMemoryBitmap() const override;
	virtual std::shared_ptr<CMemoryBitmap> CreateOutputMemoryBitmap() const override;
	virtual bool SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) override;

public:
	CGrayMultiBitmapT()
	{}

	virtual ~CGrayMultiBitmapT()
	{}

	virtual int GetNrChannels() const override
	{
		return 1;
	};

	virtual int GetNrBytesPerChannel() const override
	{
		return sizeof(TType);
	};
};

