/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DibLoader.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef DIBLOADER_H
#define DIBLOADER_H

class CDibLoader
//
//	A single image
{
public:
	typedef Container::CArray< BYTE * > CLineArray;
	typedef Container::CArray< const BYTE * > CConstLineArray;

	CDibLoader( int nWidth, int nHeight, int nBPP );
	CDibLoader( LPBITMAPINFOHEADER lpbhi );

	virtual ~CDibLoader();

	//	Get the image sizes
	inline int GetWidth() const { return m_nWidth; }
	inline int GetHeight() const { return m_nHeight; }


	//	Draw the image onto a device
	bool Draw( HDC hdc, int x, int y ) const;


	//	Get the image data
	inline BYTE *GetBits() { return m_pBits; }


	//	Get the line array for this image.
	void GetLineArray( CLineArray &arr );


	//	Access the color table (palette) for this image
	inline RGBQUAD *GetColorTable() { return m_pClrTab; }


	//	Access the mask table for this image
	inline DWORD *GetMaskTable() { return m_pClrMasks; }


	//	Access the amount of space allocated for the image data
	inline DWORD GetAllocatedImageSize() const { return m_dwImageSize; } 


	inline int GetColorsUsed() const { return m_pBMI->bmiHeader.biClrUsed; }

private:
	inline const BYTE *GetBits() const { return m_pBits; }
	void GetLineArray( CConstLineArray &arr ) const;
	inline DWORD GetDataSize() const { return m_dwImageSize; }

	int m_nWidth, m_nHeight, m_nBPP;

	BITMAPINFO *m_pBMI;
	RGBQUAD *m_pClrTab;
	DWORD	*m_pClrMasks;
	BYTE *m_pBits;	
	DWORD m_dwImageSize;

private:
	CDibLoader();
	CDibLoader( const CDibLoader & );
	CDibLoader &operator = ( const CDibLoader & );
};

#endif //DIBLOADER_H