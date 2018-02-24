/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	ScrollContainer.h
Owner:	russf@gipsysoft.com
Purpose:	General .
----------------------------------------------------------------------*/
#ifndef SCROLLCONTAINER_H
#define SCROLLCONTAINER_H

#ifndef SECTIONABC_H
	#include "SectionABC.h"
#endif	//	SECTIONABC_H

class CScrollContainer : public CParentSection
{
public:
	explicit CScrollContainer();
	virtual ~CScrollContainer();

	virtual void OnLayout( const WinHelper::CRect &rc );
	void OnDraw( GS::CDrawContext &dc );

	//	Reset the sroll container
	void Reset( int nMaxWidth, int nMaxHeight );

	//	Scroll vertically the amount specificed
	void ScrollV( int nAmount );

	//	Scroll horizontally the amount specificed
	void ScrollH( int nAmount );

	//	Get the current scroll position
	int GetScrollPos() const;
	int GetScrollPosH() const;

	//	Set the absolute position
	void SetPos( int nPos );
	void SetPosH( int nPos );

	//	Ignores extrema, just sets the position
	void SetPosAbsolute( int nHPos, int nVPos );

	//	Determine if the list can scroll in any direction
	bool CanScrollUp() const;
	bool CanScrollDown() const;
	bool CanScrollLeft() const;
	bool CanScrollRight() const;


	//
	//	Get some sizes
	inline UINT GetMaxHeight() const { return m_nMaxHeight; }
	inline UINT GetMaxWidth() const { return m_nMaxWidth; }


private:
	void InternalScroll( int cx, int cy );
	void InternalScrollAbsolute( int cx, int cy );
	void DoInternalScroll( int nDeltaX, int nDeltaY );

	int m_nScrollPos, m_nLastScrollPos;
	int m_nMaxHeight;

	int m_nScrollPosH, m_nLastScrollPosH;
	int m_nMaxWidth;

private:
	CScrollContainer( const CScrollContainer & );
	CScrollContainer& operator =( const CScrollContainer & );
};

#endif //SCROLLCONTAINER_H
