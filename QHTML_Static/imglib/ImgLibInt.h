/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ImgLibInt.h
Owner:	russf@gipsysoft.com
Purpose:	Globally used stuff but internal to the library.
----------------------------------------------------------------------*/
#ifndef IMGLIBINT_H
#define IMGLIBINT_H

#ifndef DIB_H
	#include <guitools/DIB.h>
#endif	//	DIB_H

class CFrame
//
//	A single frame
{
public:
	//	Gives ownership of the dib to the frame. Dib should be dynamically created because d'ctor will
	//	delete it.
	CFrame( GS::CDIB *pDib, int nTimeMilliseconds );

	virtual ~CFrame();

	//	Get at the actual DIB for the frame
	const GS::CDIB *GetDib() const { return m_pDib; }
	GS::CDIB *GetDib() { return m_pDib; }

	//	Get the time this frame should be displayed for. 0 == infinite
	int GetTime() const { return m_nTimeMilliseconds; }

private:
	GS::CDIB *m_pDib;
	int m_nTimeMilliseconds;

private:
	CFrame();
	CFrame( const CFrame &);
	CFrame &operator =( const CFrame &);
};

#endif //IMGLIBINT_H