#pragma once

#include "BitMapFiller.h"

class AvxBitmapFiller : public BitmapFillerInterface
{
private:
	float redScale;
	float greenScale;
	float blueScale;
	CFATYPE cfaType;
	bool isGray;
	int width;
	int height;
	size_t nrLinesWritten;
	int bytesPerChannel;
	std::vector<float> redBuffer;
	std::vector<float> greenBuffer;
	std::vector<float> blueBuffer;
	std::vector<float> cfaFactors;
	std::vector<std::uint8_t> sourceBuffer;
public:
	AvxBitmapFiller(CMemoryBitmap* pB, CDSSProgress* pP);
	virtual ~AvxBitmapFiller() {}

	virtual void SetWhiteBalance(double fRedScale, double fGreenScale, double fBlueScale);
	virtual void SetCFAType(CFATYPE cfaType);
	virtual void setGrey(bool grey);
	virtual void setWidth(LONG width);
	virtual void setHeight(LONG height);
	virtual void setMaxColors(LONG maxcolors);
	virtual size_t Write(const void* source, size_t bytesPerPixel, size_t nrPixels);
private:
	void setCfaFactors();
	bool isRgbBayerPattern() const;
};
