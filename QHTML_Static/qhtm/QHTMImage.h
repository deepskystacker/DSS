/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTMImage.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef QHTMIMAGE_H
#define QHTMIMAGE_H

#ifndef QHTM_H
	#include "QHTM.h"
#endif	//	QHTM_H

#ifndef IMGLIB_H
	#include <ImgLib/ImgLib.h>
#endif	//	IMGLIB_H

#define CQHTMImage	CImage
/*
class CQHTMImage : public CImage, public CQHTMImageABC
{
public:
	CQHTMImage();
	virtual ~CQHTMImage();

	inline BOOL DrawFrame( size_t nFrame, HDC hdc, int left, int top ) const
	{
		return CImage::DrawFrame( nFrame, hdc, left, top );
	}

	inline BOOL StretchFrame( size_t nFrame, HDC hdc, int left, int top, int right, int bottom ) const
	{
		return CImage::StretchFrame( nFrame, hdc, left, top, right, bottom );
	}

	inline size_t GetFrameCount() const
	{
		return CImage::GetFrameCount();
	}

	inline const SIZE &GetSize() const
	{
		return CImage::GetSize();
	}

	inline UINT GetFrameTime( size_t nFrame ) const
	{
		return CImage::GetFrameTime( nFrame );
	}

	inline size_t GetAnimationLoopCount() const
	{
		return GetImageLoopCount();
	}

	inline void Destroy()
	{
		CImage::Destroy();
	}

private:
	CQHTMImage( const CQHTMImage &rhs );
	CQHTMImage & operator = ( const CQHTMImage &rhs );
};
*/

#endif //QHTMIMAGE_H