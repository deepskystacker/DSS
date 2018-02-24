/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLImage.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLIMAGE_H
#define HTMLIMAGE_H

class CHTMLImage : public CHTMLParagraphObject			//	img
//
//	Image.
//	Has width, height and alignment.
{
public:
	CHTMLImage( int nWidth, int nHeight, int nBorder, LPCTSTR pcszFilename, CStyle::Align alg, CQHTMImageABC *m_pImage, const CStaticString &strALTText );
	~CHTMLImage();

	void AddDisplayElements( class CHTMLSectionCreator *psc );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

	int m_nWidth;
	int m_nHeight;
	int m_nBorder;
	StringClass m_strFilename;
	StringClass m_strALTText;
	CStyle::Align m_alg;
	CQHTMImageABC *m_pImage;
	UINT m_uID;

private:
	CHTMLImage();
	CHTMLImage( const CHTMLImage &);
	CHTMLImage& operator =( const CHTMLImage &);
};


#endif //HTMLIMAGE_H