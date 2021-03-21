#pragma once

#include "BitMapFiller.h"

class AvxBitmapFiller : public BitmapFillerInterface
{
private:
	const float redScale;
	const float greenScale;
	const float blueScale;
	CFATYPE cfaType;
	bool isGray;
	int width;
	int height;
	int bytesPerChannel;
	std::vector<float> redBuffer;
	std::vector<float> greenBuffer;
	std::vector<float> blueBuffer;
	std::vector<float> cfaFactors;
	std::vector<std::uint8_t> sourceBuffer;
public:
	AvxBitmapFiller(CMemoryBitmap* pB, CDSSProgress* pP, const double redWb, const double greenWb, const double blueWb);
	AvxBitmapFiller(const AvxBitmapFiller&) = default; // For cloning.
	virtual ~AvxBitmapFiller() {}

	virtual bool isThreadSafe() const override;
	virtual std::unique_ptr<BitmapFillerInterface> clone() override;

	virtual void SetCFAType(CFATYPE cfaType) override;
	virtual void setGrey(bool grey) override;
	virtual void setWidth(LONG width) override;
	virtual void setHeight(LONG height) override;
	virtual void setMaxColors(LONG maxcolors) override;
	virtual size_t Write(const void* source, const size_t bytesPerPixel, const size_t nrPixels, const size_t rowIndex) override;
private:
	void setCfaFactors();
	bool isRgbBayerPattern() const;
};
