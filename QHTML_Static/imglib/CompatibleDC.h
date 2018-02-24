/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	CompatibleDC.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef COMPATIBLEDC_H
#define COMPATIBLEDC_H


class CCompatibleDC  
{
public:
	CCompatibleDC( HDC hdc )
		: m_hdc( CreateCompatibleDC( hdc ) )
	{
		VAPI( m_hdc );
	}

	~CCompatibleDC()
	{
		VAPI( DeleteDC( m_hdc ) );
	}

	operator HDC() { return m_hdc; }

private:
	HDC m_hdc;
};

#endif //COMPATIBLEDC_H