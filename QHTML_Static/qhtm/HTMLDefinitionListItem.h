/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLDefinitionListItem.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLDEFINITIONLISTITEM_H
#define HTMLDEFINITIONLISTITEM_H

class CHTMLDefinitionListItem : public CHTMLDocument
//
//	List Item
//	Is a document.
{
public:
	CHTMLDefinitionListItem( CDefaults *pDefaults, bool bTerm );

	bool IsEmpty() const;
	
#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG
	bool m_bTerm;

private:
	CHTMLDefinitionListItem();
	CHTMLDefinitionListItem( const CHTMLDefinitionListItem &);
	CHTMLDefinitionListItem& operator =( const CHTMLDefinitionListItem &);
};

#endif //HTMLDEFINITIONLISTITEM_H