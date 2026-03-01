#pragma once
#include "cfa.h"

namespace DSS { class OldProgressBase; }
class CMemoryBitmap;

class BitmapFillerInterface
{
public:
	static std::unique_ptr<BitmapFillerInterface> makeBitmapFiller(CMemoryBitmap* pBitmap, DSS::OldProgressBase* pProgress);
	BitmapFillerInterface() = default;
	virtual ~BitmapFillerInterface() {}

	BitmapFillerInterface(const BitmapFillerInterface&) = default; // For cloning.

	virtual bool isThreadSafe() const;
	virtual std::unique_ptr<BitmapFillerInterface> clone() = 0;

	virtual void SetCFAType(CFATYPE CFAType) = 0;
	virtual void setGrey(bool grey) = 0;
	virtual void setWidth(int width) = 0;
	virtual void setHeight(int height) = 0;
	virtual void setMaxColors(int maxcolors) = 0;
	virtual size_t Write(const void* source, const size_t size, const size_t count, const size_t rowIndex) = 0;
};

class BitmapFillerBase : public BitmapFillerInterface
{
protected:
	DSS::OldProgressBase* pProgress;
	CMemoryBitmap* pBitmap;
	CFATYPE cfaType;
	bool isGray;
	int width;
	int height;
	int bytesPerChannel;
	std::vector<float> redBuffer;
	std::vector<float> greenBuffer;
	std::vector<float> blueBuffer;
	std::vector<float> cfaFactors;
public:
	BitmapFillerBase(CMemoryBitmap* pB, DSS::OldProgressBase* pP);
	virtual ~BitmapFillerBase() override {}

	BitmapFillerBase(const BitmapFillerBase&) = default; // For cloning.

	virtual void SetCFAType(CFATYPE cfaType) override;
	virtual void setGrey(bool grey) override;
	virtual void setWidth(int width) override;
	virtual void setHeight(int height) override;
	virtual void setMaxColors(int maxcolors) override;
protected:
	bool isRgbBayerPattern() const;
};
