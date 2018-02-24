/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ImgLib.h
Owner:	russf@gipsysoft.com
Purpose:	Imaging library.
----------------------------------------------------------------------*/
#ifndef IMGLIB_H
#define IMGLIB_H

#ifndef _WINDOWS_
	#include <Windows.h>
#endif	//	_WINDOWS_

#ifdef __cplusplus

	#ifndef DIB_H
		#include "../guitools/DIB.h"
	#endif	//	DIB_H

	//
	//	Forward declares
	class CDataSourceABC;

	//
	//	Types

	class CImage
	//
	//	Main image class.
	{
	public:
		CImage();
		virtual ~CImage();

		//	Load from data source
		bool Load( CDataSourceABC &ds );

		//	Unload all frame data
		void UnloadFrames();

		BOOL DrawFrame( size_t nFrame, HDC hdc, int left, int top ) const;
		BOOL StretchFrame( size_t nFrame, HDC hdc, int left, int top, int right, int bottom ) const;

		size_t GetFrameCount() const;

		int GetFrameTime( size_t nFrame ) const;
		const SIZE &GetFrameSize( size_t nFrame ) const;

		//
		//	Get the number of times this animatied image should loop
		UINT GetImageLoopCount() const;

		const SIZE &GetSize() const;

		GS::CDIB * GetFrameDIB( UINT nFrame );

		CImage *CreateCopy() const;

		CImage *CreateSubImage( size_t uFrame, UINT x, UINT y, UINT cx, UINT cy ) const;

		//	Create this object from a standard windows bitmap
		bool CreateFromBitmap( HBITMAP hbmp );
		bool CreateFromIcon( HICON hIcon );

		void ForceTransparent( COLORREF crTransparent );

		void Destroy();

	protected:
		CImage( class CFrameData *m_pFrameData );

		class CFrameData *m_pFrameData;

	private:
		CImage( const CImage &);
		CImage &operator = ( const CImage &);
	};
#else	//	__cplusplus
	#error C++ compiler required.
#endif	//	__cplusplus

#endif //	IMGLIB_H