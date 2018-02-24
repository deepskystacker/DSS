/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FormDisplayElementABC.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef FORMDISPLAYELEMENTABC_H
#define FORMDISPLAYELEMENTABC_H

#ifndef HTMLFORMOBJECTABC_H
	#include "HTMLFormObjectABC.h"
#endif	//	HTMLFORMOBJECTABC_H

class CFormDisplayElementABC
{
public:
	CFormDisplayElementABC()
		: m_hwnd( NULL )
		, m_pOwnerObject( NULL )
	{
	}
	virtual void ResetContent() = 0;
	virtual void UpdateFormFromControls() = 0;
	virtual void RedrawContent()
	{
		(void)InvalidateRect( m_hwnd, NULL, FALSE );
	}
	virtual void CheckIfNamed( LPCTSTR /*pcszName*/, bool /*bCheck*/ )
	{
	}
	
protected:
	class CHTMLFormObjectABC *m_pOwnerObject;
	HWND m_hwnd;
};

#endif //FORMDISPLAYELEMENTABC_H