/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLDefinitionList.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLDEFINITIONLIST_H
#define HTMLDEFINITIONLIST_H

#ifndef HTMLPARAGRAPHOBJECT_H
	#include "HTMLParagraphObject.h"
#endif	//	HTMLPARAGRAPHOBJECT_H


class CHTMLDefinitionListItem;
class CHTMLDefinitionList : public CHTMLParagraphObject				//	List
//	Can have many items
{
public:
	CHTMLDefinitionList();

	~CHTMLDefinitionList();

	//	Add a item to the list
	void AddItem( CHTMLDefinitionListItem *pItem );
	
	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	ArrayClass< CHTMLDefinitionListItem * > m_arrItems;

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:

private:
	CHTMLDefinitionList( const CHTMLDefinitionList &);
	CHTMLDefinitionList& operator =( const CHTMLDefinitionList &);
};


#endif //HTMLDEFINITIONLIST_H