/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Color.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef COLOR_H
#define COLOR_H

class CColor  
{
public:
	CColor()
		: m_bSet( false )
	{
		
	}

	bool IsSet() const { return m_bSet; }
	
	CColor & operator = ( const COLORREF cr ) { m_cr = cr; m_bSet = true; return *this; }
	operator COLORREF() const { return m_cr; }
private:
	bool m_bSet;
	COLORREF m_cr;

};

#endif //COLOR_H