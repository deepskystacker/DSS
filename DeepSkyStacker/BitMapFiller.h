#pragma once

#include "BitmapBase.h"
#include "DSSProgress.h"
#include <memory>


class BitmapFillerInterface
{
protected:
	CDSSProgress* pProgress;
	CMemoryBitmap* pBitmap;
public:
	static std::unique_ptr<BitmapFillerInterface> makeBitmapFiller(CMemoryBitmap* pBitmap, CDSSProgress* pProgress);
	BitmapFillerInterface(CMemoryBitmap* pB, CDSSProgress* pP);
	template <class... Args> BitmapFillerInterface(Args&&...) = delete;
	virtual ~BitmapFillerInterface() {}

	virtual void SetWhiteBalance(double fRedScale, double fGreenScale, double fBlueScale) = 0;
	virtual void SetCFAType(CFATYPE CFAType) = 0;
	virtual void setGrey(bool grey) = 0;
	virtual void setWidth(LONG width) = 0;
	virtual void setHeight(LONG height) = 0;
	virtual void setMaxColors(LONG maxcolors) = 0;
	virtual size_t Write(const void* source, size_t size, size_t count) = 0;
};


class BitMapFiller : public BitmapFillerInterface
{
private:
	bool m_bStarted;
	CFATYPE m_CFAType;
	DWORD m_dwPos;
	PixelIterator m_PixelIt;
	DWORD m_dwCurrentX, m_dwCurrentY;
	BYTE* m_pBuffer;
	DWORD m_dwBufferSize;
	DWORD m_dwBufferReadPos;
	DWORD m_dwBufferWritePos;
	LONG m_lWidth, m_lHeight, m_lMaxColors;
	LONG m_lBytePerChannel;
	bool m_bGrey;
	double m_fRedScale, m_fGreenScale, m_fBlueScale;

private:
	void AdjustColor(double& fColor, const double fAdjust) noexcept
	{
		const double fResult = fColor * fAdjust;
		fColor = std::min(static_cast<double>(MAXWORD - 1), fResult);
	};

	void AddToBuffer(const void* source, const DWORD lSize)
	{
		if (!m_bStarted)
			Start();
		ZASSERTSTATE(0 != m_lWidth);
		ZASSERTSTATE(0 != m_lHeight);
		ZASSERTSTATE(0 != m_lMaxColors);

		m_lBytePerChannel = (m_lMaxColors > 255) ? 2 : 1;

		//
		// Now process the bitmap
		//
		if (!m_pBuffer)
		{
			// Alloc the buffer
			m_dwBufferSize = 2 * lSize;
			ZTRACE_RUNTIME("Allocating image buffer of %d bytes", m_dwBufferSize);
			m_pBuffer = (BYTE*)malloc(m_dwBufferSize);
			if (nullptr == m_pBuffer)
			{
				ZOutOfMemory e("Could not allocate storage for image buffer");
				ZTHROW(e);
			}

			m_dwBufferReadPos = 0;
			m_dwBufferWritePos = 0;
		};
		if (lSize > m_dwBufferSize - m_dwBufferWritePos)
		{
			const DWORD newSize = lSize + m_dwBufferWritePos * 2;
			// realloc the buffer
			ZTRACE_RUNTIME("Re-allocating image buffer to %d bytes", newSize);
			BYTE* temp = (BYTE*)realloc(m_pBuffer, newSize);
			if (nullptr == temp)
			{
				ZOutOfMemory e("Could not re-allocate storage for image buffer");
				ZTHROW(e);
			}

			m_pBuffer = temp;
			m_dwBufferSize = newSize;
		};

		BYTE* pWrite = m_pBuffer;
		const BYTE* pRead = m_pBuffer;
		LONG lToRead;
		bool bEnd = false;

		pWrite += m_dwBufferWritePos;
		pRead += m_dwBufferReadPos;
		memcpy(m_pBuffer, source, lSize);
		m_dwBufferWritePos += lSize;

		while (!bEnd)
		{
			bEnd = true;
			lToRead = m_dwBufferWritePos - m_dwBufferReadPos;
			// Read pixels
			if (m_bGrey)
			{
				if (lToRead >= m_lBytePerChannel)
				{
					// Read gray
					double fGrey;
					if (m_lBytePerChannel == 2)
					{
						fGrey = ((*(pRead + 0)) << 8) + (*(pRead + 1));
						m_dwBufferReadPos += 2;
						pRead += 2;
					}
					else
					{
						fGrey = ((*(pRead + 0)) << 8);
						m_dwBufferReadPos++;
						pRead++;
					};

					if (m_dwCurrentX >= m_lWidth)
					{
						m_dwCurrentY++;
						m_dwCurrentX = 0;
						m_PixelIt->Reset(0, m_dwCurrentY);
						m_dwPos = m_dwCurrentY + 1;
					};

					switch (GetBayerColor(m_dwCurrentX, m_dwCurrentY, m_CFAType))
					{
					case BAYER_RED:
						AdjustColor(fGrey, m_fRedScale);
						break;
					case BAYER_GREEN:
						AdjustColor(fGrey, m_fGreenScale);
						break;
					case BAYER_BLUE:
						AdjustColor(fGrey, m_fBlueScale);
						break;
					};

					m_PixelIt->SetPixel(fGrey / 256.0);
					(*m_PixelIt)++;
					m_dwCurrentX++;

					bEnd = false;
				};
			}
			else
			{
				if (lToRead >= m_lBytePerChannel * 3)
				{
					// Read RGB
					double				fRed, fGreen, fBlue;

					if (m_lBytePerChannel == 2)
					{
						fRed = ((*(pRead + 0)) << 8) + (*(pRead + 1));
						fGreen = ((*(pRead + 2)) << 8) + (*(pRead + 3));
						fBlue = ((*(pRead + 4)) << 8) + (*(pRead + 5));
						m_dwBufferReadPos += 6;
						pRead += 6;
					}
					else
					{
						fRed = ((*(pRead + 0)) << 8);
						fGreen = ((*(pRead + 1)) << 8);
						fBlue = ((*(pRead + 2)) << 8);
						m_dwBufferReadPos += 3;
						pRead += 3;
					};

					if (m_dwCurrentX >= m_lWidth)
					{
						m_dwCurrentY++;
						m_dwCurrentX = 0;
						m_PixelIt->Reset(0, m_dwCurrentY);
						m_dwPos = m_dwCurrentY + 1;
					};

					AdjustColor(fRed, m_fRedScale);
					AdjustColor(fGreen, m_fGreenScale);
					AdjustColor(fBlue, m_fBlueScale);

					m_PixelIt->SetPixel(fRed / 256.0, fGreen / 256.0, fBlue / 256.0);
					m_dwCurrentX++;
					(*m_PixelIt)++;

					bEnd = false;
				};
			};
		};

		// Adjust Buffer
		LONG			lToMove = m_dwBufferWritePos - m_dwBufferReadPos;

		if (lToMove)
		{
			memmove(m_pBuffer, pRead, lToMove);
			m_dwBufferWritePos -= m_dwBufferReadPos;
			m_dwBufferReadPos = 0;
		}
		else
			m_dwBufferReadPos = m_dwBufferWritePos = 0;
	};

	int	Start()
	{
		ZFUNCTRACE_RUNTIME();
		//
		// Do initialisation not done by ctor
		//
		m_bStarted = true;
		m_pBuffer = nullptr;
		m_dwBufferSize = 0;
		m_dwBufferReadPos = 0;
		m_dwBufferWritePos = 0;
		m_dwPos = 0;
		m_dwCurrentX = 0;
		m_dwCurrentY = 0;

		//
		// If the bitmap is a 16-bit grayscale bitmap, then set the CFA type
		//
		if (auto* pGray16Bitmap = dynamic_cast<C16BitGrayBitmap*>(pBitmap))
			pGray16Bitmap->SetCFAType(m_CFAType);
		//
		// Initialise the progress dialog
		//
		if (pProgress)
			pProgress->Start2(nullptr, pBitmap->Height());

		pBitmap->GetIterator(&m_PixelIt);

		return 0;
	};

public:
	BitMapFiller(CMemoryBitmap* pBitmap, CDSSProgress* pProgress) : BitmapFillerInterface{pBitmap, pProgress}
	{
		m_bStarted = false;
		m_fRedScale = 1.0;
		m_fGreenScale = 1.0;
		m_fBlueScale = 1.0;
		m_lBytePerChannel = 2;				// Never going to handle 8 bit data !!
		m_lHeight = 0;
		m_lWidth = 0;
		m_CFAType = CFATYPE_NONE;
		m_dwPos = 0;
		m_dwCurrentX = 0;
		m_dwCurrentY = 0;
		m_pBuffer = nullptr;
		m_dwBufferSize = 0;
		m_dwBufferReadPos = 0;
		m_dwBufferWritePos = 0;
		m_lMaxColors = 0;
		m_bGrey = false;
	}

	virtual ~BitMapFiller()
	{
		if (m_pBuffer)
			free(m_pBuffer);
	};

	void	SetWhiteBalance(double fRedScale, double fGreenScale, double fBlueScale) noexcept
	{
		m_fRedScale = fRedScale;
		m_fGreenScale = fGreenScale;
		m_fBlueScale = fBlueScale;
	};

	void	SetCFAType(CFATYPE CFAType) noexcept
	{
		m_CFAType = CFAType;
	};

	void	setGrey(bool grey) noexcept
	{
		m_bGrey = grey;
	};

	void	setWidth(LONG width) noexcept
	{
		m_lWidth = width;
	};

	void	setHeight(LONG height) noexcept
	{
		m_lHeight = height;
	};

	void	setMaxColors(LONG maxcolors) noexcept
	{
		m_lMaxColors = maxcolors;
	};

#if (0)
	int	Printf(const char* format, va_list va)
	{
		int				nResult;
		CString			strText;

		strText.FormatV(format, va);
		nResult = strText.GetLength();

		AddToBuffer(strText.GetBuffer(10000), nResult);

		if (pProgress)
			pProgress->Progress2(nullptr, m_dwPos);

		return nResult;
	};
#endif

	size_t Write(const void* source, size_t size, size_t count)
	{
		AddToBuffer(source, static_cast<DWORD>(size * count));
		if (pProgress)
			pProgress->Progress2(nullptr, m_dwPos);

		return count;
	};
};
