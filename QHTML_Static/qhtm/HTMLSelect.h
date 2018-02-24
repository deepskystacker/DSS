/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLSelect.h
Owner:	russf@gipsysoft.com
Purpose:

	A listbox or combo box (or could be a tree)

----------------------------------------------------------------------*/
#ifndef HTMLSELECT_H
#define HTMLSELECT_H

#ifndef HTMLFORMOBJECTABC_H
	#include "HTMLFormObjectABC.h"
#endif //HTMLFORMOBJECTABC_H

class CHTMLOption;

class CHTMLSelect : public CHTMLFormObjectABC
//
//	List, ordered and unordered.
//	Can have many items
{
public:
	explicit CHTMLSelect( const HTMLFontDef * pFont );

	~CHTMLSelect();

	const HTMLFontDef * m_pFont;

	StringClass m_strID;

	UINT m_uSize;
	bool m_bMultiple;

	//	Add a item to the list
	void AddItem( CHTMLOption *pItem );

	virtual bool GetFormFields( Container::CArray< CHTMLFormField > &arrField );

	CHTMLOption *GetCurrentOption() const { return m_pCurrentOption; }

	void ClearCurrentOption() { m_pCurrentOption = NULL; }

	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	ArrayClass< CHTMLOption * > m_arrItems;

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	CHTMLOption *m_pCurrentOption;
private:
	CHTMLSelect();
	CHTMLSelect( const CHTMLSelect &);
	CHTMLSelect& operator =( const CHTMLSelect &);
};

#endif //HTMLSELECT_H